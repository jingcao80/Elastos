#include "elastos/droid/telephony/gsm/CGSMSmsMessageHelper.h"

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Gsm {

CAR_INTERFACE_IMPL(CGSMSmsMessageHelper, Singleton, ISmsMessageHelper)

CAR_SINGLETON_IMPL(CGSMSmsMessageHelper)

ECode CGSMSmsMessageHelper::CreateFromPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [out] */ ISmsMessage** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<GSMSmsMessage> sm = GSMSmsMessage::CreateFromPdu(pdu);
    *result = sm;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CGSMSmsMessageHelper::GetTPLayerLengthForPDU(
    /* [in] */ const String& pdu,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = GSMSmsMessage::GetTPLayerLengthForPDU(pdu);
    return NOERROR;
}

ECode CGSMSmsMessageHelper::CalculateLength(
    /* [in] */ ICharSequence* messageBody,
    /* [in] */ Boolean use7bitOnly,
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Int32> > cl = GSMSmsMessage::CalculateLength(messageBody, use7bitOnly);
    *result = cl;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CGSMSmsMessageHelper::CalculateLength(
    /* [in] */ const String& messageBody,
    /* [in] */ Boolean use7bitOnly,
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Int32> > cl = GSMSmsMessage::CalculateLength(messageBody, use7bitOnly);
    *result = cl;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CGSMSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ ArrayOf<Byte>* header,
    /* [out] */ ISubmitPdu** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<GSMSmsMessage::SubmitPdu> sp = GSMSmsMessage::GetSubmitPdu(scAddress, destinationAddress, message, statusReportRequested, header);
    *result = sp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}
ECode CGSMSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [out] */ ISubmitPdu** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<GSMSmsMessage::SubmitPdu> sp = GSMSmsMessage::GetSubmitPdu(scAddress, destinationAddress, message, statusReportRequested);
    *result = sp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CGSMSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ Int16 destinationPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Boolean statusReportRequested,
    /* [out] */ ISubmitPdu** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<GSMSmsMessage::SubmitPdu> sp = GSMSmsMessage::GetSubmitPdu(scAddress, destinationAddress, destinationPort, data, statusReportRequested);
    *result = sp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}


} // namespace Gsm
} // namespace Telephony
} // namespace Droid
} // namespace Elastos
