
#include "elastos/droid/bluetooth/CBluetoothDeviceManagerCallback.h"
#include "elastos/droid/bluetooth/CBluetoothDevice.h"
#include "elastos/core/AutoLock.h"

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CAR_INTERFACE_IMPL_2(CBluetoothDeviceManagerCallback, Object, IIBluetoothManagerCallback, IBinder);

CAR_OBJECT_IMPL(CBluetoothDeviceManagerCallback);

CBluetoothDeviceManagerCallback::CBluetoothDeviceManagerCallback()
{
}

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
