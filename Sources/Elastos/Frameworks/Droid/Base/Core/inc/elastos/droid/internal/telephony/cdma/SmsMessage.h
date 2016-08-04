#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMSMESSAGE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMSMESSAGE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/droid/internal/telephony/SmsMessageBase.h"

// import android.content.res.Resources;
// import android.os.Parcel;
// import android.os.SystemProperties;
// import android.telephony.PhoneNumberUtils;
// import android.telephony.SmsCbLocation;
// import android.telephony.SmsCbMessage;
// import android.telephony.cdma.CdmaSmsCbProgramData;
// import android.telephony.Rlog;
// import android.util.Log;
// import android.text.TextUtils;
// import android.content.res.Resources;
// import com.android.internal.telephony.PhoneFactory;
// import com.android.internal.telephony.TelephonyProperties;
// import com.android.internal.telephony.cdma.sms.CdmaSmsAddress;
// import com.android.internal.telephony.cdma.sms.CdmaSmsSubaddress;
// import com.android.internal.telephony.uicc.IccUtils;
// import com.android.internal.util.BitwiseInputStream;
// import com.android.internal.util.HexDump;
// import com.android.internal.telephony.Sms7BitEncodingTranslator;
// import java.io.BufferedOutputStream;
// import java.io.ByteArrayInputStream;
// import java.io.ByteArrayOutputStream;
// import java.io.DataInputStream;
// import java.io.DataOutputStream;
// import java.io.IOException;

using Elastos::Droid::Internal::Telephony::SmsMessageBase;
using Elastos::Droid::Telephony::ISmsCbMessage;
using Elastos::Droid::Internal::Telephony::ISmsHeader;
using Elastos::Droid::Internal::Telephony::MessageClass;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::IUserData;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::ISmsEnvelope;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::IBearerData;
using Elastos::Droid::Internal::Telephony::IGsmAlphabetTextEncodingDetails;
using Elastos::Droid::Internal::Telephony::ISmsConstants;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

/**
  * TODO(cleanup): these constants are disturbing... are they not just
  * different interpretations on one number?  And if we did not have
  * terrible class name overlap, they would not need to be directly
  * imported like this.  The class in this file could just as well be
  * named CdmaSmsMessage, could it not?
  */
/**
  * TODO(cleanup): internally returning null in many places makes
  * debugging very hard (among many other reasons) and should be made
  * more meaningful (replaced with exceptions for example).  Null
  * returns should only occur at the very outside of the module/class
  * scope.
  */
/**
  * A Short Message Service message.
  *
  */
class SmsMessage
    : public SmsMessageBase
    , public ISmsMessage
{
  friend class CdmaSMSDispatcher;
  friend class CdmaInboundSmsHandler;

public:
    class SubmitPdu
        : public SmsMessageBase::SubmitPduBase
        , public ISmsMessageSubmitPdu
    {
    public:
        CAR_INTERFACE_DECL();
    };

public:
    CAR_INTERFACE_DECL();

    SmsMessage();

    /**
      * Create an SmsMessage from a raw PDU.
      * Note: In CDMA the PDU is just a byte representation of the received Sms.
      */
    static CARAPI_(AutoPtr<ISmsMessage>) CreateFromPdu(
        /* [in] */ ArrayOf<Byte>* pdu);

    /**
      *  Create a "raw" CDMA SmsMessage from a Parcel that was forged in ril.cpp.
      *  Note: Only primitive fields are set.
      */
    static CARAPI_(AutoPtr<ISmsMessage>) NewFromParcel(
        /* [in] */ IParcel* p);

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
    static CARAPI_(AutoPtr<ISmsMessage>) CreateFromEfRecord(
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Byte>* data);

    /**
      * Note: This function is a GSM specific functionality which is not supported in CDMA mode.
      */
    static CARAPI_(Int32) GetTPLayerLengthForPDU(
        /* [in] */ const String& pdu);

    /**
      * TODO(cleanup): why do getSubmitPdu methods take an scAddr input
      * and do nothing with it?  GSM allows us to specify a SC (eg,
      * when responding to an SMS that explicitly requests the response
      * is sent to a specific SC), or pass null to use the default
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
      *         Returns null on encode error.
      * @hide
      */
    static CARAPI_(AutoPtr<SubmitPdu>) GetSubmitPdu(
        /* [in] */ const String& scAddr,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& message,
        /* [in] */ Boolean statusReportRequested,
        /* [in] */ ISmsHeader* smsHeader);

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
      *         Returns null on encode error.
      * @hide
      */
    static CARAPI_(AutoPtr<SubmitPdu>) GetSubmitPdu(
        /* [in] */ const String& scAddr,
        /* [in] */ const String& destAddr,
        /* [in] */ const String& message,
        /* [in] */ Boolean statusReportRequested,
        /* [in] */ ISmsHeader* smsHeader,
        /* [in] */ Int32 priority);

    /**
      * Get an SMS-SUBMIT PDU for a data message to a destination address and port.
      *
      * @param scAddr Service Centre address. null == use default
      * @param destAddr the address of the destination for the message
      * @param destPort the port to deliver the message to at the
      *        destination
      * @param data the data for the message
      * @return a <code>SubmitPdu</code> containing the encoded SC
      *         address, if applicable, and the encoded message.
      *         Returns null on encode error.
      */
    static CARAPI_(AutoPtr<SubmitPdu>) GetSubmitPdu(
        /* [in] */ const String& scAddr,
        /* [in] */ const String& destAddr,
        /* [in] */ Int32 destPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Boolean statusReportRequested);

    /**
      * Get an SMS-SUBMIT PDU for a data message to a destination address and port.
      *
      * @param scAddr Service Centre address. null == use default
      * @param destAddr the address of the destination for the message
      * @param destPort the port to deliver the message to at the
      *        destination
      * @param origPort  the originator port set by the sender
      * @param data the data for the message
      * @return a <code>SubmitPdu</code> containing the encoded SC
      *         address, if applicable, and the encoded message.
      *         Returns null on encode error.
      */
    static CARAPI_(AutoPtr<SubmitPdu>) GetSubmitPdu(
        /* [in] */ const String& scAddr,
        /* [in] */ const String& destAddr,
        /* [in] */ Int32 destPort,
        /* [in] */ Int32 origPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Boolean statusReportRequested);

    /**
      * Get an SMS-SUBMIT PDU for a data message to a destination address &amp; port
      *
      * @param destAddr the address of the destination for the message
      * @param userData the data for the message
      * @param statusReportRequested Indicates whether a report is requested for this message.
      * @return a <code>SubmitPdu</code> containing the encoded SC
      *         address, if applicable, and the encoded message.
      *         Returns null on encode error.
      */
    static CARAPI_(AutoPtr<SubmitPdu>) GetSubmitPdu(
        /* [in] */ const String& destAddr,
        /* [in] */ IUserData* userData,
        /* [in] */ Boolean statusReportRequested);

    /**
      * Get an SMS-SUBMIT PDU for a data message to a destination address &amp; port
      *
      * @param destAddr the address of the destination for the message
      * @param userData the data for the message
      * @param statusReportRequested Indicates whether a report is requested for this message.
      * @param priority Priority level of the message
      * @return a <code>SubmitPdu</code> containing the encoded SC
      *         address, if applicable, and the encoded message.
      *         Returns null on encode error.
      */
    static CARAPI_(AutoPtr<SubmitPdu>) GetSubmitPdu(
        /* [in] */ const String& destAddr,
        /* [in] */ IUserData* userData,
        /* [in] */ Boolean statusReportRequested,
        /* [in] */ Int32 priority);

    /**
      * Note: This function is a GSM specific functionality which is not supported in CDMA mode.
      */
    // @Override
    CARAPI GetProtocolIdentifier(
        /* [out] */ Int32* result);

    /**
      * Note: This function is a GSM specific functionality which is not supported in CDMA mode.
      */
    // @Override
    CARAPI IsReplace(
        /* [out] */ Boolean* resutl);

    /**
      * {@inheritDoc}
      * Note: This function is a GSM specific functionality which is not supported in CDMA mode.
      */
    // @Override
    CARAPI IsCphsMwiMessage(
        /* [out] */ Boolean* resutl);

    /**
      * {@inheritDoc}
      */
    // @Override
    CARAPI IsMWIClearMessage(
        /* [out] */ Boolean* resutl);

    /**
      * {@inheritDoc}
      */
    // @Override
    CARAPI IsMWISetMessage(
        /* [out] */ Boolean* resutl);

    /**
      * {@inheritDoc}
      */
    // @Override
    CARAPI IsMwiDontStore(
        /* [out] */ Boolean* resutl);

    /**
      * Returns the status for a previously submitted message.
      * For not interfering with status codes from GSM, this status code is
      * shifted to the bits 31-16.
      */
    // @Override
    CARAPI GetStatus(
        /* [out] */ Int32* result);

    /** Return true iff the bearer data message type is DELIVERY_ACK. */
    // @Override
    CARAPI IsStatusReportMessage(
        /* [out] */ Boolean* resutl);

    /**
      * Note: This function is a GSM specific functionality which is not supported in CDMA mode.
      */
    // @Override
    CARAPI IsReplyPathPresent(
        /* [out] */ Boolean* resutl);

    /**
      * Calculate the number of septets needed to encode the message.
      *
      * @param messageBody the message to encode
      * @param use7bitOnly ignore (but still count) illegal characters if true
      * @return TextEncodingDetails
      */
    static CARAPI_(AutoPtr<IGsmAlphabetTextEncodingDetails>) CalculateLength(
        /* [in] */ ICharSequence* messageBody,
        /* [in] */ Boolean use7bitOnly);

    /**
      * Returns the teleservice type of the message.
      * @return the teleservice:
      *  {@link com.android.internal.telephony.cdma.sms.SmsEnvelope#TELESERVICE_NOT_SET},
      *  {@link com.android.internal.telephony.cdma.sms.SmsEnvelope#TELESERVICE_WMT},
      *  {@link com.android.internal.telephony.cdma.sms.SmsEnvelope#TELESERVICE_WEMT},
      *  {@link com.android.internal.telephony.cdma.sms.SmsEnvelope#TELESERVICE_VMN},
      *  {@link com.android.internal.telephony.cdma.sms.SmsEnvelope#TELESERVICE_WAP}
     */
    /* package */
    virtual CARAPI GetTeleService(
        /* [out] */ Int32* result);

    /**
      * Returns the message type of the message.
      * @return the message type:
      *  {@link com.android.internal.telephony.cdma.sms.SmsEnvelope#MESSAGE_TYPE_POINT_TO_POINT},
      *  {@link com.android.internal.telephony.cdma.sms.SmsEnvelope#MESSAGE_TYPE_BROADCAST},
      *  {@link com.android.internal.telephony.cdma.sms.SmsEnvelope#MESSAGE_TYPE_ACKNOWLEDGE},
     */
    /* package */
    virtual CARAPI GetMessageType(
        /* [out] */ Int32* result);

    /**
      * Parses a broadcast SMS, possibly containing a CMAS alert.
      */
    virtual CARAPI ParseBroadcastSms(
        /* [out] */ ISmsCbMessage** result);

    /**
      * {@inheritDoc}
      */
    // @Override
    CARAPI GetMessageClass(
        /* [out] */ MessageClass* result);

    /**
      * Calculate the next message id, starting at 1 and iteratively
      * incrementing within the range 1..65535 remembering the state
      * via a persistent system property.  (See C.S0015-B, v2.0,
      * 4.3.1.5) Since this routine is expected to be accessed via via
      * binder-call, and hence should be thread-safe, it has been
      * synchronized.
      */
    // synchronized
    static CARAPI_(Int32) GetNextMessageId();

    /** This function  shall be called to get the number of voicemails.
      * @hide
      */
    /*package*/
    virtual CARAPI GetNumOfVoicemails(
        /* [out] */ Int32* result);

    /**
      * Returns a byte array that can be use to uniquely identify a received SMS message.
      * C.S0015-B  4.3.1.6 Unique Message Identification.
      *
      * @return byte array uniquely identifying the message.
      * @hide
      */
    /* package */
    virtual CARAPI GetIncomingSmsFingerprint(
        /* [out] */ ArrayOf<Byte>** result);

    /**
      * Returns the list of service category program data, if present.
      * @return a list of CdmaSmsCbProgramData objects, or null if not present
      * @hide
      */
    virtual CARAPI GetSmsCbProgramData(
        /* [out] */ IArrayList** result);// ICdmaSmsCbProgramData

    //TODO should remove
    virtual CARAPI GetSmsCbProgramData() {
        return NOERROR;
    }

protected:
    /**
      * Parses a SMS message from its BearerData stream. (mobile-terminated only)
      */
    virtual CARAPI ParseSms();

    /**
     * CT WDP header contains WDP Msg Identifier and WDP Userdata
     */
    virtual CARAPI_(Boolean) ProcessCdmaCTWdpHeader(
        /* [in] */ ISmsMessage* sms);

private:
    /**
      * Decodes pdu to an empty SMS object.
      * In the CDMA case the pdu is just an internal byte stream representation
      * of the SMS Java-object.
      * @see #createPdu()
      */
    CARAPI ParsePdu(
        /* [in] */ ArrayOf<Byte>* pdu);

    /**
      * Decodes 3GPP2 sms stored in CSIM/RUIM cards As per 3GPP2 C.S0015-0
      */
    CARAPI_(void) ParsePduFromEfRecord(
        /* [in] */ ArrayOf<Byte>* pdu);

    /**
      * Creates BearerData and Envelope from parameters for a Submit SMS.
      * @return byte stream for SubmitPdu.
      */
    static CARAPI_(AutoPtr<SubmitPdu>) PrivateGetSubmitPdu(
        /* [in] */ const String& destAddrStr,
        /* [in] */ Boolean statusReportRequested,
        /* [in] */ IUserData* userData);

    /**
      * Creates BearerData and Envelope from parameters for a Submit SMS.
      * @return byte stream for SubmitPdu.
      */
    static CARAPI_(AutoPtr<SubmitPdu>) PrivateGetSubmitPdu(
        /* [in] */ const String& destAddrStr,
        /* [in] */ Boolean statusReportRequested,
        /* [in] */ IUserData* userData,
        /* [in] */ Int32 priority);

    /**
      * Creates byte array (pseudo pdu) from SMS object.
      * Note: Do not call this method more than once per object!
      */
    CARAPI_(void) CreatePdu();

    /**
      * Converts a 4-Bit DTMF encoded symbol from the calling address number to ASCII character
      */
    CARAPI_(Byte) ConvertDtmfToAscii(
        /* [in] */ Byte dtmfDigit);

public:
    static const String LOGTAG;

private:
    static const String LOGGABLE_TAG;
    static const Boolean VDBG;
    static const Byte TELESERVICE_IDENTIFIER;
    static const Byte SERVICE_CATEGORY;
    static const Byte ORIGINATING_ADDRESS;
    static const Byte ORIGINATING_SUB_ADDRESS;
    static const Byte DESTINATION_ADDRESS;
    static const Byte DESTINATION_SUB_ADDRESS;
    static const Byte BEARER_REPLY_OPTION;
    static const Byte CAUSE_CODES;
    static const Byte BEARER_DATA;
    /**
      *  Status of a previously submitted SMS.
      *  This field applies to SMS Delivery Acknowledge messages. 0 indicates success;
      *  Here, the error class is defined by the bits from 9-8, the status code by the bits from 7-0.
      *  See C.S0015-B, v2.0, 4.5.21 for a detailed description of possible values.
      */
    Int32 status;
    /** Specifies if a return of an acknowledgment is requested for send SMS */
    static const Int32 RETURN_NO_ACK = 0;
    static const Int32 RETURN_ACK = 1;
    /**
      * Supported priority modes for CDMA SMS messages
      * (See 3GPP2 C.S0015-B, v2.0, table 4.5.9-1)
      */
    static const Int32 PRIORITY_NORMAL = 0x0;
    static const Int32 PRIORITY_INTERACTIVE = 0x1;
    static const Int32 PRIORITY_URGENT = 0x2;
    static const Int32 PRIORITY_EMERGENCY = 0x3;
    AutoPtr<ISmsEnvelope> mEnvelope;
    AutoPtr<IBearerData> mBearerData;
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMSMESSAGE_H__

