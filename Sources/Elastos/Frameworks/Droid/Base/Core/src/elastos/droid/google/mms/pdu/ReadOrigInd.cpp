#include "Elastos.Droid.Google.h"
#include "elastos/droid/google/mms/pdu/ReadOrigInd.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                             ReadOrigInd
//=====================================================================
CAR_INTERFACE_IMPL(ReadOrigInd, GenericPdu, IReadOrigInd);

ReadOrigInd::ReadOrigInd()
{
    constructor();
}

ECode ReadOrigInd::constructor()
{
    // ==================before translated======================
    // super();
    // setMessageType(PduHeaders.MESSAGE_TYPE_READ_ORIG_IND);
    return NOERROR;
}

ECode ReadOrigInd::constructor(
    /* [in] */ IPduHeaders* headers)
{
    // ==================before translated======================
    // super(headers);
    return NOERROR;
}

ECode ReadOrigInd::GetDate(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getLongInteger(PduHeaders.DATE);
    assert(0);
    return NOERROR;
}

ECode ReadOrigInd::SetDate(
    /* [in] */ Int64 value)
{
    // ==================before translated======================
    // mPduHeaders.setLongInteger(value, PduHeaders.DATE);
    assert(0);
    return NOERROR;
}

ECode ReadOrigInd::GetFrom(
    /* [out] */ IEncodedStringValue** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getEncodedStringValue(PduHeaders.FROM);
    assert(0);
    return NOERROR;
}

ECode ReadOrigInd::SetFrom(
    /* [in] */ IEncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setEncodedStringValue(value, PduHeaders.FROM);
    assert(0);
    return NOERROR;
}

ECode ReadOrigInd::GetMessageId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getTextString(PduHeaders.MESSAGE_ID);
    assert(0);
    return NOERROR;
}

ECode ReadOrigInd::SetMessageId(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setTextString(value, PduHeaders.MESSAGE_ID);
    assert(0);
    return NOERROR;
}

ECode ReadOrigInd::GetReadStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getOctet(PduHeaders.READ_STATUS);
    assert(0);
    return NOERROR;
}

ECode ReadOrigInd::SetReadStatus(
    /* [in] */ Int32 value)
{
    // ==================before translated======================
    // mPduHeaders.setOctet(value, PduHeaders.READ_STATUS);
    assert(0);
    return NOERROR;
}

ECode ReadOrigInd::GetTo(
    /* [out] */ ArrayOf<IEncodedStringValue*>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getEncodedStringValues(PduHeaders.TO);
    assert(0);
    return NOERROR;
}

ECode ReadOrigInd::SetTo(
    /* [in] */ ArrayOf<IEncodedStringValue*>* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setEncodedStringValues(value, PduHeaders.TO);
    assert(0);
    return NOERROR;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
