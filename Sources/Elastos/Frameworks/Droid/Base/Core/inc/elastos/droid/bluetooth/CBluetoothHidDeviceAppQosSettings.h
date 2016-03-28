#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHIDDEVICEAPPQOSSETTINGS_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHIDDEVICEAPPQOSSETTINGS_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothHidDeviceAppQosSettings.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothHidDeviceAppQosSettings)
    , public Object
    , public IParcelable
    , public IBluetoothHidDeviceAppQosSettings
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CBluetoothHidDeviceAppQosSettings();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 serviceType,
        /* [in] */ Int32 tokenRate,
        /* [in] */ Int32 tokenBucketSize,
        /* [in] */ Int32 peakBandwidth,
        /* [in] */ Int32 latency,
        /* [in] */ Int32 delayVariation);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI ToArray(
        /* [out, callee] */ ArrayOf<Int32>** array);

private:
    Int32 mServiceType;
    Int32 mTokenRate;
    Int32 mTokenBucketSize;
    Int32 mPeakBandwidth;
    Int32 mLatency;
    Int32 mDelayVariation;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
#endif