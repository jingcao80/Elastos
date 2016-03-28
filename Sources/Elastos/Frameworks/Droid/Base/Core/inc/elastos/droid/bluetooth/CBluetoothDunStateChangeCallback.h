#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHDUNSTATECHANGECALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHDUNSTATECHANGECALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothDunStateChangeCallback.h"
#include "elastos/droid/bluetooth/BluetoothDun.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothDunStateChangeCallback)
    , public Object
    , public IIBluetoothStateChangeCallback
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBluetoothDunStateChangeCallback();

    CARAPI OnBluetoothStateChange(
        /* [in] */ Boolean on);

    CARAPI constructor(
        /* [in] */ IBluetoothDun* host);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    BluetoothDun* mHost;
};
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif