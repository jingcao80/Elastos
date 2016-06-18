
#ifndef __ELASTOS_DROID_VIEW_Surface_H__
#define __ELASTOS_DROID_VIEW_Surface_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/Canvas.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::ICompatibilityInfoTranslator;
using Elastos::Droid::Graphics::Canvas;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::IBinder;
using Elastos::Core::ICloseGuard;

namespace Elastos {
namespace Droid {
namespace View {

class Surface
    : public Object
    , public ISurface
    , public IParcelable
{
    friend class SurfaceControl;
private:
    /**
     * A Canvas class that can handle the compatibility mode.
     * This does two things differently.
     * <ul>
     * <li>Returns the width and height of the target metrics, rather than
     * Native. For example, the canvas returns 320x480 even if an app is running
     * in WVGA high density.
     * <li>Scales the matrix in setMatrix by the application scale, except if
     * the matrix looks like obtained from getMatrix. This is a hack to handle
     * the case that an application uses getMatrix to keep the original matrix,
     * set matrix of its own, then set the original matrix back. There is no
     * perfect solution that works for all cases, and there are a lot of cases
     * that this model does not work, but we hope this works for many apps.
     * </ul>
     */

    class CompatibleCanvas : public Canvas
    {
    public:
        TO_STRING_IMPL("Surface::CompatibleCanvas")

        CompatibleCanvas(
            /* [in] */ Surface* owner);

        CARAPI SetMatrix(
            /* [in] */ IMatrix* matrix);

        CARAPI GetMatrix(
            /* [in] */ IMatrix* ctm);

    private:
        AutoPtr<IMatrix> mOrigMatrix;
        Surface* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    Surface();

    virtual ~Surface();

    /**
     * Create an empty surface, which will later be filled in by readFromParcel().
     * @hide
     */
    CARAPI constructor();

    /**
     * Create Surface from a {@link SurfaceTexture}.
     *
     * Images drawn to the Surface will be made available to the {@link
     * SurfaceTexture}, which can attach them to an OpenGL ES texture via {@link
     * SurfaceTexture#updateTexImage}.
     *
     * @param surfaceTexture The {@link SurfaceTexture} that is updated by this
     * Surface.
     */
    CARAPI constructor(
        /* [in] */ ISurfaceTexture* surfaceTexture);

    /* called from android_view_Surface_createFromIGraphicBufferProducer() */
    CARAPI constructor(
        /* [in] */ Int64 nativeObject);

    CARAPI SetDirtyRect(
        /* [in] */ IRect* dirty);

    CARAPI ReleaseResources();

    CARAPI Destroy();

    CARAPI IsValid(
        /* [out] */ Boolean* valid);

    CARAPI GetGenerationId(
        /* [out] */ Int32* generationId);

    CARAPI IsConsumerRunningBehind(
        /* [out] */ Boolean* consumer);

    CARAPI LockCanvas(
        /* [in] */ IRect * pDirty,
        /* [out] */ ICanvas** ppCanvas);

    CARAPI UnlockCanvasAndPost(
        /* [in] */ ICanvas* pCanvas);

    CARAPI UnlockCanvas(
        /* [in] */ ICanvas* pCanvas);

    /**
     * Transfer the Native state from 'other' to this surface, releasing it
     * from 'other'.  This is for use in the client side for drawing into a
     * surface; not guaranteed to work on the window manager side.
     * This is for use by the client to move the underlying surface from
     * one Surface object to another, in particular in SurfaceFlinger.
     * @hide.
     */
    CARAPI TransferFrom(
        /* [in] */ ISurface* other);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Sets the translator used to scale canvas's width/height in compatibility
     * mode.
     */
    CARAPI SetCompatibilityTranslator(
        /* [in] */ ICompatibilityInfoTranslator* translator);

    /**
     * Copy another surface to this one.  This surface now holds a reference
     * to the same data as the original surface, and is -not- the owner.
     * This is for use by the window manager when returning a window surface
     * back from a client, converting it from the representation being managed
     * by the window manager to the representation the client uses to draw
     * in to it.
     * @hide
     */
    CARAPI CopyFrom(
        /* [in] */ ISurfaceControl* other);

    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Allocate buffers ahead of time to avoid allocation delays during rendering
     * @hide
     */
    CARAPI AllocateBuffers();

    /**
     * Returns a human readable representation of a rotation.
     *
     * @param rotation The rotation.
     * @return The rotation symbolic name.
     *
     * @hide
     */
    CARAPI_(String) RotationToString(
        /* [in] */ Int32 rotation);

private:
    CARAPI NativeCreateFromSurfaceTexture(
        /* [in] */ ISurfaceTexture* surfaceTexture,
        /* [out] */ Int64* result);

    CARAPI_(Int64) NativeCreateFromSurfaceControl(
        /* [in] */ Int64 surfaceControlNativeObj);

    CARAPI NativeLockCanvas(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IRect* dirty,
        /* [out] */ Int64* result);

    CARAPI NativeUnlockCanvasAndPost(
        /* [in] */ ICanvas* canvas);

    CARAPI_(void) NativeRelease(
        /* [in] */ Int64 nativeObject);

    CARAPI_(Boolean) NativeIsValid();

    CARAPI NativeIsConsumerRunningBehind(
        /* [out] */ Boolean* result);

    CARAPI NativeTransferFrom(
        /* [in] */ ISurface* other);

    CARAPI NativeReadFromParcel(
        /* [in] */ IParcel* source,
        /* [out] */ Int64* result);

    CARAPI NativeWriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI NativeSetDirtyRect(
        /* [in] */ IRect* dirty);

    CARAPI_(void) NativeAllocateBuffers();

    CARAPI_(void) SetNativeObjectLocked(
        /* [in] */ Int64 ptr);

    CARAPI CheckNotReleasedLocked();

    CARAPI GetNativeSurface(
        /* [out] */ Int64* result);

private:
    static const char* TAG;

    AutoPtr<ICloseGuard> mCloseGuard;

    String mName;
    Int32 mGenerationId; // incremented each time mNativeObject changes
    AutoPtr<ICanvas> mCanvas;

    // A matrix to scale the matrix set by application. This is set to null for
    // non compatibility mode.
    AutoPtr<IMatrix> mCompatibleMatrix;

public:
    Int64 mNativeObject; // package scope only for SurfaceControl access
    Int64 mLockedObject;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_VIEW_Surface_H__
