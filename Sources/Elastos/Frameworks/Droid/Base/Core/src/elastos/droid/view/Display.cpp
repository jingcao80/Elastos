
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/Display.h"
#include "elastos/droid/view/DisplayAdjustments.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
#include "elastos/droid/content/res/CCompatibilityInfoHelper.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::Res::ICompatibilityInfoHelper;
using Elastos::Droid::Content::Res::CCompatibilityInfoHelper;
using Elastos::Droid::Os::IProcess;

namespace Elastos {
namespace Droid {
namespace View {

const char* Display::TAG = "Display";
const Boolean Display::DEBUG;
const Int32 Display::CACHED_APP_SIZE_DURATION_MILLIS;

CAR_INTERFACE_IMPL(Display, Object, IDisplay);

Display::Display()
    : mIsValid(TRUE)
    , mLastCachedAppSizeUpdate(0)
    , mCachedAppWidthCompat(0)
    , mCachedAppHeightCompat(0)
{}

ECode Display::constructor(
    /* [in] */ IDisplayManagerGlobal* global,
    /* [in] */ Int32 displayId,
    /* [in] */ IDisplayInfo* displayInfo /*not NULL*/,
    /* [in] */ IDisplayAdjustments* daj)
{
    assert(displayInfo);
    mGlobal = global;
    mDisplayId = displayId;
    mDisplayInfo = displayInfo;
    mDisplayAdjustments = new DisplayAdjustments(daj);
    // Cache properties that cannot change as Int64 as the display is valid.
    displayInfo->GetLayerStack(&mLayerStack);
    displayInfo->GetFlags(&mFlags);
    displayInfo->GetType(&mType);
    displayInfo->GetAddress(&mAddress);
    displayInfo->GetOwnerUid(&mOwnerUid);
    displayInfo->GetOwnerPackageName(&mOwnerPackageName);
    CDisplayMetrics::New((IDisplayMetrics**)&mTempMetrics);

    return NOERROR;
}

ECode Display::GetDisplayId(
    /* [out] */ Int32* displayId)
{
    VALIDATE_NOT_NULL(displayId);
    *displayId = mDisplayId;
    return NOERROR;
}

ECode Display::IsValid(
    /* [out] */ Boolean* isValid)
{
    VALIDATE_NOT_NULL(isValid);

    AutoLock lock(mSelfLock);
    UpdateDisplayInfoLocked();
    *isValid = mIsValid;

    return NOERROR;
}

ECode Display::GetDisplayInfo(
    /* [in] */ IDisplayInfo* outDisplayInfo,
    /* [out] */ Boolean* isValid)
{
    VALIDATE_NOT_NULL(isValid);

    AutoLock lock(mSelfLock);
    UpdateDisplayInfoLocked();
    outDisplayInfo->CopyFrom(mDisplayInfo);
    *isValid = mIsValid;

    return NOERROR;
}

ECode Display::GetLayerStack(
    /* [out] */ Int32* layerStack)
{
    VALIDATE_NOT_NULL(layerStack);
    *layerStack = mLayerStack;

    return NOERROR;
}

ECode Display::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags);
    *flags = mFlags;

    return NOERROR;
}

ECode Display::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mType;

    return NOERROR;
}

ECode Display::GetAddress(
    /* [out] */ String* address)
{
    VALIDATE_NOT_NULL(address);
    *address = mAddress;

    return NOERROR;
}


/**
 * Gets the UID of the application that owns this display, or zero if it is
 * owned by the system.
 * <p>
 * If the display is private, then only the owner can use it.
 * </p>
 *
 * @hide
 */
ECode Display::GetOwnerUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid)
    *uid = mOwnerUid;
    return NOERROR;
}

/**
 * Gets the package name of the application that owns this display, or null if it is
 * owned by the system.
 * <p>
 * If the display is private, then only the owner can use it.
 * </p>
 *
 * @hide
 */
ECode Display::GetOwnerPackageName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mOwnerPackageName;
    return NOERROR;
}

/**
 * Gets the compatibility info used by this display instance.
 *
 * @return The display adjustments holder, or null if none is required.
 * @hide
 */
ECode Display::GetDisplayAdjustments(
    /* [out] */ IDisplayAdjustments** daj)
{
    VALIDATE_NOT_NULL(daj)
    *daj = mDisplayAdjustments;
    REFCOUNT_ADD(*daj)
    return NOERROR;
}

ECode Display::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    AutoLock lock(mSelfLock);
    UpdateDisplayInfoLocked();
    return mDisplayInfo->GetName(name);
}

ECode Display::GetSize(
    /* [in] */ IPoint* outSize)
{
    AutoLock lock(mSelfLock);
    UpdateDisplayInfoLocked();
    mDisplayInfo->GetAppMetrics(mTempMetrics.Get(), mDisplayAdjustments);
    Int32 widthPixels, heightPixels;
    mTempMetrics->GetWidthPixels(&widthPixels);
    mTempMetrics->GetHeightPixels(&heightPixels);
    outSize->Set(widthPixels, heightPixels);

    return NOERROR;
}

ECode Display::GetRectSize(
    /* [in] */ IRect* outSize)
{
    AutoLock lock(mSelfLock);
    UpdateDisplayInfoLocked();
    mDisplayInfo->GetAppMetrics(mTempMetrics.Get(), mDisplayAdjustments);
    Int32 widthPixels, heightPixels;
    mTempMetrics->GetWidthPixels(&widthPixels);
    mTempMetrics->GetHeightPixels(&heightPixels);
    outSize->Set(0, 0, widthPixels, heightPixels);

    return NOERROR;
}

ECode Display::GetCurrentSizeRange(
    /* [in] */ IPoint* outSmallestSize,
    /* [in] */ IPoint* outLargestSize)
{
    AutoLock lock(mSelfLock);
    UpdateDisplayInfoLocked();
    Int32 x, y;
    mDisplayInfo->GetSmallestNominalAppWidth(&x);
    mDisplayInfo->GetSmallestNominalAppHeight(&y);
    outSmallestSize->Set(x, y);
    mDisplayInfo->GetLargestNominalAppWidth(&x);
    mDisplayInfo->GetLargestNominalAppHeight(&y);
    outLargestSize->Set(x, y);

    return NOERROR;
}

ECode Display::GetMaximumSizeDimension(
    /* [out] */ Int32* maximumSizeDimension)
{
    VALIDATE_NOT_NULL(maximumSizeDimension);

    AutoLock lock(mSelfLock);
    UpdateDisplayInfoLocked();
    Int32 logicalWidth, logicalHeight;
    mDisplayInfo->GetLogicalWidth(&logicalWidth);
    mDisplayInfo->GetLogicalHeight(&logicalHeight);
    *maximumSizeDimension =Elastos::Core::Math::Max(logicalWidth, logicalHeight);

    return NOERROR;
}

ECode Display::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    AutoLock lock(mSelfLock);
    UpdateCachedAppSizeIfNeededLocked();
    *width = mCachedAppWidthCompat;

    return NOERROR;
}

ECode Display::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    AutoLock lock(mSelfLock);
    UpdateCachedAppSizeIfNeededLocked();
    *height = mCachedAppHeightCompat;

    return NOERROR;
}

/**
 * @hide
 * Return a rectangle defining the insets of the overscan region of the display.
 * Each field of the rectangle is the number of pixels the overscan area extends
 * into the display on that side.
 */
ECode Display::GetOverscanInsets(
    /* [in] */ IRect* outRect)
{
    AutoLock lock(mSelfLock);
    UpdateDisplayInfoLocked();
    Int32 left, top, right, bottom;
    mDisplayInfo->GetOverscanLeft(&left);
    mDisplayInfo->GetOverscanTop(&top);
    mDisplayInfo->GetOverscanRight(&right);
    mDisplayInfo->GetOverscanBottom(&bottom);

    outRect->Set(left, top, right, bottom);
    return NOERROR;
}

ECode Display::GetRotation(
    /* [out] */ Int32* rotation)
{
    VALIDATE_NOT_NULL(rotation);
    AutoLock lock(mSelfLock);
    UpdateDisplayInfoLocked();
    return mDisplayInfo->GetRotation(rotation);
}

ECode Display::GetOrientation(
    /* [out] */ Int32* orientation)
{
    VALIDATE_NOT_NULL(orientation);
    return GetRotation(orientation);
}

ECode Display::GetPixelFormat(
    /* [out] */ Int32* pixelFormat)
{
    VALIDATE_NOT_NULL(pixelFormat);
    *pixelFormat = IPixelFormat::RGBA_8888;

    return NOERROR;
}

ECode Display::GetRefreshRate(
    /* [out] */ Float* refreshRate)
{
    VALIDATE_NOT_NULL(refreshRate);
    AutoLock lock(mSelfLock);
    UpdateDisplayInfoLocked();
    return mDisplayInfo->GetRefreshRate(refreshRate);
}

/**
 * Get the supported refresh rates of this display in frames per second.
 */
ECode Display::GetSupportedRefreshRates(
    /* [out, callee] */ ArrayOf<Float>** rates)
{
    VALIDATE_NOT_NULL(rates)
    AutoLock lock(mSelfLock);
    UpdateDisplayInfoLocked();
    AutoPtr<ArrayOf<Float> > refreshRates;
    mDisplayInfo->GetSupportedRefreshRates((ArrayOf<Float>**)&refreshRates);
    *rates = ArrayOf<Float>::Alloc(refreshRates->GetLength());
    (*rates)->Copy(refreshRates, refreshRates->GetLength());
    REFCOUNT_ADD(*rates);
    return NOERROR;
}

/**
 * Gets the app VSYNC offset, in nanoseconds.  This is a positive value indicating
 * the phase offset of the VSYNC events provided by Choreographer relative to the
 * display refresh.  For example, if Choreographer reports that the refresh occurred
 * at time N, it actua`lly occurred at (N - appVsyncOffset).
 * <p>
 * Apps generally do not need to be aware of this.  It's only useful for fine-grained
 * A/V synchronization.
 */
ECode Display::GetAppVsyncOffsetNanos(
    /* [out] */ Int64* nanos)
{
    VALIDATE_NOT_NULL(nanos)
    AutoLock lock(mSelfLock);
    UpdateDisplayInfoLocked();
    mDisplayInfo->GetAppVsyncOffsetNanos(nanos);
    return NOERROR;
}

ECode Display::GetPresentationDeadlineNanos(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(mSelfLock);
    UpdateDisplayInfoLocked();
    mDisplayInfo->GetPresentationDeadlineNanos(result);
    return NOERROR;
}

ECode Display::GetMetrics(
    /* [in] */ IDisplayMetrics* outMetrics)
{
    AutoLock lock(mSelfLock);
    UpdateDisplayInfoLocked();
    mDisplayInfo->GetAppMetrics(outMetrics, mDisplayAdjustments);

    return NOERROR;
}

ECode Display::GetRealSize(
    /* [in] */ IPoint* outSize)
{
    AutoLock lock(mSelfLock);
    UpdateDisplayInfoLocked();
    Int32 logicalWidth, logicalHeight;
    mDisplayInfo->GetLogicalWidth(&logicalWidth);
    mDisplayInfo->GetLogicalHeight(&logicalHeight);
    outSize->Set(logicalWidth, logicalHeight);

    return NOERROR;
}

ECode Display::GetRealMetrics(
    /* [in] */ IDisplayMetrics* outMetrics)
{
    AutoLock lock(mSelfLock);
    UpdateDisplayInfoLocked();
    AutoPtr<ICompatibilityInfoHelper> helper;
    CCompatibilityInfoHelper::AcquireSingleton((ICompatibilityInfoHelper**)&helper);
    AutoPtr<ICompatibilityInfo> info;
    helper->GetDefault((ICompatibilityInfo**)&info);
    AutoPtr<IBinder> token;
    mDisplayAdjustments->GetActivityToken((IBinder**)&token);
    return mDisplayInfo->GetLogicalMetrics(outMetrics, info, token);
}

/**
 * Gets the state of the display, such as whether it is on or off.
 *
 * @return The state of the display: one of {@link #STATE_OFF}, {@link #STATE_ON},
 * {@link #STATE_DOZE}, {@link #STATE_DOZE_SUSPEND}, or {@link #STATE_UNKNOWN}.
 */
ECode Display::GetState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state)
    AutoLock lock(mSelfLock);
    UpdateDisplayInfoLocked();
    *state = STATE_UNKNOWN;
    if(mIsValid){
        mDisplayInfo->GetState(state);
    }
    return NOERROR;
}

/**
 * Returns true if the specified UID has access to this display.
 * @hide
 */
ECode Display::HasAccess(
    /* [in] */ Int32 uid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = Display::HasAccess(uid, mFlags, mOwnerUid);
    return NOERROR;
}

/** @hide */
Boolean Display::HasAccess(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 ownerUid)
{
    return (flags & Display::FLAG_PRIVATE) == 0
            || uid == ownerUid
            || uid == IProcess::SYSTEM_UID
            || uid == 0;
}

/**
 * Returns true if the display is a public presentation display.
 * @hide
 */
ECode Display::IsPublicPresentation(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mFlags & (FLAG_PRIVATE | FLAG_PRESENTATION)) == FLAG_PRESENTATION;
    return NOERROR;
}

void Display::UpdateDisplayInfoLocked()
{
    // Note: The display manager caches display info objects on our behalf.
    AutoPtr<IDisplayInfo> newInfo;
    mGlobal->GetDisplayInfo(mDisplayId, (IDisplayInfo**)&newInfo);
    if (newInfo == NULL) {
        // Preserve the old mDisplayInfo after the display is removed.
        if (mIsValid) {
            mIsValid = FALSE;
            if (DEBUG) {
                Logger::D(TAG, "Logical display %d was removed.", mDisplayId);
            }
        }
    }
    else {
        // Use the new display info.  (It might be the same object if nothing changed.)
        mDisplayInfo = newInfo;
        if (!mIsValid) {
            mIsValid = TRUE;
            if (DEBUG) {
                Logger::D(TAG, "Logical display %d was recreated.", mDisplayId);
            }
        }
    }
}

void Display::UpdateCachedAppSizeIfNeededLocked()
{
    Int64 now = SystemClock::GetUptimeMillis();
    if (now > mLastCachedAppSizeUpdate + CACHED_APP_SIZE_DURATION_MILLIS) {
        UpdateDisplayInfoLocked();
        mDisplayInfo->GetAppMetrics(mTempMetrics.Get(), mDisplayAdjustments);
        mTempMetrics->GetWidthPixels(&mCachedAppWidthCompat);
        mTempMetrics->GetHeightPixels(&mCachedAppHeightCompat);
        mLastCachedAppSizeUpdate = now;
    }
}

// // For debugging purposes
// //@Override
// String ToString()
// {
//     AutoLock lock(mSelfLock);
//     UpdateDisplayInfoLocked();
//     mDisplayInfo->GetAppMetrics(mTempMetrics.Get(), mDisplayAdjustments);
//     return "Display id " + mDisplayId + ": " + mDisplayInfo
//             + ", " + mTempMetrics + ", isValid=" + mIsValid;
// }

String Display::TypeToString(
    /* [in] */ Int32 type)
{
    switch (type) {
        case TYPE_UNKNOWN:
            return String("UNKNOWN");
        case TYPE_BUILT_IN:
            return String("BUILT_IN");
        case TYPE_HDMI:
            return String("HDMI");
        case TYPE_WIFI:
            return String("WIFI");
        case TYPE_OVERLAY:
            return String("OVERLAY");
        case TYPE_VIRTUAL:
            return String("TYPE_VIRTUAL");
        default:
            return StringUtils::ToString(type);
    }
}

String Display::StateToString(
    /* [in] */ Int32 state)
{
    switch (state) {
        case STATE_UNKNOWN:
            return String("UNKNOWN");
        case STATE_OFF:
            return String("OFF");
        case STATE_ON:
            return String("ON");
        case STATE_DOZE:
            return String("DOZE");
        case STATE_DOZE_SUSPEND:
            return String("DOZE_SUSPEND");
        default:
            return StringUtils::ToString(state);
    }
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
