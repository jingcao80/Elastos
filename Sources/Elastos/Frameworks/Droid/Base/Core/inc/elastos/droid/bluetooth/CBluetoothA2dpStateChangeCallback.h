
#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHA2DOSTATECHANGECALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHA2DOSTATECHANGECALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothA2dpStateChangeCallback.h"
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/bluetooth/BluetoothA2dp.h"

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothA2dpStateChangeCallback)
    , public Object
    , public IIBluetoothStateChangeCallback
    , public IBinder
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CBluetoothA2dpStateChangeCallback();

    CARAPI constructor(
        /* [in] */ IInterface* host);

    CARAPI OnBluetoothStateChange(
        /* [in] */ Boolean on);

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info)
        return Object::ToString(info);
    }
private:
    BluetoothA2dp* mHost;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_BLUETOOTH_CBLUETOOTHA2DOSTATECHANGECALLBACK_H__
