LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    :=osal
LOCAL_C_INCLUDES:=../include \
                  ../../dependency/include/generic \
                  ../../dependency/include/android
LOCAL_CFLAGS    :=-Werror -Wno-switch -DPLATFORM_ANDROID
LOCAL_CPPFLAGS  :=-std=c++14 -frtti -fexceptions
LOCAL_SRC_FILES := \
                ../../src/osal_kernel.cpp \
                ../../src/osal_gfx_renderer.cpp

include $(BUILD_STATIC_LIBRARY)

all: $(LOCAL_BUILT_MODULE) post_build
post_build:
	cp -rf ../lib/android/local/* ../lib/android/
	rm -rf ../lib/android/local/
