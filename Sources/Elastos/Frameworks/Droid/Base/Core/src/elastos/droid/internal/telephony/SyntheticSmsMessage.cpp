
#include "elastos/droid/internal/telephony/SyntheticSmsMessage.h"

using Elastos::Droid::Internal::Telephony::MessageClass_UNKNOWN;

using Org::Json::IJSONObject;
// using Org::Json::CJSONObject;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

//==============================================================
//  SyntheticSmsMessage::
//==============================================================
CAR_INTERFACE_IMPL(SyntheticSmsMessage, SmsMessageBase, ISyntheticSmsMessage)

Boolean SyntheticSmsMessage::IsSyntheticPdu(
    /* [in] */ ArrayOf<Byte>* pdu)
{
    // try {
    // AutoPtr<IJSONObject> json;
    assert(0 && "TODO");
    // CJSONObject::New(String(pdu), (IJSONObject**)&json);
    Boolean res = FALSE;
    // json->OptBoolean(String("synthetic"), FALSE, &res);
    return res;
    // }
    // Catch (Exception e) {
    // }
}

AutoPtr<ISyntheticSmsMessage> SyntheticSmsMessage::CreateFromPdu(
    /* [in] */ ArrayOf<Byte>* pdu)
{
    // try {
    // TODO: use Parcelable or Bundle or something that serializes?
    String strPdu(*pdu);
    assert(0 && "TODO");
    // AutoPtr<IJSONObject> json;
    // CJSONObject::New(strPdu, (IJSONObject**)&json);
    String address, body, scAddress;
    // json->GetString(String("originatingAddress"), &address);
    // json->GetString(String("messageBody"), &body);
    // json->OptString(String("scAddress"), NULL, &scAddress);
    Int64 millis = 0;
    // json->GetInt64(String("timestampMillis"), &millis);
    AutoPtr<ISyntheticSmsMessage> message;
    // CSyntheticSmsMessage::New(
    //         address,
    //         scAddress,
    //         body,
    //         millis,
    //         (ISyntheticSmsMessage**)&message);
    return message;
    // }
    // Catch (Exception e) {
    //     e->PrintStackTrace();
    // }
}

ECode SyntheticSmsMessage::constructor(
    /* [in] */ String originatingAddress,
    /* [in] */ String scAddress,
    /* [in] */ String messageBody,
    /* [in] */ Int64 timestampMillis)
{
    mOriginatingAddress = new SyntheticAddress();
    assert(0 && "TODO");
    // mOriginatingAddress->mAddress = originatingAddress;

    mMessageBody = messageBody;
    mScTimeMillis = timestampMillis;
    mScAddress = scAddress;

    // try {
    // AutoPtr<IJSONObject> json;
    // CJSONObject::New((IJSONObject**)&json);
    // json->Put(String("originatingAddress"), originatingAddress);
    // json->Put(String("scAddress"), scAddress);
    // json->Put(String("messageBody"), messageBody);
    // json->Put(String("timestampMillis"), timestampMillis);
    // json->Put(String("synthetic"), TRUE);
    String str;
    // json->ToString(&str);
    mPdu = str.GetBytes();
    // }
    // Catch (Exception e) {
    //     e->PrintStackTrace();
    // }
    return NOERROR;
}

ECode SyntheticSmsMessage::GetMessageClass(
    /* [out] */ MessageClass* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MessageClass_UNKNOWN;
    return NOERROR;
}

ECode SyntheticSmsMessage::GetProtocolIdentifier(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    return NOERROR;
}

ECode SyntheticSmsMessage::IsReplace(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode SyntheticSmsMessage::IsCphsMwiMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode SyntheticSmsMessage::IsMWIClearMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode SyntheticSmsMessage::IsMWISetMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode SyntheticSmsMessage::IsMwiDontStore(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode SyntheticSmsMessage::GetStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    return NOERROR;
}

ECode SyntheticSmsMessage::IsStatusReportMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode SyntheticSmsMessage::IsReplyPathPresent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
