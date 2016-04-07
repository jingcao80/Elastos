
#include "elastos/droid/bluetooth/BluetoothGattCharacteristic.h"
#include "elastos/core/Math.h"

using Elastos::Core::CInteger32;
using Elastos::Core::CFloat;
using Elastos::Utility::CArrayList;

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
    InitCharacteristic(NULL, uuid, 0, properties, permissions);
}

BluetoothGattCharacteristic::BluetoothGattCharacteristic(
    /* [in] */ IBluetoothGattService* service,
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 instanceId,
    /* [in] */ Int32 properties,
    /* [in] */ Int32 permissions)
{
    InitCharacteristic(service, uuid, instanceId, properties, permissions);
}

ECode BluetoothGattCharacteristic::GetKeySize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mKeySize;
    return NOERROR;
}

ECode BluetoothGattCharacteristic::AddDescriptor(
    /* [in] */ IBluetoothGattDescriptor* descriptor,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mDescriptors->Add(descriptor);
    descriptor->SetCharacteristic(this);
    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetDescriptor(
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 instanceId,
    /* [out] */ IBluetoothGattDescriptor** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    Int32 size;
    mDescriptors->GetSize(&size);
    for(Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mDescriptors->Get(i, (IInterface**)&obj);
        IBluetoothGattDescriptor* descriptor = IBluetoothGattDescriptor::Probe(obj);
        AutoPtr<IUUID> tUuid;
        descriptor->GetUuid((IUUID**)&tUuid);
        Int32 tInstanceId;
        descriptor->GetInstanceId(&tInstanceId);
        Boolean uuidEquals;
        if ((tUuid->Equals(uuid, &uuidEquals), uuidEquals)
         && tInstanceId == instanceId) {
            *result = descriptor;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetService(
    /* [out] */ IBluetoothGattService** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mService;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::SetService(
    /* [in] */ IBluetoothGattService* service)
{
    mService = service;
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetUuid(
    /* [out] */ IUUID** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUuid;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetInstanceId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mInstance;
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetProperties(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mProperties;
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetPermissions(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPermissions;
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetWriteType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mWriteType;
    return NOERROR;
}

ECode BluetoothGattCharacteristic::SetWriteType(
    /* [in] */ Int32 writeType)
{
    mWriteType = writeType;
    return NOERROR;
}

ECode BluetoothGattCharacteristic::SetKeySize(
    /* [in] */ Int32 keySize)
{
    mKeySize = keySize;
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetDescriptors(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDescriptors;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetDescriptor(
    /* [in] */ IUUID* uuid,
    /* [out] */ IBluetoothGattDescriptor** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    Int32 size;
    mDescriptors->GetSize(&size);
    for(Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mDescriptors->Get(i, (IInterface**)&obj);
        IBluetoothGattDescriptor* descriptor = IBluetoothGattDescriptor::Probe(obj);
        AutoPtr<IUUID> tUuid;
        descriptor->GetUuid((IUUID**)&tUuid);
        Boolean uuidEquals;
        if (tUuid->Equals(uuid, &uuidEquals), uuidEquals) {
            *result = descriptor;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetValue(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetIntValue(
    /* [in] */ Int32 formatType,
    /* [in] */ Int32 offset,
    /* [out] */ IInteger32** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    Int32 valueLength = mValue->GetLength();
    if ((offset + GetTypeLen(formatType)) > valueLength) return NOERROR;

    switch (formatType) {
        case FORMAT_UINT8:
            return CInteger32::New(UnsignedByteToInt((*mValue)[offset]), result);

        case FORMAT_UINT16:
            return CInteger32::New(UnsignedBytesToInt((*mValue)[offset], (*mValue)[offset+1]), result);

        case FORMAT_UINT32:
            return CInteger32::New(UnsignedBytesToInt((*mValue)[offset], (*mValue)[offset+1],
                                      (*mValue)[offset+2], (*mValue)[offset+3]), result);

        case FORMAT_SINT8:
            return CInteger32::New(UnsignedToSigned(UnsignedByteToInt((*mValue)[offset]), 8), result);

        case FORMAT_SINT16:
            return CInteger32::New(UnsignedToSigned(UnsignedBytesToInt((*mValue)[offset],
                                                       (*mValue)[offset+1]), 16), result);

        case FORMAT_SINT32:
            return CInteger32::New(UnsignedToSigned(UnsignedBytesToInt((*mValue)[offset],
                    (*mValue)[offset+1], (*mValue)[offset+2], (*mValue)[offset+3]), 32), result);
    }

    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetFloatValue(
    /* [in] */ Int32 formatType,
    /* [in] */ Int32 offset,
    /* [out] */ IFloat** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    Int32 valueLength = mValue->GetLength();
    if ((offset + GetTypeLen(formatType)) > valueLength) return NOERROR;

    switch (formatType) {
        case FORMAT_SFLOAT:
            return CFloat::New(BytesToFloat((*mValue)[offset], (*mValue)[offset+1]), result);

        case FORMAT_FLOAT:
            return CFloat::New(BytesToFloat((*mValue)[offset], (*mValue)[offset+1],
                                (*mValue)[offset+2], (*mValue)[offset+3]), result);
    }

    return NOERROR;
}

ECode BluetoothGattCharacteristic::GetStringValue(
    /* [in] */ Int32 offset,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 valueLength = mValue->GetLength();
    if (offset > valueLength) return NOERROR;
    //byte[] strBytes = new byte[mValue.length - offset];
    AutoPtr<ArrayOf<Byte> > strBytes = ArrayOf<Byte>::Alloc(valueLength - offset);
    for (Int32 i=0; i != (valueLength - offset); ++i) (*strBytes)[i] = (*mValue)[offset+i];
    *result = String(*strBytes);
    return NOERROR;
}

ECode BluetoothGattCharacteristic::SetValue(
    /* [in] */ ArrayOf<Byte>* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mValue = value;
    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGattCharacteristic::SetValue(
    /* [in] */ Int32 value,
    /* [in] */ Int32 formatType,
    /* [in] */ Int32 offset,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    Int32 len = offset + GetTypeLen(formatType);
    if (mValue == NULL) mValue = ArrayOf<Byte>::Alloc(len);
    Int32 length = mValue->GetLength();
    if (len > length) return NOERROR;

    switch (formatType) {
        case FORMAT_SINT8:
            value = IntToSignedBits(value, 8);
            // Fall-through intended
        case FORMAT_UINT8:
            (*mValue)[offset] = (Byte)(value & 0xFF);
            break;

        case FORMAT_SINT16:
            value = IntToSignedBits(value, 16);
            // Fall-through intended
        case FORMAT_UINT16:
            (*mValue)[offset++] = (Byte)(value & 0xFF);
            (*mValue)[offset] = (Byte)((value >> 8) & 0xFF);
            break;

        case FORMAT_SINT32:
            value = IntToSignedBits(value, 32);
            // Fall-through intended
        case FORMAT_UINT32:
            (*mValue)[offset++] = (Byte)(value & 0xFF);
            (*mValue)[offset++] = (Byte)((value >> 8) & 0xFF);
            (*mValue)[offset++] = (Byte)((value >> 16) & 0xFF);
            (*mValue)[offset] = (Byte)((value >> 24) & 0xFF);
            break;

        default:
            return NOERROR;
    }
    *result = TRUE;
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
    *result = FALSE;
    Int32 len = offset + GetTypeLen(formatType);
    if (mValue == NULL) mValue = ArrayOf<Byte>::Alloc(len);
    Int32 length = mValue->GetLength();
    if (len > length) return NOERROR;

    switch (formatType) {
        case FORMAT_SFLOAT:
            mantissa = IntToSignedBits(mantissa, 12);
            exponent = IntToSignedBits(exponent, 4);
            (*mValue)[offset++] = (Byte)(mantissa & 0xFF);
            (*mValue)[offset] = (Byte)((mantissa >> 8) & 0x0F);
            (*mValue)[offset] += (Byte)((exponent & 0x0F) << 4);
            break;

        case FORMAT_FLOAT:
            mantissa = IntToSignedBits(mantissa, 24);
            exponent = IntToSignedBits(exponent, 8);
            (*mValue)[offset++] = (Byte)(mantissa & 0xFF);
            (*mValue)[offset++] = (Byte)((mantissa >> 8) & 0xFF);
            (*mValue)[offset++] = (Byte)((mantissa >> 16) & 0xFF);
            (*mValue)[offset] += (Byte)(exponent & 0xFF);
            break;

        default:
            return NOERROR;
    }

    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGattCharacteristic::SetValue(
    /* [in] */ const String& value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mValue = value.GetBytes();
    *result = TRUE;
    return NOERROR;
}

void BluetoothGattCharacteristic::InitCharacteristic(
    /* [in] */ IBluetoothGattService* service,
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 instanceId,
    /* [in] */ Int32 properties,
    /* [in] */ Int32 permissions)
{
    mUuid = uuid;
    mInstance = instanceId;
    mProperties = properties;
    mPermissions = permissions;
    mService = service;
    mValue = NULL;
    //mDescriptors = new ArrayList<BluetoothGattDescriptor>();
    CArrayList::New((IList**)&mDescriptors);

    if ((mProperties & PROPERTY_WRITE_NO_RESPONSE) != 0) {
        mWriteType = WRITE_TYPE_NO_RESPONSE;
    } else {
        mWriteType = WRITE_TYPE_DEFAULT;
    }
}

Int32 BluetoothGattCharacteristic::GetTypeLen(
    /* [in] */ Int32 formatType)
{
    return formatType & 0xF;
}

Int32 BluetoothGattCharacteristic::UnsignedByteToInt(
    /* [in] */ Byte b)
{
    return b & 0xFF;
}

Int32 BluetoothGattCharacteristic::UnsignedBytesToInt(
    /* [in] */ Byte b0,
    /* [in] */ Byte b1)
{
    return (UnsignedByteToInt(b0) + (UnsignedByteToInt(b1) << 8));
}

Int32 BluetoothGattCharacteristic::UnsignedBytesToInt(
    /* [in] */ Byte b0,
    /* [in] */ Byte b1,
    /* [in] */ Byte b2,
    /* [in] */ Byte b3)
{
    return (UnsignedByteToInt(b0) + (UnsignedByteToInt(b1) << 8))
         + (UnsignedByteToInt(b2) << 16) + (UnsignedByteToInt(b3) << 24);
}

Float BluetoothGattCharacteristic::BytesToFloat(
    /* [in] */ Byte b0,
    /* [in] */ Byte b1)
{
    Int32 mantissa = UnsignedToSigned(UnsignedByteToInt(b0)
                    + ((UnsignedByteToInt(b1) & 0x0F) << 8), 12);
    Int32 exponent = UnsignedToSigned(UnsignedByteToInt(b1) >> 4, 4);
    return (Float)(mantissa * Elastos::Core::Math::Pow(10, exponent));
}

Float BluetoothGattCharacteristic::BytesToFloat(
    /* [in] */ Byte b0,
    /* [in] */ Byte b1,
    /* [in] */ Byte b2,
    /* [in] */ Byte b3)
{
    Int32 mantissa = UnsignedToSigned(UnsignedByteToInt(b0)
                    + (UnsignedByteToInt(b1) << 8)
                    + (UnsignedByteToInt(b2) << 16), 24);
    return (Float)(mantissa * Elastos::Core::Math::Pow(10, b3));
}

Int32 BluetoothGattCharacteristic::UnsignedToSigned(
    /* [in] */ Int32 unsignedInt,
    /* [in] */ Int32 size)
{
    if ((unsignedInt & (1 << (size-1))) != 0) {
        unsignedInt = -1 * ((1 << (size-1)) - (unsignedInt & ((1 << (size-1)) - 1)));
    }
    return unsignedInt;
}

Int32 BluetoothGattCharacteristic::IntToSignedBits(
    /* [in] */ Int32 i,
    /* [in] */ Int32 size)
{
    if (i < 0) {
        i = (1 << (size-1)) + (i & ((1 << (size-1)) - 1));
    }
    return i;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
