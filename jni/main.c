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

#define ArrayCount(array) (sizeof(array)/sizeof(array[0]))
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
f64 GlobalDeltaTime = 0;
f64 GlobalTimeElapsed = 0;
//Global Input
#define ELMPUSH_BTN_ELM_ID (0)
#define ELMPOP_BTN_ELM_ID (1)

#include "widget.h"
#include "draw.h"
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

vertex3d QuadData3d[6] =
{
  //tri:a       v(Pos)            v(uv)
  {{-1.f, 0.f,  1.f,}, { 0.f,  1.f,} },
  {{ 1.f, 0.f,  1.f,}, { 1.f,  1.f,} },
  {{ 1.f, 0.f, -1.f,}, { 1.f,  0.f,} },
  //tri:b
  {{-1.f, 0.f, -1.f,}, { 0.f,  0.f,} },
  {{ 1.f, 0.f, -1.f,}, { 1.f,  0.f,} },
  {{-1.f, 0.f,  1.f,}, { 0.f,  1.f,} },
};
#include "engine.h"
u64 GetTimeNanos(void)
{
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return (u64) now.tv_sec*1000000000LL + now.tv_nsec;
}
f64 GetTimeSeconds(void)
{
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return (f64)now.tv_sec + (f64)(now.tv_nsec/(f64)1000000000.0);
}
void android_main(struct android_app* State)
{
  struct engine Engine;
  MemorySet(0, &Engine, sizeof(Engine));
  
  State->userData     = &Engine;
  State->onAppCmd     = EngineHandleCmd;
  State->onInputEvent = EngineHandleInput;
  Engine.App = State;
  
  f64 LastTime = GetTimeSeconds();
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
    f64 CurrentTime = GetTimeSeconds();
    GlobalDeltaTime    = CurrentTime-LastTime;
    GlobalTimeElapsed += GlobalDeltaTime;
    LastTime = CurrentTime;
    /*
    LOG("delta t: %f \n"
        "elapsed: %f \n",
        GlobalDeltaTime,
          GlobalTimeElapsed);
    */
  }
}
