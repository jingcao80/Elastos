
#include "elastos/droid/bluetooth/BluetoothGattCharacteristic.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//                     BluetoothGattCharacteristic
//=====================================================================
CAR_INTERFACE_IMPL(BluetoothGattCharacteristic, Object, IBluetoothGattCharacteristic);

BluetoothGattCharacteristic::BluetoothGattCharacteristic()
{
}

BluetoothGattCharacteristic::BluetoothGattCharacteristic(
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 properties,
    /* [in] */ Int32 permissions)
{
    // ==================before translated======================
    // initCharacteristic(null, uuid, 0, properties, permissions);
}

BluetoothGattCharacteristic::BluetoothGattCharacteristic(
    /* [in] */ IBluetoothGattService* service,
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 instanceId,
    /* [in] */ Int32 properties,
    /* [in] */ Int32 permissions)
{
    // ==================before translated======================
    // initCharacteristic(service, uuid, instanceId, properties, permissions);
}

ECode BluetoothGattCharacteristic::GetKeySize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mKeySize;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::AddDescriptor(
    /* [in] */ IBluetoothGattDescriptor* descriptor,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(descriptor);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mDescriptors.add(descriptor);
    // descriptor.setCharacteristic(this);
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetDescriptor(
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 instanceId,
    /* [out] */ IBluetoothGattDescriptor** result)
{
    VALIDATE_NOT_NULL(uuid);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // for(BluetoothGattDescriptor descriptor : mDescriptors) {
    //     if (descriptor.getUuid().equals(uuid)
    //      && descriptor.getInstanceId() == instanceId) {
    //         return descriptor;
    //     }
    // }
    // return null;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetService(
    /* [out] */ IBluetoothGattService** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mService;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::SetService(
    /* [in] */ IBluetoothGattService* service)
{
    VALIDATE_NOT_NULL(service);
    // ==================before translated======================
    // mService = service;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetUuid(
    /* [out] */ IUUID** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mUuid;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetInstanceId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mInstance;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetProperties(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mProperties;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetPermissions(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPermissions;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetWriteType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWriteType;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::SetWriteType(
    /* [in] */ Int32 writeType)
{
    // ==================before translated======================
    // mWriteType = writeType;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::SetKeySize(
    /* [in] */ Int32 keySize)
{
    // ==================before translated======================
    // mKeySize = keySize;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetDescriptors(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDescriptors;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetDescriptor(
    /* [in] */ IUUID* uuid,
    /* [out] */ IBluetoothGattDescriptor** result)
{
    VALIDATE_NOT_NULL(uuid);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // for(BluetoothGattDescriptor descriptor : mDescriptors) {
    //     if (descriptor.getUuid().equals(uuid)) {
    //         return descriptor;
    //     }
    // }
    // return null;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetValue(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mValue;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetIntValue(
    /* [in] */ Int32 formatType,
    /* [in] */ Int32 offset,
    /* [out] */ IInteger32** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if ((offset + getTypeLen(formatType)) > mValue.length) return null;
    //
    // switch (formatType) {
    //     case FORMAT_UINT8:
    //         return unsignedByteToInt(mValue[offset]);
    //
    //     case FORMAT_UINT16:
    //         return unsignedBytesToInt(mValue[offset], mValue[offset+1]);
    //
    //     case FORMAT_UINT32:
    //         return unsignedBytesToInt(mValue[offset],   mValue[offset+1],
    //                                   mValue[offset+2], mValue[offset+3]);
    //     case FORMAT_SINT8:
    //         return unsignedToSigned(unsignedByteToInt(mValue[offset]), 8);
    //
    //     case FORMAT_SINT16:
    //         return unsignedToSigned(unsignedBytesToInt(mValue[offset],
    //                                                    mValue[offset+1]), 16);
    //
    //     case FORMAT_SINT32:
    //         return unsignedToSigned(unsignedBytesToInt(mValue[offset],
    //                 mValue[offset+1], mValue[offset+2], mValue[offset+3]), 32);
    // }
    //
    // return null;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetFloatValue(
    /* [in] */ Int32 formatType,
    /* [in] */ Int32 offset,
    /* [out] */ IFloat** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if ((offset + getTypeLen(formatType)) > mValue.length) return null;
    //
    // switch (formatType) {
    //     case FORMAT_SFLOAT:
    //         return bytesToFloat(mValue[offset], mValue[offset+1]);
    //
    //     case FORMAT_FLOAT:
    //         return bytesToFloat(mValue[offset],   mValue[offset+1],
    //                             mValue[offset+2], mValue[offset+3]);
    // }
    //
    // return null;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetStringValue(
    /* [in] */ Int32 offset,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (offset > mValue.length) return null;
    // byte[] strBytes = new byte[mValue.length - offset];
    // for (int i=0; i != (mValue.length-offset); ++i) strBytes[i] = mValue[offset+i];
    // return new String(strBytes);
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::SetValue(
    /* [in] */ ArrayOf<Byte>* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(value);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mValue = value;
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::SetValue(
    /* [in] */ Int32 value,
    /* [in] */ Int32 formatType,
    /* [in] */ Int32 offset,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // int len = offset + getTypeLen(formatType);
    // if (mValue == null) mValue = new byte[len];
    // if (len > mValue.length) return false;
    //
    // switch (formatType) {
    //     case FORMAT_SINT8:
    //         value = intToSignedBits(value, 8);
    //         // Fall-through intended
    //     case FORMAT_UINT8:
    //         mValue[offset] = (byte)(value & 0xFF);
    //         break;
    //
    //     case FORMAT_SINT16:
    //         value = intToSignedBits(value, 16);
    //         // Fall-through intended
    //     case FORMAT_UINT16:
    //         mValue[offset++] = (byte)(value & 0xFF);
    //         mValue[offset] = (byte)((value >> 8) & 0xFF);
    //         break;
    //
    //     case FORMAT_SINT32:
    //         value = intToSignedBits(value, 32);
    //         // Fall-through intended
    //     case FORMAT_UINT32:
    //         mValue[offset++] = (byte)(value & 0xFF);
    //         mValue[offset++] = (byte)((value >> 8) & 0xFF);
    //         mValue[offset++] = (byte)((value >> 16) & 0xFF);
    //         mValue[offset] = (byte)((value >> 24) & 0xFF);
    //         break;
    //
    //     default:
    //         return false;
    // }
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::SetValue(
    /* [in] */ Int32 mantissa,
    /* [in] */ Int32 exponent,
    /* [in] */ Int32 formatType,
    /* [in] */ Int32 offset,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // int len = offset + getTypeLen(formatType);
    // if (mValue == null) mValue = new byte[len];
    // if (len > mValue.length) return false;
    //
    // switch (formatType) {
    //     case FORMAT_SFLOAT:
    //         mantissa = intToSignedBits(mantissa, 12);
    //         exponent = intToSignedBits(exponent, 4);
    //         mValue[offset++] = (byte)(mantissa & 0xFF);
    //         mValue[offset] = (byte)((mantissa >> 8) & 0x0F);
    //         mValue[offset] += (byte)((exponent & 0x0F) << 4);
    //         break;
    //
    //     case FORMAT_FLOAT:
    //         mantissa = intToSignedBits(mantissa, 24);
    //         exponent = intToSignedBits(exponent, 8);
    //         mValue[offset++] = (byte)(mantissa & 0xFF);
    //         mValue[offset++] = (byte)((mantissa >> 8) & 0xFF);
    //         mValue[offset++] = (byte)((mantissa >> 16) & 0xFF);
    //         mValue[offset] += (byte)(exponent & 0xFF);
    //         break;
    //
    //     default:
    //         return false;
    // }
    //
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::SetValue(
    /* [in] */ const String& value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mValue = value.getBytes();
    // return true;
    assert(0);
    return NOERROR;
}

void BluetoothGattCharacteristic::InitCharacteristic(
    /* [in] */ IBluetoothGattService* service,
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 instanceId,
    /* [in] */ Int32 properties,
    /* [in] */ Int32 permissions)
{
    // ==================before translated======================
    // mUuid = uuid;
    // mInstance = instanceId;
    // mProperties = properties;
    // mPermissions = permissions;
    // mService = service;
    // mValue = null;
    // mDescriptors = new ArrayList<BluetoothGattDescriptor>();
    //
    // if ((mProperties & PROPERTY_WRITE_NO_RESPONSE) != 0) {
    //     mWriteType = WRITE_TYPE_NO_RESPONSE;
    // } else {
    //     mWriteType = WRITE_TYPE_DEFAULT;
    // }
    assert(0);
}

Int32 BluetoothGattCharacteristic::GetTypeLen(
    /* [in] */ Int32 formatType)
{
    // ==================before translated======================
    // return formatType & 0xF;
    assert(0);
    return 0;
}

Int32 BluetoothGattCharacteristic::UnsignedByteToInt(
    /* [in] */ Byte b)
{
    // ==================before translated======================
    // return b & 0xFF;
    assert(0);
    return 0;
}

Int32 BluetoothGattCharacteristic::UnsignedBytesToInt(
    /* [in] */ Byte b0,
    /* [in] */ Byte b1)
{
    // ==================before translated======================
    // return (unsignedByteToInt(b0) + (unsignedByteToInt(b1) << 8));
    assert(0);
    return 0;
}

Int32 BluetoothGattCharacteristic::UnsignedBytesToInt(
    /* [in] */ Byte b0,
    /* [in] */ Byte b1,
    /* [in] */ Byte b2,
    /* [in] */ Byte b3)
{
    // ==================before translated======================
    // return (unsignedByteToInt(b0) + (unsignedByteToInt(b1) << 8))
    //      + (unsignedByteToInt(b2) << 16) + (unsignedByteToInt(b3) << 24);
    assert(0);
    return 0;
}

Float BluetoothGattCharacteristic::BytesToFloat(
    /* [in] */ Byte b0,
    /* [in] */ Byte b1)
{
    // ==================before translated======================
    // int mantissa = unsignedToSigned(unsignedByteToInt(b0)
    //                 + ((unsignedByteToInt(b1) & 0x0F) << 8), 12);
    // int exponent = unsignedToSigned(unsignedByteToInt(b1) >> 4, 4);
    // return (float)(mantissa * Math.pow(10, exponent));
    assert(0);
    return 0.0f;
}

Float BluetoothGattCharacteristic::BytesToFloat(
    /* [in] */ Byte b0,
    /* [in] */ Byte b1,
    /* [in] */ Byte b2,
    /* [in] */ Byte b3)
{
    // ==================before translated======================
    // int mantissa = unsignedToSigned(unsignedByteToInt(b0)
    //                 + (unsignedByteToInt(b1) << 8)
    //                 + (unsignedByteToInt(b2) << 16), 24);
    // return (float)(mantissa * Math.pow(10, b3));
    assert(0);
    return 0.0f;
}

Int32 BluetoothGattCharacteristic::UnsignedToSigned(
    /* [in] */ Int32 unsigned,
    /* [in] */ Int32 size)
{
    // ==================before translated======================
    // if ((unsigned & (1 << size-1)) != 0) {
    //     unsigned = -1 * ((1 << size-1) - (unsigned & ((1 << size-1) - 1)));
    // }
    // return unsigned;
    assert(0);
    return 0;
}

Int32 BluetoothGattCharacteristic::IntToSignedBits(
    /* [in] */ Int32 i,
    /* [in] */ Int32 size)
{
    // ==================before translated======================
    // if (i < 0) {
    //     i = (1 << size-1) + (i & ((1 << size-1) - 1));
    // }
    // return i;
    assert(0);
    return 0;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
