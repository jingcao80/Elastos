
#include "CBluetoothDeviceManagerCallback.h"
#include "CBluetoothDevice.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

ECode CBluetoothDeviceManagerCallback::OnBluetoothServiceUp(
    /* [in] */ IIBluetooth* bluetoothService)
{
    AutoLock lock(CBluetoothDevice::sLock);
    CBluetoothDevice::sService = bluetoothService;
    return NOERROR;
}

ECode CBluetoothDeviceManagerCallback::OnBluetoothServiceDown()
{
    AutoLock lock(CBluetoothDevice::sLock);
    CBluetoothDevice::sService = NULL;
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
