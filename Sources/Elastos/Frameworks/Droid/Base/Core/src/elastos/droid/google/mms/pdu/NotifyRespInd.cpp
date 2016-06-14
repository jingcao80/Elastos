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

NotifyRespInd::NotifyRespInd(
    /* [in] */ Int32 mmsVersion,
    /* [in] */ ArrayOf<Byte>* transactionId,
    /* [in] */ Int32 status)
{
    // ==================before translated======================
    // super();
    // setMessageType(PduHeaders.MESSAGE_TYPE_NOTIFYRESP_IND);
    // setMmsVersion(mmsVersion);
    // setTransactionId(transactionId);
    // setStatus(status);
}

NotifyRespInd::NotifyRespInd(
    /* [in] */ IPduHeaders* headers)
{
    // ==================before translated======================
    // super(headers);
}

ECode NotifyRespInd::GetReportAllowed(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getOctet(PduHeaders.REPORT_ALLOWED);
    assert(0);
    return NOERROR;
}

ECode NotifyRespInd::SetReportAllowed(
    /* [in] */ Int32 value)
{
    // ==================before translated======================
    // mPduHeaders.setOctet(value, PduHeaders.REPORT_ALLOWED);
    assert(0);
    return NOERROR;
}

ECode NotifyRespInd::SetStatus(
    /* [in] */ Int32 value)
{
    // ==================before translated======================
    // mPduHeaders.setOctet(value, PduHeaders.STATUS);
    assert(0);
    return NOERROR;
}

ECode NotifyRespInd::GetStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getOctet(PduHeaders.STATUS);
    assert(0);
    return NOERROR;
}

ECode NotifyRespInd::GetTransactionId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getTextString(PduHeaders.TRANSACTION_ID);
    assert(0);
    return NOERROR;
}

ECode NotifyRespInd::SetTransactionId(
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
