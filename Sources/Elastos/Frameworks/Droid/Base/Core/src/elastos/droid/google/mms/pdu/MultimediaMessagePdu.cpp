#include "Elastos.Droid.Google.h"
#include "elastos/droid/google/mms/pdu/MultimediaMessagePdu.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                         MultimediaMessagePdu
//=====================================================================
CAR_INTERFACE_IMPL(MultimediaMessagePdu, GenericPdu, IMultimediaMessagePdu);

MultimediaMessagePdu::MultimediaMessagePdu()
{
    // ==================before translated======================
    // super();
}

MultimediaMessagePdu::MultimediaMessagePdu(
    /* [in] */ IPduHeaders* header,
    /* [in] */ IPduBody* body)
{
    // ==================before translated======================
    // super(header);
    // mMessageBody = body;
}

MultimediaMessagePdu::MultimediaMessagePdu(
    /* [in] */ IPduHeaders* headers)
{
    // ==================before translated======================
    // super(headers);
}

ECode MultimediaMessagePdu::GetBody(
    /* [out] */ IPduBody** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mMessageBody;
    assert(0);
    return NOERROR;
}

ECode MultimediaMessagePdu::SetBody(
    /* [in] */ IPduBody* body)
{
    VALIDATE_NOT_NULL(body);
    // ==================before translated======================
    // mMessageBody = body;
    assert(0);
    return NOERROR;
}

ECode MultimediaMessagePdu::GetSubject(
    /* [out] */ IEncodedStringValue** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getEncodedStringValue(PduHeaders.SUBJECT);
    assert(0);
    return NOERROR;
}

ECode MultimediaMessagePdu::SetSubject(
    /* [in] */ IEncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.setEncodedStringValue(value, PduHeaders.SUBJECT);
    assert(0);
    return NOERROR;
}

ECode MultimediaMessagePdu::GetTo(
    /* [out] */ ArrayOf<IEncodedStringValue*>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getEncodedStringValues(PduHeaders.TO);
    assert(0);
    return NOERROR;
}

ECode MultimediaMessagePdu::AddTo(
    /* [in] */ IEncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // mPduHeaders.appendEncodedStringValue(value, PduHeaders.TO);
    assert(0);
    return NOERROR;
}

ECode MultimediaMessagePdu::GetPriority(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getOctet(PduHeaders.PRIORITY);
    assert(0);
    return NOERROR;
}

ECode MultimediaMessagePdu::SetPriority(
    /* [in] */ Int32 value)
{
    // ==================before translated======================
    // mPduHeaders.setOctet(value, PduHeaders.PRIORITY);
    assert(0);
    return NOERROR;
}

ECode MultimediaMessagePdu::GetDate(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPduHeaders.getLongInteger(PduHeaders.DATE);
    assert(0);
    return NOERROR;
}

ECode MultimediaMessagePdu::SetDate(
    /* [in] */ Int64 value)
{
    // ==================before translated======================
    // mPduHeaders.setLongInteger(value, PduHeaders.DATE);
    assert(0);
    return NOERROR;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
