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

AcknowledgeInd::AcknowledgeInd(
    /* [in] */ Int32 mmsVersion,
    /* [in] */ ArrayOf<Byte>* transactionId)
{
    // ==================before translated======================
    // super();
    //
    // setMessageType(PduHeaders.MESSAGE_TYPE_ACKNOWLEDGE_IND);
    // setMmsVersion(mmsVersion);
    // setTransactionId(transactionId);
}

AcknowledgeInd::AcknowledgeInd(
    /* [in] */ IPduHeaders* headers)
{
    // ==================before translated======================
    // super(headers);
}

ECode AcknowledgeInd::GetReportAllowed(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getOctet(PduHeaders.REPORT_ALLOWED);
    assert(0);
    return NOERROR;
}

ECode AcknowledgeInd::SetReportAllowed(
    /* [in] */ Int32 value)
{
    // ==================before translated======================
    // mPduHeaders.setOctet(value, PduHeaders.REPORT_ALLOWED);
    assert(0);
    return NOERROR;
}

ECode AcknowledgeInd::GetTransactionId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getTextString(PduHeaders.TRANSACTION_ID);
    assert(0);
    return NOERROR;
}

ECode AcknowledgeInd::SetTransactionId(
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
