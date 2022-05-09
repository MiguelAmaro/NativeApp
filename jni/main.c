#include <jni.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <android/log.h>
#include <android/asset_manager.h>
#include <android_native_app_glue.h>

#define LOG(...) ((void)__android_log_print(ANDROID_LOG_INFO, "NativeExample", __VA_ARGS__))

struct engine
{
  struct android_app* App;
  
  int Active;
  EGLDisplay Display;
  EGLSurface Surface;
  EGLContext Context;
  int32_t Width;
  int32_t Height;
  
  GLuint Buffer;
  GLuint Shader;
};

static int EngineInitDisplay(struct engine* engine)
{
  const EGLint attribs[] =
  {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_RED_SIZE, 8,
    EGL_NONE,
  };
  
  EGLDisplay display;;
  if ((display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY)
  {
    LOG("error with eglGetDisplay");
    return -1;
  }
  
  if (!eglInitialize(display, 0, 0))
  {
    LOG("error with eglInitialize");
    return -1;
  }
  
  EGLConfig config;
  EGLint numConfigs;
  if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs))
  {
    LOG("error with eglChooseConfig");
    return -1;
  }
  
  EGLint format;
  if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format))
  {
    LOG("error with eglGetConfigAttrib");
    return -1;
  }
  
  ANativeWindow_setBuffersGeometry(engine->App->window, 0, 0, format);
  
  EGLSurface surface;
  if (!(surface = eglCreateWindowSurface(display, config, engine->App->window, NULL)))
  {
    LOG("error with eglCreateWindowSurface");
    return -1;
  }
  
  const EGLint ctx_attrib[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
  EGLContext context;
  if (!(context = eglCreateContext(display, config, NULL, ctx_attrib)))
  {
    LOG("error with eglCreateContext");
    return -1;
  }
  
  if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
  {
    LOG("error with eglMakeCurrent");
    return -1;
  }
  
  LOG("GL_VENDOR = %s", glGetString(GL_VENDOR));
  LOG("GL_RENDERER = %s", glGetString(GL_RENDERER));
  LOG("GL_VERSION = %s", glGetString(GL_VERSION));
  
  EGLint w, h;
  eglQuerySurface(display, surface, EGL_WIDTH, &w);
  eglQuerySurface(display, surface, EGL_HEIGHT, &h);
  
  engine->Display = display;
  engine->Context = context;
  engine->Surface = surface;
  engine->Width = w;
  engine->Height = h;
  
  AAsset* vasset = AAssetManager_open(engine->App->activity->assetManager, "vertex.glsl", AASSET_MODE_BUFFER);
  if (!vasset)
  {
    LOG("error opening vertex.glsl");
    return -1;
  }
  const GLchar* vsrc = AAsset_getBuffer(vasset);
  GLint vlen = AAsset_getLength(vasset);
  
  GLuint v = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(v, 1, &vsrc, &vlen);
  glCompileShader(v);
  
  AAsset_close(vasset);
  
  AAsset* fasset = AAssetManager_open(engine->App->activity->assetManager, "fragment.glsl", AASSET_MODE_BUFFER);
  if (!fasset)
  {
    LOG("error opening fragment.glsl");
    return -1;
  }
  const GLchar* fsrc = AAsset_getBuffer(fasset);
  GLint flen = AAsset_getLength(fasset);
  
  GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(f, 1, &fsrc, &flen);
  glCompileShader(f);
  
  AAsset_close(fasset);
  
  GLuint p = glCreateProgram();
  glAttachShader(p, v);
  glAttachShader(p, f);
  
  glBindAttribLocation(p, 0, "vPosition");
  glBindAttribLocation(p, 1, "vColor");
  glLinkProgram(p);
  
  glDeleteShader(v);
  glDeleteShader(f);
  glUseProgram(p);
  
  const float buf[] =
  {
    0.0f,  0.5f, 1.f, 0.f, 0.f,
    -0.5f, -0.5f, 0.f, 1.f, 0.f,
    0.5f, -0.5f, 0.f, 0.f, 1.f,
  };
  
  GLuint b;
  glGenBuffers(1, &b);
  glBindBuffer(GL_ARRAY_BUFFER, b);
  glBufferData(GL_ARRAY_BUFFER, sizeof(buf), buf, GL_STATIC_DRAW);
  
  engine->Buffer = b;
  engine->Shader = p;
  
  return 0;
}

static void EngineDrawFrame(struct engine* engine)
{
  if (engine->Display == NULL)
  {
    return;
  }
  
  glClearColor(0.258824f, 0.258824f, 0.435294f, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glUseProgram(engine->Shader);
  
  glBindBuffer(GL_ARRAY_BUFFER, engine->Buffer);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (2+3)*sizeof(float), NULL);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (2+3)*sizeof(float), (void*)(2*sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  
  glDrawArrays(GL_TRIANGLES, 0, 3);
  
  eglSwapBuffers(engine->Display, engine->Surface);
}

static void EngineTermDisplay(struct engine* engine)
{
  if (engine->Display != EGL_NO_DISPLAY)
  {
    glDeleteProgram(engine->Shader);
    glDeleteBuffers(1, &engine->Buffer);
    
    eglMakeCurrent(engine->Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (engine->Context != EGL_NO_CONTEXT)
    {
      eglDestroyContext(engine->Display, engine->Context);
    }
    if (engine->Surface != EGL_NO_SURFACE)
    {
      eglDestroySurface(engine->Display, engine->Surface);
    }
    eglTerminate(engine->Display);
  }
  engine->Active = 0;
  engine->Display = EGL_NO_DISPLAY;
  engine->Context = EGL_NO_CONTEXT;
  engine->Surface = EGL_NO_SURFACE;
}

static int32_t EngineHandleInput(struct android_app* app, AInputEvent* event)
{
  return 0;
}

static void EngineHandleCmd(struct android_app* app, int32_t cmd)
{
  struct engine* engine = (struct engine*)app->userData;
  switch (cmd)
  {
    case APP_CMD_INIT_WINDOW:
    if (engine->App->window != NULL)
    {
      EngineInitDisplay(engine);
      EngineDrawFrame(engine);
    }
    break;
    
    case APP_CMD_TERM_WINDOW:
    EngineTermDisplay(engine);
    break;
    
    case APP_CMD_GAINED_FOCUS:
    engine->Active = 1;
    break;
    
    case APP_CMD_LOST_FOCUS:
    engine->Active = 0;
    EngineDrawFrame(engine);
    break;
  }
}

void android_main(struct android_app* State)
{
  struct engine Engine;
  memset(&Engine, 0, sizeof(Engine));
  
  State->userData = &Engine;
  State->onAppCmd = EngineHandleCmd;
  State->onInputEvent = EngineHandleInput;
  Engine.App = State;
  
  while (1)
  {
    int Ident;
    int Events;
    struct android_poll_source* Source;
    
    while ((Ident=ALooper_pollAll(Engine.Active ? 0 : -1, NULL, &Events, (void**)&Source)) >= 0)
    {
      if (Source != NULL)
      {
        Source->process(State, Source);
      }
      
      if (State->destroyRequested != 0)
      {
        EngineTermDisplay(&Engine);
        return;
      }
    }
    
    if (Engine.Active)
    {
      EngineDrawFrame(&Engine);
    }
  }
}
