#ifndef __ELASTOS_DROID_TELEPHONY_SMSMESSAGE_H__
#define __ELASTOS_DROID_TELEPHONY_SMSMESSAGE_H__

#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Internal::Telephony::ISmsMessageBaseSubmitPduBase;
using Elastos::Droid::Internal::Telephony::ISmsMessageBase;
using Elastos::Droid::Internal::Telephony::MessageClass;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Telephony {

/**
  * A Short Message Service message.
  * @see android.provider.Telephony.Sms.Intents#getMessagesFromIntent
  */
class SmsMessage
    : public Object
    , public ISmsMessage
{
public:
    class SubmitPdu
        : public ISmsMessageSubmitPdu
        , public Object
    {
        friend class SmsMessage;
    public:
        CAR_INTERFACE_DECL();

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    protected:
        /**
          * @hide
          */
        SubmitPdu(
            /* [in] */ ISmsMessageBaseSubmitPduBase* spb);

    public:
        AutoPtr<ArrayOf<Byte> > encodedScAddress;
        // Null if not applicable.
        AutoPtr<ArrayOf<Byte> > encodedMessage;
    };

    class NoEmsSupportConfig
        : public Object
    {
    public:
        NoEmsSupportConfig(
            /* [in] */ ArrayOf<String>* config);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    public:
        String mOperatorNumber;
        String mGid1;
        Boolean mIsPrefix;
    };

public:
    CAR_INTERFACE_DECL();

    /** set Subscription information
      *
      * @hide
      */
    virtual CARAPI SetSubId(
        /* [in] */ Int64 subId);

    /** get Subscription information
      *
      * @hide
      */
    virtual CARAPI GetSubId(
        /* [out] */ Int64* result);

    /**
      * Create an SmsMessage from a raw PDU.
      *
      * <p><b>This method will soon be deprecated</b> and all applications which handle
      * incoming SMS messages by processing the {@code SMS_RECEIVED_ACTION} broadcast
      * intent <b>must</b> now pass the new {@code format} String extra from the intent
      * into the new method {@code createFromPdu(byte[], String)} which takes an
      * extra format parameter. This is required in order to correctly decode the PDU on
      * devices that require support for both 3GPP and 3GPP2 formats at the same time,
      * such as dual-mode GSM/CDMA and CDMA/LTE phones.  Guess format based on Voice
      * technology first, if it fails use other format.
      */
    static CARAPI_(AutoPtr<ISmsMessage>) CreateFromPdu(
        /* [in] */ ArrayOf<Byte>* pdu);

    /**
      * Create an SmsMessage from a raw PDU with the specified message format. The
      * message format is passed in the {@code SMS_RECEIVED_ACTION} as the {@code format}
      * String extra, and will be either "3gpp" for GSM/UMTS/LTE messages in 3GPP format
      * or "3gpp2" for CDMA/LTE messages in 3GPP2 format.
      *
      * @param pdu the message PDU from the SMS_RECEIVED_ACTION intent
      * @param format the format extra from the SMS_RECEIVED_ACTION intent
      * @hide pending API council approval
      */
    static CARAPI_(AutoPtr<ISmsMessage>) CreateFromPdu(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ const String& format);

    /**
      * TS 27.005 3.4.1 lines[0] and lines[1] are the two lines read from the
      * +CMT unsolicited response (PDU mode, of course)
      *  +CMT: [&lt;alpha>],<length><CR><LF><pdu>
      *
      * Only public for debugging and for RIL
      *
      * {@hide}
      */
    static CARAPI_(AutoPtr<ISmsMessage>) NewFromCMT(
        /* [in] */ ArrayOf<String>* lines);

    /** @hide */
    static CARAPI_(AutoPtr<ISmsMessage>) NewFromParcel(
        /* [in] */ IParcel* p);

    /**
      * Create an SmsMessage from an SMS EF record.
      *
      * @param index Index of SMS record. This should be index in ArrayList
      *              returned by SmsManager.getAllMessagesFromSim + 1.
      * @param data Record data.
      * @return An SmsMessage representing the record.
      *
      * @hide
      */
    static CARAPI_(AutoPtr<ISmsMessage>) CreateFromEfRecord(
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Byte>* data);

    /**
      * Create an SmsMessage from an SMS EF record.
      *
      * @param index Index of SMS record. This should be index in ArrayList
      *              returned by SmsManager.getAllMessagesFromSim + 1.
      * @param data Record data.
      * @param subId Subscription Id of the SMS
      * @return An SmsMessage representing the record.
      *
      * @hide
      */
    static CARAPI_(AutoPtr<ISmsMessage>) CreateFromEfRecord(
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int64 subId);

    /**
      * Get the TP-Layer-Length for the given SMS-SUBMIT PDU Basically, the
      * length in bytes (not hex chars) less the SMSC header
      *
      * FIXME: This method is only used by a CTS test case that isn't run on CDMA devices.
      * We should probably deprecate it and remove the obsolete test case.
      */
    static CARAPI_(Int32) GetTPLayerLengthForPDU(
        /* [in] */ const String& pdu);

    /*
      * TODO(cleanup): It would make some sense if the result of
      * preprocessing a message to determine the proper encoding (i.e.
      * the resulting data structure from calculateLength) could be
      * passed as an argument to the actual final encoding function.
      * This would better ensure that the logic behind size calculation
      * actually matched the encoding.
      */
    /**
      * Calculates the number of SMS's required to encode the message body and
      * the number of characters remaining until the next message.
      *
      * @param msgBody the message to encode
      * @param use7bitOnly if true, characters that are not part of the
      *         radio-specific 7-bit encoding are counted as single
      *         space chars.  If false, and if the messageBody contains
      *         non-7-bit encodable characters, length is calculated
      *         using a 16-bit encoding.
      * @return an int[4] with int[0] being the number of SMS's
      *         required, int[1] the number of code units used, and
      *         int[2] is the number of code units remaining until the
      *         next message. int[3] is an indicator of the encoding
      *         code unit size (see the ENCODING_* definitions in SmsConstants)
      */
    static CARAPI_(AutoPtr<ArrayOf<Int32> >) CalculateLength(
        /* [in] */ ICharSequence* msgBody,
        /* [in] */ Boolean use7bitOnly);

    /**
      * Divide a message text into several fragments, none bigger than
      * the maximum SMS message text size.
      *
      * @param text text, must not be null.
      * @return an <code>ArrayList</code> of strings that, in order,
      *   comprise the original msg text
      *
      * @hide
      * return list of String
      */
    static CARAPI_(AutoPtr<IArrayList>) FragmentText(
        /* [in] */ const String& text);

    /**
      * Calculates the number of SMS's required to encode the message body and
      * the number of characters remaining until the next message, given the
      * current encoding.
      *
      * @param messageBody the message to encode
      * @param use7bitOnly if true, characters that are not part of the radio
      *         specific (GSM / CDMA) alphabet encoding are converted to as a
      *         single space characters. If false, a messageBody containing
      *         non-GSM or non-CDMA alphabet characters are encoded using
      *         16-bit encoding.
      * @return an int[4] with int[0] being the number of SMS's required, int[1]
      *         the number of code units used, and int[2] is the number of code
      *         units remaining until the next message. int[3] is the encoding
      *         type that should be used for the message.
      */
    static CARAPI_(AutoPtr<ArrayOf<Int32> >) CalculateLength(
        /* [in] */ const String& messageBody,
        /* [in] */ Boolean use7bitOnly);

    /*
      * TODO(cleanup): It looks like there is now no useful reason why
      * apps should generate pdus themselves using these routines,
      * instead of handing the raw data to SMSDispatcher (and thereby
      * have the phone process do the encoding).  Moreover, CDMA now
      * has shared state (in the form of the msgId system property)
      * which can only be modified by the phone process, and hence
      * makes the output of these routines incorrect.  Since they now
      * serve no purpose, they should probably just return null
      * directly, and be deprecated.  Going further in that direction,
      * the above parsers of serialized pdu data should probably also
      * be gotten rid of, hiding all but the necessarily visible
      * structured data from client apps.  A possible concern with
      * doing this is that apps may be using these routines to generate
      * pdus that are then sent elsewhere, some network server, for
      * example, and that always returning null would thereby break
      * otherwise useful apps.
      */
    /**
      * Get an SMS-SUBMIT PDU for a destination address and a message.
      * This method will not attempt to use any GSM national language 7 bit encodings.
      *
      * @param scAddress Service Centre address.  Null means use default.
      * @return a <code>SubmitPdu</code> containing the encoded SC
      *         address, if applicable, and the encoded message.
      *         Returns null on encode error.
      */
    static CARAPI_(AutoPtr<ISmsMessageSubmitPdu>) GetSubmitPdu(
        /* [in] */ const String& scAddress,
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& message,
        /* [in] */ Boolean statusReportRequested);

    /**
      * Get an SMS-SUBMIT PDU for a destination address and a message.
      * This method will not attempt to use any GSM national language 7 bit encodings.
      *
      * @param scAddress Service Centre address.  Null means use default.
      * @param subId Subscription of the message
      * @return a <code>SubmitPdu</code> containing the encoded SC
      *         address, if applicable, and the encoded message.
      *         Returns null on encode error.
      * @hide
      */
    static CARAPI_(AutoPtr<ISmsMessageSubmitPdu>) GetSubmitPdu(
        /* [in] */ const String& scAddress,
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& message,
        /* [in] */ Boolean statusReportRequested,
        /* [in] */ Int64 subId);

    /**
      * Get an SMS-SUBMIT PDU for a data message to a destination address &amp; port.
      * This method will not attempt to use any GSM national language 7 bit encodings.
      *
      * @param scAddress Service Centre address. null == use default
      * @param destinationAddress the address of the destination for the message
      * @param destinationPort the port to deliver the message to at the
      *        destination
      * @param data the data for the message
      * @return a <code>SubmitPdu</code> containing the encoded SC
      *         address, if applicable, and the encoded message.
      *         Returns null on encode error.
      */
    static CARAPI_(AutoPtr<ISmsMessageSubmitPdu>) GetSubmitPdu(
        /* [in] */ const String& scAddress,
        /* [in] */ const String& destinationAddress,
        /* [in] */ Int16 destinationPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Boolean statusReportRequested);

    /**
      * Returns the address of the SMS service center that relayed this message
      * or null if there is none.
      */
    virtual CARAPI GetServiceCenterAddress(
        /* [out] */ String* result);

    /**
      * Returns the originating address (sender) of this SMS message in String
      * form or null if unavailable
      */
    virtual CARAPI GetOriginatingAddress(
        /* [out] */ String* result);

    /**
      * Returns the originating address, or email from address if this message
      * was from an email gateway. Returns null if originating address
      * unavailable.
      */
    virtual CARAPI GetDisplayOriginatingAddress(
        /* [out] */ String* result);

    /**
      * Returns the message body as a String, if it exists and is text based.
      * @return message body is there is one, otherwise null
      */
    virtual CARAPI GetMessageBody(
        /* [out] */ String* result);

    /**
      * Returns the class of this message.
      */
    virtual CARAPI GetMessageClass(
        /* [out] */ MessageClass* result);

    /**
      * Returns the message body, or email message body if this message was from
      * an email gateway. Returns null if message body unavailable.
      */
    virtual CARAPI GetDisplayMessageBody(
        /* [out] */ String* result);

    /**
      * Unofficial convention of a subject line enclosed in parens empty string
      * if not present
      */
    virtual CARAPI GetPseudoSubject(
        /* [out] */ String* result);

    /**
      * Returns the service centre timestamp in currentTimeMillis() format
      */
    virtual CARAPI GetTimestampMillis(
        /* [out] */ Int64* result);

    /**
      * Returns true if message is an email.
      *
      * @return true if this message came through an email gateway and email
      *         sender / subject / parsed body are available
      */
    virtual CARAPI IsEmail(
        /* [out] */ Boolean* result);

    /**
      * @return if isEmail() is true, body of the email sent through the gateway.
      *         null otherwise
      */
    virtual CARAPI GetEmailBody(
        /* [out] */ String* result);

    /**
      * @return if isEmail() is true, email from address of email sent through
      *         the gateway. null otherwise
      */
    virtual CARAPI GetEmailFrom(
        /* [out] */ String* result);

    /**
      * Get protocol identifier.
      */
    virtual CARAPI GetProtocolIdentifier(
        /* [out] */ Int32* result);

    /**
      * See TS 23.040 9.2.3.9 returns true if this is a "replace short message"
      * SMS
      */
    virtual CARAPI IsReplace(
        /* [out] */ Boolean* result);

    /**
      * Returns true for CPHS MWI toggle message.
      *
      * @return true if this is a CPHS MWI toggle message See CPHS 4.2 section
      *         B.4.2
      */
    virtual CARAPI IsCphsMwiMessage(
        /* [out] */ Boolean* result);

    /**
      * returns true if this message is a CPHS voicemail / message waiting
      * indicator (MWI) clear message
      */
    virtual CARAPI IsMWIClearMessage(
        /* [out] */ Boolean* result);

    /**
      * returns true if this message is a CPHS voicemail / message waiting
      * indicator (MWI) set message
      */
    virtual CARAPI IsMWISetMessage(
        /* [out] */ Boolean* result);

    /**
      * returns true if this message is a "Message Waiting Indication Group:
      * Discard Message" notification and should not be stored.
      */
    virtual CARAPI IsMwiDontStore(
        /* [out] */ Boolean* result);

    /**
      * returns the user data section minus the user data header if one was
      * present.
      */
    virtual CARAPI GetUserData(
        /* [out] */ ArrayOf<Byte>** result);

    /**
      * Returns the raw PDU for the message.
      *
      * @return the raw PDU for the message.
      */
    virtual CARAPI GetPdu(
        /* [out] */ ArrayOf<Byte>** result);

    /**
      * Returns the status of the message on the SIM (read, unread, sent, unsent).
      *
      * @return the status of the message on the SIM.  These are:
      *         SmsManager.STATUS_ON_SIM_FREE
      *         SmsManager.STATUS_ON_SIM_READ
      *         SmsManager.STATUS_ON_SIM_UNREAD
      *         SmsManager.STATUS_ON_SIM_SEND
      *         SmsManager.STATUS_ON_SIM_UNSENT
      * @deprecated Use getStatusOnIcc instead.
      */
    // @Deprecated
    virtual CARAPI GetStatusOnSim(
        /* [out] */ Int32* result);

    /**
      * Returns the status of the message on the ICC (read, unread, sent, unsent).
      *
      * @return the status of the message on the ICC.  These are:
      *         SmsManager.STATUS_ON_ICC_FREE
      *         SmsManager.STATUS_ON_ICC_READ
      *         SmsManager.STATUS_ON_ICC_UNREAD
      *         SmsManager.STATUS_ON_ICC_SEND
      *         SmsManager.STATUS_ON_ICC_UNSENT
      */
    virtual CARAPI GetStatusOnIcc(
        /* [out] */ Int32* result);

    /**
      * Returns the record index of the message on the SIM (1-based index).
      * @return the record index of the message on the SIM, or -1 if this
      *         SmsMessage was not created from a SIM SMS EF record.
      * @deprecated Use getIndexOnIcc instead.
      */
    // @Deprecated
    virtual CARAPI GetIndexOnSim(
        /* [out] */ Int32* result);

    /**
      * Returns the record index of the message on the ICC (1-based index).
      * @return the record index of the message on the ICC, or -1 if this
      *         SmsMessage was not created from a ICC SMS EF record.
      */
    virtual CARAPI GetIndexOnIcc(
        /* [out] */ Int32* result);

    /**
      * GSM:
      * For an SMS-STATUS-REPORT message, this returns the status field from
      * the status report.  This field indicates the status of a previously
      * submitted SMS, if requested.  See TS 23.040, 9.2.3.15 TP-Status for a
      * description of values.
      * CDMA:
      * For not interfering with status codes from GSM, the value is
      * shifted to the bits 31-16.
      * The value is composed of an error class (bits 25-24) and a status code (bits 23-16).
      * Possible codes are described in C.S0015-B, v2.0, 4.5.21.
      *
      * @return 0 indicates the previously sent message was received.
      *         See TS 23.040, 9.9.2.3.15 and C.S0015-B, v2.0, 4.5.21
      *         for a description of other possible values.
      */
    virtual CARAPI GetStatus(
        /* [out] */ Int32* result);

    /**
      * Return true iff the message is a SMS-STATUS-REPORT message.
      */
    virtual CARAPI IsStatusReportMessage(
        /* [out] */ Boolean* result);

    /**
      * Returns true iff the <code>TP-Reply-Path</code> bit is set in
      * this message.
      */
    virtual CARAPI IsReplyPathPresent(
        /* [out] */ Boolean* result);

    /**
      * Decide if the carrier supports long SMS.
      * {@hide}
      */
    static CARAPI_(Boolean) HasEmsSupport();

    /**
      * Check where to add " x/y" in each SMS segment, begin or end.
      * {@hide}
      */
    static CARAPI_(Boolean) ShouldAppendPageNumberAsPrefix();

    /**
      * {@hide}
      * Returns the recipient address(receiver) of this SMS message in String form or null if
      * unavailable.
      */
    virtual CARAPI GetRecipientAddress(
        /* [out] */ String* result);

private:
    SmsMessage(
        /* [in] */ ISmsMessageBase* smb);

    /**
      * Determines whether or not to use CDMA format for MO SMS.
      * If SMS over IMS is supported, then format is based on IMS SMS format,
      * otherwise format is based on current phone type.
      *
      * @return true if Cdma format should be used for MO SMS, false otherwise.
      */
    static CARAPI_(Boolean) UseCdmaFormatForMoSms();

    /**
      * Determines whether or not to use CDMA format for MO SMS.
      * If SMS over IMS is supported, then format is based on IMS SMS format,
      * otherwise format is based on current phone type.
      *
      * @param subId Subscription for which phone type is returned.
      *
      * @return true if Cdma format should be used for MO SMS, false otherwise.
      */
    static CARAPI_(Boolean) UseCdmaFormatForMoSms(
        /* [in] */ Int64 subId);

    /**
      * Determines whether or not to current phone type is cdma.
      *
      * @return true if current phone type is cdma, false otherwise.
      */
    static CARAPI_(Boolean) IsCdmaVoice();

    /**
      * Determines whether or not to current phone type is cdma.
      *
      * @param subId Subscription Id of the SMS
      * @return true if current phone type is cdma, false otherwise.
      */
    static CARAPI_(Boolean) IsCdmaVoice(
        /* [in] */ Int64 subId);

    static CARAPI_(Boolean) IsNoEmsSupportConfigListExisted();

public:
    /** Contains actual SmsMessage. Only public for debugging and for framework layer.
      *
      * @hide
      */
    AutoPtr<ISmsMessageBase> mWrappedSmsMessage;

private:
    static const String LOGTAG;
    /** Indicates the subId
      *
      * @hide
      */
    Int64 mSubId;
    static AutoPtr<ArrayOf<NoEmsSupportConfig*> > mNoEmsSupportConfigList;
    static Boolean mIsNoEmsSupportConfigListLoaded;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Telephony::SmsMessage::NoEmsSupportConfig, IInterface)

#endif // __ELASTOS_DROID_TELEPHONY_SMSMESSAGE_H__

