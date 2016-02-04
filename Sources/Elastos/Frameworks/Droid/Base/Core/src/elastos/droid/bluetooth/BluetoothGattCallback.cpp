
#include "elastos/droid/bluetooth/BluetoothGattCallback.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//                        BluetoothGattCallback
//=====================================================================
CAR_INTERFACE_IMPL(BluetoothGattCallback, Object, IBluetoothGattCallback);

BluetoothGattCallback::BluetoothGattCallback()
{
}

ECode BluetoothGattCallback::OnConnectionStateChange(
    /* [in] */ IBluetoothGatt* gatt,
    /* [in] */ Int32 status,
    /* [in] */ Int32 newState)
{
    VALIDATE_NOT_NULL(gatt);
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCallback::OnServicesDiscovered(
    /* [in] */ IBluetoothGatt* gatt,
    /* [in] */ Int32 status)
{
    VALIDATE_NOT_NULL(gatt);
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCallback::OnCharacteristicRead(
    /* [in] */ IBluetoothGatt* gatt,
    /* [in] */ IBluetoothGattCharacteristic* characteristic,
    /* [in] */ Int32 status)
{
    VALIDATE_NOT_NULL(gatt);
    VALIDATE_NOT_NULL(characteristic);
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCallback::OnCharacteristicWrite(
    /* [in] */ IBluetoothGatt* gatt,
    /* [in] */ IBluetoothGattCharacteristic* characteristic,
    /* [in] */ Int32 status)
{
    VALIDATE_NOT_NULL(gatt);
    VALIDATE_NOT_NULL(characteristic);
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCallback::OnCharacteristicChanged(
    /* [in] */ IBluetoothGatt* gatt,
    /* [in] */ IBluetoothGattCharacteristic* characteristic)
{
    VALIDATE_NOT_NULL(gatt);
    VALIDATE_NOT_NULL(characteristic);
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCallback::OnDescriptorRead(
    /* [in] */ IBluetoothGatt* gatt,
    /* [in] */ IBluetoothGattDescriptor* descriptor,
    /* [in] */ Int32 status)
{
    VALIDATE_NOT_NULL(gatt);
    VALIDATE_NOT_NULL(descriptor);
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCallback::OnDescriptorWrite(
    /* [in] */ IBluetoothGatt* gatt,
    /* [in] */ IBluetoothGattDescriptor* descriptor,
    /* [in] */ Int32 status)
{
    VALIDATE_NOT_NULL(gatt);
    VALIDATE_NOT_NULL(descriptor);
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCallback::OnReliableWriteCompleted(
    /* [in] */ IBluetoothGatt* gatt,
    /* [in] */ Int32 status)
{
    VALIDATE_NOT_NULL(gatt);
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCallback::OnReadRemoteRssi(
    /* [in] */ IBluetoothGatt* gatt,
    /* [in] */ Int32 rssi,
    /* [in] */ Int32 status)
{
    VALIDATE_NOT_NULL(gatt);
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCallback::OnMtuChanged(
    /* [in] */ IBluetoothGatt* gatt,
    /* [in] */ Int32 mtu,
    /* [in] */ Int32 status)
{
    VALIDATE_NOT_NULL(gatt);
    assert(0);
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
