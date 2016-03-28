#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHIDDEVICEAPPCONFIGURATION_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHIDDEVICEAPPCONFIGURATION_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothHidDeviceAppConfiguration.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothHidDeviceAppConfiguration)
    , public Object
    , public IBluetoothHidDeviceAppConfiguration
    , public IParcelable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CBluetoothHidDeviceAppConfiguration();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int64 hash);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

private:
    Int64 mHash;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
#endif