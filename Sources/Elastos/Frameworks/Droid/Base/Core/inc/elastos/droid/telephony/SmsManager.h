#ifndef __ELASTOS_DROID_TELEPHONY_SMSMANAGER_H__
#define __ELASTOS_DROID_TELEPHONY_SMSMANAGER_H__

#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// package android.telephony;
// import android.app.ActivityThread;
// import android.app.PendingIntent;
// import android.content.ContentValues;
// import android.content.Context;
// import android.content.Intent;
// import android.net.Uri;
// import android.os.Bundle;
// import android.os.RemoteException;
// import android.os.ServiceManager;
// import android.provider.Telephony;
// import android.telephony.SubscriptionManager;
// import android.text.TextUtils;
// import android.util.ArrayMap;
// import android.util.Log;
// import com.android.internal.telephony.ISms;
// import com.android.internal.telephony.SmsRawData;
// import com.android.internal.telephony.IMms;
// import com.android.internal.telephony.uicc.IccConstants;
// import java.util.ArrayList;
// import java.util.Arrays;
// import java.util.List;
// import java.util.Map;

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Internal::Telephony::IISms;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Telephony {

/*
  * TODO(code review): Curious question... Why are a lot of these
  * methods not declared as static, since they do not seem to require
  * any local object state?  Presumably this cannot be changed without
  * interfering with the API...
  */
/**
  * Manages SMS operations such as sending data, text, and pdu SMS messages.
  * Get this object by calling the static method {@link #getDefault()}.
  *
  * <p>For information about how to behave as the default SMS app on Android 4.4 (API level 19)
  * and higher, see {@link android.provider.Telephony}.
  */
class SmsManager
    : public Object
    , public ISmsManager
{
public:
    CAR_INTERFACE_DECL();

    /**
      * Send a text based SMS.
      *
      * <p class="note"><strong>Note:</strong> Using this method requires that your app has the
      * {@link android.Manifest.permission#SEND_SMS} permission.</p>
      *
      * <p class="note"><strong>Note:</strong> Beginning with Android 4.4 (API level 19), if
      * <em>and only if</em> an app is not selected as the default SMS app, the system automatically
      * writes messages sent using this method to the SMS Provider (the default SMS app is always
      * responsible for writing its sent messages to the SMS Provider). For information about
      * how to behave as the default SMS app, see {@link android.provider.Telephony}.</p>
      *
      *
      * @param destinationAddress the address to send the message to
      * @param scAddress is the service center address or null to use
      *  the current default SMSC
      * @param text the body of the message to send
      * @param sentIntent if not NULL this <code>PendingIntent</code> is
      *  broadcast when the message is successfully sent, or failed.
      *  The result code will be <code>Activity.RESULT_OK</code> for success,
      *  or one of these errors:<br>
      *  <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
      *  <code>RESULT_ERROR_RADIO_OFF</code><br>
      *  <code>RESULT_ERROR_NULL_PDU</code><br>
      *  For <code>RESULT_ERROR_GENERIC_FAILURE</code> the sentIntent may include
      *  the extra "errorCode" containing a radio technology specific value,
      *  generally only useful for troubleshooting.<br>
      *  The per-application based SMS control checks sentIntent. If sentIntent
      *  is NULL the caller will be checked against all unknown applications,
      *  which cause smaller number of SMS to be sent in checking period.
      * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
      *  broadcast when the message is delivered to the recipient.  The
      *  raw pdu of the status report is in the extended data ("pdu").
      *
      * @throws IllegalArgumentException if destinationAddress or text are empty
      */
    virtual CARAPI SendTextMessage(
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& scAddress,
        /* [in] */ const String& text,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent);

    /**
      * Send a text based SMS with messaging options.
      *
      * @param destinationAddress the address to send the message to
      * @param scAddress is the service center address or null to use
      *  the current default SMSC
      * @param text the body of the message to send
      * @param sentIntent if not NULL this <code>PendingIntent</code> is
      *  broadcast when the message is successfully sent, or failed.
      *  The result code will be <code>Activity.RESULT_OK</code> for success,
      *  or one of these errors:<br>
      *  <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
      *  <code>RESULT_ERROR_RADIO_OFF</code><br>
      *  <code>RESULT_ERROR_NULL_PDU</code><br>
      *  For <code>RESULT_ERROR_GENERIC_FAILURE</code> the sentIntent may include
      *  the extra "errorCode" containing a radio technology specific value,
      *  generally only useful for troubleshooting.<br>
      *  The per-application based SMS control checks sentIntent. If sentIntent
      *  is NULL the caller will be checked against all unknown applications,
      *  which cause smaller number of SMS to be sent in checking period.
      * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
      *  broadcast when the message is delivered to the recipient.  The
      *  raw pdu of the status report is in the extended data ("pdu").
      * @param priority Priority level of the message
      *  Refer specification See 3GPP2 C.S0015-B, v2.0, table 4.5.9-1
      *  ---------------------------------
      *  PRIORITY      | Level of Priority
      *  ---------------------------------
      *      '00'      |     Normal
      *      '01'      |     Interactive
      *      '10'      |     Urgent
      *      '11'      |     Emergency
      *  ----------------------------------
      *  Any Other values included Negative considered as Invalid Priority Indicator of the message.
      * @param isExpectMore is a boolean to indicate the sending message is multi segmented or not.
      * @param validityPeriod Validity Period of the message in mins.
      *  Refer specification 3GPP TS 23.040 V6.8.1 section 9.2.3.12.1.
      *  Validity Period(Minimum) -> 5 mins
      *  Validity Period(Maximum) -> 635040 mins(i.e.63 weeks).
      *  Any Other values included Negative considered as Invalid Validity Period of the message.
      *
      * @throws IllegalArgumentException if destinationAddress or text are empty
      * {@hide}
      */
    virtual CARAPI SendTextMessage(
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& scAddress,
        /* [in] */ const String& text,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent,
        /* [in] */ Int32 priority,
        /* [in] */ Boolean isExpectMore,
        /* [in] */ Int32 validityPeriod);

    /**
      * TODO Move this to new CarrierSmsManager class.
      *
      * Inject an SMS PDU into the android application framework.
      *
      * The caller should have carrier privileges.
      * @see android.telephony.TelephonyManager.hasCarrierPrivileges
      *
      * @param pdu is the byte array of pdu to be injected into android application framework
      * @param format is the format of SMS pdu (3gpp or 3gpp2)
      * @param receivedIntent if not NULL this <code>PendingIntent</code> is
      *  broadcast when the message is successfully received by the
      *  android application framework. This intent is broadcasted at
      *  the same time an SMS received from radio is acknowledged back.
      *
      *  @throws IllegalArgumentException if format is not one of 3gpp and 3gpp2.
      *  {@hide}
      */
    virtual CARAPI InjectSmsPdu(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ const String& format,
        /* [in] */ IPendingIntent* receivedIntent);

    /**
      * Update the status of a pending (send-by-IP) SMS message and resend by PSTN if necessary.
      * This outbound message was handled by the carrier app. If the carrier app fails to send
      * this message, it would be resent by PSTN.
      *
      * The caller should have carrier privileges.
      * @see android.telephony.TelephonyManager.hasCarrierPrivileges
      *
      * @param messageRef the reference number of the SMS message.
      * @param success True if and only if the message was sent successfully. If its value is
      *  false, this message should be resent via PSTN.
      * {@hide}
      */
    virtual CARAPI UpdateSmsSendStatus(
        /* [in] */ Int32 messageRef,
        /* [in] */ Boolean success);

    /**
      * Divide a message text into several fragments, none bigger than
      * the maximum SMS message size.
      *
      * @param text the original message.  Must not be null.
      * @return an <code>ArrayList</code> of strings that, in order,
      *   comprise the original message
      *
      * @throws IllegalArgumentException if text is null
      */
    virtual CARAPI DivideMessage(
        /* [in] */ const String& text,
        /* [out] */ IArrayList** result);//String

    /**
      * Send a multi-part text based SMS.  The callee should have already
      * divided the message into correctly sized parts by calling
      * <code>divideMessage</code>.
      *
      * <p class="note"><strong>Note:</strong> Using this method requires that your app has the
      * {@link android.Manifest.permission#SEND_SMS} permission.</p>
      *
      * <p class="note"><strong>Note:</strong> Beginning with Android 4.4 (API level 19), if
      * <em>and only if</em> an app is not selected as the default SMS app, the system automatically
      * writes messages sent using this method to the SMS Provider (the default SMS app is always
      * responsible for writing its sent messages to the SMS Provider). For information about
      * how to behave as the default SMS app, see {@link android.provider.Telephony}.</p>
      *
      * @param destinationAddress the address to send the message to
      * @param scAddress is the service center address or null to use
      *   the current default SMSC
      * @param parts an <code>ArrayList</code> of strings that, in order,
      *   comprise the original message
      * @param sentIntents if not null, an <code>ArrayList</code> of
      *   <code>PendingIntent</code>s (one for each message part) that is
      *   broadcast when the corresponding message part has been sent.
      *   The result code will be <code>Activity.RESULT_OK</code> for success,
      *   or one of these errors:<br>
      *   <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
      *   <code>RESULT_ERROR_RADIO_OFF</code><br>
      *   <code>RESULT_ERROR_NULL_PDU</code><br>
      *   For <code>RESULT_ERROR_GENERIC_FAILURE</code> each sentIntent may include
      *   the extra "errorCode" containing a radio technology specific value,
      *   generally only useful for troubleshooting.<br>
      *   The per-application based SMS control checks sentIntent. If sentIntent
      *   is NULL the caller will be checked against all unknown applications,
      *   which cause smaller number of SMS to be sent in checking period.
      * @param deliveryIntents if not null, an <code>ArrayList</code> of
      *   <code>PendingIntent</code>s (one for each message part) that is
      *   broadcast when the corresponding message part has been delivered
      *   to the recipient.  The raw pdu of the status report is in the
      *   extended data ("pdu").
      *
      * @throws IllegalArgumentException if destinationAddress or data are empty
      */
    virtual CARAPI SendMultipartTextMessage(
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& scAddress,
        /* [in] */ IArrayList* parts,//String
        /* [in] */ IArrayList* sentIntents,//IPendingIntent
        /* [in] */ IArrayList* deliveryIntents);//IPendingIntent

    /**
      * Send a multi-part text based SMS with messaging options. The callee should have already
      * divided the message into correctly sized parts by calling
      * <code>divideMessage</code>.
      *
      * <p class="note"><strong>Note:</strong> Using this method requires that your app has the
      * {@link android.Manifest.permission#SEND_SMS} permission.</p>
      *
      * <p class="note"><strong>Note:</strong> Beginning with Android 4.4 (API level 19), if
      * <em>and only if</em> an app is not selected as the default SMS app, the system automatically
      * writes messages sent using this method to the SMS Provider (the default SMS app is always
      * responsible for writing its sent messages to the SMS Provider). For information about
      * how to behave as the default SMS app, see {@link android.provider.Telephony}.</p>
      *
      * @param destinationAddress the address to send the message to
      * @param scAddress is the service center address or null to use
      *   the current default SMSC
      * @param parts an <code>ArrayList</code> of strings that, in order,
      *   comprise the original message
      * @param sentIntents if not null, an <code>ArrayList</code> of
      *   <code>PendingIntent</code>s (one for each message part) that is
      *   broadcast when the corresponding message part has been sent.
      *   The result code will be <code>Activity.RESULT_OK</code> for success,
      *   or one of these errors:<br>
      *   <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
      *   <code>RESULT_ERROR_RADIO_OFF</code><br>
      *   <code>RESULT_ERROR_NULL_PDU</code><br>
      *   For <code>RESULT_ERROR_GENERIC_FAILURE</code> each sentIntent may include
      *   the extra "errorCode" containing a radio technology specific value,
      *   generally only useful for troubleshooting.<br>
      *   The per-application based SMS control checks sentIntent. If sentIntent
      *   is NULL the caller will be checked against all unknown applications,
      *   which cause smaller number of SMS to be sent in checking period.
      * @param deliveryIntents if not null, an <code>ArrayList</code> of
      *   <code>PendingIntent</code>s (one for each message part) that is
      *   broadcast when the corresponding message part has been delivered
      *   to the recipient.  The raw pdu of the status report is in the
      *   extended data ("pdu").
      * @param priority Priority level of the message
      *  Refer specification See 3GPP2 C.S0015-B, v2.0, table 4.5.9-1
      *  ---------------------------------
      *  PRIORITY      | Level of Priority
      *  ---------------------------------
      *      '00'      |     Normal
      *      '01'      |     Interactive
      *      '10'      |     Urgent
      *      '11'      |     Emergency
      *  ----------------------------------
      *  Any Other values included Negative considered as Invalid Priority Indicator of the message.
      * @param isExpectMore is a boolean to indicate the sending message is multi segmented or not.
      * @param validityPeriod Validity Period of the message in mins.
      *  Refer specification 3GPP TS 23.040 V6.8.1 section 9.2.3.12.1.
      *  Validity Period(Minimum) -> 5 mins
      *  Validity Period(Maximum) -> 635040 mins(i.e.63 weeks).
      *  Any Other values included Negative considered as Invalid Validity Period of the message.
      *
      * @throws IllegalArgumentException if destinationAddress or data are empty
      * {@hide}
      */
    virtual CARAPI SendMultipartTextMessage(
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& scAddress,
        /* [in] */ IArrayList* parts,//String
        /* [in] */ IArrayList* sentIntents,//IPendingIntent
        /* [in] */ IArrayList* deliveryIntents,//IPendingIntent
        /* [in] */ Int32 priority,
        /* [in] */ Boolean isExpectMore,
        /* [in] */ Int32 validityPeriod);

    /**
      * Send a data based SMS to a specific application port.
      *
      * <p class="note"><strong>Note:</strong> Using this method requires that your app has the
      * {@link android.Manifest.permission#SEND_SMS} permission.</p>
      *
      * @param destinationAddress the address to send the message to
      * @param scAddress is the service center address or null to use
      *  the current default SMSC
      * @param destinationPort the port to deliver the message to
      * @param data the body of the message to send
      * @param sentIntent if not NULL this <code>PendingIntent</code> is
      *  broadcast when the message is successfully sent, or failed.
      *  The result code will be <code>Activity.RESULT_OK</code> for success,
      *  or one of these errors:<br>
      *  <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
      *  <code>RESULT_ERROR_RADIO_OFF</code><br>
      *  <code>RESULT_ERROR_NULL_PDU</code><br>
      *  For <code>RESULT_ERROR_GENERIC_FAILURE</code> the sentIntent may include
      *  the extra "errorCode" containing a radio technology specific value,
      *  generally only useful for troubleshooting.<br>
      *  The per-application based SMS control checks sentIntent. If sentIntent
      *  is NULL the caller will be checked against all unknown applications,
      *  which cause smaller number of SMS to be sent in checking period.
      * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
      *  broadcast when the message is delivered to the recipient.  The
      *  raw pdu of the status report is in the extended data ("pdu").
      *
      * @throws IllegalArgumentException if destinationAddress or data are empty
      */
    virtual CARAPI SendDataMessage(
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& scAddress,
        /* [in] */ Int16 destinationPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent);

    /**
      * Send a data based SMS to a specific application port.
      *
      * <p class="note"><strong>Note:</strong> Using this method requires that your app has the
      * {@link android.Manifest.permission#SEND_SMS} permission.</p>
      *
      * @param destinationAddress the address to send the message to
      * @param scAddress is the service center address or null to use
      *  the current default SMSC
      * @param destinationPort the port to deliver the message to
      * @param originatorPort the port set by the sender
      * @param data the body of the message to send
      * @param sentIntent if not NULL this <code>PendingIntent</code> is
      *  broadcast when the message is successfully sent, or failed.
      *  The result code will be <code>Activity.RESULT_OK</code> for success,
      *  or one of these errors:<br>
      *  <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
      *  <code>RESULT_ERROR_RADIO_OFF</code><br>
      *  <code>RESULT_ERROR_NULL_PDU</code><br>
      *  For <code>RESULT_ERROR_GENERIC_FAILURE</code> the sentIntent may include
      *  the extra "errorCode" containing a radio technology specific value,
      *  generally only useful for troubleshooting.<br>
      *  The per-application based SMS control checks sentIntent. If sentIntent
      *  is NULL the caller will be checked against all unknown applications,
      *  which cause smaller number of SMS to be sent in checking period.
      * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
      *  broadcast when the message is delivered to the recipient.  The
      *  raw pdu of the status report is in the extended data ("pdu").
      *
      * @throws IllegalArgumentException if destinationAddress or data are empty
      * {@hide}
      */
    virtual CARAPI SendDataMessage(
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& scAddress,
        /* [in] */ Int16 destinationPort,
        /* [in] */ Int16 originatorPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent);

    /**
     * Get the default instance of the SmsManager
     *
     * @return the default instance of the SmsManager
     */
    static CARAPI_(AutoPtr<ISmsManager>) GetDefault();

    /**
      * Get the the instance of the SmsManager associated with a particular subId
      *
      * @param subId a SMS subscription id, typically accessed using SubscriptionManager
      * @return the instance of the SmsManager associated with subId
      *
      * {@hide}
      */
    static CARAPI_(AutoPtr<ISmsManager>) GetSmsManagerForSubscriber(
        /* [in] */ Int64 subId);

    /**
      * Get the associated subId. If the instance was returned by {@link #getDefault()}, then this
      * method may return different values at different points in time (if the user changes the
      * default subId). It will return SubscriptionManager.INVALID_SUB_ID if the default
      * subId cannot be determined.
      *
      * @return associated subId
      *
      * {@hide}
      */
    virtual CARAPI GetSubId(
        /* [out] */ Int64* result);

    /**
      * Copy a raw SMS PDU to the ICC.
      * ICC (Integrated Circuit Card) is the card of the device.
      * For example, this can be the SIM or USIM for GSM.
      *
      * @param smsc the SMSC for this message, or NULL for the default SMSC
      * @param pdu the raw PDU to store
      * @param status message status (STATUS_ON_ICC_READ, STATUS_ON_ICC_UNREAD,
      *               STATUS_ON_ICC_SENT, STATUS_ON_ICC_UNSENT)
      * @return true for success
      *
      * @throws IllegalArgumentException if pdu is NULL
      * {@hide}
      */
    virtual CARAPI CopyMessageToIcc(
        /* [in] */ ArrayOf<Byte>* smsc,
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ Int32 status,
        /* [out] */ Boolean* result);

    /**
      * Delete the specified message from the ICC.
      * ICC (Integrated Circuit Card) is the card of the device.
      * For example, this can be the SIM or USIM for GSM.
      *
      * @param messageIndex is the record index of the message on ICC
      * @return true for success
      *
      * {@hide}
      */
    virtual CARAPI DeleteMessageFromIcc(
        /* [in] */ Int32 messageIndex,
        /* [out] */ Boolean* result);

    /**
      * Update the specified message on the ICC.
      * ICC (Integrated Circuit Card) is the card of the device.
      * For example, this can be the SIM or USIM for GSM.
      *
      * @param messageIndex record index of message to update
      * @param newStatus new message status (STATUS_ON_ICC_READ,
      *                  STATUS_ON_ICC_UNREAD, STATUS_ON_ICC_SENT,
      *                  STATUS_ON_ICC_UNSENT, STATUS_ON_ICC_FREE)
      * @param pdu the raw PDU to store
      * @return true for success
      *
      * {@hide}
      */
    virtual CARAPI UpdateMessageOnIcc(
        /* [in] */ Int32 messageIndex,
        /* [in] */ Int32 newStatus,
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [out] */ Boolean* result);

    /**
      * Retrieves all messages currently stored on ICC.
      * ICC (Integrated Circuit Card) is the card of the device.
      * For example, this can be the SIM or USIM for GSM.
      *
      * @return <code>ArrayList</code> of <code>SmsMessage</code> objects
      *
      * {@hide}
      */
    virtual CARAPI GetAllMessagesFromIcc(
        /* [out] */ IArrayList** result);//ISmsMessage

    /**
      * Enable reception of cell broadcast (SMS-CB) messages with the given
      * message identifier. Note that if two different clients enable the same
      * message identifier, they must both disable it for the device to stop
      * receiving those messages. All received messages will be broadcast in an
      * intent with the action "android.provider.Telephony.SMS_CB_RECEIVED".
      * Note: This call is blocking, callers may want to avoid calling it from
      * the main thread of an application.
      *
      * @param messageIdentifier Message identifier as specified in TS 23.041 (3GPP)
      * or C.R1001-G (3GPP2)
      * @return true if successful, false otherwise
      * @see #disableCellBroadcast(int)
      *
      * {@hide}
      */
    virtual CARAPI EnableCellBroadcast(
        /* [in] */ Int32 messageIdentifier,
        /* [out] */ Boolean* result);

    /**
      * Disable reception of cell broadcast (SMS-CB) messages with the given
      * message identifier. Note that if two different clients enable the same
      * message identifier, they must both disable it for the device to stop
      * receiving those messages.
      * Note: This call is blocking, callers may want to avoid calling it from
      * the main thread of an application.
      *
      * @param messageIdentifier Message identifier as specified in TS 23.041 (3GPP)
      * or C.R1001-G (3GPP2)
      * @return true if successful, false otherwise
      *
      * @see #enableCellBroadcast(int)
      *
      * {@hide}
      */
    virtual CARAPI DisableCellBroadcast(
        /* [in] */ Int32 messageIdentifier,
        /* [out] */ Boolean* result);

    /**
      * Enable reception of cell broadcast (SMS-CB) messages with the given
      * message identifier range. Note that if two different clients enable the same
      * message identifier, they must both disable it for the device to stop
      * receiving those messages. All received messages will be broadcast in an
      * intent with the action "android.provider.Telephony.SMS_CB_RECEIVED".
      * Note: This call is blocking, callers may want to avoid calling it from
      * the main thread of an application.
      *
      * @param startMessageId first message identifier as specified in TS 23.041 (3GPP)
      * or C.R1001-G (3GPP2)
      * @param endMessageId last message identifier as specified in TS 23.041 (3GPP)
      * or C.R1001-G (3GPP2)
      * @return true if successful, false otherwise
      * @see #disableCellBroadcastRange(int, int)
      *
      * @throws IllegalArgumentException if endMessageId < startMessageId
      * {@hide}
      */
    virtual CARAPI EnableCellBroadcastRange(
        /* [in] */ Int32 startMessageId,
        /* [in] */ Int32 endMessageId,
        /* [out] */ Boolean* result);

    /**
      * Disable reception of cell broadcast (SMS-CB) messages with the given
      * message identifier range. Note that if two different clients enable the same
      * message identifier, they must both disable it for the device to stop
      * receiving those messages.
      * Note: This call is blocking, callers may want to avoid calling it from
      * the main thread of an application.
      *
      * @param startMessageId first message identifier as specified in TS 23.041 (3GPP)
      * or C.R1001-G (3GPP2)
      * @param endMessageId last message identifier as specified in TS 23.041 (3GPP)
      * or C.R1001-G (3GPP2)
      * @return true if successful, false otherwise
      *
      * @see #enableCellBroadcastRange(int, int)
      *
      * @throws IllegalArgumentException if endMessageId < startMessageId
      * {@hide}
      */
    virtual CARAPI DisableCellBroadcastRange(
        /* [in] */ Int32 startMessageId,
        /* [in] */ Int32 endMessageId,
        /* [out] */ Boolean* result);

    /**
      * SMS over IMS is supported if IMS is registered and SMS is supported
      * on IMS.
      *
      * @return true if SMS over IMS is supported, false otherwise
      *
      * @see #getImsSmsFormat()
      *
      * @hide
      */
    virtual CARAPI IsImsSmsSupported(
        /* [out] */ Boolean* result);

    /**
      * Gets SMS format supported on IMS.  SMS over IMS format is
      * either 3GPP or 3GPP2.
      *
      * @return SmsMessage.FORMAT_3GPP,
      *         SmsMessage.FORMAT_3GPP2
      *      or SmsMessage.FORMAT_UNKNOWN
      *
      * @see #isImsSmsSupported()
      *
      * @hide
      */
    virtual CARAPI GetImsSmsFormat(
        /* [out] */ String* result);

    /**
      * Get the default sms subId
      *
      * @return the default sms subId
      * @hide
      */
    static CARAPI_(Int64) GetDefaultSmsSubId();

    /**
      * Get the capacity count of sms on Icc card
      *
      * @return the capacity count of sms on Icc card
      * @hide
      */
    virtual CARAPI GetSmsCapacityOnIcc(
        /* [out] */ Int32* result);

    /**
      * Send an MMS message
      *
      * @param context application context
      * @param contentUri the content Uri from which the message pdu will be read
      * @param locationUrl the optional location url where message should be sent to
      * @param configOverrides the carrier-specific messaging configuration values to override for
      *  sending the message.
      * @param sentIntent if not NULL this <code>PendingIntent</code> is
      *  broadcast when the message is successfully sent, or failed
      * @throws IllegalArgumentException if contentUri is empty
      */
    virtual CARAPI SendMultimediaMessage(
        /* [in] */ IContext* context,
        /* [in] */ IUri* contentUri,
        /* [in] */ const String& locationUrl,
        /* [in] */ IBundle* configOverrides,
        /* [in] */ IPendingIntent* sentIntent);

    /**
      * Download an MMS message from carrier by a given location URL
      *
      * @param context application context
      * @param locationUrl the location URL of the MMS message to be downloaded, usually obtained
      *  from the MMS WAP push notification
      * @param contentUri the content uri to which the downloaded pdu will be written
      * @param configOverrides the carrier-specific messaging configuration values to override for
      *  downloading the message.
      * @param downloadedIntent if not NULL this <code>PendingIntent</code> is
      *  broadcast when the message is downloaded, or the download is failed
      * @throws IllegalArgumentException if locationUrl or contentUri is empty
      */
    virtual CARAPI DownloadMultimediaMessage(
        /* [in] */ IContext* context,
        /* [in] */ const String& locationUrl,
        /* [in] */ IUri* contentUri,
        /* [in] */ IBundle* configOverrides,
        /* [in] */ IPendingIntent* downloadedIntent);

    /**
      * Update the status of a pending (send-by-IP) MMS message handled by the carrier app.
      * If the carrier app fails to send this message, it may be resent via carrier network
      * depending on the status code.
      *
      * The caller should have carrier privileges.
      * @see android.telephony.TelephonyManager.hasCarrierPrivileges
      *
      * @param context application context
      * @param messageRef the reference number of the MMS message.
      * @param pdu non-empty (contains the SendConf PDU) if the message was sent successfully,
      *   otherwise, this param should be null.
      * @param status send status. It can be Activity.RESULT_OK or one of the MMS error codes.
      *   If status is Activity.RESULT_OK, the MMS was sent successfully.
      *   If status is MMS_ERROR_RETRY, this message would be resent via carrier
      *   network. The message will not be resent for other MMS error statuses.
      * @param contentUri the URI of the sent message
      * {@hide}
      */
    virtual CARAPI UpdateMmsSendStatus(
        /* [in] */ IContext* context,
        /* [in] */ Int32 messageRef,
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ Int32 status,
        /* [in] */ IUri* contentUri);

    /**
      * Update the status of a pending (download-by-IP) MMS message handled by the carrier app.
      * If the carrier app fails to download this message, it may be re-downloaded via carrier
      * network depending on the status code.
      *
      * The caller should have carrier privileges.
      * @see android.telephony.TelephonyManager.hasCarrierPrivileges
      *
      * @param context application context
      * @param messageRef the reference number of the MMS message.
      * @param status download status.  It can be Activity.RESULT_OK or one of the MMS error codes.
      *   If status is Activity.RESULT_OK, the MMS was downloaded successfully.
      *   If status is MMS_ERROR_RETRY, this message would be re-downloaded via carrier
      *   network. The message will not be re-downloaded for other MMS error statuses.
      * @param contentUri the URI of the downloaded message
      * {@hide}
      */
    virtual CARAPI UpdateMmsDownloadStatus(
        /* [in] */ IContext* context,
        /* [in] */ Int32 messageRef,
        /* [in] */ Int32 status,
        /* [in] */ IUri* contentUri);

    /**
      * Import a text message into system's SMS store
      *
      * Only default SMS apps can import SMS
      *
      * @param address the destination(source) address of the sent(received) message
      * @param type the type of the message
      * @param text the message text
      * @param timestampMillis the message timestamp in milliseconds
      * @param seen if the message is seen
      * @param read if the message is read
      * @return the message URI, null if failed
      * @hide
      */
    virtual CARAPI ImportTextMessage(
        /* [in] */ const String& address,
        /* [in] */ Int32 type,
        /* [in] */ const String& text,
        /* [in] */ Int64 timestampMillis,
        /* [in] */ Boolean seen,
        /* [in] */ Boolean read,
        /* [out] */ IUri** result);

    /**
      * Import a multimedia message into system's MMS store. Only the following PDU type is
      * supported: Retrieve.conf, Send.req, Notification.ind, Delivery.ind, Read-Orig.ind
      *
      * Only default SMS apps can import MMS
      *
      * @param contentUri the content uri from which to read the PDU of the message to import
      * @param messageId the optional message id. Use null if not specifying
      * @param timestampSecs the optional message timestamp. Use -1 if not specifying
      * @param seen if the message is seen
      * @param read if the message is read
      * @return the message URI, null if failed
      * @throws IllegalArgumentException if pdu is empty
      * {@hide}
      */
    virtual CARAPI ImportMultimediaMessage(
        /* [in] */ IUri* contentUri,
        /* [in] */ const String& messageId,
        /* [in] */ Int64 timestampSecs,
        /* [in] */ Boolean seen,
        /* [in] */ Boolean read,
        /* [out] */ IUri** result);

    /**
      * Delete a system stored SMS or MMS message
      *
      * Only default SMS apps can delete system stored SMS and MMS messages
      *
      * @param messageUri the URI of the stored message
      * @return true if deletion is successful, false otherwise
      * @throws IllegalArgumentException if messageUri is empty
      * {@hide}
      */
    virtual CARAPI DeleteStoredMessage(
        /* [in] */ IUri* messageUri,
        /* [out] */ Boolean* result);

    /**
      * Delete a system stored SMS or MMS thread
      *
      * Only default SMS apps can delete system stored SMS and MMS conversations
      *
      * @param conversationId the ID of the message conversation
      * @return true if deletion is successful, false otherwise
      * {@hide}
      */
    virtual CARAPI DeleteStoredConversation(
        /* [in] */ Int64 conversationId,
        /* [out] */ Boolean* result);

    /**
      * Update the status properties of a system stored SMS or MMS message, e.g.
      * the read status of a message, etc.
      *
      * @param messageUri the URI of the stored message
      * @param statusValues a list of status properties in key-value pairs to update
      * @return true if update is successful, false otherwise
      * @throws IllegalArgumentException if messageUri is empty
      * {@hide}
      */
    virtual CARAPI UpdateStoredMessageStatus(
        /* [in] */ IUri* messageUri,
        /* [in] */ IContentValues* statusValues,
        /* [out] */ Boolean* result);

    /**
      * Archive or unarchive a stored conversation
      *
      * @param conversationId the ID of the message conversation
      * @param archived true to archive the conversation, false to unarchive
      * @return true if update is successful, false otherwise
      * {@hide}
      */
    virtual CARAPI ArchiveStoredConversation(
        /* [in] */ Int64 conversationId,
        /* [in] */ Boolean archived,
        /* [out] */ Boolean* result);

    /**
      * Add a text message draft to system SMS store
      *
      * Only default SMS apps can add SMS draft
      *
      * @param address the destination address of message
      * @param text the body of the message to send
      * @return the URI of the stored draft message
      * {@hide}
      */
    virtual CARAPI AddTextMessageDraft(
        /* [in] */ const String& address,
        /* [in] */ const String& text,
        /* [out] */ IUri** result);

    /**
      * Add a multimedia message draft to system MMS store
      *
      * Only default SMS apps can add MMS draft
      *
      * @param contentUri the content uri from which to read the PDU data of the draft MMS
      * @return the URI of the stored draft message
      * @throws IllegalArgumentException if pdu is empty
      * {@hide}
      */
    virtual CARAPI AddMultimediaMessageDraft(
        /* [in] */ IUri* contentUri,
        /* [out] */ IUri** result);

    /**
      * Send a system stored text message.
      *
      * You can only send a failed text message or a draft text message.
      *
      * @param messageUri the URI of the stored message
      * @param scAddress is the service center address or null to use the current default SMSC
      * @param sentIntent if not NULL this <code>PendingIntent</code> is
      *  broadcast when the message is successfully sent, or failed.
      *  The result code will be <code>Activity.RESULT_OK</code> for success,
      *  or one of these errors:<br>
      *  <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
      *  <code>RESULT_ERROR_RADIO_OFF</code><br>
      *  <code>RESULT_ERROR_NULL_PDU</code><br>
      *  For <code>RESULT_ERROR_GENERIC_FAILURE</code> the sentIntent may include
      *  the extra "errorCode" containing a radio technology specific value,
      *  generally only useful for troubleshooting.<br>
      *  The per-application based SMS control checks sentIntent. If sentIntent
      *  is NULL the caller will be checked against all unknown applications,
      *  which cause smaller number of SMS to be sent in checking period.
      * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
      *  broadcast when the message is delivered to the recipient.  The
      *  raw pdu of the status report is in the extended data ("pdu").
      *
      * @throws IllegalArgumentException if messageUri is empty
      * {@hide}
      */
    virtual CARAPI SendStoredTextMessage(
        /* [in] */ IUri* messageUri,
        /* [in] */ const String& scAddress,
        /* [in] */ IPendingIntent* sentIntent,
        /* [in] */ IPendingIntent* deliveryIntent);

    /**
      * Send a system stored multi-part text message.
      *
      * You can only send a failed text message or a draft text message.
      * The provided <code>PendingIntent</code> lists should match the part number of the
      * divided text of the stored message by using <code>divideMessage</code>
      *
      * @param messageUri the URI of the stored message
      * @param scAddress is the service center address or null to use
      *   the current default SMSC
      * @param sentIntents if not null, an <code>ArrayList</code> of
      *   <code>PendingIntent</code>s (one for each message part) that is
      *   broadcast when the corresponding message part has been sent.
      *   The result code will be <code>Activity.RESULT_OK</code> for success,
      *   or one of these errors:<br>
      *   <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
      *   <code>RESULT_ERROR_RADIO_OFF</code><br>
      *   <code>RESULT_ERROR_NULL_PDU</code><br>
      *   For <code>RESULT_ERROR_GENERIC_FAILURE</code> each sentIntent may include
      *   the extra "errorCode" containing a radio technology specific value,
      *   generally only useful for troubleshooting.<br>
      *   The per-application based SMS control checks sentIntent. If sentIntent
      *   is NULL the caller will be checked against all unknown applications,
      *   which cause smaller number of SMS to be sent in checking period.
      * @param deliveryIntents if not null, an <code>ArrayList</code> of
      *   <code>PendingIntent</code>s (one for each message part) that is
      *   broadcast when the corresponding message part has been delivered
      *   to the recipient.  The raw pdu of the status report is in the
      *   extended data ("pdu").
      *
      * @throws IllegalArgumentException if messageUri is empty
      * {@hide}
      */
    virtual CARAPI SendStoredMultipartTextMessage(
        /* [in] */ IUri* messageUri,
        /* [in] */ const String& scAddress,
        /* [in] */ IArrayList* sentIntents,//IPendingIntent
        /* [in] */ IArrayList* deliveryIntents);//IPendingIntent

    /**
      * Send a system stored MMS message
      *
      * This is used for sending a previously sent, but failed-to-send, message or
      * for sending a text message that has been stored as a draft.
      *
      * @param messageUri the URI of the stored message
      * @param configOverrides the carrier-specific messaging configuration values to override for
      *  sending the message.
      * @param sentIntent if not NULL this <code>PendingIntent</code> is
      *  broadcast when the message is successfully sent, or failed
      * @throws IllegalArgumentException if messageUri is empty
      * {@hide}
      */
    virtual CARAPI SendStoredMultimediaMessage(
        /* [in] */ IUri* messageUri,
        /* [in] */ IBundle* configOverrides,
        /* [in] */ IPendingIntent* sentIntent);

    /**
      * Turns on/off the flag to automatically write sent/received SMS/MMS messages into system
      *
      * When this flag is on, all SMS/MMS sent/received are stored by system automatically
      * When this flag is off, only SMS/MMS sent by non-default SMS apps are stored by system
      * automatically
      *
      * This flag can only be changed by default SMS apps
      *
      * @param enabled Whether to enable message auto persisting
      * {@hide}
      */
    virtual CARAPI SetAutoPersisting(
        /* [in] */ Boolean enabled);

    /**
      * Get the value of the flag to automatically write sent/received SMS/MMS messages into system
      *
      * When this flag is on, all SMS/MMS sent/received are stored by system automatically
      * When this flag is off, only SMS/MMS sent by non-default SMS apps are stored by system
      * automatically
      *
      * @return the current value of the auto persist flag
      * {@hide}
      */
    virtual CARAPI GetAutoPersisting(
        /* [out] */ Boolean* result);

    /**
      * Get carrier-dependent configuration values.
      *
      * @return bundle key/values pairs of configuration values
      */
    virtual CARAPI GetCarrierConfigValues(
        /* [out] */ IBundle** result);

private:

    SmsManager(
        /* [in] */ Int64 subId);

    /**
      * Returns the ISms service, or throws an UnsupportedOperationException if
      * the service does not exist.
      */
    static CARAPI_(AutoPtr<IISms>) GetISmsServiceOrThrow();

    static CARAPI_(AutoPtr<IISms>) GetISmsService();

    /**
      * Create a list of <code>SmsMessage</code>s from a list of RawSmsData
      * records returned by <code>getAllMessagesFromIcc()</code>
      *
      * @param records SMS EF records, returned by
      *   <code>getAllMessagesFromIcc</code>
      * @return <code>ArrayList</code> of <code>ISmsMessage</code> objects.
      */
    CARAPI_(AutoPtr<IArrayList>) CreateMessageListFromRawRecords(
        /* [in] */ IList* records);//ISmsRawData

    CARAPI_(void) GrantCarrierPackageUriPermission(
        /* [in] */ IContext* context,
        /* [in] */ IUri* contentUri,
        /* [in] */ const String& action,
        /* [in] */ Int32 permission);

private:
    /**
      * A psuedo-subId that represents the default subId at any given time. The actual subId it
      * represents changes as the default subId is changed.
      */
    static const Int32 DEFAULT_SUB_ID = -1002;
    /** Singleton object constructed during class initialization. */
    static AutoPtr<ISmsManager> sInstance;
    static Object sLockObject;
    static AutoPtr<IMap> sSubInstances;//Long, SmsManager
    /** A concrete subId, or the pseudo DEFAULT_SUB_ID */
    Int64 mSubId;
    static const String PHONE_PACKAGE_NAME;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_SMSMANAGER_H__

