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
                ../../src/osal_storage_buffer.cpp \
                ../../src/osal_storage_vfs.cpp \
                ../../src/osal_gfx_renderer.cpp \
                ../../src/osal_gfx_ibo.cpp \
                ../../src/osal_gfx_vbo.cpp \
                ../../src/osal_gfx_tex.cpp \
                ../../src/osal_gfx_shader.cpp \
                ../../src/osal_gfx_canvas.cpp \
                ../../src/osal_gfx_drawable.cpp \
                ../../src/osal_gfx_draw2d.cpp \
                ../../src/osal_gfx_draw2d_shaders.cpp \
                ../../src/osal_gfx_resmgr.cpp \
                ../../src/osal_time_service.cpp \
                ../../src/osal_util_jni.cpp \
                ../../src/osal_util_log.cpp \
                ../../src/osal_storage_driver_file.cpp \
                ../../src/osal_storage_driver_asset.cpp \
                ../../src/osal_ui_service.cpp \
                ../../src/osal_ui_widget.cpp \
                ../../src/osal_ui_pane.cpp \
                ../../src/osal_ui_label.cpp \
                ../../src/osal_ui_button.cpp \
                ../../src/osal_ui_slider.cpp

include $(BUILD_STATIC_LIBRARY)

all: $(LOCAL_BUILT_MODULE) post_build
post_build: $(LOCAL_BUILT_MODULE)
	cp -rf ../lib/android/local/* ../lib/android/
	rm -rf ../lib/android/local/
