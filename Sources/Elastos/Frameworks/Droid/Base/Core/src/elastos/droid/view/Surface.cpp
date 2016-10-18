
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/Surface.h"
#include "elastos/droid/view/SurfaceControl.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/graphics/CSurfaceTexture.h"
#include "elastos/droid/graphics/CRegion.h"
#include "elastos/droid/graphics/NativeCanvas.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/os/NativeBinder.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

#include <gui/SurfaceComposerClient.h>
#include <ui/PixelFormat.h>
#include <skia/core/SkCanvas.h>
#include <skia/core/SkBitmap.h>
#include <skia/core/SkRegion.h>
#include <utils/String8.h>
#include <skia/core/SkPixelRef.h>
//#include <ScopedUtfChars.h>
#include <ui/DisplayInfo.h>
#include <ui/Rect.h>
#include <ui/Region.h>
#include <binder/IBinder.h>
#include <binder/IMemory.h>
#include <binder/Parcel.h>
#include <android/native_window.h>
#include <gui/Surface.h>

using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::DroidObjectForIBinder;
using Elastos::Droid::Graphics::CSurfaceTexture;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::NativeCanvas;
using Elastos::Core::AutoLock;
using Elastos::Core::ICloseGuardHelper;
using Elastos::Core::CCloseGuardHelper;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;
using android::sp;

namespace Elastos {
namespace Droid {
namespace View {

static const void *sRefBaseOwner;

// ----------------------------------------------------------------------------

static inline bool isSurfaceValid(const sp<android::Surface>& sur) {
    return android::Surface::isValid(sur);
}

// ----------------------------------------------------------------------------

const char* Surface::TAG = "Surface";

static inline SkColorType convertPixelFormat(android::PixelFormat format) {
    /* note: if PIXEL_FORMAT_RGBX_8888 means that all alpha bytes are 0xFF, then
        we can map to kN32_SkColorType, and optionally call
        bitmap.setAlphaType(kOpaque_SkAlphaType) on the resulting SkBitmap
        (as an accelerator)
    */
    switch (format) {
    case android::PIXEL_FORMAT_RGBX_8888:    return kN32_SkColorType;
    case android::PIXEL_FORMAT_RGBA_8888:    return kN32_SkColorType;
    case android::PIXEL_FORMAT_RGB_565:      return kRGB_565_SkColorType;
    default:                        return kUnknown_SkColorType;
    }
}

////////////////////////////////////////////////////////////
// Surface::CompatibleCanvas

Surface::CompatibleCanvas::CompatibleCanvas(
    /* [in] */ Surface* owner)
    : mOwner(owner)
{
}

ECode Surface::CompatibleCanvas::SetMatrix(
    /* [in] */ IMatrix* matrix)
{
    Boolean isEquals;
    AutoPtr<IObject> obj = IObject::Probe(mOrigMatrix);
    if (mOwner->mCompatibleMatrix == NULL || mOrigMatrix == NULL
        || (obj->Equals(matrix, &isEquals), isEquals)) {
       // don't scale the matrix if it's not compatibility mode, or
       // the matrix was obtained from getMatrix.
       return Canvas::SetMatrix(matrix);
    }
    else {
       AutoPtr<IMatrix> m;
       CMatrix::New(mOwner->mCompatibleMatrix, (IMatrix**)&m);
       Boolean res;
       m->PreConcat(matrix, &res);
       return Canvas::SetMatrix(m);
    }
}

ECode Surface::CompatibleCanvas::GetMatrix(
    /* [in] */ IMatrix* m)
{
    Canvas::GetMatrix(m);
    if (mOrigMatrix == NULL) {
        CMatrix::New((IMatrix**)&mOrigMatrix);
    }
    return  mOrigMatrix->Set(m);
}

CAR_INTERFACE_IMPL_2(Surface, Object, ISurface, IParcelable)

ECode Surface::NativeCreateFromSurfaceTexture(
    /* [in] */ ISurfaceTexture* surfaceTexture,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = 0;
    CSurfaceTexture* stImp = (CSurfaceTexture*)surfaceTexture;
    sp<android::IGraphicBufferProducer> producer = (android::IGraphicBufferProducer*)(stImp->mProducer);
    if (producer == NULL) {
        SLOGGERD("Surface", "SurfaceTexture has already been released")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    sp<android::Surface> surface(new android::Surface(producer, true));
    if (surface == NULL) {
        return E_OUT_OF_RESOURCES_EXCEPTION;
        // jniThrowException(env, OutOfResourcesException, NULL);
    }

    surface->incStrong(&sRefBaseOwner);
    *result = reinterpret_cast<Int64>(surface.get());
    return NOERROR;
}

Int64 Surface::NativeCreateFromSurfaceControl(
    /* [in] */ Int64 surfaceControlNativeObj)
{
    /*
     * This is used by the WindowManagerService just after constructing
     * a Surface and is necessary for returning the Surface reference to
     * the caller. At this point, we should only have a SurfaceControl.
     */
    sp<android::SurfaceControl> ctrl(reinterpret_cast<android::SurfaceControl *>(surfaceControlNativeObj));
    sp<android::Surface> surface(ctrl->getSurface());
    if (surface != NULL) {
        surface->incStrong(&sRefBaseOwner);
    }
    return reinterpret_cast<Int64>(surface.get());
}

ECode Surface::NativeLockCanvas(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IRect* dirty,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    sp<android::Surface> surface(reinterpret_cast<android::Surface *>(mNativeObject));
    if (!isSurfaceValid(surface) || canvas == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::Rect dirtyRect;
    android::Rect* dirtyRectPtr = NULL;

    if (dirty) {
        Int32 top, bottom, left, right;
        dirty->Get(&left, &top, &right, &bottom);

        dirtyRect.left   = left;
        dirtyRect.top    = top;
        dirtyRect.right  = right;
        dirtyRect.bottom = bottom;
        dirtyRectPtr = &dirtyRect;
    }

    ANativeWindow_Buffer outBuffer;
    android::status_t err = surface->lock(&outBuffer, dirtyRectPtr);
    if (err < 0) {
        if (err == android::NO_MEMORY) {
            return E_OUT_OF_RESOURCES_EXCEPTION;
        }
        else {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    // Associate a SkCanvas object to this surface
    Canvas* canvasImp = (Canvas*)canvas;

    canvasImp->SetSurfaceFormat(outBuffer.format);

    SkImageInfo info = SkImageInfo::Make(outBuffer.width, outBuffer.height,
                                         convertPixelFormat(outBuffer.format),
                                         kPremul_SkAlphaType);
    if (outBuffer.format == android::PIXEL_FORMAT_RGBX_8888) {
        info.fAlphaType = kOpaque_SkAlphaType;
    }

    SkBitmap bitmap;
    ssize_t bpr = outBuffer.stride * android::bytesPerPixel(outBuffer.format);
    bitmap.setInfo(info, bpr);
    if (outBuffer.width > 0 && outBuffer.height > 0) {
        bitmap.setPixels(outBuffer.bits);
    } else {
        // be safe with an empty bitmap.
        bitmap.setPixels(NULL);
    }

    canvasImp->SetNativeBitmap(reinterpret_cast<Int64>(&bitmap));

    if (dirtyRectPtr) {
        SkCanvas* nativeCanvas = reinterpret_cast<NativeCanvas*>(canvasImp->mNativeCanvas)->getSkCanvas();
        assert(nativeCanvas != NULL);
        nativeCanvas->clipRect(SkRect::Make(reinterpret_cast<const SkIRect&>(dirtyRect)));
    }

    if (dirty) {
        dirty->Set(dirtyRect.left, dirtyRect.top, dirtyRect.right, dirtyRect.bottom);
    }

    // Create another reference to the surface and return it.  This reference
    // should be passed to nativeUnlockCanvasAndPost in place of mNativeObject,
    // because the latter could be replaced while the surface is locked.
    sp<android::Surface> lockedSurface(surface);
    lockedSurface->incStrong(&sRefBaseOwner);
    *result = reinterpret_cast<Int64>(lockedSurface.get());
    return NOERROR;
}

ECode Surface::NativeUnlockCanvasAndPost(
    /* [in] */ ICanvas* canvas)
{
    sp<android::Surface> surface(reinterpret_cast<android::Surface *>(mNativeObject));
    if (!isSurfaceValid(surface)) {
        return NOERROR;
    }

    // detach the canvas from the surface
    Canvas* canvasImp = (Canvas*)canvas;
    canvasImp->SetNativeBitmap(0);

    // unlock surface
    android::status_t err = surface->unlockAndPost();
    if (err < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

void Surface::NativeRelease(
    /* [in] */ Int64 nativeObject)
{
    sp<android::Surface> sur(reinterpret_cast<android::Surface *>(nativeObject));
    sur->decStrong(&sRefBaseOwner);
}

Boolean Surface::NativeIsValid()
{
    sp<android::Surface> sur(reinterpret_cast<android::Surface *>(mNativeObject));
    return isSurfaceValid(sur) ? TRUE : FALSE;
}

ECode Surface::NativeIsConsumerRunningBehind(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    sp<android::Surface> sur(reinterpret_cast<android::Surface *>(mNativeObject));
    if (!isSurfaceValid(sur)) {
        *result = FALSE;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    int value = 0;
    ANativeWindow* anw = static_cast<ANativeWindow*>(sur.get());
    anw->query(anw, NATIVE_WINDOW_CONSUMER_RUNNING_BEHIND, &value);
    *result = value == 0 ? FALSE : TRUE;
    return NOERROR;
}

ECode Surface::NativeReadFromParcel(
    /* [in] */ IParcel* source,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    if (source == NULL) {
        *result = 0;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;

    }
    android::Parcel* parcel;
    source->GetElementPayload((Handle32*)&parcel);
    if (parcel == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    sp<android::Surface> self(reinterpret_cast<android::Surface *>(mNativeObject));
    sp<android::IBinder> binder(parcel->readStrongBinder());

    // update the Surface only if the underlying IGraphicBufferProducer
    // has changed.
    if (self != NULL
            && (self->getIGraphicBufferProducer()->asBinder() == binder)) {
        // same IGraphicBufferProducer, return ourselves
        *result = reinterpret_cast<Int64>(self.get());
        return NOERROR;
    }

    sp<android::Surface> sur;
    sp<android::IGraphicBufferProducer> gbp(android::interface_cast<android::IGraphicBufferProducer>(binder));
    if (gbp != NULL) {
        // we have a new IGraphicBufferProducer, create a new Surface for it
        sur = new android::Surface(gbp, true);
        // and keep a reference before passing to java
        sur->incStrong(&sRefBaseOwner);
    }

    if (self != NULL) {
        // and loose the java reference to ourselves
        self->decStrong(&sRefBaseOwner);
    }

    *result = reinterpret_cast<Int64>(sur.get());
    return NOERROR;
}

ECode Surface::NativeWriteToParcel(
    /* [in] */ IParcel* dest)
{
    if (dest == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;

    }
    android::Parcel* parcel;
    dest->GetElementPayload((Handle32*)&parcel);
    if (parcel == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    sp<android::Surface> self(reinterpret_cast<android::Surface *>(mNativeObject));
    parcel->writeStrongBinder(self != 0 ? self->getIGraphicBufferProducer()->asBinder() : NULL);
    return NOERROR;
}

void Surface::NativeAllocateBuffers()
{
    sp<android::Surface> surface(reinterpret_cast<android::Surface *>(mNativeObject));
    if (!isSurfaceValid(surface)) {
        return;
    }

    surface->allocateBuffers();
}

ECode Surface::NativeSetDirtyRect(
    /* [in] */ IRect* dirty)
{
#ifdef QCOM_BSP
    sp<android::Surface> surface(reinterpret_cast<android::Surface *>(mNativeObject));

    if (!isSurfaceValid(surface)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 l, t, r, b;
    dirty->Get(&l, &t, &r, &b);

    android::Rect rect;
    rect.left = l;
    rect.top = t;
    rect.right = r;
    rect.bottom = b;

    surface->setDirtyRect(&rect);
#endif
    return NOERROR;
}

void Surface::SetNativeObjectLocked(
    /* [in] */ Int64 ptr)
{
    if (mNativeObject != ptr) {
        if (mNativeObject == 0 && ptr != 0) {
            mCloseGuard->Open(String("Surface::Release"));
        }
        else if (mNativeObject != 0 && ptr == 0) {
            mCloseGuard->Close();
        }
        mNativeObject = ptr;
        mGenerationId += 1;
    }
}

ECode Surface::CheckNotReleasedLocked()
{
    if (mNativeObject == 0) {
        // throw new IllegalStateException("Surface has already been released.");
        SLOGGERE(TAG, "Surface has already been released.")
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

Surface::Surface()
    : mGenerationId(0)
    , mNativeObject(0)
    , mLockedObject(0)
{
    AutoPtr<ICloseGuardHelper> helper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&helper);
    helper->Get((ICloseGuard**)&mCloseGuard);

    AutoPtr<CompatibleCanvas> cc = new CompatibleCanvas(this);
    cc->constructor();
    mCanvas = cc.Get();
}

Surface::~Surface()
{
    // try {
        if (mCloseGuard != NULL) {
            mCloseGuard->WarnIfOpen();
        }
        ReleaseResources();
    // } finally {
        // super.finalize();
    // }
}

ECode Surface::constructor()
{
    return NOERROR;
}

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
ECode Surface::constructor(
    /* [in] */ ISurfaceTexture* surfaceTexture)
{
    if (surfaceTexture == NULL) {
        SLOGGERE(TAG, "surfaceTexture must not be null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("surfaceTexture must not be null");
    }

    {
        AutoLock lock(this);
        mName = Object::ToString(surfaceTexture);
        Int64 ptr;
        NativeCreateFromSurfaceTexture(surfaceTexture, &ptr);
        SetNativeObjectLocked(ptr);
    }
    return NOERROR;
}

/* called from android_view_Surface_createFromIGraphicBufferProducer() */
ECode Surface::constructor(
    /* [in] */ Int64 nativeObject)
{
    AutoLock lock(this);
    SetNativeObjectLocked(nativeObject);
    return NOERROR;
}

/**
 * Set dirty region passed from HW renderer.
 * @hide
 */
ECode Surface::SetDirtyRect(
    /* [in] */ IRect* dirty)
 {
    if (mNativeObject != 0) {
        FAIL_RETURN(NativeSetDirtyRect(dirty))
    }
    return NOERROR;
}


ECode Surface::ReleaseResources()
{
    AutoLock lock(this);
    if (mNativeObject != 0) {
        NativeRelease(mNativeObject);
        SetNativeObjectLocked(0);
    }
    return NOERROR;
}

ECode Surface::Destroy()
{
    ReleaseResources();
    return NOERROR;
}

/**
 * Copy another surface to this one.  This surface now holds a reference
 * to the same data as the original surface, and is -not- the owner.
 * This is for use by the window manager when returning a window surface
 * back from a client, converting it from the representation being managed
 * by the window manager to the representation the client uses to draw
 * in to it.
 * @hide
 */
ECode Surface::CopyFrom(
    /* [in] */ ISurfaceControl* other)
{
    if (other == NULL) {
        // throw new IllegalArgumentException("other must not be null");
        SLOGGERD(TAG, "other must not be null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    SurfaceControl* otherImp = (SurfaceControl*)other;
    Int64 surfaceControlPtr  = otherImp->mNativeObject;
    if (surfaceControlPtr == 0) {
        // throw new NullPointerException(
        //         "SurfaceControl native object is null. Are you using a released SurfaceControl?");
        SLOGGERD(TAG, "SurfaceControl native object is null. Are you using a released SurfaceControl?")
        return E_NULL_POINTER_EXCEPTION;

    }
    Int64 newNativeObject = NativeCreateFromSurfaceControl(surfaceControlPtr);

    {
        AutoLock lock(this);
        if (mNativeObject != 0) {
            NativeRelease(mNativeObject);
        }
        SetNativeObjectLocked(newNativeObject);
    }
    return NOERROR;
}

ECode Surface::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    AutoLock lock(this);
    StringBuilder sb("Surface(name=");
    sb += mName;
    sb += ")/@0x";
    sb += StringUtils::ToHexString((Int32)this);
    *str = sb.ToString();
    return NOERROR;
}

/**
 * Allocate buffers ahead of time to avoid allocation delays during rendering
 * @hide
 */
ECode Surface::AllocateBuffers()
{
    AutoLock lock(this);
    FAIL_RETURN(CheckNotReleasedLocked())
    NativeAllocateBuffers();
    return NOERROR;
}

ECode Surface::IsValid(
    /* [out] */ Boolean* valid)
{
    VALIDATE_NOT_NULL(valid);

    AutoLock lock(this);
    if (mNativeObject == 0) {
        *valid = FALSE;
        return NOERROR;
    }

    *valid = NativeIsValid();
    return NOERROR;
}

ECode Surface::GetGenerationId(
    /* [out] */ Int32* generationId)
{
    VALIDATE_NOT_NULL(generationId);

    AutoLock lock(this);
    *generationId = mGenerationId;
    return NOERROR;
}

ECode Surface::IsConsumerRunningBehind(
    /* [out] */ Boolean* consumer)
{
    VALIDATE_NOT_NULL(consumer);

    AutoLock lock(this);
    FAIL_RETURN(CheckNotReleasedLocked())
    return NativeIsConsumerRunningBehind(consumer);
}

ECode Surface::LockCanvas(
    /* [in] */ IRect* dirty,
    /* [out] */ ICanvas** canvas)
{
    VALIDATE_NOT_NULL(canvas);
    *canvas = NULL;

    AutoLock lock(this);
    FAIL_RETURN(CheckNotReleasedLocked())
    if (mLockedObject != 0) {
        // Ideally, nativeLockCanvas() would throw in this situation and prevent the
        // double-lock, but that won't happen if mNativeObject was updated.  We can't
        // abandon the old mLockedObject because it might still be in use, so instead
        // we just refuse to re-lock the Surface.
        SLOGGERE(TAG, "Surface was already locked")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(NativeLockCanvas(mCanvas, dirty, &mLockedObject))
    *canvas = mCanvas;
    REFCOUNT_ADD(*canvas)

    return NOERROR;
}

ECode Surface::UnlockCanvasAndPost(
    /* [in] */ ICanvas* canvas)
{
    if (canvas != mCanvas) {
        // throw new IllegalArgumentException("canvas object must be the same instance that "
        //         + "was previously returned by lockCanvas");
        SLOGGERE(TAG, String("canvas object must be the same instance that ")
                + "was previously returned by lockCanvas")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    {
        AutoLock lock(this);
        FAIL_RETURN(CheckNotReleasedLocked())
        if (mNativeObject != mLockedObject) {
            SLOGGERE(TAG, String("WARNING: Surface's mNativeObject (0x") +
                    StringUtils::ToHexString(mNativeObject) + ") != mLockedObject (0x" +
                    StringUtils::ToHexString(mLockedObject) +")");
        }
        if (mLockedObject == 0) {
            SLOGGERE(TAG, "Surface was not locked")
            return E_ILLEGAL_STATE_EXCEPTION;
            // throw new IllegalStateException("Surface was not locked");
        }
        // try {
            ECode ec= NativeUnlockCanvasAndPost(canvas);
        // } finally {
            NativeRelease(mLockedObject);
            mLockedObject = 0;
            return ec;
        // }
    }
}

ECode Surface::UnlockCanvas(
    /* [in] */ ICanvas* canvas)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Surface::SetCompatibilityTranslator(
    /* [in] */ ICompatibilityInfoTranslator* translator)
{
    if (translator != NULL) {
        Float appScale;
        translator->GetApplicationScale(&appScale);
        mCompatibleMatrix = NULL;
        CMatrix::New((IMatrix**)&mCompatibleMatrix);
        assert(mCompatibleMatrix != NULL);

        mCompatibleMatrix->SetScale(appScale, appScale);
    }
    return NOERROR;
}

ECode Surface::TransferFrom(
    /* [in] */ ISurface* other)
{
    if (other == NULL) {
        // assert(0);
        SLOGGERE(TAG, "other must not be null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        //throw new IllegalArgumentException("other must not be null");
    }
    if (other != this) {
        Int64 newPtr;
        {
            Surface* otherImp = (Surface*)other;
            AutoLock lock(otherImp);
            newPtr = otherImp->mNativeObject;
            otherImp->SetNativeObjectLocked(0);
        }

        {
            AutoLock lock(this);
            if (mNativeObject != 0) {
                NativeRelease(mNativeObject);
            }
            SetNativeObjectLocked(newPtr);
        }
    }

    return NOERROR;
}

ECode Surface::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    if (source == NULL) {
        SLOGGERE(TAG, "source must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    source->ReadString(&mName);
    Int64 ptr;
    NativeReadFromParcel(source, &ptr);
    SetNativeObjectLocked(ptr);
    return NOERROR;
}

ECode Surface::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    if (dest == NULL) {
        SLOGGERE(TAG, "dest must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    {
        AutoLock lock(this);
        dest->WriteString(mName);
        NativeWriteToParcel(dest);
    }

    // if ((flags & Parcelable.PARCELABLE_WRITE_RETURN_VALUE) != 0) {
        ReleaseResources();
    // }
    return NOERROR;
}

ECode Surface::GetNativeSurface(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(this);
    *result = mNativeObject;
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
