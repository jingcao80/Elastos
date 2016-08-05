
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
}

ECode SendConf::constructor()
{
    GenericPdu::constructor();
    SetMessageType(IPduHeaders::MESSAGE_TYPE_SEND_CONF);
    return NOERROR;
}

ECode SendConf::constructor(
    /* [in] */ IPduHeaders* headers)
{
    GenericPdu::constructor(headers);
    return NOERROR;
}

ECode SendConf::GetMessageId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetTextString(IPduHeaders::MESSAGE_ID, result);
}

ECode SendConf::SetMessageId(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    mPduHeaders->SetTextString(value, IPduHeaders::MESSAGE_ID);
    return NOERROR;
}

ECode SendConf::GetResponseStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetOctet(IPduHeaders::RESPONSE_STATUS, result);
}

ECode SendConf::SetResponseStatus(
    /* [in] */ Int32 value)
{
    mPduHeaders->SetOctet(value, IPduHeaders::RESPONSE_STATUS);
    return NOERROR;
}

ECode SendConf::GetTransactionId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetTextString(IPduHeaders::TRANSACTION_ID, result);
}

ECode SendConf::SetTransactionId(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    mPduHeaders->SetTextString(value, IPduHeaders::TRANSACTION_ID);
    return NOERROR;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
