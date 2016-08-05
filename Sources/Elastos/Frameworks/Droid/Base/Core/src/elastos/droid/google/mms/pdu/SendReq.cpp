#include "Elastos.Droid.Google.h"
#include "elastos/droid/google/mms/pdu/SendReq.h"
#include "elastos/droid/google/mms/pdu/CEncodedStringValue.h"

#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                               SendReq
//=====================================================================
CAR_INTERFACE_IMPL(SendReq, MultimediaMessagePdu, ISendReq);
const String SendReq::TAG("SendReq");

SendReq::SendReq()
{
}

ECode SendReq::constructor()
{
    MultimediaMessagePdu::constructor();

    // try {
        SetMessageType(IPduHeaders::MESSAGE_TYPE_SEND_REQ);
        SetMmsVersion(IPduHeaders::CURRENT_MMS_VERSION);
        // FIXME: Content-type must be decided according to whether
        // SMIL part present.
        SetContentType(String("application/vnd.wap.multipart.related").GetBytes());
        AutoPtr<IEncodedStringValue> p;
        CEncodedStringValue::New(IPduHeaders::FROM_INSERT_ADDRESS_TOKEN_STR.GetBytes(), (IEncodedStringValue**)&p);
        SetFrom(p);
        SetTransactionId(GenerateTransactionId());
    // } catch (InvalidHeaderValueException e) {
    //     // Impossible to reach here since all headers we set above are valid.
    //     Log.e(TAG, "Unexpected InvalidHeaderValueException.", e);
    //     throw new RuntimeException(e);
    // }
    return NOERROR;
}

ECode SendReq::constructor(
    /* [in] */ ArrayOf<Byte>* contentType,
    /* [in] */ IEncodedStringValue* from,
    /* [in] */ Int32 mmsVersion,
    /* [in] */ ArrayOf<Byte>* transactionId)
{
    MultimediaMessagePdu::constructor();
    SetMessageType(IPduHeaders::MESSAGE_TYPE_SEND_REQ);
    SetContentType(contentType);
    SetFrom(from);
    SetMmsVersion(mmsVersion);
    SetTransactionId(transactionId);
    return NOERROR;
}

ECode SendReq::constructor(
    /* [in] */ IPduHeaders* headers)
{
    MultimediaMessagePdu::constructor(headers);
    return NOERROR;
}

ECode SendReq::constructor(
    /* [in] */ IPduHeaders* headers,
    /* [in] */ IPduBody* body)
{
    MultimediaMessagePdu::constructor(headers, body);
    return NOERROR;
}

ECode SendReq::GetBcc(
    /* [out] */ ArrayOf<IEncodedStringValue*>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetEncodedStringValues(IPduHeaders::BCC, result);
}

ECode SendReq::AddBcc(
    /* [in] */ IEncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    mPduHeaders->AppendEncodedStringValue(value, IPduHeaders::BCC);
    return NOERROR;
}

ECode SendReq::SetBcc(
    /* [in] */ ArrayOf<IEncodedStringValue*>* value)
{
    VALIDATE_NOT_NULL(value);
    mPduHeaders->SetEncodedStringValues(value, IPduHeaders::BCC);
    return NOERROR;
}

ECode SendReq::GetCc(
    /* [out] */ ArrayOf<IEncodedStringValue*>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetEncodedStringValues(IPduHeaders::CC, result);
}

ECode SendReq::AddCc(
    /* [in] */ IEncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    mPduHeaders->AppendEncodedStringValue(value, IPduHeaders::CC);
    return NOERROR;
}

ECode SendReq::SetCc(
    /* [in] */ ArrayOf<IEncodedStringValue*>* value)
{
    VALIDATE_NOT_NULL(value);
    mPduHeaders->SetEncodedStringValues(value, IPduHeaders::CC);
    return NOERROR;
}

ECode SendReq::GetContentType(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetTextString(IPduHeaders::CONTENT_TYPE, result);
}

ECode SendReq::SetContentType(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);

    mPduHeaders->SetTextString(value, IPduHeaders::CONTENT_TYPE);
    return NOERROR;
}

ECode SendReq::GetDeliveryReport(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetOctet(IPduHeaders::DELIVERY_REPORT, result);
}

ECode SendReq::SetDeliveryReport(
    /* [in] */ Int32 value)
{
    mPduHeaders->SetOctet(value, IPduHeaders::DELIVERY_REPORT);
    return NOERROR;
}

ECode SendReq::GetExpiry(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetLongInteger(IPduHeaders::EXPIRY, result);
}

ECode SendReq::SetExpiry(
    /* [in] */ Int64 value)
{
    mPduHeaders->SetLongInteger(value, IPduHeaders::EXPIRY);
    return NOERROR;
}

ECode SendReq::GetMessageSize(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetLongInteger(IPduHeaders::MESSAGE_SIZE, result);
}

ECode SendReq::SetMessageSize(
    /* [in] */ Int64 value)
{
    mPduHeaders->SetLongInteger(value, IPduHeaders::MESSAGE_SIZE);
    return NOERROR;
}

ECode SendReq::GetMessageClass(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetTextString(IPduHeaders::MESSAGE_CLASS, result);
}

ECode SendReq::SetMessageClass(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    mPduHeaders->SetTextString(value, IPduHeaders::MESSAGE_CLASS);
    return NOERROR;
}

ECode SendReq::GetReadReport(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetOctet(IPduHeaders::READ_REPORT, result);
}

ECode SendReq::SetReadReport(
    /* [in] */ Int32 value)
{
    mPduHeaders->SetOctet(value, IPduHeaders::READ_REPORT);
    return NOERROR;
}

ECode SendReq::SetTo(
    /* [in] */ ArrayOf<IEncodedStringValue*>* value)
{
    VALIDATE_NOT_NULL(value);
    mPduHeaders->SetEncodedStringValues(value, IPduHeaders::TO);
    return NOERROR;
}

ECode SendReq::GetTransactionId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetTextString(IPduHeaders::TRANSACTION_ID, result);
}

ECode SendReq::SetTransactionId(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    mPduHeaders->SetTextString(value, IPduHeaders::TRANSACTION_ID);
    return NOERROR;
}

AutoPtr<ArrayOf<Byte> > SendReq::GenerateTransactionId()
{
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 mills = 0;
    sys->GetCurrentTimeMillis(&mills);
    String transactionId(String("T") + StringUtils::ToHexString(mills));
    return transactionId.GetBytes();
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
