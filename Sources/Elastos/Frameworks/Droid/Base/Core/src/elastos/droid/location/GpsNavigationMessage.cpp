
#include "elastos/droid/location/GpsNavigationMessage.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Location {

const String GpsNavigationMessage::TAG("GpsNavigationMessage");
const AutoPtr<ArrayOf<Byte> > GpsNavigationMessage::EMPTY_ARRAY = ArrayOf<Byte>::Alloc(0);

CAR_INTERFACE_IMPL_2(GpsNavigationMessage, Object, IGpsNavigationMessage, IParcelable)

GpsNavigationMessage::GpsNavigationMessage()
{
}

ECode GpsNavigationMessage::constructor()
{
    Initialize();
    return NOERROR;
}

ECode GpsNavigationMessage::Set(
    /* [in] */ IGpsNavigationMessage* navigationMessage)
{
    AutoPtr<GpsNavigationMessage> _navigationMessage = (GpsNavigationMessage*)navigationMessage;
    mType = _navigationMessage->mType;
    mPrn = _navigationMessage->mPrn;
    mMessageId = _navigationMessage->mMessageId;
    mSubmessageId = _navigationMessage->mSubmessageId;
    mData = _navigationMessage->mData;
    return NOERROR;
}

ECode GpsNavigationMessage::Reset()
{
    return Initialize();
}

ECode GpsNavigationMessage::GetType(
    /* [out] */ Byte* type)
{
    VALIDATE_NOT_NULL(type)
    *type = mType;
    return NOERROR;
}

ECode GpsNavigationMessage::SetType(
    /* [in] */ Byte value)
{
    switch(value) {
        case IGpsNavigationMessage::TYPE_UNKNOWN:
        case IGpsNavigationMessage::TYPE_L1CA:
        case IGpsNavigationMessage::TYPE_L2CNAV:
        case IGpsNavigationMessage::TYPE_L5CNAV:
        case IGpsNavigationMessage::TYPE_CNAV2:
            mType = value;
            break;
        default:
            StringBuilder sb("Sanitizing invalid 'type': ");
            sb += value;
            Logger::D(TAG, sb.ToString());
            mType = IGpsNavigationMessage::TYPE_UNKNOWN;
            break;
    }
    return NOERROR;
}

ECode GpsNavigationMessage::GetTypeString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    switch(mType) {
        case IGpsNavigationMessage::TYPE_UNKNOWN:
            *str = String("Unknown");
            return NOERROR;
        case IGpsNavigationMessage::TYPE_L1CA:
            *str = String("L1 C/A");
            return NOERROR;
        case IGpsNavigationMessage::TYPE_L2CNAV:
            *str = String("L2-CNAV");
            return NOERROR;
        case IGpsNavigationMessage::TYPE_L5CNAV:
            *str = String("L5-CNAV");
            return NOERROR;
        case IGpsNavigationMessage::TYPE_CNAV2:
            *str = String("CNAV-2");
            return NOERROR;
        default:
            *str = String("<Invalid>");
            return NOERROR;
    }
}

ECode GpsNavigationMessage::GetPrn(
    /* [out] */ Byte* prn)
{
    VALIDATE_NOT_NULL(prn)
    *prn = mPrn;
    return NOERROR;
}

ECode GpsNavigationMessage::SetPrn(
    /* [in] */ Byte value)
{
    mPrn = value;
    return NOERROR;
}

ECode GpsNavigationMessage::GetMessageId(
    /* [out] */ Int16* msgId)
{
    VALIDATE_NOT_NULL(msgId)
    *msgId = mMessageId;
    return NOERROR;
}

ECode GpsNavigationMessage::SetMessageId(
    /* [in] */ Int16 value)
{
    mMessageId = value;
    return NOERROR;
}

ECode GpsNavigationMessage::GetSubmessageId(
    /* [out] */ Int16* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mSubmessageId;
    return NOERROR;
}

ECode GpsNavigationMessage::SetSubmessageId(
    /* [in] */ Int16 value)
{
    mSubmessageId = value;
    return NOERROR;
}

ECode GpsNavigationMessage::GetData(
    /* [out, callee] */ ArrayOf<Byte>** data)
{
    VALIDATE_NOT_NULL(data)
    *data = mData.Get();
    REFCOUNT_ADD(*data)
    return NOERROR;
}

ECode GpsNavigationMessage::SetData(
    /* [in] */ ArrayOf<Byte>* value)
{
    if (value == NULL) {
        return E_INVALID_PARAMETER_EXCEPTION;
    }

    mData = value;
    return NOERROR;
}

ECode GpsNavigationMessage::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    Byte type;
    parcel->ReadByte(&type);
    SetType(type);
    Byte prn;
    parcel->ReadByte(&prn);
    SetPrn(prn);
    Int16 messageId;
    parcel->ReadInt16(&messageId);
    SetMessageId(messageId);
    Int16 subMessageId;
    parcel->ReadInt16(&subMessageId);
    SetSubmessageId(subMessageId);
    Int32 dataLength;
    parcel->ReadInt32(&dataLength);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(dataLength);
    parcel->ReadArrayOf((Handle32*)&data);
    SetData(data.Get());
    return NOERROR;
}

ECode GpsNavigationMessage::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->WriteByte(mType);
    parcel->WriteByte(mPrn);
    parcel->WriteInt32(mMessageId);
    parcel->WriteInt32(mSubmessageId);
    parcel->WriteInt32(mData->GetLength());
    parcel->WriteArrayOf((Handle32)(ArrayOf<Byte>*)mData);
    return NOERROR;
}

ECode GpsNavigationMessage::ToString(
    /* [out] */ String* str)
{
#if 0 // String.format() is not existed.
    final String format = "   %-15s = %s\n";
    StringBuilder builder = new StringBuilder("GpsNavigationMessage:\n");

    builder.append(String.format(format, "Type", getTypeString()));
    builder.append(String.format(format, "Prn", mPrn));
    builder.append(String.format(format, "MessageId", mMessageId));
    builder.append(String.format(format, "SubmessageId", mSubmessageId));

    builder.append(String.format(format, "Data", "{"));
    String prefix = "        ";
    for(byte value : mData) {
        builder.append(prefix);
        builder.append(value);
        prefix = ", ";
    }
    builder.append(" }");

    return builder.toString();
#endif
    return E_NOT_IMPLEMENTED;
}

ECode GpsNavigationMessage::Initialize()
{
    mType = IGpsNavigationMessage::TYPE_UNKNOWN;
    mPrn = 0;
    mMessageId = -1;
    mSubmessageId = -1;
    mData = EMPTY_ARRAY;
    return NOERROR;
}

} // namespace Location
} // namepsace Droid
} // namespace Elastos
