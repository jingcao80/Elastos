
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSmsMessage_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSmsMessage_H__

#include "_Elastos_Droid_Internal_Telephony_Gsm_CSmsMessage.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/telephony/SmsMessageBase.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Telephony::IGsmAlphabetTextEncodingDetails;
using Elastos::Droid::Internal::Telephony::SmsMessageBase;
using Elastos::IO::IByteArrayOutputStream;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

/**
 * A Short Message Service message.
 *
 */
CarClass(CSmsMessage)
    , public SmsMessageBase
    , public ISmsMessage
{
public:
    class SubmitPdu
        : public SmsMessageBase::SubmitPduBase
        , public ISmsMessageSubmitPdu
    {
    public:
        CAR_INTERFACE_DECL()
    };

private:
    class PduParser
        : public Object
    {
    public:
        PduParser(
            /* [in] */ ArrayOf<Byte>* pdu);

        /**
         * Parse and return the SC address prepended to SMS messages coming via
         * the TS 27.005 / AT interface.  Returns null on invalid address
         */
        CARAPI GetSCAddress(
            /* [out] */ String* result);

        /**
         * returns non-sign-extended byte value
         */
        CARAPI GetByte(
            /* [out] */ Int32* result);

        /**
         * Any address except the SC address (eg, originating address) See TS
         * 23.040 9.1.2.5
         */
        CARAPI GetAddress(
            /* [out] */ IGsmSmsAddress** result);

        /**
         * Parses an SC timestamp and returns a currentTimeMillis()-style
         * timestamp
         */
        CARAPI GetSCTimestampMillis(
            /* [out] */ Int64* result);

        /**
         * Pulls the user data out of the PDU, and separates the payload from
         * the header if there is one.
         *
         * @param hasUserDataHeader true if there is a user data header
         * @param dataInSeptets true if the data payload is in septets instead
         *  of octets
         * @return the number of septets or octets in the user data payload
         */
        CARAPI ConstructUserData(
            /* [in] */ Boolean hasUserDataHeader,
            /* [in] */ Boolean dataInSeptets,
            /* [out] */ Int32* result);

        /**
         * Returns the user data payload, not including the headers
         *
         * @return the user data payload, not including the headers
         */
        CARAPI GetUserData(
            /* [out] */ ArrayOf<Byte>** result);

        /**
         * Returns an object representing the user data headers
         *
         * {@hide}
         */
        CARAPI GetUserDataHeader(
            /* [out] */ ISmsHeader** result);

        /**
         * Interprets the user data payload as packed GSM 7bit characters, and
         * decodes them into a String.
         *
         * @param septetCount the number of septets in the user data payload
         * @return a String with the decoded characters
         */
        CARAPI GetUserDataGSM7Bit(
            /* [in] */ Int32 septetCount,
            /* [in] */ Int32 languageTable,
            /* [in] */ Int32 languageShiftTable,
            /* [out] */ String* result);

        /**
         * Interprets the user data payload as pack GSM 8-bit (a GSM alphabet string that's
         * stored in 8-bit unpacked format) characters, and decodes them into a String.
         *
         * @param byteCount the number of byest in the user data payload
         * @return a String with the decoded characters
         */
        CARAPI GetUserDataGSM8bit(
            /* [in] */ Int32 byteCount,
            /* [out] */ String* result);

        /**
         * Interprets the user data payload as UCS2 characters, and
         * decodes them into a String.
         *
         * @param byteCount the number of bytes in the user data payload
         * @return a String with the decoded characters
         */
        CARAPI GetUserDataUCS2(
            /* [in] */ Int32 byteCount,
            /* [out] */ String* result);

        /**
         * Interprets the user data payload as KSC-5601 characters, and
         * decodes them into a String.
         *
         * @param byteCount the number of bytes in the user data payload
         * @return a String with the decoded characters
         */
        CARAPI GetUserDataKSC5601(
            /* [in] */ Int32 byteCount,
            /* [out] */ String* result);

        CARAPI MoreDataPresent(
            /* [out] */ Boolean* result);

    public:
        AutoPtr<ArrayOf<Byte> > mPdu;
        Int32 mCur;
        AutoPtr<ISmsHeader> mUserDataHeader;
        AutoPtr<ArrayOf<Byte> > mUserData;
        Int32 mUserDataSeptetPadding;
    };

public:
    CSmsMessage();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Create an SmsMessage from a raw PDU.
     */
    static CARAPI CreateFromPdu(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [out] */ ISmsMessage** result);

    /**
     * 3GPP TS 23.040 9.2.3.9 specifies that Type Zero messages are indicated
     * by TP_PID field set to value 0x40
     */
    CARAPI IsTypeZero(
        /* [out] */ Boolean* result);

    /**
     * TS 27.005 3.4.1 lines[0] and lines[1] are the two lines read from the
     * +CMT unsolicited response (PDU mode, of course)
     *  +CMT: [&lt;alpha>],<length><CR><LF><pdu>
     *
     * Only public for debugging
     *
     * {@hide}
     */
    static CARAPI NewFromCMT(
        /* [in] */ ArrayOf<String>* lines,
        /* [out] */ ISmsMessage** result);

    /** @hide */
    static CARAPI NewFromCDS(
        /* [in] */ const String& line,
        /* [out] */ ISmsMessage** result);

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
    static CARAPI CreateFromEfRecord(
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ ISmsMessage** result);

    /**
     * Get the TP-Layer-Length for the given SMS-SUBMIT PDU Basically, the
     * length in bytes (not hex chars) less the SMSC header
     */
    static CARAPI GetTPLayerLengthForPDU(
        /* [in] */ const String& pdu,
        /* [out] */ Int32* result);

    /**
     * Get Encoded Relative Validty Period Value from Validity period in mins.
     *
     * @param validityPeriod Validity period in mins.
     *
     * Refer specification 3GPP TS 23.040 V6.8.1 section 9.2.3.12.1.
     * ||relValidityPeriod (TP-VP)  ||                 ||  validityPeriod   ||
     *
     *      0 to 143                            --->       (TP-VP + 1) x 5 minutes
     *
     *      144 to 167                         --->        12 hours + ((TP-VP -143) x 30 minutes)
     *
     *      168 to 196                         --->        (TP-VP - 166) x 1 day
     *
     *      197 to 255                         --->        (TP-VP - 192) x 1 week
     *
     * @return relValidityPeriod Encoded Relative Validity Period Value.
     * @hide
     */
    static CARAPI GetRelativeValidityPeriod(
        /* [in] */ Int32 validityPeriod,
        /* [out] */ Int32* result);

    /**
     * Get an SMS-SUBMIT PDU for a destination address and a message
     *
     * @param scAddress Service Centre address.  Null means use default.
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns null on encode error.
     * @hide
     */
    static CARAPI GetSubmitPdu(
        /* [in] */ const String& scAddress,
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& message,
        /* [in] */ Boolean statusReportRequested,
        /* [in] */ ArrayOf<Byte>* header,
        /* [out] */ ISmsMessageSubmitPdu** result);

    /**
     * Get an SMS-SUBMIT PDU for a destination address and a message using the
     * specified encoding.
     *
     * @param scAddress Service Centre address.  Null means use default.
     * @param encoding Encoding defined by constants in
     *        com.android.internal.telephony.SmsConstants.ENCODING_*
     * @param languageTable
     * @param languageShiftTable
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns null on encode error.
     * @hide
     */
    static CARAPI GetSubmitPdu(
        /* [in] */ const String& scAddress,
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& message,
        /* [in] */ Boolean statusReportRequested,
        /* [in] */ ArrayOf<Byte>* header,
        /* [in] */ Int32 encoding,
        /* [in] */ Int32 languageTable,
        /* [in] */ Int32 languageShiftTable,
        /* [out] */ ISmsMessageSubmitPdu** result);

    /**
     * Get an SMS-SUBMIT PDU for a destination address and a message using the
     * specified encoding.
     *
     * @param scAddress Service Centre address.  Null means use default.
     * @param encoding Encoding defined by constants in
     *        com.android.internal.telephony.SmsConstants.ENCODING_*
     * @param languageTable
     * @param languageShiftTable
     * @param validityPeriod Validity Period of the message in Minutes.
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns null on encode error.
     * @hide
     */
    static CARAPI GetSubmitPdu(
        /* [in] */ const String& scAddress,
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& message,
        /* [in] */ Boolean statusReportRequested,
        /* [in] */ ArrayOf<Byte>* header,
        /* [in] */ Int32 encoding,
        /* [in] */ Int32 languageTable,
        /* [in] */ Int32 languageShiftTable,
        /* [in] */ Int32 validityPeriod,
        /* [out] */ ISmsMessageSubmitPdu** result);

    /**
     * Get an SMS-SUBMIT PDU for a destination address and a message
     *
     * @param scAddress Service Centre address.  Null means use default.
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns null on encode error.
     */
    static CARAPI GetSubmitPdu(
        /* [in] */ const String& scAddress,
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& message,
        /* [in] */ Boolean statusReportRequested,
        /* [out] */ ISmsMessageSubmitPdu** result);

    /**
     * Get an SMS-SUBMIT PDU for a destination address and a message
     *
     * @param scAddress Service Centre address.  Null means use default.
     * @param destinationAddress the address of the destination for the message
     * @param statusReportRequested staus report of the message Requested
     * @param validityPeriod Validity Period of the message in Minutes.
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns null on encode error.
     */
    static CARAPI GetSubmitPdu(
        /* [in] */ const String& scAddress,
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& message,
        /* [in] */ Boolean statusReportRequested,
        /* [in] */ Int32 validityPeriod,
        /* [out] */ ISmsMessageSubmitPdu** result);

    /**
     * Get an SMS-SUBMIT PDU for a data message to a destination address &amp; port
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
    static CARAPI GetSubmitPdu(
        /* [in] */ const String& scAddress,
        /* [in] */ const String& destinationAddress,
        /* [in] */ Int32 destinationPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Boolean statusReportRequested,
        /* [out] */ ISmsMessageSubmitPdu** result);

     /**
     * Get an SMS-SUBMIT PDU for a data message to a destination address and port
     *
     * @param scAddress Service Centre address. null == use default
     * @param destinationAddress the address of the destination for the message
     * @param destinationPort the port to deliver the message to at the destination
     * @param originatorPort the originator port set by the sender
     * @param data the data for the message
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns null on encode error.
     */
    static CARAPI GetSubmitPdu(
        /* [in] */ const String& scAddress,
        /* [in] */ const String& destinationAddress,
        /* [in] */ Int32 destinationPort,
        /* [in] */ Int32 originatorPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Boolean statusReportRequested,
        /* [out] */ ISmsMessageSubmitPdu** result);

    /**
     * Calculate the number of septets needed to encode the message.
     *
     * @param msgBody the message to encode
     * @param use7bitOnly ignore (but still count) illegal characters if true
     * @return TextEncodingDetails
     */
    static CARAPI CalculateLength(
        /* [in] */ ICharSequence* msgBody,
        /* [in] */ Boolean use7bitOnly,
        /* [out] */ IGsmAlphabetTextEncodingDetails** result);

    CARAPI GetProtocolIdentifier(
        /* [out] */ Int32* result);

    /**
     * Returns the TP-Data-Coding-Scheme byte, for acknowledgement of SMS-PP download messages.
     * @return the TP-DCS field of the SMS header
     */
    CARAPI GetDataCodingScheme(
        /* [out] */ Int32* result);

    CARAPI IsReplace(
        /* [out] */ Boolean* result);

    CARAPI IsCphsMwiMessage(
        /* [out] */ Boolean* result);

    CARAPI IsMWIClearMessage(
        /* [out] */ Boolean* result);

    CARAPI IsMWISetMessage(
        /* [out] */ Boolean* result);

    CARAPI IsMwiDontStore(
        /* [out] */ Boolean* result);

    CARAPI GetStatus(
        /* [out] */ Int32* result);

    CARAPI IsStatusReportMessage(
        /* [out] */ Boolean* result);

    CARAPI IsReplyPathPresent(
        /* [out] */ Boolean* result);

    CARAPI GetMessageClass(
        /* [out] */ MessageClass* result);

    /**
     * Returns true if this is a (U)SIM data download type SM.
     * See 3GPP TS 31.111 section 9.1 and TS 23.040 section 9.2.3.9.
     *
     * @return true if this is a USIM data download message; FALSE otherwise
     */
    CARAPI IsUsimDataDownload(
        /* [out] */ Boolean* result);

    CARAPI GetNumOfVoicemails(
        /* [out] */ Int32* result);

private:
    /**
     * Packs header and UCS-2 encoded message. Includes TP-UDL & TP-UDHL if necessary
     *
     * @return encoded message as UCS2
     * @throws UnsupportedEncodingException
     */
    static CARAPI_(AutoPtr<ArrayOf<Byte> >) EncodeUCS2(
        /* [in] */ const String& message,
        /* [in] */ ArrayOf<Byte>* header);

    /**
     * Create the beginning of a SUBMIT PDU.  This is the part of the
     * SUBMIT PDU that is common to the two versions of {@link #getSubmitPdu},
     * one of which takes a byte array and the other of which takes a
     * <code>String</code>.
     *
     * @param scAddress Service Centre address. null == use default
     * @param destinationAddress the address of the destination for the message
     * @param mtiByte
     * @param ret <code>SubmitPdu</code> containing the encoded SC
     *        address, if applicable, and the encoded message
     */
    static CARAPI_(AutoPtr<IByteArrayOutputStream>) GetSubmitPduHead(
            /* [in] */ const String& scAddress,
            /* [in] */ const String& destinationAddress,
            /* [in] */ Byte mtiByte,
            /* [in] */ Boolean statusReportRequested,
            /* [in] */ SubmitPdu* ret);

    /**
     * TS 27.005 3.1, &lt;pdu&gt; definition "In the case of SMS: 3GPP TS 24.011 [6]
     * SC address followed by 3GPP TS 23.040 [3] TPDU in hexadecimal format:
     * ME/TA converts each octet of TP data unit into two IRA character long
     * hex number (e.g. octet with integer value 42 is presented to TE as two
     * characters 2A (IRA 50 and 65))" ...in the case of cell broadcast,
     * something else...
     */
    CARAPI ParsePdu(
        /* [in] */ ArrayOf<Byte>* pdu);

    /**
     * Parses a SMS-STATUS-REPORT message.
     *
     * @param p A PduParser, cued past the first byte.
     * @param firstByte The first byte of the PDU, which contains MTI, etc.
     */
    CARAPI_(void) ParseSmsStatusReport(
        /* [in] */ PduParser* p,
        /* [in] */ Int32 firstByte);

    CARAPI_(void) ParseSmsDeliver(
        /* [in] */ PduParser* p,
        /* [in] */ Int32 firstByte);

    /**
     * Parses a SMS-SUBMIT message.
     *
     * @param p A PduParser, cued past the first byte.
     * @param firstByte The first byte of the PDU, which contains MTI, etc.
     */
    CARAPI_(void) ParseSmsSubmit(
        /* [in] */ PduParser* p,
        /* [in] */ Int32 firstByte);

    /**
     * Parses the User Data of an SMS.
     *
     * @param p The current PduParser.
     * @param hasUserDataHeader Indicates whether a header is present in the
     *                          User Data.
     */
    CARAPI_(void) ParseUserData(
        /* [in] */ PduParser* p,
        /* [in] */ Boolean hasUserDataHeader);

public:
    static const String TAG; // = "SmsMessage";

private:
    static const Boolean VDBG = FALSE;

    MessageClass mMessageClass;

    /**
     * TP-Message-Type-Indicator
     * 9.2.3
     */
    Int32 mMti;

    /** TP-Protocol-Identifier (TP-PID) */
    Int32 mProtocolIdentifier;

    // TP-Data-Coding-Scheme
    // see TS 23.038
    Int32 mDataCodingScheme;

    // TP-Reply-Path
    // e.g. 23.040 9.2.2.1
    Boolean mReplyPathPresent; // = FALSE;

    /**
     *  TP-Status - status of a previously submitted SMS.
     *  This field applies to SMS-STATUS-REPORT messages.  0 indicates success;
     *  see TS 23.040, 9.2.3.15 for description of other possible values.
     */
    Int32 mStatus;

    /**
     *  TP-Status - status of a previously submitted SMS.
     *  This field is true iff the message is a SMS-STATUS-REPORT message.
     */
    Boolean mIsStatusReportMessage; // = FALSE;

    Int32 mVoiceMailCount; // = 0;

    static const Int32 VALIDITY_PERIOD_FORMAT_NONE = 0x00;
    static const Int32 VALIDITY_PERIOD_FORMAT_ENHANCED = 0x01;
    static const Int32 VALIDITY_PERIOD_FORMAT_RELATIVE = 0x02;
    static const Int32 VALIDITY_PERIOD_FORMAT_ABSOLUTE = 0x03;

    //Validity Period min - 5 mins
    static const Int32 VALIDITY_PERIOD_MIN = 5;
    //Validity Period max - 63 weeks
    static const Int32 VALIDITY_PERIOD_MAX = 635040;

    static const Int32 INVALID_VALIDITY_PERIOD = -1;
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSmsMessage_H__
