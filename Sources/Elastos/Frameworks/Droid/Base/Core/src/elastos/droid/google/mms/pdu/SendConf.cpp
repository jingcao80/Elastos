
#include "Elastos.Droid.Google.h"
#include "elastos/droid/google/mms/pdu/SendConf.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                               SendConf
//=====================================================================
CAR_INTERFACE_IMPL(SendConf, GenericPdu, ISendConf);

SendConf::SendConf()
{
    // ==================before translated======================
    // super();
    // setMessageType(PduHeaders.MESSAGE_TYPE_SEND_CONF);
}

SendConf::SendConf(
    /* [in] */ IPduHeaders* headers)
{
    // ==================before translated======================
    // super(headers);
}

ECode SendConf::GetMessageId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getTextString(PduHeaders.MESSAGE_ID);
    assert(0);
    return NOERROR;
}

ECode SendConf::SetMessageId(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setTextString(value, PduHeaders.MESSAGE_ID);
    assert(0);
    return NOERROR;
}

ECode SendConf::GetResponseStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getOctet(PduHeaders.RESPONSE_STATUS);
    assert(0);
    return NOERROR;
}

ECode SendConf::SetResponseStatus(
    /* [in] */ Int32 value)
{
    // ==================before translated======================
    // mPduHeaders.setOctet(value, PduHeaders.RESPONSE_STATUS);
    assert(0);
    return NOERROR;
}

ECode SendConf::GetTransactionId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getTextString(PduHeaders.TRANSACTION_ID);
    assert(0);
    return NOERROR;
}

ECode SendConf::SetTransactionId(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setTextString(value, PduHeaders.TRANSACTION_ID);
    assert(0);
    return NOERROR;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
