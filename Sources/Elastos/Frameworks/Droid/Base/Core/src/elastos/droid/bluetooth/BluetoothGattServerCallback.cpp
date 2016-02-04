
#include "elastos/droid/bluetooth/BluetoothGattServerCallback.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//                     BluetoothGattServerCallback
//=====================================================================
ECode BluetoothGattServerCallback::OnConnectionStateChange(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 status,
    /* [in] */ Int32 newState)
{
    return NOERROR;
}

ECode BluetoothGattServerCallback::OnServiceAdded(
    /* [in] */ Int32 status,
    /* [in] */ IBluetoothGattService* service)
{
    return NOERROR;
}

ECode BluetoothGattServerCallback::OnCharacteristicReadRequest(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 requestId,
    /* [in] */ Int32 offset,
    /* [in] */ IBluetoothGattCharacteristic* characteristic)
{
    return NOERROR;
}

ECode BluetoothGattServerCallback::OnCharacteristicWriteRequest(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 requestId,
    /* [in] */ IBluetoothGattCharacteristic* characteristic,
    /* [in] */ Boolean preparedWrite,
    /* [in] */ Boolean responseNeeded,
    /* [in] */ Int32 offset,
    /* [in] */ ArrayOf<Byte>* value)
{
    return NOERROR;
}

ECode BluetoothGattServerCallback::OnDescriptorReadRequest(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 requestId,
    /* [in] */ Int32 offset,
    /* [in] */ IBluetoothGattDescriptor* descriptor)
{
    return NOERROR;
}

ECode BluetoothGattServerCallback::OnDescriptorWriteRequest(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 requestId,
    /* [in] */ IBluetoothGattDescriptor* descriptor,
    /* [in] */ Boolean preparedWrite,
    /* [in] */ Boolean responseNeeded,
    /* [in] */ Int32 offset,
    /* [in] */ ArrayOf<Byte>* value)
{
    return NOERROR;
}

ECode BluetoothGattServerCallback::OnExecuteWrite(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 requestId,
    /* [in] */ Boolean execute)
{
    return NOERROR;
}

ECode BluetoothGattServerCallback::OnNotificationSent(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 status)
{
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
