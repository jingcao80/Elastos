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
}

ECode DeliveryInd::constructor()
{
    GenericPdu::constructor();
    SetMessageType(IPduHeaders::MESSAGE_TYPE_DELIVERY_IND);
    return NOERROR;
}

ECode DeliveryInd::constructor(
    /* [in] */ IPduHeaders* headers)
{
    GenericPdu::constructor(headers);
    return NOERROR;
}

ECode DeliveryInd::GetDate(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetLongInteger(IPduHeaders::DATE, result);
}

ECode DeliveryInd::SetDate(
    /* [in] */ Int64 value)
{
    return mPduHeaders->SetLongInteger(value, IPduHeaders::DATE);
}

ECode DeliveryInd::GetMessageId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetTextString(IPduHeaders::MESSAGE_ID, result);
}

ECode DeliveryInd::SetMessageId(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    return mPduHeaders->SetTextString(value, IPduHeaders::MESSAGE_ID);
}

ECode DeliveryInd::GetStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetOctet(IPduHeaders::STATUS, result);
}

ECode DeliveryInd::SetStatus(
    /* [in] */ Int32 value)
{
    return mPduHeaders->SetOctet(value, IPduHeaders::STATUS);
}

ECode DeliveryInd::GetTo(
    /* [out] */ ArrayOf<IEncodedStringValue*>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetEncodedStringValues(IPduHeaders::TO, result);
}

ECode DeliveryInd::SetTo(
    /* [in] */ ArrayOf<IEncodedStringValue*>* value)
{
    VALIDATE_NOT_NULL(value);
    return mPduHeaders->SetEncodedStringValues(value, IPduHeaders::TO);
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
