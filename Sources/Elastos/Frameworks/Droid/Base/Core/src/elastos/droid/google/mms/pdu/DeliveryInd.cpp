#include "Elastos.Droid.Google.h"
#include "elastos/droid/google/mms/pdu/DeliveryInd.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                             DeliveryInd
//=====================================================================
CAR_INTERFACE_IMPL(DeliveryInd, GenericPdu, IDeliveryInd);

DeliveryInd::DeliveryInd()
{
    // ==================before translated======================
    // super();
    // setMessageType(PduHeaders.MESSAGE_TYPE_DELIVERY_IND);
}

DeliveryInd::DeliveryInd(
    /* [in] */ IPduHeaders* headers)
{
    // ==================before translated======================
    // super(headers);
}

ECode DeliveryInd::GetDate(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getLongInteger(PduHeaders.DATE);
    assert(0);
    return NOERROR;
}

ECode DeliveryInd::SetDate(
    /* [in] */ Int64 value)
{
    // ==================before translated======================
    // mPduHeaders.setLongInteger(value, PduHeaders.DATE);
    assert(0);
    return NOERROR;
}

ECode DeliveryInd::GetMessageId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getTextString(PduHeaders.MESSAGE_ID);
    assert(0);
    return NOERROR;
}

ECode DeliveryInd::SetMessageId(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setTextString(value, PduHeaders.MESSAGE_ID);
    assert(0);
    return NOERROR;
}

ECode DeliveryInd::GetStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getOctet(PduHeaders.STATUS);
    assert(0);
    return NOERROR;
}

ECode DeliveryInd::SetStatus(
    /* [in] */ Int32 value)
{
    // ==================before translated======================
    // mPduHeaders.setOctet(value, PduHeaders.STATUS);
    assert(0);
    return NOERROR;
}

ECode DeliveryInd::GetTo(
    /* [out] */ ArrayOf<EncodedStringValue*>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getEncodedStringValues(PduHeaders.TO);
    assert(0);
    return NOERROR;
}

ECode DeliveryInd::SetTo(
    /* [in] */ ArrayOf<EncodedStringValue*>* value)
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
