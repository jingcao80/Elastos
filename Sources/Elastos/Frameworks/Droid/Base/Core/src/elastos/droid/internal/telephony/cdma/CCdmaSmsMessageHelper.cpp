#include "elastos/droid/internal/telephony/cdma/CCdmaSmsMessageHelper.h"
#include "elastos/droid/internal/telephony/cdma/CDMASmsMessage.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

CAR_INTERFACE_IMPL(CCdmaSmsMessageHelper, Singleton, ISmsMessageHelper)
CAR_SINGLETON_IMPL(CCdmaSmsMessageHelper)


ECode CCdmaSmsMessageHelper::CreateFromPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [out] */ ISmsMessage** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISmsMessage> sm = CDMASmsMessage::CreateFromPdu(pdu);
    *result = sm;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CCdmaSmsMessageHelper::NewFromParcel(
    /* [in] */ IParcel* p,
    /* [out] */ ISmsMessage** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISmsMessage> sm = CDMASmsMessage::NewFromParcel(p);
    *result = sm;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CCdmaSmsMessageHelper::CreateFromEfRecord(
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ ISmsMessage** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISmsMessage> sm = CDMASmsMessage::CreateFromEfRecord(index, data);
    *result = sm;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CCdmaSmsMessageHelper::GetTPLayerLengthForPDU(
    /* [in] */ const String& pdu,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CDMASmsMessage::GetTPLayerLengthForPDU(pdu);
    return NOERROR;
}

ECode CCdmaSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& scAddr,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ ISmsHeader* smsHeader,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<CDMASmsMessage::SubmitPdu> sp = CDMASmsMessage::GetSubmitPdu(scAddr, destAddr, message, statusReportRequested, smsHeader);
    *result = sp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CCdmaSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& scAddr,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ ISmsHeader* smsHeader,
    /* [in] */ Int32 priority,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<CDMASmsMessage::SubmitPdu> sp = CDMASmsMessage::GetSubmitPdu(scAddr, destAddr, message, statusReportRequested, smsHeader, priority);
    *result = sp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CCdmaSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& scAddr,
    /* [in] */ const String& destAddr,
    /* [in] */ Int32 destPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Boolean statusReportRequested,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<CDMASmsMessage::SubmitPdu> sp = CDMASmsMessage::GetSubmitPdu(scAddr, destAddr, destPort, data, statusReportRequested);
    *result = sp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CCdmaSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& scAddr,
    /* [in] */ const String& destAddr,
    /* [in] */ Int32 destPort,
    /* [in] */ Int32 origPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Boolean statusReportRequested,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<CDMASmsMessage::SubmitPdu> sp = CDMASmsMessage::GetSubmitPdu(scAddr, destAddr, destPort, origPort, data, statusReportRequested);
    *result = sp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CCdmaSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& destAddr,
    /* [in] */ IUserData* userData,
    /* [in] */ Boolean statusReportRequested,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<CDMASmsMessage::SubmitPdu> sp = CDMASmsMessage::GetSubmitPdu(destAddr, userData, statusReportRequested);
    *result = sp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CCdmaSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& destAddr,
    /* [in] */ IUserData* userData,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ Int32 priority,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<CDMASmsMessage::SubmitPdu> sp = CDMASmsMessage::GetSubmitPdu(destAddr, userData, statusReportRequested, priority);
    *result = sp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CCdmaSmsMessageHelper::CalculateLength(
    /* [in] */ ICharSequence* messageBody,
    /* [in] */ Boolean use7bitOnly,
    /* [out] */ IGsmAlphabetTextEncodingDetails** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IGsmAlphabetTextEncodingDetails> gated = CDMASmsMessage::CalculateLength(messageBody, use7bitOnly);
    *result = gated;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
