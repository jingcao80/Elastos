
#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHADAPTERMANAGERCALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHADAPTERMANAGERCALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothAdapterManagerCallback.h"
#include "CBluetoothAdapter.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothAdapterManagerCallback)
{
public:
    CARAPI OnBluetoothServiceUp(
        /* [in] */ IIBluetooth* bluetoothService);

    CARAPI OnBluetoothServiceDown();

    CARAPI constructor(
        /* [in] */ IBluetoothAdapter* host);

private:
    CBluetoothAdapter* mHost;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_BLUETOOTH_CBLUETOOTHADAPTERMANAGERCALLBACK_H__
