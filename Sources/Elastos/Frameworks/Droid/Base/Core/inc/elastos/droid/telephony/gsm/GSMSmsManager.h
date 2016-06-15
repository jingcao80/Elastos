#ifndef __ELASTOS_DROID_TELEPHONY_GSM_GSMSMSMANAGER_H__
#define __ELASTOS_DROID_TELEPHONY_GSM_GSMSMSMANAGER_H__

#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Droid::App::IPendingIntent;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Gsm {

/**
  * Manages SMS operations such as sending data, text, and pdu SMS messages.
  * Get this object by calling the static method SmsManager.getDefault().
  * @deprecated Replaced by android.telephony.SmsManager that supports both GSM and CDMA.
  */
// @Deprecated public final
class GSMSmsManager
    : public Object
    , public ISmsManager
{
public:
    CAR_INTERFACE_DECL();

    /** Get the default instance of the SmsManager
     *
     * @return the default instance of the SmsManager
     * @deprecated Use android.telephony.SmsManager.
     */
    // @Deprecated
    static CARAPI_(AutoPtr<GSMSmsManager>) GetDefault();

    /**
     * Send a text based SMS.
     *
     * @param destinationAddress the address to send the message to
     * @param scAddress is the service center address or null to use
     *  the current default SMSC
     * @param text the body of the message to send
     * @param sentIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is successfully sent, or failed.
     *  The result code will be <code>Activity.RESULT_OK<code> for success,
     *  or one of these errors:
     *  <code>RESULT_ERROR_GENERIC_FAILURE</code>
     *  <code>RESULT_ERROR_RADIO_OFF</code>
     *  <code>RESULT_ERROR_NULL_PDU</code>.
     *  The per-application based SMS control checks sentIntent. If sentIntent
     *  is NULL the caller will be checked against all unknown applications,
     *  which cause smaller number of SMS to be sent in checking period.
     * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is delivered to the recipient.  The
     *  raw pdu of the status report is in the extended data ("pdu").
     *
     * @throws IllegalArgumentException if destinationAddress or text are empty
     * @deprecated Use android.telephony.SmsManager.
     */
    // @Deprecated
    CARAPI SendTextMessage(
            /* [in] */ const String& destinationAddress,
            /* [in] */ const String& scAddress,
            /* [in] */ const String& text,
            /* [in] */ IPendingIntent* sentIntent,
            /* [in] */ IPendingIntent* deliveryIntent);

    /**
     * Divide a text message into several messages, none bigger than
     * the maximum SMS message size.
     *
     * @param text the original message.  Must not be null.
     * @return an <code>ArrayList</code> of strings that, in order,
     *   comprise the original message
     * @deprecated Use android.telephony.SmsManager.
     */
    // @Deprecated
    CARAPI DivideMessage(
            /* [in] */ const String& text,
            /* [out] */ IArrayList** result);

    /**
     * Send a multi-part text based SMS.  The callee should have already
     * divided the message into correctly sized parts by calling
     * <code>divideMessage</code>.
     *
     * @param destinationAddress the address to send the message to
     * @param scAddress is the service center address or null to use
     *   the current default SMSC
     * @param parts an <code>ArrayList</code> of strings that, in order,
     *   comprise the original message
     * @param sentIntents if not null, an <code>ArrayList</code> of
     *   <code>PendingIntent</code>s (one for each message part) that is
     *   broadcast when the corresponding message part has been sent.
     *   The result code will be <code>Activity.RESULT_OK<code> for success,
     *   or one of these errors:
     *   <code>RESULT_ERROR_GENERIC_FAILURE</code>
     *   <code>RESULT_ERROR_RADIO_OFF</code>
     *   <code>RESULT_ERROR_NULL_PDU</code>.
     *   The per-application based SMS control checks sentIntent. If sentIntent
     *   is NULL the caller will be checked against all unknown applicaitons,
     *   which cause smaller number of SMS to be sent in checking period.
     * @param deliveryIntents if not null, an <code>ArrayList</code> of
     *   <code>PendingIntent</code>s (one for each message part) that is
     *   broadcast when the corresponding message part has been delivered
     *   to the recipient.  The raw pdu of the status report is in the
     *   extended data ("pdu").
     *
     * @throws IllegalArgumentException if destinationAddress or data are empty
     * @deprecated Use android.telephony.SmsManager.
     */
    // @Deprecated
    CARAPI SendMultipartTextMessage(
            /* [in] */ const String& destinationAddress,
            /* [in] */ const String& scAddress,
            /* [in] */ IArrayList* parts,//String
            /* [in] */ IArrayList* sentIntents,//IPendingIntent
            /* [in] */ IArrayList* deliveryIntents);//IPendingIntent

    /**
     * Send a data based SMS to a specific application port.
     *
     * @param destinationAddress the address to send the message to
     * @param scAddress is the service center address or null to use
     *  the current default SMSC
     * @param destinationPort the port to deliver the message to
     * @param data the body of the message to send
     * @param sentIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is sucessfully sent, or failed.
     *  The result code will be <code>Activity.RESULT_OK<code> for success,
     *  or one of these errors:
     *  <code>RESULT_ERROR_GENERIC_FAILURE</code>
     *  <code>RESULT_ERROR_RADIO_OFF</code>
     *  <code>RESULT_ERROR_NULL_PDU</code>.
     *  The per-application based SMS control checks sentIntent. If sentIntent
     *  is NULL the caller will be checked against all unknown applicaitons,
     *  which cause smaller number of SMS to be sent in checking period.
     * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is delivered to the recipient.  The
     *  raw pdu of the status report is in the extended data ("pdu").
     *
     * @throws IllegalArgumentException if destinationAddress or data are empty
     * @deprecated Use android.telephony.SmsManager.
     */
    // @Deprecated
    CARAPI SendDataMessage(
            /* [in] */ const String& destinationAddress,
            /* [in] */ const String& scAddress,
            /* [in] */ Int16 destinationPort,
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ IPendingIntent* sentIntent,
            /* [in] */ IPendingIntent* deliveryIntent);

    /**
     * Copy a raw SMS PDU to the SIM.
     *
     * @param smsc the SMSC for this message, or NULL for the default SMSC
     * @param pdu the raw PDU to store
     * @param status message status (STATUS_ON_SIM_READ, STATUS_ON_SIM_UNREAD,
     *               STATUS_ON_SIM_SENT, STATUS_ON_SIM_UNSENT)
     * @return true for success
     * @deprecated Use android.telephony.SmsManager.
     * {@hide}
     */
    // @Deprecated
    CARAPI CopyMessageToSim(
            /* [in] */ ArrayOf<Byte>* smsc,
            /* [in] */ ArrayOf<Byte>* pdu,
            /* [in] */ Int32 status,
            /* [out] */ Boolean* result);

    /**
     * Delete the specified message from the SIM.
     *
     * @param messageIndex is the record index of the message on SIM
     * @return true for success
     * @deprecated Use android.telephony.SmsManager.
     * {@hide}
     */
    // @Deprecated
    CARAPI DeleteMessageFromSim(
            /* [in] */ Int32 messageIndex,
            /* [out] */ Boolean* result);

    /**
     * Update the specified message on the SIM.
     *
     * @param messageIndex record index of message to update
     * @param newStatus new message status (STATUS_ON_SIM_READ,
     *                  STATUS_ON_SIM_UNREAD, STATUS_ON_SIM_SENT,
     *                  STATUS_ON_SIM_UNSENT, STATUS_ON_SIM_FREE)
     * @param pdu the raw PDU to store
     * @return true for success
     * @deprecated Use android.telephony.SmsManager.
     * {@hide}
     */
    // @Deprecated
    CARAPI UpdateMessageOnSim(
            /* [in] */ Int32 messageIndex,
            /* [in] */ Int32 newStatus,
            /* [in] */ ArrayOf<Byte>* pdu,
            /* [out] */ Boolean* result);

    /**
     * Retrieves all messages currently stored on SIM.
     * @return <code>ArrayList</code> of <code>SmsMessage</code> objects
     * @deprecated Use android.telephony.SmsManager.
     * {@hide}
     */
    // @Deprecated
    //CARAPI_(AutoPtr< IArrayList< AutoPtr<android::telephony::SmsMessage> > >)
    CARAPI GetAllMessagesFromSim(
        /* out */ IArrayList** result);

private:
    // @Deprecated
    GSMSmsManager();

private:
    static AutoPtr<GSMSmsManager> sInstance;
    AutoPtr<Elastos::Droid::Telephony::ISmsManager> mSmsMgrProxy;
};

} // namespace Gsm
} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_GSM_GSMSMSMANAGER_H__
