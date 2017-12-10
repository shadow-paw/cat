package com.shadowpaw.cat;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.widget.FrameLayout;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class CatView extends FrameLayout {
    static { System.loadLibrary("cat-app"); }
    private static final int EVENT_TOUCHDOWN = 1;
    private static final int EVENT_TOUCHUP = 2;
    private static final int EVENT_TOUCHMOVE = 3;

    private GLSurfaceView mSurface;
    private Object mJniHandle;
    private boolean mContextReady;

    private native Object  jniInit(AssetManager mgr);
    private native void    jniFini(Object handle);
    private native void    jniStartup(Object handle);
    private native void    jniShutdown(Object handle);
    private native void    jniResize(Object handle, int width, int height);
    private native void    jniTouch(Object handle, int type, int pointerId, int x, int y);
    private native void    jniRender(Object handle);
    private native boolean jniTimer(Object handle);
    private native void    jniContextRestored(Object handle);
    private native void    jniContextLost(Object handle);
    private native void    jniPause(Object handle);
    private native void    jniResume(Object handle);

    public CatView(Context context){
        super(context);
        mContextReady = false;
        mSurface = new GLSurfaceView(context);
        addView(mSurface, new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        mJniHandle = jniInit(getContext().getAssets());
        jniStartup(mJniHandle);
        mSurface.setZOrderOnTop(false);
        mSurface.getHolder().setFormat(PixelFormat.RGBX_8888);
        mSurface.setEGLContextClientVersion(2);
        mSurface.setRenderer(new GLSurfaceView.Renderer() {
            @Override
            public void onSurfaceCreated(GL10 gl, EGLConfig config) {
                removeCallbacks(onTimer);
                post(onTimer);
            }
            @Override
            public void onSurfaceChanged(GL10 gl, int width, int height) {
                gl.glViewport(0, 0, width, height);
                if (mContextReady) jniContextLost(mJniHandle);
                jniContextRestored(mJniHandle);
                mContextReady = true;
                final int w = width, h = height;
                post(new Runnable() {
                    @Override
                    public void run() {
                        jniResize(mJniHandle, w, h);
                    }
                });
            }
            @Override
            public void onDrawFrame(GL10 gl) {
                jniRender(mJniHandle);
            }
        });
        mSurface.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }
    public void onDestroy() {
        removeCallbacks(onTimer);
        jniShutdown(mJniHandle);
        jniFini(mJniHandle);
    }
    public void onResume() {
        mSurface.onResume();
        if (mJniHandle!=null) jniResume(mJniHandle);
        if (mJniHandle!=null) post(onTimer);
    }
    public void onPause() {
        removeCallbacks(onTimer);
        if (mJniHandle!=null) jniPause(mJniHandle);
        mSurface.onPause();
    }
    // ---------------------------------------------------------------
    // Timer Event
    // ---------------------------------------------------------------
    private final Runnable onTimer = new Runnable() {
        @Override
        public void run() {
            if (jniTimer(mJniHandle)) mSurface.requestRender();
            postDelayed(onTimer, 33);
        }
    };
    // ---------------------------------------------------------------
    // Touch Event
    // ---------------------------------------------------------------
    private static class TouchMoveData {
        int pointerId, x, y;
    }
    @Override
    public boolean performClick() {
        return super.performClick();
    }
    @Override
    public boolean onTouchEvent(MotionEvent ev) {
        switch (ev.getAction()) {
            case MotionEvent.ACTION_DOWN: {
                int index = ev.getActionIndex();
                int pointerId = ev.getPointerId(index);
                int x = (int)ev.getX(index);
                int y = (int)ev.getY(index);
                jniTouch (mJniHandle, EVENT_TOUCHDOWN, pointerId, x, y);
                mSurface.requestRender();
                return true;
            }
            case MotionEvent.ACTION_CANCEL:
            case MotionEvent.ACTION_UP: {
                int index = ev.getActionIndex();
                int pointerId = ev.getPointerId(index);
                int x = (int)ev.getX(index);
                int y = (int)ev.getY(index);
                jniTouch (mJniHandle, EVENT_TOUCHUP, pointerId, x, y);
                mSurface.requestRender();
                performClick();
                return true;
            }
            case MotionEvent.ACTION_MOVE: {
                for (int p = 0; p < ev.getPointerCount(); p++) {
                    TouchMoveData d = new TouchMoveData();
                    d.pointerId = ev.getPointerId(p);
                    d.x = (int) ev.getX(p);
                    d.y = (int) ev.getY(p);
                    jniTouch(mJniHandle, EVENT_TOUCHMOVE, d.pointerId, d.x, d.y);
                }
                mSurface.requestRender();
                return true;
            }
        } return super.onTouchEvent(ev);
    }
}