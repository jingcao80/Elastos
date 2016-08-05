#include "Elastos.Droid.Google.h"
#include "elastos/droid/google/mms/pdu/AcknowledgeInd.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                            AcknowledgeInd
//=====================================================================
CAR_INTERFACE_IMPL(AcknowledgeInd, GenericPdu, IAcknowledgeInd);

AcknowledgeInd::AcknowledgeInd()
{
}

ECode AcknowledgeInd::constructor(
    /* [in] */ Int32 mmsVersion,
    /* [in] */ ArrayOf<Byte>* transactionId)
{
    GenericPdu::constructor();

    SetMessageType(IPduHeaders::MESSAGE_TYPE_ACKNOWLEDGE_IND);
    SetMmsVersion(mmsVersion);
    SetTransactionId(transactionId);
    return NOERROR;
}

ECode AcknowledgeInd::constructor(
    /* [in] */ IPduHeaders* headers)
{
    GenericPdu::constructor(headers);
    return NOERROR;
}

ECode AcknowledgeInd::GetReportAllowed(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetOctet(IPduHeaders::REPORT_ALLOWED, result);
}

ECode AcknowledgeInd::SetReportAllowed(
    /* [in] */ Int32 value)
{
    return mPduHeaders->SetOctet(value, IPduHeaders::REPORT_ALLOWED);
}

ECode AcknowledgeInd::GetTransactionId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetTextString(IPduHeaders::TRANSACTION_ID, result);
}

ECode AcknowledgeInd::SetTransactionId(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    return mPduHeaders->SetTextString(value, IPduHeaders::TRANSACTION_ID);
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
