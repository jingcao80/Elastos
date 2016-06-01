
#include "elastos/droid/google/mms/pdu/SendReq.h"

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
    // ==================before translated======================
    // super();
    //
    // try {
    //     setMessageType(PduHeaders.MESSAGE_TYPE_SEND_REQ);
    //     setMmsVersion(PduHeaders.CURRENT_MMS_VERSION);
    //     // FIXME: Content-type must be decided according to whether
    //     // SMIL part present.
    //     setContentType("application/vnd.wap.multipart.related".getBytes());
    //     setFrom(new EncodedStringValue(PduHeaders.FROM_INSERT_ADDRESS_TOKEN_STR.getBytes()));
    //     setTransactionId(generateTransactionId());
    // } catch (InvalidHeaderValueException e) {
    //     // Impossible to reach here since all headers we set above are valid.
    //     Log.e(TAG, "Unexpected InvalidHeaderValueException.", e);
    //     throw new RuntimeException(e);
    // }
}

SendReq::SendReq(
    /* [in] */ ArrayOf<Byte>* contentType,
    /* [in] */ IEncodedStringValue* from,
    /* [in] */ Int32 mmsVersion,
    /* [in] */ ArrayOf<Byte>* transactionId)
{
    // ==================before translated======================
    // super();
    // setMessageType(PduHeaders.MESSAGE_TYPE_SEND_REQ);
    // setContentType(contentType);
    // setFrom(from);
    // setMmsVersion(mmsVersion);
    // setTransactionId(transactionId);
}

SendReq::SendReq(
    /* [in] */ IPduHeaders* headers)
{
    // ==================before translated======================
    // super(headers);
}

SendReq::SendReq(
    /* [in] */ IPduHeaders* headers,
    /* [in] */ IPduBody* body)
{
    // ==================before translated======================
    // super(headers, body);
}

ECode SendReq::GetBcc(
    /* [out] */ ArrayOf<IEncodedStringValue*>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getEncodedStringValues(PduHeaders.BCC);
    assert(0);
    return NOERROR;
}

ECode SendReq::AddBcc(
    /* [in] */ IEncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.appendEncodedStringValue(value, PduHeaders.BCC);
    assert(0);
    return NOERROR;
}

ECode SendReq::SetBcc(
    /* [in] */ ArrayOf<IEncodedStringValue*>* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setEncodedStringValues(value, PduHeaders.BCC);
    assert(0);
    return NOERROR;
}

ECode SendReq::GetCc(
    /* [out] */ ArrayOf<IEncodedStringValue*>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getEncodedStringValues(PduHeaders.CC);
    assert(0);
    return NOERROR;
}

ECode SendReq::AddCc(
    /* [in] */ IEncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.appendEncodedStringValue(value, PduHeaders.CC);
    assert(0);
    return NOERROR;
}

ECode SendReq::SetCc(
    /* [in] */ ArrayOf<IEncodedStringValue*>* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setEncodedStringValues(value, PduHeaders.CC);
    assert(0);
    return NOERROR;
}

ECode SendReq::GetContentType(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getTextString(PduHeaders.CONTENT_TYPE);
    assert(0);
    return NOERROR;
}

ECode SendReq::SetContentType(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setTextString(value, PduHeaders.CONTENT_TYPE);
    assert(0);
    return NOERROR;
}

ECode SendReq::GetDeliveryReport(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getOctet(PduHeaders.DELIVERY_REPORT);
    assert(0);
    return NOERROR;
}

ECode SendReq::SetDeliveryReport(
    /* [in] */ Int32 value)
{
    // ==================before translated======================
    // mPduHeaders.setOctet(value, PduHeaders.DELIVERY_REPORT);
    assert(0);
    return NOERROR;
}

ECode SendReq::GetExpiry(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getLongInteger(PduHeaders.EXPIRY);
    assert(0);
    return NOERROR;
}

ECode SendReq::SetExpiry(
    /* [in] */ Int64 value)
{
    // ==================before translated======================
    // mPduHeaders.setLongInteger(value, PduHeaders.EXPIRY);
    assert(0);
    return NOERROR;
}

ECode SendReq::GetMessageSize(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getLongInteger(PduHeaders.MESSAGE_SIZE);
    assert(0);
    return NOERROR;
}

ECode SendReq::SetMessageSize(
    /* [in] */ Int64 value)
{
    // ==================before translated======================
    // mPduHeaders.setLongInteger(value, PduHeaders.MESSAGE_SIZE);
    assert(0);
    return NOERROR;
}

ECode SendReq::GetMessageClass(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getTextString(PduHeaders.MESSAGE_CLASS);
    assert(0);
    return NOERROR;
}

ECode SendReq::SetMessageClass(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setTextString(value, PduHeaders.MESSAGE_CLASS);
    assert(0);
    return NOERROR;
}

ECode SendReq::GetReadReport(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getOctet(PduHeaders.READ_REPORT);
    assert(0);
    return NOERROR;
}

ECode SendReq::SetReadReport(
    /* [in] */ Int32 value)
{
    // ==================before translated======================
    // mPduHeaders.setOctet(value, PduHeaders.READ_REPORT);
    assert(0);
    return NOERROR;
}

ECode SendReq::SetTo(
    /* [in] */ ArrayOf<IEncodedStringValue*>* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setEncodedStringValues(value, PduHeaders.TO);
    assert(0);
    return NOERROR;
}

ECode SendReq::GetTransactionId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getTextString(PduHeaders.TRANSACTION_ID);
    assert(0);
    return NOERROR;
}

ECode SendReq::SetTransactionId(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setTextString(value, PduHeaders.TRANSACTION_ID);
    assert(0);
    return NOERROR;
}

AutoPtr<ArrayOf<Byte> > SendReq::GenerateTransactionId()
{
    // ==================before translated======================
    // String transactionId = "T" + Long.toHexString(System.currentTimeMillis());
    // return transactionId.getBytes();
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
