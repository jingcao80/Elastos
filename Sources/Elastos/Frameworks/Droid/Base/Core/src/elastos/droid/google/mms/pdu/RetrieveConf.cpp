#include "Elastos.Droid.Google.h"
#include "elastos/droid/google/mms/pdu/RetrieveConf.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                             RetrieveConf
//=====================================================================
CAR_INTERFACE_IMPL(RetrieveConf, MultimediaMessagePdu, IRetrieveConf);

RetrieveConf::RetrieveConf()
{
    // ==================before translated======================
    // super();
    // setMessageType(PduHeaders.MESSAGE_TYPE_RETRIEVE_CONF);
    // setTransactionId(generateTransactionId());
}

RetrieveConf::RetrieveConf(
    /* [in] */ IPduHeaders* headers)
{
    // ==================before translated======================
    // super(headers);
    // setTransactionId(generateTransactionId());
}

RetrieveConf::RetrieveConf(
    /* [in] */ IPduHeaders* headers,
    /* [in] */ IPduBody* body)
{
    // ==================before translated======================
    // super(headers, body);
    // setTransactionId(generateTransactionId());
}

ECode RetrieveConf::GetCc(
    /* [out] */ ArrayOf<IEncodedStringValue*>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getEncodedStringValues(PduHeaders.CC);
    assert(0);
    return NOERROR;
}

ECode RetrieveConf::AddCc(
    /* [in] */ IEncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.appendEncodedStringValue(value, PduHeaders.CC);
    assert(0);
    return NOERROR;
}

ECode RetrieveConf::GetContentType(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getTextString(PduHeaders.CONTENT_TYPE);
    assert(0);
    return NOERROR;
}

ECode RetrieveConf::SetContentType(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setTextString(value, PduHeaders.CONTENT_TYPE);
    assert(0);
    return NOERROR;
}

ECode RetrieveConf::GetDeliveryReport(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getOctet(PduHeaders.DELIVERY_REPORT);
    assert(0);
    return NOERROR;
}

ECode RetrieveConf::SetDeliveryReport(
    /* [in] */ Int32 value)
{
    // ==================before translated======================
    // mPduHeaders.setOctet(value, PduHeaders.DELIVERY_REPORT);
    assert(0);
    return NOERROR;
}

ECode RetrieveConf::GetFrom(
    /* [out] */ IEncodedStringValue** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getEncodedStringValue(PduHeaders.FROM);
    assert(0);
    return NOERROR;
}

ECode RetrieveConf::SetFrom(
    /* [in] */ IEncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setEncodedStringValue(value, PduHeaders.FROM);
    assert(0);
    return NOERROR;
}

ECode RetrieveConf::GetMessageClass(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getTextString(PduHeaders.MESSAGE_CLASS);
    assert(0);
    return NOERROR;
}

ECode RetrieveConf::SetMessageClass(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setTextString(value, PduHeaders.MESSAGE_CLASS);
    assert(0);
    return NOERROR;
}

ECode RetrieveConf::GetMessageId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getTextString(PduHeaders.MESSAGE_ID);
    assert(0);
    return NOERROR;
}

ECode RetrieveConf::SetMessageId(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setTextString(value, PduHeaders.MESSAGE_ID);
    assert(0);
    return NOERROR;
}

ECode RetrieveConf::GetReadReport(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getOctet(PduHeaders.READ_REPORT);
    assert(0);
    return NOERROR;
}

ECode RetrieveConf::SetReadReport(
    /* [in] */ Int32 value)
{
    // ==================before translated======================
    // mPduHeaders.setOctet(value, PduHeaders.READ_REPORT);
    assert(0);
    return NOERROR;
}

ECode RetrieveConf::GetRetrieveStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getOctet(PduHeaders.RETRIEVE_STATUS);
    assert(0);
    return NOERROR;
}

ECode RetrieveConf::SetRetrieveStatus(
    /* [in] */ Int32 value)
{
    // ==================before translated======================
    // mPduHeaders.setOctet(value, PduHeaders.RETRIEVE_STATUS);
    assert(0);
    return NOERROR;
}

ECode RetrieveConf::GetRetrieveText(
    /* [out] */ IEncodedStringValue** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getEncodedStringValue(PduHeaders.RETRIEVE_TEXT);
    assert(0);
    return NOERROR;
}

ECode RetrieveConf::SetRetrieveText(
    /* [in] */ IEncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setEncodedStringValue(value, PduHeaders.RETRIEVE_TEXT);
    assert(0);
    return NOERROR;
}

ECode RetrieveConf::GetTransactionId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getTextString(PduHeaders.TRANSACTION_ID);
    assert(0);
    return NOERROR;
}

ECode RetrieveConf::SetTransactionId(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setTextString(value, PduHeaders.TRANSACTION_ID);
    assert(0);
    return NOERROR;
}

AutoPtr<ArrayOf<Byte> > RetrieveConf::GenerateTransactionId()
{
    // ==================before translated======================
    // String transactionId = "T" + Long.toHexString(System.currentTimeMillis());
    // return transactionId.getBytes();
    assert(0);
    AutoPtr<ArrayOf<Byte> > empty;
    return empty;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
