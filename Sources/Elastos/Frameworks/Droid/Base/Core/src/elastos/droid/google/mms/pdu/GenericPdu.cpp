#include "Elastos.Droid.Google.h"
#include "elastos/droid/google/mms/pdu/GenericPdu.h"

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
    // ==================before translated======================
    // mPduHeaders = new PduHeaders();
}

GenericPdu::GenericPdu(
    /* [in] */ IPduHeaders* headers)
{
    // ==================before translated======================
    // mPduHeaders = headers;
}

ECode GenericPdu::GetPduHeaders(
    /* [out] */ IPduHeaders** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders;
    assert(0);
    return NOERROR;
}

ECode GenericPdu::GetMessageType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getOctet(PduHeaders.MESSAGE_TYPE);
    assert(0);
    return NOERROR;
}

ECode GenericPdu::SetMessageType(
    /* [in] */ Int32 value)
{
    // ==================before translated======================
    // mPduHeaders.setOctet(value, PduHeaders.MESSAGE_TYPE);
    assert(0);
    return NOERROR;
}

ECode GenericPdu::GetMmsVersion(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getOctet(PduHeaders.MMS_VERSION);
    assert(0);
    return NOERROR;
}

ECode GenericPdu::SetMmsVersion(
    /* [in] */ Int32 value)
{
    // ==================before translated======================
    // mPduHeaders.setOctet(value, PduHeaders.MMS_VERSION);
    assert(0);
    return NOERROR;
}

ECode GenericPdu::GetFrom(
    /* [out] */ EncodedStringValue** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getEncodedStringValue(PduHeaders.FROM);
    assert(0);
    return NOERROR;
}

ECode GenericPdu::SetFrom(
    /* [in] */ EncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setEncodedStringValue(value, PduHeaders.FROM);
    assert(0);
    return NOERROR;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
