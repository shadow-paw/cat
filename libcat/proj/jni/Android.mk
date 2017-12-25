LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    :=cat
LOCAL_C_INCLUDES:=../include \
                  ../../dependency/include/generic \
                  ../../dependency/include/android
LOCAL_CFLAGS    :=-Werror -Wno-switch -DPLATFORM_ANDROID
LOCAL_CPPFLAGS  :=-std=c++14 -frtti -fexceptions
LOCAL_SRC_FILES := \
                ../../src/cat_kernel.cpp \
                ../../src/cat_data_buffer.cpp \
                ../../src/cat_util_jni.cpp \
                ../../src/cat_util_log.cpp \
                ../../src/cat_util_string.cpp \
                ../../src/cat_storage_vfs.cpp \
                ../../src/cat_storage_driver_file.cpp \
                ../../src/cat_storage_driver_asset.cpp \
                ../../src/cat_storage_resmgr.cpp \
                ../../src/cat_time_service.cpp \
                ../../src/cat_net_service.cpp \
                ../../src/cat_net_http.cpp \
                ../../src/cat_gfx_renderer.cpp \
                ../../src/cat_gfx_ibo.cpp \
                ../../src/cat_gfx_vbo.cpp \
                ../../src/cat_gfx_fbo.cpp \
                ../../src/cat_gfx_tex.cpp \
                ../../src/cat_gfx_shader.cpp \
                ../../src/cat_gfx_drawablecanvas.cpp \
                ../../src/cat_gfx_drawable.cpp \
                ../../src/cat_gfx_draw2d.cpp \
                ../../src/cat_gfx_draw2d_shaders.cpp \
                ../../src/cat_ui_service.cpp \
                ../../src/cat_ui_animator.cpp \
                ../../src/cat_ui_widget.cpp \
                ../../src/cat_ui_pane.cpp \
                ../../src/cat_ui_label.cpp \
                ../../src/cat_ui_button.cpp \
                ../../src/cat_ui_slider.cpp \
                ../../src/cat_ui_effectview.cpp \
                ../../src/cat_ui_edit.cpp

include $(BUILD_STATIC_LIBRARY)

all: $(LOCAL_BUILT_MODULE) post_build
post_build: $(LOCAL_BUILT_MODULE)
	cp -rf ../lib/android/local/* ../lib/android/
