APP_ABI := armeabi-v7a
#APP_ABI := arm64-v8a
NDK_TOOLCHAIN_VERSION=4.9
APP_PLATFORM := android-14
APP_STL := gnustl_static
APP_CPPFLAGS += -frtti 
APP_CPPFLAGS += -fexceptions
APP_CPPFLAGS += -DANDROID
APP_CPPFLAGS += -DENABLE_LOGGING
APP_CPPFLAGS += -std=c++11