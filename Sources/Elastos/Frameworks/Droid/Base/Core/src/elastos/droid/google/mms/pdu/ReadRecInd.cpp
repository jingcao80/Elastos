#include "Elastos.Droid.Google.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/google/mms/pdu/ReadRecInd.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                              ReadRecInd
//=====================================================================
CAR_INTERFACE_IMPL(ReadRecInd, GenericPdu, IReadRecInd);

ReadRecInd::ReadRecInd(
    /* [in] */ IEncodedStringValue* from,
    /* [in] */ ArrayOf<Byte>* messageId,
    /* [in] */ Int32 mmsVersion,
    /* [in] */ Int32 readStatus,
    /* [in] */ ArrayOf<IEncodedStringValue*>* to)
{
    // ==================before translated======================
    // super();
    // setMessageType(PduHeaders.MESSAGE_TYPE_READ_REC_IND);
    // setFrom(from);
    // setMessageId(messageId);
    // setMmsVersion(mmsVersion);
    // setTo(to);
    // setReadStatus(readStatus);
}

ReadRecInd::ReadRecInd(
    /* [in] */ IPduHeaders* headers)
{
    // ==================before translated======================
    // super(headers);
}

ECode ReadRecInd::GetDate(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getLongInteger(PduHeaders.DATE);
    assert(0);
    return NOERROR;
}

ECode ReadRecInd::SetDate(
    /* [in] */ Int64 value)
{
    // ==================before translated======================
    // mPduHeaders.setLongInteger(value, PduHeaders.DATE);
    assert(0);
    return NOERROR;
}

ECode ReadRecInd::GetMessageId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getTextString(PduHeaders.MESSAGE_ID);
    assert(0);
    return NOERROR;
}

ECode ReadRecInd::SetMessageId(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setTextString(value, PduHeaders.MESSAGE_ID);
    assert(0);
    return NOERROR;
}

ECode ReadRecInd::GetTo(
    /* [out] */ ArrayOf<IEncodedStringValue*>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getEncodedStringValues(PduHeaders.TO);
    assert(0);
    return NOERROR;
}

ECode ReadRecInd::SetTo(
    /* [in] */ ArrayOf<IEncodedStringValue*>* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setEncodedStringValues(value, PduHeaders.TO);
    assert(0);
    return NOERROR;
}

ECode ReadRecInd::GetReadStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getOctet(PduHeaders.READ_STATUS);
    assert(0);
    return NOERROR;
}

ECode ReadRecInd::SetReadStatus(
    /* [in] */ Int32 value)
{
    // ==================before translated======================
    // mPduHeaders.setOctet(value, PduHeaders.READ_STATUS);
    assert(0);
    return NOERROR;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
