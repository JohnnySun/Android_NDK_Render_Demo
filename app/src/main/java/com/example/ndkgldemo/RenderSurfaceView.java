package com.example.ndkgldemo;

import android.content.Context;
import android.support.annotation.NonNull;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class RenderSurfaceView extends SurfaceView implements SurfaceHolder.Callback {
    private boolean hasSurface = false;
    public RenderSurfaceView(Context context) {
        super(context);
        init();
    }

    public RenderSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public RenderSurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    public RenderSurfaceView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        init();
    }

    private void init() {
        initRender();
        this.getHolder().addCallback(this);
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int format, int width, int height) {
        setSurface(surfaceHolder.getSurface(), width, height);
        hasSurface = true;
        postRenderFrame();
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {
        hasSurface = false;
        setSurface(null, 0, 0);
    }

    private void postRenderFrame() {
        if (!hasSurface) {
            return;
        }
        renderFrame();
        postDelayed(new Runnable() {
            @Override
            public void run() {
                postRenderFrame();
            }
        }, 32);
    }
    private native void setSurface(Surface surface, int width, int height);

    private native void initRender();

    private native void renderFrame();
}
