
namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

//===============================================================
//               DrawGLFunctor::DestroyRunnable
//===============================================================

DrawGLFunctor::DestroyRunnable::DestroyRunnable(
    /* [in] */  Int64 nativeDrawGLFunctor)
    : mNativeDrawGLFunctor(nativeDrawGLFunctor)
{
}

// Called when the outer DrawGLFunctor instance has been GC'ed, i.e this is its finalizer.
//@Override
ECode DrawGLFunctor::DestroyRunnable::Run()
{
    DetachNativeFunctor();
    NativeDestroyGLFunctor(mNativeDrawGLFunctor);
    mNativeDrawGLFunctor = 0;
    return NOERROR;
}

void DrawGLFunctor::DestroyRunnable::DetachNativeFunctor()
{
    if (mNativeDrawGLFunctor != 0 && mViewRootImpl != NULL) {
        mViewRootImpl->DetachFunctor(mNativeDrawGLFunctor);
    }
    mViewRootImpl = NULL;
}

//===============================================================
//                        DrawGLFunctor
//===============================================================

const String DrawGLFunctor::TAG = DrawGLFunctor.class.getSimpleName();

DrawGLFunctor::DrawGLFunctor(
    /* [in] */ Int64 viewContext)
{
    mDestroyRunnable = new DestroyRunnable(nativeCreateGLFunctor(viewContext));
    mCleanupReference = new CleanupReference(this, mDestroyRunnable);
}

void DrawGLFunctor::Destroy()
{
    Detach();
    if (mCleanupReference != NULL) {
        mCleanupReference->CleanupNow();
        mCleanupReference = NULL;
        mDestroyRunnable = NULL;
    }
}

void DrawGLFunctor::Detach()
{
    mDestroyRunnable->DetachNativeFunctor();
}

Boolean DrawGLFunctor::RequestDrawGL(
    /* [in] */ IHardwareCanvas* canvas,
    /* [in] */ IViewRootImpl* viewRootImpl,
    /* [in] */ Boolean waitForCompletion)
{
    if (mDestroyRunnable->mNativeDrawGLFunctor == 0) {
//        throw new RuntimeException("requested DrawGL on already destroyed DrawGLFunctor");
        assert(0);
    }

    if (viewRootImpl == NULL) {
        // Can happen during teardown when window is leaked.
        return FALSE;
    }

    mDestroyRunnable->mViewRootImpl = viewRootImpl;
    if (canvas == NULL) {
        viewRootImpl->InvokeFunctor(mDestroyRunnable->mNativeDrawGLFunctor, waitForCompletion);
        return TRUEs;
    }

    canvas->CallDrawGLFunction(mDestroyRunnable->mNativeDrawGLFunctor);
    if (waitForCompletion) {
        viewRootImpl->InvokeFunctor(mDestroyRunnable->mNativeDrawGLFunctor,
                waitForCompletion);
    }

    return TRUE;
}

void DrawGLFunctor::SetChromiumAwDrawGLFunction(
    /* [in] */ Int64 functionPointer)
{
    NativeSetChromiumAwDrawGLFunction(functionPointer);
}

Int64 DrawGLFunctor::NativeCreateGLFunctor(
    /* [in] */ Int64 viewContext)
{
}

void DrawGLFunctor::NativeDestroyGLFunctor(
    /* [in] */ Int64 functor)
{
}

void DrawGLFunctor::NativeSetChromiumAwDrawGLFunction(
    /* [in] */ Int64 functionPointer)
{
}

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
