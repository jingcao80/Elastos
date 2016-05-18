
#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHPBAPSTATECHANGECALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHPBAPSTATECHANGECALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothPbapStateChangeCallback.h"
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/bluetooth/BluetoothPbap.h"

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothPbapStateChangeCallback)
    , public Object
    , public IIBluetoothStateChangeCallback
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBluetoothPbapStateChangeCallback();

    CARAPI OnBluetoothStateChange(
        /* [in] */ Boolean on);

    CARAPI constructor(
        /* [in] */ IBluetoothPbap* host);

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info)
        return Object::ToString(info);
    }
private:
    BluetoothPbap* mHost;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_BLUETOOTH_CBLUETOOTHPBAPSTATECHANGECALLBACK_H__
