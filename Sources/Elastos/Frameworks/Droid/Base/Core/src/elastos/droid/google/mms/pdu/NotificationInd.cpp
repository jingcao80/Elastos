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
    // ==================before translated======================
    // super();
    // setMessageType(PduHeaders.MESSAGE_TYPE_NOTIFICATION_IND);
    return NOERROR;
}

ECode NotificationInd::constructor(
    /* [in] */ IPduHeaders* headers)
{
    // ==================before translated======================
    // super(headers);
    return NOERROR;
}

ECode NotificationInd::GetContentClass(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getOctet(PduHeaders.CONTENT_CLASS);
    assert(0);
    return NOERROR;
}

ECode NotificationInd::SetContentClass(
    /* [in] */ Int32 value)
{
    // ==================before translated======================
    // mPduHeaders.setOctet(value, PduHeaders.CONTENT_CLASS);
    assert(0);
    return NOERROR;
}

ECode NotificationInd::GetContentLocation(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getTextString(PduHeaders.CONTENT_LOCATION);
    assert(0);
    return NOERROR;
}

ECode NotificationInd::SetContentLocation(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setTextString(value, PduHeaders.CONTENT_LOCATION);
    assert(0);
    return NOERROR;
}

ECode NotificationInd::GetExpiry(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getLongInteger(PduHeaders.EXPIRY);
    assert(0);
    return NOERROR;
}

ECode NotificationInd::SetExpiry(
    /* [in] */ Int64 value)
{
    // ==================before translated======================
    // mPduHeaders.setLongInteger(value, PduHeaders.EXPIRY);
    assert(0);
    return NOERROR;
}

ECode NotificationInd::GetFrom(
    /* [out] */ IEncodedStringValue** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getEncodedStringValue(PduHeaders.FROM);
    assert(0);
    return NOERROR;
}

ECode NotificationInd::SetFrom(
    /* [in] */ IEncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setEncodedStringValue(value, PduHeaders.FROM);
    assert(0);
    return NOERROR;
}

ECode NotificationInd::GetMessageClass(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getTextString(PduHeaders.MESSAGE_CLASS);
    assert(0);
    return NOERROR;
}

ECode NotificationInd::SetMessageClass(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setTextString(value, PduHeaders.MESSAGE_CLASS);
    assert(0);
    return NOERROR;
}

ECode NotificationInd::GetMessageSize(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getLongInteger(PduHeaders.MESSAGE_SIZE);
    assert(0);
    return NOERROR;
}

ECode NotificationInd::SetMessageSize(
    /* [in] */ Int64 value)
{
    // ==================before translated======================
    // mPduHeaders.setLongInteger(value, PduHeaders.MESSAGE_SIZE);
    assert(0);
    return NOERROR;
}

ECode NotificationInd::GetSubject(
    /* [out] */ IEncodedStringValue** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getEncodedStringValue(PduHeaders.SUBJECT);
    assert(0);
    return NOERROR;
}

ECode NotificationInd::SetSubject(
    /* [in] */ IEncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setEncodedStringValue(value, PduHeaders.SUBJECT);
    assert(0);
    return NOERROR;
}

ECode NotificationInd::GetTransactionId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getTextString(PduHeaders.TRANSACTION_ID);
    assert(0);
    return NOERROR;
}

ECode NotificationInd::SetTransactionId(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setTextString(value, PduHeaders.TRANSACTION_ID);
    assert(0);
    return NOERROR;
}

ECode NotificationInd::GetDeliveryReport(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getOctet(PduHeaders.DELIVERY_REPORT);
    assert(0);
    return NOERROR;
}

ECode NotificationInd::SetDeliveryReport(
    /* [in] */ Int32 value)
{
    // ==================before translated======================
    // mPduHeaders.setOctet(value, PduHeaders.DELIVERY_REPORT);
    assert(0);
    return NOERROR;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
