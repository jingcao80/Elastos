
#ifndef __ELASTOS_DROID_CHROMIUM_CHROMIUM_DRAWGLFUNCTOR_H__
#define __ELASTOS_DROID_CHROMIUM_CHROMIUM_DRAWGLFUNCTOR_H__

// import android.view.HardwareCanvas;
// import android.view.ViewRootImpl;
// import android.util.Log;

// import org.chromium.content.common.CleanupReference;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

// Simple Java abstraction and wrapper for the native DrawGLFunctor flow.
// An instance of this class can be constructed, bound to a single view context (i.e. AwContennts)
// and then drawn and detached from the view tree any number of times (using requestDrawGL and
// detach respectively). Then when finished with, it can be explicitly released by calling
// destroy() or will clean itself up as required via finalizer / CleanupReference.
class DrawGLFunctor
{
private:
    // Holds the core resources of the class, everything required to correctly cleanup.
    // IMPORTANT: this class must not hold any reference back to the outer DrawGLFunctor
    // instance, as that will defeat GC of that object.
    class DestroyRunnable
        : public Object
        , public IRunnable
    {
    public:
        DestroyRunnable(
            /* [in] */ Int64 nativeDrawGLFunctor);

        CAR_INTERFACE_DECL();

        // Called when the outer DrawGLFunctor instance has been GC'ed, i.e this is its finalizer.
        //@Override
        CARAPI Run();

        CARAPI_(void) DetachNativeFunctor();

    public:
        AutoPtr<IViewRootImpl> mViewRootImpl;
        Int64 mNativeDrawGLFunctor;
    };

public:
    DrawGLFunctor(
        /* [in] */ Int64 viewContext);

    CARAPI_(void) Destroy();

    CARAPI_(void) Detach();

    CARAPI_(Boolean) RequestDrawGL(
        /* [in] */ IHardwareCanvas* canvas,
        /* [in] */ IViewRootImpl* viewRootImpl,
        /* [in] */ Boolean waitForCompletion);

    static CARAPI_(void) SetChromiumAwDrawGLFunction(
        /* [in] */ Int64 functionPointer);

private:
    static CARAPI_(Int64) NativeCreateGLFunctor(
        /* [in] */ Int64 viewContext);

    static CARAPI_(void) NativeDestroyGLFunctor(
        /* [in] */ Int64 functor);

    static CARAPI_(void) NativeSetChromiumAwDrawGLFunction(
        /* [in] */ Int64 functionPointer);

private:
    static const String TAG;

    // Pointer to native side instance
    AutoPtr<CleanupReference> mCleanupReference;
    AutoPtr<DestroyRunnable> mDestroyRunnable;
};

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_CHROMIUM_CHROMIUM_DRAWGLFUNCTOR_H__
