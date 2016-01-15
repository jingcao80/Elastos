
#ifndef __ELASTOS_DROID_LOCATION_FUSEDBATCHOPTIONS_H__
#define __ELASTOS_DROID_LOCATION_FUSEDBATCHOPTIONS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Location.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Location {

/**
 * A data class representing a set of options to configure batching sessions.
 * @hide
 */
class FusedBatchOptions
    : public Object
    , public IFusedBatchOptions
    , public IParcelable
{
public:
    /**
     * Definition of enum flag sets needed by this class.
     * Such values need to be kept in sync with the ones in fused_location.h
     */
    class SourceTechnologies
    {
    public:
        static Int32 GNSS;
        static Int32 WIFI;
        static Int32 SENSORS;
        static Int32 CELL;
        static Int32 BLUETOOTH;
    };

    class BatchFlags
    {
    public:
        // follow the definitions to the letter in fused_location.h
        static Int32 WAKEUP_ON_FIFO_FULL;
        static Int32 CALLBACK_ON_LOCATION_FIX;
    };

public:
    CAR_INTERFACE_DECL()

    FusedBatchOptions();

    CARAPI constructor();

    CARAPI SetMaxPowerAllocationInMW(
        /* [in] */ Double value);

    CARAPI GetMaxPowerAllocationInMW(
        /* [out] */ Double* result);

    CARAPI SetPeriodInNS(
        /* [in] */ Int64 value);

    CARAPI GetPeriodInNS(
        /* [out] */ Int64* result);

    CARAPI SetSourceToUse(
        /* [in] */ Int32 source);

    CARAPI ResetSourceToUse(
        /* [in] */ Int32 source);

    CARAPI IsSourceToUseSet(
        /* [in] */ Int32 source,
        /* [out] */ Boolean* result);

    CARAPI GetSourcesToUse(
        /* [out] */ Int32* result);

    CARAPI SetFlag(
        /* [in] */ Int32 flag);

    CARAPI ResetFlag(
        /* [in] */ Int32 flag);

    CARAPI IsFlagSet(
        /* [in] */ Int32 flag,
        /* [out] */ Boolean* result);

    CARAPI GetFlags(
        /* [out] */ Int32* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    Int64 mPeriodInNS;
    Int32 mSourcesToUse;
    Int32 mFlags;

    // the default value is set to request fixes at no cost
    Double mMaxPowerAllocationInMW;
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_FUSEDBATCHOPTIONS_H__
