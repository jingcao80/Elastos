
#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHDEVICEMANAGERCALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHDEVICEMANAGERCALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothDeviceManagerCallback.h"
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothDeviceManagerCallback)
    , public Object
    , public IIBluetoothManagerCallback
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBluetoothDeviceManagerCallback();

    CARAPI OnBluetoothServiceUp(
        /* [in] */ IIBluetooth* bluetoothService);

    CARAPI OnBluetoothServiceDown();

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info)
        return Object::ToString(info);
    }
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_BLUETOOTH_CBLUETOOTHDEVICEMANAGERCALLBACK_H__
