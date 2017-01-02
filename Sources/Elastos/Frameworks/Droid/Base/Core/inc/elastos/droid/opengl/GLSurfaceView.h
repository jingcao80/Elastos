//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_OPENGL_GLSURFACEVIEW_H__
#define __ELASTOS_DROID_OPENGL_GLSURFACEVIEW_H__

#include "Elastos.Droid.Opengl.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/view/SurfaceView.h"

#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/io/Writer.h>

using Elastos::IO::IWriter;
using Elastos::IO::IFlushable;
using Elastos::IO::ICloseable;
using Elastos::IO::Writer;
using Elastos::Core::Thread;
using Elastos::Core::Object;
using Elastos::Core::StringBuilder;
using Elastos::Droid::View::SurfaceView;
using Elastos::Droid::View::ISurfaceHolder;
using Elastos::Droid::View::ISurfaceHolderCallback;
using Elastosx::Microedition::Khronos::Egl::IEGL10;
using Elastosx::Microedition::Khronos::Egl::IEGL11;
using Elastosx::Microedition::Khronos::Opengles::IGL;
using Elastosx::Microedition::Khronos::Opengles::IGL10;

namespace Elastos {
namespace Droid {
namespace Opengl {

class GLSurfaceView
    : public SurfaceView
    , public IGLSurfaceView
    , public ISurfaceHolderCallback
{
typedef Elastosx::Microedition::Khronos::Egl::IEGLConfig XIEGLConfig;
typedef Elastosx::Microedition::Khronos::Egl::IEGLContext XIEGLContext;
typedef Elastosx::Microedition::Khronos::Egl::IEGLDisplay XIEGLDisplay;
typedef Elastosx::Microedition::Khronos::Egl::IEGLSurface XIEGLSurface;

private:
    class DefaultContextFactory
        : public IEGLContextFactory
        , public Object
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
        , public Object
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
        , public Object
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
        : public Object
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
        : public Thread
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
        Boolean mFinishedCreatingEglSurface;
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

    class GLThreadManager :public Object
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
        : public Writer
    {
    public:
        CARAPI Write(
            /* [in] */ ArrayOf<Char32>* buffer,
            /* [in] */ Int32 offset,
            /* [in] */ Int32 count);

        CARAPI Close();

        CARAPI Flush();

    private:
        CARAPI FlushBuilder();

        StringBuilder mBuilder;
    };

public:
    CAR_INTERFACE_DECL()

    GLSurfaceView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    ~GLSurfaceView();

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
    CARAPI OnAttachedToWindow();

    CARAPI OnDetachedFromWindow();

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
