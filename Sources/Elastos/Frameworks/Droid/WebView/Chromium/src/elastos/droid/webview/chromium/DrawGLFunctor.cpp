
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/webview/chromium/DrawGLFunctor.h"
#include "elastos/droid/webkit/webview/chromium/NativeDrawGLFunctor.h"

using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

//=====================================================================
//                    DrawGLFunctor::DestroyRunnable
//=====================================================================
CAR_INTERFACE_IMPL(DrawGLFunctor::DestroyRunnable, Object, IRunnable)

DrawGLFunctor::DestroyRunnable::DestroyRunnable(
    /* [in] */ Int64 nativeDrawGLFunctor)
    : mNativeDrawGLFunctor(nativeDrawGLFunctor)
{
    // ==================before translated======================
    // mNativeDrawGLFunctor = nativeDrawGLFunctor;
}

ECode DrawGLFunctor::DestroyRunnable::Run()
{
    // ==================before translated======================
    // detachNativeFunctor();
    // nativeDestroyGLFunctor(mNativeDrawGLFunctor);
    // mNativeDrawGLFunctor = 0;

    DetachNativeFunctor();
    NativeDestroyGLFunctor(mNativeDrawGLFunctor);
    mNativeDrawGLFunctor = 0;
    return NOERROR;
}

ECode DrawGLFunctor::DestroyRunnable::DetachNativeFunctor()
{
    // ==================before translated======================
    // if (mNativeDrawGLFunctor != 0 && mViewRootImpl != null) {
    //     mViewRootImpl.detachFunctor(mNativeDrawGLFunctor);
    // }
    // mViewRootImpl = null;

    if (mNativeDrawGLFunctor != 0 && mViewRootImpl != NULL) {
        mViewRootImpl->DetachFunctor(mNativeDrawGLFunctor);
    }
    mViewRootImpl = NULL;
    return NOERROR;
}

//=====================================================================
//                            DrawGLFunctor
//=====================================================================
const String DrawGLFunctor::TAG(String("DrawGLFunctor"));//DrawGLFunctor.class.getSimpleName();

DrawGLFunctor::DrawGLFunctor(
    /* [in] */ Int64 viewContext)
{
    // ==================before translated======================
    // mDestroyRunnable = new DestroyRunnable(nativeCreateGLFunctor(viewContext));
    // mCleanupReference = new CleanupReference(this, mDestroyRunnable);

    mDestroyRunnable = new DestroyRunnable(NativeCreateGLFunctor(viewContext));
    mCleanupReference = new CleanupReference(this, mDestroyRunnable);
}

ECode DrawGLFunctor::Destroy()
{
    // ==================before translated======================
    // detach();
    // if (mCleanupReference != null) {
    //     mCleanupReference.cleanupNow();
    //     mCleanupReference = null;
    //     mDestroyRunnable = null;
    // }

    Detach();
    if (mCleanupReference != NULL) {
        mCleanupReference->CleanupNow();
        mCleanupReference = NULL;
        mDestroyRunnable = NULL;
    }
    return NOERROR;
}

ECode DrawGLFunctor::Detach()
{
    // ==================before translated======================
    // mDestroyRunnable.detachNativeFunctor();

    mDestroyRunnable->DetachNativeFunctor();
    return NOERROR;
}

Boolean DrawGLFunctor::RequestDrawGL(
    /* [in] */ IHardwareCanvas* canvas,
    /* [in] */ IViewRootImpl* viewRootImpl,
    /* [in] */ Boolean waitForCompletion)
{
    // ==================before translated======================
    // if (mDestroyRunnable.mNativeDrawGLFunctor == 0) {
    //     throw new RuntimeException("requested DrawGL on already destroyed DrawGLFunctor");
    // }
    // if (viewRootImpl == null) {
    //     // Can happen during teardown when window is leaked.
    //     return false;
    // }
    //
    // mDestroyRunnable.mViewRootImpl = viewRootImpl;
    // if (canvas == null) {
    //     viewRootImpl.invokeFunctor(mDestroyRunnable.mNativeDrawGLFunctor, waitForCompletion);
    //     return true;
    // }
    //
    // canvas.callDrawGLFunction(mDestroyRunnable.mNativeDrawGLFunctor);
    // if (waitForCompletion) {
    //     viewRootImpl.invokeFunctor(mDestroyRunnable.mNativeDrawGLFunctor,
    //             waitForCompletion);
    // }
    // return true;

    if (0 == mDestroyRunnable->mNativeDrawGLFunctor) {
        //throw new RuntimeException("requested DrawGL on already destroyed DrawGLFunctor");
        assert(0);
    }

    if (viewRootImpl == NULL) {
        // Can happen during teardown when window is leaked.
        return FALSE;
    }

    mDestroyRunnable->mViewRootImpl = viewRootImpl;
    if (canvas == NULL) {
        viewRootImpl->InvokeFunctor(mDestroyRunnable->mNativeDrawGLFunctor, waitForCompletion);
        return TRUE;
    }

    Int32 result = 0;
    canvas->CallDrawGLFunction(mDestroyRunnable->mNativeDrawGLFunctor, &result);
    if (waitForCompletion) {
        viewRootImpl->InvokeFunctor(mDestroyRunnable->mNativeDrawGLFunctor, waitForCompletion);
    }

    return TRUE;
}

ECode DrawGLFunctor::SetChromiumAwDrawGLFunction(
    /* [in] */ Int64 functionPointer)
{
    // ==================before translated======================
    // nativeSetChromiumAwDrawGLFunction(functionPointer);

    NativeSetChromiumAwDrawGLFunction(functionPointer);
    return NOERROR;
}

// Raise the file handle soft limit to the hard limit since gralloc buffers
// uses file handles.
static void RaiseFileNumberLimit() {
    static bool have_raised_limit = FALSE;
    if (have_raised_limit)
        return;

    have_raised_limit = TRUE;
    struct rlimit limit_struct;
    limit_struct.rlim_cur = 0;
    limit_struct.rlim_max = 0;
    if (getrlimit(RLIMIT_NOFILE, &limit_struct) == 0) {
        limit_struct.rlim_cur = limit_struct.rlim_max;
        if (setrlimit(RLIMIT_NOFILE, &limit_struct) != 0) {
            ALOGE("setrlimit failed: %s", strerror(errno));
        }
    } else {
        ALOGE("getrlimit failed: %s", strerror(errno));
    }
}

Int64 DrawGLFunctor::NativeCreateGLFunctor(
    /* [in] */ Int64 viewContext)
{
    RaiseFileNumberLimit();
    return (Int64)(new NativeDrawGLFunctor(viewContext));
}

ECode DrawGLFunctor::NativeDestroyGLFunctor(
    /* [in] */ Int64 functor)
{
    delete (NativeDrawGLFunctor*)(functor);
    return NOERROR;
}

ECode DrawGLFunctor::NativeSetChromiumAwDrawGLFunction(
    /* [in] */ Int64 functionPointer)
{
    NativeDrawGLFunctor::g_aw_drawgl_function = (AwDrawGLFunction*)(functionPointer);
    return NOERROR;
}

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
