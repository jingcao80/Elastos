#ifndef __ELASTOS_DROID_OPENGL_GLSURFACEVIEW_H__
#define __ELASTOS_DROID_OPENGL_GLSURFACEVIEW_H__

#include "elastos/droid/view/SurfaceView.h"
#include <elastos/core/Object.h>
#include "elastos/io/Writer.h"
#include <elastos/core/StringBuilder.h>

using Elastos::IO::IWriter;
using Elastos::IO::IFlushable;
using Elastos::IO::ICloseable;
using Elastos::IO::Writer;
using Elastos::Core::StringBuilder;
using Elastos::Droid::View::SurfaceView;
using Elastos::Droid::View::ISurfaceHolder;
using Elastosx::Microedition::Khronos::Egl::IEGL10;
using Elastosx::Microedition::Khronos::Egl::IEGL11;
using Elastosx::Microedition::Khronos::Opengles::IGL;
using Elastosx::Microedition::Khronos::Opengles::IGL10;

namespace Elastos {
namespace Droid {
namespace Opengl {

extern "C" const InterfaceID EIID_GLSurfaceView;

class GLSurfaceView
    : public SurfaceView
{
typedef Elastosx::Microedition::Khronos::Egl::IEGLConfig XIEGLConfig;
typedef Elastosx::Microedition::Khronos::Egl::IEGLContext XIEGLContext;
typedef Elastosx::Microedition::Khronos::Egl::IEGLDisplay XIEGLDisplay;
typedef Elastosx::Microedition::Khronos::Egl::IEGLSurface XIEGLSurface;

private:
    class DefaultContextFactory
        : public IEGLContextFactory
        , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        DefaultContextFactory(
            /* [in] */ GLSurfaceView* host);

        CARAPI CreateContext(
            /* [in] */ IEGL10* egl,
            /* [in] */ XIEGLDisplay* display,
            /* [in] */ XIEGLConfig* eglConfig,
            /* [out] */ XIEGLContext** ctx);

        CARAPI DestroyContext(
            /* [in] */ IEGL10* egl,
            /* [in] */ XIEGLDisplay* display,
            /* [in] */ XIEGLContext* context);
    private:
        Int32 EGL_CONTEXT_CLIENT_VERSION_EX;
        GLSurfaceView* mHost;
    };

    class DefaultWindowSurfaceFactory
        : public IEGLWindowSurfaceFactory
        , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI CreateWindowSurface(
            /* [in] */ IEGL10* egl,
            /* [in] */ XIEGLDisplay* display,
            /* [in] */ XIEGLConfig* config,
            /* [in] */ IInterface* nativeWindow,
            /* [out] */ XIEGLSurface** surface);

        CARAPI DestroySurface(
            /* [in] */ IEGL10* egl,
            /* [in] */ XIEGLDisplay* display,
            /* [in] */ XIEGLSurface* surface);
    };

    class BaseConfigChooser
        : public IEGLConfigChooser
        , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        BaseConfigChooser(
            /* [in] */ ArrayOf<Int32>* configSpec,
            /* [in] */ GLSurfaceView* host);

        CARAPI ChooseConfig(
            /* [in] */ IEGL10* egl,
            /* [in] */ XIEGLDisplay* display,
            /* [out] */ XIEGLConfig** config);

        virtual CARAPI ChooseConfig(
            /* [in] */ IEGL10* egl,
            /* [in] */ XIEGLDisplay* display,
            /* [in] */ ArrayOf<XIEGLConfig*>* configs,
            /* [out] */ XIEGLConfig** rst) = 0;

    private:
        CARAPI_(AutoPtr<ArrayOf<Int32> >) FilterConfigSpec(
            /* [in] */ ArrayOf<Int32>* configSpec);

    protected:
        AutoPtr<ArrayOf<Int32> > mConfigSpec;
        GLSurfaceView* mHost;
    };

    class ComponentSizeChooser
        : public BaseConfigChooser
    {
    public:
        ComponentSizeChooser(
            /* [in] */ Int32 redSize,
            /* [in] */ Int32 greenSize,
            /* [in] */ Int32 blueSize,
            /* [in] */ Int32 alphaSize,
            /* [in] */ Int32 depthSize,
            /* [in] */ Int32 stencilSize,
            /* [in] */ GLSurfaceView* host);

        virtual CARAPI ChooseConfig(
            /* [in] */ IEGL10* egl,
            /* [in] */ XIEGLDisplay* display,
            /* [in] */ ArrayOf<XIEGLConfig*>* configs,
            /* [out] */ XIEGLConfig** rst);

    private:
        CARAPI_(Int32) FindConfigAttrib(
            /* [in] */ IEGL10* egl,
            /* [in] */ XIEGLDisplay* display,
            /* [in] */ XIEGLConfig* configs,
            /* [in] */ Int32 attribute,
            /* [in] */ Int32 defaultValue);

        CARAPI_(AutoPtr<ArrayOf<Int32> >) GetSuperParam(
            /* [in] */ Int32 redSize,
            /* [in] */ Int32 greenSize,
            /* [in] */ Int32 blueSize,
            /* [in] */ Int32 alphaSize,
            /* [in] */ Int32 depthSize,
            /* [in] */ Int32 stencilSize);

    protected:
        Int32 mRedSize;
        Int32 mGreenSize;
        Int32 mBlueSize;
        Int32 mAlphaSize;
        Int32 mDepthSize;
        Int32 mStencilSize;

    private:
        AutoPtr<ArrayOf<Int32> > mValue;
    };

    class SimpleEGLConfigChooser
        : public ComponentSizeChooser
    {
    public:
        SimpleEGLConfigChooser(
            /* [in] */ Boolean withDepthBuffer,
            /* [in] */ GLSurfaceView* host);
    };

    class GLThread;
    class EglHelper
        : public ElRefBase
    {
    public:
        EglHelper(
            /* [in] */ IWeakReference* glSurfaceViewWeakRef);

        CARAPI Start();

        CARAPI CreateSurface(
            /* [out] */ Boolean* rst);

        CARAPI_(AutoPtr<IGL>) CreateGL();

        CARAPI_(Int32) Swap();

        CARAPI DestroySurface();

        CARAPI Finish();

        static CARAPI ThrowEglException(
            /* [in] */ const String& function,
            /* [in] */ Int32 error);

        static CARAPI LogEglErrorAsWarning(
            /* [in] */ const String& tag,
            /* [in] */ const String& function,
            /* [in] */ Int32 error);

        static CARAPI_(String) FormatEglError(
            /* [in] */ const String& function,
            /* [in] */ Int32 error);

    private:
        CARAPI DestroySurfaceImp();

        static CARAPI_(String) GetErrorString(
            /* [in] */ Int32 error);

        CARAPI ThrowEglException(
            /* [in] */ const String& function);
    private:
        AutoPtr<IWeakReference> mGLSurfaceViewWeakRef;
        AutoPtr<IEGL10> mEgl;
        AutoPtr<XIEGLDisplay> mEglDisplay;
        AutoPtr<XIEGLSurface> mEglSurface;
        AutoPtr<XIEGLConfig> mEglConfig;
        AutoPtr<XIEGLContext> mEglContext;
        friend class GLThread;
    };

    class GLThreadManager;
    class GLThread
        : public ThreadBase
    {
    public:
        GLThread(
            /* [in] */ IWeakReference* glSurfaceViewWeakRef);

        CARAPI Run();

        CARAPI_(Boolean) AbleToDraw();

        CARAPI SetRenderMode(
            /* [in] */ Int32 renderMode);

        CARAPI_(Int32) GetRenderMode();

        CARAPI RequestRender();

        CARAPI SurfaceCreated();

        CARAPI SurfaceDestroyed();

        CARAPI OnPause();

        CARAPI OnResume();

        CARAPI OnWindowResize(
            /* [in] */ Int32 w,
            /* [in] */ Int32 h);

        CARAPI RequestExitAndWait();

        CARAPI RequestReleaseEglContextLocked();

        CARAPI QueueEvent(
            /* [in] */ IRunnable* r);

    private:
        CARAPI StopEglSurfaceLocked();

        CARAPI StopEglContextLocked();

        CARAPI GuardedRun();

        CARAPI_(Boolean) ReadyToDraw();

    private:
        Boolean mShouldExit;
        Boolean mExited;
        Boolean mRequestPaused;
        Boolean mPaused;
        Boolean mHasSurface;
        Boolean mSurfaceIsBad;
        Boolean mWaitingForSurface;
        Boolean mHaveEglContext;
        Boolean mHaveEglSurface;
        Boolean mShouldReleaseEglContext;
        Int32 mWidth;
        Int32 mHeight;
        Int32 mRenderMode;
        Boolean mRequestRender;
        Boolean mRenderComplete;
        List<AutoPtr<IRunnable> > mEventQueue;
        Boolean mSizeChanged;
        AutoPtr<EglHelper> mEglHelper;
        AutoPtr<IWeakReference> mGLSurfaceViewWeakRef;
        friend class GLThreadManager;
    };

    class GLThreadManager :public ElRefBase
    {
    public:
        GLThreadManager();

        CARAPI ThreadExiting(
            /* [in] */ GLThread* thread);

        CARAPI_(Boolean) TryAcquireEglContextLocked(
            /* [in] */ GLThread* thread);

        CARAPI ReleaseEglContextLocked(
            /* [in] */ GLThread* thread);

        CARAPI_(Boolean) ShouldReleaseEGLContextWhenPausing();

        CARAPI_(Boolean) ShouldTerminateEGLWhenPausing();

        CARAPI CheckGLDriver(
            /* [in] */ IGL10* gl);

    private:
        CARAPI CheckGLESVersion();

    private:
        Boolean mGLESVersionCheckComplete;
        Int32 mGLESVersion;
        Boolean mGLESDriverCheckComplete;
        Boolean mMultipleGLESContextsAllowed;
        Boolean mLimitedGLESContexts;
        static Int32 kGLES_20;
        static String kMSM7K_RENDERER_PREFIX;
        static String TAG;
        AutoPtr<GLThread> mEglOwner;
    };

    class LogWriter
        : public IWriter
        , public ICloseable
        , public IFlushable
        , public Writer
        , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI Write(
            /* [in] */ Int32 oneChar32);

        CARAPI WriteChars(
            /* [in] */ const ArrayOf<Char32>& buffer);

        CARAPI WriteChars(
            /* [in] */ const ArrayOf<Char32>& buffer,
            /* [in] */ Int32 offset,
            /* [in] */ Int32 count);

        CARAPI WriteString(
            /* [in] */ const String& str);

        CARAPI WriteString(
            /* [in] */ const String& str,
            /* [in] */ Int32 offset,
            /* [in] */ Int32 count);

        CARAPI CheckError(
            /* [out] */ Boolean* hasError);

        CARAPI Close();

        CARAPI Flush();

        CARAPI AppendChar(
            /* [out] */ Char32 c);

        CARAPI AppendCharSequence(
            /* [out] */ ICharSequence* csq);

        CARAPI AppendCharSequence(
            /* [out] */ ICharSequence* csq,
            /* [out] */ Int32 start,
            /* [out] */ Int32 end);

        CARAPI GetLock(
            /* [out] */ IInterface** lockobj);
    private:
        CARAPI FlushBuilder();

        StringBuilder mBuilder;
    };

public:
    GLSurfaceView(
        /* [in] */ IContext* context);

    GLSurfaceView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    ~GLSurfaceView();

    virtual CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid) = 0;

    virtual CARAPI GetWeakReference(
        /* [out] */ IWeakReference** weakReference) = 0;

    CARAPI SetGLWrapper(
        /* [in] */ IGLWrapper* glWrapper);

    CARAPI SetDebugFlags(
        /* [in] */ Int32 debugFlags);

    CARAPI GetDebugFlags(
        /* [out] */ Int32* flags);

    CARAPI SetPreserveEGLContextOnPause(
        /* [in] */ Boolean preserveOnPause);

    CARAPI GetPreserveEGLContextOnPause(
        /* [out] */ Boolean* preserveEGLContextOnPause);

    CARAPI SetRenderer(
        /* [in] */ IRenderer* renderer);

    CARAPI SetEGLContextFactory(
        /* [in] */ IEGLContextFactory* factory);

    CARAPI SetEGLWindowSurfaceFactory(
        /* [in] */ IEGLWindowSurfaceFactory* factory);

    CARAPI SetEGLConfigChooser(
        /* [in] */ IEGLConfigChooser* configChooser);

    CARAPI SetEGLConfigChooser(
        /* [in] */ Boolean needDepth);

    CARAPI SetEGLConfigChooser(
        /* [in] */ Int32 redSize,
        /* [in] */ Int32 greenSize,
        /* [in] */ Int32 blueSize,
        /* [in] */ Int32 alphaSize,
        /* [in] */ Int32 depthSize,
        /* [in] */ Int32 stencilSize);

    CARAPI SetEGLContextClientVersion(
        /* [in] */ Int32 v);

    CARAPI SetRenderMode(
        /* [in] */ Int32 renderMode);

    CARAPI GetRenderMode(
        /* [out] */ Int32* renderMode);

    CARAPI RequestRender();

    CARAPI SurfaceCreated(
        /* [in] */ ISurfaceHolder* holder);

    CARAPI SurfaceChanged(
        /* [in] */ ISurfaceHolder* holder,
        /* [in] */ Int32 format,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI SurfaceDestroyed(
        /* [in] */ ISurfaceHolder* holder);

    CARAPI OnPause();

    CARAPI OnResume();

    CARAPI QueueEvent(
        /* [in] */ IRunnable* r);

protected:
    GLSurfaceView();

    CARAPI OnAttachedToWindow();

    CARAPI OnDetachedFromWindow();

    CARAPI Init(
        /* [in] */ IContext* context);

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

private:
    CARAPI InitInternal();

    CARAPI CheckRenderThreadState();

private:
    static AutoPtr<GLThreadManager> sGLThreadManager;

    AutoPtr<IWeakReference> mThisWeakRef;
    AutoPtr<GLThread> mGLThread;
    AutoPtr<IRenderer> mRenderer;
    Boolean mDetached;
    AutoPtr<IEGLConfigChooser> mEGLConfigChooser;
    AutoPtr<IEGLContextFactory> mEGLContextFactory;
    AutoPtr<IEGLWindowSurfaceFactory> mEGLWindowSurfaceFactory;
    AutoPtr<IGLWrapper> mGLWrapper;
    Int32 mDebugFlags;
    Int32 mEGLContextClientVersion;
    Boolean mPreserveEGLContextOnPause;

    static String TAG;
    static Boolean LOG_ATTACH_DETACH;
    static Boolean LOG_THREADS;
    static Boolean LOG_PAUSE_RESUME;
    static Boolean LOG_SURFACE;
    static Boolean LOG_RENDERER;
    static Boolean LOG_RENDERER_DRAW_FRAME;
    static Boolean LOG_EGL;
    static Object sLockMgr;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos


#endif //__ELASTOS_DROID_OPENGL_GLSURFACEVIEW_H__
