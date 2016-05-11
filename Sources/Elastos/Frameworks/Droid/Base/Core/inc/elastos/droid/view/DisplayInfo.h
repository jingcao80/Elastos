
#ifndef __ELASTOS_DROID_VIEW_DISPLAYINFO_H__
#define __ELASTOS_DROID_VIEW_DISPLAYINFO_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::Res::ICompatibilityInfo;

namespace Elastos {
namespace Droid {
namespace View {

/**
 * Describes the characteristics of a particular logical display.
 * @hide
 */
class DisplayInfo
    : public Object
    , public IDisplayInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    DisplayInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IDisplayInfo* other);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetLayerStack(
        /* [out] */ Int32* layerStack);

    CARAPI SetLayerStack(
        /* [in] */ Int32 layerStack);

    CARAPI GetFlags(
        /* [out] */ Int32* flags);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    CARAPI GetType(
        /* [out] */ Int32* type);

    CARAPI SetType(
        /* [in] */ Int32 type);

    CARAPI GetAddress(
        /* [out] */ String* address);

    CARAPI SetAddress(
        /* [in] */ const String& address);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI SetName(
        /* [in] */ const String& name);

    CARAPI GetAppWidth(
        /* [out] */ Int32* appWidth);

    CARAPI SetAppWidth(
        /* [in] */ Int32 appWidth);

    CARAPI GetAppHeight(
        /* [out] */ Int32* appHeight);

    CARAPI SetAppHeight(
        /* [in] */ Int32 appHeight);

    CARAPI GetSmallestNominalAppWidth(
        /* [out] */ Int32* smallestNominalAppWidth);

    CARAPI SetSmallestNominalAppWidth(
        /* [in] */ Int32 smallestNominalAppWidth);

    CARAPI GetSmallestNominalAppHeight(
        /* [out] */ Int32* smallestNominalAppHeight);

    CARAPI SetSmallestNominalAppHeight(
        /* [in] */ Int32 smallestNominalAppHeight);

    CARAPI GetLargestNominalAppWidth(
        /* [out] */ Int32* largestNominalAppWidth);

    CARAPI SetLargestNominalAppWidth(
        /* [in] */ Int32 largestNominalAppWidth);

    CARAPI GetLargestNominalAppHeight(
        /* [out] */ Int32* largestNominalAppHeight);

    CARAPI SetLargestNominalAppHeight(
        /* [in] */ Int32 largestNominalAppHeight);

    CARAPI GetLogicalWidth(
        /* [out] */ Int32* logicalWidth);

    CARAPI SetLogicalWidth(
        /* [in] */ Int32 logicalWidth);

    CARAPI GetLogicalHeight(
        /* [out] */ Int32* logicalHeight);

    CARAPI SetLogicalHeight(
        /* [in] */ Int32 logicalHeight);

    CARAPI GetOverscanLeft(
        /* [out] */ Int32* left);

    CARAPI GetOverscanTop(
        /* [out] */ Int32* top);

    CARAPI GetOverscanRight(
        /* [out] */ Int32* right);

    CARAPI GetOverscanBottom(
        /* [out] */ Int32* bottom);

    CARAPI SetOverscanLeft(
        /* [in] */ Int32 left);

    CARAPI SetOverscanTop(
        /* [in] */ Int32 top);

    CARAPI SetOverscanRight(
        /* [in] */ Int32 right);

    CARAPI SetOverscanBottom(
        /* [in] */ Int32 bottom);

    CARAPI GetRotation(
        /* [out] */ Int32* rotation);

    CARAPI SetRotation(
        /* [in] */ Int32 rotation);

    CARAPI GetRefreshRate(
        /* [out] */ Float* refreshRate);

    CARAPI SetRefreshRate(
        /* [in] */ Float refreshRate);

    CARAPI GetSupportedRefreshRates(
        /* [out, callee] */ ArrayOf<Float>** rates);

    CARAPI SetSupportedRefreshRates(
        /* [in] */ ArrayOf<Float>* rates);

    CARAPI GetLogicalDensityDpi(
        /* [out] */ Int32* logicalDensityDpi);

    CARAPI SetLogicalDensityDpi(
        /* [in] */ Int32 logicalDensityDpi);

    CARAPI GetPhysicalXDpi(
        /* [out] */ Float* physicalXDpi);

    CARAPI SetPhysicalXDpi(
        /* [in] */ Float physicalXDpi);

    CARAPI GetPhysicalYDpi(
        /* [out] */ Float* physicalYDpi);

    CARAPI SetPhysicalYDpi(
        /* [in] */ Float physicalYDpi);

    CARAPI GetAppVsyncOffsetNanos(
        /* [out] */ Int64* nanos);

    CARAPI SetAppVsyncOffsetNanos(
        /* [in] */ Int64 nanos);

    CARAPI GetPresentationDeadlineNanos(
        /* [out] */ Int64* nanos);

    CARAPI SetPresentationDeadlineNanos(
        /* [in] */ Int64 nanos);

    CARAPI GetState(
        /* [out] */ Int32* state);

    CARAPI SetState(
        /* [in] */ Int32 state);

    CARAPI GetOwnerUid(
        /* [out] */ Int32* uid);

    CARAPI SetOwnerUid(
        /* [in] */ Int32 uid);

    CARAPI GetOwnerPackageName(
        /* [out] */ String* name);

    CARAPI SetOwnerPackageName(
        /* [in] */ const String& name);

    CARAPI Equals(
        /* [in] */ IDisplayInfo* otherObj,
        /* [out] */ Boolean* isEquals);

    CARAPI Equals(
        /* [in] */ IInterface* otherObj,
        /* [out] */ Boolean* isEquals);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI CopyFrom(
        /* [in] */ IDisplayInfo* otherObj);

    CARAPI GetAppMetrics(
        /* [in] */ IDisplayMetrics* outMetrics);

    CARAPI GetAppMetrics(
        /* [in] */ IDisplayMetrics* outMetrics,
        /* [in] */ IDisplayAdjustments* displayAdjustments);

    CARAPI GetAppMetrics(
        /* [in] */ IDisplayMetrics* outMetrics,
        /* [in] */ ICompatibilityInfo* ci,
        /* [in] */ IBinder* token);

    CARAPI GetLogicalMetrics(
        /* [in] */ IDisplayMetrics* outMetrics,
        /* [in] */ ICompatibilityInfo* ci,
        /* [in] */ IBinder* token);

    CARAPI GetNaturalWidth(
        /* [out] */ Int32* width);

    CARAPI GetNaturalHeight(
        /* [out] */ Int32* height);

    CARAPI HasAccess(
        /* [in] */ Int32 uid,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI GetMetricsWithSize(
        /* [in] */ IDisplayMetrics* outMetricsObj,
        /* [in] */ ICompatibilityInfo* cih,
        /* [in] */ IBinder* token,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    static String FlagsToString(
        /* [in] */ Int32 flags);

public:
    /**
     * The surface flinger layer stack associated with this logical display.
     */
    Int32 mLayerStack;

    /**
     * Display flags.
     */
    Int32 mFlags;

    /**
     * Display type.
     */
    Int32 mType;

    /**
     * Display address, or NULL if none.
     * Interpretation varies by display type.
     */
    String mAddress;

    /**
     * The human-readable name of the display.
     */
    String mName;

    /**
     * The width of the portion of the display that is available to applications, in pixels.
     * Represents the size of the display minus any system decorations.
     */
    Int32 mAppWidth;

    /**
     * The height of the portion of the display that is available to applications, in pixels.
     * Represents the size of the display minus any system decorations.
     */
    Int32 mAppHeight;

    /**
     * The smallest value of {@link #appWidth} that an application is likely to encounter,
     * in pixels, excepting cases where the width may be even smaller due to the presence
     * of a soft keyboard, for example.
     */
    Int32 mSmallestNominalAppWidth;

    /**
     * The smallest value of {@link #appHeight} that an application is likely to encounter,
     * in pixels, excepting cases where the height may be even smaller due to the presence
     * of a soft keyboard, for example.
     */
    Int32 mSmallestNominalAppHeight;

    /**
     * The largest value of {@link #appWidth} that an application is likely to encounter,
     * in pixels, excepting cases where the width may be even larger due to system decorations
     * such as the status bar being hidden, for example.
     */
    Int32 mLargestNominalAppWidth;

    /**
     * The largest value of {@link #appHeight} that an application is likely to encounter,
     * in pixels, excepting cases where the height may be even larger due to system decorations
     * such as the status bar being hidden, for example.
     */
    Int32 mLargestNominalAppHeight;

    /**
     * The logical width of the display, in pixels.
     * Represents the usable size of the display which may be smaller than the
     * physical size when the system is emulating a smaller display.
     */
    Int32 mLogicalWidth;

    /**
     * The logical height of the display, in pixels.
     * Represents the usable size of the display which may be smaller than the
     * physical size when the system is emulating a smaller display.
     */
    Int32 mLogicalHeight;

    /**
     * @hide
     * Number of overscan pixels on the left side of the display.
     */
    Int32 mOverscanLeft;

    /**
     * @hide
     * Number of overscan pixels on the top side of the display.
     */
    Int32 mOverscanTop;

    /**
     * @hide
     * Number of overscan pixels on the right side of the display.
     */
    Int32 mOverscanRight;

    /**
     * @hide
     * Number of overscan pixels on the bottom side of the display.
     */
    Int32 mOverscanBottom;

    /**
     * The rotation of the display relative to its natural orientation.
     * May be one of {@link android.view.Surface#ROTATION_0},
     * {@link android.view.Surface#ROTATION_90}, {@link android.view.Surface#ROTATION_180},
     * {@link android.view.Surface#ROTATION_270}.
     * <p>
     * The value of this field is indeterminate if the logical display is presented on
     * more than one physical display.
     * </p>
     */
    Int32 mRotation;

    /**
     * The refresh rate of this display in frames per second.
     * <p>
     * The value of this field is indeterminate if the logical display is presented on
     * more than one physical display.
     * </p>
     */
    Float mRefreshRate;

    AutoPtr<ArrayOf<Float> > mSupportedRefreshRates;

    /**
     * The logical display density which is the basis for density-independent
     * pixels.
     */
    Int32 mLogicalDensityDpi;

    /**
     * The exact physical pixels per inch of the screen in the X dimension.
     * <p>
     * The value of this field is indeterminate if the logical display is presented on
     * more than one physical display.
     * </p>
     */
    Float mPhysicalXDpi;

    /**
     * The exact physical pixels per inch of the screen in the Y dimension.
     * <p>
     * The value of this field is indeterminate if the logical display is presented on
     * more than one physical display.
     * </p>
     */
    Float mPhysicalYDpi;

    /**
     * This is a positive value indicating the phase offset of the VSYNC events provided by
     * Choreographer relative to the display refresh.  For example, if Choreographer reports
     * that the refresh occurred at time N, it actually occurred at (N - appVsyncOffsetNanos).
     */
    Int64 mAppVsyncOffsetNanos;

    /**
     * This is how far in advance a buffer must be queued for presentation at
     * a given time.  If you want a buffer to appear on the screen at
     * time N, you must submit the buffer before (N - bufferDeadlineNanos).
     */
    Int64 mPresentationDeadlineNanos;

    /**
     * The state of the display, such as {@link android.view.Display#STATE_ON}.
     */
    Int32 mState;

    /**
     * The UID of the application that owns this display, or zero if it is owned by the system.
     * <p>
     * If the display is private, then only the owner can use it.
     * </p>
     */
    Int32 mOwnerUid;

    /**
     * The package name of the application that owns this display, or null if it is
     * owned by the system.
     * <p>
     * If the display is private, then only the owner can use it.
     * </p>
     */
    String mOwnerPackageName;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CDISPLAYINFO_H__
