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

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/internal/telephony/cdma/CDMASmsMessage.h"
#include "elastos/droid/internal/telephony/cdma/sms/BearerData.h"
#include "elastos/droid/internal/telephony/cdma/sms/CBearerData.h"
#include "elastos/droid/internal/telephony/cdma/sms/CCdmaSmsAddress.h"
#include "elastos/droid/internal/telephony/cdma/sms/CCdmaSmsSubaddress.h"
#include "elastos/droid/internal/telephony/cdma/sms/CSmsEnvelope.h"
#include "elastos/droid/internal/telephony/cdma/sms/CUserData.h"
#include "elastos/droid/internal/telephony/cdma/sms/UserData.h"
#include "elastos/droid/internal/telephony/CSms7BitEncodingTranslator.h"
#include "elastos/droid/internal/telephony/CSmsHeader.h"
#include "elastos/droid/internal/telephony/IccUtils.h"
#include "elastos/droid/internal/utility/CBitwiseInputStream.h"
#include "elastos/droid/internal/utility/HexDump.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/R.h"
#include "elastos/droid/telephony/CSmsCbLocation.h"
#include "elastos/droid/telephony/CSmsCbMessage.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::BearerData;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::CBearerData;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::CCdmaSmsAddress;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::CCdmaSmsSubaddress;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::CSmsEnvelope;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::CUserData;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::ICdmaSmsAddress;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::ICdmaSmsSubaddress;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::UserData;
using Elastos::Droid::Internal::Telephony::CSms7BitEncodingTranslator;
using Elastos::Droid::Internal::Telephony::CSmsHeader;
using Elastos::Droid::Internal::Telephony::IccUtils;
using Elastos::Droid::Internal::Telephony::ISms7BitEncodingTranslator;
using Elastos::Droid::Internal::Utility::CBitwiseInputStream;
using Elastos::Droid::Internal::Utility::HexDump;
using Elastos::Droid::Internal::Utility::IBitwiseInputStream;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::R;
using Elastos::Droid::Telephony::CSmsCbLocation;
using Elastos::Droid::Telephony::CSmsCbMessage;
using Elastos::Droid::Telephony::ISmsCbLocation;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::IO::CBufferedOutputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::IBufferedOutputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IDataInput;
using Elastos::IO::IDataInputStream;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                              CDMASmsMessage::SubmitPdu
//=====================================================================
CAR_INTERFACE_IMPL(CDMASmsMessage::SubmitPdu, SmsMessageBase::SubmitPduBase, ISmsMessageSubmitPdu);

//=====================================================================
//                              CDMASmsMessage
//=====================================================================
CAR_INTERFACE_IMPL(CDMASmsMessage, SmsMessageBase, ISmsMessage);

const String CDMASmsMessage::LOGTAG("CDMASmsMessage");
const String CDMASmsMessage::LOGGABLE_TAG("CDMA:SMS");
const Boolean CDMASmsMessage::VDBG = FALSE;
const Byte CDMASmsMessage::TELESERVICE_IDENTIFIER = 0x00;
const Byte CDMASmsMessage::SERVICE_CATEGORY = 0x01;
const Byte CDMASmsMessage::ORIGINATING_ADDRESS = 0x02;
const Byte CDMASmsMessage::ORIGINATING_SUB_ADDRESS = 0x03;
const Byte CDMASmsMessage::DESTINATION_ADDRESS = 0x04;
const Byte CDMASmsMessage::DESTINATION_SUB_ADDRESS = 0x05;
const Byte CDMASmsMessage::BEARER_REPLY_OPTION = 0x06;
const Byte CDMASmsMessage::CAUSE_CODES = 0x07;
const Byte CDMASmsMessage::BEARER_DATA = 0x08;
const Int32 CDMASmsMessage::RETURN_NO_ACK;
const Int32 CDMASmsMessage::RETURN_ACK;
const Int32 CDMASmsMessage::PRIORITY_NORMAL;
const Int32 CDMASmsMessage::PRIORITY_INTERACTIVE;
const Int32 CDMASmsMessage::PRIORITY_URGENT;
const Int32 CDMASmsMessage::PRIORITY_EMERGENCY;

CDMASmsMessage::CDMASmsMessage()
    : status(0)
{
}

AutoPtr<ISmsMessage> CDMASmsMessage::CreateFromPdu(
    /* [in] */ ArrayOf<Byte>* pdu)
{
    AutoPtr<CDMASmsMessage> msg = new CDMASmsMessage();

    // try {
    msg->ParsePdu(pdu);
    return ISmsMessage::Probe(msg);
    // } catch (RuntimeException ex) {
    //     Logger::E(LOGTAG, "SMS PDU parsing failed: ", ex);
    //     return NULL;
    // } catch (OutOfMemoryError e) {
    //     Logger::E(LOGTAG, "SMS PDU parsing failed with out of memory: ", e);
    //     return NULL;
    // }
}

AutoPtr<ISmsMessage> CDMASmsMessage::NewFromParcel(
    /* [in] */ IParcel* p)
{
    // Note: Parcel.readByte actually reads one Int and masks to byte
    AutoPtr<CDMASmsMessage> msg = new CDMASmsMessage();

    AutoPtr<CSmsEnvelope> env;
    CSmsEnvelope::NewByFriend((CSmsEnvelope**)&env);
    AutoPtr<CCdmaSmsAddress> addr;
    CCdmaSmsAddress::NewByFriend((CCdmaSmsAddress**)&addr);
    AutoPtr<CCdmaSmsSubaddress> subaddr;
    CCdmaSmsSubaddress::NewByFriend((CCdmaSmsSubaddress**)&subaddr);
    AutoPtr<ArrayOf<Byte> > data;
    Byte count;
    Int32 countInt;
    Int32 addressDigitMode;

    //currently not supported by the modem-lib: env->mMessageType
    p->ReadInt32(&env->teleService); //p_cur->uTeleserviceID

    Byte byte;
    p->ReadByte(&byte);
    if (0 != byte) { //p_cur->bIsServicePresent
        env->messageType = ISmsEnvelope::MESSAGE_TYPE_BROADCAST;
    }
    else {
        if (ISmsEnvelope::TELESERVICE_NOT_SET == env->teleService) {
            // assume type ACK
            env->messageType = ISmsEnvelope::MESSAGE_TYPE_ACKNOWLEDGE;
        }
        else {
            env->messageType = ISmsEnvelope::MESSAGE_TYPE_POINT_TO_POINT;
        }
    }
    p->ReadInt32(&env->serviceCategory); //p_cur->uServicecategory

    // address
    p->ReadInt32(&addressDigitMode);
    Int32 val;
    addr->digitMode = (Byte) (0xFF & addressDigitMode); //p_cur->sAddress.digit_mode
    addr->numberMode = (Byte) (0xFF & (p->ReadInt32(&val), val)); //p_cur->sAddress.number_mode
    p->ReadInt32(&addr->ton); //p_cur->sAddress.number_type
    addr->numberPlan = (Byte) (0xFF & (p->ReadInt32(&val), val)); //p_cur->sAddress.number_plan
    p->ReadByte(&count); //p_cur->sAddress.number_of_digits
    addr->numberOfDigits = count;
    data = ArrayOf<Byte>::Alloc(count);
    //p_cur->sAddress.digits[digitCount]
    for (Int32 index = 0; index < count; index++) {
        p->ReadByte(&(*data)[index]);

        // convert the value if it is 4-bit DTMF to 8 bit
        if (addressDigitMode == ICdmaSmsAddress::DIGIT_MODE_4BIT_DTMF) {
            (*data)[index] = msg->ConvertDtmfToAscii((*data)[index]);
        }
    }

    addr->origBytes = data;

    p->ReadInt32(&subaddr->type); // p_cur->sSubAddress.subaddressType
    p->ReadByte(&subaddr->odd);     // p_cur->sSubAddress.odd
    p->ReadByte(&count);           // p_cur->sSubAddress.number_of_digits

    if (count < 0) {
        count = 0;
    }

    // p_cur->sSubAddress.digits[digitCount] :

    data = ArrayOf<Byte>::Alloc(count);

    for (Int32 index = 0; index < count; ++index) {
        p->ReadByte(&(*data)[index]);
    }

    subaddr->origBytes = data;

    /* currently not supported by the modem-lib:
        env->bearerReply
        env->replySeqNo
        env->errorClass
        env->causeCode
    */

    // bearer data
    p->ReadInt32(&countInt); //p_cur->uBearerDataLen
    if (countInt < 0) {
        countInt = 0;
    }

    data = ArrayOf<Byte>::Alloc(countInt);
    for (Int32 index = 0; index < countInt; index++) {
        p->ReadByte(&(*data)[index]);
    }
    // BD gets further decoded when accessed in SMSDispatcher
    env->bearerData = data;

    // link the the filled objects to the SMS
    env->origAddress = addr;
    env->origSubaddress = subaddr;
    msg->mOriginatingAddress = addr;
    msg->mEnvelope = env;

    // create byte stream representation for transportation through the layers.
    msg->CreatePdu();

    return ISmsMessage::Probe(msg);
}

AutoPtr<ISmsMessage> CDMASmsMessage::CreateFromEfRecord(
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Byte>* data)
{
    // try {
    AutoPtr<CDMASmsMessage> msg = new CDMASmsMessage();

    msg->mIndexOnIcc = index;

    // First byte is status: RECEIVED_READ, RECEIVED_UNREAD, STORED_SENT,
    // or STORED_UNSENT
    // See 3GPP2 C.S0023 3.4.27
    if (((*data)[0] & 1) == 0) {
        Logger::W(LOGTAG, "SMS parsing failed: Trying to parse a free record");
        return NULL;
    }
    else {
        msg->mStatusOnIcc = (*data)[0] & 0x07;
    }

    // Second byte is the MSG_LEN, length of the message
    // See 3GPP2 C.S0023 3.4.27
    Int32 size = (*data)[1] & 0xFF;

    // Note: Data may include trailing FF's.  That's OK; message
    // should still parse correctly.
    AutoPtr<ArrayOf<Byte> > pdu = ArrayOf<Byte>::Alloc(size);
    pdu->Copy(0, data, 2, size);
    // the message has to be parsed before it can be displayed
    // see gsm.SmsMessage
    msg->ParsePduFromEfRecord(pdu);
    return ISmsMessage::Probe(msg);
    // } catch (RuntimeException ex) {
    //     Logger::E(LOGTAG, "SMS PDU parsing failed: ", ex);
    //     return NULL;
    // }
}

Int32 CDMASmsMessage::GetTPLayerLengthForPDU(
    /* [in] */ const String& pdu)
{
    Logger::W(LOGTAG, "getTPLayerLengthForPDU: is not supported in CDMA mode.");
    return 0;
}

AutoPtr<CDMASmsMessage::SubmitPdu> CDMASmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddr,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ ISmsHeader* smsHeader)
{
    return GetSubmitPdu(scAddr, destAddr, message, statusReportRequested, smsHeader, -1);
}

AutoPtr<CDMASmsMessage::SubmitPdu> CDMASmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddr,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ ISmsHeader* smsHeader,
    /* [in] */ Int32 priority)
{

    /**
     * TODO(cleanup): Do we really want silent failure like this?
     * Would it not be much more reasonable to make sure we don't
     * call this function if we really want nothing done?
     */
    if (message == NULL || destAddr == NULL) {
        return NULL;
    }

    AutoPtr<CUserData> uData;
    CUserData::NewByFriend((CUserData**)&uData);
    uData->payloadStr = message;
    uData->userDataHeader = smsHeader;
    return PrivateGetSubmitPdu(destAddr, statusReportRequested, IUserData::Probe(uData), priority);
}

AutoPtr<CDMASmsMessage::SubmitPdu> CDMASmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddr,
    /* [in] */ const String& destAddr,
    /* [in] */ Int32 destPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Boolean statusReportRequested)
{
    return GetSubmitPdu(scAddr, destAddr, destPort, 0, data, statusReportRequested);
}

AutoPtr<CDMASmsMessage::SubmitPdu> CDMASmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddr,
    /* [in] */ const String& destAddr,
    /* [in] */ Int32 destPort,
    /* [in] */ Int32 origPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Boolean statusReportRequested)
{
    /**
     * TODO(cleanup): this is not a general-purpose SMS creation
     * method, but rather something specialized to messages
     * containing OCTET encoded (meaning non-human-readable) user
     * data.  The name should reflect that, and not just overload.
     */

// TODO: Need SmsHeader.PortAddrs
    // SmsHeader.PortAddrs portAddrs = new SmsHeader.PortAddrs();
    // portAddrs.destPort = destPort;
    // portAddrs.origPort = origPort;
    // portAddrs.areEightBits = FALSE;

    AutoPtr<ISmsHeader> smsHeader;
    CSmsHeader::New((ISmsHeader**)&smsHeader);
// TODO: Need SmsHeader.PortAddrs
    // smsHeader.portAddrs = portAddrs;

    AutoPtr<CUserData> uData;
    CUserData::NewByFriend((CUserData**)&uData);
    uData->userDataHeader = smsHeader;
    uData->msgEncoding = IUserData::ENCODING_OCTET;
    uData->msgEncodingSet = TRUE;
    uData->payload = data;

    return PrivateGetSubmitPdu(destAddr, statusReportRequested, uData);
}

AutoPtr<CDMASmsMessage::SubmitPdu> CDMASmsMessage::GetSubmitPdu(
    /* [in] */ const String& destAddr,
    /* [in] */ IUserData* userData,
    /* [in] */ Boolean statusReportRequested)
{
    return PrivateGetSubmitPdu(destAddr, statusReportRequested, userData);
}

AutoPtr<CDMASmsMessage::SubmitPdu> CDMASmsMessage::GetSubmitPdu(
    /* [in] */ const String& destAddr,
    /* [in] */ IUserData* userData,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ Int32 priority)
{
    return PrivateGetSubmitPdu(destAddr, statusReportRequested, userData, priority);
}

ECode CDMASmsMessage::GetProtocolIdentifier(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::W(LOGTAG, "getProtocolIdentifier: is not supported in CDMA mode.");
    // (3GPP TS 23.040): "no interworking, but SME to SME protocol":
    *result = 0;
    return NOERROR;
}

ECode CDMASmsMessage::IsReplace(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::W(LOGTAG, "isReplace: is not supported in CDMA mode.");
    *result = FALSE;
    return NOERROR;
}

ECode CDMASmsMessage::IsCphsMwiMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::W(LOGTAG, "isCphsMwiMessage: is not supported in CDMA mode.");
    *result = FALSE;
    return NOERROR;
}

ECode CDMASmsMessage::IsMWIClearMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ((mBearerData != NULL) && (((BearerData*)mBearerData.Get())->numberOfMessages == 0));
    return NOERROR;
}

ECode CDMASmsMessage::IsMWISetMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ((mBearerData != NULL) &&
            (((BearerData*)mBearerData.Get())->numberOfMessages > 0));
    return NOERROR;
}

ECode CDMASmsMessage::IsMwiDontStore(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ((mBearerData != NULL) &&
            (((BearerData*)mBearerData.Get())->numberOfMessages > 0) &&
            (((BearerData*)mBearerData.Get())->userData == NULL));
    return NOERROR;
}

ECode CDMASmsMessage::GetStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (status << 16);
    return NOERROR;
}

ECode CDMASmsMessage::IsStatusReportMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (((BearerData*)mBearerData.Get())->messageType ==
            IBearerData::MESSAGE_TYPE_DELIVERY_ACK);
    return NOERROR;
}

ECode CDMASmsMessage::IsReplyPathPresent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::W(LOGTAG, "isReplyPathPresent: is not supported in CDMA mode.");
    *result = FALSE;
    return NOERROR;
}

AutoPtr<IGsmAlphabetTextEncodingDetails> CDMASmsMessage::CalculateLength(
    /* [in] */ ICharSequence* messageBody,
    /* [in] */ Boolean use7bitOnly)
{
    AutoPtr<ICharSequence> newMsgBody;
    AutoPtr<IResources> r = CResources::GetSystem();
    Boolean b;
    r->GetBoolean(R::bool_::config_sms_force_7bit_encoding, &b);
    if (b) {
        String str;
        AutoPtr<ISms7BitEncodingTranslator> t;
        CSms7BitEncodingTranslator::AcquireSingleton((ISms7BitEncodingTranslator**)&t);
        t->Translate(messageBody, &str);
        newMsgBody = CoreUtils::Convert(str);
    }
    if (TextUtils::IsEmpty(newMsgBody)) {
        newMsgBody = messageBody;
    }

    AutoPtr<IGsmAlphabetTextEncodingDetails> details =
            BearerData::CalcTextEncodingDetails(newMsgBody, use7bitOnly);
    return details;
}

ECode CDMASmsMessage::GetTeleService(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ((CSmsEnvelope*)mEnvelope.Get())->teleService;
    return NOERROR;
}

ECode CDMASmsMessage::GetMessageType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // NOTE: mEnvelope.messageType is not set correctly for cell broadcasts with some RILs.
    // Use the service category parameter to detect CMAS and other cell broadcast messages.
    if (((CSmsEnvelope*)mEnvelope.Get())->serviceCategory != 0) {
        *result = ISmsEnvelope::MESSAGE_TYPE_BROADCAST;
    } else {
        *result = ISmsEnvelope::MESSAGE_TYPE_POINT_TO_POINT;
    }
    return NOERROR;
}

ECode CDMASmsMessage::ParseBroadcastSms(
    /* [out] */ ISmsCbMessage** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<CSmsEnvelope> env = (CSmsEnvelope*)mEnvelope.Get();
    AutoPtr<IBearerData> bData =
            BearerData::Decode(env->bearerData, env->serviceCategory);
    if (bData == NULL) {
        Logger::W(LOGTAG, "BearerData.decode() returned NULL");
        *result = NULL;
        return NOERROR;
    }

    if (Logger::IsLoggable(LOGGABLE_TAG, Logger::VERBOSE)) {
        Logger::D(LOGTAG, "MT raw BearerData = %s",
                HexDump::ToHexString(env->bearerData).string());
    }

    String plmn;
    SystemProperties::Get(ITelephonyProperties::PROPERTY_OPERATOR_NUMERIC, &plmn);
    AutoPtr<ISmsCbLocation> location;
    CSmsCbLocation::New(plmn, (ISmsCbLocation**)&location);

    String lang;
    bData->GetLanguage(&lang);
    AutoPtr<BearerData> bd = (BearerData*)bData.Get();
    return CSmsCbMessage::New(ISmsCbMessage::MESSAGE_FORMAT_3GPP2,
            ISmsCbMessage::GEOGRAPHICAL_SCOPE_PLMN_WIDE, bd->messageId, location,
            env->serviceCategory, lang, bd->userData->payloadStr,
            bd->priority, NULL, bd->cmasWarningInfo, result);
}

ECode CDMASmsMessage::GetMessageClass(
    /* [out] */ MessageClass* result)
{
    VALIDATE_NOT_NULL(result);
    if (IBearerData::DISPLAY_MODE_IMMEDIATE ==
        ((BearerData*)mBearerData.Get())->displayMode ) {
        *result = MessageClass_CLASS_0;
    }
    else {
        *result = MessageClass_UNKNOWN;
    }
    return NOERROR;
}

// synchronized
Int32 CDMASmsMessage::GetNextMessageId()
{
    // Testing and dialog with partners has indicated that
    // msgId==0 is (sometimes?) treated specially by lower levels.
    // Specifically, the ID is not preserved for delivery ACKs.
    // Hence, avoid 0 -- constraining the range to 1..65535.
    Int32 msgId;
    SystemProperties::GetInt32(ITelephonyProperties::PROPERTY_CDMA_MSG_ID, 1, &msgId);
    String nextMsgId = StringUtils::ToString((msgId % 0xFFFF) + 1);
    SystemProperties::Set(ITelephonyProperties::PROPERTY_CDMA_MSG_ID, nextMsgId);
    if (Logger::IsLoggable(LOGGABLE_TAG, Logger::VERBOSE)) {
        Logger::D(LOGTAG, "next %s = %s",
            ITelephonyProperties::PROPERTY_CDMA_MSG_ID.string(), nextMsgId.string());
        String str;
        SystemProperties::Get(ITelephonyProperties::PROPERTY_CDMA_MSG_ID, &str);
        Logger::D(LOGTAG, "readback gets %s", str.string());
    }
    return msgId;
}

ECode CDMASmsMessage::GetNumOfVoicemails(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ((BearerData*)mBearerData.Get())->numberOfMessages;
    return NOERROR;
}

ECode CDMASmsMessage::GetIncomingSmsFingerprint(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IByteArrayOutputStream> output;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&output);

    AutoPtr<CSmsEnvelope> se = (CSmsEnvelope*)mEnvelope.Get();
    IOutputStream::Probe(output)->Write(se->serviceCategory);
    IOutputStream::Probe(output)->Write(se->teleService);
    AutoPtr<CCdmaSmsAddress> addr = (CCdmaSmsAddress*)se->origAddress.Get();
    IOutputStream::Probe(output)->Write(addr->origBytes, 0,
            addr->origBytes->GetLength());
    IOutputStream::Probe(output)->Write(se->bearerData, 0, se->bearerData->GetLength());
    IOutputStream::Probe(output)->Write(addr->origBytes, 0,
            addr->origBytes->GetLength());

    return output->ToByteArray(result);
}

ECode CDMASmsMessage::GetSmsCbProgramData(
    /* [out] */ IArrayList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = ((BearerData*)mBearerData.Get())->serviceCategoryProgramData;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDMASmsMessage::ParseSms()
{
    // Message Waiting Info Record defined in 3GPP2 C.S-0005, 3.7.5.6
    // It contains only an 8-bit number with the number of messages waiting
    AutoPtr<BearerData> bearerData;
    AutoPtr<CSmsEnvelope> se = (CSmsEnvelope*)mEnvelope.Get();
    if (se->teleService == ISmsEnvelope::TELESERVICE_MWI) {
        CBearerData::New((IBearerData**)&mBearerData);
        bearerData = (BearerData*)mBearerData.Get();

        if (se->bearerData != NULL) {
            bearerData->numberOfMessages = 0x000000FF & (*se->bearerData)[0];
        }
        if (VDBG) {
            Logger::D(LOGTAG, "parseSms: get MWI %s",
                  StringUtils::ToString(bearerData->numberOfMessages).string());
        }
        return NOERROR;
    }
    bearerData = BearerData::Decode(se->bearerData);
    mBearerData = IBearerData::Probe(bearerData);

    if (Logger::IsLoggable(LOGGABLE_TAG, Logger::VERBOSE)) {
        Logger::D(LOGTAG, "MT raw BearerData = '%s'",
                  HexDump::ToHexString(se->bearerData).string());
        // Logger::D(LOGTAG, "MT (decoded) BearerData = " + mBearerData);
    }
    mMessageRef = bearerData->messageId;
    if (bearerData->userData != NULL) {
        mUserData = bearerData->userData->payload;
        mUserDataHeader = bearerData->userData->userDataHeader;
        mMessageBody = bearerData->userData->payloadStr;
    }

    if (mOriginatingAddress != NULL) {
        AutoPtr<SmsAddress> originatingAddress = (SmsAddress*)mOriginatingAddress.Get();
        originatingAddress->address = String(*originatingAddress->origBytes);
        if (originatingAddress->ton == ICdmaSmsAddress::TON_INTERNATIONAL_OR_IP) {
            if (originatingAddress->address.GetChar(0) != '+') {
                originatingAddress->address = String("+") + originatingAddress->address;
            }
        }
        if (VDBG) Logger::V(LOGTAG, "SMS originating address: %s",
                originatingAddress->address.string());
    }

    if (bearerData->msgCenterTimeStamp != NULL) {
        ITime::Probe(bearerData->msgCenterTimeStamp)->ToMillis(TRUE, &mScTimeMillis);
    }

    if (VDBG) Logger::D(LOGTAG, "SMS SC timestamp: %lld", mScTimeMillis);

    // Message Type (See 3GPP2 C.S0015-B, v2, 4.5.1)
    if (bearerData->messageType == IBearerData::MESSAGE_TYPE_DELIVERY_ACK) {
        // The BearerData MsgStatus subparameter should only be
        // included for DELIVERY_ACK messages.  If it occurred for
        // other messages, it would be unclear what the status
        // being reported refers to.  The MsgStatus subparameter
        // is primarily useful to indicate error conditions -- a
        // message without this subparameter is assumed to
        // indicate successful delivery (status == 0).
        if (! bearerData->messageStatusSet) {
            Logger::D(LOGTAG, String("DELIVERY_ACK message without msgStatus (") +
                    (mUserData == NULL ? "also missing" : "does have") +
                    " userData).");
            status = 0;
        }
        else {
            status = bearerData->errorClass << 8;
            status |= bearerData->messageStatus;
        }
    }
    else if ((bearerData->messageType != IBearerData::MESSAGE_TYPE_DELIVER)
            && (bearerData->messageType != IBearerData::MESSAGE_TYPE_SUBMIT)) {
        // throw new RuntimeException("Unsupported message type: " + mBearerData.messageType);
        return E_RUNTIME_EXCEPTION;
    }

    if (!mMessageBody.IsNull()) {
        if (VDBG) Logger::V(LOGTAG, "SMS message body: '%s'", mMessageBody.string());
        ParseMessageBody();
    }
    else if ((mUserData != NULL) && VDBG) {
        String str = IccUtils::BytesToHexString(mUserData);
        Logger::V(LOGTAG, "SMS payload: '%s", str.string());
    }
    return NOERROR;
}

Boolean CDMASmsMessage::ProcessCdmaCTWdpHeader(
    /* [in] */ ISmsMessage* sms)
{
    Int32 subparamId = 0;
    Int32 subParamLen = 0;
    Int32 msgID = 0;
    Boolean decodeSuccess = FALSE;

    // try {
    AutoPtr<ArrayOf<Byte> > array;
    ISmsMessageBase::Probe(sms)->GetUserData((ArrayOf<Byte>**)&array);
    AutoPtr<IBitwiseInputStream> inStream;
    CBitwiseInputStream::New(array, (IBitwiseInputStream**)&inStream);

    /* Decode WDP Messsage Identifier */
    inStream->Read(8, &subparamId);
    if (subparamId != 0) {
        Logger::E(LOGTAG, "Invalid WDP SubparameterId");
        return FALSE;
    }
    inStream->Read(8, &subParamLen);
    if (subParamLen != 3) {
        Logger::E(LOGTAG, "Invalid WDP subparameter length");
        return FALSE;
    }

    AutoPtr<CDMASmsMessage> sm = (CDMASmsMessage*)sms;
    AutoPtr<BearerData> bd = (BearerData*)sm->mBearerData.Get();
    inStream->Read(4, &bd->messageType);
    Int32 val;
    msgID = (inStream->Read(8, &val), val) << 8;
    msgID |= (inStream->Read(8, &val), val);
    bd->hasUserDataHeader = ((inStream->Read(1, &val), val) == 1);
    if (bd->hasUserDataHeader) {
        Logger::E(LOGTAG, "Invalid WDP UserData header value");
        return FALSE;
    }
    inStream->Skip(3);
    bd->messageId = msgID;
    sm->mMessageRef = msgID;

    /* Decode WDP User Data */
    inStream->Read(8, &subparamId);
    subParamLen = (inStream->Read(8, &val), val) * 8;
    inStream->Read(5, &bd->userData->msgEncoding);
    Int32 consumedBits = 5;
    if (bd->userData->msgEncoding != 0) {
        Logger::E(LOGTAG, "Invalid WDP encoding");
        return FALSE;
    }
    inStream->Read(8, &bd->userData->numFields);
    consumedBits += 8;
    Int32 remainingBits = subParamLen - consumedBits;
    Int32 dataBits = bd->userData->numFields * 8;
    dataBits = dataBits < remainingBits ? dataBits : remainingBits;
    inStream->ReadByteArray(dataBits, (ArrayOf<Byte>**)&bd->userData->payload);
    sm->mUserData = bd->userData->payload;
    decodeSuccess = TRUE;
    // } catch (BitwiseInputStream.AccessException ex) {
    //     Logger::E(LOGTAG, "CT WDP Header decode failed: " + ex);
    // }

    return decodeSuccess;
}

ECode CDMASmsMessage::ParsePdu(
    /* [in] */ ArrayOf<Byte>* pdu)
{
    AutoPtr<IByteArrayInputStream> bais;
    CByteArrayInputStream::New(pdu, (IByteArrayInputStream**)&bais);
    AutoPtr<IDataInputStream> dis;
    CDataInputStream::New(IInputStream::Probe(bais), (IDataInputStream**)&dis);
    Int32 length;
    Int32 bearerDataLength;
    AutoPtr<CSmsEnvelope> env;
    CSmsEnvelope::NewByFriend((CSmsEnvelope**)&env);
    AutoPtr<CCdmaSmsAddress> addr;
    CCdmaSmsAddress::NewByFriend((CCdmaSmsAddress**)&addr);

    // try {
    IDataInput::Probe(dis)->ReadInt32(&env->messageType);
    IDataInput::Probe(dis)->ReadInt32(&env->teleService);
    IDataInput::Probe(dis)->ReadInt32(&env->serviceCategory);

    Byte byte;
    IDataInput::Probe(dis)->ReadByte(&byte);
    addr->digitMode = byte;
    IDataInput::Probe(dis)->ReadByte(&byte);
    addr->numberMode = byte;
    IDataInput::Probe(dis)->ReadByte(&byte);
    addr->ton = byte;
    IDataInput::Probe(dis)->ReadByte(&byte);
    addr->numberPlan = byte;

    IDataInput::Probe(dis)->ReadUnsignedByte(&length);
    addr->numberOfDigits = length;

    // sanity check on the length
    if (length > pdu->GetLength()) {
        // throw new RuntimeException(
        //         "createFromPdu: Invalid pdu, addr->numberOfDigits " + length
        //         + " > pdu len " + pdu.length);
        return E_RUNTIME_EXCEPTION;
    }

    addr->origBytes = ArrayOf<Byte>::Alloc(length);
    Int32 number;
    IInputStream::Probe(dis)->Read(addr->origBytes, 0, length, &number); // digits

    IDataInput::Probe(dis)->ReadInt32(&env->bearerReply);
    // CauseCode values:
    IDataInput::Probe(dis)->ReadByte(&env->replySeqNo);
    IDataInput::Probe(dis)->ReadByte(&env->errorClass);
    IDataInput::Probe(dis)->ReadByte(&env->causeCode);

    //encoded BearerData:
    IDataInput::Probe(dis)->ReadInt32(&bearerDataLength);
    // sanity check on the length
    if (bearerDataLength > pdu->GetLength()) {
        // throw new RuntimeException(
        //         "createFromPdu: Invalid pdu, bearerDataLength " + bearerDataLength
        //         + " > pdu len " + pdu.length);
        return E_RUNTIME_EXCEPTION;
    }

    env->bearerData = ArrayOf<Byte>::Alloc(bearerDataLength);
    IInputStream::Probe(dis)->Read(env->bearerData, 0, bearerDataLength, &number);
    ICloseable::Probe(dis)->Close();
    // } catch (IOException ex) {
    //     throw new RuntimeException(
    //             "createFromPdu: conversion from byte array to object failed: " + ex, ex);
    // } catch (Exception ex) {
    //     Logger::E(LOGTAG, "createFromPdu: conversion from byte array to object failed: " + ex);
    // }

    // link the filled objects to this SMS
    mOriginatingAddress = addr;
    env->origAddress = addr;
    mEnvelope = env;
    mPdu = pdu;

    ParseSms();
    return NOERROR;
}

void CDMASmsMessage::ParsePduFromEfRecord(
    /* [in] */ ArrayOf<Byte>* pdu)
{
    AutoPtr<IByteArrayInputStream> bais;
    CByteArrayInputStream::New(pdu, (IByteArrayInputStream**)&bais);
    AutoPtr<IDataInputStream> dis;
    CDataInputStream::New(IInputStream::Probe(bais), (IDataInputStream**)&dis);
    AutoPtr<CSmsEnvelope> env;
    CSmsEnvelope::NewByFriend((CSmsEnvelope**)&env);
    AutoPtr<CCdmaSmsAddress> addr;
    CCdmaSmsAddress::NewByFriend((CCdmaSmsAddress**)&addr);
    AutoPtr<CCdmaSmsSubaddress> subAddr;
    CCdmaSmsSubaddress::NewByFriend((CCdmaSmsSubaddress**)&subAddr);

    // try {
    Byte byte;
    IDataInput::Probe(dis)->ReadByte(&byte);
    env->messageType = byte;

    Int32 num;
    IInputStream::Probe(dis)->Available(&num);
    while (num > 0) {
        IDataInput::Probe(dis)->ReadByte(&byte);
        Int32 parameterId = byte;
        Int32 parameterLen;
        IDataInput::Probe(dis)->ReadUnsignedByte(&parameterLen);
        AutoPtr<ArrayOf<Byte> > parameterData = ArrayOf<Byte>::Alloc(parameterLen);

        switch (parameterId) {
            case TELESERVICE_IDENTIFIER:
                /*
                 * 16 bit parameter that identifies which upper layer
                 * service access point is sending or should receive
                 * this message
                 */
                IDataInput::Probe(dis)->ReadUnsignedInt16(&env->teleService);
                Logger::I(LOGTAG, "teleservice = %d", env->teleService);
                break;
            case SERVICE_CATEGORY:
                /*
                 * 16 bit parameter that identifies type of service as
                 * in 3GPP2 C.S0015-0 Table 3.4.3.2-1
                 */
                IDataInput::Probe(dis)->ReadUnsignedInt16(&env->serviceCategory);
                break;
            case ORIGINATING_ADDRESS:
            case DESTINATION_ADDRESS: {
                Int32 number;
                IInputStream::Probe(dis)->Read(parameterData, 0, parameterLen, &number);
                AutoPtr<IBitwiseInputStream> addrBis;
                CBitwiseInputStream::New(parameterData, (IBitwiseInputStream**)&addrBis);
                addrBis->Read(1, &addr->digitMode);
                addrBis->Read(1, &addr->numberMode);
                Int32 numberType = 0;
                if (addr->digitMode == ICdmaSmsAddress::DIGIT_MODE_8BIT_CHAR) {
                    addrBis->Read(3, &numberType);
                    addr->ton = numberType;

                    if (addr->numberMode == ICdmaSmsAddress::NUMBER_MODE_NOT_DATA_NETWORK)
                        addrBis->Read(4, &addr->numberPlan);
                }

                addrBis->Read(8, &addr->numberOfDigits);

                AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(addr->numberOfDigits);
                Byte b = 0x00;

                if (addr->digitMode == ICdmaSmsAddress::DIGIT_MODE_4BIT_DTMF) {
                    /* As per 3GPP2 C.S0005-0 Table 2.7.1.3.2.4-4 */
                    for (Int32 index = 0; index < addr->numberOfDigits; index++) {
                        Int32 val;
                        b = (Byte) (0xF & (addrBis->Read(4, &val), val));
                        // convert the value if it is 4-bit DTMF to 8
                        // bit
                        (*data)[index] = ConvertDtmfToAscii(b);
                    }
                }
                else if (addr->digitMode == ICdmaSmsAddress::DIGIT_MODE_8BIT_CHAR) {
                    if (addr->numberMode == ICdmaSmsAddress::NUMBER_MODE_NOT_DATA_NETWORK) {
                        for (Int32 index = 0; index < addr->numberOfDigits; index++) {
                            Int32 val;
                            b = (Byte) (0xFF & (addrBis->Read(8, &val), val));
                            (*data)[index] = b;
                        }

                    }
                    else if (addr->numberMode == ICdmaSmsAddress::NUMBER_MODE_DATA_NETWORK) {
                        if (numberType == 2)
                            Logger::E(LOGTAG, "TODO: Originating Addr is email id");
                        else
                            Logger::E(LOGTAG,
                                  "TODO: Originating Addr is data network address");
                    }
                    else {
                        Logger::E(LOGTAG, "Originating Addr is of incorrect type");
                    }
                }
                else {
                    Logger::E(LOGTAG, "Incorrect Digit mode");
                }
                addr->origBytes = data;
                // Logger::I(LOGTAG, "Originating Addr=" + addr->ToString());
                if (parameterId == DESTINATION_ADDRESS) {
                    env->destAddress = addr;
                    mRecipientAddress = addr;
                }
                break;
            }
            case ORIGINATING_SUB_ADDRESS:
            case DESTINATION_SUB_ADDRESS: {
                Int32 number;
                IInputStream::Probe(dis)->Read(parameterData, 0, parameterLen, &number);
                AutoPtr<CBitwiseInputStream> subAddrBis;
                CBitwiseInputStream::NewByFriend(parameterData, (CBitwiseInputStream**)&subAddrBis);
                subAddrBis->Read(3, &subAddr->type);
                AutoPtr<ArrayOf<Byte> > array;
                subAddrBis->ReadByteArray(1, (ArrayOf<Byte>**)&array);
                (*array)[0] = subAddr->odd;
                Int32 subAddrLen;
                subAddrBis->Read(8, &subAddrLen);
                AutoPtr<ArrayOf<Byte> > subdata = ArrayOf<Byte>::Alloc(subAddrLen);
                for (Int32 index = 0; index < subAddrLen; index++) {
                    Int32 val;
                    Byte b = (Byte) (0xFF & (subAddrBis->Read(4, &val), val));
                    // convert the value if it is 4-bit DTMF to 8 bit
                    (*subdata)[index] = ConvertDtmfToAscii(b);
                }
                subAddr->origBytes = subdata;
                break;
            }
            case BEARER_REPLY_OPTION: {
                Int32 number;
                IInputStream::Probe(dis)->Read(parameterData, 0, parameterLen, &number);
                AutoPtr<IBitwiseInputStream> replyOptBis;
                CBitwiseInputStream::New(parameterData, (IBitwiseInputStream**)&replyOptBis);
                replyOptBis->Read(6, &env->bearerReply);
                break;
            }
            case CAUSE_CODES: {
                Int32 number;
                IInputStream::Probe(dis)->Read(parameterData, 0, parameterLen, &number);
                AutoPtr<IBitwiseInputStream> ccBis;
                CBitwiseInputStream::New(parameterData, (IBitwiseInputStream**)&ccBis);
                AutoPtr<ArrayOf<Byte> > array;
                ccBis->ReadByteArray(6, (ArrayOf<Byte>**)&array);
                env->replySeqNo = (*array)[0];
                array = NULL;
                ccBis->ReadByteArray(2, (ArrayOf<Byte>**)&array);
                env->errorClass = (*array)[0];
                if (env->errorClass != 0x00)
                    array = NULL;
                    ccBis->ReadByteArray(8, (ArrayOf<Byte>**)&array);
                    env->causeCode =  (*array)[0];
                break;
            }
            case BEARER_DATA:
                Int32 number;
                IInputStream::Probe(dis)->Read(parameterData, 0, parameterLen, &number);
                env->bearerData = parameterData;
                break;
            default:
                // throw new Exception("unsupported parameterId (" + parameterId + ")");
                return;
        }
    }
    ICloseable::Probe(bais)->Close();
    ICloseable::Probe(dis)->Close();
    // } catch (Exception ex) {
    //     Logger::E(LOGTAG, "parsePduFromEfRecord: conversion from pdu to SmsMessage failed" + ex);
    // }

    // link the filled objects to this SMS
    mOriginatingAddress = addr;
    env->origAddress = addr;
    env->origSubaddress = ICdmaSmsSubaddress::Probe(subAddr);
    mEnvelope = env;
    mPdu = pdu;

    ParseSms();
}

AutoPtr<CDMASmsMessage::SubmitPdu> CDMASmsMessage::PrivateGetSubmitPdu(
    /* [in] */ const String& destAddrStr,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ IUserData* userData)
{
    return PrivateGetSubmitPdu(destAddrStr, statusReportRequested, userData, -1);
}

AutoPtr<CDMASmsMessage::SubmitPdu> CDMASmsMessage::PrivateGetSubmitPdu(
    /* [in] */ const String& destAddrStr,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ IUserData* userData,
    /* [in] */ Int32 priority)
{

    /**
     * TODO(cleanup): give this function a more meaningful name.
     */

    /**
     * TODO(cleanup): Make returning NULL from the getSubmitPdu
     * variations meaningful -- clean up the error feedback
     * mechanism, and avoid NULL pointer exceptions.
     */

    /**
     * North America Plus Code :
     * Convert + code to 011 and dial out for international SMS
     */
    String str;
    PhoneNumberUtils::CdmaCheckAndProcessPlusCodeForSms(destAddrStr, &str);
    AutoPtr<CCdmaSmsAddress> destAddr = (CCdmaSmsAddress*)CCdmaSmsAddress::Parse(str).Get();
    if (destAddr == NULL) return NULL;

    AutoPtr<CBearerData> bearerData;
    CBearerData::NewByFriend((CBearerData**)&bearerData);
    bearerData->messageType = IBearerData::MESSAGE_TYPE_SUBMIT;

    bearerData->messageId = GetNextMessageId();

    bearerData->deliveryAckReq = statusReportRequested;
    bearerData->userAckReq = FALSE;
    bearerData->readAckReq = FALSE;
    bearerData->reportReq = FALSE;
    if (priority >= PRIORITY_NORMAL && priority <= PRIORITY_EMERGENCY) {
        bearerData->priorityIndicatorSet = TRUE;
        bearerData->priority = priority;
    }

    bearerData->userData = (UserData*)userData;

    AutoPtr<ArrayOf<Byte> > encodedBearerData = BearerData::Encode(bearerData);
    if (Logger::IsLoggable(LOGGABLE_TAG, Logger::VERBOSE)) {
        // Logger::D(LOGTAG, "MO (encoded) BearerData = " + bearerData);
        Logger::D(LOGTAG, "MO raw BearerData = '%s'",
                HexDump::ToHexString(encodedBearerData).string());
    }
    if (encodedBearerData == NULL) return NULL;

    Int32 teleservice = bearerData->hasUserDataHeader ?
            ISmsEnvelope::TELESERVICE_WEMT : ISmsEnvelope::TELESERVICE_WMT;

    AutoPtr<IResources> resource = CResources::GetSystem();
    if (resource != NULL) {
        Boolean ascii7bitForLongMsg;
        resource->GetBoolean(R::bool_::config_ascii_7bit_support_for_long_message, &ascii7bitForLongMsg);
        if (ascii7bitForLongMsg) {
            Logger::D(LOGTAG, "ascii7bitForLongMsg = %d", ascii7bitForLongMsg);
            teleservice = ISmsEnvelope::TELESERVICE_WMT;
        }
    }
    AutoPtr<CSmsEnvelope> envelope;
    CSmsEnvelope::NewByFriend((CSmsEnvelope**)&envelope);
    envelope->messageType = ISmsEnvelope::MESSAGE_TYPE_POINT_TO_POINT;
    envelope->teleService = teleservice;
    envelope->destAddress = destAddr;
    envelope->bearerReply = RETURN_ACK;
    envelope->bearerData = encodedBearerData;

    /**
     * TODO(cleanup): envelope looks to be a pointless class, get
     * rid of it.  Also -- most of the envelope fields set here
     * are ignored, why?
     */

    // try {
    /**
     * TODO(cleanup): reference a spec and get rid of the ugly comments
     */
    AutoPtr<IByteArrayOutputStream> baos;
    CByteArrayOutputStream::New(100, (IByteArrayOutputStream**)&baos);
    AutoPtr<IDataOutputStream> dos;
    CDataOutputStream::New(IOutputStream::Probe(baos), (IDataOutputStream**)&dos);
    dos->WriteInt32(envelope->teleService);
    dos->WriteInt32(0); //servicePresent
    dos->WriteInt32(0); //serviceCategory
    IOutputStream::Probe(dos)->Write(destAddr->digitMode);
    IOutputStream::Probe(dos)->Write(destAddr->numberMode);
    IOutputStream::Probe(dos)->Write(destAddr->ton); // number_type
    IOutputStream::Probe(dos)->Write(destAddr->numberPlan);
    IOutputStream::Probe(dos)->Write(destAddr->numberOfDigits);
    IOutputStream::Probe(dos)->Write(destAddr->origBytes, 0, destAddr->origBytes->GetLength()); // digits
    // Subaddress is not supported.
    IOutputStream::Probe(dos)->Write(0); //subaddressType
    IOutputStream::Probe(dos)->Write(0); //subaddr_odd
    IOutputStream::Probe(dos)->Write(0); //subaddr_nbr_of_digits
    IOutputStream::Probe(dos)->Write(encodedBearerData->GetLength());
    IOutputStream::Probe(dos)->Write(encodedBearerData, 0, encodedBearerData->GetLength());
    ICloseable::Probe(dos)->Close();

    AutoPtr<SubmitPdu> pdu = new SubmitPdu();
    baos->ToByteArray((ArrayOf<Byte>**)&pdu->mEncodedMessage);
    pdu->mEncodedScAddress = NULL;
    return pdu;
}

void CDMASmsMessage::CreatePdu()
{
    AutoPtr<CSmsEnvelope> env = (CSmsEnvelope*)mEnvelope.Get();
    AutoPtr<CCdmaSmsAddress> addr = (CCdmaSmsAddress*)env->origAddress.Get();
    AutoPtr<IByteArrayOutputStream> baos;
    CByteArrayOutputStream::New(100, (IByteArrayOutputStream**)&baos);
    AutoPtr<IBufferedOutputStream> stream;
    CBufferedOutputStream::New(IOutputStream::Probe(baos), (IBufferedOutputStream**)&stream);
    AutoPtr<IDataOutputStream> dos;
    CDataOutputStream::New(IOutputStream::Probe(stream), (IDataOutputStream**)&dos);

    // try {
    dos->WriteInt32(env->messageType);
    dos->WriteInt32(env->teleService);
    dos->WriteInt32(env->serviceCategory);

    dos->WriteByte(addr->digitMode);
    dos->WriteByte(addr->numberMode);
    dos->WriteByte(addr->ton);
    dos->WriteByte(addr->numberPlan);
    dos->WriteByte(addr->numberOfDigits);
    IOutputStream::Probe(dos)->Write(addr->origBytes, 0, addr->origBytes->GetLength()); // digits

    dos->WriteInt32(env->bearerReply);
    // CauseCode values:
    dos->WriteByte(env->replySeqNo);
    dos->WriteByte(env->errorClass);
    dos->WriteByte(env->causeCode);
    //encoded BearerData:
    dos->WriteInt32(env->bearerData->GetLength());
    IOutputStream::Probe(dos)->Write(env->bearerData, 0, env->bearerData->GetLength());
    ICloseable::Probe(dos)->Close();

    /**
     * TODO(cleanup) -- The mPdu field is managed in
     * a fragile manner, and it would be much nicer if
     * accessing the serialized representation used a less
     * fragile mechanism.  Maybe the getPdu method could
     * generate a representation if there was not yet one?
     */

    baos->ToByteArray((ArrayOf<Byte>**)&mPdu);
    // } catch (IOException ex) {
    //     Logger::E(LOGTAG, "createPdu: conversion from object to byte array failed: " + ex);
    // }
}

Byte CDMASmsMessage::ConvertDtmfToAscii(
    /* [in] */ Byte dtmfDigit)
{
    Byte asciiDigit;

    switch (dtmfDigit) {
    case  0: asciiDigit = 68; break; // 'D'
    case  1: asciiDigit = 49; break; // '1'
    case  2: asciiDigit = 50; break; // '2'
    case  3: asciiDigit = 51; break; // '3'
    case  4: asciiDigit = 52; break; // '4'
    case  5: asciiDigit = 53; break; // '5'
    case  6: asciiDigit = 54; break; // '6'
    case  7: asciiDigit = 55; break; // '7'
    case  8: asciiDigit = 56; break; // '8'
    case  9: asciiDigit = 57; break; // '9'
    case 10: asciiDigit = 48; break; // '0'
    case 11: asciiDigit = 42; break; // '*'
    case 12: asciiDigit = 35; break; // '#'
    case 13: asciiDigit = 65; break; // 'A'
    case 14: asciiDigit = 66; break; // 'B'
    case 15: asciiDigit = 67; break; // 'C'
    default:
        asciiDigit = 32; // Invalid DTMF code
        break;
    }

    return asciiDigit;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
