#include "elastos/droid/opengl/GLSurfaceView.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/opengl/CGLDebugHelper.h"
#include "elastos/droid/opengl/gles/CEGL10Helper.h"
#include "elastos/droid/opengl/gles/CEGLContextImpl.h"
#include "Elastos.Droid.Content.h"

#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::IO::EIID_IWriter;
using Elastos::IO::EIID_ICloseable;
using Elastos::IO::EIID_IFlushable;
using Elastos::Core::StringUtils;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Content::Pm::IConfigurationInfo;
using Elastosx::Microedition::Khronos::Egl::IEGLContextHelper;
using Elastosx::Microedition::Khronos::Egl::IEGL;
using Elastos::Droid::View::Accessibility::EIID_IAccessibilityEventSource;
using Elastos::Droid::View::EIID_ISurfaceHolderCallback;
using Elastos::Droid::View::EIID_IKeyEventCallback;
using Elastos::Droid::View::EIID_ISurfaceView;
using Elastos::Droid::View::EIID_ISurfaceHolderCallback;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::ISurfaceHolderCallback;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawableCallback;

using Elastosx::Microedition::Khronos::Egl::IEGL10Helper;
using Elastos::Droid::Opengl::Gles::CEGL10Helper;
using Elastos::Droid::Opengl::Gles::CEGLContextImpl;

namespace Elastos {
namespace Droid {
namespace Opengl {

String GLSurfaceView::TAG("GLSurfaceView");
Boolean GLSurfaceView::LOG_ATTACH_DETACH = FALSE;
Boolean GLSurfaceView::LOG_THREADS = FALSE;
Boolean GLSurfaceView::LOG_PAUSE_RESUME = FALSE;
Boolean GLSurfaceView::LOG_SURFACE = FALSE;
Boolean GLSurfaceView::LOG_RENDERER = FALSE;
Boolean GLSurfaceView::LOG_RENDERER_DRAW_FRAME = FALSE;
Boolean GLSurfaceView::LOG_EGL = FALSE;
Object GLSurfaceView::sLockMgr;
Int32 GLSurfaceView::GLThreadManager::kGLES_20 = 0x20000;
String GLSurfaceView::GLThreadManager::kMSM7K_RENDERER_PREFIX("Q3Dimension MSM7500 ");
String GLSurfaceView::GLThreadManager::TAG("GLThreadManager");
AutoPtr<GLSurfaceView::GLThreadManager> GLSurfaceView::sGLThreadManager = new GLThreadManager();

CAR_INTERFACE_IMPL(GLSurfaceView::DefaultContextFactory, Object, IEGLContextFactory)
CAR_INTERFACE_IMPL(GLSurfaceView::DefaultWindowSurfaceFactory, Object, IEGLWindowSurfaceFactory)
CAR_INTERFACE_IMPL(GLSurfaceView::BaseConfigChooser, Object, IEGLConfigChooser)

GLSurfaceView::DefaultContextFactory::DefaultContextFactory(
    /* [in] */ GLSurfaceView* host)
    : EGL_CONTEXT_CLIENT_VERSION_EX(0x3098)
{
    mHost = host;
}

ECode GLSurfaceView::DefaultContextFactory::CreateContext(
    /* [in] */ IEGL10* egl,
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* eglConfig,
    /* [out] */ XIEGLContext** ctx)
{
    VALIDATE_NOT_NULL(ctx)

    AutoPtr<ArrayOf<Int32> > attrib_list = ArrayOf<Int32>::Alloc(3);
    (*attrib_list)[0] = EGL_CONTEXT_CLIENT_VERSION_EX;
    (*attrib_list)[1] = mHost->mEGLContextClientVersion;
    (*attrib_list)[2] = IEGL10::_EGL_NONE;

    AutoPtr<IEGL10Helper> egl10Helper;
    CEGL10Helper::AcquireSingleton((IEGL10Helper**)&egl10Helper);
    AutoPtr<XIEGLContext> noContext;
    egl10Helper->GetNoContext((XIEGLContext**)&noContext);
    return egl->EglCreateContext(display, eglConfig, noContext,
        mHost->mEGLContextClientVersion != 0 ? attrib_list : NULL, ctx);
}

ECode GLSurfaceView::DefaultContextFactory::DestroyContext(
    /* [in] */ IEGL10* egl,
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLContext* context)
{
    Boolean destroyContext;
    egl->EglDestroyContext(display, context, &destroyContext);
    if (!destroyContext) {
            SLOGGERE("DefaultContextFactory", "display: %p, context: %p", display, context);
        if (LOG_THREADS) {
            Int64 tid;
            Thread::GetCurrentThread()->GetId(&tid);
            SLOGGERE("DefaultContextFactory", "tid: %ld", tid);
        }
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode GLSurfaceView::DefaultWindowSurfaceFactory::CreateWindowSurface(
    /* [in] */ IEGL10* egl,
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* config,
    /* [in] */ IInterface* nativeWindow,
    /* [out] */ XIEGLSurface** surface)
{
    VALIDATE_NOT_NULL(surface)

    ECode ec = egl->EglCreateWindowSurface(display, config, nativeWindow, NULL, surface);
    if (ec != NOERROR) {
        SLOGGERE("GLSurfaceView", "eglCreateWindowSurface error");
    }
    return ec;
}

ECode GLSurfaceView::DefaultWindowSurfaceFactory::DestroySurface(
    /* [in] */ IEGL10* egl,
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLSurface* surface)
{
    Boolean rst;
    return egl->EglDestroySurface(display, surface, &rst);
}

GLSurfaceView::BaseConfigChooser::BaseConfigChooser(
    /* [in] */ ArrayOf<Int32>* configSpec,
    /* [in] */ GLSurfaceView* host)
{
    mHost = host;
    mConfigSpec = FilterConfigSpec(configSpec);
}

ECode GLSurfaceView::BaseConfigChooser::ChooseConfig(
    /* [in] */ IEGL10* egl,
    /* [in] */ XIEGLDisplay* display,
    /* [out] */ XIEGLConfig** config)
{
    VALIDATE_NOT_NULL(config)

    AutoPtr<ArrayOf<Int32> > num_config = ArrayOf<Int32>::Alloc(1);
    Boolean eglChooseConfig;
    egl->EglChooseConfig(display, mConfigSpec, NULL, 0, num_config, &eglChooseConfig);
    if (!eglChooseConfig) {
        SLOGGERD("GLSurfaceView", "eglChooseConfig failed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 numConfigs = (*num_config)[0];

    if (numConfigs <= 0) {
        SLOGGERD("GLSurfaceView", "No configs match configSpec")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ArrayOf<XIEGLConfig*> > configs = ArrayOf<XIEGLConfig*>::Alloc(numConfigs);
    egl->EglChooseConfig(display, mConfigSpec, configs, numConfigs, num_config, &eglChooseConfig);
    if (!eglChooseConfig) {
        SLOGGERD("GLSurfaceView", "eglChooseConfig#2 failed")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    ChooseConfig(egl, display, configs, config);
    if (*config == NULL) {
        SLOGGERD("GLSurfaceView", "No config chosen")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

AutoPtr<ArrayOf<Int32> > GLSurfaceView::BaseConfigChooser::FilterConfigSpec(
    /* [in] */ ArrayOf<Int32>* configSpec)
{
    if(mHost->mEGLContextClientVersion != 2 && mHost->mEGLContextClientVersion != 3) {
        return configSpec;
    }
    /* We know none of the subclasses define EGL_RENDERABLE_TYPE.
     * And we know the configSpec is well formed.
     */
    Int32 len = configSpec->GetLength();
    AutoPtr<ArrayOf<Int32> > newConfigSpec = ArrayOf<Int32>::Alloc(len + 2);
    newConfigSpec->Copy(0, configSpec, 0, len - 1);
    (*newConfigSpec)[len - 1] = IEGL10::_EGL_RENDERABLE_TYPE;
    if (mHost->mEGLContextClientVersion == 2) {
        (*newConfigSpec)[len] = IEGL14::EGL_OPENGL_ES2_BIT;  /* EGL_OPENGL_ES2_BIT */
    } else {
        (*newConfigSpec)[len] = IEGLExt::EGL_OPENGL_ES3_BIT_KHR; /* EGL_OPENGL_ES3_BIT_KHR */
    }
    (*newConfigSpec)[len + 1] = IEGL10::_EGL_NONE;
    return newConfigSpec;
}

GLSurfaceView::ComponentSizeChooser::ComponentSizeChooser(
    /* [in] */ Int32 redSize,
    /* [in] */ Int32 greenSize,
    /* [in] */ Int32 blueSize,
    /* [in] */ Int32 alphaSize,
    /* [in] */ Int32 depthSize,
    /* [in] */ Int32 stencilSize,
    /* [in] */ GLSurfaceView* host) : GLSurfaceView::BaseConfigChooser(
            GetSuperParam(redSize, greenSize, blueSize, alphaSize, depthSize, stencilSize), host)
{
    mValue = ArrayOf<Int32>::Alloc(1);
    mRedSize = redSize;
    mGreenSize = greenSize;
    mBlueSize = blueSize;
    mAlphaSize = alphaSize;
    mDepthSize = depthSize;
    mStencilSize = stencilSize;
}

ECode GLSurfaceView::ComponentSizeChooser::ChooseConfig(
    /* [in] */ IEGL10* egl,
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ ArrayOf<XIEGLConfig*>* configs,
    /* [out] */ XIEGLConfig** rst)
{
    VALIDATE_NOT_NULL(rst)

    Int32 len = configs->GetLength();
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<XIEGLConfig> config = (*configs)[i];
        Int32 d = FindConfigAttrib(egl, display, config,
                IEGL10::_EGL_DEPTH_SIZE, 0);
        Int32 s = FindConfigAttrib(egl, display, config,
                IEGL10::_EGL_STENCIL_SIZE, 0);
        if ((d >= mDepthSize) && (s >= mStencilSize)) {
            Int32 r = FindConfigAttrib(egl, display, config,
                    IEGL10::_EGL_RED_SIZE, 0);
            Int32 g = FindConfigAttrib(egl, display, config,
                     IEGL10::_EGL_GREEN_SIZE, 0);
            Int32 b = FindConfigAttrib(egl, display, config,
                      IEGL10::_EGL_BLUE_SIZE, 0);
            Int32 a = FindConfigAttrib(egl, display, config,
                    IEGL10::_EGL_ALPHA_SIZE, 0);
            if ((r == mRedSize) && (g == mGreenSize)
                    && (b == mBlueSize) && (a == mAlphaSize)) {
                *rst = config;
                REFCOUNT_ADD(*rst);
                return NOERROR;
            }
        }
    }
    *rst = NULL;
    return NOERROR;
}


Int32 GLSurfaceView::ComponentSizeChooser::FindConfigAttrib(
    /* [in] */ IEGL10* egl,
    /* [in] */ XIEGLDisplay* display,
    /* [in] */ XIEGLConfig* config,
    /* [in] */ Int32 attribute,
    /* [in] */ Int32 defaultValue)
{
    Boolean r;
    egl->EglGetConfigAttrib(display, config, attribute, mValue, &r);
    if (r) {
        return (*mValue)[0];
    }
    return defaultValue;
}

AutoPtr<ArrayOf<Int32> > GLSurfaceView::ComponentSizeChooser::GetSuperParam(
    /* [in] */ Int32 redSize,
    /* [in] */ Int32 greenSize,
    /* [in] */ Int32 blueSize,
    /* [in] */ Int32 alphaSize,
    /* [in] */ Int32 depthSize,
    /* [in] */ Int32 stencilSize)
{
    AutoPtr<ArrayOf<Int32> > supParam = ArrayOf<Int32>::Alloc(13);
    (*supParam)[0] = IEGL10::_EGL_RED_SIZE;
    (*supParam)[1] = redSize;
    (*supParam)[2] = IEGL10::_EGL_GREEN_SIZE;
    (*supParam)[3] = greenSize;
    (*supParam)[4] = IEGL10::_EGL_BLUE_SIZE;
    (*supParam)[5] = blueSize;
    (*supParam)[6] = IEGL10::_EGL_ALPHA_SIZE;
    (*supParam)[7] = alphaSize;
    (*supParam)[8] = IEGL10::_EGL_DEPTH_SIZE;
    (*supParam)[9] = depthSize;
    (*supParam)[10] = IEGL10::_EGL_STENCIL_SIZE;
    (*supParam)[11] = stencilSize;
    (*supParam)[12] = IEGL10::_EGL_NONE;
    return supParam;
}

GLSurfaceView::SimpleEGLConfigChooser::SimpleEGLConfigChooser(
    /* [in] */ Boolean withDepthBuffer,
    /* [in] */ GLSurfaceView* host) : ComponentSizeChooser(8, 8, 8, 0, withDepthBuffer ? 16 : 0, 0, host)
{}

GLSurfaceView::EglHelper::EglHelper(
    /* [in] */ IWeakReference* glSurfaceViewWeakRef)
{
    mGLSurfaceViewWeakRef = glSurfaceViewWeakRef;
}

ECode GLSurfaceView::EglHelper::Start()
{
    if (LOG_EGL) {
        Int64 tid;
        Thread::GetCurrentThread()->GetId(&tid);
        SLOGGERW("EglHelper", "Start tid = %ld", tid)
    }
    /*
     * Get an EGL instance
     */
    mEgl = IEGL10::Probe(CEGLContextImpl::GetEGL());
    /*
     * Get to the default display.
     */
    mEgl->EglGetDisplay(NULL, (XIEGLDisplay**)&mEglDisplay);

    AutoPtr<IEGL10Helper> egl10Helper;
    CEGL10Helper::AcquireSingleton((IEGL10Helper**)&egl10Helper);
    AutoPtr<XIEGLDisplay> noDisplay;
    egl10Helper->GetNoDisplay((XIEGLDisplay**)&noDisplay);
    if (mEglDisplay == noDisplay) {
        SLOGGERE("EglHelper", "eglGetDisplay failed")
        return E_RUNTIME_EXCEPTION;
    }
    /*
     * We can now initialize EGL for that display
     */
    AutoPtr<ArrayOf<Int32> > version = ArrayOf<Int32>::Alloc(2);
    Boolean isInit;
    mEgl->EglInitialize(mEglDisplay, version, &isInit);
    if(!isInit) {
        SLOGGERE("EglHelper", "eglInitialize failed")
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<IGLSurfaceView> obj;
    GLSurfaceView* view;
    mGLSurfaceViewWeakRef->Resolve(EIID_IGLSurfaceView, (IInterface**)&obj);
    if (obj) {
        view = (GLSurfaceView*)(obj.Get());
        view->mEGLConfigChooser->ChooseConfig(mEgl, mEglDisplay, (XIEGLConfig**)&mEglConfig);

        /*
        * Create an EGL context. We want to do this as rarely as we can, because an
        * EGL context is a somewhat heavy object.
        */

        view->mEGLContextFactory->CreateContext(mEgl, mEglDisplay, mEglConfig, (XIEGLContext**)&mEglContext);
    } else {
        mEglConfig = NULL;
        mEglContext = NULL;
    }

    AutoPtr<XIEGLContext> noContext;
    egl10Helper->GetNoContext((XIEGLContext**)&noContext);
    if (mEglContext == NULL || mEglContext == noContext) {
        mEglContext = NULL;
        return ThrowEglException(String("createContext"));
    }
    if (LOG_EGL) {
        Int64 tid;
        Thread::GetCurrentThread()->GetId(&tid);
        SLOGGERW("EglHelper", "createContext = %p, tid = %ld", mEglContext.Get(), tid)
    }

    mEglSurface = NULL;
    return NOERROR;
}

ECode GLSurfaceView::EglHelper::CreateSurface(
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst)

    *rst = FALSE;
    if (LOG_EGL) {
        Int64 tid;
        Thread::GetCurrentThread()->GetId(&tid);
        SLOGGERW("EglHelper", "createSurface()  tid= %ld", tid)
    }
    /*
     * Check preconditions.
     */
    if (mEgl == NULL) {
        SLOGGERE("EglHelper", "egl not initialized")
        return E_RUNTIME_EXCEPTION;
    }
    if (mEglDisplay == NULL) {
        SLOGGERE("EglHelper", "eglDisplay not initialized")
        return E_RUNTIME_EXCEPTION;
    }
    if (mEglConfig == NULL) {
        SLOGGERE("EglHelper", "mEglConfig not initialized")
        return E_RUNTIME_EXCEPTION;
    }

    /*
     *  The window size has changed, so we need to create a new
     *  surface.
     */
    DestroySurfaceImp();

    /*
     * Create an EGL surface we can render into.
     */

    AutoPtr<IGLSurfaceView> obj;
    GLSurfaceView* view;
    mGLSurfaceViewWeakRef->Resolve(EIID_IGLSurfaceView, (IInterface**)&obj);
    if (obj) {
        view = (GLSurfaceView*)(obj.Get());
        AutoPtr<ISurfaceHolder> holder;
        view->GetHolder((ISurfaceHolder**)&holder);
        view->mEGLWindowSurfaceFactory->CreateWindowSurface(mEgl,
                mEglDisplay, mEglConfig, holder, (XIEGLSurface**)&mEglSurface);
    } else {
        mEglSurface = NULL;
        mEglContext = NULL;
    }

    AutoPtr<IEGL10Helper> egl10Helper;
    CEGL10Helper::AcquireSingleton((IEGL10Helper**)&egl10Helper);
    AutoPtr<XIEGLSurface> noSurface;
    egl10Helper->GetNoSurface((XIEGLSurface**)&noSurface);
    if (mEglSurface == NULL || mEglSurface == noSurface) {
        Int32 error;
        mEgl->EglGetError(&error);
        if (error == IEGL10::_EGL_BAD_NATIVE_WINDOW) {
            SLOGGERE("EglHelper", "createWindowSurface returned EGL_BAD_NATIVE_WINDOW.");
        }
        *rst = FALSE;
        return NOERROR;
    }

    /*
     * Before we can issue GL commands, we need to make sure
     * the context is current and bound to a surface.
     */
     Boolean r;
     mEgl->EglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext, &r);
    if (!r) {
        /*
         * Could not make the context current, probably because the underlying
         * SurfaceView surface has been destroyed.
         */
        Int32 err;
        mEgl->EglGetError(&err);
        LogEglErrorAsWarning(String("EGLHelper"), String("eglMakeCurrent"), err);
        *rst = FALSE;
        return NOERROR;
    }

    *rst = TRUE;
    return NOERROR;
}

AutoPtr<IGL> GLSurfaceView::EglHelper::CreateGL()
{
    AutoPtr<IGL> gl;
    mEglContext->GetGL((IGL**)&gl);
    AutoPtr<IGLSurfaceView> obj;
    GLSurfaceView* view;
    AutoPtr<IGL> rstGl;
    mGLSurfaceViewWeakRef->Resolve(EIID_IGLSurfaceView, (IInterface**)&obj);
    if (obj) {
        view = (GLSurfaceView*)(obj.Get());
        if (view->mGLWrapper != NULL) {
            view->mGLWrapper->Wrap(gl, (IGL**)&gl);
        }

        if ((view->mDebugFlags & (IGLSurfaceView::DEBUG_CHECK_GL_ERROR | IGLSurfaceView::DEBUG_LOG_GL_CALLS)) != 0) {
            Int32 configFlags = 0;
            AutoPtr<IWriter> log = NULL;
            if ((view->mDebugFlags & IGLSurfaceView::DEBUG_CHECK_GL_ERROR) != 0) {
                configFlags |= IGLDebugHelper::_CONFIG_CHECK_GL_ERROR;
            }
            if ((view->mDebugFlags & IGLSurfaceView::DEBUG_LOG_GL_CALLS) != 0) {
                log = new LogWriter();
            }
            AutoPtr<IGLDebugHelper> helper;
            CGLDebugHelper::AcquireSingleton((IGLDebugHelper**)&helper);
            helper->Wrap(gl, configFlags, log, (IGL**)&rstGl);
        }
    }
    return gl;
}

Int32 GLSurfaceView::EglHelper::Swap()
{
    Boolean r;
    mEgl->EglSwapBuffers(mEglDisplay, mEglSurface, &r);
    if (!r) {
        Int32 error;
        mEgl->EglGetError(&error);
        return error;
    }
    return IEGL10::_EGL_SUCCESS;
}

ECode GLSurfaceView::EglHelper::DestroySurface()
{
    if (LOG_EGL) {
        Int64 tid;
        Thread::GetCurrentThread()->GetId(&tid);
        SLOGGERW("EglHelper", "destroySurface()  tid= %ld", tid)
    }
    DestroySurfaceImp();
    return NOERROR;
}

ECode GLSurfaceView::EglHelper::Finish()
{
    if (LOG_EGL) {
        Int64 tid;
        Thread::GetCurrentThread()->GetId(&tid);
        SLOGGERW("EglHelper", "finish()  tid= %ld", tid)
    }
    if (mEglContext != NULL) {
        AutoPtr<IGLSurfaceView> obj;
        GLSurfaceView* view;
        mGLSurfaceViewWeakRef->Resolve(EIID_IGLSurfaceView, (IInterface**)&obj);
        if (obj) {
            view = (GLSurfaceView*)(obj.Get());
            view->mEGLContextFactory->DestroyContext(mEgl, mEglDisplay, mEglContext);
        }
        mEglContext = NULL;
    }
    if (mEglDisplay != NULL) {
        Boolean r;
        mEgl->EglTerminate(mEglDisplay, &r);
        mEglDisplay = NULL;
    }
    return NOERROR;
}

ECode GLSurfaceView::EglHelper::DestroySurfaceImp()
{
    AutoPtr<IEGL10Helper> helper;
    CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
    AutoPtr<XIEGLSurface> noSur;
    helper->GetNoSurface((XIEGLSurface**)&noSur);
    AutoPtr<XIEGLContext> noCtx;
    helper->GetNoContext((XIEGLContext**)&noCtx);
    if (mEglSurface != NULL && mEglSurface != noSur) {
        Boolean r;
        mEgl->EglMakeCurrent(mEglDisplay, noSur,
                noSur,
                noCtx,
                &r);
        AutoPtr<IGLSurfaceView> obj;
        GLSurfaceView* view;
        mGLSurfaceViewWeakRef->Resolve(EIID_IGLSurfaceView, (IInterface**)&obj);
        if (obj) {
            view = (GLSurfaceView*)(obj.Get());
            view->mEGLWindowSurfaceFactory->DestroySurface(mEgl, mEglDisplay, mEglSurface);
        }
        mEglSurface = NULL;
    }
    return NOERROR;
}

ECode GLSurfaceView::EglHelper::ThrowEglException(
    /* [in] */ const String& function,
    /* [in] */ Int32 error)
{
    String mes = FormatEglError(function, error);
    if (LOG_EGL) {
        Int64 tid;
        Thread::GetCurrentThread()->GetId(&tid);
        SLOGGERE("EglHelper", String("LogEglException, tid = %ld") + mes, tid)
    }
    return E_RUNTIME_EXCEPTION;
}

ECode GLSurfaceView::EglHelper::LogEglErrorAsWarning(
    /* [in] */ const String& tag,
    /* [in] */ const String& function,
    /* [in] */ Int32 error)
{
    SLOGGERW(tag, FormatEglError(function, error))
    return NOERROR;
}

String GLSurfaceView::EglHelper::FormatEglError(
    /* [in] */ const String& function,
    /* [in] */ Int32 error)
{
    return function + "failed: " + GetErrorString(error);
}

String GLSurfaceView::EglHelper::GetErrorString(
    /* [in] */ Int32 error)
{
    switch (error) {
    case IEGL10::_EGL_SUCCESS:
        return String("EGL_SUCCESS");
    case IEGL10::_EGL_NOT_INITIALIZED:
        return String("EGL_NOT_INITIALIZED");
    case IEGL10::_EGL_BAD_ACCESS:
        return String("EGL_BAD_ACCESS");
    case IEGL10::_EGL_BAD_ALLOC:
        return String("EGL_BAD_ALLOC");
    case IEGL10::_EGL_BAD_ATTRIBUTE:
        return String("EGL_BAD_ATTRIBUTE");
    case IEGL10::_EGL_BAD_CONFIG:
        return String("EGL_BAD_CONFIG");
    case IEGL10::_EGL_BAD_CONTEXT:
        return String("EGL_BAD_CONTEXT");
    case IEGL10::_EGL_BAD_CURRENT_SURFACE:
        return String("EGL_BAD_CURRENT_SURFACE");
    case IEGL10::_EGL_BAD_DISPLAY:
        return String("EGL_BAD_DISPLAY");
    case IEGL10::_EGL_BAD_MATCH:
        return String("EGL_BAD_MATCH");
    case IEGL10::_EGL_BAD_NATIVE_PIXMAP:
        return String("EGL_BAD_NATIVE_PIXMAP");
    case IEGL10::_EGL_BAD_NATIVE_WINDOW:
        return String("EGL_BAD_NATIVE_WINDOW");
    case IEGL10::_EGL_BAD_PARAMETER:
        return String("EGL_BAD_PARAMETER");
    case IEGL10::_EGL_BAD_SURFACE:
        return String("EGL_BAD_SURFACE");
    case IEGL11::_EGL_CONTEXT_LOST:
        return String("EGL_CONTEXT_LOST");
    default:
        return StringUtils::ToString(error, 16);
    }
}

ECode GLSurfaceView::EglHelper::ThrowEglException(
    /* [in] */ const String& function)
{
    Int32 error;
    mEgl->EglGetError(&error);
    return ThrowEglException(function, error);
}

GLSurfaceView::GLThread::GLThread(
    /* [in] */ IWeakReference* glSurfaceViewWeakRef)
    : mShouldExit(FALSE)
    , mExited(FALSE)
    , mRequestPaused(FALSE)
    , mPaused(FALSE)
    , mHasSurface(FALSE)
    , mSurfaceIsBad(FALSE)
    , mWaitingForSurface(FALSE)
    , mHaveEglContext(FALSE)
    , mHaveEglSurface(FALSE)
    , mShouldReleaseEglContext(FALSE)
    , mRenderComplete(FALSE)
    , mSizeChanged(TRUE)
{
    Thread::constructor();
    mWidth = 0;
    mHeight = 0;
    mRequestRender = TRUE;
    mRenderMode = IGLSurfaceView::RENDERMODE_CONTINUOUSLY;
    mGLSurfaceViewWeakRef = glSurfaceViewWeakRef;
}

ECode GLSurfaceView::GLThread::Run()
{
    Int64 id;
    GetId(&id);
    SetName(String("GLThread ") + StringUtils::ToString(id));
    if (LOG_THREADS) {
        SLOGGERI("GLThread", "starting tid = %ld", id)
    }

    // try {
        GuardedRun();
    // } catch (InterruptedException e) {
        // fall thru and exit normally
    // } finally {
        sGLThreadManager->ThreadExiting(this);
    // }
    return NOERROR;
}

Boolean GLSurfaceView::GLThread::AbleToDraw()
{
    return mHaveEglContext && mHaveEglSurface && ReadyToDraw();
}

ECode GLSurfaceView::GLThread::SetRenderMode(
    /* [in] */ Int32 renderMode)
{
    if ( !((IGLSurfaceView::RENDERMODE_WHEN_DIRTY <= renderMode) && (renderMode <= IGLSurfaceView::RENDERMODE_CONTINUOUSLY)) ) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(sLockMgr);
    mRenderMode = renderMode;
    sLockMgr.NotifyAll();

    return NOERROR;
}

Int32 GLSurfaceView::GLThread::GetRenderMode()
{
    AutoLock lock(sLockMgr);
    return mRenderMode;
}

ECode GLSurfaceView::GLThread::RequestRender()
{
    AutoLock lock(sLockMgr);
    mRequestRender = TRUE;
    sLockMgr.NotifyAll();
    return NOERROR;
}

ECode GLSurfaceView::GLThread::SurfaceCreated()
{
    AutoLock lock(sLockMgr);
    if (LOG_THREADS) {
        Int64 tid;
        GetId(&tid);
        SLOGGERI("GLThread", "surfaceCreated tid = %d", tid)
    }
    mHasSurface = TRUE;
    mFinishedCreatingEglSurface = FALSE;

    sLockMgr.NotifyAll();
    while((mWaitingForSurface) && !mFinishedCreatingEglSurface && (!mExited)) {
        // try {
        if (sLockMgr.Wait() == (ECode)E_INTERRUPTED_EXCEPTION) {
            GetCurrentThread()->Interrupt();
        }
    }
    return NOERROR;
}

ECode GLSurfaceView::GLThread::SurfaceDestroyed()
{
    AutoLock lock(sLockMgr);
    if (LOG_THREADS) {
        Int64 id;
        GetId(&id);
        SLOGGERI("GLThread", "surfaceDestroyed tid = %d", id)
    }
    mHasSurface = FALSE;
    sLockMgr.NotifyAll();
    while((!mWaitingForSurface) && (!mExited)) {
        // try {
        if (sLockMgr.Wait() == (ECode)E_INTERRUPTED_EXCEPTION) {
            GetCurrentThread()->Interrupt();
        }
    }
    return NOERROR;
}

ECode GLSurfaceView::GLThread::OnPause()
{
    AutoLock lock(sLockMgr);
    if (LOG_PAUSE_RESUME) {
        Int64 id;
        GetId(&id);
        SLOGGERI("GLThread", "onPause tid = %d", id)
    }
    mRequestPaused = TRUE;
    sLockMgr.NotifyAll();
    while ((! mExited) && (! mPaused)) {
        if (LOG_PAUSE_RESUME) {
            SLOGGERI("Main thread", "onPause waiting for mPaused.")
        }
        // try {
        if (sLockMgr.Wait() == (ECode)E_INTERRUPTED_EXCEPTION) {
            GetCurrentThread()->Interrupt();
        }
    }
    return NOERROR;
}

ECode GLSurfaceView::GLThread::OnResume()
{
    if (LOG_PAUSE_RESUME) {
        Int64 id;
        GetId(&id);
        SLOGGERI("GLThread", "OnResume tid = %d", id)
    }
    mRequestPaused = FALSE;
    mRequestRender = TRUE;
    mRenderComplete = FALSE;
    sLockMgr.NotifyAll();
    while ((! mExited) && mPaused && (!mRenderComplete)) {
        if (LOG_PAUSE_RESUME) {
            SLOGGERI("Main thread", "onResume waiting for !mPaused.")
        }
        // try {
        if (sLockMgr.Wait() == (ECode)E_INTERRUPTED_EXCEPTION) {
            GetCurrentThread()->Interrupt();
        }
    }
    return NOERROR;
}

ECode GLSurfaceView::GLThread::OnWindowResize(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    AutoLock lock(sLockMgr);
    mWidth = w;
    mHeight = h;
    mSizeChanged = TRUE;
    mRequestRender = TRUE;
    mRenderComplete = FALSE;
    sLockMgr.NotifyAll();

    // Wait for thread to react to resize and render a frame
    while (! mExited && !mPaused && !mRenderComplete
            && AbleToDraw()) {
        if (LOG_SURFACE) {
            Int64 id;
            GetId(&id);
            SLOGGERI("Main thread", "onWindowResize waiting for render complete from tid = %d", id)
        }
        // try {
        if (sLockMgr.Wait() == (ECode)E_INTERRUPTED_EXCEPTION) {
            GetCurrentThread()->Interrupt();
        }
    }
    return NOERROR;
}

ECode GLSurfaceView::GLThread::RequestExitAndWait()
{
    AutoLock lock(sLockMgr);
    mShouldExit = TRUE;
    sLockMgr.NotifyAll();
    while (! mExited) {
        // try {
        if (sLockMgr.Wait() == (ECode)E_INTERRUPTED_EXCEPTION) {
            GetCurrentThread()->Interrupt();
        }
    }
    return NOERROR;
}

ECode GLSurfaceView::GLThread::RequestReleaseEglContextLocked()
{
    mShouldReleaseEglContext = TRUE;
    sLockMgr.NotifyAll();
    return NOERROR;
}

ECode GLSurfaceView::GLThread::QueueEvent(
    /* [in] */ IRunnable* r)
{
    if (r == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoLock lock(sLockMgr);
    mEventQueue.PushBack(r);
    sLockMgr.NotifyAll();
    return NOERROR;
}

ECode GLSurfaceView::GLThread::StopEglSurfaceLocked()
{
    if (mHaveEglSurface) {
        mHaveEglSurface = FALSE;
        mEglHelper->DestroySurface();
    }
    return NOERROR;
}

ECode GLSurfaceView::GLThread::StopEglContextLocked()
{
    if (mHaveEglContext) {
        mEglHelper->Finish();
        mHaveEglContext = FALSE;
        sGLThreadManager->ReleaseEglContextLocked(this);
    }
    return NOERROR;
}

ECode GLSurfaceView::GLThread::GuardedRun()
{
    mEglHelper = new EglHelper(mGLSurfaceViewWeakRef);
    mHaveEglContext = FALSE;
    mHaveEglSurface = FALSE;
    // try {
        AutoPtr<IGL10> gl;
        Boolean createEglContext = FALSE;
        Boolean createEglSurface = FALSE;
        Boolean createGlInterface = FALSE;
        Boolean lostEglContext = FALSE;
        Boolean sizeChanged = FALSE;
        Boolean wantRenderNotification = FALSE;
        Boolean doRenderNotification = FALSE;
        Boolean askedToReleaseEglContext = FALSE;
        Int32 w = 0;
        Int32 h = 0;
        AutoPtr<IRunnable> event;

        while (TRUE) {
            {
                AutoLock lock(sLockMgr);
                while (TRUE) {
                    if (mShouldExit) {
                        return NOERROR;
                    }

                    if (! mEventQueue.IsEmpty()) {
                        event = mEventQueue[0];
                        mEventQueue.Remove(0);
                        break;
                    }

                    // Update the pause state.
                    Boolean pausing = FALSE;
                    if (mPaused != mRequestPaused) {
                        pausing = mRequestPaused;
                        mPaused = mRequestPaused;
                        sLockMgr.NotifyAll();
                        if (LOG_PAUSE_RESUME) {
                            Int64 tid;
                            GetId(&tid);
                            SLOGGERI("GLThread", String("mPaused is now ") +=
                                mPaused ? String("TRUE") : String("FALSE") += String(" tid = %ld"), tid);
                        }
                    }

                    // Do we need to give up the EGL context?
                    if (mShouldReleaseEglContext) {
                        if (LOG_SURFACE) {
                            Int64 tid;
                            GetId(&tid);
                            SLOGGERI("GLThread", "releasing EGL context because asked to tid = %ld", tid);
                        }
                        StopEglSurfaceLocked();
                        StopEglContextLocked();
                        mShouldReleaseEglContext = FALSE;
                        askedToReleaseEglContext = TRUE;
                    }

                    // Have we lost the EGL context?
                    if (lostEglContext) {
                        StopEglSurfaceLocked();
                        StopEglContextLocked();
                        lostEglContext = FALSE;
                    }

                    // When pausing, release the EGL surface:
                    if (pausing && mHaveEglSurface) {
                        if (LOG_SURFACE) {
                            Int64 tid;
                            GetId(&tid);
                            SLOGGERI("GLThread", "releasing EGL surface because paused tid= %ld", tid);
                        }
                        StopEglSurfaceLocked();
                    }

                    // When pausing, optionally release the EGL Context:
                    if (pausing && mHaveEglContext) {
                        AutoPtr<IGLSurfaceView> obj;
                        GLSurfaceView* view;
                        mGLSurfaceViewWeakRef->Resolve(EIID_IGLSurfaceView, (IInterface**)&obj);
                        Boolean preserveEglContextOnPause = FALSE;
                        if (obj) {
                            view = (GLSurfaceView*)(obj.Get());
                            preserveEglContextOnPause = view->mPreserveEGLContextOnPause;
                        }
                        if (!preserveEglContextOnPause || sGLThreadManager->ShouldReleaseEGLContextWhenPausing()) {
                            StopEglContextLocked();
                            if (LOG_SURFACE) {
                                Int64 tid;
                                GetId(&tid);
                                SLOGGERI("GLThread", "Releasing EGL context because paused tid = %ld", tid)
                            }
                        }
                    }

                    // When pausing, optionally terminate EGL:
                    if (pausing) {
                        if (sGLThreadManager->ShouldTerminateEGLWhenPausing()) {
                            mEglHelper->Finish();
                            if (LOG_SURFACE) {
                                Int64 tid;
                                GetId(&tid);
                                SLOGGERI("GLThread", "terminating EGL because paused tid = %ld", tid)
                            }
                        }
                    }

                    // Have we lost the SurfaceView surface?
                    if ((! mHasSurface) && (! mWaitingForSurface)) {
                        if (LOG_SURFACE) {
                            Int64 tid;
                            GetId(&tid);
                            SLOGGERI("GLThread", "noticed surfaceView surface lost tid = %ld", tid)
                        }
                        if (mHaveEglSurface) {
                            StopEglSurfaceLocked();
                        }
                        mWaitingForSurface = TRUE;
                        mSurfaceIsBad = FALSE;
                        sLockMgr.NotifyAll();
                    }

                    // Have we acquired the surface view surface?
                    if (mHasSurface && mWaitingForSurface) {
                        if (LOG_SURFACE) {
                            Int64 tid;
                            GetId(&tid);
                            SLOGGERI("GLThread", "noticed surfaceView surface acquired tid = %ld", tid)
                        }
                        mWaitingForSurface = FALSE;
                        sLockMgr.NotifyAll();
                    }

                    if (doRenderNotification) {
                        if (LOG_SURFACE) {
                            Int64 tid;
                            GetId(&tid);
                            SLOGGERI("GLThread", "sending render notification tid = %ld", tid)
                        }
                        wantRenderNotification = FALSE;
                        doRenderNotification = FALSE;
                        mRenderComplete = TRUE;
                        sLockMgr.NotifyAll();
                    }

                    // Ready to draw?
                    if (ReadyToDraw()) {

                        // If we don't have an EGL context, try to acquire one.
                        if (! mHaveEglContext) {
                            if (askedToReleaseEglContext) {
                                askedToReleaseEglContext = FALSE;
                            } else if (sGLThreadManager->TryAcquireEglContextLocked(this)) {
                                ECode ec = mEglHelper->Start();
                                if (FAILED(ec)) {
                                    sGLThreadManager->ReleaseEglContextLocked(this);
                                    {
                                        AutoLock lock(sLockMgr);
                                        StopEglSurfaceLocked();
                                        StopEglContextLocked();
                                    }
                                    return ec;
                                }
                                mHaveEglContext = TRUE;
                                createEglContext = TRUE;

                                sLockMgr.NotifyAll();
                            }
                        }

                        if (mHaveEglContext && !mHaveEglSurface) {
                            mHaveEglSurface = TRUE;
                            createEglSurface = TRUE;
                            createGlInterface = TRUE;
                            sizeChanged = TRUE;
                        }

                        if (mHaveEglSurface) {
                            if (mSizeChanged) {
                                sizeChanged = TRUE;
                                w = mWidth;
                                h = mHeight;
                                wantRenderNotification = TRUE;
                                if (LOG_SURFACE) {
                                    Int64 tid;
                                    GetId(&tid);
                                    SLOGGERI("GLThread",
                                            "noticing that we want render notification tid = %ld", tid)
                                }

                                // Destroy and recreate the EGL surface.
                                createEglSurface = TRUE;

                                mSizeChanged = FALSE;
                            }
                            mRequestRender = FALSE;
                            sLockMgr.NotifyAll();
                            break;
                        }
                    }

                    // By design, this is the only place in a GLThread thread where we wait().
                    if (LOG_THREADS) {
                        Int64 tid;
                        GetId(&tid);
                        StringBuilder sb("waiting tid = "); sb += tid;
                        sb += "mHaveEglContext = "; sb += mHaveEglContext;
                        sb += "mHaveEglSurface = "; sb += mHaveEglSurface;
                        sb += " mFinishedCreatingEglSurface: "; sb += mFinishedCreatingEglSurface;
                        sb += "mPaused = "; sb += mPaused;
                        sb += "mHasSurface = "; sb += mHasSurface;
                        sb += "mSurfaceIsBad = "; sb += mSurfaceIsBad;
                        sb += "mWaitingForSurface = "; sb += mWaitingForSurface;
                        sb += "mWidth = "; sb += mWidth;
                        sb += "mHeight = "; sb += mHeight;
                        sb += "mRequestRender = "; sb += mRequestRender;
                        sb += "mRenderMode = "; sb += mRenderMode;
                        String logStr = sb.ToString();
                        SLOGGERI("GLThread", logStr)
                    }
                    if(FAILED(sLockMgr.Wait())) {
                        {
                            AutoLock lock(sLockMgr);
                            StopEglSurfaceLocked();
                            StopEglContextLocked();
                        }
                    }
                }
            } // end of synchronized(sGLThreadManager)

            if (event != NULL) {
                event->Run();
                event = NULL;
                continue;
            }

            if (createEglSurface) {
                if (LOG_SURFACE) {
                    SLOGGERW("GLThread", "egl createSurface")
                }
                Boolean created;
                mEglHelper->CreateSurface(&created);
                if (created) {
                    {
                        AutoLock lock(sLockMgr);
                        mFinishedCreatingEglSurface = TRUE;
                        sLockMgr.NotifyAll();
                    }
                } else {
                    {
                        AutoLock lock(sLockMgr);
                        mFinishedCreatingEglSurface = TRUE;
                        mSurfaceIsBad = TRUE;
                        sLockMgr.NotifyAll();
                    }
                    continue;
                }
                createEglSurface = FALSE;
            }

            if (createGlInterface) {
                AutoPtr<IGL> glTmp = mEglHelper->CreateGL();
                gl = IGL10::Probe(glTmp);

                sGLThreadManager->CheckGLDriver(gl);
                createGlInterface = FALSE;
            }

            if (createEglContext) {
                if (LOG_RENDERER) {
                    SLOGGERW("GLThread", "onSurfaceCreated")
                }
                AutoPtr<IGLSurfaceView> obj;
                GLSurfaceView* view;
                mGLSurfaceViewWeakRef->Resolve(EIID_IGLSurfaceView, (IInterface**)&obj);
                if (obj) {
                    view = (GLSurfaceView*)(obj.Get());
                    view->mRenderer->OnSurfaceCreated(gl, mEglHelper->mEglConfig);
                }
                createEglContext = FALSE;
            }

            if (sizeChanged) {
                if (LOG_RENDERER) {
                    SLOGGERW("GLThread", "onSurfaceChanged(%d, %d)", w, h);
                }
                AutoPtr<IGLSurfaceView> obj;
                GLSurfaceView* view;
                mGLSurfaceViewWeakRef->Resolve(EIID_IGLSurfaceView, (IInterface**)&obj);
                if (obj) {
                    view = (GLSurfaceView*)(obj.Get());
                    view->mRenderer->OnSurfaceChanged(gl, w, h);
                }
                sizeChanged = FALSE;
            }

            if (LOG_RENDERER_DRAW_FRAME) {
                Int64 tid;
                GetId(&tid);
                SLOGGERW("GLThread", "onDrawFrame tid = %ld", tid)
            }
            {
                AutoPtr<IGLSurfaceView> obj;
                GLSurfaceView* view;
                mGLSurfaceViewWeakRef->Resolve(EIID_IGLSurfaceView, (IInterface**)&obj);
                if (obj) {
                    view = (GLSurfaceView*)(obj.Get());
                    view->mRenderer->OnDrawFrame(gl);
                }
            }
            Int32 swapError = mEglHelper->Swap();
            switch (swapError) {
                case IEGL10::_EGL_SUCCESS:
                    break;
                case IEGL11::_EGL_CONTEXT_LOST:
                    if (LOG_SURFACE) {
                        Int64 tid;
                        GetId(&tid);
                        SLOGGERI("GLThread", "egl context lost tid = %ld", tid)
                    }
                    lostEglContext = TRUE;
                    break;
                default:
                    // Other errors typically mean that the current surface is bad,
                    // probably because the SurfaceView surface has been destroyed,
                    // but we haven't been notified yet.
                    // Log the error to help developers understand why rendering stopped.
                    EglHelper::LogEglErrorAsWarning(String("GLThread"), String("eglSwapBuffers"), swapError);

                    {
                        AutoLock lock(sLockMgr);
                        mSurfaceIsBad = TRUE;
                        sLockMgr.NotifyAll();
                    }
                    break;
            }

            if (wantRenderNotification) {
                doRenderNotification = TRUE;
            }
    }
    return NOERROR;
}

Boolean GLSurfaceView::GLThread::ReadyToDraw()
{
    return (!mPaused) && mHasSurface && (!mSurfaceIsBad)
        && (mWidth > 0) && (mHeight > 0)
        && (mRequestRender || (mRenderMode == IGLSurfaceView::RENDERMODE_CONTINUOUSLY));
}

GLSurfaceView::GLThreadManager::GLThreadManager()
    : mGLESVersionCheckComplete(FALSE)
    , mGLESVersion(0)
    , mGLESDriverCheckComplete(FALSE)
    , mMultipleGLESContextsAllowed(FALSE)
    , mLimitedGLESContexts(FALSE)
{}

ECode GLSurfaceView::GLThreadManager::ThreadExiting(
    /* [in] */ GLThread* thread)
{
    AutoLock lock(sLockMgr);
    if (LOG_THREADS) {
        Int64 id;
        thread->GetId(&id);
        SLOGGERI("GLThread", "exiting tid = %ld", id)
    }
    thread->mExited = TRUE;
    if (mEglOwner.Get() == thread) {
        mEglOwner = NULL;
    }
    sLockMgr.NotifyAll();
    return NOERROR;
}

Boolean GLSurfaceView::GLThreadManager::TryAcquireEglContextLocked(
    /* [in] */ GLThread* thread)
{
    if (mEglOwner.Get() == thread || mEglOwner == NULL) {
        mEglOwner = thread;
        sLockMgr.NotifyAll();
        return TRUE;
    }
    CheckGLESVersion();
    if (mMultipleGLESContextsAllowed) {
        return TRUE;
    }
    // Notify the owning thread that it should release the context.
    // TODO: implement a fairness policy. Currently
    // if the owning thread is drawing continuously it will just
    // reacquire the EGL context.
    if (mEglOwner != NULL) {
        mEglOwner->RequestReleaseEglContextLocked();
    }
    return FALSE;
}

ECode GLSurfaceView::GLThreadManager::ReleaseEglContextLocked(
    /* [in] */ GLThread* thread)
{
    if (mEglOwner.Get() == thread) {
        mEglOwner = NULL;
    }
    sLockMgr.NotifyAll();
    return NOERROR;
}

Boolean GLSurfaceView::GLThreadManager::ShouldReleaseEGLContextWhenPausing()
{
    AutoLock lock(sLockMgr);
    // Release the EGL context when pausing even if
    // the hardware supports multiple EGL contexts.
    // Otherwise the device could run out of EGL contexts.
    return mLimitedGLESContexts;
}

Boolean GLSurfaceView::GLThreadManager::ShouldTerminateEGLWhenPausing()
{
    AutoLock lock(sLockMgr);
    CheckGLESVersion();
    return !mMultipleGLESContextsAllowed;
}

ECode GLSurfaceView::GLThreadManager::CheckGLDriver(
    /* [in] */ IGL10* gl)
{
    AutoLock lock(sLockMgr);
    if (! mGLESDriverCheckComplete) {
        CheckGLESVersion();
        String renderer;
        gl->GlGetString(IGL10::_GL_RENDERER, &renderer);
        if (mGLESVersion < kGLES_20) {
            mMultipleGLESContextsAllowed =
                ! renderer.StartWith(kMSM7K_RENDERER_PREFIX);
            sLockMgr.NotifyAll();
        }
        mLimitedGLESContexts = !mMultipleGLESContextsAllowed;
        if (LOG_SURFACE) {
            StringBuilder sb("checkGLDriver renderer = \"");
            sb += renderer;
            sb += "\" multipleContextsAllowed = ";
            sb += mMultipleGLESContextsAllowed;
            sb += " mLimitedGLESContexts = ";
            sb += mLimitedGLESContexts;
            String logStr = sb.ToString();
            SLOGGERW(TAG, logStr)
        }
        mGLESDriverCheckComplete = TRUE;
    }
    return NOERROR;
}

ECode GLSurfaceView::GLThreadManager::CheckGLESVersion()
{
    if (! mGLESVersionCheckComplete) {
        SystemProperties::GetInt32(
                String("ro.opengles.version"),
                IConfigurationInfo::GL_ES_VERSION_UNDEFINED, &mGLESVersion);
        if (mGLESVersion >= kGLES_20) {
            mMultipleGLESContextsAllowed = TRUE;
        }
        if (LOG_SURFACE) {
            SLOGGERW(TAG, String("checkGLESVersion mGLESVersion = %d mMultipleGLESContextsAllowed = %s"),
                 mGLESVersion, mMultipleGLESContextsAllowed ? "TRUE" : "FALSE");
        }
        mGLESVersionCheckComplete = TRUE;
    }
    return NOERROR;
}

ECode GLSurfaceView::LogWriter::Write(
    /* [in] */ ArrayOf<Char32>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    for(Int32 i = 0; i < count; i++) {
        Char32 c = (*buffer)[offset + i];
        if (c == '\n') {
            FlushBuilder();
        }
        else {
            mBuilder += c;
        }
    }
    return NOERROR;
}

ECode GLSurfaceView::LogWriter::Close()
{
    return FlushBuilder();
}

ECode GLSurfaceView::LogWriter::Flush()
{
    return FlushBuilder();
}

ECode GLSurfaceView::LogWriter::FlushBuilder()
{
    if (mBuilder.GetLength() > 0) {
        SLOGGERV("GLSurfaceView", mBuilder.ToString());
        mBuilder.Delete(0, mBuilder.GetLength());
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL_7(GLSurfaceView, SurfaceView, IGLSurfaceView, ISurfaceView, IView, IDrawableCallback, IKeyEventCallback, IAccessibilityEventSource, ISurfaceHolderCallback)
GLSurfaceView::GLSurfaceView()
    : mDetached(FALSE)
    , mDebugFlags(0)
    , mEGLContextClientVersion(0)
    , mPreserveEGLContextOnPause(FALSE)
{
}

ECode GLSurfaceView::constructor(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(SurfaceView::constructor(context));
    InitInternal();
    return NOERROR;
}

ECode GLSurfaceView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(SurfaceView::constructor(context));
    InitInternal();
    return NOERROR;
}

GLSurfaceView::~GLSurfaceView()
{
    // try {
        if (mGLThread != NULL) {
            // GLThread may still be running if this view was never
            // attached to a window.
            mGLThread->RequestExitAndWait();
        }
    // } finally {
        // super.finalize();
    // }
}

ECode GLSurfaceView::SetGLWrapper(
    /* [in] */ IGLWrapper* glWrapper)
{
    mGLWrapper = glWrapper;
    return NOERROR;
}

ECode GLSurfaceView::SetDebugFlags(
    /* [in] */ Int32 debugFlags)
{
    mDebugFlags = debugFlags;
    return NOERROR;
}

ECode GLSurfaceView::GetDebugFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags)

    *flags = mDebugFlags;
    return NOERROR;
}

ECode GLSurfaceView::SetPreserveEGLContextOnPause(
    /* [in] */ Boolean preserveOnPause)
{
    mPreserveEGLContextOnPause = preserveOnPause;
    return NOERROR;
}

ECode GLSurfaceView::GetPreserveEGLContextOnPause(
    /* [out] */ Boolean* preserveEGLContextOnPause)
{
    VALIDATE_NOT_NULL(preserveEGLContextOnPause)

    *preserveEGLContextOnPause = mPreserveEGLContextOnPause;
    return NOERROR;
}

ECode GLSurfaceView::SetRenderer(
    /* [in] */ IRenderer* renderer)
{
    CheckRenderThreadState();
    if (mEGLConfigChooser == NULL) {
        mEGLConfigChooser = new SimpleEGLConfigChooser(TRUE, this);
    }
    if (mEGLContextFactory == NULL) {
        mEGLContextFactory = new DefaultContextFactory(this);
    }
    if (mEGLWindowSurfaceFactory == NULL) {
        mEGLWindowSurfaceFactory = new DefaultWindowSurfaceFactory();
    }
    mRenderer = renderer;
    if (mThisWeakRef == NULL) {
        GetWeakReference((IWeakReference**)&mThisWeakRef);
    }
    mGLThread = new GLThread(mThisWeakRef);
    mGLThread->Start();
    return NOERROR;
}

ECode GLSurfaceView::SetEGLContextFactory(
    /* [in] */ IEGLContextFactory* factory)
{
    CheckRenderThreadState();
    mEGLContextFactory = factory;
    return NOERROR;
}

ECode GLSurfaceView::SetEGLWindowSurfaceFactory(
    /* [in] */ IEGLWindowSurfaceFactory* factory)
{
    CheckRenderThreadState();
    mEGLWindowSurfaceFactory = factory;
    return NOERROR;
}

ECode GLSurfaceView::SetEGLConfigChooser(
    /* [in] */ IEGLConfigChooser* configChooser)
{
    CheckRenderThreadState();
    mEGLConfigChooser = configChooser;
    return NOERROR;
}

ECode GLSurfaceView::SetEGLConfigChooser(
    /* [in] */ Boolean needDepth)
{
    AutoPtr<SimpleEGLConfigChooser> chooser = new SimpleEGLConfigChooser(needDepth, this);
    SetEGLConfigChooser(chooser);
    return NOERROR;
}

ECode GLSurfaceView::SetEGLConfigChooser(
    /* [in] */ Int32 redSize,
    /* [in] */ Int32 greenSize,
    /* [in] */ Int32 blueSize,
    /* [in] */ Int32 alphaSize,
    /* [in] */ Int32 depthSize,
    /* [in] */ Int32 stencilSize)
{
    AutoPtr<ComponentSizeChooser> chooser = new ComponentSizeChooser(redSize, greenSize,
                blueSize, alphaSize, depthSize, stencilSize, this);
    SetEGLConfigChooser(chooser);
    return NOERROR;
}

ECode GLSurfaceView::SetEGLContextClientVersion(
    /* [in] */ Int32 v)
{
    CheckRenderThreadState();
    mEGLContextClientVersion = v;
    return NOERROR;
}

ECode GLSurfaceView::SetRenderMode(
    /* [in] */ Int32 renderMode)
{
    mGLThread->SetRenderMode(renderMode);
    return NOERROR;
}

ECode GLSurfaceView::GetRenderMode(
    /* [out] */ Int32* renderMode)
{
    VALIDATE_NOT_NULL(renderMode)

    *renderMode = mGLThread->GetRenderMode();
    return NOERROR;
}

ECode GLSurfaceView::RequestRender()
{
    return mGLThread->RequestRender();
}

ECode GLSurfaceView::SurfaceCreated(
    /* [in] */ ISurfaceHolder* holder)
{
    return mGLThread->SurfaceCreated();
}

ECode GLSurfaceView::SurfaceChanged(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return mGLThread->OnWindowResize(width, height);
}

ECode GLSurfaceView::SurfaceDestroyed(
    /* [in] */ ISurfaceHolder* holder)
{
    return mGLThread->SurfaceDestroyed();
}

ECode GLSurfaceView::OnPause()
{
    return mGLThread->OnPause();
}

ECode GLSurfaceView::OnResume()
{
    return mGLThread->OnResume();
}

ECode GLSurfaceView::QueueEvent(
    /* [in] */ IRunnable* r)
{
    return mGLThread->QueueEvent(r);
}

ECode GLSurfaceView::OnAttachedToWindow()
{
    SurfaceView::OnAttachedToWindow();
    if (LOG_ATTACH_DETACH) {
        SLOGGERD(TAG, "onAttachedToWindow reattach = %s", mDetached ? "TRUE" : "FALSE")
    }
    if (mDetached && (mRenderer != NULL)) {
        Int32 renderMode = IGLSurfaceView::RENDERMODE_CONTINUOUSLY;
        if (mGLThread != NULL) {
            renderMode = mGLThread->GetRenderMode();
        }
        if (mThisWeakRef == NULL) {
            GetWeakReference((IWeakReference**)&mThisWeakRef);
        }
        mGLThread = new GLThread(mThisWeakRef);
        if (renderMode != IGLSurfaceView::RENDERMODE_CONTINUOUSLY) {
            mGLThread->SetRenderMode(renderMode);
        }
        mGLThread->Start();
    }
    mDetached = FALSE;
    return NOERROR;
}

ECode GLSurfaceView::OnDetachedFromWindow()
{
    if (LOG_ATTACH_DETACH) {
        SLOGGERD(TAG, "onDetachedFromWindow");
    }
    if (mGLThread != NULL) {
        mGLThread->RequestExitAndWait();
    }
    mDetached = TRUE;
    SurfaceView::OnDetachedFromWindow();
    return NOERROR;
}

ECode GLSurfaceView::InitInternal()
{
    // Install a SurfaceHolder.Callback so we get notified when the
    // underlying surface is created and destroyed
    AutoPtr<ISurfaceHolder> holder;
    GetHolder((ISurfaceHolder**)&holder);
    holder->AddCallback(this);
    // setFormat is done by SurfaceView in SDK 2.3 and newer. Uncomment
    // this statement if back-porting to 2.2 or older:
    // holder.setFormat(PixelFormat.RGB_565);
    //
    // setType is not needed for SDK 2.0 or newer. Uncomment this
    // statement if back-porting this code to older SDKs.
    // holder.setType(SurfaceHolder.SURFACE_TYPE_GPU);
    return NOERROR;
}

ECode GLSurfaceView::CheckRenderThreadState()
{
    if (mGLThread != NULL) {
        SLOGGERE(TAG, "setRenderer has already been called for this instance.");
            return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos
