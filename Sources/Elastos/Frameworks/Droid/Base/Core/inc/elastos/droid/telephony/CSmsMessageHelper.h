#ifndef __ELASTOS_DROID_TELEPHONY_CSMSMESSAGEHELPER_H__
#define __ELASTOS_DROID_TELEPHONY_CSMSMESSAGEHELPER_H__

#include "_Elastos_Droid_Telephony_CSmsMessageHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CSmsMessageHelper)
    , public Singleton
    , public ISmsMessageHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

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
    CARAPI CreateFromPdu(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [out] */ ISmsMessage** result);

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
    CARAPI CreateFromPdu(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ const String& format,
        /* [out] */ ISmsMessage** result);

    /**
     * TS 27.005 3.4.1 lines[0] and lines[1] are the two lines read from the
     * +CMT unsolicited response (PDU mode, of course)
     *  +CMT: [&lt;alpha>],<length><CR><LF><pdu>
     *
     * Only public for debugging and for RIL
     *
     * {@hide}
     */
    CARAPI NewFromCMT(
        /* [in] */ ArrayOf<String>* lines,
        /* [out] */ ISmsMessage** result);

    /** @hide */
    CARAPI NewFromParcel(
        /* [in] */ IParcel* p,
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
    CARAPI CreateFromEfRecord(
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ ISmsMessage** result);

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
    CARAPI CreateFromEfRecord(
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int64 subId,
        /* [out] */ ISmsMessage** result);

    /**
     * Get the TP-Layer-Length for the given SMS-SUBMIT PDU Basically, the
     * length in bytes (not hex chars) less the SMSC header
     *
     * FIXME: This method is only used by a CTS test case that isn't run on CDMA devices.
     * We should probably deprecate it and remove the obsolete test case.
     */
    CARAPI GetTPLayerLengthForPDU(
        /* [in] */ const String& pdu,
        /* [out] */ Int32* result);

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
    CARAPI CalculateLength(
        /* [in] */ ICharSequence* msgBody,
        /* [in] */ Boolean use7bitOnly,
        /* [out, callee] */ ArrayOf<Int32>** result);

    /**
     * Divide a message text into several fragments, none bigger than
     * the maximum SMS message text size.
     *
     * @param text text, must not be null.
     * @return an <code>ArrayList</code> of strings that, in order,
     *   comprise the original msg text
     *
     * @hide
     */
    CARAPI FragmentText(
        /* [in] */ const String& text,
        /* [out] */ IArrayList** result);

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
    CARAPI CalculateLength(
        /* [in] */ const String& messageBody,
        /* [in] */ Boolean use7bitOnly,
        /* [out, callee] */ ArrayOf<Int32>** result);

    /**
     * Get an SMS-SUBMIT PDU for a destination address and a message.
     * This method will not attempt to use any GSM national language 7 bit encodings.
     *
     * @param scAddress Service Centre address.  Null means use default.
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns null on encode error.
     */
    CARAPI GetSubmitPdu(
        /* [in] */ const String& scAddress,
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& message,
        /* [in] */ Boolean statusReportRequested,
        /* [out] */ ISmsMessageSubmitPdu** result);

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
    CARAPI GetSubmitPdu(
        /* [in] */ const String& scAddress,
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& message,
        /* [in] */ Boolean statusReportRequested,
        /* [in] */ Int64 subId,
        /* [out] */ ISmsMessageSubmitPdu** result);

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
    CARAPI GetSubmitPdu(
        /* [in] */ const String& scAddress,
        /* [in] */ const String& destinationAddress,
        /* [in] */ Int16 destinationPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Boolean statusReportRequested,
        /* [out] */ ISmsMessageSubmitPdu** result);

    /**
     * Decide if the carrier supports long SMS.
     * {@hide}
     */
    CARAPI HasEmsSupport(
        /* [out] */ Boolean* result);

    /**
     * Check where to add " x/y" in each SMS segment, begin or end.
     * {@hide}
     */
    CARAPI ShouldAppendPageNumberAsPrefix(
        /* [out] */ Boolean* result);

};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CSMSMESSAGEHELPER_H__
