/*
 * Copyright (C) 2006 The Android Open Source Project
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

package com.android.internal.telephony;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::Sqlite::ISqliteWrapper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ITelephony;
using Elastos::Droid::Provider::Telephony::ISms;
using Elastos::Droid::Provider::Telephony::Sms::IIntents;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISmsManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::IHtml;
using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::IEventLog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Internal::IR;
using Elastos::Droid::Internal::Telephony::GsmAlphabet::ITextEncodingDetails;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IArrays;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::IRandom;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger;

using static android::Telephony::SmsManager::IRESULT_ERROR_FDN_CHECK_FAILURE;
using static android::Telephony::SmsManager::IRESULT_ERROR_GENERIC_FAILURE;
using static android::Telephony::SmsManager::IRESULT_ERROR_LIMIT_EXCEEDED;
using static android::Telephony::SmsManager::IRESULT_ERROR_NO_SERVICE;
using static android::Telephony::SmsManager::IRESULT_ERROR_NULL_PDU;
using static android::Telephony::SmsManager::IRESULT_ERROR_RADIO_OFF;

public abstract class SMSDispatcher extends Handler {
    static const String TAG = "SMSDispatcher";    // accessed from inner class
    static const Boolean DBG = FALSE;
    private static const String SEND_NEXT_MSG_EXTRA = "SendNextMsg";

    /** Permission required to send SMS to short codes without user confirmation. */
    private static const String SEND_SMS_NO_CONFIRMATION_PERMISSION =
            "android.permission.SEND_SMS_NO_CONFIRMATION";

    private static const Int32 PREMIUM_RULE_USE_SIM = 1;
    private static const Int32 PREMIUM_RULE_USE_NETWORK = 2;
    private static const Int32 PREMIUM_RULE_USE_BOTH = 3;
    private final AtomicInteger mPremiumSmsRule = new AtomicInteger(PREMIUM_RULE_USE_SIM);
    private final SettingsObserver mSettingsObserver;

    /** SMS send complete. */
    protected static const Int32 EVENT_SEND_SMS_COMPLETE = 2;

    /** Retry sending a previously failed SMS message */
    private static const Int32 EVENT_SEND_RETRY = 3;

    /** Confirmation required for sending a large number of messages. */
    private static const Int32 EVENT_SEND_LIMIT_REACHED_CONFIRMATION = 4;

    /** Send the user confirmed SMS */
    static const Int32 EVENT_SEND_CONFIRMED_SMS = 5;  // accessed from inner class

    /** Don't send SMS (user did not confirm). */
    static const Int32 EVENT_STOP_SENDING = 7;        // accessed from inner class

    /** Confirmation required for third-party apps sending to an SMS short code. */
    private static const Int32 EVENT_CONFIRM_SEND_TO_POSSIBLE_PREMIUM_SHORT_CODE = 8;

    /** Confirmation required for third-party apps sending to an SMS short code. */
    private static const Int32 EVENT_CONFIRM_SEND_TO_PREMIUM_SHORT_CODE = 9;

    /** Handle status report from {@code CdmaInboundSmsHandler}. */
    protected static const Int32 EVENT_HANDLE_STATUS_REPORT = 10;

    /** Radio is ON */
    protected static const Int32 EVENT_RADIO_ON = 11;

    /** IMS registration/SMS format changed */
    protected static const Int32 EVENT_IMS_STATE_CHANGED = 12;

    /** Callback from RIL_REQUEST_IMS_REGISTRATION_STATE */
    protected static const Int32 EVENT_IMS_STATE_DONE = 13;

    // other
    protected static const Int32 EVENT_NEW_ICC_SMS = 14;
    protected static const Int32 EVENT_ICC_CHANGED = 15;

    /** Class2 SMS  */
    static const protected Int32 EVENT_SMS_ON_ICC = 16;

    protected PhoneBase mPhone;
    protected final Context mContext;
    protected final ContentResolver mResolver;
    protected final CommandsInterface mCi;
    protected final TelephonyManager mTelephonyManager;

    /** Maximum number of times to retry sending a failed SMS. */
    private static const Int32 MAX_SEND_RETRIES = 3;
    /** Delay before next send attempt on a failed SMS, in milliseconds. */
    private static const Int32 SEND_RETRY_DELAY = 2000;
    /** single part SMS */
    private static const Int32 SINGLE_PART_SMS = 1;
    /** Message sending queue limit */
    private static const Int32 MO_MSG_QUEUE_LIMIT = 5;

    /**
     * Message reference for a CONCATENATED_8_BIT_REFERENCE or
     * CONCATENATED_16_BIT_REFERENCE message set.  Should be
     * incremented for each set of concatenated messages.
     * Static field shared by all dispatcher objects.
     */
    private static Int32 sConcatenatedRef = new Random()->NextInt(256);

    /** Outgoing message counter. Shared by all dispatchers. */
    private SmsUsageMonitor mUsageMonitor;

    protected ImsSMSDispatcher mImsSMSDispatcher;

    /** Number of outgoing SmsTrackers waiting for user confirmation. */
    private Int32 mPendingTrackerCount;

    /* Flags indicating whether the current device allows sms service */
    protected Boolean mSmsCapable = TRUE;
    protected Boolean mSmsSendDisabled;
    private Boolean mSmsPseudoMultipart;

    protected static Int32 GetNextConcatenatedRef() {
        sConcatenatedRef += 1;
        return sConcatenatedRef;
    }

    /**
     * Create a new SMS dispatcher.
     * @param phone the Phone to use
     * @param usageMonitor the SmsUsageMonitor to use
     */
    protected SMSDispatcher(PhoneBase phone, SmsUsageMonitor usageMonitor,
            ImsSMSDispatcher imsSMSDispatcher) {
        mPhone = phone;
        mImsSMSDispatcher = imsSMSDispatcher;
        mContext = phone->GetContext();
        mResolver = mContext->GetContentResolver();
        mCi = phone.mCi;
        mUsageMonitor = usageMonitor;
        mTelephonyManager = (TelephonyManager) mContext->GetSystemService(Context.TELEPHONY_SERVICE);
        mSettingsObserver = new SettingsObserver(this, mPremiumSmsRule, mContext);
        mContext->GetContentResolver()->RegisterContentObserver(Settings::Global::>GetUriFor(
                Settings::Global::SMS_SHORT_CODE_RULE), FALSE, mSettingsObserver);

        mSmsCapable = mContext->GetResources()->GetBoolean(
                R.bool.config_sms_capable);
        mSmsSendDisabled = !SystemProperties->GetBoolean(
                                TelephonyProperties.PROPERTY_SMS_SEND, mSmsCapable);
        mSmsPseudoMultipart = SystemProperties->GetBoolean("telephony.sms.pseudo_multipart", FALSE);
        Rlog->D(TAG, "SMSDispatcher: ctor mSmsCapable=" + mSmsCapable + " format=" + GetFormat()
                + " mSmsSendDisabled=" + mSmsSendDisabled);
    }

    /**
     * Observe the secure setting for updated premium sms determination rules
     */
    private static class SettingsObserver extends ContentObserver {
        private final AtomicInteger mPremiumSmsRule;
        private final Context mContext;
        SettingsObserver(Handler handler, AtomicInteger premiumSmsRule, Context context) {
            Super(handler);
            mPremiumSmsRule = premiumSmsRule;
            mContext = context;
            OnChange(FALSE); // load initial value;
        }

        //@Override
        CARAPI OnChange(Boolean selfChange) {
            mPremiumSmsRule->Set(Settings::Global::>GetInt(mContext->GetContentResolver(),
                    Settings::Global::SMS_SHORT_CODE_RULE, PREMIUM_RULE_USE_SIM));
        }
    }

    protected void UpdatePhoneObject(PhoneBase phone) {
        mPhone = phone;
        mUsageMonitor = phone.mSmsUsageMonitor;
        Rlog->D(TAG, "Active phone changed to " + mPhone->GetPhoneName() );
    }

    /** Unregister for incoming SMS events. */
    CARAPI Dispose() {
        mContext->GetContentResolver()->UnregisterContentObserver(mSettingsObserver);
    }

    /**
     * The format of the message PDU in the associated broadcast intent.
     * This will be either "3gpp" for GSM/UMTS/LTE messages in 3GPP format
     * or "3gpp2" for CDMA/LTE messages in 3GPP2 format.
     *
     * Note: All applications which handle incoming SMS messages by processing the
     * SMS_RECEIVED_ACTION broadcast intent MUST pass the "format" extra from the intent
     * into the new methods in {@link android.telephony.SmsMessage} which take an
     * extra format parameter. This is required in order to correctly decode the PDU on
     * devices which require support for both 3GPP and 3GPP2 formats at the same time,
     * such as CDMA/LTE devices and GSM/CDMA world phones.
     *
     * @return the format of the message PDU
     */
    protected abstract String GetFormat();

    /**
     * Pass the Message object to subclass to handle. Currently used to pass CDMA status reports
     * from {@link com.android.internal.telephony.cdma.CdmaInboundSmsHandler}.
     * @param o the SmsMessage containing the status report
     */
    protected void HandleStatusReport(Object o) {
        Rlog->D(TAG, "HandleStatusReport() called with no subclass.");
    }

    /* TODO: Need to figure out how to keep track of status report routing in a
     *       persistent manner. If the phone process Restarts (reboot or crash),
     *       we will lose this list and any status reports that come in after
     *       will be dropped.
     */
    /** Sent messages awaiting a delivery status report. */
    protected final ArrayList<SmsTracker> deliveryPendingList = new ArrayList<SmsTracker>();

    /** Outgoing messages being handled by the carrier app. */
    protected final List<SmsTracker> sendPendingList =
        Collections->SynchronizedList(new ArrayList<SmsTracker>());

    /**
     * Handles events coming from the phone stack. Overridden from handler.
     *
     * @param msg the message to handle
     */
    //@Override
    CARAPI HandleMessage(Message msg) {
        Switch (msg.what) {
        case EVENT_SEND_SMS_COMPLETE:
            // An outbound SMS has been successfully transferred, or failed.
            HandleSendComplete((AsyncResult) msg.obj);
            break;

        case EVENT_SEND_RETRY:
            Rlog->D(TAG, "SMS retry..");
            SendRetrySms((SmsTracker) msg.obj);
            break;

        case EVENT_SEND_LIMIT_REACHED_CONFIRMATION:
            HandleReachSentLimit((SmsTracker)(msg.obj));
            break;

        case EVENT_CONFIRM_SEND_TO_POSSIBLE_PREMIUM_SHORT_CODE:
            HandleConfirmShortCode(FALSE, (SmsTracker)(msg.obj));
            break;

        case EVENT_CONFIRM_SEND_TO_PREMIUM_SHORT_CODE:
            HandleConfirmShortCode(TRUE, (SmsTracker)(msg.obj));
            break;

        case EVENT_SEND_CONFIRMED_SMS:
        {
            SmsTracker tracker = (SmsTracker) msg.obj;
            If (tracker->IsMultipart()) {
                SendMultipartSms(tracker);
            } else {
                If (mPendingTrackerCount > 1) {
                    tracker.mExpectMore = TRUE;
                } else {
                    tracker.mExpectMore = FALSE;
                }
                SendSms(tracker);
            }
            mPendingTrackerCount--;
            break;
        }

        case EVENT_STOP_SENDING:
        {
            SmsTracker tracker = (SmsTracker) msg.obj;
            tracker->OnFailed(mContext, RESULT_ERROR_LIMIT_EXCEEDED, 0/*errorCode*/);
            mPendingTrackerCount--;
            break;
        }

        case EVENT_HANDLE_STATUS_REPORT:
            HandleStatusReport(msg.obj);
            break;

        default:
            Rlog->E(TAG, "HandleMessage() ignoring message of unexpected type " + msg.what);
        }
    }

    private static Boolean IsSystemUid(Context context, String pkgName) {
        final PackageManager packageManager = context->GetPackageManager();
        try {
            return packageManager->GetPackageInfo(pkgName, 0)
                .applicationInfo.uid == Process.SYSTEM_UID;
        } Catch (PackageManager.NameNotFoundException e) {
            return FALSE;
        }
    }

    /**
     * Called when SMS send completes. Broadcasts a sentIntent on success.
     * On failure, either sets up retries or broadcasts a sentIntent with
     * the failure in the result code.
     *
     * @param ar AsyncResult passed into the message handler.  ar.result should
     *           an SmsResponse instance if send was successful.  ar.userObj
     *           should be an SmsTracker instance.
     */
    protected void HandleSendComplete(AsyncResult ar) {
        SmsTracker tracker = (SmsTracker) ar.userObj;
        PendingIntent sentIntent = tracker.mSentIntent;

        If (ar.result != NULL) {
            tracker.mMessageRef = ((SmsResponse)ar.result).mMessageRef;
        } else {
            Rlog->D(TAG, "SmsResponse was NULL");
        }

        If (ar.exception == NULL) {
            If (DBG) Rlog->D(TAG, "SMS send complete. Broadcasting intent: " + sentIntent);

            If (tracker.mDeliveryIntent != NULL) {
                // Expecting a status report.  Add it to the list.
                deliveryPendingList->Add(tracker);
            }
            tracker->OnSent(mContext);
        } else {
            If (DBG) Rlog->D(TAG, "SMS send failed");

            Int32 ss = mPhone->GetServiceState()->GetState();

            If ( tracker.mImsRetry > 0 && ss != ServiceState.STATE_IN_SERVICE) {
                // This is retry after failure over IMS but voice is not available.
                // Set retry to max allowed, so no retry is sent and
                //   cause RESULT_ERROR_GENERIC_FAILURE to be returned to app.
                tracker.mRetryCount = MAX_SEND_RETRIES;

                Rlog->D(TAG, "handleSendComplete: Skipping retry: "
                +" IsIms()="+IsIms()
                +" mRetryCount="+tracker.mRetryCount
                +" mImsRetry="+tracker.mImsRetry
                +" mMessageRef="+tracker.mMessageRef
                +" SS= "+mPhone->GetServiceState()->GetState());
            }

            // if sms over IMS is not supported on data and voice is not available...
            If (!IsIms() && ss != ServiceState.STATE_IN_SERVICE) {
                tracker->OnFailed(mContext, GetNotInServiceError(ss), 0/*errorCode*/);
            } else If ((((CommandException)(ar.exception)).GetCommandError()
                    == CommandException.Error.SMS_FAIL_RETRY) &&
                   tracker.mRetryCount < MAX_SEND_RETRIES) {
                // Retry after a delay if needed.
                // TODO: According to TS 23.040, 9.2.3.6, we should resend
                //       with the same TP-MR as the failed message, and
                //       TP-RD set to 1.  However, we don't have a means of
                //       knowing the MR for the failed Message (EF_SMSstatus
                //       may or may not have the MR corresponding to this
                //       message, depending on the failure).  Also, in some
                //       implementations this retry is handled by the baseband.
                tracker.mRetryCount++;
                Message retryMsg = ObtainMessage(EVENT_SEND_RETRY, tracker);
                SendMessageDelayed(retryMsg, SEND_RETRY_DELAY);
            } else {
                Int32 errorCode = 0;
                If (ar.result != NULL) {
                    errorCode = ((SmsResponse)ar.result).mErrorCode;
                }
                Int32 error = RESULT_ERROR_GENERIC_FAILURE;
                If (((CommandException)(ar.exception)).GetCommandError()
                        == CommandException.Error.FDN_CHECK_FAILURE) {
                    error = RESULT_ERROR_FDN_CHECK_FAILURE;
                }
                tracker->OnFailed(mContext, error, errorCode);
            }
        }
    }

    /**
     * Handles outbound message when the phone is not in service.
     *
     * @param ss     Current service state.  Valid values are:
     *                  OUT_OF_SERVICE
     *                  EMERGENCY_ONLY
     *                  POWER_OFF
     * @param sentIntent the PendingIntent to send the error to
     */
    protected static void HandleNotInService(Int32 ss, PendingIntent sentIntent) {
        If (sentIntent != NULL) {
            try {
                If (ss == ServiceState.STATE_POWER_OFF) {
                    sentIntent->Send(RESULT_ERROR_RADIO_OFF);
                } else {
                    sentIntent->Send(RESULT_ERROR_NO_SERVICE);
                }
            } Catch (CanceledException ex) {}
        }
    }

    /**
     * @param ss service state
     * @return The result error based on input service state for not in service error
     */
    protected static Int32 GetNotInServiceError(Int32 ss) {
        If (ss == ServiceState.STATE_POWER_OFF) {
            return RESULT_ERROR_RADIO_OFF;
        }
        return RESULT_ERROR_NO_SERVICE;
    }

    /**
     * Send a data based SMS to a specific application port.
     *
     * @param destAddr the address to send the message to
     * @param scAddr is the service center address or NULL to use
     *  the current default SMSC
     * @param destPort the port to deliver the message to
     * @param origPort the port set by the sender
     * @param data the body of the message to send
     * @param sentIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is successfully sent, or failed.
     *  The result code will be <code>Activity.RESULT_OK<code> for success,
     *  or one of these errors:<br>
     *  <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
     *  <code>RESULT_ERROR_RADIO_OFF</code><br>
     *  <code>RESULT_ERROR_NULL_PDU</code><br>
     *  <code>RESULT_ERROR_NO_SERVICE</code><br>.
     *  For <code>RESULT_ERROR_GENERIC_FAILURE</code> the sentIntent may include
     *  the extra "errorCode" containing a radio technology specific value,
     *  generally only useful for troubleshooting.<br>
     *  The per-application based SMS control checks sentIntent. If sentIntent
     *  is NULL the caller will be checked against all unknown applications,
     *  which cause smaller number of SMS to be sent in checking period.
     * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is delivered to the recipient.  The
     *  raw pdu of the status report is in the extended Data ("pdu").
     */
    protected abstract void SendData(String destAddr, String scAddr, Int32 destPort, Int32 origPort,
            Byte[] data, PendingIntent sentIntent, PendingIntent deliveryIntent);

    /**
     * Send a text based SMS.
     *  @param destAddr the address to send the message to
     * @param scAddr is the service center address or NULL to use
     *  the current default SMSC
     * @param text the body of the message to send
     * @param sentIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is successfully sent, or failed.
     *  The result code will be <code>Activity.RESULT_OK<code> for success,
     *  or one of these errors:<br>
     *  <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
     *  <code>RESULT_ERROR_RADIO_OFF</code><br>
     *  <code>RESULT_ERROR_NULL_PDU</code><br>
     *  <code>RESULT_ERROR_NO_SERVICE</code><br>.
     *  For <code>RESULT_ERROR_GENERIC_FAILURE</code> the sentIntent may include
     *  the extra "errorCode" containing a radio technology specific value,
     *  generally only useful for troubleshooting.<br>
     *  The per-application based SMS control checks sentIntent. If sentIntent
     *  is NULL the caller will be checked against all unknown applications,
     *  which cause smaller number of SMS to be sent in checking period.
     * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is delivered to the recipient.  The
     * @param messageUri optional URI of the message if it is already stored in the system
     * @param callingPkg the calling package name
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
     */
    protected abstract void SendText(String destAddr, String scAddr, String text,
            PendingIntent sentIntent, PendingIntent deliveryIntent, Uri messageUri,
            String callingPkg, Int32 priority, Boolean isExpectMore, Int32 validityPeriod);

    /**
     * Inject an SMS PDU into the android platform.
     *
     * @param pdu is the Byte array of pdu to be injected into android telephony layer
     * @param format is the format of SMS Pdu (3gpp or 3gpp2)
     * @param receivedIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is successfully received by the
     *  android telephony layer. This intent is broadcasted at
     *  the same time an SMS received from radio is responded back.
     */
    protected abstract void InjectSmsPdu(Byte[] pdu, String format, PendingIntent receivedIntent);

    /**
     * Calculate the number of septets needed to encode the message.
     *
     * @param messageBody the message to encode
     * @param use7bitOnly Ignore (but still count) illegal characters if TRUE
     * @return TextEncodingDetails
     */
    protected abstract TextEncodingDetails CalculateLength(CharSequence messageBody,
            Boolean use7bitOnly);

    /**
     * Update the status of a Pending (send-by-IP) SMS message and resend by PSTN if necessary.
     * This outbound message was handled by the carrier app. If the carrier app fails to send
     * this message, it would be resent by PSTN.
     *
     * @param messageRef the reference number of the SMS message.
     * @param success True if and only if the message was sent successfully. If its value is
     *  FALSE, this message should be resent via PSTN.
     */
    protected abstract void UpdateSmsSendStatus(Int32 messageRef, Boolean success);

    /**
     * Handler for a {@link GsmSMSDispatcher} or {@link CdmaSMSDispatcher} broadcast.
     * If SMS sending is successfuly, sends EVENT_SEND_SMS_COMPLETE message. Otherwise,
     * send the message via the GSM/CDMA network.
     */
    protected final class SMSDispatcherReceiver extends BroadcastReceiver {

        private final SmsTracker mTracker;

        public SMSDispatcherReceiver(SmsTracker tracker) {
            mTracker = tracker;
        }

        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            If (action->Equals(Intents.SMS_SEND_ACTION)) {
                Int32 rc = GetResultCode();
                If (rc == Activity.RESULT_OK) {
                    Rlog->D(TAG, "Sending SMS by IP pending.");
                    Bundle resultExtras = GetResultExtras(FALSE);
                    If (resultExtras != NULL && resultExtras->ContainsKey("messageref")) {
                        mTracker.mMessageRef = resultExtras->GetInt("messageref");
                        Rlog->D(TAG, "messageref = " + mTracker.mMessageRef);
                    } else {
                        Rlog->E(TAG, "Can't find messageref in result extras.");
                    }
                    sendPendingList->Add(mTracker);
                } else {
                    Rlog->D(TAG, "Sending SMS by IP failed.");
                    SendSmsByPstn(mTracker);
                }
            } else {
                Rlog->E(TAG, "unexpected BroadcastReceiver action: " + action);
            }
        }
    }

    /**
     * Send a multi-part text based SMS.
     *  @param destAddr the address to send the message to
     * @param scAddr is the service center address or NULL to use
     *   the current default SMSC
     * @param parts an <code>ArrayList</code> of strings that, in order,
     *   comprise the original message
     * @param sentIntents if not NULL, an <code>ArrayList</code> of
     *   <code>PendingIntent</code>S (one for each message part) that is
     *   broadcast when the corresponding message part has been sent.
     *   The result code will be <code>Activity.RESULT_OK<code> for success,
     *   or one of these errors:
     *   <code>RESULT_ERROR_GENERIC_FAILURE</code>
     *   <code>RESULT_ERROR_RADIO_OFF</code>
     *   <code>RESULT_ERROR_NULL_PDU</code>
     *   <code>RESULT_ERROR_NO_SERVICE</code>.
     *  The per-application based SMS control checks sentIntent. If sentIntent
     *  is NULL the caller will be checked against all unknown applications,
     *  which cause smaller number of SMS to be sent in checking period.
     * @param deliveryIntents if not NULL, an <code>ArrayList</code> of
     *   <code>PendingIntent</code>S (one for each message part) that is
     *   broadcast when the corresponding message part has been delivered
     *   to the recipient.  The raw pdu of the status report is in the
     * @param messageUri optional URI of the message if it is already stored in the system
     * @param callingPkg the calling package name
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
     */
    protected void SendMultipartText(String destAddr, String scAddr,
            ArrayList<String> parts, ArrayList<PendingIntent> sentIntents,
            ArrayList<PendingIntent> deliveryIntents, Uri messageUri, String callingPkg,
            Int32 priority, Boolean isExpectMore, Int32 validityPeriod) {

        If (mSmsPseudoMultipart) {
            // Send as individual messages as the combination of device and
            // carrier behavior may not process concatenated messages correctly.
            SendPseudoMultipartText(destAddr, scAddr, parts, sentIntents, deliveryIntents,
                    messageUri, callingPkg, priority, isExpectMore, validityPeriod);
            return;
        }

        If (messageUri == NULL) {
            If (SmsApplication->ShouldWriteMessageForPackage(callingPkg, mContext)) {
                messageUri = WriteOutboxMessage(
                        GetSubId(),
                        destAddr,
                        GetMultipartMessageText(parts),
                        deliveryIntents != NULL && deliveryIntents->Size() > 0,
                        callingPkg);
            }
        } else {
            MoveToOutbox(GetSubId(), messageUri, callingPkg);
        }
        Int32 refNumber = GetNextConcatenatedRef() & 0x00FF;
        Int32 msgCount = parts->Size();
        Int32 encoding = SmsConstants.ENCODING_UNKNOWN;

        TextEncodingDetails[] encodingForParts = new TextEncodingDetails[msgCount];
        For (Int32 i = 0; i < msgCount; i++) {
            TextEncodingDetails details = CalculateLength(parts->Get(i), FALSE);
            If (encoding != details.codeUnitSize
                    && (encoding == SmsConstants.ENCODING_UNKNOWN
                            || encoding == SmsConstants.ENCODING_7BIT)) {
                encoding = details.codeUnitSize;
            }
            encodingForParts[i] = details;
        }

        // States to track at the message Level (for all parts)
        final AtomicInteger unsentPartCount = new AtomicInteger(msgCount);
        final AtomicBoolean anyPartFailed = new AtomicBoolean(FALSE);

        For (Int32 i = 0; i < msgCount; i++) {
            SmsHeader.ConcatRef concatRef = new SmsHeader->ConcatRef();
            concatRef.refNumber = refNumber;
            concatRef.seqNumber = i + 1;  // 1-based sequence
            concatRef.msgCount = msgCount;
            // TODO: We currently set this to TRUE since our messaging app will never
            // send more than 255 Parts (it converts the message to MMS well before that).
            // However, we should support 3rd party messaging apps that might need 16-bit
            // references
            // Note:  It's not sufficient to just flip this bit to TRUE; it will have
            // ripple Effects (several calculations assume 8-bit ref).
            concatRef.isEightBits = TRUE;
            SmsHeader smsHeader = new SmsHeader();
            smsHeader.concatRef = concatRef;

            // Set the national language tables for 3GPP 7-bit encoding, if enabled.
            If (encoding == SmsConstants.ENCODING_7BIT) {
                smsHeader.languageTable = encodingForParts[i].languageTable;
                smsHeader.languageShiftTable = encodingForParts[i].languageShiftTable;
            }

            PendingIntent sentIntent = NULL;
            If (sentIntents != NULL && sentIntents->Size() > i) {
                sentIntent = sentIntents->Get(i);
            }

            PendingIntent deliveryIntent = NULL;
            If (deliveryIntents != NULL && deliveryIntents->Size() > i) {
                deliveryIntent = deliveryIntents->Get(i);
            }

            SendNewSubmitPdu(destAddr, scAddr, parts->Get(i), smsHeader, encoding,
                    sentIntent, deliveryIntent, (i == (msgCount - 1)), priority, isExpectMore,
                    validityPeriod, unsentPartCount, anyPartFailed, messageUri);
        }
    }

    /**
     * Send a multi-part text based SMS as individual messages
     * (i.e., without User Data Headers).
     *
     * @param destAddr the address to send the message to
     * @param scAddr is the service center address or NULL to use
     * the current default SMSC
     * @param parts an <code>ArrayList</code> of strings that, in order,
     * comprise the original message
     * @param sentIntents if not NULL, an <code>ArrayList</code> of
     * <code>PendingIntent</code>S (one for each message part) that is
     * broadcast when the corresponding message part has been sent.
     * The result code will be <code>Activity.RESULT_OK<code> for success,
     * or one of these errors:
     * <code>RESULT_ERROR_GENERIC_FAILURE</code>
     * <code>RESULT_ERROR_RADIO_OFF</code>
     * <code>RESULT_ERROR_NULL_PDU</code>
     * <code>RESULT_ERROR_NO_SERVICE</code>.
     * The per-application based SMS control checks sentIntent. If sentIntent
     * is NULL the caller will be checked against all unknown applications,
     * which cause smaller number of SMS to be sent in checking period.
     * @param deliveryIntents if not NULL, an <code>ArrayList</code> of
     * <code>PendingIntent</code>S (one for each message part) that is
     * broadcast when the corresponding message part has been delivered
     * to the recipient. The raw pdu of the status report is in the
     * extended Data ("pdu").
     * @param messageUri optional URI of the message if it is already stored in the system
     * @param callingPkg the calling package name
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
     */
    private void SendPseudoMultipartText(String destAddr, String scAddr,
            ArrayList<String> parts, ArrayList<PendingIntent> sentIntents,
            ArrayList<PendingIntent> deliveryIntents,
            Uri messageUri, String callingPkg,
            Int32 priority, Boolean isExpectMore, Int32 validityPeriod) {
        Int32 msgCount = parts->Size();
        For (Int32 i = 0; i < msgCount; i++) {
            PendingIntent sentIntent = NULL;
            If (sentIntents != NULL && sentIntents->Size() > i) {
                sentIntent = sentIntents->Get(i);
            }
            PendingIntent deliveryIntent = NULL;
            If (deliveryIntents != NULL && deliveryIntents->Size() > i) {
                deliveryIntent = deliveryIntents->Get(i);
            }
            SendText(destAddr, scAddr, parts->Get(i), sentIntent, deliveryIntent,
                     messageUri, callingPkg, priority, isExpectMore, validityPeriod);
        }
    }

    /**
     * Create a new SubmitPdu and send it.
     */
    protected abstract void SendNewSubmitPdu(String destinationAddress, String scAddress,
            String message, SmsHeader smsHeader, Int32 encoding,
            PendingIntent sentIntent, PendingIntent deliveryIntent, Boolean lastPart, Int32 priority,
            Boolean isExpectMore, Int32 validityPeriod, AtomicInteger unsentPartCount,
            AtomicBoolean anyPartFailed, Uri messageUri);

    /**
     * Send a SMS
     * @param tracker will contain:
     * -smsc the SMSC to send the message through, or NULL for the
     *  default SMSC
     * -pdu the raw PDU to send
     * -sentIntent if not NULL this <code>Intent</code> is
     *  broadcast when the message is successfully sent, or failed.
     *  The result code will be <code>Activity.RESULT_OK<code> for success,
     *  or one of these errors:
     *  <code>RESULT_ERROR_GENERIC_FAILURE</code>
     *  <code>RESULT_ERROR_RADIO_OFF</code>
     *  <code>RESULT_ERROR_NULL_PDU</code>
     *  <code>RESULT_ERROR_NO_SERVICE</code>.
     *  The per-application based SMS control checks sentIntent. If sentIntent
     *  is NULL the caller will be checked against all unknown applications,
     *  which cause smaller number of SMS to be sent in checking period.
     * -deliveryIntent if not NULL this <code>Intent</code> is
     *  broadcast when the message is delivered to the recipient.  The
     *  raw pdu of the status report is in the extended Data ("pdu").
     * -param destAddr the destination phone Number (for short code confirmation)
     */
    protected void SendRawPdu(SmsTracker tracker) {
        HashMap map = tracker.mData;
        Byte pdu[] = (Byte[]) map->Get("pdu");

        If (mSmsSendDisabled) {
            Rlog->E(TAG, "Device does not support sending sms.");
            tracker->OnFailed(mContext, RESULT_ERROR_NO_SERVICE, 0/*errorCode*/);
            return;
        }

        If (pdu == NULL) {
            Rlog->E(TAG, "Empty PDU");
            tracker->OnFailed(mContext, RESULT_ERROR_NULL_PDU, 0/*errorCode*/);
            return;
        }
        
        PendingIntent sentIntent = tracker.mSentIntent;
        // Get calling app package name via UID from Binder call
        PackageManager pm = mContext->GetPackageManager();
        Int32 callingUid = Binder->GetCallingUid();
        // Special case: We're being proxied by the telephony stack itself,
        // so use the intent generator's UID if one exists
        String[] packageNames;

        If (callingUid == android.os.Process.PHONE_UID && sentIntent != NULL &&
                sentIntent->GetCreatorPackage() != NULL) {
            packageNames = new String[] { sentIntent->GetCreatorPackage() };
        } else {
            packageNames = pm->GetPackagesForUid(callingUid);
        }

        If (packageNames == NULL || packageNames.length == 0) {
            // Refuse to send SMS if we can't get the calling package name.
            Rlog->E(TAG, "Can't get calling app package name: refusing to send SMS");
            tracker->OnFailed(mContext, RESULT_ERROR_GENERIC_FAILURE, 0/*errorCode*/);
            return;
        }

        // Get package info via packagemanager
        PackageInfo appInfo;
        try {
            // XXX this is lossy- apps can share a UID
            appInfo = pm->GetPackageInfo(packageNames[0], PackageManager.GET_SIGNATURES);
        } Catch (PackageManager.NameNotFoundException e) {
            Rlog->E(TAG, "Can't get calling app package info: refusing to send SMS");
            tracker->OnFailed(mContext, RESULT_ERROR_GENERIC_FAILURE, 0/*errorCode*/);
            return;
        }

        // CheckDestination() returns TRUE if the destination is not a premium short code or the
        // sending app is approved to send to short codes. Otherwise, a message is sent to our
        // handler with the SmsTracker to request user confirmation before sending.
        If (CheckDestination(tracker)) {
            // check for excessive outgoing SMS usage by this app
            If (!mUsageMonitor->Check(appInfo.packageName, SINGLE_PART_SMS)) {
                SendMessage(ObtainMessage(EVENT_SEND_LIMIT_REACHED_CONFIRMATION, tracker));
                return;
            }

            SendSms(tracker);
        }
    }

    /**
     * Check if destination is a potential premium short code and sender is not pre-approved to
     * send to short codes.
     *
     * @param tracker the tracker for the SMS to send
     * @return TRUE if the destination is approved; FALSE if user confirmation event was sent
     */
    Boolean CheckDestination(SmsTracker tracker) {
        List<String> ignorePackages = Arrays->AsList(
                mContext->GetResources()->GetStringArray(R.array.config_ignored_sms_packages));

        String packageName = ResolvePackageName(tracker);

        If (ignorePackages->Contains(packageName)) {
            return TRUE;
        }

        If (mContext->CheckCallingOrSelfPermission(SEND_SMS_NO_CONFIRMATION_PERMISSION)
                == PackageManager.PERMISSION_GRANTED) {
            return TRUE;            // app is pre-approved to send to short codes
        } else {
            Int32 rule = mPremiumSmsRule->Get();
            Int32 smsCategory = SmsUsageMonitor.CATEGORY_NOT_SHORT_CODE;
            If (rule == PREMIUM_RULE_USE_SIM || rule == PREMIUM_RULE_USE_BOTH) {
                String simCountryIso = mTelephonyManager->GetSimCountryIso();
                If (simCountryIso == NULL || simCountryIso->Length() != 2) {
                    Rlog->E(TAG, "Can't get SIM country Iso: trying network country Iso");
                    simCountryIso = mTelephonyManager->GetNetworkCountryIso();
                }

                smsCategory = mUsageMonitor->CheckDestination(tracker.mDestAddress, simCountryIso);
            }
            If (rule == PREMIUM_RULE_USE_NETWORK || rule == PREMIUM_RULE_USE_BOTH) {
                String networkCountryIso = mTelephonyManager->GetNetworkCountryIso();
                If (networkCountryIso == NULL || networkCountryIso->Length() != 2) {
                    Rlog->E(TAG, "Can't get Network country Iso: trying SIM country Iso");
                    networkCountryIso = mTelephonyManager->GetSimCountryIso();
                }

                smsCategory = SmsUsageMonitor->MergeShortCodeCategories(smsCategory,
                        mUsageMonitor->CheckDestination(tracker.mDestAddress, networkCountryIso));
            }

            If (smsCategory == SmsUsageMonitor.CATEGORY_NOT_SHORT_CODE
                    || smsCategory == SmsUsageMonitor.CATEGORY_FREE_SHORT_CODE
                    || smsCategory == SmsUsageMonitor.CATEGORY_STANDARD_SHORT_CODE) {
                return TRUE;    // not a premium short code
            }

            // Wait for user confirmation unless the user has set permission to always allow/deny
            Int32 premiumSmsPermission = mUsageMonitor->GetPremiumSmsPermission(
                    tracker.mAppInfo.packageName);
            If (premiumSmsPermission == SmsUsageMonitor.PREMIUM_SMS_PERMISSION_UNKNOWN) {
                // First time trying to send to premium SMS.
                premiumSmsPermission = SmsUsageMonitor.PREMIUM_SMS_PERMISSION_ASK_USER;
            }

            Switch (premiumSmsPermission) {
                case SmsUsageMonitor.PREMIUM_SMS_PERMISSION_ALWAYS_ALLOW:
                    Rlog->D(TAG, "User approved this app to send to premium SMS");
                    return TRUE;

                case SmsUsageMonitor.PREMIUM_SMS_PERMISSION_NEVER_ALLOW:
                    Rlog->W(TAG, "User denied this app from sending to premium SMS");
                    SendMessage(ObtainMessage(EVENT_STOP_SENDING, tracker));
                    return FALSE;   // reject this message

                case SmsUsageMonitor.PREMIUM_SMS_PERMISSION_ASK_USER:
                default:
                    Int32 event;
                    If (smsCategory == SmsUsageMonitor.CATEGORY_POSSIBLE_PREMIUM_SHORT_CODE) {
                        event = EVENT_CONFIRM_SEND_TO_POSSIBLE_PREMIUM_SHORT_CODE;
                    } else {
                        event = EVENT_CONFIRM_SEND_TO_PREMIUM_SHORT_CODE;
                    }
                    SendMessage(ObtainMessage(event, tracker));
                    return FALSE;   // wait for user confirmation
            }
        }
    }

    /**
     * Deny sending an SMS if the outgoing queue limit is reached. Used when the message
     * must be confirmed by the user due to excessive usage or potential premium SMS detected.
     * @param tracker the SmsTracker for the message to send
     * @return TRUE if the message was denied; FALSE to continue with send confirmation
     */
    private Boolean DenyIfQueueLimitReached(SmsTracker tracker) {
        If (mPendingTrackerCount >= MO_MSG_QUEUE_LIMIT) {
            // Deny sending message when the queue limit is reached.
            Rlog->E(TAG, "Denied because queue limit reached");
            tracker->OnFailed(mContext, RESULT_ERROR_LIMIT_EXCEEDED, 0/*errorCode*/);
            return TRUE;
        }
        mPendingTrackerCount++;
        return FALSE;
    }

    /**
     * Returns the label for the specified app package name.
     * @param appPackage the package name of the app requesting to send an SMS
     * @return the label for the specified app, or the package name if GetApplicationInfo() fails
     */
    private CharSequence GetAppLabel(String appPackage) {
        PackageManager pm = mContext->GetPackageManager();
        try {
            ApplicationInfo appInfo = pm->GetApplicationInfo(appPackage, 0);
            return appInfo->LoadLabel(pm);
        } Catch (PackageManager.NameNotFoundException e) {
            Rlog->E(TAG, "PackageManager Name Not Found for package " + appPackage);
            return appPackage;  // fall back to package name if we can't get app label
        }
    }

    /**
     * Returns the package name from the original creator of the sms, even
     * if the package is mapped with others in a specific UID (like System UID)
     *
     * @param tracker
     * @return the package name that created the original sms
     */
    private String ResolvePackageName(SmsTracker tracker) {
        PendingIntent sentIntent = tracker.mSentIntent;
        String packageName = tracker.mAppInfo.applicationInfo.packageName;
        // System UID maps to multiple packages. Try to narrow it
        // down to an actual sender if possible
        If (IsSystemUid(mContext, packageName) && sentIntent != NULL &&
                sentIntent->GetCreatorPackage() != NULL) {
            packageName = sentIntent->GetCreatorPackage();
        }
        return packageName;
    }

    /**
     * Post an alert when SMS needs confirmation due to excessive usage.
     * @param tracker an SmsTracker for the current message.
     */
    protected void HandleReachSentLimit(SmsTracker tracker) {
        If (DenyIfQueueLimitReached(tracker)) {
            return;     // queue limit reached; error was returned to caller
        }

        CharSequence appLabel = GetAppLabel(tracker.mAppInfo.packageName);
        Resources r = Resources->GetSystem();
        Spanned messageText = Html->FromHtml(r->GetString(R::string::sms_control_message, appLabel));

        ConfirmDialogListener listener = new ConfirmDialogListener(tracker, NULL);

        AlertDialog d = new AlertDialog->Builder(mContext)
                .SetTitle(R::string::sms_control_title)
                .SetIcon(R.drawable.stat_sys_warning)
                .SetMessage(messageText)
                .SetPositiveButton(r->GetString(R::string::sms_control_yes), listener)
                .SetNegativeButton(r->GetString(R::string::sms_control_no), listener)
                .SetOnCancelListener(listener)
                .Create();

        d->GetWindow()->SetType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
        d->Show();
    }

    /**
     * Post an alert for user confirmation when sending to a potential short code.
     * @param isPremium TRUE if the destination is known to be a premium short code
     * @param tracker the SmsTracker for the current message.
     */
    protected void HandleConfirmShortCode(Boolean isPremium, SmsTracker tracker) {
        If (DenyIfQueueLimitReached(tracker)) {
            return;     // queue limit reached; error was returned to caller
        }

        Int32 detailsId;
        If (isPremium) {
            detailsId = R::string::sms_premium_short_code_details;
        } else {
            detailsId = R::string::sms_short_code_details;
        }

        CharSequence appLabel = GetAppLabel(tracker.mAppInfo.packageName);
        Resources r = Resources->GetSystem();
        Spanned messageText = Html->FromHtml(r->GetString(R::string::sms_short_code_confirm_message,
                appLabel, tracker.mDestAddress));

        LayoutInflater inflater = (LayoutInflater) mContext->GetSystemService(
                Context.LAYOUT_INFLATER_SERVICE);
        View layout = inflater->Inflate(R.layout.sms_short_code_confirmation_dialog, NULL);

        ConfirmDialogListener listener = new ConfirmDialogListener(tracker,
                (TextView)layout->FindViewById(R.id.sms_short_code_remember_undo_instruction));


        TextView messageView = (TextView) layout->FindViewById(R.id.sms_short_code_confirm_message);
        messageView->SetText(messageText);

        ViewGroup detailsLayout = (ViewGroup) layout->FindViewById(
                R.id.sms_short_code_detail_layout);
        TextView detailsView = (TextView) detailsLayout->FindViewById(
                R.id.sms_short_code_detail_message);
        detailsView->SetText(detailsId);

        CheckBox rememberChoice = (CheckBox) layout->FindViewById(
                R.id.sms_short_code_remember_choice_checkbox);
        rememberChoice->SetOnCheckedChangeListener(listener);

        AlertDialog d = new AlertDialog->Builder(mContext)
                .SetView(layout)
                .SetPositiveButton(r->GetString(R::string::sms_short_code_confirm_allow), listener)
                .SetNegativeButton(r->GetString(R::string::sms_short_code_confirm_deny), listener)
                .SetOnCancelListener(listener)
                .Create();

        d->GetWindow()->SetType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
        d->Show();

        listener->SetPositiveButton(d->GetButton(DialogInterface.BUTTON_POSITIVE));
        listener->SetNegativeButton(d->GetButton(DialogInterface.BUTTON_NEGATIVE));
    }

    /**
     * Returns the premium SMS permission for the specified package. If the package has never
     * been seen before, the default {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_ASK_USER}
     * will be returned.
     * @param packageName the name of the package to query permission
     * @return one of {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_UNKNOWN},
     *  {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_ASK_USER},
     *  {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_NEVER_ALLOW}, or
     *  {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_ALWAYS_ALLOW}
     */
    public Int32 GetPremiumSmsPermission(String packageName) {
        return mUsageMonitor->GetPremiumSmsPermission(packageName);
    }

    /**
     * Sets the premium SMS permission for the specified package and save the value asynchronously
     * to persistent storage.
     * @param packageName the name of the package to set permission
     * @param permission one of {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_ASK_USER},
     *  {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_NEVER_ALLOW}, or
     *  {@link SmsUsageMonitor#PREMIUM_SMS_PERMISSION_ALWAYS_ALLOW}
     */
    CARAPI SetPremiumSmsPermission(String packageName, Int32 permission) {
        mUsageMonitor->SetPremiumSmsPermission(packageName, permission);
    }

    /**
     * Send the message along to the radio.
     *
     * @param tracker holds the SMS message to send
     */
    protected abstract void SendSms(SmsTracker tracker);

    /**
     * Send the SMS via the PSTN network.
     *
     * @param tracker holds the Sms tracker ready to be sent
     */
    protected abstract void SendSmsByPstn(SmsTracker tracker);

    /**
     * Retry the message along to the radio.
     *
     * @param tracker holds the SMS message to send
     */
    CARAPI SendRetrySms(SmsTracker tracker) {
        // re-routing to ImsSMSDispatcher
        If (mImsSMSDispatcher != NULL) {
            mImsSMSDispatcher->SendRetrySms(tracker);
        } else {
            Rlog->E(TAG, mImsSMSDispatcher + " is NULL. Retry failed");
        }
    }

    /**
     * Send the multi-part SMS based on multipart Sms tracker
     *
     * @param tracker holds the multipart Sms tracker ready to be sent
     */
    private void SendMultipartSms(SmsTracker tracker) {
        ArrayList<String> parts;
        ArrayList<PendingIntent> sentIntents;
        ArrayList<PendingIntent> deliveryIntents;

        HashMap<String, Object> map = tracker.mData;

        String destinationAddress = (String) map->Get("destination");
        String scAddress = (String) map->Get("scaddress");

        parts = (ArrayList<String>) map->Get("parts");
        sentIntents = (ArrayList<PendingIntent>) map->Get("sentIntents");
        deliveryIntents = (ArrayList<PendingIntent>) map->Get("deliveryIntents");

        // check if in service
        Int32 ss = mPhone->GetServiceState()->GetState();
        // if sms over IMS is not supported on data and voice is not available...
        If (!IsIms() && ss != ServiceState.STATE_IN_SERVICE) {
            For (Int32 i = 0, count = parts->Size(); i < count; i++) {
                PendingIntent sentIntent = NULL;
                If (sentIntents != NULL && sentIntents->Size() > i) {
                    sentIntent = sentIntents->Get(i);
                }
                HandleNotInService(ss, sentIntent);
            }
            return;
        }

        SendMultipartText(destinationAddress, scAddress, parts, sentIntents, deliveryIntents,
                NULL/*messageUri*/, NULL/*callingPkg*/, -1, tracker.mExpectMore,
                tracker.mvalidityPeriod);
    }

    /**
     * Keeps track of an SMS that has been sent to the RIL, until it has
     * successfully been sent, or we're done trying.
     */
    protected static const class SmsTracker {
        // fields need to be public for derived SmsDispatchers
        public final HashMap<String, Object> mData;
        public Int32 mRetryCount;
        public Int32 mImsRetry; // nonzero indicates initial message was sent over Ims
        public Int32 mMessageRef;
        public Boolean mExpectMore;
        public Int32 mvalidityPeriod;
        String mFormat;

        public final PendingIntent mSentIntent;
        public final PendingIntent mDeliveryIntent;

        public final PackageInfo mAppInfo;
        public final String mDestAddress;

        public final SmsHeader mSmsHeader;

        private Int64 mTimestamp = System->CurrentTimeMillis();
        public Uri mMessageUri; // Uri of persisted message if we wrote one

        // Reference to states of a multipart message that this part belongs to
        private AtomicInteger mUnsentPartCount;
        private AtomicBoolean mAnyPartFailed;

        private SmsTracker(HashMap<String, Object> data, PendingIntent sentIntent,
                PendingIntent deliveryIntent, PackageInfo appInfo, String destAddr, String format,
                AtomicInteger unsentPartCount, AtomicBoolean anyPartFailed, Uri messageUri,
                SmsHeader smsHeader, Boolean isExpectMore, Int32 validityPeriod) {
            mData = data;
            mSentIntent = sentIntent;
            mDeliveryIntent = deliveryIntent;
            mRetryCount = 0;
            mAppInfo = appInfo;
            mDestAddress = destAddr;
            mFormat = format;
            mExpectMore = isExpectMore;
            mImsRetry = 0;
            mMessageRef = 0;
            mvalidityPeriod = validityPeriod;
            mUnsentPartCount = unsentPartCount;
            mAnyPartFailed = anyPartFailed;
            mMessageUri = messageUri;
            mSmsHeader = smsHeader;
        }

        /**
         * Returns whether this tracker holds a multi-part SMS.
         * @return TRUE if the tracker holds a multi-part SMS; FALSE otherwise
         */
        Boolean IsMultipart() {
            return mData->ContainsKey("parts");
        }

        /**
         * Persist this as a sent message
         */
        void WriteSentMessage(Context context) {
            String text = (String)mData->Get("text");
            If (text != NULL) {
                Boolean deliveryReport = (mDeliveryIntent != NULL);
                // Using invalid threadId 0 here. When the message is inserted into the db, the
                // provider looks up the threadId based on the Recipient(s).
                mMessageUri = Sms->AddMessageToUri(context->GetContentResolver(),
                        Telephony.Sms.Sent.CONTENT_URI,
                        mDestAddress,
                        text /*body*/,
                        NULL /*subject*/,
                        mTimestamp /*date*/,
                        TRUE /*read*/,
                        deliveryReport /*deliveryReport*/,
                        0 /*threadId*/);
            }
        }

        /**
         * Update the status of this message if we persisted it
         */
        CARAPI UpdateSentMessageStatus(Context context, Int32 status) {
            If (mMessageUri != NULL) {
                // If we wrote this message in writeSentMessage, update it now
                ContentValues values = new ContentValues(1);
                values->Put(Sms.STATUS, status);
                SqliteWrapper->Update(context, context->GetContentResolver(),
                        mMessageUri, values, NULL, NULL);
            }
        }

        /**
         * Update the error_code column of a message
         *
         * @param context The Context
         * @param errorCode The error code
         */
        private void UpdateMessageErrorCode(Context context, Int32 errorCode) {
            If (mMessageUri == NULL) {
                return;
            }
            final ContentValues values = new ContentValues(1);
            values->Put(Sms.ERROR_CODE, errorCode);
            final Int64 identity = Binder->ClearCallingIdentity();
            try {
                If (SqliteWrapper->Update(context, context->GetContentResolver(), mMessageUri, values,
                        NULL/*where*/, NULL/*selectionArgs*/) != 1) {
                    Rlog->E(TAG, "Failed to update message error code");
                }
            } finally {
                Binder->RestoreCallingIdentity(identity);
            }
        }

        /**
         * Set the final state of a message: FAILED or SENT
         *
         * @param context The Context
         * @param messageType The final message type
         */
        private void SetMessageFinalState(Context context, Int32 messageType) {
            If (mMessageUri == NULL) {
                return;
            }
            final ContentValues values = new ContentValues(1);
            values->Put(Sms.TYPE, messageType);
            final Int64 identity = Binder->ClearCallingIdentity();
            try {
                If (SqliteWrapper->Update(context, context->GetContentResolver(), mMessageUri, values,
                        NULL/*where*/, NULL/*selectionArgs*/) != 1) {
                    Rlog->E(TAG, "Failed to move message to " + messageType);
                }
            } finally {
                Binder->RestoreCallingIdentity(identity);
            }
        }

        /**
         * Handle a failure of a single part message or a part of a multipart message
         *
         * @param context The Context
         * @param error The error to send back with
         * @param errorCode
         */
        CARAPI OnFailed(Context context, Int32 error, Int32 errorCode) {
            If (mAnyPartFailed != NULL) {
                mAnyPartFailed->Set(TRUE);
            }
            // is single part or last part of multipart message
            Boolean isSinglePartOrLastPart = TRUE;
            If (mUnsentPartCount != NULL) {
                isSinglePartOrLastPart = mUnsentPartCount->DecrementAndGet() == 0;
            }
            If (errorCode != 0) {
                UpdateMessageErrorCode(context, errorCode);
            }
            If (isSinglePartOrLastPart) {
                SetMessageFinalState(context, Sms.MESSAGE_TYPE_FAILED);
            }
            If (mSentIntent != NULL) {
                try {
                    // Extra information to send with the sent intent
                    Intent fillIn = new Intent();
                    If (mMessageUri != NULL) {
                        // Pass this to SMS apps so that they know where it is stored
                        fillIn->PutExtra("uri", mMessageUri->ToString());
                    }
                    If (errorCode != 0) {
                        fillIn->PutExtra("errorCode", errorCode);
                    }
                    If (mUnsentPartCount != NULL && isSinglePartOrLastPart) {
                        // Is multipart and last part
                        fillIn->PutExtra(SEND_NEXT_MSG_EXTRA, TRUE);
                    }
                    mSentIntent->Send(context, error, fillIn);
                } Catch (CanceledException ex) {
                    Rlog->E(TAG, "Failed to send result");
                }
            }
        }

        /**
         * Handle the sent of a single part message or a part of a multipart message
         *
         * @param context The Context
         */
        CARAPI OnSent(Context context) {
            // is single part or last part of multipart message
            Boolean isSinglePartOrLastPart = TRUE;
            If (mUnsentPartCount != NULL) {
                isSinglePartOrLastPart = mUnsentPartCount->DecrementAndGet() == 0;
            }
            If (isSinglePartOrLastPart) {
                Boolean success = TRUE;
                If (mAnyPartFailed != NULL && mAnyPartFailed->Get()) {
                    success = FALSE;
                }
                If (success) {
                    SetMessageFinalState(context, Sms.MESSAGE_TYPE_SENT);
                } else {
                    SetMessageFinalState(context, Sms.MESSAGE_TYPE_FAILED);
                }
            }
            If (mSentIntent != NULL) {
                try {
                    // Extra information to send with the sent intent
                    Intent fillIn = new Intent();
                    If (mMessageUri != NULL) {
                        // Pass this to SMS apps so that they know where it is stored
                        fillIn->PutExtra("uri", mMessageUri->ToString());
                    }
                    If (mUnsentPartCount != NULL && isSinglePartOrLastPart) {
                        // Is multipart and last part
                        fillIn->PutExtra(SEND_NEXT_MSG_EXTRA, TRUE);
                    }
                    mSentIntent->Send(context, Activity.RESULT_OK, fillIn);
                } Catch (CanceledException ex) {
                    Rlog->E(TAG, "Failed to send result");
                }
            }
        }
    }

    protected SmsTracker GetSmsTracker(HashMap<String, Object> data, PendingIntent sentIntent,
            PendingIntent deliveryIntent, String format, AtomicInteger unsentPartCount,
            AtomicBoolean anyPartFailed, Uri messageUri, SmsHeader smsHeader,
            Boolean isExpectMore, Int32 validityPeriod) {
        // Get calling app package name via UID from Binder call
        PackageManager pm = mContext->GetPackageManager();
        Int32 callingUid = Binder->GetCallingUid();
        // Special case: We're being proxied by the telephony stack itself,
        // so use the intent generator's UID if one exists
        String[] packageNames;

        If (callingUid == android.os.Process.PHONE_UID && sentIntent != NULL &&
                sentIntent->GetCreatorPackage() != NULL) {
            packageNames = new String[] { sentIntent->GetCreatorPackage() };
        } else {
            packageNames = pm->GetPackagesForUid(callingUid);
        }

        // Get package info via packagemanager
        PackageInfo appInfo = NULL;
        If (packageNames != NULL && packageNames.length > 0) {
            try {
                // XXX this is lossy- apps can share a UID
                appInfo = pm->GetPackageInfo(packageNames[0], PackageManager.GET_SIGNATURES);
            } Catch (PackageManager.NameNotFoundException e) {
                // error will be logged in sendRawPdu
            }
        }
        // Strip non-digits from destination phone number before checking for short codes
        // and before displaying the number to the user if confirmation is required.
        String destAddr = PhoneNumberUtils->ExtractNetworkPortion((String) data->Get("destAddr"));
        return new SmsTracker(data, sentIntent, deliveryIntent, appInfo, destAddr, format,
                unsentPartCount, anyPartFailed, messageUri, smsHeader, isExpectMore, validityPeriod);
    }

    protected SmsTracker GetSmsTracker(HashMap<String, Object> data, PendingIntent sentIntent,
            PendingIntent deliveryIntent, String format, Uri messageUri, Boolean isExpectMore) {
        return GetSmsTracker(data, sentIntent, deliveryIntent, format, NULL/*unsentPartCount*/,
                NULL/*anyPartFailed*/, messageUri, NULL/*smsHeader*/, isExpectMore, -1);
    }

    protected SmsTracker GetSmsTracker(HashMap<String, Object> data, PendingIntent sentIntent,
            PendingIntent deliveryIntent, String format, Uri messageUri, Boolean isExpectMore, 
            Int32 validityPeriod ) {
        return GetSmsTracker(data, sentIntent, deliveryIntent, format, NULL/*unsentPartCount*/, 
                NULL/*anyPartFailed*/, messageUri, NULL/*smsHeader*/, isExpectMore, validityPeriod);
    }

    protected HashMap<String, Object> GetSmsTrackerMap(String destAddr, String scAddr,
            String text, SmsMessageBase.SubmitPduBase pdu) {
        HashMap<String, Object> map = new HashMap<String, Object>();
        map->Put("destAddr", destAddr);
        map->Put("scAddr", scAddr);
        map->Put("text", text);
        map->Put("smsc", pdu.encodedScAddress);
        map->Put("pdu", pdu.encodedMessage);
        return map;
    }

    protected HashMap<String, Object> GetSmsTrackerMap(String destAddr, String scAddr,
            Int32 destPort, Int32 origPort, Byte[] data, SmsMessageBase.SubmitPduBase pdu) {
        HashMap<String, Object> map = new HashMap<String, Object>();
        map->Put("destAddr", destAddr);
        map->Put("scAddr", scAddr);
        map->Put("destPort", destPort);
        map->Put("origPort", origPort);
        map->Put("data", data);
        map->Put("smsc", pdu.encodedScAddress);
        map->Put("pdu", pdu.encodedMessage);
        return map;
    }

    /**
     * Dialog listener for SMS confirmation dialog.
     */
    private final class ConfirmDialogListener
            implements DialogInterface.OnClickListener, DialogInterface.OnCancelListener,
            CompoundButton.OnCheckedChangeListener {

        private final SmsTracker mTracker;
        private Button mPositiveButton;
        private Button mNegativeButton;
        private Boolean mRememberChoice;    // default is unchecked
        private final TextView mRememberUndoInstruction;

        ConfirmDialogListener(SmsTracker tracker, TextView textView) {
            mTracker = tracker;
            mRememberUndoInstruction = textView;
        }

        void SetPositiveButton(Button button) {
            mPositiveButton = button;
        }

        void SetNegativeButton(Button button) {
            mNegativeButton = button;
        }

        //@Override
        CARAPI OnClick(DialogInterface dialog, Int32 which) {
            // Always set the SMS permission so that Settings will show a permission setting
            // for the App (it won't be shown until after the app tries to send to a short code).
            Int32 newSmsPermission = SmsUsageMonitor.PREMIUM_SMS_PERMISSION_ASK_USER;

            If (which == DialogInterface.BUTTON_POSITIVE) {
                Rlog->D(TAG, "CONFIRM sending SMS");
                // XXX this is lossy- apps can have more than one signature
                EventLog->WriteEvent(EventLogTags.EXP_DET_SMS_SENT_BY_USER,
                                    mTracker.mAppInfo.applicationInfo == NULL ?
                                    -1 : mTracker.mAppInfo.applicationInfo.uid);
                SendMessage(ObtainMessage(EVENT_SEND_CONFIRMED_SMS, mTracker));
                If (mRememberChoice) {
                    newSmsPermission = SmsUsageMonitor.PREMIUM_SMS_PERMISSION_ALWAYS_ALLOW;
                }
            } else If (which == DialogInterface.BUTTON_NEGATIVE) {
                Rlog->D(TAG, "DENY sending SMS");
                // XXX this is lossy- apps can have more than one signature
                EventLog->WriteEvent(EventLogTags.EXP_DET_SMS_DENIED_BY_USER,
                                    mTracker.mAppInfo.applicationInfo == NULL ?
                                    -1 :  mTracker.mAppInfo.applicationInfo.uid);
                SendMessage(ObtainMessage(EVENT_STOP_SENDING, mTracker));
                If (mRememberChoice) {
                    newSmsPermission = SmsUsageMonitor.PREMIUM_SMS_PERMISSION_NEVER_ALLOW;
                }
            }
            SetPremiumSmsPermission(mTracker.mAppInfo.packageName, newSmsPermission);
        }

        //@Override
        CARAPI OnCancel(DialogInterface dialog) {
            Rlog->D(TAG, "dialog dismissed: don't send SMS");
            SendMessage(ObtainMessage(EVENT_STOP_SENDING, mTracker));
        }

        //@Override
        CARAPI OnCheckedChanged(CompoundButton buttonView, Boolean isChecked) {
            Rlog->D(TAG, "remember this choice: " + isChecked);
            mRememberChoice = isChecked;
            If (isChecked) {
                mPositiveButton->SetText(R::string::sms_short_code_confirm_always_allow);
                mNegativeButton->SetText(R::string::sms_short_code_confirm_never_allow);
                If (mRememberUndoInstruction != NULL) {
                    mRememberUndoInstruction.
                            SetText(R::string::sms_short_code_remember_undo_instruction);
                    mRememberUndoInstruction->SetPadding(0,0,0,32);
                }
            } else {
                mPositiveButton->SetText(R::string::sms_short_code_confirm_allow);
                mNegativeButton->SetText(R::string::sms_short_code_confirm_deny);
                If (mRememberUndoInstruction != NULL) {
                    mRememberUndoInstruction->SetText("");
                    mRememberUndoInstruction->SetPadding(0,0,0,0);
                }
            }
        }
    }

    public Boolean IsIms() {
        If (mImsSMSDispatcher != NULL) {
            return mImsSMSDispatcher->IsIms();
        } else {
            Rlog->E(TAG, mImsSMSDispatcher + " is NULL");
            return FALSE;
        }
    }

    public String GetImsSmsFormat() {
        If (mImsSMSDispatcher != NULL) {
            return mImsSMSDispatcher->GetImsSmsFormat();
        } else {
            Rlog->E(TAG, mImsSMSDispatcher + " is NULL");
            return NULL;
        }
    }

    protected Uri WriteOutboxMessage(Int64 subId, String address, String text,
            Boolean requireDeliveryReport, String creator) {
        final ContentValues values = new ContentValues(8);
        Int32 phoneId = SubscriptionManager->GetPhoneId(subId);
        values->Put(Telephony.Sms.PHONE_ID, phoneId);
        values->Put(Telephony.Sms.ADDRESS, address);
        values->Put(Telephony.Sms.BODY, text);
        values->Put(Telephony.Sms.DATE, System->CurrentTimeMillis()); // milliseconds
        values->Put(Telephony.Sms.SEEN, 1);
        values->Put(Telephony.Sms.READ, 1);
        If (!TextUtils->IsEmpty(creator)) {
            values->Put(Telephony.Sms.CREATOR, creator);
        }
        If (requireDeliveryReport) {
            values->Put(Telephony.Sms.STATUS, Telephony.Sms.STATUS_PENDING);
        }
        final Int64 identity = Binder->ClearCallingIdentity();
        try {
            final Uri uri =  mContext->GetContentResolver()->Insert(
                    Telephony.Sms.Outbox.CONTENT_URI, values);
            return uri;
        } Catch (Exception e) {
            Rlog->E(TAG, "writeOutboxMessage: Failed to persist outbox message", e);
            return NULL;
        } finally {
            Binder->RestoreCallingIdentity(identity);
        }
    }

    protected void MoveToOutbox(Int64 subId, Uri messageUri, String creator) {
        final ContentValues values = new ContentValues(4);
        Int32 phoneId = SubscriptionManager->GetPhoneId(subId);
        values->Put(Telephony.Sms.PHONE_ID, phoneId);
        If (!TextUtils->IsEmpty(creator)) {
            // Reset creator/sender
            values->Put(Telephony.Sms.CREATOR, creator);
        }
        // Reset the timestamp
        values->Put(Telephony.Sms.DATE, System->CurrentTimeMillis()); // milliseconds
        values->Put(Telephony.Sms.TYPE, Telephony.Sms.MESSAGE_TYPE_OUTBOX);
        final Int64 identity = Binder->ClearCallingIdentity();
        try {
            If (mContext->GetContentResolver()->Update(messageUri, values,
                    NULL/*where*/, NULL/*selectionArgs*/) != 1) {
                Rlog->E(TAG, "moveToOutbox: failed to update message " + messageUri);
            }
        } Catch (Exception e) {
            Rlog->E(TAG, "moveToOutbox: Failed to update message", e);
        } finally {
            Binder->RestoreCallingIdentity(identity);
        }
    }

    private String GetMultipartMessageText(ArrayList<String> parts) {
        final StringBuilder sb = new StringBuilder();
        For (String part : parts) {
            If (part != NULL) {
                sb->Append(part);
            }
        }
        return sb->ToString();
    }

    protected String GetCarrierAppPackageName(Intent intent) {
        UiccCard card = UiccController->GetInstance()->GetUiccCard();
        If (card == NULL) {
            return NULL;
        }

        List<String> carrierPackages = card->GetCarrierPackageNamesForIntent(
            mContext->GetPackageManager(), intent);
        Return (carrierPackages != NULL && carrierPackages->Size() == 1) ?
                carrierPackages->Get(0) : NULL;

    }

    protected Int64 GetSubId() {
        return SubscriptionController->GetInstance()->GetSubIdUsingPhoneId(mPhone.mPhoneId);
    }
}
