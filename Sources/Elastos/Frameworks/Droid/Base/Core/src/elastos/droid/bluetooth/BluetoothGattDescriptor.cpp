
#include "elastos/droid/bluetooth/BluetoothGattDescriptor.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//                       BluetoothGattDescriptor
//=====================================================================
//AutoPtr< ArrayOf<Byte> > BluetoothGattDescriptor::ENABLE_NOTIFICATION_VALUE = BluetoothGattDescriptor::MiddleInitEnableNotificationValue();
//AutoPtr< ArrayOf<Byte> > BluetoothGattDescriptor::ENABLE_INDICATION_VALUE = BluetoothGattDescriptor::MiddleInitEnableIndicationValue();
//AutoPtr< ArrayOf<Byte> > BluetoothGattDescriptor::DISABLE_NOTIFICATION_VALUE = BluetoothGattDescriptor::MiddleInitDisableNotificationValue();

CAR_INTERFACE_IMPL(BluetoothGattDescriptor, Object, IBluetoothGattDescriptor);

BluetoothGattDescriptor::BluetoothGattDescriptor()
{
}

ECode BluetoothGattDescriptor::constructor(
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 permissions)
{
    InitDescriptor(NULL, uuid, 0, permissions);
    return NOERROR;
}

BluetoothGattDescriptor::BluetoothGattDescriptor(
    /* [in] */ IBluetoothGattCharacteristic* characteristic,
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 instance,
    /* [in] */ Int32 permissions)
{
    InitDescriptor(characteristic, uuid, instance, permissions);
}

ECode BluetoothGattDescriptor::GetCharacteristic(
    /* [out] */ IBluetoothGattCharacteristic** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCharacteristic;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BluetoothGattDescriptor::SetCharacteristic(
    /* [in] */ IBluetoothGattCharacteristic* characteristic)
{
    mCharacteristic = characteristic;
    return NOERROR;
}

ECode BluetoothGattDescriptor::GetUuid(
    /* [out] */ IUUID** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUuid;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BluetoothGattDescriptor::GetInstanceId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mInstance;
    return NOERROR;
}

ECode BluetoothGattDescriptor::GetPermissions(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPermissions;
    return NOERROR;
}

ECode BluetoothGattDescriptor::GetValue(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BluetoothGattDescriptor::SetValue(
    /* [in] */ ArrayOf<Byte>* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mValue = value;
    *result = TRUE;
    return NOERROR;
}

AutoPtr< ArrayOf<Byte> > BluetoothGattDescriptor::MiddleInitEnableNotificationValue()
{
    // ==================before translated======================
    // ->WWZ_SIGN: ARRAY_INIT_START {
    // 0x01, 0x00
    // ->WWZ_SIGN: ARRAY_INIT_END }
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

AutoPtr< ArrayOf<Byte> > BluetoothGattDescriptor::MiddleInitEnableIndicationValue()
{
    // ==================before translated======================
    // ->WWZ_SIGN: ARRAY_INIT_START {
    // 0x02, 0x00
    // ->WWZ_SIGN: ARRAY_INIT_END }
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

AutoPtr< ArrayOf<Byte> > BluetoothGattDescriptor::MiddleInitDisableNotificationValue()
{
    // ==================before translated======================
    // ->WWZ_SIGN: ARRAY_INIT_START {
    // 0x00, 0x00
    // ->WWZ_SIGN: ARRAY_INIT_END }
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

void BluetoothGattDescriptor::InitDescriptor(
    /* [in] */ IBluetoothGattCharacteristic* characteristic,
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 instance,
    /* [in] */ Int32 permissions)
{
    mCharacteristic = characteristic;
    mUuid = uuid;
    mInstance = instance;
    mPermissions = permissions;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
