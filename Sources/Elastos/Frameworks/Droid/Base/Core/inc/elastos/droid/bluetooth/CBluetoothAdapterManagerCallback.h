
#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHADAPTERMANAGERCALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHADAPTERMANAGERCALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothAdapterManagerCallback.h"
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/bluetooth/CBluetoothAdapter.h"

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothAdapterManagerCallback)
    , public Object
    , public IIBluetoothManagerCallback
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBluetoothAdapterManagerCallback();

    CARAPI OnBluetoothServiceUp(
        /* [in] */ IIBluetooth* bluetoothService);

    CARAPI OnBluetoothServiceDown();

    CARAPI constructor(
        /* [in] */ IBluetoothAdapter* host);

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info)
        return Object::ToString(info);
    }
private:
    CBluetoothAdapter* mHost;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_BLUETOOTH_CBLUETOOTHADAPTERMANAGERCALLBACK_H__
