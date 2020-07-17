#include <jni.h>
#include <string>
#include <time.h>
#include <android/native_window_jni.h>
#include <android/log.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

ANativeWindow * window = nullptr;
EGLSurface  m_surface = EGL_NO_SURFACE;
EGLDisplay mDisplay = EGL_NO_DISPLAY;
EGLContext m_context = EGL_NO_CONTEXT;
EGLConfig m_config = nullptr;
int m_width = 0;
int m_height = 0;

// from android samples
/* return current time in milliseconds */
static long now_ms() {
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return 1000.0 * res.tv_sec + (long) res.tv_nsec / 1e6;

}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_ndkgldemo_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndkgldemo_RenderSurfaceView_initRender(JNIEnv *env, jobject thiz) {
    int nGLRenderType = EGL_OPENGL_ES2_BIT;
    mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (mDisplay == EGL_NO_DISPLAY) {
        __android_log_print(ANDROID_LOG_ERROR, "DEMOTEST", "createEglDisplay Error");
        abort();
    }
    EGLint major, minor;
    if (!eglInitialize(mDisplay, &major, &minor)) {
        __android_log_print(ANDROID_LOG_ERROR, "DEMOTEST", "eglInitialize() returned error 0x%x", eglGetError());
        abort();
    }

    EGLint configSpec[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_RENDERABLE_TYPE, nGLRenderType,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_NONE
    };

    EGLConfig config;
    EGLint numConfigs;
    __android_log_print(ANDROID_LOG_DEBUG, "DEMOTEST", "Initializing context");


    if (!eglChooseConfig(mDisplay, configSpec, &config, 1, &numConfigs)) {
        __android_log_print(ANDROID_LOG_ERROR, "DEMOTEST", "eglChooseConfig() returned error 0x%x", eglGetError());
        abort();
    }

    m_config = config;
    EGLint attribList[]{
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };
    m_context = eglCreateContext(mDisplay, config, EGL_NO_CONTEXT, attribList);
    if (m_context == EGL_NO_CONTEXT) {
        __android_log_print(ANDROID_LOG_ERROR, "DEMOTEST", "eglCreateContext Failed:  0x%x", eglGetError());
    }

}

void makeCurrent() {
    if (window == EGL_NO_SURFACE) {
        __android_log_print(ANDROID_LOG_ERROR, "DEMOTEST", "Invalid Surface!");
        abort();
    }
    if (!eglMakeCurrent(mDisplay, m_surface, m_surface, m_context)) {
        __android_log_print(ANDROID_LOG_ERROR, "DEMOTEST", "eglMakeCurrent failed: 0x%x", eglGetError());
        abort();
    }
}

void doneCurrent() {
    if (mDisplay == EGL_NO_DISPLAY) {
        return;
    }

    if (EGL_TRUE != eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
        __android_log_print(ANDROID_LOG_ERROR, "DEMOTEST", "%s:: eglMakeCurrent error 0x%x",  __FUNCTION__, eglGetError());
        abort();
    }
}

void draw() {
    long k = now_ms()/50;
    float red = (k % 255)/(float)255;
    float green = ((2*k) % 255)/(float)255;
    float blue = ((3*k) % 255)/(float)255;
    glClearColor(red, green, blue,1);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, m_width, m_height);
    eglSwapBuffers(mDisplay, m_surface);
}

void destroySurface() {
    if(mDisplay != EGL_NO_DISPLAY && m_surface != EGL_NO_SURFACE) {
        eglDestroySurface(mDisplay, m_surface);
        m_surface = EGL_NO_SURFACE;
    }
}

void releaseSurface() {
    destroySurface();
    doneCurrent();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndkgldemo_RenderSurfaceView_setSurface(JNIEnv *env, jobject thiz, jobject surface,
                                                        jint width, jint height) {
    if (surface == NULL && window != nullptr) {
        ANativeWindow_release(window);
        releaseSurface();
        return;
    }
    window = ANativeWindow_fromSurface(env, surface);
    m_width = width;
    m_height = height;
    ANativeWindow_setBuffersGeometry(window, width, height, WINDOW_FORMAT_RGBA_8888);
    EGLint attribList[]{
            EGL_NONE
    };
    m_surface = eglCreateWindowSurface(mDisplay, m_config, window, attribList);
    makeCurrent();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndkgldemo_RenderSurfaceView_renderFrame(JNIEnv *env, jobject thiz) {
    draw();
}