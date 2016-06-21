#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telephony.h"

#include "elastos/droid/internal/telephony/cdma/SmsMessage.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                              SmsMessage::SubmitPdu
//=====================================================================
CAR_INTERFACE_IMPL(SmsMessage::SubmitPdu, /*TODO SmsMessageBase::SubmitPduBase*/Object, ISmsMessageSubmitPdu);

//=====================================================================
//                              SmsMessage
//=====================================================================
CAR_INTERFACE_IMPL(SmsMessage, SmsMessageBase, ISmsMessage);

const String SmsMessage::LOGTAG("SmsMessage");
const String SmsMessage::LOGGABLE_TAG("CDMA:SMS");
const Boolean SmsMessage::VDBG = FALSE;
const Byte SmsMessage::TELESERVICE_IDENTIFIER = 0x00;
const Byte SmsMessage::SERVICE_CATEGORY = 0x01;
const Byte SmsMessage::ORIGINATING_ADDRESS = 0x02;
const Byte SmsMessage::ORIGINATING_SUB_ADDRESS = 0x03;
const Byte SmsMessage::DESTINATION_ADDRESS = 0x04;
const Byte SmsMessage::DESTINATION_SUB_ADDRESS = 0x05;
const Byte SmsMessage::BEARER_REPLY_OPTION = 0x06;
const Byte SmsMessage::CAUSE_CODES = 0x07;
const Byte SmsMessage::BEARER_DATA = 0x08;
const Int32 SmsMessage::RETURN_NO_ACK;
const Int32 SmsMessage::RETURN_ACK;
const Int32 SmsMessage::PRIORITY_NORMAL;
const Int32 SmsMessage::PRIORITY_INTERACTIVE;
const Int32 SmsMessage::PRIORITY_URGENT;
const Int32 SmsMessage::PRIORITY_EMERGENCY;

AutoPtr<ISmsMessage> SmsMessage::CreateFromPdu(
    /* [in] */ ArrayOf<Byte>* pdu)
{
    // ==================before translated======================
    // SmsMessage msg = new SmsMessage();
    //
    // try {
    //     msg.parsePdu(pdu);
    //     return msg;
    // } catch (RuntimeException ex) {
    //     Rlog.e(LOGTAG, "SMS PDU parsing failed: ", ex);
    //     return null;
    // } catch (OutOfMemoryError e) {
    //     Log.e(LOGTAG, "SMS PDU parsing failed with out of memory: ", e);
    //     return null;
    // }
    assert(0);
    AutoPtr<SmsMessage> empty;
    return empty;
}

AutoPtr<ISmsMessage> SmsMessage::NewFromParcel(
    /* [in] */ IParcel* p)
{
    // ==================before translated======================
    // // Note: Parcel.readByte actually reads one Int and masks to byte
    // SmsMessage msg = new SmsMessage();
    // SmsEnvelope env = new SmsEnvelope();
    // CdmaSmsAddress addr = new CdmaSmsAddress();
    // CdmaSmsSubaddress subaddr = new CdmaSmsSubaddress();
    // byte[] data;
    // byte count;
    // int countInt;
    // int addressDigitMode;
    //
    // //currently not supported by the modem-lib: env.mMessageType
    // env.teleService = p.readInt(); //p_cur->uTeleserviceID
    //
    // if (0 != p.readByte()) { //p_cur->bIsServicePresent
    //     env.messageType = SmsEnvelope.MESSAGE_TYPE_BROADCAST;
    // }
    // else {
    //     if (SmsEnvelope.TELESERVICE_NOT_SET == env.teleService) {
    //         // assume type ACK
    //         env.messageType = SmsEnvelope.MESSAGE_TYPE_ACKNOWLEDGE;
    //     } else {
    //         env.messageType = SmsEnvelope.MESSAGE_TYPE_POINT_TO_POINT;
    //     }
    // }
    // env.serviceCategory = p.readInt(); //p_cur->uServicecategory
    //
    // // address
    // addressDigitMode = p.readInt();
    // addr.digitMode = (byte) (0xFF & addressDigitMode); //p_cur->sAddress.digit_mode
    // addr.numberMode = (byte) (0xFF & p.readInt()); //p_cur->sAddress.number_mode
    // addr.ton = p.readInt(); //p_cur->sAddress.number_type
    // addr.numberPlan = (byte) (0xFF & p.readInt()); //p_cur->sAddress.number_plan
    // count = p.readByte(); //p_cur->sAddress.number_of_digits
    // addr.numberOfDigits = count;
    // data = new byte[count];
    // //p_cur->sAddress.digits[digitCount]
    // for (int index=0; index < count; index++) {
    //     data[index] = p.readByte();
    //
    //     // convert the value if it is 4-bit DTMF to 8 bit
    //     if (addressDigitMode == CdmaSmsAddress.DIGIT_MODE_4BIT_DTMF) {
    //         data[index] = msg.convertDtmfToAscii(data[index]);
    //     }
    // }
    //
    // addr.origBytes = data;
    //
    // subaddr.type = p.readInt(); // p_cur->sSubAddress.subaddressType
    // subaddr.odd = p.readByte();     // p_cur->sSubAddress.odd
    // count = p.readByte();           // p_cur->sSubAddress.number_of_digits
    //
    // if (count < 0) {
    //     count = 0;
    // }
    //
    // // p_cur->sSubAddress.digits[digitCount] :
    //
    // data = new byte[count];
    //
    // for (int index = 0; index < count; ++index) {
    //     data[index] = p.readByte();
    // }
    //
    // subaddr.origBytes = data;
    //
    // /* currently not supported by the modem-lib:
    //     env.bearerReply
    //     env.replySeqNo
    //     env.errorClass
    //     env.causeCode
    // */
    //
    // // bearer data
    // countInt = p.readInt(); //p_cur->uBearerDataLen
    // if (countInt < 0) {
    //     countInt = 0;
    // }
    //
    // data = new byte[countInt];
    // for (int index=0; index < countInt; index++) {
    //     data[index] = p.readByte();
    // }
    // // BD gets further decoded when accessed in SMSDispatcher
    // env.bearerData = data;
    //
    // // link the the filled objects to the SMS
    // env.origAddress = addr;
    // env.origSubaddress = subaddr;
    // msg.mOriginatingAddress = addr;
    // msg.mEnvelope = env;
    //
    // // create byte stream representation for transportation through the layers.
    // msg.createPdu();
    //
    // return msg;
    assert(0);
    AutoPtr<SmsMessage> empty;
    return empty;
}

AutoPtr<ISmsMessage> SmsMessage::CreateFromEfRecord(
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Byte>* data)
{
    // ==================before translated======================
    // try {
    //     SmsMessage msg = new SmsMessage();
    //
    //     msg.mIndexOnIcc = index;
    //
    //     // First byte is status: RECEIVED_READ, RECEIVED_UNREAD, STORED_SENT,
    //     // or STORED_UNSENT
    //     // See 3GPP2 C.S0023 3.4.27
    //     if ((data[0] & 1) == 0) {
    //         Rlog.w(LOGTAG, "SMS parsing failed: Trying to parse a free record");
    //         return null;
    //     } else {
    //         msg.mStatusOnIcc = data[0] & 0x07;
    //     }
    //
    //     // Second byte is the MSG_LEN, length of the message
    //     // See 3GPP2 C.S0023 3.4.27
    //     int size = data[1] & 0xFF;
    //
    //     // Note: Data may include trailing FF's.  That's OK; message
    //     // should still parse correctly.
    //     byte[] pdu = new byte[size];
    //     System.arraycopy(data, 2, pdu, 0, size);
    //     // the message has to be parsed before it can be displayed
    //     // see gsm.SmsMessage
    //     msg.parsePduFromEfRecord(pdu);
    //     return msg;
    // } catch (RuntimeException ex) {
    //     Rlog.e(LOGTAG, "SMS PDU parsing failed: ", ex);
    //     return null;
    // }
    assert(0);
    AutoPtr<SmsMessage> empty;
    return empty;
}

Int32 SmsMessage::GetTPLayerLengthForPDU(
    /* [in] */ const String& pdu)
{
    // ==================before translated======================
    // Rlog.w(LOGTAG, "getTPLayerLengthForPDU: is not supported in CDMA mode.");
    // return 0;
    assert(0);
    return 0;
}

AutoPtr<SmsMessage::SubmitPdu> SmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddr,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ ISmsHeader* smsHeader)
{
    // ==================before translated======================
    // return getSubmitPdu(scAddr, destAddr, message, statusReportRequested, smsHeader, -1);
    assert(0);
    AutoPtr<SubmitPdu> empty;
    return empty;
}

AutoPtr<SmsMessage::SubmitPdu> SmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddr,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ ISmsHeader* smsHeader,
    /* [in] */ Int32 priority)
{
    // ==================before translated======================
    //
    // /**
    //  * TODO(cleanup): Do we really want silent failure like this?
    //  * Would it not be much more reasonable to make sure we don't
    //  * call this function if we really want nothing done?
    //  */
    // if (message == null || destAddr == null) {
    //     return null;
    // }
    //
    // UserData uData = new UserData();
    // uData.payloadStr = message;
    // uData.userDataHeader = smsHeader;
    // return privateGetSubmitPdu(destAddr, statusReportRequested, uData, priority);
    assert(0);
    AutoPtr<SubmitPdu> empty;
    return empty;
}

AutoPtr<SmsMessage::SubmitPdu> SmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddr,
    /* [in] */ const String& destAddr,
    /* [in] */ Int32 destPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Boolean statusReportRequested)
{
    // ==================before translated======================
    // return getSubmitPdu(scAddr, destAddr, destPort, 0, data,statusReportRequested);
    assert(0);
    AutoPtr<SubmitPdu> empty;
    return empty;
}

AutoPtr<SmsMessage::SubmitPdu> SmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddr,
    /* [in] */ const String& destAddr,
    /* [in] */ Int32 destPort,
    /* [in] */ Int32 origPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Boolean statusReportRequested)
{
    // ==================before translated======================
    //
    // /**
    //  * TODO(cleanup): this is not a general-purpose SMS creation
    //  * method, but rather something specialized to messages
    //  * containing OCTET encoded (meaning non-human-readable) user
    //  * data.  The name should reflect that, and not just overload.
    //  */
    //
    // SmsHeader.PortAddrs portAddrs = new SmsHeader.PortAddrs();
    // portAddrs.destPort = destPort;
    // portAddrs.origPort = origPort;
    // portAddrs.areEightBits = false;
    //
    // SmsHeader smsHeader = new SmsHeader();
    // smsHeader.portAddrs = portAddrs;
    //
    // UserData uData = new UserData();
    // uData.userDataHeader = smsHeader;
    // uData.msgEncoding = UserData.ENCODING_OCTET;
    // uData.msgEncodingSet = true;
    // uData.payload = data;
    //
    // return privateGetSubmitPdu(destAddr, statusReportRequested, uData);
    assert(0);
    AutoPtr<SubmitPdu> empty;
    return empty;
}

AutoPtr<SmsMessage::SubmitPdu> SmsMessage::GetSubmitPdu(
    /* [in] */ const String& destAddr,
    /* [in] */ IUserData* userData,
    /* [in] */ Boolean statusReportRequested)
{
    // ==================before translated======================
    // return privateGetSubmitPdu(destAddr, statusReportRequested, userData);
    assert(0);
    AutoPtr<SubmitPdu> empty;
    return empty;
}

AutoPtr<SmsMessage::SubmitPdu> SmsMessage::GetSubmitPdu(
    /* [in] */ const String& destAddr,
    /* [in] */ IUserData* userData,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ Int32 priority)
{
    // ==================before translated======================
    // return privateGetSubmitPdu(destAddr, statusReportRequested, userData, priority);
    assert(0);
    AutoPtr<SubmitPdu> empty;
    return empty;
}

ECode SmsMessage::GetProtocolIdentifier(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Rlog.w(LOGTAG, "getProtocolIdentifier: is not supported in CDMA mode.");
    // // (3GPP TS 23.040): "no interworking, but SME to SME protocol":
    // return 0;
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsReplace(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // Rlog.w(LOGTAG, "isReplace: is not supported in CDMA mode.");
    // return false;
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsCphsMwiMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // Rlog.w(LOGTAG, "isCphsMwiMessage: is not supported in CDMA mode.");
    // return false;
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsMWIClearMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return ((mBearerData != null) && (mBearerData.numberOfMessages == 0));
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsMWISetMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return ((mBearerData != null) && (mBearerData.numberOfMessages > 0));
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsMwiDontStore(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return ((mBearerData != null) &&
    //         (mBearerData.numberOfMessages > 0) &&
    //         (mBearerData.userData == null));
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return (status << 16);
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsStatusReportMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return (mBearerData.messageType == BearerData.MESSAGE_TYPE_DELIVERY_ACK);
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsReplyPathPresent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // Rlog.w(LOGTAG, "isReplyPathPresent: is not supported in CDMA mode.");
    // return false;
    assert(0);
    return NOERROR;
}

AutoPtr<IGsmAlphabetTextEncodingDetails> SmsMessage::CalculateLength(
    /* [in] */ ICharSequence* messageBody,
    /* [in] */ Boolean use7bitOnly)
{
    // ==================before translated======================
    // CharSequence newMsgBody = null;
    // Resources r = Resources.getSystem();
    // if (r.getBoolean(com.android.internal.R.bool.config_sms_force_7bit_encoding)) {
    //     newMsgBody  = Sms7BitEncodingTranslator.translate(messageBody);
    // }
    // if (TextUtils.isEmpty(newMsgBody)) {
    //     newMsgBody = messageBody;
    // }
    // return BearerData.calcTextEncodingDetails(newMsgBody, use7bitOnly);
    assert(0);
    AutoPtr<IGsmAlphabetTextEncodingDetails> empty;
    return empty;
}

ECode SmsMessage::GetTeleService(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mEnvelope.teleService;
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetMessageType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // NOTE: mEnvelope.messageType is not set correctly for cell broadcasts with some RILs.
    // // Use the service category parameter to detect CMAS and other cell broadcast messages.
    // if (mEnvelope.serviceCategory != 0) {
    //     return SmsEnvelope.MESSAGE_TYPE_BROADCAST;
    // } else {
    //     return SmsEnvelope.MESSAGE_TYPE_POINT_TO_POINT;
    // }
    assert(0);
    return NOERROR;
}

ECode SmsMessage::ParseBroadcastSms(
    /* [out] */ ISmsCbMessage** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // BearerData bData = BearerData.decode(mEnvelope.bearerData, mEnvelope.serviceCategory);
    // if (bData == null) {
    //     Rlog.w(LOGTAG, "BearerData.decode() returned null");
    //     return null;
    // }
    //
    // if (Rlog.isLoggable(LOGGABLE_TAG, Log.VERBOSE)) {
    //     Rlog.d(LOGTAG, "MT raw BearerData = " + HexDump.toHexString(mEnvelope.bearerData));
    // }
    //
    // String plmn = SystemProperties.get(TelephonyProperties.PROPERTY_OPERATOR_NUMERIC);
    // SmsCbLocation location = new SmsCbLocation(plmn);
    //
    // return new SmsCbMessage(SmsCbMessage.MESSAGE_FORMAT_3GPP2,
    //         SmsCbMessage.GEOGRAPHICAL_SCOPE_PLMN_WIDE, bData.messageId, location,
    //         mEnvelope.serviceCategory, bData.getLanguage(), bData.userData.payloadStr,
    //         bData.priority, null, bData.cmasWarningInfo);
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetMessageClass(
    /* [out] */ MessageClass* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (BearerData.DISPLAY_MODE_IMMEDIATE == mBearerData.displayMode ) {
    //     return SmsConstants.MessageClass.CLASS_0;
    // } else {
    //     return SmsConstants.MessageClass.UNKNOWN;
    // }
    assert(0);
    return NOERROR;
}

// synchronized
Int32 SmsMessage::GetNextMessageId()
{
    // ==================before translated======================
    // // Testing and dialog with partners has indicated that
    // // msgId==0 is (sometimes?) treated specially by lower levels.
    // // Specifically, the ID is not preserved for delivery ACKs.
    // // Hence, avoid 0 -- constraining the range to 1..65535.
    // int msgId = SystemProperties.getInt(TelephonyProperties.PROPERTY_CDMA_MSG_ID, 1);
    // String nextMsgId = Integer.toString((msgId % 0xFFFF) + 1);
    // SystemProperties.set(TelephonyProperties.PROPERTY_CDMA_MSG_ID, nextMsgId);
    // if (Rlog.isLoggable(LOGGABLE_TAG, Log.VERBOSE)) {
    //     Rlog.d(LOGTAG, "next " + TelephonyProperties.PROPERTY_CDMA_MSG_ID + " = " + nextMsgId);
    //     Rlog.d(LOGTAG, "readback gets " +
    //             SystemProperties.get(TelephonyProperties.PROPERTY_CDMA_MSG_ID));
    // }
    // return msgId;
    assert(0);
    return 0;
}

ECode SmsMessage::GetNumOfVoicemails(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mBearerData.numberOfMessages;
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetIncomingSmsFingerprint(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // ByteArrayOutputStream output = new ByteArrayOutputStream();
    //
    // output.write(mEnvelope.serviceCategory);
    // output.write(mEnvelope.teleService);
    // output.write(mEnvelope.origAddress.origBytes, 0, mEnvelope.origAddress.origBytes.length);
    // output.write(mEnvelope.bearerData, 0, mEnvelope.bearerData.length);
    // output.write(mEnvelope.origSubaddress.origBytes, 0,
    //         mEnvelope.origSubaddress.origBytes.length);
    //
    // return output.toByteArray();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetSmsCbProgramData(
    /* [out] */ IArrayList** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mBearerData.serviceCategoryProgramData;
    assert(0);
    return NOERROR;
}

void SmsMessage::ParseSms()
{
    // ==================before translated======================
    // // Message Waiting Info Record defined in 3GPP2 C.S-0005, 3.7.5.6
    // // It contains only an 8-bit number with the number of messages waiting
    // if (mEnvelope.teleService == SmsEnvelope.TELESERVICE_MWI) {
    //     mBearerData = new BearerData();
    //     if (mEnvelope.bearerData != null) {
    //         mBearerData.numberOfMessages = 0x000000FF & mEnvelope.bearerData[0];
    //     }
    //     if (VDBG) {
    //         Rlog.d(LOGTAG, "parseSms: get MWI " +
    //               Integer.toString(mBearerData.numberOfMessages));
    //     }
    //     return;
    // }
    // mBearerData = BearerData.decode(mEnvelope.bearerData);
    // if (Rlog.isLoggable(LOGGABLE_TAG, Log.VERBOSE)) {
    //     Rlog.d(LOGTAG, "MT raw BearerData = '" +
    //               HexDump.toHexString(mEnvelope.bearerData) + "'");
    //     Rlog.d(LOGTAG, "MT (decoded) BearerData = " + mBearerData);
    // }
    // mMessageRef = mBearerData.messageId;
    // if (mBearerData.userData != null) {
    //     mUserData = mBearerData.userData.payload;
    //     mUserDataHeader = mBearerData.userData.userDataHeader;
    //     mMessageBody = mBearerData.userData.payloadStr;
    // }
    //
    // if (mOriginatingAddress != null) {
    //     mOriginatingAddress.address = new String(mOriginatingAddress.origBytes);
    //     if (mOriginatingAddress.ton == CdmaSmsAddress.TON_INTERNATIONAL_OR_IP) {
    //         if (mOriginatingAddress.address.charAt(0) != '+') {
    //             mOriginatingAddress.address = "+" + mOriginatingAddress.address;
    //         }
    //     }
    //     if (VDBG) Rlog.v(LOGTAG, "SMS originating address: "
    //             + mOriginatingAddress.address);
    // }
    //
    // if (mBearerData.msgCenterTimeStamp != null) {
    //     mScTimeMillis = mBearerData.msgCenterTimeStamp.toMillis(true);
    // }
    //
    // if (VDBG) Rlog.d(LOGTAG, "SMS SC timestamp: " + mScTimeMillis);
    //
    // // Message Type (See 3GPP2 C.S0015-B, v2, 4.5.1)
    // if (mBearerData.messageType == BearerData.MESSAGE_TYPE_DELIVERY_ACK) {
    //     // The BearerData MsgStatus subparameter should only be
    //     // included for DELIVERY_ACK messages.  If it occurred for
    //     // other messages, it would be unclear what the status
    //     // being reported refers to.  The MsgStatus subparameter
    //     // is primarily useful to indicate error conditions -- a
    //     // message without this subparameter is assumed to
    //     // indicate successful delivery (status == 0).
    //     if (! mBearerData.messageStatusSet) {
    //         Rlog.d(LOGTAG, "DELIVERY_ACK message without msgStatus (" +
    //                 (mUserData == null ? "also missing" : "does have") +
    //                 " userData).");
    //         status = 0;
    //     } else {
    //         status = mBearerData.errorClass << 8;
    //         status |= mBearerData.messageStatus;
    //     }
    // } else if ((mBearerData.messageType != BearerData.MESSAGE_TYPE_DELIVER)
    //         && (mBearerData.messageType != BearerData.MESSAGE_TYPE_SUBMIT)) {
    //     throw new RuntimeException("Unsupported message type: " + mBearerData.messageType);
    // }
    //
    // if (mMessageBody != null) {
    //     if (VDBG) Rlog.v(LOGTAG, "SMS message body: '" + mMessageBody + "'");
    //     parseMessageBody();
    // } else if ((mUserData != null) && VDBG) {
    //     Rlog.v(LOGTAG, "SMS payload: '" + IccUtils.bytesToHexString(mUserData) + "'");
    // }
    assert(0);
}

Boolean SmsMessage::ProcessCdmaCTWdpHeader(
    /* [in] */ ISmsMessage* sms)
{
    // ==================before translated======================
    // int subparamId = 0;
    // int subParamLen = 0;
    // int msgID = 0;
    // boolean decodeSuccess = false;
    // try {
    //     BitwiseInputStream inStream = new BitwiseInputStream(sms.getUserData());
    //
    //     /* Decode WDP Messsage Identifier */
    //     subparamId = inStream.read(8);
    //     if (subparamId != 0) {
    //         Rlog.e(LOGTAG, "Invalid WDP SubparameterId");
    //         return false;
    //     }
    //     subParamLen = inStream.read(8);
    //     if (subParamLen != 3) {
    //         Rlog.e(LOGTAG, "Invalid WDP subparameter length");
    //         return false;
    //     }
    //     sms.mBearerData.messageType = inStream.read(4);
    //     msgID = inStream.read(8) << 8;
    //     msgID |= inStream.read(8);
    //     sms.mBearerData.hasUserDataHeader = (inStream.read(1) == 1);
    //     if (sms.mBearerData.hasUserDataHeader) {
    //         Rlog.e(LOGTAG, "Invalid WDP UserData header value");
    //         return false;
    //     }
    //     inStream.skip(3);
    //     sms.mBearerData.messageId = msgID;
    //     sms.mMessageRef = msgID;
    //
    //     /* Decode WDP User Data */
    //     subparamId = inStream.read(8);
    //     subParamLen = inStream.read(8) * 8;
    //     sms.mBearerData.userData.msgEncoding = inStream.read(5);
    //     int consumedBits = 5;
    //     if (sms.mBearerData.userData.msgEncoding != 0) {
    //         Rlog.e(LOGTAG, "Invalid WDP encoding");
    //         return false;
    //     }
    //     sms.mBearerData.userData.numFields = inStream.read(8);
    //     consumedBits += 8;
    //     int remainingBits = subParamLen - consumedBits;
    //     int dataBits = sms.mBearerData.userData.numFields * 8;
    //     dataBits = dataBits < remainingBits ? dataBits : remainingBits;
    //     sms.mBearerData.userData.payload = inStream.readByteArray(dataBits);
    //     sms.mUserData = sms.mBearerData.userData.payload;
    //     decodeSuccess = true;
    // } catch (BitwiseInputStream.AccessException ex) {
    //     Rlog.e(LOGTAG, "CT WDP Header decode failed: " + ex);
    // }
    // return decodeSuccess;
    assert(0);
    return FALSE;
}

void SmsMessage::ParsePdu(
    /* [in] */ ArrayOf<Byte>* pdu)
{
    // ==================before translated======================
    // ByteArrayInputStream bais = new ByteArrayInputStream(pdu);
    // DataInputStream dis = new DataInputStream(bais);
    // int length;
    // int bearerDataLength;
    // SmsEnvelope env = new SmsEnvelope();
    // CdmaSmsAddress addr = new CdmaSmsAddress();
    //
    // try {
    //     env.messageType = dis.readInt();
    //     env.teleService = dis.readInt();
    //     env.serviceCategory = dis.readInt();
    //
    //     addr.digitMode = dis.readByte();
    //     addr.numberMode = dis.readByte();
    //     addr.ton = dis.readByte();
    //     addr.numberPlan = dis.readByte();
    //
    //     length = dis.readUnsignedByte();
    //     addr.numberOfDigits = length;
    //
    //     // sanity check on the length
    //     if (length > pdu.length) {
    //         throw new RuntimeException(
    //                 "createFromPdu: Invalid pdu, addr.numberOfDigits " + length
    //                 + " > pdu len " + pdu.length);
    //     }
    //     addr.origBytes = new byte[length];
    //     dis.read(addr.origBytes, 0, length); // digits
    //
    //     env.bearerReply = dis.readInt();
    //     // CauseCode values:
    //     env.replySeqNo = dis.readByte();
    //     env.errorClass = dis.readByte();
    //     env.causeCode = dis.readByte();
    //
    //     //encoded BearerData:
    //     bearerDataLength = dis.readInt();
    //     // sanity check on the length
    //     if (bearerDataLength > pdu.length) {
    //         throw new RuntimeException(
    //                 "createFromPdu: Invalid pdu, bearerDataLength " + bearerDataLength
    //                 + " > pdu len " + pdu.length);
    //     }
    //     env.bearerData = new byte[bearerDataLength];
    //     dis.read(env.bearerData, 0, bearerDataLength);
    //     dis.close();
    // } catch (IOException ex) {
    //     throw new RuntimeException(
    //             "createFromPdu: conversion from byte array to object failed: " + ex, ex);
    // } catch (Exception ex) {
    //     Rlog.e(LOGTAG, "createFromPdu: conversion from byte array to object failed: " + ex);
    // }
    //
    // // link the filled objects to this SMS
    // mOriginatingAddress = addr;
    // env.origAddress = addr;
    // mEnvelope = env;
    // mPdu = pdu;
    //
    // parseSms();
    assert(0);
}

void SmsMessage::ParsePduFromEfRecord(
    /* [in] */ ArrayOf<Byte>* pdu)
{
    // ==================before translated======================
    // ByteArrayInputStream bais = new ByteArrayInputStream(pdu);
    // DataInputStream dis = new DataInputStream(bais);
    // SmsEnvelope env = new SmsEnvelope();
    // CdmaSmsAddress addr = new CdmaSmsAddress();
    // CdmaSmsSubaddress subAddr = new CdmaSmsSubaddress();
    //
    // try {
    //     env.messageType = dis.readByte();
    //
    //     while (dis.available() > 0) {
    //         int parameterId = dis.readByte();
    //         int parameterLen = dis.readUnsignedByte();
    //         byte[] parameterData = new byte[parameterLen];
    //
    //         switch (parameterId) {
    //             case TELESERVICE_IDENTIFIER:
    //                 /*
    //                  * 16 bit parameter that identifies which upper layer
    //                  * service access point is sending or should receive
    //                  * this message
    //                  */
    //                 env.teleService = dis.readUnsignedShort();
    //                 Rlog.i(LOGTAG, "teleservice = " + env.teleService);
    //                 break;
    //             case SERVICE_CATEGORY:
    //                 /*
    //                  * 16 bit parameter that identifies type of service as
    //                  * in 3GPP2 C.S0015-0 Table 3.4.3.2-1
    //                  */
    //                 env.serviceCategory = dis.readUnsignedShort();
    //                 break;
    //             case ORIGINATING_ADDRESS:
    //             case DESTINATION_ADDRESS:
    //                 dis.read(parameterData, 0, parameterLen);
    //                 BitwiseInputStream addrBis = new BitwiseInputStream(parameterData);
    //                 addr.digitMode = addrBis.read(1);
    //                 addr.numberMode = addrBis.read(1);
    //                 int numberType = 0;
    //                 if (addr.digitMode == CdmaSmsAddress.DIGIT_MODE_8BIT_CHAR) {
    //                     numberType = addrBis.read(3);
    //                     addr.ton = numberType;
    //
    //                     if (addr.numberMode == CdmaSmsAddress.NUMBER_MODE_NOT_DATA_NETWORK)
    //                         addr.numberPlan = addrBis.read(4);
    //                 }
    //
    //                 addr.numberOfDigits = addrBis.read(8);
    //
    //                 byte[] data = new byte[addr.numberOfDigits];
    //                 byte b = 0x00;
    //
    //                 if (addr.digitMode == CdmaSmsAddress.DIGIT_MODE_4BIT_DTMF) {
    //                     /* As per 3GPP2 C.S0005-0 Table 2.7.1.3.2.4-4 */
    //                     for (int index = 0; index < addr.numberOfDigits; index++) {
    //                         b = (byte) (0xF & addrBis.read(4));
    //                         // convert the value if it is 4-bit DTMF to 8
    //                         // bit
    //                         data[index] = convertDtmfToAscii(b);
    //                     }
    //                 } else if (addr.digitMode == CdmaSmsAddress.DIGIT_MODE_8BIT_CHAR) {
    //                     if (addr.numberMode == CdmaSmsAddress.NUMBER_MODE_NOT_DATA_NETWORK) {
    //                         for (int index = 0; index < addr.numberOfDigits; index++) {
    //                             b = (byte) (0xFF & addrBis.read(8));
    //                             data[index] = b;
    //                         }
    //
    //                     } else if (addr.numberMode == CdmaSmsAddress.NUMBER_MODE_DATA_NETWORK) {
    //                         if (numberType == 2)
    //                             Rlog.e(LOGTAG, "TODO: Originating Addr is email id");
    //                         else
    //                             Rlog.e(LOGTAG,
    //                                   "TODO: Originating Addr is data network address");
    //                     } else {
    //                         Rlog.e(LOGTAG, "Originating Addr is of incorrect type");
    //                     }
    //                 } else {
    //                     Rlog.e(LOGTAG, "Incorrect Digit mode");
    //                 }
    //                 addr.origBytes = data;
    //                 Rlog.i(LOGTAG, "Originating Addr=" + addr.toString());
    //                 if (parameterId == DESTINATION_ADDRESS) {
    //                     env.destAddress = addr;
    //                     mRecipientAddress = addr;
    //                 }
    //                 break;
    //             case ORIGINATING_SUB_ADDRESS:
    //             case DESTINATION_SUB_ADDRESS:
    //                 dis.read(parameterData, 0, parameterLen);
    //                 BitwiseInputStream subAddrBis = new BitwiseInputStream(parameterData);
    //                 subAddr.type = subAddrBis.read(3);
    //                 subAddr.odd = subAddrBis.readByteArray(1)[0];
    //                 int subAddrLen = subAddrBis.read(8);
    //                 byte[] subdata = new byte[subAddrLen];
    //                 for (int index = 0; index < subAddrLen; index++) {
    //                     b = (byte) (0xFF & subAddrBis.read(4));
    //                     // convert the value if it is 4-bit DTMF to 8 bit
    //                     subdata[index] = convertDtmfToAscii(b);
    //                 }
    //                 subAddr.origBytes = subdata;
    //                 break;
    //             case BEARER_REPLY_OPTION:
    //                 dis.read(parameterData, 0, parameterLen);
    //                 BitwiseInputStream replyOptBis = new BitwiseInputStream(parameterData);
    //                 env.bearerReply = replyOptBis.read(6);
    //                 break;
    //             case CAUSE_CODES:
    //                 dis.read(parameterData, 0, parameterLen);
    //                 BitwiseInputStream ccBis = new BitwiseInputStream(parameterData);
    //                 env.replySeqNo = ccBis.readByteArray(6)[0];
    //                 env.errorClass = ccBis.readByteArray(2)[0];
    //                 if (env.errorClass != 0x00)
    //                     env.causeCode = ccBis.readByteArray(8)[0];
    //                 break;
    //             case BEARER_DATA:
    //                 dis.read(parameterData, 0, parameterLen);
    //                 env.bearerData = parameterData;
    //                 break;
    //             default:
    //                 throw new Exception("unsupported parameterId (" + parameterId + ")");
    //         }
    //     }
    //     bais.close();
    //     dis.close();
    // } catch (Exception ex) {
    //     Rlog.e(LOGTAG, "parsePduFromEfRecord: conversion from pdu to SmsMessage failed" + ex);
    // }
    //
    // // link the filled objects to this SMS
    // mOriginatingAddress = addr;
    // env.origAddress = addr;
    // env.origSubaddress = subAddr;
    // mEnvelope = env;
    // mPdu = pdu;
    //
    // parseSms();
    assert(0);
}

AutoPtr<SmsMessage::SubmitPdu> SmsMessage::PrivateGetSubmitPdu(
    /* [in] */ const String& destAddrStr,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ IUserData* userData)
{
    // ==================before translated======================
    // return privateGetSubmitPdu(destAddrStr, statusReportRequested, userData, -1);
    assert(0);
    AutoPtr<SubmitPdu> empty;
    return empty;
}

AutoPtr<SmsMessage::SubmitPdu> SmsMessage::PrivateGetSubmitPdu(
    /* [in] */ const String& destAddrStr,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ IUserData* userData,
    /* [in] */ Int32 priority)
{
    // ==================before translated======================
    //
    // /**
    //  * TODO(cleanup): give this function a more meaningful name.
    //  */
    //
    // /**
    //  * TODO(cleanup): Make returning null from the getSubmitPdu
    //  * variations meaningful -- clean up the error feedback
    //  * mechanism, and avoid null pointer exceptions.
    //  */
    //
    // /**
    //  * North America Plus Code :
    //  * Convert + code to 011 and dial out for international SMS
    //  */
    // CdmaSmsAddress destAddr = CdmaSmsAddress.parse(
    //         PhoneNumberUtils.cdmaCheckAndProcessPlusCodeForSms(destAddrStr));
    // if (destAddr == null) return null;
    //
    // BearerData bearerData = new BearerData();
    // bearerData.messageType = BearerData.MESSAGE_TYPE_SUBMIT;
    //
    // bearerData.messageId = getNextMessageId();
    //
    // bearerData.deliveryAckReq = statusReportRequested;
    // bearerData.userAckReq = false;
    // bearerData.readAckReq = false;
    // bearerData.reportReq = false;
    // if (priority >= PRIORITY_NORMAL && priority <= PRIORITY_EMERGENCY) {
    //     bearerData.priorityIndicatorSet = true;
    //     bearerData.priority = priority;
    // }
    //
    // bearerData.userData = userData;
    //
    // byte[] encodedBearerData = BearerData.encode(bearerData);
    // if (Rlog.isLoggable(LOGGABLE_TAG, Log.VERBOSE)) {
    //     Rlog.d(LOGTAG, "MO (encoded) BearerData = " + bearerData);
    //     Rlog.d(LOGTAG, "MO raw BearerData = '" + HexDump.toHexString(encodedBearerData) + "'");
    // }
    // if (encodedBearerData == null) return null;
    //
    // int teleservice = bearerData.hasUserDataHeader ?
    //         SmsEnvelope.TELESERVICE_WEMT : SmsEnvelope.TELESERVICE_WMT;
    //
    // Resources resource = Resources.getSystem();
    // if (resource != null) {
    //     boolean ascii7bitForLongMsg = resource.
    //         getBoolean(com.android.internal.R.bool.config_ascii_7bit_support_for_long_message);
    //     if (ascii7bitForLongMsg) {
    //         Rlog.d(LOGTAG, "ascii7bitForLongMsg = " + ascii7bitForLongMsg);
    //         teleservice = SmsEnvelope.TELESERVICE_WMT;
    //     }
    // }
    // SmsEnvelope envelope = new SmsEnvelope();
    // envelope.messageType = SmsEnvelope.MESSAGE_TYPE_POINT_TO_POINT;
    // envelope.teleService = teleservice;
    // envelope.destAddress = destAddr;
    // envelope.bearerReply = RETURN_ACK;
    // envelope.bearerData = encodedBearerData;
    //
    // /**
    //  * TODO(cleanup): envelope looks to be a pointless class, get
    //  * rid of it.  Also -- most of the envelope fields set here
    //  * are ignored, why?
    //  */
    //
    // try {
    //     /**
    //      * TODO(cleanup): reference a spec and get rid of the ugly comments
    //      */
    //     ByteArrayOutputStream baos = new ByteArrayOutputStream(100);
    //     DataOutputStream dos = new DataOutputStream(baos);
    //     dos.writeInt(envelope.teleService);
    //     dos.writeInt(0); //servicePresent
    //     dos.writeInt(0); //serviceCategory
    //     dos.write(destAddr.digitMode);
    //     dos.write(destAddr.numberMode);
    //     dos.write(destAddr.ton); // number_type
    //     dos.write(destAddr.numberPlan);
    //     dos.write(destAddr.numberOfDigits);
    //     dos.write(destAddr.origBytes, 0, destAddr.origBytes.length); // digits
    //     // Subaddress is not supported.
    //     dos.write(0); //subaddressType
    //     dos.write(0); //subaddr_odd
    //     dos.write(0); //subaddr_nbr_of_digits
    //     dos.write(encodedBearerData.length);
    //     dos.write(encodedBearerData, 0, encodedBearerData.length);
    //     dos.close();
    //
    //     SubmitPdu pdu = new SubmitPdu();
    //     pdu.encodedMessage = baos.toByteArray();
    //     pdu.encodedScAddress = null;
    //     return pdu;
    // } catch(IOException ex) {
    //     Rlog.e(LOGTAG, "creating SubmitPdu failed: " + ex);
    // }
    // return null;
    assert(0);
    AutoPtr<SubmitPdu> empty;
    return empty;
}

void SmsMessage::CreatePdu()
{
    // ==================before translated======================
    // SmsEnvelope env = mEnvelope;
    // CdmaSmsAddress addr = env.origAddress;
    // ByteArrayOutputStream baos = new ByteArrayOutputStream(100);
    // DataOutputStream dos = new DataOutputStream(new BufferedOutputStream(baos));
    //
    // try {
    //     dos.writeInt(env.messageType);
    //     dos.writeInt(env.teleService);
    //     dos.writeInt(env.serviceCategory);
    //
    //     dos.writeByte(addr.digitMode);
    //     dos.writeByte(addr.numberMode);
    //     dos.writeByte(addr.ton);
    //     dos.writeByte(addr.numberPlan);
    //     dos.writeByte(addr.numberOfDigits);
    //     dos.write(addr.origBytes, 0, addr.origBytes.length); // digits
    //
    //     dos.writeInt(env.bearerReply);
    //     // CauseCode values:
    //     dos.writeByte(env.replySeqNo);
    //     dos.writeByte(env.errorClass);
    //     dos.writeByte(env.causeCode);
    //     //encoded BearerData:
    //     dos.writeInt(env.bearerData.length);
    //     dos.write(env.bearerData, 0, env.bearerData.length);
    //     dos.close();
    //
    //     /**
    //      * TODO(cleanup) -- The mPdu field is managed in
    //      * a fragile manner, and it would be much nicer if
    //      * accessing the serialized representation used a less
    //      * fragile mechanism.  Maybe the getPdu method could
    //      * generate a representation if there was not yet one?
    //      */
    //
    //     mPdu = baos.toByteArray();
    // } catch (IOException ex) {
    //     Rlog.e(LOGTAG, "createPdu: conversion from object to byte array failed: " + ex);
    // }
    assert(0);
}

Byte SmsMessage::ConvertDtmfToAscii(
    /* [in] */ Byte dtmfDigit)
{
    // ==================before translated======================
    // byte asciiDigit;
    //
    // switch (dtmfDigit) {
    // case  0: asciiDigit = 68; break; // 'D'
    // case  1: asciiDigit = 49; break; // '1'
    // case  2: asciiDigit = 50; break; // '2'
    // case  3: asciiDigit = 51; break; // '3'
    // case  4: asciiDigit = 52; break; // '4'
    // case  5: asciiDigit = 53; break; // '5'
    // case  6: asciiDigit = 54; break; // '6'
    // case  7: asciiDigit = 55; break; // '7'
    // case  8: asciiDigit = 56; break; // '8'
    // case  9: asciiDigit = 57; break; // '9'
    // case 10: asciiDigit = 48; break; // '0'
    // case 11: asciiDigit = 42; break; // '*'
    // case 12: asciiDigit = 35; break; // '#'
    // case 13: asciiDigit = 65; break; // 'A'
    // case 14: asciiDigit = 66; break; // 'B'
    // case 15: asciiDigit = 67; break; // 'C'
    // default:
    //     asciiDigit = 32; // Invalid DTMF code
    //     break;
    // }
    //
    // return asciiDigit;
    assert(0);
    return 0;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
