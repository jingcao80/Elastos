
#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHPANSTATECHANGECALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHPANSTATECHANGECALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothPanStateChangeCallback.h"
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/bluetooth/BluetoothPan.h"

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothPanStateChangeCallback)
    , public Object
    , public IIBluetoothStateChangeCallback
    , public IBinder
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CBluetoothPanStateChangeCallback();

    CARAPI OnBluetoothStateChange(
        /* [in] */ Boolean on);

    CARAPI constructor(
        /* [in] */ IInterface* host);

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info)
        return Object::ToString(info);
    }
private:
    BluetoothPan* mHost;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_BLUETOOTH_CBLUETOOTHPANSTATECHANGECALLBACK_H__
