#include "Elastos.Droid.Google.h"
#include "elastos/droid/google/mms/pdu/GenericPdu.h"
#include "elastos/droid/google/mms/pdu/PduHeaders.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                              GenericPdu
//=====================================================================
CAR_INTERFACE_IMPL(GenericPdu, Object, IGenericPdu);

GenericPdu::GenericPdu()
{
    constructor();
}

ECode GenericPdu::constructor()
{
    mPduHeaders = new PduHeaders();
    return NOERROR;
}

ECode GenericPdu::constructor(
    /* [in] */ IPduHeaders* headers)
{
    mPduHeaders = headers;
    return NOERROR;
}

ECode GenericPdu::GetPduHeaders(
    /* [out] */ IPduHeaders** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPduHeaders;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode GenericPdu::GetMessageType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetOctet(IPduHeaders::MESSAGE_TYPE, result);
}

ECode GenericPdu::SetMessageType(
    /* [in] */ Int32 value)
{
    return mPduHeaders->SetOctet(value, IPduHeaders::MESSAGE_TYPE);
}

ECode GenericPdu::GetMmsVersion(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetOctet(IPduHeaders::MMS_VERSION, result);
}

ECode GenericPdu::SetMmsVersion(
    /* [in] */ Int32 value)
{
    return mPduHeaders->SetOctet(value, IPduHeaders::MMS_VERSION);
}

ECode GenericPdu::GetFrom(
    /* [out] */ IEncodedStringValue** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetEncodedStringValue(IPduHeaders::FROM, result);
}

ECode GenericPdu::SetFrom(
    /* [in] */ IEncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    return mPduHeaders->SetEncodedStringValue(value, IPduHeaders::FROM);
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
