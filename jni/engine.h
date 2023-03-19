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
  draw_bucket Bucket;
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
  GlobalRes.x = Engine->Width;
  GlobalRes.y = Engine->Height;
  
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
  //changing the order doesnt affect anything
  GfxCtx.VBufferId = GfxVertexBufferCreate(QuadData, sizeof(vertex), 6);
  GfxCtx.IBufferId = GfxInstanceBufferCreate(NULL, sizeof(quad_attribs), DRAW_BUCKET_MAX_COUNT);
  GfxCtx.ShaderId  = GfxShaderProgramCreate(VertShaderSrc, VertShaderSrcLength,
                                            FragShaderSrc, FragShaderSrcLength);
  GfxCtx.LayoutId  = GfxVertexLayoutCreate(&GfxCtx);
  
  AAsset_close(VAsset);
  AAsset_close(FAsset);
  glUseProgram(GfxCtx.ShaderId);
  Engine->GfxCtx = GfxCtx;
  
  UIStateInit(&GlobalUIState);
  UIStateElementPush(&GlobalUIState, 
                     UIElementInit(R2f(40.0f, GlobalRes.y-980.0f, (40.0f)+200.0f, (GlobalRes.y-980.0f)+200.0f),
                                   V4f(0.0f, 1.0f, 1.0f, 1.0f), ELMPUSH_BTN_ELM_ID, UI_Flag_Selectable));
  UIStateElementPush(&GlobalUIState,
                     UIElementInit(R2f(40.0f+200.0f, GlobalRes.y-980.0f, (40.0f)+200.0f+200.0f, (GlobalRes.y-980.0f)+200.0f),
                                   V4f(1.0f, 0.0f, 0.0f, 1.0f), ELMPOP_BTN_ELM_ID, UI_Flag_Selectable));
  UIStateElementPush(&GlobalUIState,
                     UIElementInit(R2f(40.0f, 40.0f, GlobalRes.x-40.0f, GlobalRes.y-1000.0f),
                                   V4f(1.0f, 0.0f, 0.0f, 1.0f), GlobalUIState.ElementCount+1, UI_Flag_None));
  return 0;
}
static void EngineUpdate(struct engine* Engine)
{
  // TODO(MIGUEL): make it so that element doesnt move on initial selection
  // TODO(MIGUEL): make elm hierarchies
  // TODO(MIGUEL): use attrib stacks
  
  //- ui logic begin 
  // NOTE(MIGUEL): system user input/event/response data should be captured and stored in the ui state struct
  //               using a UIBegin() call
  // NOTE(MIGUEL): this using a stack based approach to elm creation for simplicity rather than a hash based approach
  //               which is the more flexible one.
  u32 IdGenerator = GlobalUIState.ElementCount; //is just a counter
  u32 TouchedCount = 0;
  m2f Projection = M2fIdentity();
  m2f Rotate     = M2fIdentity();
  m2f Scale      = M2fScale(1.5f, 1.5f);
  M2fMultiply(&Rotate, &Scale, &Projection);
  
  for(ui_elm *Current = GlobalUIState.Elements;
      ElementIsBeforeLastPushed(&GlobalUIState, Current); Current++)
  {
    ui_elm    *Element = Current;
    ui_user_sig Signal = UIDoButton(Element);
    if(Signal.IsTouched && Signal.IsSelected)
    {
      if(Signal.JustPressed && Element->Id==ELMPUSH_BTN_ELM_ID)
      {
        u32 Id = IdGenerator++;
        u32 OffsetY = fmod((Id-3)*100.0f, GlobalRes.y);
        u32 OffsetX = floorf((Id-3)*100.0f/GlobalRes.x)*4.0f;
        ui_elm ElementToPush = UIElementInit(R2f(0.0f+OffsetX            , 000.0f+OffsetY,
                                                 GlobalRes.x*0.5f+OffsetX, 100.0f+OffsetY),
                                             V4f(1.0f, 0.0f, 0.0f, 1.0f), Id, UI_Flag_Selectable);
        UIStateElementPush(&GlobalUIState, ElementToPush);
      }
      if(Signal.JustPressed && Element->Id==ELMPOP_BTN_ELM_ID)
      {
        if(ElementStorageCount(&GlobalUIState) > 3)
        {
          UIStateElementPop(&GlobalUIState);
          IdGenerator--;
        }
      }
    }
    TouchedCount += Signal.IsTouched;
  }
  // NOTE(MIGUEL): v this type of code that relies on post processe info doen on all ui elm should be 
  //                 in a UIEnd() function.
  if(TouchedCount==0 && GlobalJustPressed)
  {
    GlobalUIState.SelectedId = UI_NULL_ELEMENT_ID;
  }
  //- ui logic end
  
  DrawBucketBegin(&Engine->Bucket);
  DrawBucketPushUIElements(&Engine->Bucket,
                           GlobalUIState.ZList.Bottom,
                           GlobalUIState.ElementCount);
  DrawBucketEnd(&Engine->Bucket); //does nothing for now. look at stub def comment for my impl idea
  return;
}
static void EngineDrawFrame(struct engine* Engine)
{
  if (Engine->Display == NULL) { return; }
  
  GfxClearScreen(0.1f, 0.1f, 0.12f, 1.0f);
  
  GfxCtxDrawBucketInstanced(&Engine->GfxCtx, &Engine->Bucket);
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
    //touch pos taking into account offset from stuff like nav bar
    GlobalTouchPos.x = AMotionEvent_getRawX(Event,PtrIndex) + AMotionEvent_getXOffset(Event);
    GlobalTouchPos.y = AMotionEvent_getRawY(Event,PtrIndex) + AMotionEvent_getYOffset(Event);
    //GlobalTouchDelta.x = GlobalTouchPos.x-AMotionEvent_getHistoricalX(Event, PtrIndex, 1);
    //GlobalTouchDelta.y = GlobalTouchPos.y-AMotionEvent_getHistoricalY(Event, PtrIndex, 1);
    
    LOG("Input| x: %f, y: %f", GlobalTouchPos.x, GlobalTouchPos.y);
    switch(Action)
    {
      case AMOTION_EVENT_ACTION_POINTER_DOWN:
      {
        //GlobalTouchPos.x = AMotionEvent_getRawX(Event, PtrIndex);
        //GlobalTouchPos.y = AMotionEvent_getRawY(Event, PtrIndex);
        
      } break;
      case AMOTION_EVENT_ACTION_DOWN:
      {
        GlobalJustPressed = GlobalIsPressed?0:1;
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
