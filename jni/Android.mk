# https://developer.android.com/ndk/guides/android_mk.html

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := cglm
LOCAL_C_INCLUDES := -I ${LOCAL_PATH}/cglm/include
LOCAL_C_SRC_FILES := $(wildcard ${LOCAL_PATH}/cglm/src/*.c)
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := main
LOCAL_SRC_FILES := main.c 
LOCAL_C_INCLUDES := ${LOCAL_PATH}/cglm/include
LOCAL_CFLAGS := -fdiagnostics-absolute-paths -g
#LOCAL_CPPFLAGS
#LOCAL_LDFLAGS
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv2
LOCAL_STATIC_LIBRARIES := android_native_app_glue
#LOCAL_SHARED_LIBRARIES
#LOCAL_ARM_MODE := arm
#LOCAL_ARM_NEON
#LOCAL_EXPORT_CFLAGS
#LOCAL_EXPORT_CPPFLAGS
#LOCAL_EXPORT_C_INCLUDES
#LOCAL_EXPORT_LDFLAGS
#LOCAL_EXPORT_LDLIBS
include $(BUILD_SHARED_LIBRARY)


$(call import-module,android/native_app_glue)
