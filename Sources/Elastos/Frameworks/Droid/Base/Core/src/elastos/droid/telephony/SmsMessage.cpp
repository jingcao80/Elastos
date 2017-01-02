//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/internal/telephony/cdma/CDMASmsMessage.h"
#include "elastos/droid/internal/telephony/gsm/SmsMessage.h"
#include "elastos/droid/internal/telephony/CGsmAlphabet.h"
#include "elastos/droid/internal/telephony/CGsmAlphabetTextEncodingDetails.h"
#include "elastos/droid/internal/telephony/CSms7BitEncodingTranslator.h"
#include "elastos/droid/internal/telephony/SyntheticSmsMessage.h"
#include "elastos/droid/R.h"
#include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/telephony/SmsManager.h"
#include "elastos/droid/telephony/SmsMessage.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::CGsmAlphabet;
using Elastos::Droid::Internal::Telephony::CGsmAlphabetTextEncodingDetails;
using Elastos::Droid::Internal::Telephony::CSms7BitEncodingTranslator;
using Elastos::Droid::Internal::Telephony::IGsmAlphabet;
using Elastos::Droid::Internal::Telephony::IGsmAlphabetTextEncodingDetails;
using Elastos::Droid::Internal::Telephony::ISms7BitEncodingTranslator;
using Elastos::Droid::Internal::Telephony::ISmsConstants;
using Elastos::Droid::Internal::Telephony::SyntheticSmsMessage;
using Elastos::Droid::R;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::SmsManager;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telephony {

//=====================================================================
//                        SmsMessage::SubmitPdu
//=====================================================================
CAR_INTERFACE_IMPL( SmsMessage::SubmitPdu, Object, ISmsMessageSubmitPdu);

ECode SmsMessage::SubmitPdu::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("SubmitPdu: encodedScAddress = ")
            + Arrays::ToString(encodedScAddress)
            + ", encodedMessage = "
            + Arrays::ToString(encodedMessage);
    return NOERROR;
}

SmsMessage::SubmitPdu::SubmitPdu(
    /* [in] */ ISmsMessageBaseSubmitPduBase* spb)
{
// TODO: Need Elastos::Droid::Internal::Telephony::SmsMessageBase::SubmitPduBase
    // AutoPtr<Elastos::Droid::Internal::Telephony::SmsMessageBase::SubmitPduBase> sp
    //          = (Elastos::Droid::Internal::Telephony::SmsMessageBase::SubmitPduBase*)spb;
    // this->encodedMessage = spb->encodedMessage;
    // this->encodedScAddress = spb->encodedScAddress;
}

//=====================================================================
//                    SmsMessage::NoEmsSupportConfig
//=====================================================================
SmsMessage::NoEmsSupportConfig::NoEmsSupportConfig(
    /* [in] */ ArrayOf<String>* config)
{
    mOperatorNumber = (*config)[0];
    mIsPrefix = (*config)[1].Equals("prefix");
    mGid1 = config->GetLength() > 2 ? (*config)[2] : String(NULL);
}

ECode SmsMessage::NoEmsSupportConfig::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("NoEmsSupportConfig { mOperatorNumber = ") + mOperatorNumber
            + ", mIsPrefix = " + StringUtils::BooleanToString(mIsPrefix)
            + ", mGid1 = " + mGid1 + " }";
    return NOERROR;
}

//=====================================================================
//                              SmsMessage
//=====================================================================
CAR_INTERFACE_IMPL(SmsMessage, Object, ISmsMessage);

const String SmsMessage::LOGTAG("SmsMessage");
AutoPtr< ArrayOf<SmsMessage::NoEmsSupportConfig*> > SmsMessage::mNoEmsSupportConfigList = NULL;
Boolean SmsMessage::mIsNoEmsSupportConfigListLoaded = FALSE;

ECode SmsMessage::SetSubId(
    /* [in] */ Int64 subId)
{
    mSubId = subId;
    return NOERROR;
}

ECode SmsMessage::GetSubId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSubId;
    return NOERROR;
}

AutoPtr<ISmsMessage> SmsMessage::CreateFromPdu(
    /* [in] */ ArrayOf<Byte>* pdu)
{
    AutoPtr<ISmsMessage> message;

    // cdma(3gpp2) vs gsm(3gpp) format info was not given,
    // guess from active voice phone type
    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    Int32 activePhone;
    tm->GetCurrentPhoneType(&activePhone);

    String format = (ITelephonyManager::PHONE_TYPE_CDMA == activePhone) ?
            ISmsConstants::FORMAT_3GPP2 : ISmsConstants::FORMAT_3GPP;
    if (SyntheticSmsMessage::IsSyntheticPdu(pdu)) {
        format = FORMAT_SYNTHETIC;
    }
    message = CreateFromPdu(pdu, format);

    if (NULL == message || NULL == ((SmsMessage*)message.Get())->mWrappedSmsMessage) {
        // decoding pdu failed based on activePhone type, must be other format
        format = (ITelephonyManager::PHONE_TYPE_CDMA == activePhone) ?
                ISmsConstants::FORMAT_3GPP : ISmsConstants::FORMAT_3GPP2;
        message = CreateFromPdu(pdu, format);
    }
    return message;
}

AutoPtr<ISmsMessage> SmsMessage::CreateFromPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ const String& format)
{
    AutoPtr<ISmsMessageBase> wrappedMessage;

    if (ISmsConstants::FORMAT_3GPP2.Equals(format)) {
        wrappedMessage = ISmsMessageBase::Probe(
                Elastos::Droid::Internal::Telephony::Cdma::CDMASmsMessage::CreateFromPdu(pdu));
    }
    else if (ISmsConstants::FORMAT_3GPP.Equals(format)) {
        AutoPtr<Elastos::Droid::Internal::Telephony::Gsm::ISmsMessage> sm;
        Elastos::Droid::Internal::Telephony::Gsm::SmsMessage::CreateFromPdu(pdu,
                (Elastos::Droid::Internal::Telephony::Gsm::ISmsMessage**)&sm);
        wrappedMessage = ISmsMessageBase::Probe(sm);
    }
    else if (FORMAT_SYNTHETIC.Equals(format)) {
        wrappedMessage = ISmsMessageBase::Probe(SyntheticSmsMessage::CreateFromPdu(pdu));
    }
    else {
        Logger::E(LOGTAG, "createFromPdu(): unsupported message format %s", format.string());
        return NULL;
    }

    return new SmsMessage(wrappedMessage);
}

AutoPtr<ISmsMessage> SmsMessage::NewFromCMT(
    /* [in] */ ArrayOf<String>* lines)
{
    // received SMS in 3GPP format
    AutoPtr<Elastos::Droid::Internal::Telephony::Gsm::ISmsMessage> sm;
    Elastos::Droid::Internal::Telephony::Gsm::SmsMessage::NewFromCMT(
            lines, (Elastos::Droid::Internal::Telephony::Gsm::ISmsMessage**)&sm);
    AutoPtr<ISmsMessageBase> wrappedMessage = ISmsMessageBase::Probe(sm);

    return new SmsMessage(wrappedMessage);
}

AutoPtr<ISmsMessage> SmsMessage::NewFromParcel(
    /* [in] */ IParcel* p)
{
    // received SMS in 3GPP2 format
    AutoPtr<ISmsMessageBase> wrappedMessage =
            ISmsMessageBase::Probe(Elastos::Droid::Internal::Telephony::Cdma::CDMASmsMessage::NewFromParcel(p));

    return new SmsMessage(wrappedMessage);
}

AutoPtr<ISmsMessage> SmsMessage::CreateFromEfRecord(
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Byte>* data)
{
    AutoPtr<ISmsMessageBase> wrappedMessage;

    if (IsCdmaVoice()) {
        wrappedMessage = ISmsMessageBase::Probe(
                Elastos::Droid::Internal::Telephony::Cdma::CDMASmsMessage::CreateFromEfRecord(
                index, data));
    }
    else {
        AutoPtr<Elastos::Droid::Internal::Telephony::Gsm::ISmsMessage> sm;
        Elastos::Droid::Internal::Telephony::Gsm::SmsMessage::CreateFromEfRecord(
                index, data, (Elastos::Droid::Internal::Telephony::Gsm::ISmsMessage**)&sm);
        wrappedMessage = ISmsMessageBase::Probe(sm);
    }

    return wrappedMessage != NULL ? new SmsMessage(wrappedMessage) : NULL;
}

AutoPtr<ISmsMessage> SmsMessage::CreateFromEfRecord(
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int64 subId)
{
    AutoPtr<ISmsMessageBase> wrappedMessage;

    if (IsCdmaVoice(subId)) {
        wrappedMessage = ISmsMessageBase::Probe(
                Elastos::Droid::Internal::Telephony::Cdma::CDMASmsMessage::CreateFromEfRecord(
                index, data));
    }
    else {
        AutoPtr<Elastos::Droid::Internal::Telephony::Gsm::ISmsMessage> sm;
        Elastos::Droid::Internal::Telephony::Gsm::SmsMessage::CreateFromEfRecord(
                index, data, (Elastos::Droid::Internal::Telephony::Gsm::ISmsMessage**)&sm);
        wrappedMessage = ISmsMessageBase::Probe(sm);
    }

    return wrappedMessage != NULL ? new SmsMessage(wrappedMessage) : NULL;
}

Int32 SmsMessage::GetTPLayerLengthForPDU(
    /* [in] */ const String& pdu)
{
    if (IsCdmaVoice()) {
        return Elastos::Droid::Internal::Telephony::Cdma::CDMASmsMessage::GetTPLayerLengthForPDU(pdu);
    }
    else {
        Int32 val;
        Elastos::Droid::Internal::Telephony::Gsm::SmsMessage::GetTPLayerLengthForPDU(pdu, &val);
        return val;
    }
}

AutoPtr<ArrayOf<Int32> > SmsMessage::CalculateLength(
    /* [in] */ ICharSequence* msgBody,
    /* [in] */ Boolean use7bitOnly)
{
    // this function is for MO SMS
    AutoPtr<IGsmAlphabetTextEncodingDetails> ted;

    if (UseCdmaFormatForMoSms()) {
        ted = Elastos::Droid::Internal::Telephony::Cdma::CDMASmsMessage::CalculateLength(
                msgBody, use7bitOnly);
    }
    else {
        Elastos::Droid::Internal::Telephony::Gsm::SmsMessage::CalculateLength(
                msgBody, use7bitOnly, (IGsmAlphabetTextEncodingDetails**)&ted);
    }

    AutoPtr<CGsmAlphabetTextEncodingDetails> cted = (CGsmAlphabetTextEncodingDetails*)ted.Get();

    AutoPtr<ArrayOf<Int32> > ret = ArrayOf<Int32>::Alloc(4);
    (*ret)[0] = cted->mMsgCount;
    (*ret)[1] = cted->mCodeUnitCount;
    (*ret)[2] = cted->mCodeUnitsRemaining;
    (*ret)[3] = cted->mCodeUnitSize;
    return ret;
}

AutoPtr<IArrayList> SmsMessage::FragmentText(
    /* [in] */ const String& text)
{
    // This function is for MO SMS
    AutoPtr<IGsmAlphabetTextEncodingDetails> ted;

    if (UseCdmaFormatForMoSms()) {
        ted = Elastos::Droid::Internal::Telephony::Cdma::CDMASmsMessage::CalculateLength(
                CoreUtils::Convert(text), FALSE);
    }
    else {
        Elastos::Droid::Internal::Telephony::Gsm::SmsMessage::CalculateLength(
                CoreUtils::Convert(text), FALSE, (IGsmAlphabetTextEncodingDetails**)&ted);
    }

    // TODO(cleanup): The code here could be rolled into the logic
    // below cleanly if these MAX_* constants were defined more
    // flexibly...

    AutoPtr<CGsmAlphabetTextEncodingDetails> cted = (CGsmAlphabetTextEncodingDetails*)ted.Get();

    Int32 limit;
    if (cted->mCodeUnitSize == ISmsConstants::ENCODING_7BIT) {
        Int32 udhLength;
        if (cted->mLanguageTable != 0 && cted->mLanguageShiftTable != 0) {
            udhLength = IGsmAlphabet::UDH_SEPTET_COST_TWO_SHIFT_TABLES;
        }
        else if (cted->mLanguageTable != 0 || cted->mLanguageShiftTable != 0) {
            udhLength = IGsmAlphabet::UDH_SEPTET_COST_ONE_SHIFT_TABLE;
        }
        else {
            udhLength = 0;
        }

        if (cted->mMsgCount > 1) {
            udhLength += IGsmAlphabet::UDH_SEPTET_COST_CONCATENATED_MESSAGE;
        }

        if (udhLength != 0) {
            udhLength += IGsmAlphabet::UDH_SEPTET_COST_LENGTH;
        }

        limit = ISmsConstants::MAX_USER_DATA_SEPTETS - udhLength;
    }
    else {
        if (cted->mMsgCount > 1) {
            limit = ISmsConstants::MAX_USER_DATA_BYTES_WITH_HEADER;
            // If EMS is not supported, break down EMS into single segment SMS
            // and add page info " x/y".
            // In the case of UCS2 encoding, we need 8 bytes for this,
            // but we only have 6 bytes from UDH, so truncate the limit for
            // each segment by 2 bytes (1 char).
            // Make sure total number of segments is less than 10.
            if (!HasEmsSupport() && cted->mMsgCount < 10) {
                limit -= 2;
            }
        }
        else {
            limit = ISmsConstants::MAX_USER_DATA_BYTES;
        }
    }

    String newMsgBody;
    AutoPtr<IResources> r = CResources::GetSystem();
    Boolean b;
    r->GetBoolean(R::bool_::config_sms_force_7bit_encoding, &b);
    if (b) {
        AutoPtr<ISms7BitEncodingTranslator> t;
        CSms7BitEncodingTranslator::AcquireSingleton((ISms7BitEncodingTranslator**)&t);
        t->Translate(CoreUtils::Convert(text), &newMsgBody);
    }
    if (TextUtils::IsEmpty(newMsgBody)) {
        newMsgBody = text;
    }
    Int32 pos = 0;  // Index in code units.
    Int32 textLen = newMsgBody.GetLength();
    AutoPtr<IArrayList> result;
    CArrayList::New(cted->mMsgCount, (IArrayList**)&result);
    while (pos < textLen) {
        Int32 nextPos = 0;  // Counts code units.
        if (cted->mCodeUnitSize == ISmsConstants::ENCODING_7BIT) {
            if (UseCdmaFormatForMoSms() && cted->mMsgCount == 1) {
                // For a singleton CDMA message, the encoding must be ASCII...
                nextPos = pos + Elastos::Core::Math::Min(limit, textLen - pos);
            }
            else {
                // For multi-segment messages, CDMA 7bit equals GSM 7bit encoding (EMS mode).
                AutoPtr<IGsmAlphabet> gsm;
                CGsmAlphabet::AcquireSingleton((IGsmAlphabet**)&gsm);
                gsm->FindGsmSeptetLimitIndex(newMsgBody, pos, limit,
                        cted->mLanguageTable, cted->mLanguageShiftTable, &nextPos);
            }
        }
        else {  // Assume unicode.
            nextPos = pos + Elastos::Core::Math::Min(limit / 2, textLen - pos);
        }
        if ((nextPos <= pos) || (nextPos > textLen)) {
            Logger::E(LOGTAG, "fragmentText failed (%d >= %d or %d >= %d)"
                    , pos, nextPos, nextPos, textLen);
            break;
        }
        result->Add(CoreUtils::Convert(newMsgBody.Substring(pos, nextPos)));
        pos = nextPos;
    }
    return result;
}

AutoPtr<ArrayOf<Int32> > SmsMessage::CalculateLength(
    /* [in] */ const String& messageBody,
    /* [in] */ Boolean use7bitOnly)
{
    return CalculateLength(CoreUtils::Convert(messageBody), use7bitOnly);
}

AutoPtr<ISmsMessageSubmitPdu> SmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested)
{
    AutoPtr<ISmsMessageBaseSubmitPduBase> spb;

    if (UseCdmaFormatForMoSms()) {
        spb = ISmsMessageBaseSubmitPduBase::Probe(
                Elastos::Droid::Internal::Telephony::Cdma::CDMASmsMessage::GetSubmitPdu(scAddress,
                destinationAddress, message, statusReportRequested, NULL));
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

AutoPtr<ISmsMessageSubmitPdu> SmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ Int64 subId)
{
    AutoPtr<ISmsMessageBaseSubmitPduBase> spb;
    if (UseCdmaFormatForMoSms(subId)) {
        spb = ISmsMessageBaseSubmitPduBase::Probe(
                Elastos::Droid::Internal::Telephony::Cdma::CDMASmsMessage::GetSubmitPdu(scAddress,
                destinationAddress, message, statusReportRequested, NULL));
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

AutoPtr<ISmsMessageSubmitPdu> SmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ Int16 destinationPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Boolean statusReportRequested)
{
    AutoPtr<ISmsMessageBaseSubmitPduBase> spb;

    if (UseCdmaFormatForMoSms()) {
        spb = ISmsMessageBaseSubmitPduBase::Probe(
                Elastos::Droid::Internal::Telephony::Cdma::CDMASmsMessage::GetSubmitPdu(scAddress,
                destinationAddress, destinationPort, data, statusReportRequested));
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

ECode SmsMessage::GetServiceCenterAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetServiceCenterAddress(result);
}

ECode SmsMessage::GetOriginatingAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (mWrappedSmsMessage != NULL) {
        mWrappedSmsMessage->GetOriginatingAddress(result);
    }
    else {
        *result = String(NULL);
    }
    return NOERROR;
}

ECode SmsMessage::GetDisplayOriginatingAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetDisplayOriginatingAddress(result);
}

ECode SmsMessage::GetMessageBody(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetMessageBody(result);
}

ECode SmsMessage::GetMessageClass(
    /* [out] */ MessageClass* result)
{
    VALIDATE_NOT_NULL(result);
    MessageClass cls;
    mWrappedSmsMessage->GetMessageClass(&cls);

    switch(cls) {
        case SmsMessage_MessageClass_CLASS_0: return SmsMessage_MessageClass_CLASS_0;
        case SmsMessage_MessageClass_CLASS_1: return SmsMessage_MessageClass_CLASS_1;
        case SmsMessage_MessageClass_CLASS_2: return SmsMessage_MessageClass_CLASS_2;
        case SmsMessage_MessageClass_CLASS_3: return SmsMessage_MessageClass_CLASS_3;
        default: return SmsMessage_MessageClass_UNKNOWN;

    }
    return NOERROR;
}

ECode SmsMessage::GetDisplayMessageBody(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetDisplayMessageBody(result);
}

ECode SmsMessage::GetPseudoSubject(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetPseudoSubject(result);
}

ECode SmsMessage::GetTimestampMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetTimestampMillis(result);
}

ECode SmsMessage::IsEmail(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->IsEmail(result);
}

ECode SmsMessage::GetEmailBody(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetEmailBody(result);
}

ECode SmsMessage::GetEmailFrom(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetEmailFrom(result);
}

ECode SmsMessage::GetProtocolIdentifier(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetProtocolIdentifier(result);
}

ECode SmsMessage::IsReplace(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->IsReplace(result);
}

ECode SmsMessage::IsCphsMwiMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->IsCphsMwiMessage(result);
}

ECode SmsMessage::IsMWIClearMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->IsMWIClearMessage(result);
}

ECode SmsMessage::IsMWISetMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->IsMWISetMessage(result);
}

ECode SmsMessage::IsMwiDontStore(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->IsMwiDontStore(result);
}

ECode SmsMessage::GetUserData(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetUserData(result);
}

ECode SmsMessage::GetPdu(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetPdu(result);
}

ECode SmsMessage::GetStatusOnSim(
    /* [out] */ Int32* result)
{
    return mWrappedSmsMessage->GetStatusOnIcc(result);
}

ECode SmsMessage::GetStatusOnIcc(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetStatusOnIcc(result);
}

ECode SmsMessage::GetIndexOnSim(
    /* [out] */ Int32* result)
{
    return mWrappedSmsMessage->GetIndexOnIcc(result);
}

ECode SmsMessage::GetIndexOnIcc(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetIndexOnIcc(result);
}

ECode SmsMessage::GetStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetStatus(result);
}

ECode SmsMessage::IsStatusReportMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->IsStatusReportMessage(result);
}

ECode SmsMessage::IsReplyPathPresent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->IsReplyPathPresent(result);
}

Boolean SmsMessage::HasEmsSupport()
{
    if (!IsNoEmsSupportConfigListExisted()) {
        return TRUE;
    }

    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    String simOperator;
    tm->GetSimOperator(&simOperator);

    String gid;
    tm->GetGroupIdLevel1(&gid);

    for (Int32 i = 0; i < mNoEmsSupportConfigList->GetLength(); i++) {
        AutoPtr<NoEmsSupportConfig> currentConfig = (*mNoEmsSupportConfigList)[i];

        if (simOperator.StartWith(currentConfig->mOperatorNumber) &&
            (TextUtils::IsEmpty(currentConfig->mGid1) ||
            (!TextUtils::IsEmpty(currentConfig->mGid1)
            && currentConfig->mGid1.EqualsIgnoreCase(gid)))) {
            return FALSE;
        }
     }
    return TRUE;
}

Boolean SmsMessage::ShouldAppendPageNumberAsPrefix()
{
    if (!IsNoEmsSupportConfigListExisted()) {
        return FALSE;
    }

    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    String simOperator;
    tm->GetSimOperator(&simOperator);

    String gid;
    tm->GetGroupIdLevel1(&gid);

    for (Int32 i = 0; i < mNoEmsSupportConfigList->GetLength(); i++) {
        AutoPtr<NoEmsSupportConfig> currentConfig = (*mNoEmsSupportConfigList)[i];

        if (simOperator.StartWith(currentConfig->mOperatorNumber) &&
            (TextUtils::IsEmpty(currentConfig->mGid1) ||
            (!TextUtils::IsEmpty(currentConfig->mGid1)
            && currentConfig->mGid1.EqualsIgnoreCase(gid)))) {
            return currentConfig->mIsPrefix;
        }
    }
    return FALSE;
}

ECode SmsMessage::GetRecipientAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrappedSmsMessage->GetRecipientAddress(result);
}

SmsMessage::SmsMessage(
    /* [in] */ ISmsMessageBase* smb)
{
    mWrappedSmsMessage = smb;
}

Boolean SmsMessage::UseCdmaFormatForMoSms()
{
    Int64 sid;
    SubscriptionManager::GetDefaultSmsSubId(&sid);
    AutoPtr<ISmsManager> smsManager = SmsManager::GetSmsManagerForSubscriber(sid);

    Boolean b;
    if (smsManager->IsImsSmsSupported(&b), !b) {
        // use Voice technology to determine SMS format.
        return IsCdmaVoice();
    }
    // IMS is registered with SMS support, check the SMS format supported
    String str;
    smsManager->GetImsSmsFormat(&str);
    return (str.Equals(ISmsConstants::FORMAT_3GPP2));
}

Boolean SmsMessage::UseCdmaFormatForMoSms(
    /* [in] */ Int64 subId)
{
    AutoPtr<ISmsManager> smsManager = SmsManager::GetSmsManagerForSubscriber(subId);

    Boolean b;
    if (smsManager->IsImsSmsSupported(&b), !b) {
        // use Voice technology to determine SMS format.
        return IsCdmaVoice(subId);
    }
    // IMS is registered with SMS support, check the SMS format supported
    String str;
    smsManager->GetImsSmsFormat(&str);
    return (str.Equals(ISmsConstants::FORMAT_3GPP2));
}

Boolean SmsMessage::IsCdmaVoice()
{
    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    Int32 activePhone;
    tm->GetCurrentPhoneType(&activePhone);

    return (ITelephonyManager::PHONE_TYPE_CDMA == activePhone);
}

Boolean SmsMessage::IsCdmaVoice(
    /* [in] */ Int64 subId)
{
    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    Int32 activePhone;
    tm->GetCurrentPhoneType(&activePhone);

    return (ITelephonyManager::PHONE_TYPE_CDMA == activePhone);
}

Boolean SmsMessage::IsNoEmsSupportConfigListExisted()
{
    if (!mIsNoEmsSupportConfigListLoaded) {
        AutoPtr<IResources> r = CResources::GetSystem();
        if (r != NULL) {
            AutoPtr<ArrayOf<String> > listArray;
            r->GetStringArray(
                    R::array::no_ems_support_sim_operators, (ArrayOf<String>**)&listArray);

            if ((listArray != NULL) && (listArray->GetLength() > 0)) {
                Int32 len = listArray->GetLength();
                mNoEmsSupportConfigList = ArrayOf<NoEmsSupportConfig*>::Alloc(len);

                for (Int32 i = 0; i < listArray->GetLength(); i++) {
                    AutoPtr<ArrayOf<String> > array;
                    StringUtils::Split((*listArray)[i], String(";"), (ArrayOf<String>**)&array);
                    AutoPtr<NoEmsSupportConfig> tmp = new NoEmsSupportConfig(array);
                    mNoEmsSupportConfigList->Set(i, tmp);
                }
            }
            mIsNoEmsSupportConfigListLoaded = TRUE;
        }
    }

    if (mNoEmsSupportConfigList != NULL && mNoEmsSupportConfigList->GetLength() != 0) {
        return TRUE;
    }

    return FALSE;
}

ECode SmsMessage::ToString(
    /* [out] */ String* result)
{
    return IObject::Probe(mWrappedSmsMessage)->ToString(result);
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
