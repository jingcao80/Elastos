#include "Elastos.Droid.Google.h"
#include "elastos/droid/google/mms/pdu/NotifyRespInd.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                            NotifyRespInd
//=====================================================================
CAR_INTERFACE_IMPL(NotifyRespInd, GenericPdu, INotifyRespInd);

NotifyRespInd::NotifyRespInd()
{
}

ECode NotifyRespInd::constructor(
    /* [in] */ Int32 mmsVersion,
    /* [in] */ ArrayOf<Byte>* transactionId,
    /* [in] */ Int32 status)
{
    GenericPdu::constructor();
    SetMessageType(IPduHeaders::MESSAGE_TYPE_NOTIFYRESP_IND);
    SetMmsVersion(mmsVersion);
    SetTransactionId(transactionId);
    SetStatus(status);
    return NOERROR;
}

ECode NotifyRespInd::constructor(
    /* [in] */ IPduHeaders* headers)
{
    GenericPdu::constructor(headers);
    return NOERROR;
}

ECode NotifyRespInd::GetReportAllowed(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetOctet(IPduHeaders::REPORT_ALLOWED, result);
}

ECode NotifyRespInd::SetReportAllowed(
    /* [in] */ Int32 value)
{
    return mPduHeaders->SetOctet(value, IPduHeaders::REPORT_ALLOWED);
}

ECode NotifyRespInd::SetStatus(
    /* [in] */ Int32 value)
{
    return mPduHeaders->SetOctet(value, IPduHeaders::STATUS);
}

ECode NotifyRespInd::GetStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetOctet(IPduHeaders::STATUS, result);
}

ECode NotifyRespInd::GetTransactionId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetTextString(IPduHeaders::TRANSACTION_ID, result);
}

ECode NotifyRespInd::SetTransactionId(
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
