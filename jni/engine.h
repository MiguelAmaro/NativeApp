#ifndef ENGINE_H
#define ENGINE_H


struct engine
{
  struct android_app* App;
  
  int Active;
  EGLDisplay Display;
  EGLSurface Surface;
  EGLContext Context;
  int32_t Width;
  int32_t Height;
  gfx_ctx GfxCtx;
};
static int EngineInitDisplay(struct engine* Engine)
{
  const EGLint Attribs[] =
  {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_RED_SIZE, 8,
    EGL_NONE,
  };
  
  EGLDisplay Display;
  if ((Display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY)
  {
    LOG("error with eglGetDisplay");
    return -1;
  }
  
  if (!eglInitialize(Display, 0, 0))
  {
    LOG("error with eglInitialize");
    return -1;
  }
  
  EGLConfig Config;
  EGLint NumConfigs;
  if (!eglChooseConfig(Display, Attribs, &Config, 1, &NumConfigs))
  {
    LOG("error with eglChooseConfig");
    return -1;
  }
  
  EGLint Format;
  if (!eglGetConfigAttrib(Display, Config, EGL_NATIVE_VISUAL_ID, &Format))
  {
    LOG("error with eglGetConfigAttrib");
    return -1;
  }
  
  ANativeWindow_setBuffersGeometry(Engine->App->window, 0, 0, Format);
  
  EGLSurface Surface;
  if (!(Surface = eglCreateWindowSurface(Display, Config, Engine->App->window, NULL)))
  {
    LOG("error with eglCreateWindowSurface");
    return -1;
  }
  
  const EGLint CtxAttrib[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
  EGLContext Context;
  if (!(Context = eglCreateContext(Display, Config, NULL, CtxAttrib)))
  {
    LOG("error with eglCreateContext");
    return -1;
  }
  
  if (eglMakeCurrent(Display, Surface, Surface, Context) == EGL_FALSE)
  {
    LOG("error with eglMakeCurrent");
    return -1;
  }
  
  LOG("GL_VENDOR = %s", glGetString(GL_VENDOR));
  LOG("GL_RENDERER = %s", glGetString(GL_RENDERER));
  LOG("GL_VERSION = %s", glGetString(GL_VERSION));
  
  EGLint Width, Height;
  eglQuerySurface(Display, Surface, EGL_WIDTH, &Width);
  eglQuerySurface(Display, Surface, EGL_HEIGHT, &Height);
  
  Engine->Display = Display;
  Engine->Context = Context;
  Engine->Surface = Surface;
  Engine->Width = Width;
  Engine->Height = Height;
  
  
  AAsset* VAsset = AAssetManager_open(Engine->App->activity->assetManager, "vertex.glsl", AASSET_MODE_BUFFER);
  AAsset* FAsset = AAssetManager_open(Engine->App->activity->assetManager, "fragment.glsl", AASSET_MODE_BUFFER);
  if (!VAsset)
  {
    LOG("error opening vertex.glsl");
    return -1;
  }
  if (!FAsset)
  {
    LOG("error opening fragment.glsl");
    return -1;
  }
  
  const char* VertShaderSrc = AAsset_getBuffer(VAsset);
  const char* FragShaderSrc = AAsset_getBuffer(FAsset);
  s32         VertShaderSrcLength = AAsset_getLength(VAsset);
  s32         FragShaderSrcLength = AAsset_getLength(FAsset);
  
  
  gfx_ctx GfxCtx = GfxCtxInit();
  GfxCtx.VBufferId = GfxVertexBufferCreate(QuadData, sizeof(QuadData), 1);
  GfxCtx.IBufferId = GfxInstanceBufferCreate(NULL, sizeof(r2f), 64);
  GfxCtx.ShaderId  = GfxShaderProgramCreate(VertShaderSrc, VertShaderSrcLength,
                                            FragShaderSrc, FragShaderSrcLength);
  
  AAsset_close(VAsset);
  AAsset_close(FAsset);
  glUseProgram(GfxCtx.ShaderId);
  Engine->GfxCtx = GfxCtx;
  
  return 0;
}
static void EngineDrawFrame(struct engine* Engine)
{
  if (Engine->Display == NULL)
  {
    return;
  }
  
  glClearColor(0.0f, 0.0f, 0.0f, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
#if 1
  
  m2f Projection = M2fIdentity();
  m2f Rotate     = M2fIdentity();
  m2f Scale      = M2fScale(1.5f, 1.5f);
  M2fMultiply(&Rotate, &Scale, &Projection);
  
  static ui Element = {
    .Rect = { 0.0f, 0.0f, 1080.0f, 1080.0f },
    .IsSelected = 0,
    .Color = {0.3f, 0.2f, 0.8f, 1.0f},
  };
  Element.IsSelected = IsInRect(Element.Rect, GlobalTouchPos)?1:0;
  //LOG("%s | x: %f; y: %f", InRect?"true":"false", GlobalTouchPos.x, GlobalTouchPos.y);
  
  if(Element.IsSelected)
  {
    v2f HalfDim = V2f((Element.Rect.max.x-Element.Rect.min.x)*0.5f,
                      (Element.Rect.max.y-Element.Rect.min.y)*0.5f);
    Element.Rect.min.x = GlobalTouchPos.x-HalfDim.x;
    Element.Rect.min.y = GlobalTouchPos.y-HalfDim.y;
    Element.Rect.max.x = GlobalTouchPos.x+HalfDim.x;
    Element.Rect.max.y = GlobalTouchPos.y+HalfDim.y;
    Element.Color = V4f(1.0f, 1.0f, 1.0f, 0.4f);
    if(GlobalIsPressed)
    {
      Element.Color = V4f(0.3f, 1.0f, 0.8f, 1.0f);
    }
  } else
  {
    Element.Color = V4f(0.3f, 0.2f, 0.8f, 1.0f);
  }
  
  GfxCtxDraw(&Engine->GfxCtx, Element);
#endif
  
  eglSwapBuffers(Engine->Display, Engine->Surface);
  return;
}
static void EngineTermDisplay(struct engine* Engine)
{
  if (Engine->Display != EGL_NO_DISPLAY)
  {
    glDeleteProgram(Engine->GfxCtx.ShaderId);
    glDeleteBuffers(1, &Engine->GfxCtx.VBufferId);
    
    eglMakeCurrent(Engine->Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (Engine->Context != EGL_NO_CONTEXT)
    {
      eglDestroyContext(Engine->Display, Engine->Context);
    }
    if (Engine->Surface != EGL_NO_SURFACE)
    {
      eglDestroySurface(Engine->Display, Engine->Surface);
    }
    eglTerminate(Engine->Display);
  }
  Engine->Active = 0;
  Engine->Display = EGL_NO_DISPLAY;
  Engine->Context = EGL_NO_CONTEXT;
  Engine->Surface = EGL_NO_SURFACE;
  return;
}

static int32_t EngineHandleInput(struct android_app* App, AInputEvent* Event)
{
  // NOTE(MIGUEL): do i even need to take input in this function?????
  if((AInputEvent_getSource(Event) == AINPUT_SOURCE_TOUCHSCREEN) &&
     (AInputEvent_getType  (Event) == AINPUT_EVENT_TYPE_MOTION))
  {
    u32 Action   = AMotionEvent_getAction(Event);
    u32 PtrIndex = ((Action >>  AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT));
    
    
    GlobalTouchPos.x = AMotionEvent_getRawX(Event,
                                            PtrIndex);
    
    GlobalTouchPos.y = AMotionEvent_getRawY(Event,
                                            PtrIndex);
    
    LOG("Input| x: %f, y: %f", GlobalTouchPos.x, GlobalTouchPos.y);
    switch(Action)
    {
      case AMOTION_EVENT_ACTION_POINTER_DOWN:
      {
        GlobalTouchPos.x = AMotionEvent_getRawX(Event, PtrIndex);
        GlobalTouchPos.y = AMotionEvent_getRawY(Event, PtrIndex);
      } break;
      case AMOTION_EVENT_ACTION_DOWN:
      {
        GlobalIsPressed = 1;
      } break;
      case AMOTION_EVENT_ACTION_UP:
      {
        GlobalIsPressed = 0;
      } break;
      default:
      {
        
      } break;
    }
    
  }
  return 0;
}
static void EngineHandleCmd(struct android_app* App, int32_t Cmd)
{
  struct engine* Engine = (struct engine*)App->userData;
  switch (Cmd)
  {
    case APP_CMD_INIT_WINDOW:
    if (Engine->App->window != NULL)
    {
      EngineInitDisplay(Engine);
      EngineDrawFrame(Engine);
    }
    break;
    case APP_CMD_TERM_WINDOW:
    EngineTermDisplay(Engine);
    break;
    case APP_CMD_GAINED_FOCUS:
    Engine->Active = 1;
    break;
    case APP_CMD_LOST_FOCUS:
    Engine->Active = 0;
    EngineDrawFrame(Engine);
    break;
  }
}

#endif //ENGINE_H
