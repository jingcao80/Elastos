
#ifndef __ELASTOS_DROID_LOCATION_CRITERIA_H__
#define __ELASTOS_DROID_LOCATION_CRITERIA_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Location.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Location {

class Criteria
    : public Object
    , public ICriteria
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    Criteria();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICriteria* criteria);

    /**
     * Indicates the desired horizontal accuracy (latitude and longitude).
     * Accuracy may be {@link #ACCURACY_LOW}, {@link #ACCURACY_MEDIUM},
     * {@link #ACCURACY_HIGH} or {@link #NO_REQUIREMENT}.
     * More accurate location may consume more power and may take longer.
     *
     * @throws IllegalArgumentException if accuracy is not one of the supported constants
     */
    CARAPI SetHorizontalAccuracy(
        /* [in] */ Int32 accuracy);

    /**
     * Returns a constant indicating the desired horizontal accuracy (latitude and longitude).
     * Accuracy may be {@link #ACCURACY_LOW}, {@link #ACCURACY_MEDIUM},
     * {@link #ACCURACY_HIGH} or {@link #NO_REQUIREMENT}.
     */
    CARAPI GetHorizontalAccuracy(
        /* [out] */ Int32* accuracy);

    /**
     * Indicates the desired vertical accuracy (altitude).
     * Accuracy may be {@link #ACCURACY_LOW}, {@link #ACCURACY_MEDIUM},
     * {@link #ACCURACY_HIGH} or {@link #NO_REQUIREMENT}.
     * More accurate location may consume more power and may take longer.
     *
     * @throws IllegalArgumentException if accuracy is not one of the supported constants
     */
    CARAPI SetVerticalAccuracy(
        /* [in] */ Int32 accuracy);

    /**
     * Returns a constant indicating the desired vertical accuracy (altitude).
     * Accuracy may be {@link #ACCURACY_LOW}, {@link #ACCURACY_HIGH},
     * or {@link #NO_REQUIREMENT}.
     */
    CARAPI GetVerticalAccuracy(
        /* [out] */ Int32* accuracy);

    CARAPI SetSpeedAccuracy(
        /* [in] */ Int32 accuracy);

    CARAPI GetSpeedAccuracy(
        /* [out] */ Int32* accuracy);

    CARAPI SetBearingAccuracy(
        /* [in] */ Int32 accuracy);

    CARAPI GetBearingAccuracy(
        /* [out] */ Int32* accuracy);

    CARAPI SetAccuracy(
        /* [in] */ Int32 accuracy);

    CARAPI GetAccuracy(
        /* [out] */ Int32* accuracy);

    CARAPI SetPowerRequirement(
        /* [in] */ Int32 level);

    CARAPI GetPowerRequirement(
        /* [out] */ Int32* requirement);

    /**
     * Indicates whether the provider is allowed to incur monetary cost.
     */
    CARAPI SetCostAllowed(
        /* [in] */ Boolean costAllowed);

    /**
     * Returns whether the provider is allowed to incur monetary cost.
     */
    CARAPI IsCostAllowed(
        /* [out] */ Boolean* costAllowed);

    CARAPI SetAltitudeRequired(
        /* [in] */ Boolean altitudeRequired);

    /**
     * Returns whether the provider must provide altitude information.
     * Not all fixes are guaranteed to contain such information.
     */
    CARAPI IsAltitudeRequired(
        /* [out] */ Boolean* altitudeRequired);

    CARAPI SetSpeedRequired(
        /* [in] */ Boolean speedRequired);

    /**
     * Returns whether the provider must provide speed information.
     * Not all fixes are guaranteed to contain such information.
     */
    CARAPI IsSpeedRequired(
        /* [out] */ Boolean* speedRequired);

    CARAPI SetBearingRequired(
        /* [in] */ Boolean bearingRequired);

    /**
     * Returns whether the provider must provide bearing information.
     * Not all fixes are guaranteed to contain such information.
     */
    CARAPI IsBearingRequired(
        /* [out] */ Boolean* bearingRequired);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    // @Override
    CARAPI ToString(
        /* [out] */ String* info);

private:
    static CARAPI PowerToString(
        /* [in] */ Int32 power,
        /* [out] */ String* strOut);

    static CARAPI AccuracyToString(
        /* [in] */ Int32 accuracy,
        /* [out] */ String* strOut);

private:
    Int32 mHorizontalAccuracy;
    Int32 mVerticalAccuracy;
    Int32 mSpeedAccuracy;
    Int32 mBearingAccuracy;
    Int32 mPowerRequirement;
    Boolean mAltitudeRequired;
    Boolean mBearingRequired;
    Boolean mSpeedRequired;
    Boolean mCostAllowed;
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_CRITERIA_H__
