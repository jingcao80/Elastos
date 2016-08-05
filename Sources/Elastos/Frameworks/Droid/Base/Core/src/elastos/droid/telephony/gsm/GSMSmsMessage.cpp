#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cdma/CDMASmsMessage.h"
#include "elastos/droid/internal/telephony/CGsmAlphabetTextEncodingDetails.h"
#include "elastos/droid/internal/telephony/gsm/SmsMessage.h"
#include "elastos/droid/internal/telephony/SmsHeader.h"
#include "elastos/droid/internal/telephony/SyntheticSmsMessage.h"
#include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/telephony/gsm/GSMSmsMessage.h"
#include "elastos/droid/telephony/SmsMessage.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/Arrays.h>

using Elastos::Droid::Internal::Telephony::CGsmAlphabetTextEncodingDetails;
using Elastos::Droid::Internal::Telephony::IGsmAlphabetTextEncodingDetails;
using Elastos::Droid::Internal::Telephony::ISmsHeader;
using Elastos::Droid::Internal::Telephony::SmsHeader;
using Elastos::Droid::Internal::Telephony::SyntheticSmsMessage;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::SmsMessage;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Gsm {

//=====================================================================
//                        GSMSmsMessage::SubmitPdu
//=====================================================================
CAR_INTERFACE_IMPL(GSMSmsMessage::SubmitPdu, Object, ISubmitPdu);

GSMSmsMessage::SubmitPdu::SubmitPdu()
{
}

ECode GSMSmsMessage::SubmitPdu::constructor()
{
    return NOERROR;
}

ECode GSMSmsMessage::SubmitPdu::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("SubmitPdu: encodedScAddress = ")
            + Arrays::ToString(encodedScAddress)
            + ", encodedMessage = "
            + Arrays::ToString(encodedMessage);
    return NOERROR;
}

GSMSmsMessage::SubmitPdu::SubmitPdu(
   /* [in] */ ISmsMessageBaseSubmitPduBase* spb)
{
// TODO: Need Elastos::Droid::Internal::Telephony::SmsMessageBase::SubmitPduBase
    // AutoPtr<Elastos::Droid::Internal::Telephony::SmsMessageBase::SubmitPduBase> sp
    //          = (Elastos::Droid::Internal::Telephony::SmsMessageBase::SubmitPduBase*)spb;
    // this->encodedMessage = sp->encodedMessage;
    // this->encodedScAddress = sp->encodedScAddress;
}

//=====================================================================
//                              GSMSmsMessage
//=====================================================================
CAR_INTERFACE_IMPL(GSMSmsMessage, Object, ISmsMessage);

GSMSmsMessage::GSMSmsMessage()
{
}

ECode GSMSmsMessage::constructor()
{
    return constructor(GetSmsFacility());
}

AutoPtr<GSMSmsMessage> GSMSmsMessage::CreateFromPdu(
    /* [in] */ ArrayOf<Byte>* pdu)
{
    AutoPtr<ISmsMessageBase> wrappedMessage;
    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    Int32 activePhone;
    tm->GetCurrentPhoneType(&activePhone);

    if (ITelephonyManager::PHONE_TYPE_CDMA == activePhone) {
        wrappedMessage = ISmsMessageBase::Probe(
                Elastos::Droid::Internal::Telephony::Cdma::CDMASmsMessage::CreateFromPdu(pdu));
    }
    else {
        AutoPtr<Elastos::Droid::Internal::Telephony::Gsm::ISmsMessage> sm;
        Elastos::Droid::Internal::Telephony::Gsm::SmsMessage::CreateFromPdu(pdu,
                (Elastos::Droid::Internal::Telephony::Gsm::ISmsMessage**)&sm);
        wrappedMessage = ISmsMessageBase::Probe(sm);
    }

    // Hangouts, and some other bad citizens, are using this deprecated API call.
    // Make sure that WhisperPush and Voice+ integration properly work,
    // as this may be a synthetic message.
    if (wrappedMessage == NULL) {
        // returns null if it is not synthetic. (likely malformed)
        wrappedMessage = ISmsMessageBase::Probe(SyntheticSmsMessage::CreateFromPdu(pdu));
    }

    AutoPtr<GSMSmsMessage> ret = new GSMSmsMessage();
    ret->constructor(wrappedMessage);
    return ret;
}

Int32 GSMSmsMessage::GetTPLayerLengthForPDU(
    /* [in] */ const String& pdu)
{
    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    Int32 activePhone;
    tm->GetCurrentPhoneType(&activePhone);

    if (ITelephonyManager::PHONE_TYPE_CDMA == activePhone) {
        return Elastos::Droid::Internal::Telephony::Cdma::CDMASmsMessage::GetTPLayerLengthForPDU(pdu);
    }
    else {
        Int32 ret;
        Elastos::Droid::Internal::Telephony::Gsm::SmsMessage::GetTPLayerLengthForPDU(pdu, &ret);
        return ret;
    }
}

AutoPtr< ArrayOf<Int32> > GSMSmsMessage::CalculateLength(
    /* [in] */ ICharSequence* messageBody,
    /* [in] */ Boolean use7bitOnly)
{
    AutoPtr<IGsmAlphabetTextEncodingDetails> ted;
    Elastos::Droid::Internal::Telephony::Gsm::SmsMessage::CalculateLength(
            messageBody, use7bitOnly, (IGsmAlphabetTextEncodingDetails**)&ted);
    AutoPtr<CGsmAlphabetTextEncodingDetails> cted = (CGsmAlphabetTextEncodingDetails*)ted.Get();

    AutoPtr<ArrayOf<Int32> > ret = ArrayOf<Int32>::Alloc(4);
    (*ret)[0] = cted->mMsgCount;
    (*ret)[1] = cted->mCodeUnitCount;
    (*ret)[2] = cted->mCodeUnitsRemaining;
    (*ret)[3] = cted->mCodeUnitSize;
    return ret;
}

AutoPtr< ArrayOf<Int32> > GSMSmsMessage::CalculateLength(
    /* [in] */ const String& messageBody,
    /* [in] */ Boolean use7bitOnly)
{
    return CalculateLength(CoreUtils::Convert(messageBody), use7bitOnly);
}

AutoPtr<GSMSmsMessage::SubmitPdu> GSMSmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ ArrayOf<Byte>* header)
{
    AutoPtr<ISmsMessageBaseSubmitPduBase> spb;
    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    Int32 activePhone;
    tm->GetCurrentPhoneType(&activePhone);

    if (ITelephonyManager::PHONE_TYPE_CDMA == activePhone) {
        AutoPtr<ISmsHeader> sh;
        SmsHeader::FromByteArray(header, (ISmsHeader**)&sh);
        spb = ISmsMessageBaseSubmitPduBase::Probe(
                Elastos::Droid::Internal::Telephony::Cdma::CDMASmsMessage::GetSubmitPdu(
                scAddress, destinationAddress, message, statusReportRequested, sh));
    }
    else {
        AutoPtr<Elastos::Droid::Internal::Telephony::Gsm::ISmsMessageSubmitPdu> smsp;
        Elastos::Droid::Internal::Telephony::Gsm::SmsMessage::GetSubmitPdu(scAddress,
                destinationAddress, message, statusReportRequested, header,
                (Elastos::Droid::Internal::Telephony::Gsm::ISmsMessageSubmitPdu**)&smsp);
        spb = ISmsMessageBaseSubmitPduBase::Probe(smsp);
    }

    return new SubmitPdu(spb);
}

AutoPtr<GSMSmsMessage::SubmitPdu> GSMSmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested)
{
    AutoPtr<ISmsMessageBaseSubmitPduBase> spb;
    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    Int32 activePhone;
    tm->GetCurrentPhoneType(&activePhone);

    if (ITelephonyManager::PHONE_TYPE_CDMA == activePhone) {
        spb = ISmsMessageBaseSubmitPduBase::Probe(
                Elastos::Droid::Internal::Telephony::Cdma::CDMASmsMessage::GetSubmitPdu(
                scAddress, destinationAddress, message, statusReportRequested, NULL));
    }
    else {
        AutoPtr<Elastos::Droid::Internal::Telephony::Gsm::ISmsMessageSubmitPdu> smsp;
        Elastos::Droid::Internal::Telephony::Gsm::SmsMessage::GetSubmitPdu(scAddress,
                destinationAddress, message, statusReportRequested,
                (Elastos::Droid::Internal::Telephony::Gsm::ISmsMessageSubmitPdu**)&smsp);
        spb = ISmsMessageBaseSubmitPduBase::Probe(smsp);
    }

    return new SubmitPdu(spb);
}

AutoPtr<GSMSmsMessage::SubmitPdu> GSMSmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ Int16 destinationPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Boolean statusReportRequested)
{
    AutoPtr<ISmsMessageBaseSubmitPduBase> spb;
    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    Int32 activePhone;
    tm->GetCurrentPhoneType(&activePhone);

    if (ITelephonyManager::PHONE_TYPE_CDMA == activePhone) {
        spb = ISmsMessageBaseSubmitPduBase::Probe(
                Elastos::Droid::Internal::Telephony::Cdma::CDMASmsMessage::GetSubmitPdu(
                scAddress, destinationAddress, destinationPort, data, statusReportRequested));
    }
    else {
        AutoPtr<Elastos::Droid::Internal::Telephony::Gsm::ISmsMessageSubmitPdu> smsp;
        Elastos::Droid::Internal::Telephony::Gsm::SmsMessage::GetSubmitPdu(scAddress,
                destinationAddress, destinationPort, data, statusReportRequested,
                (Elastos::Droid::Internal::Telephony::Gsm::ISmsMessageSubmitPdu**)&smsp);
        spb = ISmsMessageBaseSubmitPduBase::Probe(smsp);
    }

    return new SubmitPdu(spb);
}

ECode GSMSmsMessage::GetServiceCenterAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetServiceCenterAddress(result);
}

ECode GSMSmsMessage::GetOriginatingAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetOriginatingAddress(result);
}

ECode GSMSmsMessage::GetDisplayOriginatingAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetDisplayOriginatingAddress(result);
}

ECode GSMSmsMessage::GetMessageBody(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetMessageBody(result);
}

ECode GSMSmsMessage::GetMessageClass(
    /* [out] */ Int32/*TODO MessageClass*/* result)
{
    VALIDATE_NOT_NULL(result);
    *result  = 0;//TODO MessageClass::MessageClass_UNKNOWN;
    // Int32 index = mWrappedSmsMessage.getMessageClass().ordinal();
    //
    // return MessageClass.values()[index];
    return NOERROR;
}

ECode GSMSmsMessage::GetDisplayMessageBody(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetDisplayMessageBody(result);
}

ECode GSMSmsMessage::GetPseudoSubject(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetPseudoSubject(result);
}

ECode GSMSmsMessage::GetTimestampMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetTimestampMillis(result);
}

ECode GSMSmsMessage::IsEmail(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->IsEmail(result);
}

ECode GSMSmsMessage::GetEmailBody(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetEmailBody(result);
}

ECode GSMSmsMessage::GetEmailFrom(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetEmailFrom(result);
}

ECode GSMSmsMessage::GetProtocolIdentifier(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetProtocolIdentifier(result);
}

ECode GSMSmsMessage::IsReplace(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->IsReplace(result);
}

ECode GSMSmsMessage::IsCphsMwiMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->IsCphsMwiMessage(result);
}

ECode GSMSmsMessage::IsMWIClearMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->IsMWIClearMessage(result);
}

ECode GSMSmsMessage::IsMWISetMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->IsMWISetMessage(result);
}

ECode GSMSmsMessage::IsMwiDontStore(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->IsMwiDontStore(result);
}

ECode GSMSmsMessage::GetUserData(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetUserData(result);
}

ECode GSMSmsMessage::GetPdu(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetPdu(result);
}

ECode GSMSmsMessage::GetStatusOnSim(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetStatusOnIcc(result);
}

ECode GSMSmsMessage::GetStatusOnIcc(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetStatusOnIcc(result);
}

ECode GSMSmsMessage::GetIndexOnSim(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetIndexOnIcc(result);
}

ECode GSMSmsMessage::GetIndexOnIcc(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetIndexOnIcc(result);
}

ECode GSMSmsMessage::GetStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetStatus(result);
}

ECode GSMSmsMessage::IsStatusReportMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->IsStatusReportMessage(result);
}

ECode GSMSmsMessage::IsReplyPathPresent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->IsReplyPathPresent(result);
}

ECode GSMSmsMessage::constructor(
    /* [in] */ ISmsMessageBase* smb)
{
    mWrappedSmsMessage = smb;
    return NOERROR;
}

AutoPtr<ISmsMessageBase> GSMSmsMessage::GetSmsFacility()
{
    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    Int32 activePhone;
    tm->GetCurrentPhoneType(&activePhone);

    if (ITelephonyManager::PHONE_TYPE_CDMA == activePhone) {
        return new Elastos::Droid::Internal::Telephony::Cdma::CDMASmsMessage();
    }
    else {
        return new Elastos::Droid::Internal::Telephony::Gsm::SmsMessage();
    }
}

} // namespace Gsm
} // namespace Telephony
} // namespace Droid
} // namespace Elastos


