#include "Elastos.Droid.Google.h"
#include "elastos/droid/google/mms/pdu/NotificationInd.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                           NotificationInd
//=====================================================================
CAR_INTERFACE_IMPL(NotificationInd, GenericPdu, INotificationInd);

NotificationInd::NotificationInd()
{
    constructor();
}

ECode NotificationInd::constructor()
{
    GenericPdu::constructor();
    SetMessageType(IPduHeaders::MESSAGE_TYPE_NOTIFICATION_IND);
    return NOERROR;
}

ECode NotificationInd::constructor(
    /* [in] */ IPduHeaders* headers)
{
    GenericPdu::constructor(headers);
    return NOERROR;
}

ECode NotificationInd::GetContentClass(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetOctet(IPduHeaders::CONTENT_CLASS, result);
}

ECode NotificationInd::SetContentClass(
    /* [in] */ Int32 value)
{
    return mPduHeaders->SetOctet(value, IPduHeaders::CONTENT_CLASS);
}

ECode NotificationInd::GetContentLocation(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetTextString(IPduHeaders::CONTENT_LOCATION, result);
}

ECode NotificationInd::SetContentLocation(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    return mPduHeaders->SetTextString(value, IPduHeaders::CONTENT_LOCATION);
}

ECode NotificationInd::GetExpiry(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetLongInteger(IPduHeaders::EXPIRY, result);
}

ECode NotificationInd::SetExpiry(
    /* [in] */ Int64 value)
{
    mPduHeaders->SetLongInteger(value, IPduHeaders::EXPIRY);
    return NOERROR;
}

ECode NotificationInd::GetFrom(
    /* [out] */ IEncodedStringValue** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetEncodedStringValue(IPduHeaders::FROM, result);
}

ECode NotificationInd::SetFrom(
    /* [in] */ IEncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    return mPduHeaders->SetEncodedStringValue(value, IPduHeaders::FROM);
}

ECode NotificationInd::GetMessageClass(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetTextString(IPduHeaders::MESSAGE_CLASS, result);
}

ECode NotificationInd::SetMessageClass(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    return mPduHeaders->SetTextString(value, IPduHeaders::MESSAGE_CLASS);
}

ECode NotificationInd::GetMessageSize(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetLongInteger(IPduHeaders::MESSAGE_SIZE, result);
}

ECode NotificationInd::SetMessageSize(
    /* [in] */ Int64 value)
{
    mPduHeaders->SetLongInteger(value, IPduHeaders::MESSAGE_SIZE);
    return NOERROR;
}

ECode NotificationInd::GetSubject(
    /* [out] */ IEncodedStringValue** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetEncodedStringValue(IPduHeaders::SUBJECT, result);
}

ECode NotificationInd::SetSubject(
    /* [in] */ IEncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    return mPduHeaders->SetEncodedStringValue(value, IPduHeaders::SUBJECT);
}

ECode NotificationInd::GetTransactionId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetTextString(IPduHeaders::TRANSACTION_ID, result);
}

ECode NotificationInd::SetTransactionId(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    return mPduHeaders->SetTextString(value, IPduHeaders::TRANSACTION_ID);
}

ECode NotificationInd::GetDeliveryReport(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetOctet(IPduHeaders::DELIVERY_REPORT, result);
}

ECode NotificationInd::SetDeliveryReport(
    /* [in] */ Int32 value)
{
    return mPduHeaders->SetOctet(value, IPduHeaders::DELIVERY_REPORT);
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
