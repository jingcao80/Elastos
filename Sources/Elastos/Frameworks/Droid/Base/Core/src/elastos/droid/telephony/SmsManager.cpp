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

package android.telephony;

using Elastos::Droid::App::IActivityThread;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Provider::ITelephony;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Internal::Telephony::IISms;
using Elastos::Droid::Internal::Telephony::ISmsRawData;
using Elastos::Droid::Internal::Telephony::IIMms;
using Elastos::Droid::Internal::Telephony::Uicc::IIccConstants;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

/*
 * TODO(code review): Curious question... Why are a lot of these
 * methods not declared as static, since they do not seem to require
 * any local object state?  Presumably this cannot be changed without
 * interfering with the API...
 */

/**
 * Manages SMS operations such as sending data, text, and pdu SMS messages.
 * Get this object by calling the static method {@link #GetDefault()}.
 *
 * <p>For information about how to behave as the default SMS app on Android 4.4 (API level 19)
 * and higher, see {@link android.provider.Telephony}.
 */
public class SmsManager {
    /**
     * A psuedo-subId that represents the default subId at any given time. The actual subId it
     * represents changes as the default subId is changed.
     */
    private static const Int32 DEFAULT_SUB_ID = -1002;

    /** Singleton object constructed during class initialization. */
    private static const SmsManager sInstance = new SmsManager(DEFAULT_SUB_ID);
    private static const Object sLockObject = new Object();
    private static const Map<Long, SmsManager> sSubInstances = new ArrayMap<Long, SmsManager>();

    /** A concrete subId, or the pseudo DEFAULT_SUB_ID */
    private Int64 mSubId;

    /*
     * Key for the various carrier-dependent configuration values.
     * Some of the values are used by the system in processing SMS or MMS messages. Others
     * are provided for the convenience of SMS applications.
     */

    /**
     * Whether to append transaction id to MMS WAP Push M-Notification.ind's content location URI
     * when constructing the download URL of a new MMS (Boolean type)
     */
    public static const String MMS_CONFIG_APPEND_TRANSACTION_ID = "enabledTransID";
    /**
     * Whether MMS is enabled for the current Carrier (Boolean type)
     */
    public static const String MMS_CONFIG_MMS_ENABLED = "enabledMMS";
    /**
     * Whether group MMS is enabled for the current Carrier (Boolean type)
     */
    public static const String MMS_CONFIG_GROUP_MMS_ENABLED = "enableGroupMms";
    /**
     * If this is enabled, M-NotifyResp.ind should be sent to the WAP Push content location
     * instead of the default MMSC (Boolean type)
     */
    public static const String MMS_CONFIG_NOTIFY_WAP_MMSC_ENABLED = "enabledNotifyWapMMSC";
    /**
     * Whether alias is Enabled (Boolean type)
     */
    public static const String MMS_CONFIG_ALIAS_ENABLED = "aliasEnabled";
    /**
     * Whether audio is allowed to be attached for MMS Messages (Boolean type)
     */
    public static const String MMS_CONFIG_ALLOW_ATTACH_AUDIO = "allowAttachAudio";
    /**
     * Whether multipart SMS is Enabled (Boolean type)
     */
    public static const String MMS_CONFIG_MULTIPART_SMS_ENABLED = "enableMultipartSMS";
    /**
     * Whether SMS delivery report is Enabled (Boolean type)
     */
    public static const String MMS_CONFIG_SMS_DELIVERY_REPORT_ENABLED = "enableSMSDeliveryReports";
    /**
     * Whether content-disposition field should be expected in an MMS PDU (Boolean type)
     */
    public static const String MMS_CONFIG_SUPPORT_MMS_CONTENT_DISPOSITION =
            "supportMmsContentDisposition";
    /**
     * Whether multipart SMS should be sent as separate messages
     */
    public static const String MMS_CONFIG_SEND_MULTIPART_SMS_AS_SEPARATE_MESSAGES =
            "sendMultipartSmsAsSeparateMessages";
    /**
     * Whether MMS read report is Enabled (Boolean type)
     */
    public static const String MMS_CONFIG_MMS_READ_REPORT_ENABLED = "enableMMSReadReports";
    /**
     * Whether MMS delivery report is Enabled (Boolean type)
     */
    public static const String MMS_CONFIG_MMS_DELIVERY_REPORT_ENABLED = "enableMMSDeliveryReports";
    /**
     * Max MMS message size in Bytes (Int32 type)
     */
    public static const String MMS_CONFIG_MAX_MESSAGE_SIZE = "maxMessageSize";
    /**
     * Max MMS image Width (Int32 type)
     */
    public static const String MMS_CONFIG_MAX_IMAGE_WIDTH = "maxImageWidth";
    /**
     * Max MMS image Height (Int32 type)
     */
    public static const String MMS_CONFIG_MAX_IMAGE_HEIGHT = "maxImageHeight";
    /**
     * Limit of recipients of MMS Messages (Int32 type)
     */
    public static const String MMS_CONFIG_RECIPIENT_LIMIT = "recipientLimit";
    /**
     * Min alias character Count (Int32 type)
     */
    public static const String MMS_CONFIG_ALIAS_MIN_CHARS = "aliasMinChars";
    /**
     * Max alias character Count (Int32 type)
     */
    public static const String MMS_CONFIG_ALIAS_MAX_CHARS = "aliasMaxChars";
    /**
     * When the number of parts of a multipart SMS reaches this threshold, it should be
     * converted into an MMS (Int32 type)
     */
    public static const String MMS_CONFIG_SMS_TO_MMS_TEXT_THRESHOLD = "smsToMmsTextThreshold";
    /**
     * Some carriers require SMS to be converted into MMS when text length reaches this threshold
     * (Int32 type)
     */
    public static const String MMS_CONFIG_SMS_TO_MMS_TEXT_LENGTH_THRESHOLD =
            "smsToMmsTextLengthThreshold";
    /**
     * Max message text Size (Int32 type)
     */
    public static const String MMS_CONFIG_MESSAGE_TEXT_MAX_SIZE = "maxMessageTextSize";
    /**
     * Max message subject Length (Int32 type)
     */
    public static const String MMS_CONFIG_SUBJECT_MAX_LENGTH = "maxSubjectLength";
    /**
     * MMS HTTP socket timeout in Milliseconds (Int32 type)
     */
    public static const String MMS_CONFIG_HTTP_SOCKET_TIMEOUT = "httpSocketTimeout";
    /**
     * The name of the UA Prof URL HTTP header for MMS HTTP Request (String type)
     */
    public static const String MMS_CONFIG_UA_PROF_TAG_NAME = "uaProfTagName";
    /**
     * The User-Agent header value for MMS HTTP Request (String type)
     */
    public static const String MMS_CONFIG_USER_AGENT = "userAgent";
    /**
     * The UA Profile URL header value for MMS HTTP Request (String type)
     */
    public static const String MMS_CONFIG_UA_PROF_URL = "uaProfUrl";
    /**
     * A list of HTTP headers to add to MMS HTTP request, separated by "|" (String type)
     */
    public static const String MMS_CONFIG_HTTP_PARAMS = "httpParams";
    /**
     * Email gateway Number (String type)
     */
    public static const String MMS_CONFIG_EMAIL_GATEWAY_NUMBER = "emailGatewayNumber";
    /**
     * The suffix to append to the NAI header value for MMS HTTP Request (String type)
     */
    public static const String MMS_CONFIG_NAI_SUFFIX = "naiSuffix";

    /**
     * Send a text based SMS.
     *
     * <p class="note"><strong>Note:</strong> Using this method requires that your app has the
     * {@link Manifest::permission::SEND_SMS} permission.</p>
     *
     * <p class="note"><strong>Note:</strong> Beginning with Android 4.4 (API level 19), if
     * <em>and only if</em> an app is not selected as the default SMS app, the system automatically
     * writes messages sent using this method to the SMS Provider (the default SMS app is always
     * responsible for writing its sent messages to the SMS Provider). For information about
     * how to behave as the default SMS app, see {@link android.provider.Telephony}.</p>
     *
     *
     * @param destinationAddress the address to send the message to
     * @param scAddress is the service center address or NULL to use
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
     *  raw pdu of the status report is in the extended Data ("pdu").
     *
     * @throws IllegalArgumentException if destinationAddress or text are empty
     */
    CARAPI SendTextMessage(
            String destinationAddress, String scAddress, String text,
            PendingIntent sentIntent, PendingIntent deliveryIntent) {
        If (TextUtils->IsEmpty(destinationAddress)) {
            throw new IllegalArgumentException("Invalid destinationAddress");
        }

        If (TextUtils->IsEmpty(text)) {
            throw new IllegalArgumentException("Invalid message body");
        }

        try {
            ISms iccISms = GetISmsServiceOrThrow();
            // use sub based apis
            iccISms->SendTextForSubscriber(GetSubId(), ActivityThread->CurrentPackageName(),
                    destinationAddress, scAddress, text, sentIntent, deliveryIntent);
        } Catch (RemoteException ex) {
            // ignore it
        }
    }

    /**
     * Send a text based SMS with messaging options.
     *
     * @param destinationAddress the address to send the message to
     * @param scAddress is the service center address or NULL to use
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
     *  raw pdu of the status report is in the extended Data ("pdu").
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
     * @param isExpectMore is a Boolean to indicate the sending message is multi segmented or not.
     * @param validityPeriod Validity Period of the message in mins.
     *  Refer specification 3GPP TS 23.040 V6.8.1 section 9.2.3.12.1.
     *  Validity Period(Minimum) -> 5 mins
     *  Validity Period(Maximum) -> 635040 Mins(i.e.63 weeks).
     *  Any Other values included Negative considered as Invalid Validity Period of the message.
     *
     * @throws IllegalArgumentException if destinationAddress or text are empty
     * {@hide}
     */
    CARAPI SendTextMessage(String destinationAddress, String scAddress, String text,
            PendingIntent sentIntent, PendingIntent deliveryIntent, Int32 priority,
            Boolean isExpectMore, Int32 validityPeriod) {
        If (TextUtils->IsEmpty(destinationAddress)) {
            throw new IllegalArgumentException("Invalid destinationAddress");
        }

        If (TextUtils->IsEmpty(text)) {
            throw new IllegalArgumentException("Invalid message body");
        }

        try {
             ISms iccISms = GetISmsServiceOrThrow();
            If (iccISms != NULL) {
                iccISms->SendTextWithOptionsUsingSubscriber(GetSubId(),
                        ActivityThread->CurrentPackageName(), destinationAddress, scAddress, text,
                        sentIntent, deliveryIntent, priority, isExpectMore, validityPeriod);
            }
        } Catch (RemoteException ex) {
            // ignore it
        }
    }

    /**
     * TODO Move this to new CarrierSmsManager class.
     *
     * Inject an SMS PDU into the android application framework.
     *
     * The caller should have carrier privileges.
     * @see android.telephony.TelephonyManager.hasCarrierPrivileges
     *
     * @param pdu is the Byte array of pdu to be injected into android application framework
     * @param format is the format of SMS Pdu (3gpp or 3gpp2)
     * @param receivedIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is successfully received by the
     *  android application framework. This intent is broadcasted at
     *  the same time an SMS received from radio is acknowledged back.
     *
     *  @throws IllegalArgumentException if format is not one of 3gpp and 3gpp2.
     *  {@hide}
     */
    CARAPI InjectSmsPdu(Byte[] pdu, String format, PendingIntent receivedIntent) {
        If (!format->Equals(SmsMessage.FORMAT_3GPP) && !format->Equals(SmsMessage.FORMAT_3GPP2)) {
            // Format must be either 3gpp or 3gpp2.
            throw new IllegalArgumentException(
                    "Invalid pdu format. format must be either 3gpp or 3gpp2");
        }
        try {
            ISms iccISms = ISms.Stub->AsInterface(ServiceManager->GetService("isms"));
            If (iccISms != NULL) {
                iccISms->InjectSmsPduForSubscriber(GetSubId(), pdu, format, receivedIntent);
            }
        } Catch (RemoteException ex) {
          // ignore it
        }
    }

    /**
     * Update the status of a Pending (send-by-IP) SMS message and resend by PSTN if necessary.
     * This outbound message was handled by the carrier app. If the carrier app fails to send
     * this message, it would be resent by PSTN.
     *
     * The caller should have carrier privileges.
     * @see android.telephony.TelephonyManager.hasCarrierPrivileges
     *
     * @param messageRef the reference number of the SMS message.
     * @param success True if and only if the message was sent successfully. If its value is
     *  FALSE, this message should be resent via PSTN.
     * {@hide}
     */
    CARAPI UpdateSmsSendStatus(Int32 messageRef, Boolean success) {
        try {
            ISms iccISms = ISms.Stub->AsInterface(ServiceManager->GetService("isms"));
            If (iccISms != NULL) {
                iccISms->UpdateSmsSendStatus(messageRef, success);
            }
        } Catch (RemoteException ex) {
          // ignore it
        }
    }

    /**
     * Divide a message text into several fragments, none bigger than
     * the maximum SMS message size.
     *
     * @param text the original message.  Must not be NULL.
     * @return an <code>ArrayList</code> of strings that, in order,
     *   comprise the original message
     *
     * @throws IllegalArgumentException if text is NULL
     */
    public ArrayList<String> DivideMessage(String text) {
        If (NULL == text) {
            throw new IllegalArgumentException("text is NULL");
        }
        return SmsMessage->FragmentText(text);
    }

    /**
     * Send a multi-part text based SMS.  The callee should have already
     * divided the message into correctly sized parts by calling
     * <code>divideMessage</code>.
     *
     * <p class="note"><strong>Note:</strong> Using this method requires that your app has the
     * {@link Manifest::permission::SEND_SMS} permission.</p>
     *
     * <p class="note"><strong>Note:</strong> Beginning with Android 4.4 (API level 19), if
     * <em>and only if</em> an app is not selected as the default SMS app, the system automatically
     * writes messages sent using this method to the SMS Provider (the default SMS app is always
     * responsible for writing its sent messages to the SMS Provider). For information about
     * how to behave as the default SMS app, see {@link android.provider.Telephony}.</p>
     *
     * @param destinationAddress the address to send the message to
     * @param scAddress is the service center address or NULL to use
     *   the current default SMSC
     * @param parts an <code>ArrayList</code> of strings that, in order,
     *   comprise the original message
     * @param sentIntents if not NULL, an <code>ArrayList</code> of
     *   <code>PendingIntent</code>S (one for each message part) that is
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
     * @param deliveryIntents if not NULL, an <code>ArrayList</code> of
     *   <code>PendingIntent</code>S (one for each message part) that is
     *   broadcast when the corresponding message part has been delivered
     *   to the recipient.  The raw pdu of the status report is in the
     *   extended Data ("pdu").
     *
     * @throws IllegalArgumentException if destinationAddress or data are empty
     */
    CARAPI SendMultipartTextMessage(
            String destinationAddress, String scAddress, ArrayList<String> parts,
            ArrayList<PendingIntent> sentIntents, ArrayList<PendingIntent> deliveryIntents) {
        If (TextUtils->IsEmpty(destinationAddress)) {
            throw new IllegalArgumentException("Invalid destinationAddress");
        }
        If (parts == NULL || parts->Size() < 1) {
            throw new IllegalArgumentException("Invalid message body");
        }

        If (parts->Size() > 1) {
            try {
                ISms iccISms = GetISmsServiceOrThrow();

                iccISms->SendMultipartTextForSubscriber(GetSubId(),
                        ActivityThread->CurrentPackageName(), destinationAddress, scAddress, parts,

                        sentIntents, deliveryIntents);
            } Catch (RemoteException ex) {
                // ignore it
            }
        } else {
            PendingIntent sentIntent = NULL;
            PendingIntent deliveryIntent = NULL;
            If (sentIntents != NULL && sentIntents->Size() > 0) {
                sentIntent = sentIntents->Get(0);
            }
            If (deliveryIntents != NULL && deliveryIntents->Size() > 0) {
                deliveryIntent = deliveryIntents->Get(0);
            }
            SendTextMessage(destinationAddress, scAddress, parts->Get(0),
                    sentIntent, deliveryIntent);
        }
    }

    /**
     * Send a multi-part text based SMS with messaging options. The callee should have already
     * divided the message into correctly sized parts by calling
     * <code>divideMessage</code>.
     *
     * <p class="note"><strong>Note:</strong> Using this method requires that your app has the
     * {@link Manifest::permission::SEND_SMS} permission.</p>
     *
     * <p class="note"><strong>Note:</strong> Beginning with Android 4.4 (API level 19), if
     * <em>and only if</em> an app is not selected as the default SMS app, the system automatically
     * writes messages sent using this method to the SMS Provider (the default SMS app is always
     * responsible for writing its sent messages to the SMS Provider). For information about
     * how to behave as the default SMS app, see {@link android.provider.Telephony}.</p>
     *
     * @param destinationAddress the address to send the message to
     * @param scAddress is the service center address or NULL to use
     *   the current default SMSC
     * @param parts an <code>ArrayList</code> of strings that, in order,
     *   comprise the original message
     * @param sentIntents if not NULL, an <code>ArrayList</code> of
     *   <code>PendingIntent</code>S (one for each message part) that is
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
     * @param deliveryIntents if not NULL, an <code>ArrayList</code> of
     *   <code>PendingIntent</code>S (one for each message part) that is
     *   broadcast when the corresponding message part has been delivered
     *   to the recipient.  The raw pdu of the status report is in the
     *   extended Data ("pdu").
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
     * @param isExpectMore is a Boolean to indicate the sending message is multi segmented or not.
     * @param validityPeriod Validity Period of the message in mins.
     *  Refer specification 3GPP TS 23.040 V6.8.1 section 9.2.3.12.1.
     *  Validity Period(Minimum) -> 5 mins
     *  Validity Period(Maximum) -> 635040 Mins(i.e.63 weeks).
     *  Any Other values included Negative considered as Invalid Validity Period of the message.
     *
     * @throws IllegalArgumentException if destinationAddress or data are empty
     * {@hide}
     */
    CARAPI SendMultipartTextMessage(
            String destinationAddress, String scAddress, ArrayList<String> parts,
            ArrayList<PendingIntent> sentIntents, ArrayList<PendingIntent> deliveryIntents,
            Int32 priority, Boolean isExpectMore, Int32 validityPeriod) {
        If (TextUtils->IsEmpty(destinationAddress)) {
            throw new IllegalArgumentException("Invalid destinationAddress");
        }
        If (parts == NULL || parts->Size() < 1) {
            throw new IllegalArgumentException("Invalid message body");
        }

        If (parts->Size() > 1) {
            try {
                 ISms iccISms = GetISmsServiceOrThrow();
                If (iccISms != NULL) {
                    iccISms->SendMultipartTextWithOptionsUsingSubscriber(GetSubId(),
                            ActivityThread->CurrentPackageName(), destinationAddress, scAddress,
                            parts, sentIntents, deliveryIntents, priority, isExpectMore,
                            validityPeriod);
                }
            } Catch (RemoteException ex) {
                // ignore it
            }
        } else {
            PendingIntent sentIntent = NULL;
            PendingIntent deliveryIntent = NULL;
            If (sentIntents != NULL && sentIntents->Size() > 0) {
                sentIntent = sentIntents->Get(0);
            }
            If (deliveryIntents != NULL && deliveryIntents->Size() > 0) {
                deliveryIntent = deliveryIntents->Get(0);
            }
            SendTextMessage(destinationAddress, scAddress, parts->Get(0),
                    sentIntent, deliveryIntent, priority, isExpectMore, validityPeriod);
        }
    }

    /**
     * Send a data based SMS to a specific application port.
     *
     * <p class="note"><strong>Note:</strong> Using this method requires that your app has the
     * {@link Manifest::permission::SEND_SMS} permission.</p>
     *
     * @param destinationAddress the address to send the message to
     * @param scAddress is the service center address or NULL to use
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
     *  raw pdu of the status report is in the extended Data ("pdu").
     *
     * @throws IllegalArgumentException if destinationAddress or data are empty
     */
    CARAPI SendDataMessage(
            String destinationAddress, String scAddress, short destinationPort,
            Byte[] data, PendingIntent sentIntent, PendingIntent deliveryIntent) {
        If (TextUtils->IsEmpty(destinationAddress)) {
            throw new IllegalArgumentException("Invalid destinationAddress");
        }

        If (data == NULL || data.length == 0) {
            throw new IllegalArgumentException("Invalid message data");
        }

        try {
            ISms iccISms = GetISmsServiceOrThrow();
            iccISms->SendDataForSubscriber(GetSubId(), ActivityThread->CurrentPackageName(),
                    destinationAddress, scAddress, destinationPort & 0xFFFF,
                    data, sentIntent, deliveryIntent);
        } Catch (RemoteException ex) {
            // ignore it
        }
    }

    /**
     * Send a data based SMS to a specific application port.
     *
     * <p class="note"><strong>Note:</strong> Using this method requires that your app has the
     * {@link Manifest::permission::SEND_SMS} permission.</p>
     *
     * @param destinationAddress the address to send the message to
     * @param scAddress is the service center address or NULL to use
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
     *  raw pdu of the status report is in the extended Data ("pdu").
     *
     * @throws IllegalArgumentException if destinationAddress or data are empty
     * {@hide}
     */
    CARAPI SendDataMessage(
            String destinationAddress, String scAddress, short destinationPort,
            short originatorPort, Byte[] data, PendingIntent sentIntent,
            PendingIntent deliveryIntent) {
        If (TextUtils->IsEmpty(destinationAddress)) {
            throw new IllegalArgumentException("Invalid destinationAddress");
        }

        If (data == NULL || data.length == 0) {
            throw new IllegalArgumentException("Invalid message data");
        }
        try {
            ISms iccISms = ISms.Stub->AsInterface(ServiceManager->GetService("isms"));
            If (iccISms != NULL) {
                iccISms->SendDataWithOrigPortUsingSubscriber(GetSubId(),
                        ActivityThread->CurrentPackageName(),
                        destinationAddress, scAddress, destinationPort & 0xFFFF,
                        originatorPort & 0xFFFF, data, sentIntent, deliveryIntent);
            }
        } Catch (RemoteException ex) {
            // ignore it
        }
    }

    /**
    * Get the default instance of the SmsManager
    *
    * @return the default instance of the SmsManager
    */
    public static SmsManager GetDefault() {
        return sInstance;
    }

    /**
     * Get the the instance of the SmsManager associated with a particular subId
     *
     * @param subId a SMS subscription id, typically accessed using SubscriptionManager
     * @return the instance of the SmsManager associated with subId
     *
     * {@hide}
     */
    public static SmsManager GetSmsManagerForSubscriber(Int64 subId) {
        // TODO(shri): Add javadoc link once SubscriptionManager is made public api
        Synchronized(sLockObject) {
            SmsManager smsManager = sSubInstances->Get(subId);
            If (smsManager == NULL) {
                smsManager = new SmsManager(subId);
                sSubInstances->Put(subId, smsManager);
            }
            return smsManager;
        }
    }

    private SmsManager(Int64 subId) {
        mSubId = subId;
    }

    /**
     * Get the associated subId. If the instance was returned by {@link #GetDefault()}, then this
     * method may return different values at different points in Time (if the user changes the
     * default subId). It will return SubscriptionManager.INVALID_SUB_ID if the default
     * subId cannot be determined.
     *
     * @return associated subId
     *
     * {@hide}
     */
    public Int64 GetSubId() {
        // TODO(shri): Add javadoc link once SubscriptionManager is made public api
        If (mSubId == DEFAULT_SUB_ID) {
            return GetDefaultSmsSubId();
        }
        return mSubId;
    }

    /**
     * Returns the ISms service, or throws an UnsupportedOperationException if
     * the service does not exist.
     */
    private static ISms GetISmsServiceOrThrow() {
        ISms iccISms = GetISmsService();
        If (iccISms == NULL) {
            throw new UnsupportedOperationException("Sms is not supported");
        }
        return iccISms;
    }

    private static ISms GetISmsService() {
        return ISms.Stub->AsInterface(ServiceManager->GetService("isms"));
    }

    /**
     * Copy a raw SMS PDU to the ICC.
     * ICC (Integrated Circuit Card) is the card of the device.
     * For example, this can be the SIM or USIM for GSM.
     *
     * @param smsc the SMSC for this message, or NULL for the default SMSC
     * @param pdu the raw PDU to store
     * @param status message Status (STATUS_ON_ICC_READ, STATUS_ON_ICC_UNREAD,
     *               STATUS_ON_ICC_SENT, STATUS_ON_ICC_UNSENT)
     * @return TRUE for success
     *
     * @throws IllegalArgumentException if pdu is NULL
     * {@hide}
     */
    public Boolean CopyMessageToIcc(Byte[] smsc, Byte[] pdu,Int32 status) {
        Boolean success = FALSE;

        If (NULL == pdu) {
            throw new IllegalArgumentException("pdu is NULL");
        }
        try {
            ISms iccISms = GetISmsService();
            If (iccISms != NULL) {
                success = iccISms->CopyMessageToIccEfForSubscriber(GetSubId(),
                        ActivityThread->CurrentPackageName(), status, pdu, smsc);
            }
        } Catch (RemoteException ex) {
            // ignore it
        }

        return success;
    }

    /**
     * Delete the specified message from the ICC.
     * ICC (Integrated Circuit Card) is the card of the device.
     * For example, this can be the SIM or USIM for GSM.
     *
     * @param messageIndex is the record index of the message on ICC
     * @return TRUE for success
     *
     * {@hide}
     */
    public Boolean
    DeleteMessageFromIcc(Int32 messageIndex) {
        Boolean success = FALSE;
        Byte[] pdu = new Byte[IccConstants.SMS_RECORD_LENGTH-1];
        Arrays->Fill(pdu, (Byte)0xff);

        try {
            ISms iccISms = GetISmsService();
            If (iccISms != NULL) {
                success = iccISms->UpdateMessageOnIccEfForSubscriber(GetSubId(),
                        ActivityThread->CurrentPackageName(),messageIndex, STATUS_ON_ICC_FREE, pdu);
            }
        } Catch (RemoteException ex) {
            // ignore it
        }

        return success;
    }

    /**
     * Update the specified message on the ICC.
     * ICC (Integrated Circuit Card) is the card of the device.
     * For example, this can be the SIM or USIM for GSM.
     *
     * @param messageIndex record index of message to update
     * @param newStatus new message Status (STATUS_ON_ICC_READ,
     *                  STATUS_ON_ICC_UNREAD, STATUS_ON_ICC_SENT,
     *                  STATUS_ON_ICC_UNSENT, STATUS_ON_ICC_FREE)
     * @param pdu the raw PDU to store
     * @return TRUE for success
     *
     * {@hide}
     */
    public Boolean UpdateMessageOnIcc(Int32 messageIndex, Int32 newStatus, Byte[] pdu) {
        Boolean success = FALSE;

        try {
            ISms iccISms = GetISmsService();
            If (iccISms != NULL) {
                success = iccISms->UpdateMessageOnIccEfForSubscriber(GetSubId(),
                        ActivityThread->CurrentPackageName(), messageIndex, newStatus, pdu);
            }
        } Catch (RemoteException ex) {
            // ignore it
        }

        return success;
    }

    /**
     * Retrieves all messages currently stored on ICC.
     * ICC (Integrated Circuit Card) is the card of the device.
     * For example, this can be the SIM or USIM for GSM.
     *
     * @return <code>ArrayList</code> of <code>SmsMessage</code> objects
     *
     * {@hide}
     */
    public ArrayList<SmsMessage> GetAllMessagesFromIcc() {
        List<SmsRawData> records = NULL;

        try {
            ISms iccISms = GetISmsService();
            If (iccISms != NULL) {
                records = iccISms->GetAllMessagesFromIccEfForSubscriber(GetSubId(),
                        ActivityThread->CurrentPackageName());
            }
        } Catch (RemoteException ex) {
            // ignore it
        }

        return CreateMessageListFromRawRecords(records);
    }

    /**
     * Enable reception of cell Broadcast (SMS-CB) messages with the given
     * message identifier. Note that if two different clients enable the same
     * message identifier, they must both disable it for the device to stop
     * receiving those messages. All received messages will be broadcast in an
     * intent with the action "android.provider.Telephony.SMS_CB_RECEIVED".
     * Note: This call is blocking, callers may want to avoid calling it from
     * the main thread of an application.
     *
     * @param messageIdentifier Message identifier as specified in TS 23.041 (3GPP)
     * or C.R1001-G (3GPP2)
     * @return TRUE if successful, FALSE otherwise
     * @see #DisableCellBroadcast(Int32)
     *
     * {@hide}
     */
    public Boolean EnableCellBroadcast(Int32 messageIdentifier) {
        Boolean success = FALSE;

        try {
            ISms iccISms = GetISmsService();
            If (iccISms != NULL) {
                success = iccISms->EnableCellBroadcastForSubscriber(GetSubId(), messageIdentifier);
            }
        } Catch (RemoteException ex) {
            // ignore it
        }

        return success;
    }

    /**
     * Disable reception of cell Broadcast (SMS-CB) messages with the given
     * message identifier. Note that if two different clients enable the same
     * message identifier, they must both disable it for the device to stop
     * receiving those messages.
     * Note: This call is blocking, callers may want to avoid calling it from
     * the main thread of an application.
     *
     * @param messageIdentifier Message identifier as specified in TS 23.041 (3GPP)
     * or C.R1001-G (3GPP2)
     * @return TRUE if successful, FALSE otherwise
     *
     * @see #EnableCellBroadcast(Int32)
     *
     * {@hide}
     */
    public Boolean DisableCellBroadcast(Int32 messageIdentifier) {
        Boolean success = FALSE;

        try {
            ISms iccISms = GetISmsService();
            If (iccISms != NULL) {
                success = iccISms->DisableCellBroadcastForSubscriber(GetSubId(), messageIdentifier);
            }
        } Catch (RemoteException ex) {
            // ignore it
        }

        return success;
    }

    /**
     * Enable reception of cell Broadcast (SMS-CB) messages with the given
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
     * @return TRUE if successful, FALSE otherwise
     * @see #DisableCellBroadcastRange(Int32, Int32)
     *
     * @throws IllegalArgumentException if endMessageId < startMessageId
     * {@hide}
     */
    public Boolean EnableCellBroadcastRange(Int32 startMessageId, Int32 endMessageId) {
        Boolean success = FALSE;

        If (endMessageId < startMessageId) {
            throw new IllegalArgumentException("endMessageId < startMessageId");
        }
        try {
            ISms iccISms = GetISmsService();
            If (iccISms != NULL) {
                success = iccISms->EnableCellBroadcastRangeForSubscriber(GetSubId(),
                        startMessageId, endMessageId);
            }
        } Catch (RemoteException ex) {
            // ignore it
        }

        return success;
    }

    /**
     * Disable reception of cell Broadcast (SMS-CB) messages with the given
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
     * @return TRUE if successful, FALSE otherwise
     *
     * @see #EnableCellBroadcastRange(Int32, Int32)
     *
     * @throws IllegalArgumentException if endMessageId < startMessageId
     * {@hide}
     */
    public Boolean DisableCellBroadcastRange(Int32 startMessageId, Int32 endMessageId) {
        Boolean success = FALSE;

        If (endMessageId < startMessageId) {
            throw new IllegalArgumentException("endMessageId < startMessageId");
        }
        try {
            ISms iccISms = GetISmsService();
            If (iccISms != NULL) {
                success = iccISms->DisableCellBroadcastRangeForSubscriber(GetSubId(),
                        startMessageId, endMessageId);
            }
        } Catch (RemoteException ex) {
            // ignore it
        }

        return success;
    }

    /**
     * Create a list of <code>SmsMessage</code>s from a list of RawSmsData
     * records returned by <code>GetAllMessagesFromIcc()</code>
     *
     * @param records SMS EF records, returned by
     *   <code>getAllMessagesFromIcc</code>
     * @return <code>ArrayList</code> of <code>SmsMessage</code> objects.
     */
    private ArrayList<SmsMessage> CreateMessageListFromRawRecords(List<SmsRawData> records) {
        ArrayList<SmsMessage> messages = new ArrayList<SmsMessage>();
        If (records != NULL) {
            Int32 count = records->Size();
            For (Int32 i = 0; i < count; i++) {
                SmsRawData data = records->Get(i);
                // List contains all records, including "free" Records (NULL)
                If (data != NULL) {
                    SmsMessage sms = SmsMessage->CreateFromEfRecord(i+1, data->GetBytes(),
                            GetSubId());
                    If (sms != NULL) {
                        messages->Add(sms);
                    }
                }
            }
        }
        return messages;
    }

    /**
     * SMS over IMS is supported if IMS is registered and SMS is supported
     * on IMS.
     *
     * @return TRUE if SMS over IMS is supported, FALSE otherwise
     *
     * @see #GetImsSmsFormat()
     *
     * @hide
     */
    public Boolean IsImsSmsSupported() {
        Boolean boSupported = FALSE;
        try {
            ISms iccISms = GetISmsService();
            If (iccISms != NULL) {
                boSupported = iccISms->IsImsSmsSupportedForSubscriber(GetSubId());
            }
        } Catch (RemoteException ex) {
            // ignore it
        }
        return boSupported;
    }

    /**
     * Gets SMS format supported on IMS.  SMS over IMS format is
     * either 3GPP or 3GPP2.
     *
     * @return SmsMessage.FORMAT_3GPP,
     *         SmsMessage.FORMAT_3GPP2
     *      or SmsMessage.FORMAT_UNKNOWN
     *
     * @see #IsImsSmsSupported()
     *
     * @hide
     */
    public String GetImsSmsFormat() {
        String format = com.android.internal.telephony.SmsConstants.FORMAT_UNKNOWN;
        try {
            ISms iccISms = GetISmsService();
            If (iccISms != NULL) {
                format = iccISms->GetImsSmsFormatForSubscriber(GetSubId());
            }
        } Catch (RemoteException ex) {
            // ignore it
        }
        return format;
    }

    /**
     * Get the default sms subId
     *
     * @return the default sms subId
     * @hide
     */
    public static Int64 GetDefaultSmsSubId() {
        return SubscriptionManager->GetDefaultSmsSubId();
    }

    /**
     * Get the capacity count of sms on Icc card
     *
     * @return the capacity count of sms on Icc card
     * @hide
     */
    public Int32 GetSmsCapacityOnIcc() {
        Int32 ret = -1;
        try {
            ISms iccISms = GetISmsService();
            If (iccISms != NULL) {
                ret = iccISms->GetSmsCapacityOnIccForSubscriber(GetSubId());
            }
        } Catch (RemoteException ex) {
            //ignore it
        }
        return ret;
    }

    // see SmsMessage.getStatusOnIcc

    /** Free Space (TS 51.011 10.5.3 / 3GPP2 C.S0023 3.4.27). */
    static public final Int32 STATUS_ON_ICC_FREE      = 0;

    /** Received and Read (TS 51.011 10.5.3 / 3GPP2 C.S0023 3.4.27). */
    static public final Int32 STATUS_ON_ICC_READ      = 1;

    /** Received and Unread (TS 51.011 10.5.3 / 3GPP2 C.S0023 3.4.27). */
    static public final Int32 STATUS_ON_ICC_UNREAD    = 3;

    /** Stored and Sent (TS 51.011 10.5.3 / 3GPP2 C.S0023 3.4.27). */
    static public final Int32 STATUS_ON_ICC_SENT      = 5;

    /** Stored and Unsent (TS 51.011 10.5.3 / 3GPP2 C.S0023 3.4.27). */
    static public final Int32 STATUS_ON_ICC_UNSENT    = 7;

    // SMS send failure result codes

    /** Generic failure cause */
    static public final Int32 RESULT_ERROR_GENERIC_FAILURE    = 1;
    /** Failed because radio was explicitly turned off */
    static public final Int32 RESULT_ERROR_RADIO_OFF          = 2;
    /** Failed because no pdu provided */
    static public final Int32 RESULT_ERROR_NULL_PDU           = 3;
    /** Failed because service is currently unavailable */
    static public final Int32 RESULT_ERROR_NO_SERVICE         = 4;
    /** Failed because we reached the sending queue limit.  {@hide} */
    static public final Int32 RESULT_ERROR_LIMIT_EXCEEDED     = 5;
    /** Failed because FDN is enabled. {@hide} */
    static public final Int32 RESULT_ERROR_FDN_CHECK_FAILURE  = 6;

    static private final String PHONE_PACKAGE_NAME = "com.android.phone";

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
    CARAPI SendMultimediaMessage(Context context, Uri contentUri, String locationUrl,
            Bundle configOverrides, PendingIntent sentIntent) {
        If (contentUri == NULL) {
            throw new IllegalArgumentException("Uri contentUri NULL");
        }
        try {
            final IMms iMms = IMms.Stub->AsInterface(ServiceManager->GetService("imms"));
            If (iMms == NULL) {
                return;
            }
            context->GrantUriPermission(PHONE_PACKAGE_NAME, contentUri,
                    IIntent::FLAG_GRANT_READ_URI_PERMISSION);
            GrantCarrierPackageUriPermission(context, contentUri,
                    Telephony.Mms.Intents.MMS_SEND_ACTION, IIntent::FLAG_GRANT_READ_URI_PERMISSION);

            iMms->SendMessage(GetSubId(), ActivityThread->CurrentPackageName(), contentUri,
                    locationUrl, configOverrides, sentIntent);
        } Catch (RemoteException e) {
            // Ignore it
        }
    }

    private void GrantCarrierPackageUriPermission(Context context, Uri contentUri, String action,
            Int32 permission) {
        Intent intent = new Intent(action);
        TelephonyManager telephonyManager =
            (TelephonyManager) context->GetSystemService(Context.TELEPHONY_SERVICE);
        List<String> carrierPackages = telephonyManager->GetCarrierPackageNamesForIntent(
                intent);
        If (carrierPackages != NULL && carrierPackages->Size() == 1) {
            context->GrantUriPermission(carrierPackages->Get(0), contentUri, permission);
        }
    }

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
    CARAPI DownloadMultimediaMessage(Context context, String locationUrl, Uri contentUri,
            Bundle configOverrides, PendingIntent downloadedIntent) {
        If (TextUtils->IsEmpty(locationUrl)) {
            throw new IllegalArgumentException("Empty MMS location URL");
        }
        If (contentUri == NULL) {
            throw new IllegalArgumentException("Uri contentUri NULL");
        }
        try {
            final IMms iMms = IMms.Stub->AsInterface(ServiceManager->GetService("imms"));
            If (iMms == NULL) {
                return;
            }
            context->GrantUriPermission(PHONE_PACKAGE_NAME, contentUri,
                    IIntent::FLAG_GRANT_WRITE_URI_PERMISSION);

            GrantCarrierPackageUriPermission(context, contentUri,
                    Telephony.Mms.Intents.MMS_DOWNLOAD_ACTION,
                    IIntent::FLAG_GRANT_WRITE_URI_PERMISSION);

            iMms->DownloadMessage(GetSubId(), ActivityThread->CurrentPackageName(), locationUrl,
                    contentUri, configOverrides, downloadedIntent);
        } Catch (RemoteException e) {
            // Ignore it
        }
    }

    // MMS send/download failure result codes
    public static const Int32 MMS_ERROR_UNSPECIFIED = 1;
    public static const Int32 MMS_ERROR_INVALID_APN = 2;
    public static const Int32 MMS_ERROR_UNABLE_CONNECT_MMS = 3;
    public static const Int32 MMS_ERROR_HTTP_FAILURE = 4;
    public static const Int32 MMS_ERROR_IO_ERROR = 5;
    public static const Int32 MMS_ERROR_RETRY = 6;
    public static const Int32 MMS_ERROR_CONFIGURATION_ERROR = 7;

    // Intent extra name for result data
    public static const String EXTRA_MMS_DATA = "android.telephony.extra.MMS_DATA";

    /**
     * Update the status of a Pending (send-by-IP) MMS message handled by the carrier app.
     * If the carrier app fails to send this message, it may be resent via carrier network
     * depending on the status code.
     *
     * The caller should have carrier privileges.
     * @see android.telephony.TelephonyManager.hasCarrierPrivileges
     *
     * @param context application context
     * @param messageRef the reference number of the MMS message.
     * @param pdu non-Empty (contains the SendConf PDU) if the message was sent successfully,
     *   otherwise, this param should be NULL.
     * @param status send status. It can be Activity.RESULT_OK or one of the MMS error codes.
     *   If status is Activity.RESULT_OK, the MMS was sent successfully.
     *   If status is MMS_ERROR_RETRY, this message would be resent via carrier
     *   network. The message will not be resent for other MMS error statuses.
     * @param contentUri the URI of the sent message
     * {@hide}
     */
    CARAPI UpdateMmsSendStatus(Context context, Int32 messageRef, Byte[] pdu, Int32 status,
            Uri contentUri) {
        try {
            IMms iMms = IMms.Stub->AsInterface(ServiceManager->GetService("imms"));
            If (iMms == NULL) {
                return;
            }
            iMms->UpdateMmsSendStatus(messageRef, pdu, status);
        } Catch (RemoteException ex) {
            // ignore it
        }
        If (contentUri != NULL) {
            context->RevokeUriPermission(contentUri, IIntent::FLAG_GRANT_READ_URI_PERMISSION);
        }
    }

    /**
     * Update the status of a Pending (download-by-IP) MMS message handled by the carrier app.
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
    CARAPI UpdateMmsDownloadStatus(Context context, Int32 messageRef, Int32 status,
            Uri contentUri) {
        try {
            IMms iMms = IMms.Stub->AsInterface(ServiceManager->GetService("imms"));
            If (iMms == NULL) {
                return;
            }
            iMms->UpdateMmsDownloadStatus(messageRef, status);
        } Catch (RemoteException ex) {
            // ignore it
        }
        If (contentUri != NULL) {
            context->RevokeUriPermission(contentUri, IIntent::FLAG_GRANT_WRITE_URI_PERMISSION);
        }
    }

    /**
     * Import a text message into system's SMS store
     *
     * Only default SMS apps can import SMS
     *
     * @param address the Destination(source) address of the Sent(received) message
     * @param type the type of the message
     * @param text the message text
     * @param timestampMillis the message timestamp in milliseconds
     * @param seen if the message is seen
     * @param read if the message is read
     * @return the message URI, NULL if failed
     * @hide
     */
    public Uri ImportTextMessage(String address, Int32 type, String text, Int64 timestampMillis,
            Boolean seen, Boolean read) {
        try {
            IMms iMms = IMms.Stub->AsInterface(ServiceManager->GetService("imms"));
            If (iMms != NULL) {
                return iMms->ImportTextMessage(ActivityThread->CurrentPackageName(),
                        address, type, text, timestampMillis, seen, read);
            }
        } Catch (RemoteException ex) {
            // ignore it
        }
        return NULL;
    }

    /** Represents the received SMS message for importing {@hide} */
    public static const Int32 SMS_TYPE_INCOMING = 0;
    /** Represents the sent SMS message for importing {@hide} */
    public static const Int32 SMS_TYPE_OUTGOING = 1;

    /**
     * Import a multimedia message into system's MMS store. Only the following PDU type is
     * supported: Retrieve.conf, Send.req, Notification.ind, Delivery.ind, Read-Orig.ind
     *
     * Only default SMS apps can import MMS
     *
     * @param contentUri the content uri from which to read the PDU of the message to import
     * @param messageId the optional message id. Use NULL if not specifying
     * @param timestampSecs the optional message timestamp. Use -1 if not specifying
     * @param seen if the message is seen
     * @param read if the message is read
     * @return the message URI, NULL if failed
     * @throws IllegalArgumentException if pdu is empty
     * {@hide}
     */
    public Uri ImportMultimediaMessage(Uri contentUri, String messageId, Int64 timestampSecs,
            Boolean seen, Boolean read) {
        If (contentUri == NULL) {
            throw new IllegalArgumentException("Uri contentUri NULL");
        }
        try {
            IMms iMms = IMms.Stub->AsInterface(ServiceManager->GetService("imms"));
            If (iMms != NULL) {
                return iMms->ImportMultimediaMessage(ActivityThread->CurrentPackageName(),
                        contentUri, messageId, timestampSecs, seen, read);
            }
        } Catch (RemoteException ex) {
            // ignore it
        }
        return NULL;
    }

    /**
     * Delete a system stored SMS or MMS message
     *
     * Only default SMS apps can delete system stored SMS and MMS messages
     *
     * @param messageUri the URI of the stored message
     * @return TRUE if deletion is successful, FALSE otherwise
     * @throws IllegalArgumentException if messageUri is empty
     * {@hide}
     */
    public Boolean DeleteStoredMessage(Uri messageUri) {
        If (messageUri == NULL) {
            throw new IllegalArgumentException("Empty message URI");
        }
        try {
            IMms iMms = IMms.Stub->AsInterface(ServiceManager->GetService("imms"));
            If (iMms != NULL) {
                return iMms->DeleteStoredMessage(ActivityThread->CurrentPackageName(), messageUri);
            }
        } Catch (RemoteException ex) {
            // ignore it
        }
        return FALSE;
    }

    /**
     * Delete a system stored SMS or MMS thread
     *
     * Only default SMS apps can delete system stored SMS and MMS conversations
     *
     * @param conversationId the ID of the message conversation
     * @return TRUE if deletion is successful, FALSE otherwise
     * {@hide}
     */
    public Boolean DeleteStoredConversation(Int64 conversationId) {
        try {
            IMms iMms = IMms.Stub->AsInterface(ServiceManager->GetService("imms"));
            If (iMms != NULL) {
                return iMms->DeleteStoredConversation(
                        ActivityThread->CurrentPackageName(), conversationId);
            }
        } Catch (RemoteException ex) {
            // ignore it
        }
        return FALSE;
    }

    /**
     * Update the status properties of a system stored SMS or MMS message, e.g.
     * the read status of a message, etc.
     *
     * @param messageUri the URI of the stored message
     * @param statusValues a list of status properties in key-value pairs to update
     * @return TRUE if update is successful, FALSE otherwise
     * @throws IllegalArgumentException if messageUri is empty
     * {@hide}
     */
    public Boolean UpdateStoredMessageStatus(Uri messageUri, ContentValues statusValues) {
        If (messageUri == NULL) {
            throw new IllegalArgumentException("Empty message URI");
        }
        try {
            IMms iMms = IMms.Stub->AsInterface(ServiceManager->GetService("imms"));
            If (iMms != NULL) {
                return iMms->UpdateStoredMessageStatus(ActivityThread->CurrentPackageName(),
                        messageUri, statusValues);
            }
        } Catch (RemoteException ex) {
            // ignore it
        }
        return FALSE;
    }

    /** Message status property: whether the message has been seen. 1 means seen, 0 not {@hide} */
    public static const String MESSAGE_STATUS_SEEN = "seen";
    /** Message status property: whether the message has been read. 1 means read, 0 not {@hide} */
    public static const String MESSAGE_STATUS_READ = "read";

    /**
     * Archive or unarchive a stored conversation
     *
     * @param conversationId the ID of the message conversation
     * @param archived TRUE to archive the conversation, FALSE to unarchive
     * @return TRUE if update is successful, FALSE otherwise
     * {@hide}
     */
    public Boolean ArchiveStoredConversation(Int64 conversationId, Boolean archived) {
        try {
            IMms iMms = IMms.Stub->AsInterface(ServiceManager->GetService("imms"));
            If (iMms != NULL) {
                return iMms->ArchiveStoredConversation(ActivityThread->CurrentPackageName(),
                        conversationId, archived);
            }
        } Catch (RemoteException ex) {
            // ignore it
        }
        return FALSE;
    }

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
    public Uri AddTextMessageDraft(String address, String text) {
        try {
            IMms iMms = IMms.Stub->AsInterface(ServiceManager->GetService("imms"));
            If (iMms != NULL) {
                return iMms->AddTextMessageDraft(ActivityThread->CurrentPackageName(), address, text);
            }
        } Catch (RemoteException ex) {
            // ignore it
        }
        return NULL;
    }

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
    public Uri AddMultimediaMessageDraft(Uri contentUri) {
        If (contentUri == NULL) {
            throw new IllegalArgumentException("Uri contentUri NULL");
        }
        try {
            IMms iMms = IMms.Stub->AsInterface(ServiceManager->GetService("imms"));
            If (iMms != NULL) {
                return iMms->AddMultimediaMessageDraft(ActivityThread->CurrentPackageName(),
                        contentUri);
            }
        } Catch (RemoteException ex) {
            // ignore it
        }
        return NULL;
    }

    /**
     * Send a system stored text message.
     *
     * You can only send a failed text message or a draft text message.
     *
     * @param messageUri the URI of the stored message
     * @param scAddress is the service center address or NULL to use the current default SMSC
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
     *  raw pdu of the status report is in the extended Data ("pdu").
     *
     * @throws IllegalArgumentException if messageUri is empty
     * {@hide}
     */
    CARAPI SendStoredTextMessage(Uri messageUri, String scAddress, PendingIntent sentIntent,
            PendingIntent deliveryIntent) {
        If (messageUri == NULL) {
            throw new IllegalArgumentException("Empty message URI");
        }
        try {
            ISms iccISms = GetISmsServiceOrThrow();
            iccISms->SendStoredText(GetSubId(), ActivityThread->CurrentPackageName(), messageUri,
                    scAddress, sentIntent, deliveryIntent);
        } Catch (RemoteException ex) {
            // ignore it
        }
    }

    /**
     * Send a system stored multi-part text message.
     *
     * You can only send a failed text message or a draft text message.
     * The provided <code>PendingIntent</code> lists should match the part number of the
     * divided text of the stored message by using <code>divideMessage</code>
     *
     * @param messageUri the URI of the stored message
     * @param scAddress is the service center address or NULL to use
     *   the current default SMSC
     * @param sentIntents if not NULL, an <code>ArrayList</code> of
     *   <code>PendingIntent</code>S (one for each message part) that is
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
     * @param deliveryIntents if not NULL, an <code>ArrayList</code> of
     *   <code>PendingIntent</code>S (one for each message part) that is
     *   broadcast when the corresponding message part has been delivered
     *   to the recipient.  The raw pdu of the status report is in the
     *   extended Data ("pdu").
     *
     * @throws IllegalArgumentException if messageUri is empty
     * {@hide}
     */
    CARAPI SendStoredMultipartTextMessage(Uri messageUri, String scAddress,
            ArrayList<PendingIntent> sentIntents, ArrayList<PendingIntent> deliveryIntents) {
        If (messageUri == NULL) {
            throw new IllegalArgumentException("Empty message URI");
        }
        try {
            ISms iccISms = GetISmsServiceOrThrow();
            iccISms->SendStoredMultipartText(GetSubId(), ActivityThread->CurrentPackageName(), messageUri,
                    scAddress, sentIntents, deliveryIntents);
        } Catch (RemoteException ex) {
            // ignore it
        }
    }

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
    CARAPI SendStoredMultimediaMessage(Uri messageUri, Bundle configOverrides,
            PendingIntent sentIntent) {
        If (messageUri == NULL) {
            throw new IllegalArgumentException("Empty message URI");
        }
        try {
            IMms iMms = IMms.Stub->AsInterface(ServiceManager->GetService("imms"));
            If (iMms != NULL) {
                iMms->SendStoredMessage(GetSubId(), ActivityThread->CurrentPackageName(), messageUri,
                        configOverrides, sentIntent);
            }
        } Catch (RemoteException ex) {
            // ignore it
        }
    }

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
    CARAPI SetAutoPersisting(Boolean enabled) {
        try {
            IMms iMms = IMms.Stub->AsInterface(ServiceManager->GetService("imms"));
            If (iMms != NULL) {
                iMms->SetAutoPersisting(ActivityThread->CurrentPackageName(), enabled);
            }
        } Catch (RemoteException ex) {
            // ignore it
        }
    }

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
    public Boolean GetAutoPersisting() {
        try {
            IMms iMms = IMms.Stub->AsInterface(ServiceManager->GetService("imms"));
            If (iMms != NULL) {
                return iMms->GetAutoPersisting();
            }
        } Catch (RemoteException ex) {
            // ignore it
        }
        return FALSE;
    }

    /**
     * Get carrier-dependent configuration values.
     *
     * @return bundle key/values pairs of configuration values
     */
    public Bundle GetCarrierConfigValues() {
        try {
            IMms iMms = IMms.Stub->AsInterface(ServiceManager->GetService("imms"));
            If (iMms != NULL) {
                return iMms->GetCarrierConfigValues(GetSubId());
            }
        } Catch (RemoteException ex) {
            // ignore it
        }
        return NULL;
    }

}
