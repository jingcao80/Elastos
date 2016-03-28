#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHSAPSTATECHANGECALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHSAPSTATECHANGECALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothSapStateChangeCallback.h"
#include "elastos/droid/bluetooth/BluetoothSap.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothSapStateChangeCallback)
    , public Object
    , public IIBluetoothStateChangeCallback
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBluetoothSapStateChangeCallback();

    CARAPI OnBluetoothStateChange(
        /* [in] */ Boolean on);

    CARAPI constructor(
        /* [in] */ IBluetoothSap* host);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    BluetoothSap* mHost;
};
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif