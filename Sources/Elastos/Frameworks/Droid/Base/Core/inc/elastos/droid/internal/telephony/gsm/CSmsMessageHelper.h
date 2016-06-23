
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSMSMESSAGEHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSMSMESSAGEHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_Gsm_CSmsMessageHelper.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::IGsmAlphabetTextEncodingDetails;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

/**
 * A Short Message Service message.
 *
 */
CarClass(CSmsMessageHelper)
    , public Singleton
    , public ISmsMessageHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Create an SmsMessage from a raw PDU.
     */
    CARAPI CreateFromPdu(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [out] */ ISmsMessage** result);

    /**
     * TS 27.005 3.4.1 lines[0] and lines[1] are the two lines read from the
     * +CMT unsolicited response (PDU mode, of course)
     *  +CMT: [&lt;alpha>],<length><CR><LF><pdu>
     *
     * Only public for debugging
     *
     * {@hide}
     */
    CARAPI NewFromCMT(
        /* [in] */ ArrayOf<String>* lines,
        /* [out] */ ISmsMessage** result);

    /** @hide */
    CARAPI NewFromCDS(
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
    CARAPI CreateFromEfRecord(
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ ISmsMessage** result);

    /**
     * Get the TP-Layer-Length for the given SMS-SUBMIT PDU Basically, the
     * length in bytes (not hex chars) less the SMSC header
     */
    CARAPI GetTPLayerLengthForPDU(
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
    CARAPI GetRelativeValidityPeriod(
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
    CARAPI GetSubmitPdu(
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
    CARAPI GetSubmitPdu(
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
    CARAPI GetSubmitPdu(
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
    CARAPI GetSubmitPdu(
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
    CARAPI GetSubmitPdu(
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
    CARAPI GetSubmitPdu(
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
    CARAPI GetSubmitPdu(
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
    CARAPI CalculateLength(
        /* [in] */ ICharSequence* msgBody,
        /* [in] */ Boolean use7bitOnly,
        /* [out] */ IGsmAlphabetTextEncodingDetails** result);
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSMSMESSAGEHELPER_H__
