/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony.cdma;

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::ISmsCbLocation;
using Elastos::Droid::Telephony::ISmsCbMessage;
using Elastos::Droid::Telephony::Cdma::ICdmaSmsCbProgramData;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Content::Res::IResources;

using Elastos::Droid::Internal::Telephony::GsmAlphabet::ITextEncodingDetails;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::ISmsConstants;
using Elastos::Droid::Internal::Telephony::ISmsHeader;
using Elastos::Droid::Internal::Telephony::ISmsMessageBase;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::IBearerData;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::ICdmaSmsAddress;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::ICdmaSmsSubaddress;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::ISmsEnvelope;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::IUserData;
using Elastos::Droid::Internal::Telephony::Uicc::IIccUtils;
using Elastos::Droid::Internal::Utility::IBitwiseInputStream;
using Elastos::Droid::Internal::Utility::IHexDump;
using Elastos::Droid::Internal::Telephony::ISms7BitEncodingTranslator;

using Elastos::IO::IBufferedOutputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::IDataInputStream;
using Elastos::IO::IDataOutputStream;
using Elastos::Utility::IArrayList;

/**
 * TODO(cleanup): these constants are disturbing... are they not just
 * different interpretations on one number?  And if we did not have
 * terrible class name overlap, they would not need to be directly
 * imported like this.  The class in this file could just as well be
 * named CdmaSmsMessage, could it not?
 */

/**
 * TODO(cleanup): internally returning NULL in many places makes
 * debugging very Hard (among many other reasons) and should be made
 * more Meaningful (replaced with exceptions for example).  Null
 * returns should only occur at the very outside of the module/class
 * scope.
 */

/**
 * A Short Message Service message.
 *
 */
public class SmsMessage extends SmsMessageBase {
    static const String LOG_TAG = "SmsMessage";
    static private final String LOGGABLE_TAG = "CDMA:SMS";
    private static const Boolean VDBG = FALSE;

    private static const Byte TELESERVICE_IDENTIFIER                    = 0x00;
    private static const Byte SERVICE_CATEGORY                          = 0x01;
    private static const Byte ORIGINATING_ADDRESS                       = 0x02;
    private static const Byte ORIGINATING_SUB_ADDRESS                   = 0x03;
    private static const Byte DESTINATION_ADDRESS                       = 0x04;
    private static const Byte DESTINATION_SUB_ADDRESS                   = 0x05;
    private static const Byte BEARER_REPLY_OPTION                       = 0x06;
    private static const Byte CAUSE_CODES                               = 0x07;
    private static const Byte BEARER_DATA                               = 0x08;

    /**
     *  Status of a previously submitted SMS.
     *  This field applies to SMS Delivery Acknowledge messages. 0 indicates success;
     *  Here, the error class is defined by the bits from 9-8, the status code by the bits from 7-0.
     *  See C.S0015-B, v2.0, 4.5.21 for a detailed description of possible values.
     */
    private Int32 status;

    /** Specifies if a return of an acknowledgment is requested for send SMS */
    private static const Int32 RETURN_NO_ACK  = 0;
    private static const Int32 RETURN_ACK     = 1;

    /**
     * Supported priority modes for CDMA SMS messages
     * (See 3GPP2 C.S0015-B, v2.0, table 4.5.9-1)
     */
    private static const Int32 PRIORITY_NORMAL        = 0x0;
    private static const Int32 PRIORITY_INTERACTIVE   = 0x1;
    private static const Int32 PRIORITY_URGENT        = 0x2;
    private static const Int32 PRIORITY_EMERGENCY     = 0x3;

    private SmsEnvelope mEnvelope;
    private BearerData mBearerData;

    public static class SubmitPdu extends SubmitPduBase {
    }

    /**
     * Create an SmsMessage from a raw PDU.
     * Note: In CDMA the PDU is just a Byte representation of the received Sms.
     */
    public static SmsMessage CreateFromPdu(Byte[] pdu) {
        SmsMessage msg = new SmsMessage();

        try {
            msg->ParsePdu(pdu);
            return msg;
        } Catch (RuntimeException ex) {
            Rlog->E(LOG_TAG, "SMS PDU parsing failed: ", ex);
            return NULL;
        } Catch (OutOfMemoryError e) {
            Logger::E(LOG_TAG, "SMS PDU parsing failed with out of memory: ", e);
            return NULL;
        }
    }

    /**
     *  Create a "raw" CDMA SmsMessage from a Parcel that was forged in ril.cpp.
     *  Note: Only primitive fields are set.
     */
    public static SmsMessage NewFromParcel(Parcel p) {
        // Note: Parcel.readByte actually reads one Int and masks to Byte
        SmsMessage msg = new SmsMessage();
        SmsEnvelope env = new SmsEnvelope();
        CdmaSmsAddress addr = new CdmaSmsAddress();
        CdmaSmsSubaddress subaddr = new CdmaSmsSubaddress();
        Byte[] data;
        Byte count;
        Int32 countInt;
        Int32 addressDigitMode;

        //currently not supported by the modem-lib: env.mMessageType
        env.teleService = p->ReadInt(); //p_cur->uTeleserviceID

        If (0 != p->ReadByte()) { //p_cur->bIsServicePresent
            env.messageType = SmsEnvelope.MESSAGE_TYPE_BROADCAST;
        }
        else {
            If (SmsEnvelope.TELESERVICE_NOT_SET == env.teleService) {
                // assume type ACK
                env.messageType = SmsEnvelope.MESSAGE_TYPE_ACKNOWLEDGE;
            } else {
                env.messageType = SmsEnvelope.MESSAGE_TYPE_POINT_TO_POINT;
            }
        }
        env.serviceCategory = p->ReadInt(); //p_cur->uServicecategory

        // address
        addressDigitMode = p->ReadInt();
        addr.digitMode = (Byte) (0xFF & addressDigitMode); //p_cur->sAddress.digit_mode
        addr.numberMode = (Byte) (0xFF & p->ReadInt()); //p_cur->sAddress.number_mode
        addr.ton = p->ReadInt(); //p_cur->sAddress.number_type
        addr.numberPlan = (Byte) (0xFF & p->ReadInt()); //p_cur->sAddress.number_plan
        count = p->ReadByte(); //p_cur->sAddress.number_of_digits
        addr.numberOfDigits = count;
        data = new Byte[count];
        //p_cur->sAddress.digits[digitCount]
        For (Int32 index=0; index < count; index++) {
            data[index] = p->ReadByte();

            // convert the value if it is 4-bit DTMF to 8 bit
            If (addressDigitMode == CdmaSmsAddress.DIGIT_MODE_4BIT_DTMF) {
                data[index] = msg->ConvertDtmfToAscii(data[index]);
            }
        }

        addr.origBytes = data;

        subaddr.type = p->ReadInt(); // p_cur->sSubAddress.subaddressType
        subaddr.odd = p->ReadByte();     // p_cur->sSubAddress.odd
        count = p->ReadByte();           // p_cur->sSubAddress.number_of_digits

        If (count < 0) {
            count = 0;
        }

        // p_cur->sSubAddress.digits[digitCount] :

        data = new Byte[count];

        For (Int32 index = 0; index < count; ++index) {
            data[index] = p->ReadByte();
        }

        subaddr.origBytes = data;

        /* currently not supported by the modem-lib:
            env.bearerReply
            env.replySeqNo
            env.errorClass
            env.causeCode
        */

        // bearer data
        countInt = p->ReadInt(); //p_cur->uBearerDataLen
        If (countInt < 0) {
            countInt = 0;
        }

        data = new Byte[countInt];
        For (Int32 index=0; index < countInt; index++) {
            data[index] = p->ReadByte();
        }
        // BD gets further decoded when accessed in SMSDispatcher
        env.bearerData = data;

        // link the the filled objects to the SMS
        env.origAddress = addr;
        env.origSubaddress = subaddr;
        msg.mOriginatingAddress = addr;
        msg.mEnvelope = env;

        // create Byte stream representation for transportation through the layers.
        msg->CreatePdu();

        return msg;
    }

    /**
     * Create an SmsMessage from an SMS EF record.
     *
     * @param index Index of SMS record. This should be index in ArrayList
     *              returned by RuimSmsInterfaceManager.getAllMessagesFromIcc + 1.
     * @param data Record data.
     * @return An SmsMessage representing the record.
     *
     * @hide
     */
    public static SmsMessage CreateFromEfRecord(Int32 index, Byte[] data) {
        try {
            SmsMessage msg = new SmsMessage();

            msg.mIndexOnIcc = index;

            // First Byte is status: RECEIVED_READ, RECEIVED_UNREAD, STORED_SENT,
            // or STORED_UNSENT
            // See 3GPP2 C.S0023 3.4.27
            If ((data[0] & 1) == 0) {
                Rlog->W(LOG_TAG, "SMS parsing failed: Trying to parse a free record");
                return NULL;
            } else {
                msg.mStatusOnIcc = data[0] & 0x07;
            }

            // Second Byte is the MSG_LEN, length of the message
            // See 3GPP2 C.S0023 3.4.27
            Int32 size = data[1] & 0xFF;

            // Note: Data may include trailing FF's.  That's OK; message
            // should still parse correctly.
            Byte[] pdu = new Byte[size];
            System->Arraycopy(data, 2, pdu, 0, size);
            // the message has to be parsed before it can be displayed
            // see gsm.SmsMessage
            msg->ParsePduFromEfRecord(pdu);
            return msg;
        } Catch (RuntimeException ex) {
            Rlog->E(LOG_TAG, "SMS PDU parsing failed: ", ex);
            return NULL;
        }

    }

    /**
     * Note: This function is a GSM specific functionality which is not supported in CDMA mode.
     */
    public static Int32 GetTPLayerLengthForPDU(String pdu) {
        Rlog->W(LOG_TAG, "getTPLayerLengthForPDU: is not supported in CDMA mode.");
        return 0;
    }

    /**
     * TODO(cleanup): why do getSubmitPdu methods take an scAddr input
     * and do nothing with it?  GSM allows us to specify a SC (eg,
     * when responding to an SMS that explicitly requests the response
     * is sent to a specific SC), or pass NULL to use the default
     * value.  Is there no similar notion in CDMA? Or do we just not
     * have it hooked up?
     */

    /**
     * Get an SMS-SUBMIT PDU for a destination address and a message
     *
     * @param scAddr                Service Centre address.  Null means use default.
     * @param destAddr              Address of the recipient.
     * @param message               String representation of the message payload.
     * @param statusReportRequested Indicates whether a report is requested for this message.
     * @param smsHeader             Array containing the data for the User Data Header, preceded
     *                              by the Element Identifiers.
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns NULL on encode error.
     * @hide
     */
    public static SubmitPdu GetSubmitPdu(String scAddr, String destAddr, String message,
            Boolean statusReportRequested, SmsHeader smsHeader) {
        return GetSubmitPdu(scAddr, destAddr, message, statusReportRequested, smsHeader, -1);
    }

    /**
     * Get an SMS-SUBMIT PDU for a destination address and a message
     *
     * @param scAddr                Service Centre address.  Null means use default.
     * @param destAddr              Address of the recipient.
     * @param message               String representation of the message payload.
     * @param statusReportRequested Indicates whether a report is requested for this message.
     * @param smsHeader             Array containing the data for the User Data Header, preceded
     *                              by the Element Identifiers.
     * @param priority              Priority level of the message
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns NULL on encode error.
     * @hide
     */
    public static SubmitPdu GetSubmitPdu(String scAddr, String destAddr, String message,
            Boolean statusReportRequested, SmsHeader smsHeader, Int32 priority) {

        /**
         * TODO(cleanup): Do we really want silent failure like this?
         * Would it not be much more reasonable to make sure we don't
         * call this function if we really want nothing done?
         */
        If (message == NULL || destAddr == NULL) {
            return NULL;
        }

        UserData uData = new UserData();
        uData.payloadStr = message;
        uData.userDataHeader = smsHeader;
        return PrivateGetSubmitPdu(destAddr, statusReportRequested, uData, priority);
    }

    /**
     * Get an SMS-SUBMIT PDU for a data message to a destination address and port.
     *
     * @param scAddr Service Centre address. NULL == use default
     * @param destAddr the address of the destination for the message
     * @param destPort the port to deliver the message to at the
     *        destination
     * @param data the data for the message
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns NULL on encode error.
     */
    public static SubmitPdu GetSubmitPdu(String scAddr, String destAddr,
            Int32 destPort, Byte[] data, Boolean statusReportRequested) {
        return GetSubmitPdu(scAddr, destAddr, destPort, 0, data,statusReportRequested);
    }


    /**
     * Get an SMS-SUBMIT PDU for a data message to a destination address and port.
     *
     * @param scAddr Service Centre address. NULL == use default
     * @param destAddr the address of the destination for the message
     * @param destPort the port to deliver the message to at the
     *        destination
     * @param origPort  the originator port set by the sender
     * @param data the data for the message
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns NULL on encode error.
     */
    public static SubmitPdu GetSubmitPdu(String scAddr, String destAddr, Int32 destPort,
             Int32 origPort, Byte[] data, Boolean statusReportRequested) {

        /**
         * TODO(cleanup): this is not a general-purpose SMS creation
         * method, but rather something specialized to messages
         * containing OCTET Encoded (meaning non-human-readable) user
         * data.  The name should reflect that, and not just overload.
         */

        SmsHeader.PortAddrs portAddrs = new SmsHeader->PortAddrs();
        portAddrs.destPort = destPort;
        portAddrs.origPort = origPort;
        portAddrs.areEightBits = FALSE;

        SmsHeader smsHeader = new SmsHeader();
        smsHeader.portAddrs = portAddrs;

        UserData uData = new UserData();
        uData.userDataHeader = smsHeader;
        uData.msgEncoding = UserData.ENCODING_OCTET;
        uData.msgEncodingSet = TRUE;
        uData.payload = data;

        return PrivateGetSubmitPdu(destAddr, statusReportRequested, uData);
    }

    /**
     * Get an SMS-SUBMIT PDU for a data message to a destination address &amp; port
     *
     * @param destAddr the address of the destination for the message
     * @param userData the data for the message
     * @param statusReportRequested Indicates whether a report is requested for this message.
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns NULL on encode error.
     */
    public static SubmitPdu GetSubmitPdu(String destAddr, UserData userData,
            Boolean statusReportRequested) {
        return PrivateGetSubmitPdu(destAddr, statusReportRequested, userData);
    }

    /**
     * Get an SMS-SUBMIT PDU for a data message to a destination address &amp; port
     *
     * @param destAddr the address of the destination for the message
     * @param userData the data for the message
     * @param statusReportRequested Indicates whether a report is requested for this message.
     * @param priority Priority level of the message
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns NULL on encode error.
     */
    public static SubmitPdu GetSubmitPdu(String destAddr, UserData userData,
            Boolean statusReportRequested, Int32 priority) {
        return PrivateGetSubmitPdu(destAddr, statusReportRequested, userData, priority);
    }

    /**
     * Note: This function is a GSM specific functionality which is not supported in CDMA mode.
     */
    //@Override
    public Int32 GetProtocolIdentifier() {
        Rlog->W(LOG_TAG, "getProtocolIdentifier: is not supported in CDMA mode.");
        // (3GPP TS 23.040): "no interworking, but SME to SME protocol":
        return 0;
    }

    /**
     * Note: This function is a GSM specific functionality which is not supported in CDMA mode.
     */
    //@Override
    public Boolean IsReplace() {
        Rlog->W(LOG_TAG, "isReplace: is not supported in CDMA mode.");
        return FALSE;
    }

    /**
     * {@inheritDoc}
     * Note: This function is a GSM specific functionality which is not supported in CDMA mode.
     */
    //@Override
    public Boolean IsCphsMwiMessage() {
        Rlog->W(LOG_TAG, "isCphsMwiMessage: is not supported in CDMA mode.");
        return FALSE;
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    public Boolean IsMWIClearMessage() {
        Return ((mBearerData != NULL) && (mBearerData.numberOfMessages == 0));
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    public Boolean IsMWISetMessage() {
        Return ((mBearerData != NULL) && (mBearerData.numberOfMessages > 0));
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    public Boolean IsMwiDontStore() {
        Return ((mBearerData != NULL) &&
                (mBearerData.numberOfMessages > 0) &&
                (mBearerData.userData == NULL));
    }

    /**
     * Returns the status for a previously submitted message.
     * For not interfering with status codes from GSM, this status code is
     * shifted to the bits 31-16.
     */
    //@Override
    public Int32 GetStatus() {
        Return (status << 16);
    }

    /** Return TRUE iff the bearer data message type is DELIVERY_ACK. */
    //@Override
    public Boolean IsStatusReportMessage() {
        Return (mBearerData.messageType == BearerData.MESSAGE_TYPE_DELIVERY_ACK);
    }

    /**
     * Note: This function is a GSM specific functionality which is not supported in CDMA mode.
     */
    //@Override
    public Boolean IsReplyPathPresent() {
        Rlog->W(LOG_TAG, "isReplyPathPresent: is not supported in CDMA mode.");
        return FALSE;
    }

    /**
     * Calculate the number of septets needed to encode the message.
     *
     * @param messageBody the message to encode
     * @param use7bitOnly Ignore (but still count) illegal characters if TRUE
     * @return TextEncodingDetails
     */
    public static TextEncodingDetails CalculateLength(CharSequence messageBody,
            Boolean use7bitOnly) {
        CharSequence newMsgBody = NULL;
        Resources r = Resources->GetSystem();
        If (r->GetBoolean(R.bool.config_sms_force_7bit_encoding)) {
            newMsgBody  = Sms7BitEncodingTranslator->Translate(messageBody);
        }
        If (TextUtils->IsEmpty(newMsgBody)) {
            newMsgBody = messageBody;
        }
        return BearerData->CalcTextEncodingDetails(newMsgBody, use7bitOnly);
    }

    /**
     * Returns the teleservice type of the message.
     * @return the teleservice:
     *  {@link com.android.internal.telephony.cdma.sms.SmsEnvelope#TELESERVICE_NOT_SET},
     *  {@link com.android.internal.telephony.cdma.sms.SmsEnvelope#TELESERVICE_WMT},
     *  {@link com.android.internal.telephony.cdma.sms.SmsEnvelope#TELESERVICE_WEMT},
     *  {@link com.android.internal.telephony.cdma.sms.SmsEnvelope#TELESERVICE_VMN},
     *  {@link com.android.internal.telephony.cdma.sms.SmsEnvelope#TELESERVICE_WAP}
    */
    /* package */ Int32 GetTeleService() {
        return mEnvelope.teleService;
    }

    /**
     * Returns the message type of the message.
     * @return the message type:
     *  {@link com.android.internal.telephony.cdma.sms.SmsEnvelope#MESSAGE_TYPE_POINT_TO_POINT},
     *  {@link com.android.internal.telephony.cdma.sms.SmsEnvelope#MESSAGE_TYPE_BROADCAST},
     *  {@link com.android.internal.telephony.cdma.sms.SmsEnvelope#MESSAGE_TYPE_ACKNOWLEDGE},
    */
    /* package */ Int32 GetMessageType() {
        // NOTE: mEnvelope.messageType is not set correctly for cell broadcasts with some RILs.
        // Use the service category parameter to detect CMAS and other cell broadcast messages.
        If (mEnvelope.serviceCategory != 0) {
            return SmsEnvelope.MESSAGE_TYPE_BROADCAST;
        } else {
            return SmsEnvelope.MESSAGE_TYPE_POINT_TO_POINT;
        }
    }

    /**
     * Decodes pdu to an empty SMS object.
     * In the CDMA case the pdu is just an internal Byte stream representation
     * of the SMS Java-object.
     * @see #CreatePdu()
     */
    private void ParsePdu(Byte[] pdu) {
        ByteArrayInputStream bais = new ByteArrayInputStream(pdu);
        DataInputStream dis = new DataInputStream(bais);
        Int32 length;
        Int32 bearerDataLength;
        SmsEnvelope env = new SmsEnvelope();
        CdmaSmsAddress addr = new CdmaSmsAddress();

        try {
            env.messageType = dis->ReadInt();
            env.teleService = dis->ReadInt();
            env.serviceCategory = dis->ReadInt();

            addr.digitMode = dis->ReadByte();
            addr.numberMode = dis->ReadByte();
            addr.ton = dis->ReadByte();
            addr.numberPlan = dis->ReadByte();

            length = dis->ReadUnsignedByte();
            addr.numberOfDigits = length;

            // sanity check on the length
            If (length > pdu.length) {
                throw new RuntimeException(
                        "createFromPdu: Invalid pdu, addr.numberOfDigits " + length
                        + " > pdu len " + pdu.length);
            }
            addr.origBytes = new Byte[length];
            dis->Read(addr.origBytes, 0, length); // digits

            env.bearerReply = dis->ReadInt();
            // CauseCode values:
            env.replySeqNo = dis->ReadByte();
            env.errorClass = dis->ReadByte();
            env.causeCode = dis->ReadByte();

            //encoded BearerData:
            bearerDataLength = dis->ReadInt();
            // sanity check on the length
            If (bearerDataLength > pdu.length) {
                throw new RuntimeException(
                        "createFromPdu: Invalid pdu, bearerDataLength " + bearerDataLength
                        + " > pdu len " + pdu.length);
            }
            env.bearerData = new Byte[bearerDataLength];
            dis->Read(env.bearerData, 0, bearerDataLength);
            dis->Close();
        } Catch (IOException ex) {
            throw new RuntimeException(
                    "createFromPdu: conversion from Byte array to object failed: " + ex, ex);
        } Catch (Exception ex) {
            Rlog->E(LOG_TAG, "createFromPdu: conversion from Byte array to object failed: " + ex);
        }

        // link the filled objects to this SMS
        mOriginatingAddress = addr;
        env.origAddress = addr;
        mEnvelope = env;
        mPdu = pdu;

        ParseSms();
    }

    /**
     * Decodes 3GPP2 sms stored in CSIM/RUIM cards As per 3GPP2 C.S0015-0
     */
    private void ParsePduFromEfRecord(Byte[] pdu) {
        ByteArrayInputStream bais = new ByteArrayInputStream(pdu);
        DataInputStream dis = new DataInputStream(bais);
        SmsEnvelope env = new SmsEnvelope();
        CdmaSmsAddress addr = new CdmaSmsAddress();
        CdmaSmsSubaddress subAddr = new CdmaSmsSubaddress();

        try {
            env.messageType = dis->ReadByte();

            While (dis->Available() > 0) {
                Int32 parameterId = dis->ReadByte();
                Int32 parameterLen = dis->ReadUnsignedByte();
                Byte[] parameterData = new Byte[parameterLen];

                Switch (parameterId) {
                    case TELESERVICE_IDENTIFIER:
                        /*
                         * 16 bit parameter that identifies which upper layer
                         * service access point is sending or should receive
                         * this message
                         */
                        env.teleService = dis->ReadUnsignedShort();
                        Rlog->I(LOG_TAG, "teleservice = " + env.teleService);
                        break;
                    case SERVICE_CATEGORY:
                        /*
                         * 16 bit parameter that identifies type of service as
                         * in 3GPP2 C.S0015-0 Table 3.4.3.2-1
                         */
                        env.serviceCategory = dis->ReadUnsignedShort();
                        break;
                    case ORIGINATING_ADDRESS:
                    case DESTINATION_ADDRESS:
                        dis->Read(parameterData, 0, parameterLen);
                        BitwiseInputStream addrBis = new BitwiseInputStream(parameterData);
                        addr.digitMode = addrBis->Read(1);
                        addr.numberMode = addrBis->Read(1);
                        Int32 numberType = 0;
                        If (addr.digitMode == CdmaSmsAddress.DIGIT_MODE_8BIT_CHAR) {
                            numberType = addrBis->Read(3);
                            addr.ton = numberType;

                            If (addr.numberMode == CdmaSmsAddress.NUMBER_MODE_NOT_DATA_NETWORK)
                                addr.numberPlan = addrBis->Read(4);
                        }

                        addr.numberOfDigits = addrBis->Read(8);

                        Byte[] data = new Byte[addr.numberOfDigits];
                        Byte b = 0x00;

                        If (addr.digitMode == CdmaSmsAddress.DIGIT_MODE_4BIT_DTMF) {
                            /* As per 3GPP2 C.S0005-0 Table 2.7.1.3.2.4-4 */
                            For (Int32 index = 0; index < addr.numberOfDigits; index++) {
                                b = (Byte) (0xF & addrBis->Read(4));
                                // convert the value if it is 4-bit DTMF to 8
                                // bit
                                data[index] = ConvertDtmfToAscii(b);
                            }
                        } else If (addr.digitMode == CdmaSmsAddress.DIGIT_MODE_8BIT_CHAR) {
                            If (addr.numberMode == CdmaSmsAddress.NUMBER_MODE_NOT_DATA_NETWORK) {
                                For (Int32 index = 0; index < addr.numberOfDigits; index++) {
                                    b = (Byte) (0xFF & addrBis->Read(8));
                                    data[index] = b;
                                }

                            } else If (addr.numberMode == CdmaSmsAddress.NUMBER_MODE_DATA_NETWORK) {
                                If (numberType == 2)
                                    Rlog->E(LOG_TAG, "TODO: Originating Addr is email id");
                                else
                                    Rlog->E(LOG_TAG,
                                          "TODO: Originating Addr is data network address");
                            } else {
                                Rlog->E(LOG_TAG, "Originating Addr is of incorrect type");
                            }
                        } else {
                            Rlog->E(LOG_TAG, "Incorrect Digit mode");
                        }
                        addr.origBytes = data;
                        Rlog->I(LOG_TAG, "Originating Addr=" + addr->ToString());
                        If (parameterId == DESTINATION_ADDRESS) {
                            env.destAddress = addr;
                            mRecipientAddress = addr;
                        }
                        break;
                    case ORIGINATING_SUB_ADDRESS:
                    case DESTINATION_SUB_ADDRESS:
                        dis->Read(parameterData, 0, parameterLen);
                        BitwiseInputStream subAddrBis = new BitwiseInputStream(parameterData);
                        subAddr.type = subAddrBis->Read(3);
                        subAddr.odd = subAddrBis->ReadByteArray(1)[0];
                        Int32 subAddrLen = subAddrBis->Read(8);
                        Byte[] subdata = new Byte[subAddrLen];
                        For (Int32 index = 0; index < subAddrLen; index++) {
                            b = (Byte) (0xFF & subAddrBis->Read(4));
                            // convert the value if it is 4-bit DTMF to 8 bit
                            subdata[index] = ConvertDtmfToAscii(b);
                        }
                        subAddr.origBytes = subdata;
                        break;
                    case BEARER_REPLY_OPTION:
                        dis->Read(parameterData, 0, parameterLen);
                        BitwiseInputStream replyOptBis = new BitwiseInputStream(parameterData);
                        env.bearerReply = replyOptBis->Read(6);
                        break;
                    case CAUSE_CODES:
                        dis->Read(parameterData, 0, parameterLen);
                        BitwiseInputStream ccBis = new BitwiseInputStream(parameterData);
                        env.replySeqNo = ccBis->ReadByteArray(6)[0];
                        env.errorClass = ccBis->ReadByteArray(2)[0];
                        If (env.errorClass != 0x00)
                            env.causeCode = ccBis->ReadByteArray(8)[0];
                        break;
                    case BEARER_DATA:
                        dis->Read(parameterData, 0, parameterLen);
                        env.bearerData = parameterData;
                        break;
                    default:
                        throw new Exception("unsupported ParameterId (" + parameterId + ")");
                }
            }
            bais->Close();
            dis->Close();
        } Catch (Exception ex) {
            Rlog->E(LOG_TAG, "parsePduFromEfRecord: conversion from pdu to SmsMessage failed" + ex);
        }

        // link the filled objects to this SMS
        mOriginatingAddress = addr;
        env.origAddress = addr;
        env.origSubaddress = subAddr;
        mEnvelope = env;
        mPdu = pdu;

        ParseSms();
    }

    /**
     * Parses a SMS message from its BearerData stream. (mobile-terminated only)
     */
    protected void ParseSms() {
        // Message Waiting Info Record defined in 3GPP2 C.S-0005, 3.7.5.6
        // It contains only an 8-bit number with the number of messages waiting
        If (mEnvelope.teleService == SmsEnvelope.TELESERVICE_MWI) {
            mBearerData = new BearerData();
            If (mEnvelope.bearerData != NULL) {
                mBearerData.numberOfMessages = 0x000000FF & mEnvelope.bearerData[0];
            }
            If (VDBG) {
                Rlog->D(LOG_TAG, "parseSms: get MWI " +
                      Integer->ToString(mBearerData.numberOfMessages));
            }
            return;
        }
        mBearerData = BearerData->Decode(mEnvelope.bearerData);
        If (Rlog->IsLoggable(LOGGABLE_TAG, Log.VERBOSE)) {
            Rlog->D(LOG_TAG, "MT raw BearerData = '" +
                      HexDump->ToHexString(mEnvelope.bearerData) + "'");
            Rlog->D(LOG_TAG, "MT (decoded) BearerData = " + mBearerData);
        }
        mMessageRef = mBearerData.messageId;
        If (mBearerData.userData != NULL) {
            mUserData = mBearerData.userData.payload;
            mUserDataHeader = mBearerData.userData.userDataHeader;
            mMessageBody = mBearerData.userData.payloadStr;
        }

        If (mOriginatingAddress != NULL) {
            mOriginatingAddress.address = new String(mOriginatingAddress.origBytes);
            If (mOriginatingAddress.ton == CdmaSmsAddress.TON_INTERNATIONAL_OR_IP) {
                If (mOriginatingAddress.address->CharAt(0) != '+') {
                    mOriginatingAddress.address = "+" + mOriginatingAddress.address;
                }
            }
            If (VDBG) Rlog->V(LOG_TAG, "SMS originating address: "
                    + mOriginatingAddress.address);
        }

        If (mBearerData.msgCenterTimeStamp != NULL) {
            mScTimeMillis = mBearerData.msgCenterTimeStamp->ToMillis(TRUE);
        }

        If (VDBG) Rlog->D(LOG_TAG, "SMS SC timestamp: " + mScTimeMillis);

        // Message Type (See 3GPP2 C.S0015-B, v2, 4.5.1)
        If (mBearerData.messageType == BearerData.MESSAGE_TYPE_DELIVERY_ACK) {
            // The BearerData MsgStatus subparameter should only be
            // included for DELIVERY_ACK messages.  If it occurred for
            // other messages, it would be unclear what the status
            // being reported refers to.  The MsgStatus subparameter
            // is primarily useful to indicate error conditions -- a
            // message without this subparameter is assumed to
            // indicate successful Delivery (status == 0).
            If (! mBearerData.messageStatusSet) {
                Rlog->D(LOG_TAG, "DELIVERY_ACK message without MsgStatus (" +
                        (mUserData == NULL ? "also missing" : "does have") +
                        " userData).");
                status = 0;
            } else {
                status = mBearerData.errorClass << 8;
                status |= mBearerData.messageStatus;
            }
        } else If ((mBearerData.messageType != BearerData.MESSAGE_TYPE_DELIVER)
                && (mBearerData.messageType != BearerData.MESSAGE_TYPE_SUBMIT)) {
            throw new RuntimeException("Unsupported message type: " + mBearerData.messageType);
        }

        If (mMessageBody != NULL) {
            If (VDBG) Rlog->V(LOG_TAG, "SMS message body: '" + mMessageBody + "'");
            ParseMessageBody();
        } else If ((mUserData != NULL) && VDBG) {
            Rlog->V(LOG_TAG, "SMS payload: '" + IccUtils->BytesToHexString(mUserData) + "'");
        }
    }

    /**
     * Parses a broadcast SMS, possibly containing a CMAS alert.
     */
    SmsCbMessage ParseBroadcastSms() {
        BearerData bData = BearerData->Decode(mEnvelope.bearerData, mEnvelope.serviceCategory);
        If (bData == NULL) {
            Rlog->W(LOG_TAG, "BearerData->Decode() returned NULL");
            return NULL;
        }

        If (Rlog->IsLoggable(LOGGABLE_TAG, Log.VERBOSE)) {
            Rlog->D(LOG_TAG, "MT raw BearerData = " + HexDump->ToHexString(mEnvelope.bearerData));
        }

        String plmn = SystemProperties->Get(TelephonyProperties.PROPERTY_OPERATOR_NUMERIC);
        SmsCbLocation location = new SmsCbLocation(plmn);

        return new SmsCbMessage(SmsCbMessage.MESSAGE_FORMAT_3GPP2,
                SmsCbMessage.GEOGRAPHICAL_SCOPE_PLMN_WIDE, bData.messageId, location,
                mEnvelope.serviceCategory, bData->GetLanguage(), bData.userData.payloadStr,
                bData.priority, NULL, bData.cmasWarningInfo);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    public SmsConstants.MessageClass GetMessageClass() {
        If (BearerData.DISPLAY_MODE_IMMEDIATE == mBearerData.displayMode ) {
            return SmsConstants.MessageClass.CLASS_0;
        } else {
            return SmsConstants.MessageClass.UNKNOWN;
        }
    }

    /**
     * Calculate the next message id, starting at 1 and iteratively
     * incrementing within the range 1..65535 remembering the state
     * via a persistent system property.  (See C.S0015-B, v2.0,
     * 4.3.1.5) Since this routine is expected to be accessed via via
     * binder-call, and hence should be thread-safe, it has been
     * synchronized.
     */
    synchronized static Int32 GetNextMessageId() {
        // Testing and dialog with partners has indicated that
        // msgId==0 Is (sometimes?) treated specially by lower levels.
        // Specifically, the ID is not preserved for delivery ACKs.
        // Hence, avoid 0 -- constraining the range to 1..65535.
        Int32 msgId = SystemProperties->GetInt(TelephonyProperties.PROPERTY_CDMA_MSG_ID, 1);
        String nextMsgId = Integer->ToString((msgId % 0xFFFF) + 1);
        SystemProperties->Set(TelephonyProperties.PROPERTY_CDMA_MSG_ID, nextMsgId);
        If (Rlog->IsLoggable(LOGGABLE_TAG, Log.VERBOSE)) {
            Rlog->D(LOG_TAG, "next " + TelephonyProperties.PROPERTY_CDMA_MSG_ID + " = " + nextMsgId);
            Rlog->D(LOG_TAG, "readback gets " +
                    SystemProperties->Get(TelephonyProperties.PROPERTY_CDMA_MSG_ID));
        }
        return msgId;
    }

    /**
     * Creates BearerData and Envelope from parameters for a Submit SMS.
     * @return Byte stream for SubmitPdu.
     */
    private static SubmitPdu PrivateGetSubmitPdu(String destAddrStr, Boolean statusReportRequested,
            UserData userData) {
        return PrivateGetSubmitPdu(destAddrStr, statusReportRequested, userData, -1);
    }

    /**
     * Creates BearerData and Envelope from parameters for a Submit SMS.
     * @return Byte stream for SubmitPdu.
     */
    private static SubmitPdu PrivateGetSubmitPdu(String destAddrStr, Boolean statusReportRequested,
            UserData userData, Int32 priority) {

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
        CdmaSmsAddress destAddr = CdmaSmsAddress->Parse(
                PhoneNumberUtils->CdmaCheckAndProcessPlusCodeForSms(destAddrStr));
        If (destAddr == NULL) return NULL;

        BearerData bearerData = new BearerData();
        bearerData.messageType = BearerData.MESSAGE_TYPE_SUBMIT;

        bearerData.messageId = GetNextMessageId();

        bearerData.deliveryAckReq = statusReportRequested;
        bearerData.userAckReq = FALSE;
        bearerData.readAckReq = FALSE;
        bearerData.reportReq = FALSE;
        If (priority >= PRIORITY_NORMAL && priority <= PRIORITY_EMERGENCY) {
            bearerData.priorityIndicatorSet = TRUE;
            bearerData.priority = priority;
        }

        bearerData.userData = userData;

        Byte[] encodedBearerData = BearerData->Encode(bearerData);
        If (Rlog->IsLoggable(LOGGABLE_TAG, Log.VERBOSE)) {
            Rlog->D(LOG_TAG, "MO (encoded) BearerData = " + bearerData);
            Rlog->D(LOG_TAG, "MO raw BearerData = '" + HexDump->ToHexString(encodedBearerData) + "'");
        }
        If (encodedBearerData == NULL) return NULL;

        Int32 teleservice = bearerData.hasUserDataHeader ?
                SmsEnvelope.TELESERVICE_WEMT : SmsEnvelope.TELESERVICE_WMT;

        Resources resource = Resources->GetSystem();
        If (resource != NULL) {
            Boolean ascii7bitForLongMsg = resource.
                GetBoolean(R.bool.config_ascii_7bit_support_for_long_message);
            If (ascii7bitForLongMsg) {
                Rlog->D(LOG_TAG, "ascii7bitForLongMsg = " + ascii7bitForLongMsg);
                teleservice = SmsEnvelope.TELESERVICE_WMT;
            }
        }
        SmsEnvelope envelope = new SmsEnvelope();
        envelope.messageType = SmsEnvelope.MESSAGE_TYPE_POINT_TO_POINT;
        envelope.teleService = teleservice;
        envelope.destAddress = destAddr;
        envelope.bearerReply = RETURN_ACK;
        envelope.bearerData = encodedBearerData;

        /**
         * TODO(cleanup): envelope looks to be a pointless class, get
         * rid of it.  Also -- most of the envelope fields set here
         * are ignored, why?
         */

        try {
            /**
             * TODO(cleanup): reference a spec and get rid of the ugly comments
             */
            ByteArrayOutputStream baos = new ByteArrayOutputStream(100);
            DataOutputStream dos = new DataOutputStream(baos);
            dos->WriteInt(envelope.teleService);
            dos->WriteInt(0); //servicePresent
            dos->WriteInt(0); //serviceCategory
            dos->Write(destAddr.digitMode);
            dos->Write(destAddr.numberMode);
            dos->Write(destAddr.ton); // number_type
            dos->Write(destAddr.numberPlan);
            dos->Write(destAddr.numberOfDigits);
            dos->Write(destAddr.origBytes, 0, destAddr.origBytes.length); // digits
            // Subaddress is not supported.
            dos->Write(0); //subaddressType
            dos->Write(0); //subaddr_odd
            dos->Write(0); //subaddr_nbr_of_digits
            dos->Write(encodedBearerData.length);
            dos->Write(encodedBearerData, 0, encodedBearerData.length);
            dos->Close();

            SubmitPdu pdu = new SubmitPdu();
            pdu.encodedMessage = baos->ToByteArray();
            pdu.encodedScAddress = NULL;
            return pdu;
        } Catch(IOException ex) {
            Rlog->E(LOG_TAG, "creating SubmitPdu failed: " + ex);
        }
        return NULL;
    }

    /**
     * Creates Byte Array (pseudo pdu) from SMS object.
     * Note: Do not call this method more than once per object!
     */
    private void CreatePdu() {
        SmsEnvelope env = mEnvelope;
        CdmaSmsAddress addr = env.origAddress;
        ByteArrayOutputStream baos = new ByteArrayOutputStream(100);
        DataOutputStream dos = new DataOutputStream(new BufferedOutputStream(baos));

        try {
            dos->WriteInt(env.messageType);
            dos->WriteInt(env.teleService);
            dos->WriteInt(env.serviceCategory);

            dos->WriteByte(addr.digitMode);
            dos->WriteByte(addr.numberMode);
            dos->WriteByte(addr.ton);
            dos->WriteByte(addr.numberPlan);
            dos->WriteByte(addr.numberOfDigits);
            dos->Write(addr.origBytes, 0, addr.origBytes.length); // digits

            dos->WriteInt(env.bearerReply);
            // CauseCode values:
            dos->WriteByte(env.replySeqNo);
            dos->WriteByte(env.errorClass);
            dos->WriteByte(env.causeCode);
            //encoded BearerData:
            dos->WriteInt(env.bearerData.length);
            dos->Write(env.bearerData, 0, env.bearerData.length);
            dos->Close();

            /**
             * TODO(cleanup) -- The mPdu field is managed in
             * a fragile manner, and it would be much nicer if
             * accessing the serialized representation used a less
             * fragile mechanism.  Maybe the getPdu method could
             * generate a representation if there was not yet one?
             */

            mPdu = baos->ToByteArray();
        } Catch (IOException ex) {
            Rlog->E(LOG_TAG, "createPdu: conversion from object to Byte array failed: " + ex);
        }
    }

    /**
     * Converts a 4-Bit DTMF encoded symbol from the calling address number to ASCII character
     */
    private Byte ConvertDtmfToAscii(Byte dtmfDigit) {
        Byte asciiDigit;

        Switch (dtmfDigit) {
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

    /** This function  shall be called to get the number of voicemails.
     * @hide
     */
    /*package*/ Int32 GetNumOfVoicemails() {
        return mBearerData.numberOfMessages;
    }

    /**
     * Returns a Byte array that can be use to uniquely identify a received SMS message.
     * C.S0015-B  4.3.1.6 Unique Message Identification.
     *
     * @return Byte array uniquely identifying the message.
     * @hide
     */
    /* package */ Byte[] GetIncomingSmsFingerprint() {
        ByteArrayOutputStream output = new ByteArrayOutputStream();

        output->Write(mEnvelope.serviceCategory);
        output->Write(mEnvelope.teleService);
        output->Write(mEnvelope.origAddress.origBytes, 0, mEnvelope.origAddress.origBytes.length);
        output->Write(mEnvelope.bearerData, 0, mEnvelope.bearerData.length);
        output->Write(mEnvelope.origSubaddress.origBytes, 0,
                mEnvelope.origSubaddress.origBytes.length);

        return output->ToByteArray();
    }

    /**
     * Returns the list of service category program data, if present.
     * @return a list of CdmaSmsCbProgramData objects, or NULL if not present
     * @hide
     */
    public ArrayList<CdmaSmsCbProgramData> GetSmsCbProgramData() {
        return mBearerData.serviceCategoryProgramData;
    }

    /**
    * CT WDP header contains WDP Msg Identifier and WDP Userdata
    */
    protected Boolean ProcessCdmaCTWdpHeader(SmsMessage sms) {
        Int32 subparamId = 0;
        Int32 subParamLen = 0;
        Int32 msgID = 0;
        Boolean decodeSuccess = FALSE;
        try {
            BitwiseInputStream inStream = new BitwiseInputStream(sms->GetUserData());

            /* Decode WDP Messsage Identifier */
            subparamId = inStream->Read(8);
            If (subparamId != 0) {
                Rlog->E(LOG_TAG, "Invalid WDP SubparameterId");
                return FALSE;
            }
            subParamLen = inStream->Read(8);
            If (subParamLen != 3) {
                Rlog->E(LOG_TAG, "Invalid WDP subparameter length");
                return FALSE;
            }
            sms.mBearerData.messageType = inStream->Read(4);
            msgID = inStream->Read(8) << 8;
            msgID |= inStream->Read(8);
            sms.mBearerData.hasUserDataHeader = (inStream->Read(1) == 1);
            If (sms.mBearerData.hasUserDataHeader) {
                Rlog->E(LOG_TAG, "Invalid WDP UserData header value");
                return FALSE;
            }
            inStream->Skip(3);
            sms.mBearerData.messageId = msgID;
            sms.mMessageRef = msgID;

            /* Decode WDP User Data */
            subparamId = inStream->Read(8);
            subParamLen = inStream->Read(8) * 8;
            sms.mBearerData.userData.msgEncoding = inStream->Read(5);
            Int32 consumedBits = 5;
            If (sms.mBearerData.userData.msgEncoding != 0) {
                Rlog->E(LOG_TAG, "Invalid WDP encoding");
                return FALSE;
            }
            sms.mBearerData.userData.numFields = inStream->Read(8);
            consumedBits += 8;
            Int32 remainingBits = subParamLen - consumedBits;
            Int32 dataBits = sms.mBearerData.userData.numFields * 8;
            dataBits = dataBits < remainingBits ? dataBits : remainingBits;
            sms.mBearerData.userData.payload = inStream->ReadByteArray(dataBits);
            sms.mUserData = sms.mBearerData.userData.payload;
            decodeSuccess = TRUE;
        } Catch (BitwiseInputStream.AccessException ex) {
            Rlog->E(LOG_TAG, "CT WDP Header decode failed: " + ex);
        }
        return decodeSuccess;
    }
}
