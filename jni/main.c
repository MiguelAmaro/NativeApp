#include <jni.h>

#include <assert.h>
#include <string.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android_native_app_glue.h>

#define LOG(...) ((void)__android_log_print(ANDROID_LOG_INFO, "NativeApp", __VA_ARGS__))
#define Assert(condition, ...) \
do { \
if(!(condition)) \
{ ((void)__android_log_print(ANDROID_LOG_WARN, "NativeApp[ASSERT]!", __VA_ARGS__)); } \
} while(0)

#define SymbolToString(symbol) #symbol
#define ThisFuncionAsString() __FUNCTION__
#include "types.h"
#include "mymath.h"
#include "ui.h"

//Global Input
v2f GlobalRes      = {0};
v2f GlobalTouchPos = {0};
v2f GlobalTouchDelta = {0};
b32 GlobalIsPressed = 0;
b32 GlobalJustPressed = 0;
b32 GlobalJustReleased = 0;

//Global Input
#define ELMPUSH_BTN_ELM_ID (0)
#define ELMPOP_BTN_ELM_ID (1)

#include "gfx.h"
#include "render.h"


vertex QuadData[6] =
{
  //tri:a//Pos            Color
  {{-1.0f,  1.0f},    {1.f, 1.f, 0.f,} },
  {{1.0f ,  1.0f},    {1.f, 0.f, 0.f,} },
  {{1.0f , -1.0f},    {0.f, 1.f, 0.f,} },
  //tri:b
  {{-1.0f, -1.0f},    {0.f, 0.f, 1.f,} },
  {{1.0f , -1.0f},    {0.f, 1.f, 0.f,} },
  {{-1.0f,  1.0f},    {1.f, 1.f, 0.f,} },
};
#include "engine.h"

void android_main(struct android_app* State)
{
  struct engine Engine;
  MemorySet(0, &Engine, sizeof(Engine));
  
  State->userData     = &Engine;
  State->onAppCmd     = EngineHandleCmd;
  State->onInputEvent = EngineHandleInput;
  Engine.App = State;
  
  u32 isRunning = 1;
  while(isRunning)
  {
    int Ident;
    int Events;
    struct android_poll_source* Source;
    GlobalJustPressed = 0; // NOTE(MIGUEL): sketchy impl
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
      GlobalRes.x = Engine.Width;
      GlobalRes.y = Engine.Height;
      EngineUpdate(&Engine);
      EngineDrawFrame(&Engine);
    }
  }
}
