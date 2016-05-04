/*
 * Copyright (C) 2013 The Android Open Source Project
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
using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreferenceManager;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ITelephony;
using Elastos::Droid::Provider::Telephony::Sms::IIntents;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::ISmsManager;
using Elastos::Droid::Telephony::ISmsMessage;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Internal::Telephony::Utility::IBlacklistUtils;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Utility::IHexDump;
using Elastos::Droid::Internal::Utility::IState;
using Elastos::Droid::Internal::Utility::IStateMachine;

using Elastos::IO::IByteArrayOutputStream;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;
using Elastos::Utility::ICollections;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

using static android::Telephony::TelephonyManager::IPHONE_TYPE_CDMA;

/**
 * This class broadcasts incoming SMS messages to interested apps after storing them in
 * the SmsProvider "raw" table and ACKing them to the SMSC. After each message has been
 * broadcast, its parts are removed from the raw table. If the device crashes after ACKing
 * but before the broadcast completes, the pending messages will be rebroadcast on the next boot.
 *
 * <p>The state machine starts in {@link IdleState} state. When the {@link SMSDispatcher} receives a
 * new SMS from the radio, it calls {@link #dispatchNormalMessage},
 * which sends a message to the state machine, causing the wakelock to be acquired in
 * {@link #haltedProcessMessage}, which transitions to {@link DeliveringState} state, where the message
 * is saved to the raw table, then acknowledged via the {@link SMSDispatcher} which called us.
 *
 * <p>After saving the SMS, if the message is Complete (either single-part or the final segment
 * of a multi-part SMS), we broadcast the completed PDUs as an ordered broadcast, then transition to
 * {@link WaitingState} state to wait for the broadcast to complete. When the local
 * {@link BroadcastReceiver} is called with the result, it sends {@link #EVENT_BROADCAST_COMPLETE}
 * to the state machine, causing us to either broadcast the next pending Message (if one has
 * arrived while waiting for the broadcast to complete), or to transition back to the halted state
 * after all messages are processed. Then the wakelock is released and we wait for the next SMS.
 */
public abstract class InboundSmsHandler extends StateMachine {
    protected static const Boolean DBG = TRUE;
    private static const Boolean VDBG = FALSE;  // STOPSHIP if TRUE, logs user data

    /** Query projection for checking for duplicate message segments. */
    private static const String[] PDU_PROJECTION = {
            "pdu"
    };

    /** Query projection for combining concatenated message segments. */
    private static const String[] PDU_SEQUENCE_PORT_PROJECTION = {
            "pdu",
            "sequence",
            "destination_port"
    };

    static const Int32 PDU_COLUMN = 0;
    static const Int32 SEQUENCE_COLUMN = 1;
    static const Int32 DESTINATION_PORT_COLUMN = 2;
    static const Int32 DATE_COLUMN = 3;
    static const Int32 REFERENCE_NUMBER_COLUMN = 4;
    static const Int32 COUNT_COLUMN = 5;
    static const Int32 ADDRESS_COLUMN = 6;
    static const Int32 ID_COLUMN = 7;

    static const String SELECT_BY_ID = "_id=?";
    static const String SELECT_BY_REFERENCE = "address=? AND reference_number=? AND count=?";

    /** New SMS received as an AsyncResult. */
    public static const Int32 EVENT_NEW_SMS = 1;

    /** Message type containing a {@link InboundSmsTracker} ready to broadcast to listeners. */
    static const Int32 EVENT_BROADCAST_SMS = 2;

    /** Message from resultReceiver notifying {@link WaitingState} of a completed broadcast. */
    static const Int32 EVENT_BROADCAST_COMPLETE = 3;

    /** Sent on exit from {@link WaitingState} to return to idle after sending all broadcasts. */
    static const Int32 EVENT_RETURN_TO_IDLE = 4;

    /** Release wakelock after a short timeout when returning to idle state. */
    static const Int32 EVENT_RELEASE_WAKELOCK = 5;

    /** Sent by {@link SmsBroadcastUndelivered} after cleaning the raw table. */
    static const Int32 EVENT_START_ACCEPTING_SMS = 6;

    /** Update phone object */
    static const Int32 EVENT_UPDATE_PHONE_OBJECT = 7;

    /** New SMS received as an AsyncResult. */
    public static const Int32 EVENT_INJECT_SMS = 8;

    /** Wakelock release delay when returning to idle state. */
    private static const Int32 WAKELOCK_TIMEOUT = 3000;

    /** URI for raw table of SMS provider. */
    private static const Uri sRawUri = Uri->WithAppendedPath(Telephony.Sms.CONTENT_URI, "raw");

    protected final Context mContext;
    private final ContentResolver mResolver;

    /** Special handler for WAP push messages. */
    private final WapPushOverSms mWapPush;

    /** Wake lock to ensure device stays awake while dispatching the SMS intents. */
    final PowerManager.WakeLock mWakeLock;

    /** DefaultState throws an exception or logs an error for unhandled message types. */
    final DefaultState mDefaultState = new DefaultState();

    /** Startup state. Waiting for {@link SmsBroadcastUndelivered} to complete. */
    final StartupState mStartupState = new StartupState();

    /** Idle state. Waiting for messages to process. */
    final IdleState mIdleState = new IdleState();

    /** Delivering state. Saves the PDU in the raw table and acknowledges to SMSC. */
    final DeliveringState mDeliveringState = new DeliveringState();

    /** Broadcasting state. Waits for current broadcast to complete before delivering next. */
    final WaitingState mWaitingState = new WaitingState();

    /** Helper class to check whether storage is available for incoming messages. */
    protected SmsStorageMonitor mStorageMonitor;

    private final Boolean mSmsReceiveDisabled;

    protected PhoneBase mPhone;

    protected CellBroadcastHandler mCellBroadcastHandler;

    private UserManager mUserManager;

    /**
     * Create a new SMS broadcast helper.
     * @param name the class name for logging
     * @param context the context of the phone app
     * @param storageMonitor the SmsStorageMonitor to check for storage availability
     */
    protected InboundSmsHandler(String name, Context context, SmsStorageMonitor storageMonitor,
            PhoneBase phone, CellBroadcastHandler cellBroadcastHandler) {
        Super(name);

        mContext = context;
        mStorageMonitor = storageMonitor;
        mPhone = phone;
        mCellBroadcastHandler = cellBroadcastHandler;
        mResolver = context->GetContentResolver();
        mWapPush = new WapPushOverSms(context);

        Boolean smsCapable = mContext->GetResources()->GetBoolean(
                R.bool.config_sms_capable);
        mSmsReceiveDisabled = !SystemProperties->GetBoolean(
                TelephonyProperties.PROPERTY_SMS_RECEIVE, smsCapable);

        PowerManager pm = (PowerManager) mContext->GetSystemService(Context.POWER_SERVICE);
        mWakeLock = pm->NewWakeLock(PowerManager.PARTIAL_WAKE_LOCK, name);
        mWakeLock->Acquire();    // wake lock released after we enter idle state
        mUserManager = (UserManager) mContext->GetSystemService(Context.USER_SERVICE);

        AddState(mDefaultState);
        AddState(mStartupState, mDefaultState);
        AddState(mIdleState, mDefaultState);
        AddState(mDeliveringState, mDefaultState);
            AddState(mWaitingState, mDeliveringState);

        SetInitialState(mStartupState);
        If (DBG) Log("created InboundSmsHandler");
    }

    /**
     * Tell the state machine to quit after processing all messages.
     */
    CARAPI Dispose() {
        Quit();
    }

    /**
     * Update the phone object when it changes.
     */
    CARAPI UpdatePhoneObject(PhoneBase phone) {
        SendMessage(EVENT_UPDATE_PHONE_OBJECT, phone);
    }

    /**
     * Dispose of the WAP push object and release the wakelock.
     */
    //@Override
    protected void OnQuitting() {
        mWapPush->Dispose();

        While (mWakeLock->IsHeld()) {
            mWakeLock->Release();
        }
    }

    // CAF_MSIM Is this used anywhere ? if not remove it
    public PhoneBase GetPhone() {
        return mPhone;
    }

    /**
     * This parent state throws an Exception (for debug builds) or prints an error for unhandled
     * message types.
     */
    class DefaultState extends State {
        //@Override
        public Boolean ProcessMessage(Message msg) {
            Switch (msg.what) {
                case EVENT_UPDATE_PHONE_OBJECT: {
                    OnUpdatePhoneObject((PhoneBase) msg.obj);
                    break;
                }
                default: {
                    String errorText = "processMessage: unhandled message type " + msg.what +
                        " currState=" + GetCurrentState()->GetName();
                    If (Build.IS_DEBUGGABLE) {
                        Loge("---- Dumping InboundSmsHandler ----");
                        Loge("Total records=" + GetLogRecCount());
                        For (Int32 i = Math->Max(GetLogRecSize() - 20, 0); i < GetLogRecSize(); i++) {
                            Loge("Rec[%d]: %s\n" + i + GetLogRec(i).ToString());
                        }
                        Loge("---- Dumped InboundSmsHandler ----");

                        throw new RuntimeException(errorText);
                    } else {
                        Loge(errorText);
                    }
                    break;
                }
            }
            return HANDLED;
        }
    }

    /**
     * The Startup state waits for {@link SmsBroadcastUndelivered} to process the raw table and
     * notify the state machine to broadcast any complete PDUs that might not have been broadcast.
     */
    class StartupState extends State {
        //@Override
        public Boolean ProcessMessage(Message msg) {
            Log("StartupState.processMessage:" + msg.what);
            Switch (msg.what) {
                case EVENT_NEW_SMS:
                case EVENT_INJECT_SMS:
                case EVENT_BROADCAST_SMS:
                    DeferMessage(msg);
                    return HANDLED;

                case EVENT_START_ACCEPTING_SMS:
                    TransitionTo(mIdleState);
                    return HANDLED;

                case EVENT_BROADCAST_COMPLETE:
                case EVENT_RETURN_TO_IDLE:
                case EVENT_RELEASE_WAKELOCK:
                default:
                    // let DefaultState handle these unexpected message types
                    return NOT_HANDLED;
            }
        }
    }

    /**
     * In the idle state the wakelock is released until a new SM arrives, then we transition
     * to Delivering mode to handle it, acquiring the wakelock on exit.
     */
    class IdleState extends State {
        //@Override
        CARAPI Enter() {
            If (DBG) Log("entering Idle state");
            SendMessageDelayed(EVENT_RELEASE_WAKELOCK, WAKELOCK_TIMEOUT);
        }

        //@Override
        CARAPI Exit() {
            mWakeLock->Acquire();
            If (DBG) Log("acquired wakelock, leaving Idle state");
        }

        //@Override
        public Boolean ProcessMessage(Message msg) {
            Log("IdleState.processMessage:" + msg.what);
            If (DBG) Log("Idle state processing message type " + msg.what);
            Switch (msg.what) {
                case EVENT_NEW_SMS:
                case EVENT_INJECT_SMS:
                case EVENT_BROADCAST_SMS:
                    DeferMessage(msg);
                    TransitionTo(mDeliveringState);
                    return HANDLED;

                case EVENT_RELEASE_WAKELOCK:
                    mWakeLock->Release();
                    If (DBG) {
                        If (mWakeLock->IsHeld()) {
                            // this is okay as Int64 as we call Release() for every Acquire()
                            Log("mWakeLock is still held after release");
                        } else {
                            Log("mWakeLock released");
                        }
                    }
                    return HANDLED;

                case EVENT_RETURN_TO_IDLE:
                    // already in idle state; ignore
                    return HANDLED;

                case EVENT_BROADCAST_COMPLETE:
                case EVENT_START_ACCEPTING_SMS:
                default:
                    // let DefaultState handle these unexpected message types
                    return NOT_HANDLED;
            }
        }
    }

    /**
     * In the delivering state, the inbound SMS is processed and stored in the raw table.
     * The message is acknowledged before we exit this state. If there is a message to broadcast,
     * transition to {@link WaitingState} state to send the ordered broadcast and wait for the
     * results. When all messages have been processed, the halting state will release the wakelock.
     */
    class DeliveringState extends State {
        //@Override
        CARAPI Enter() {
            If (DBG) Log("entering Delivering state");
        }

        //@Override
        CARAPI Exit() {
            If (DBG) Log("leaving Delivering state");
        }

        //@Override
        public Boolean ProcessMessage(Message msg) {
            Log("DeliveringState.processMessage:" + msg.what);
            Switch (msg.what) {
                case EVENT_NEW_SMS:
                    // handle new SMS from RIL
                    HandleNewSms((AsyncResult) msg.obj);
                    SendMessage(EVENT_RETURN_TO_IDLE);
                    return HANDLED;

                case EVENT_INJECT_SMS:
                    // handle new injected SMS
                    HandleInjectSms((AsyncResult) msg.obj);
                    SendMessage(EVENT_RETURN_TO_IDLE);
                    return HANDLED;

                case EVENT_BROADCAST_SMS:
                    // if any broadcasts were sent, transition to waiting state
                    If (ProcessMessagePart((InboundSmsTracker) msg.obj)) {
                        TransitionTo(mWaitingState);
                    }
                    return HANDLED;

                case EVENT_RETURN_TO_IDLE:
                    // return to idle after processing all other messages
                    TransitionTo(mIdleState);
                    return HANDLED;

                case EVENT_RELEASE_WAKELOCK:
                    mWakeLock->Release();    // decrement wakelock from previous entry to Idle
                    If (!mWakeLock->IsHeld()) {
                        // wakelock should still be held until 3 seconds after we enter Idle
                        Loge("mWakeLock released while delivering/broadcasting!");
                    }
                    return HANDLED;

                // we shouldn't get this message type in this state, log error and halt.
                case EVENT_BROADCAST_COMPLETE:
                case EVENT_START_ACCEPTING_SMS:
                default:
                    // let DefaultState handle these unexpected message types
                    return NOT_HANDLED;
            }
        }
    }

    /**
     * The waiting state delegates handling of new SMS to parent {@link DeliveringState}, but
     * defers handling of the {@link #EVENT_BROADCAST_SMS} phase until after the current
     * result receiver sends {@link #EVENT_BROADCAST_COMPLETE}. Before transitioning to
     * {@link DeliveringState}, {@link #EVENT_RETURN_TO_IDLE} is sent to transition to
     * {@link IdleState} after any deferred {@link #EVENT_BROADCAST_SMS} messages are handled.
     */
    class WaitingState extends State {
        //@Override
        public Boolean ProcessMessage(Message msg) {
            Log("WaitingState.processMessage:" + msg.what);
            Switch (msg.what) {
                case EVENT_BROADCAST_SMS:
                    // defer until the current broadcast completes
                    DeferMessage(msg);
                    return HANDLED;

                case EVENT_BROADCAST_COMPLETE:
                    // return to idle after handling all deferred messages
                    SendMessage(EVENT_RETURN_TO_IDLE);
                    TransitionTo(mDeliveringState);
                    return HANDLED;

                case EVENT_RETURN_TO_IDLE:
                    // not ready to return to idle; ignore
                    return HANDLED;

                default:
                    // parent state handles the other message types
                    return NOT_HANDLED;
            }
        }
    }

    void HandleNewSms(AsyncResult ar) {
        If (ar.exception != NULL) {
            Loge("Exception processing incoming SMS: " + ar.exception);
            return;
        }

        Int32 result, blacklistMatchType = -1;
        SmsMessage sms = NULL;

        try {
            sms = (SmsMessage) ar.result;
            result = DispatchMessage(sms.mWrappedSmsMessage);
        } Catch (RuntimeException ex) {
            Loge("Exception dispatching message", ex);
            result = Intents.RESULT_SMS_GENERIC_ERROR;
        }

        // Translate (internal) blacklist check results to
        // RESULT_SMS_HANDLED + match type
        Switch (result) {
            case Intents.RESULT_SMS_BLACKLISTED_UNKNOWN:
                blacklistMatchType = BlacklistUtils.MATCH_UNKNOWN;
                result = Intents.RESULT_SMS_HANDLED;
                break;
            case Intents.RESULT_SMS_BLACKLISTED_LIST:
                blacklistMatchType = BlacklistUtils.MATCH_LIST;
                result = Intents.RESULT_SMS_HANDLED;
                break;
            case Intents.RESULT_SMS_BLACKLISTED_REGEX:
                blacklistMatchType = BlacklistUtils.MATCH_REGEX;
                result = Intents.RESULT_SMS_HANDLED;
                break;
        }


        // RESULT_OK means that the SMS will be acknowledged by special handling,
        // e.g. for SMS-PP data download. Any other result, we should ack here.
        If (result != Activity.RESULT_OK) {
            Boolean handled = (result == Intents.RESULT_SMS_HANDLED);
            NotifyAndAcknowledgeLastIncomingSms(handled, result, blacklistMatchType, sms, NULL);
        }
    }

    /**
     * This method is called when a new SMS PDU is injected into application framework.
     * @param ar is the AsyncResult that has the SMS PDU to be injected.
     */
    void HandleInjectSms(AsyncResult ar) {
        Int32 result;
        PendingIntent receivedIntent = NULL;
        try {
            receivedIntent = (PendingIntent) ar.userObj;
            SmsMessage sms = (SmsMessage) ar.result;
            If (sms == NULL) {
              result = Intents.RESULT_SMS_GENERIC_ERROR;
            } else {
              result = DispatchMessage(sms.mWrappedSmsMessage);
            }
        } Catch (RuntimeException ex) {
            Loge("Exception dispatching message", ex);
            result = Intents.RESULT_SMS_GENERIC_ERROR;
        }

        If (receivedIntent != NULL) {
            try {
                receivedIntent->Send(result);
            } Catch (CanceledException e) { }
        }
    }

    /**
     * Process an SMS message from the RIL, calling subclass methods to handle 3GPP and
     * 3GPP2-specific message types.
     *
     * @param smsb the SmsMessageBase object from the RIL
     * @return a result code from {@link android.provider.Telephony.Sms.Intents},
     *  or {@link Activity#RESULT_OK} for delayed acknowledgment to SMSC
     */
    public Int32 DispatchMessage(SmsMessageBase smsb) {
        // If sms is NULL, there was a parsing error.
        If (smsb == NULL) {
            Loge("dispatchSmsMessage: message is NULL");
            return Intents.RESULT_SMS_GENERIC_ERROR;
        }

        If (mSmsReceiveDisabled) {
            // Device doesn't support receiving SMS,
            Log("Received short message on device which doesn't support "
                    + "receiving SMS. Ignored.");
            return Intents.RESULT_SMS_HANDLED;
        }

        return DispatchMessageRadioSpecific(smsb);
    }

    /**
     * Process voicemail notification, SMS-PP data download, CDMA CMAS, CDMA WAP push, and other
     * 3GPP/3GPP2-specific messages. Regular SMS messages are handled by calling the shared
     * {@link #dispatchNormalMessage} from this class.
     *
     * @param smsb the SmsMessageBase object from the RIL
     * @return a result code from {@link android.provider.Telephony.Sms.Intents},
     *  or {@link Activity#RESULT_OK} for delayed acknowledgment to SMSC
     */
    protected abstract Int32 DispatchMessageRadioSpecific(SmsMessageBase smsb);

    /**
     * Send an acknowledge message to the SMSC.
     * @param success indicates that last message was successfully received.
     * @param result result code indicating any error
     * @param response callback message sent when operation completes.
     */
    protected abstract void AcknowledgeLastIncomingSms(Boolean success,
            Int32 result, Message response);

    /**
     * Called when the phone changes the default method updates mPhone
     * mStorageMonitor and mCellBroadcastHandler.updatePhoneObject.
     * Override if different or other behavior is desired.
     *
     * @param phone
     */
    protected void OnUpdatePhoneObject(PhoneBase phone) {
        mPhone = phone;
        mStorageMonitor = mPhone.mSmsStorageMonitor;
        Log("onUpdatePhoneObject: phone=" + mPhone->GetClass()->GetSimpleName());
    }

    /**
     * Notify interested apps if the framework has rejected an incoming SMS,
     * and send an acknowledge message to the network.
     * @param success indicates that last message was successfully received.
     * @param result result code indicating any error
     * @param blacklistMatchType blacklist type if the message was blacklisted,
     *                           -1 if it wasn't blacklisted
     * @param sms incoming SMS
     * @param response callback message sent when operation completes.
     */
    void NotifyAndAcknowledgeLastIncomingSms(Boolean success,
            Int32 result, Int32 blacklistMatchType, SmsMessage sms, Message response) {
        If (!success || blacklistMatchType >= 0) {
            // broadcast SMS_REJECTED_ACTION intent
            Intent intent = new Intent(Intents.SMS_REJECTED_ACTION);
            intent->PutExtra("result", result);
            intent->PutExtra("blacklisted", blacklistMatchType >= 0);
            If (blacklistMatchType >= 0) {
                intent->PutExtra("blacklistMatchType", blacklistMatchType);
            }
            If (sms != NULL) {
                intent->PutExtra("sender", sms->GetOriginatingAddress());
                intent->PutExtra("timestamp", sms->GetTimestampMillis());
            }
            If (DBG) Log("NotifyAndAcknowledgeLastIncomingSms(): reject intent= " + intent);
            mContext->SendBroadcast(intent, Manifest::permission::RECEIVE_SMS);
        }
        AcknowledgeLastIncomingSms(success, result, response);
    }

    /**
     * Return TRUE if this handler is for 3GPP2 messages; FALSE for 3GPP format.
     * @return TRUE for the 3GPP2 handler; FALSE for the 3GPP handler
     */
    protected abstract Boolean Is3gpp2();

    /**
     * Dispatch a normal incoming SMS. This is called from {@link #dispatchMessageRadioSpecific}
     * if no format-specific handling was required. Saves the PDU to the SMS provider raw table,
     * creates an {@link InboundSmsTracker}, then sends it to the state machine as an
     * {@link #EVENT_BROADCAST_SMS}. Returns {@link Intents#RESULT_SMS_HANDLED} or an error value.
     *
     * @param sms the message to dispatch
     * @return {@link Intents#RESULT_SMS_HANDLED} if the message was accepted, or an error status
     */
    protected Int32 DispatchNormalMessage(SmsMessageBase sms) {
        Int32 blacklistResult = CheckIfBlacklisted(sms);
        If (blacklistResult != Intents.RESULT_SMS_HANDLED) {
            return blacklistResult;
        }

        SmsHeader smsHeader = sms->GetUserDataHeader();
        InboundSmsTracker tracker;

        If ((smsHeader == NULL) || (smsHeader.concatRef == NULL)) {
            // Message is not concatenated.
            Int32 destPort = -1;
            If (smsHeader != NULL && smsHeader.portAddrs != NULL) {
                // The message was sent to a port.
                destPort = smsHeader.portAddrs.destPort;
                If (DBG) Log("destination port: " + destPort);
            }

            tracker = new InboundSmsTracker(sms->GetPdu(), sms->GetTimestampMillis(), destPort,
                    Is3gpp2(), FALSE);
        } else {
            // Create a tracker for this message segment.
            SmsHeader.ConcatRef concatRef = smsHeader.concatRef;
            SmsHeader.PortAddrs portAddrs = smsHeader.portAddrs;
            Int32 destPort = (portAddrs != NULL ? portAddrs.destPort : -1);

            tracker = new InboundSmsTracker(sms->GetPdu(), sms->GetTimestampMillis(), destPort,
                    Is3gpp2(), sms->GetOriginatingAddress(), concatRef.refNumber,
                    concatRef.seqNumber, concatRef.msgCount, FALSE);
        }

        If (VDBG) Log("created tracker: " + tracker);
        return AddTrackerToRawTableAndSendMessage(tracker);
    }

    private Int32 CheckIfBlacklisted(SmsMessageBase sms) {
        Int32 result = BlacklistUtils->IsListed(mContext,
                sms->GetOriginatingAddress(), BlacklistUtils.BLOCK_MESSAGES);

        Switch (result) {
            case BlacklistUtils.MATCH_UNKNOWN:
                return Intents.RESULT_SMS_BLACKLISTED_UNKNOWN;
            case BlacklistUtils.MATCH_LIST:
                return Intents.RESULT_SMS_BLACKLISTED_LIST;
            case BlacklistUtils.MATCH_REGEX:
                return Intents.RESULT_SMS_BLACKLISTED_REGEX;
        }

        return Intents.RESULT_SMS_HANDLED;
    }

    /**
     * Helper to add the tracker to the raw table and then send a message to broadcast it, if
     * successful. Returns the SMS intent status to return to the SMSC.
     * @param tracker the tracker to save to the raw table and then deliver
     * @return {@link Intents#RESULT_SMS_HANDLED} or {@link Intents#RESULT_SMS_GENERIC_ERROR}
     * or {@link Intents#RESULT_SMS_DUPLICATED}
     */
    protected Int32 AddTrackerToRawTableAndSendMessage(InboundSmsTracker tracker) {
        Switch(AddTrackerToRawTable(tracker)) {
        case Intents.RESULT_SMS_HANDLED:
            SendMessage(EVENT_BROADCAST_SMS, tracker);
            return Intents.RESULT_SMS_HANDLED;

        case Intents.RESULT_SMS_DUPLICATED:
            return Intents.RESULT_SMS_HANDLED;

        case Intents.RESULT_SMS_GENERIC_ERROR:
        default:
            return Intents.RESULT_SMS_GENERIC_ERROR;
        }
    }

    /**
     * Process the inbound SMS segment. If the message is complete, send it as an ordered
     * broadcast to interested receivers and return TRUE. If the message is a segment of an
     * incomplete multi-part SMS, return FALSE.
     * @param tracker the tracker containing the message segment to process
     * @return TRUE if an ordered broadcast was sent; FALSE if waiting for more message segments
     */
    Boolean ProcessMessagePart(InboundSmsTracker tracker) {
        Int32 messageCount = tracker->GetMessageCount();
        Byte[][] pdus;
        Int32 destPort = tracker->GetDestPort();
        String address = "";

        If (messageCount == 1) {
            // single-part message
            pdus = new Byte[][]{tracker->GetPdu()};
        } else {
            // multi-part message
            Cursor cursor = NULL;
            try {
                // used by several query selection arguments
                address = tracker->GetAddress();
                String refNumber = Integer->ToString(tracker->GetReferenceNumber());
                String count = Integer->ToString(tracker->GetMessageCount());

                // query for all segments and broadcast message if we have all the parts
                String[] whereArgs = {address, refNumber, count};
                cursor = mResolver->Query(sRawUri, PDU_SEQUENCE_PORT_PROJECTION,
                        SELECT_BY_REFERENCE, whereArgs, NULL);

                Int32 cursorCount = cursor->GetCount();
                If (cursorCount < messageCount) {
                    // Wait for the other message parts to arrive. It's also possible for the last
                    // segment to arrive before processing the EVENT_BROADCAST_SMS for one of the
                    // earlier segments. In that case, the broadcast will be sent as soon as all
                    // segments are in the table, and any later EVENT_BROADCAST_SMS messages will
                    // get a row count of 0 and return.
                    return FALSE;
                }

                // All the parts are in place, deal with them
                pdus = new Byte[messageCount][];
                While (cursor->MoveToNext()) {
                    // subtract offset to convert sequence to 0-based array index
                    Int32 index = cursor->GetInt(SEQUENCE_COLUMN) - tracker->GetIndexOffset();

                    pdus[index] = HexDump->HexStringToByteArray(cursor->GetString(PDU_COLUMN));

                    // Read the destination port from the first Segment (needed for CDMA WAP PDU).
                    // It's not a bad idea to prefer the port from the first segment in other cases.
                    If (index == 0 && !cursor->IsNull(DESTINATION_PORT_COLUMN)) {
                        Int32 port = cursor->GetInt(DESTINATION_PORT_COLUMN);
                        // strip format flags and convert to real port number, or -1
                        port = InboundSmsTracker->GetRealDestPort(port);
                        If (port != -1) {
                            destPort = port;
                        }
                    }
                }
            } Catch (SQLException e) {
                Loge("Can't access multipart SMS database", e);
                return FALSE;
            } finally {
                If (cursor != NULL) {
                    cursor->Close();
                }
            }
        }

        BroadcastReceiver resultReceiver = new SmsBroadcastReceiver(tracker);

        If (destPort == SmsHeader.PORT_WAP_PUSH) {
            // Build up the data stream
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            For (Byte[] pdu : pdus) {
                // 3GPP needs to extract the User Data from the PDU; 3GPP2 has already done this
                If (!tracker->Is3gpp2()) {
                    SmsMessage msg = SmsMessage->CreateFromPdu(pdu, SmsConstants.FORMAT_3GPP);
                    pdu = msg->GetUserData();
                    If (address == "") {
                       address = msg->GetOriginatingAddress();
                    } else If(address == ""){
                       address = tracker->GetAddress();
                    }
                }
                output->Write(pdu, 0, pdu.length);
            }
            Int32 result = mWapPush->DispatchWapPdu(output->ToByteArray(), resultReceiver,
                    this, address);
            If (DBG) Log("DispatchWapPdu() returned " + result);
            // result is Activity.RESULT_OK if an ordered broadcast was sent
            Return (result == Activity.RESULT_OK);
        }

        Intent intent;
        List<String> regAddresses = Settings.Secure->GetDelimitedStringAsList(mContext->GetContentResolver(),
                Settings.Secure.PROTECTED_SMS_ADDRESSES , "|");

        List<String> allAddresses = Intents
                .GetNormalizedAddressesFromPdus(pdus, tracker->GetFormat());

        If (!Collections->Disjoint(regAddresses, allAddresses)) {
            intent = new Intent(Intents.PROTECTED_SMS_RECEIVED_ACTION);
            intent->PutExtra("pdus", pdus);
            intent->PutExtra("format", tracker->GetFormat());
            DispatchIntent(intent, Manifest::permission::RECEIVE_PROTECTED_SMS,
                    AppOpsManager.OP_RECEIVE_SMS, resultReceiver, UserHandle.OWNER);
            return TRUE;
        }

        intent = new Intent(Intents.SMS_FILTER_ACTION);
        List<String> carrierPackages = NULL;
        UiccCard card = UiccController->GetInstance()->GetUiccCard();
        If (card != NULL) {
            carrierPackages = card->GetCarrierPackageNamesForIntent(
                    mContext->GetPackageManager(), intent);
        }
        If (carrierPackages != NULL && carrierPackages->Size() == 1) {
            intent->SetPackage(carrierPackages->Get(0));
            intent->PutExtra("destport", destPort);
        } else {
            SetAndDirectIntent(intent, destPort);
        }

        intent->PutExtra("pdus", pdus);
        intent->PutExtra("format", tracker->GetFormat());
        DispatchIntent(intent, Manifest::permission::RECEIVE_SMS,
                AppOpsManager.OP_RECEIVE_SMS, resultReceiver, UserHandle.OWNER);
        return TRUE;
    }

    /**
     * Dispatch the intent with the specified permission, appOp, and result receiver, using
     * this state machine's handler thread to run the result receiver.
     *
     * @param intent the intent to broadcast
     * @param permission receivers are required to have this permission
     * @param appOp app op that is being performed when dispatching to a receiver
     * @param user user to deliver the intent to
     */
    protected void DispatchIntent(Intent intent, String permission, Int32 appOp,
            BroadcastReceiver resultReceiver, UserHandle user) {
        intent->AddFlags(IIntent::FLAG_RECEIVER_NO_ABORT);
        SubscriptionManager->PutPhoneIdAndSubIdExtra(intent, mPhone->GetPhoneId());
        If (user->Equals(UserHandle.ALL)) {
            // Get a list of currently started users.
            Int32[] users = NULL;
            try {
                users = ActivityManagerNative->GetDefault()->GetRunningUserIds();
            } Catch (RemoteException re) {
            }
            If (users == NULL) {
                users = new Int32[] {user->GetIdentifier()};
            }
            // Deliver the broadcast only to those running users that are permitted
            // by user policy.
            For (Int32 i = users.length - 1; i >= 0; i--) {
                UserHandle targetUser = new UserHandle(users[i]);
                If (users[i] != UserHandle.USER_OWNER) {
                    // Is the user not allowed to use SMS?
                    If (mUserManager->HasUserRestriction(UserManager.DISALLOW_SMS, targetUser)) {
                        continue;
                    }
                    // Skip unknown users and managed profiles as well
                    UserInfo info = mUserManager->GetUserInfo(users[i]);
                    If (info == NULL || info->IsManagedProfile()) {
                        continue;
                    }
                }
                // Only pass in the resultReceiver when the USER_OWNER is processed.
                mContext->SendOrderedBroadcastAsUser(intent, targetUser, permission, appOp,
                        users[i] == UserHandle.USER_OWNER ? resultReceiver : NULL,
                        GetHandler(), Activity.RESULT_OK, NULL, NULL);
            }
        } else {
            mContext->SendOrderedBroadcastAsUser(intent, user, permission, appOp,
                    resultReceiver,
                    GetHandler(), Activity.RESULT_OK, NULL, NULL);
        }
    }

    /**
     * Helper for {@link SmsBroadcastUndelivered} to delete an old message in the raw table.
     */
    void DeleteFromRawTable(String deleteWhere, String[] deleteWhereArgs) {
        Int32 rows = mResolver->Delete(sRawUri, deleteWhere, deleteWhereArgs);
        If (rows == 0) {
            Loge("No rows were deleted from raw table!");
        } else If (DBG) {
            Log("Deleted " + rows + " rows from raw table.");
        }
    }

    /**
     * Set the appropriate intent action and direct the intent to the default SMS app or the
     * appropriate port.
     *
     * @param intent the intent to set and direct
     * @param destPort the destination port
     */
    void SetAndDirectIntent(Intent intent, Int32 destPort) {
        If (destPort == -1) {
            intent->SetAction(Intents.SMS_DELIVER_ACTION);

            // Direct the intent to only the default SMS app. If we can't find a default SMS app
            // then sent it to all broadcast receivers.
            // We are deliberately delivering to the primary user's default SMS App.
            ComponentName componentName = SmsApplication->GetDefaultSmsApplication(mContext, TRUE);
            If (componentName != NULL) {
                // Deliver SMS message only to this receiver.
                intent->SetComponent(componentName);
                Log("Delivering SMS to: " + componentName->GetPackageName() +
                    " " + componentName->GetClassName());
            } else {
                intent->SetComponent(NULL);
            }
        } else {
            intent->SetAction(Intents.DATA_SMS_RECEIVED_ACTION);
            Uri uri = Uri->Parse("sms://localhost:" + destPort);
            intent->SetData(uri);
            intent->SetComponent(NULL);
        }
    }

    /**
     * Insert a message PDU into the raw table so we can acknowledge it immediately.
     * If the device crashes before the broadcast to listeners completes, it will be delivered
     * from the raw table on the next device boot. For single-part messages, the deleteWhere
     * and deleteWhereArgs fields of the tracker will be set to delete the correct row after
     * the ordered broadcast completes.
     *
     * @param tracker the tracker to add to the raw table
     * @return TRUE on success; FALSE on failure to write to database
     */
    private Int32 AddTrackerToRawTable(InboundSmsTracker tracker) {
        If (tracker->GetMessageCount() != 1) {
            // check for duplicate message segments
            Cursor cursor = NULL;
            try {
                // sequence numbers are 1-based except for CDMA WAP, which is 0-based
                Int32 sequence = tracker->GetSequenceNumber();

                // convert to strings for query
                String address = tracker->GetAddress();
                String refNumber = Integer->ToString(tracker->GetReferenceNumber());
                String count = Integer->ToString(tracker->GetMessageCount());

                String seqNumber = Integer->ToString(sequence);

                // set the delete selection args for multi-part message
                String[] deleteWhereArgs = {address, refNumber, count};
                tracker->SetDeleteWhere(SELECT_BY_REFERENCE, deleteWhereArgs);

                // Check for duplicate message segments
                cursor = mResolver->Query(sRawUri, PDU_PROJECTION,
                        "address=? AND reference_number=? AND count=? AND sequence=?",
                        new String[] {address, refNumber, count, seqNumber}, NULL);

                // MoveToNext() returns FALSE if no duplicates were found
                If (cursor->MoveToNext()) {
                    Loge("Discarding duplicate message segment, refNumber=" + refNumber
                            + " seqNumber=" + seqNumber);
                    String oldPduString = cursor->GetString(PDU_COLUMN);
                    Byte[] pdu = tracker->GetPdu();
                    Byte[] oldPdu = HexDump->HexStringToByteArray(oldPduString);
                    If (!Arrays->Equals(oldPdu, tracker->GetPdu())) {
                        Loge("Warning: dup message segment PDU of length " + pdu.length
                                + " is different from existing PDU of length " + oldPdu.length);
                    }
                    return Intents.RESULT_SMS_DUPLICATED;   // reject message
                }
                cursor->Close();
            } Catch (SQLException e) {
                Loge("Can't access multipart SMS database", e);
                return Intents.RESULT_SMS_GENERIC_ERROR;    // reject message
            } finally {
                If (cursor != NULL) {
                    cursor->Close();
                }
            }
        }

        ContentValues values = tracker->GetContentValues();

        If (VDBG) Log("adding content values to raw table: " + values->ToString());
        Uri newUri = mResolver->Insert(sRawUri, values);
        If (DBG) Log("URI of new row -> " + newUri);

        try {
            Int64 rowId = ContentUris->ParseId(newUri);
            If (tracker->GetMessageCount() == 1) {
                // set the delete selection args for single-part message
                tracker->SetDeleteWhere(SELECT_BY_ID, new String[]{Long->ToString(rowId)});
            }
            return Intents.RESULT_SMS_HANDLED;
        } Catch (Exception e) {
            Loge("error parsing URI for new row: " + newUri, e);
            return Intents.RESULT_SMS_GENERIC_ERROR;
        }
    }

    /**
     * Returns whether the default message format for the current radio technology is 3GPP2.
     * @return TRUE if the radio technology uses 3GPP2 format by default, FALSE for 3GPP format
     */
    static Boolean IsCurrentFormat3gpp2() {
        Int32 activePhone = TelephonyManager->GetDefault()->GetCurrentPhoneType();
        Return (PHONE_TYPE_CDMA == activePhone);
    }

    protected void StoreVoiceMailCount() {
        // Store the voice mail count in persistent memory.
        String imsi = mPhone->GetSubscriberId();
        Int32 mwi = mPhone->GetVoiceMessageCount();

        Log("Storing Voice Mail Count = " + mwi
                    + " for mVmCountKey = " + ((PhoneBase)mPhone).VM_COUNT
                    + " vmId = " + ((PhoneBase)mPhone).VM_ID
                    + " subId = "+ mPhone->GetSubId()
                    + " in preferences.");

        SharedPreferences sp = PreferenceManager->GetDefaultSharedPreferences(mContext);
        SharedPreferences.Editor editor = sp->Edit();
        editor->PutInt(mPhone.VM_COUNT + mPhone->GetSubId(), mwi);
        editor->PutString(mPhone.VM_ID + mPhone->GetSubId(), imsi);
        editor->Commit();
    }

    /**
     * Handler for an {@link InboundSmsTracker} broadcast. Deletes PDUs from the raw table and
     * logs the broadcast Duration (as an error if the other receivers were especially slow).
     */
    private final class SmsBroadcastReceiver extends BroadcastReceiver {
        private final String mDeleteWhere;
        private final String[] mDeleteWhereArgs;
        private Int64 mBroadcastTimeNano;

        SmsBroadcastReceiver(InboundSmsTracker tracker) {
            mDeleteWhere = tracker->GetDeleteWhere();
            mDeleteWhereArgs = tracker->GetDeleteWhereArgs();
            mBroadcastTimeNano = System->NanoTime();
        }

        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            If (action->Equals(Intents.SMS_FILTER_ACTION)) {
                Int32 rc = GetResultCode();
                If (rc == Activity.RESULT_OK) {
                  // Overwrite pdus data if the SMS filter has set it.
                  Bundle resultExtras = GetResultExtras(FALSE);
                  If (resultExtras != NULL && resultExtras->ContainsKey("pdus")) {
                      intent->PutExtra("pdus", (Byte[][]) resultExtras->Get("pdus"));
                  }
                  If (intent->HasExtra("destport")) {
                      Int32 destPort = intent->GetIntExtra("destport", -1);
                      intent->RemoveExtra("destport");
                      SetAndDirectIntent(intent, destPort);
                      If (SmsManager->GetDefault()->GetAutoPersisting()) {
                          final Uri uri = WriteInboxMessage(intent);
                          If (uri != NULL) {
                              // Pass this to SMS apps so that they know where it is stored
                              intent->PutExtra("uri", uri->ToString());
                          }
                      }
                      DispatchIntent(intent, Manifest::permission::RECEIVE_SMS,
                                     AppOpsManager.OP_RECEIVE_SMS, this, UserHandle.OWNER);
                  } else {
                      Loge("destport doesn't exist in the extras for SMS filter action.");
                  }
                } else {
                  // Drop this SMS.
                  Log("SMS filtered by result code " + rc);
                  DeleteFromRawTable(mDeleteWhere, mDeleteWhereArgs);
                  SendMessage(EVENT_BROADCAST_COMPLETE);
                }
            } else If (action->Equals(Intents.SMS_DELIVER_ACTION)) {
                // Now dispatch the notification only intent
                intent->SetAction(Intents.SMS_RECEIVED_ACTION);
                intent->SetComponent(NULL);
                // All running users will be notified of the received sms.
                DispatchIntent(intent, Manifest::permission::RECEIVE_SMS,
                        AppOpsManager.OP_RECEIVE_SMS, this, UserHandle.ALL);
            } else If (action->Equals(Intents.WAP_PUSH_DELIVER_ACTION)) {
                // Now dispatch the notification only intent
                intent->SetAction(Intents.WAP_PUSH_RECEIVED_ACTION);
                intent->SetComponent(NULL);
                // Only the primary user will receive notification of incoming mms.
                // That app will do the actual downloading of the mms.
                DispatchIntent(intent, Manifest::permission::RECEIVE_SMS,
                        AppOpsManager.OP_RECEIVE_SMS, this, UserHandle.OWNER);
            } else {
                // Now that the intents have been deleted we can clean up the PDU data.
                If (!Intents.DATA_SMS_RECEIVED_ACTION->Equals(action)
                        && !Intents.SMS_RECEIVED_ACTION->Equals(action)
                        && !Intents.DATA_SMS_RECEIVED_ACTION->Equals(action)
                        && !Intents.WAP_PUSH_RECEIVED_ACTION->Equals(action)) {
                    Loge("unexpected BroadcastReceiver action: " + action);
                }

                Int32 rc = GetResultCode();
                If ((rc != Activity.RESULT_OK) && (rc != Intents.RESULT_SMS_HANDLED)) {
                    Loge("a broadcast receiver set the result code to " + rc
                            + ", deleting from raw table anyway!");
                } else If (DBG) {
                    Log("successful broadcast, deleting from raw table.");
                }

                DeleteFromRawTable(mDeleteWhere, mDeleteWhereArgs);
                SendMessage(EVENT_BROADCAST_COMPLETE);

                Int32 durationMillis = (Int32) ((System->NanoTime() - mBroadcastTimeNano) / 1000000);
                If (durationMillis >= 5000) {
                    Loge("Slow ordered broadcast completion time: " + durationMillis + " ms");
                } else If (DBG) {
                    Log("ordered broadcast completed in: " + durationMillis + " ms");
                }
            }
        }
    }

    /**
     * Log with debug level.
     * @param s the string to log
     */
    //@Override
    protected void Log(String s) {
        Rlog->D(GetName(), s);
    }

    /**
     * Log with error level.
     * @param s the string to log
     */
    //@Override
    protected void Loge(String s) {
        Rlog->E(GetName(), s);
    }

    /**
     * Log with error level.
     * @param s the string to log
     * @param e is a Throwable which logs additional information.
     */
    //@Override
    protected void Loge(String s, Throwable e) {
        Rlog->E(GetName(), s, e);
    }

    /**
     * Store a received SMS into Telephony provider
     *
     * @param intent The intent containing the received SMS
     * @return The URI of written message
     */
    private Uri WriteInboxMessage(Intent intent) {
        final SmsMessage[] messages = Telephony.Sms.Intents->GetMessagesFromIntent(intent);
        If (messages == NULL || messages.length < 1) {
            Loge("Failed to parse SMS pdu");
            return NULL;
        }
        // Sometimes, SmsMessage.mWrappedSmsMessage is NULL causing NPE when we access
        // the methods on it although the SmsMessage itself is not NULL. So do this check
        // before we do anything on the parsed SmsMessages.
        For (final SmsMessage sms : messages) {
            try {
                sms->GetDisplayMessageBody();
            } Catch (NullPointerException e) {
                Loge("NPE inside SmsMessage");
                return NULL;
            }
        }
        final ContentValues values = ParseSmsMessage(messages);
        final Int64 identity = Binder->ClearCallingIdentity();
        try {
            return mContext->GetContentResolver()->Insert(Telephony.Sms.Inbox.CONTENT_URI, values);
        } Catch (Exception e) {
            Loge("Failed to persist inbox message", e);
        } finally {
            Binder->RestoreCallingIdentity(identity);
        }
        return NULL;
    }

    /**
     * Convert SmsMessage[] into SMS database schema columns
     *
     * @param msgs The SmsMessage array of the received SMS
     * @return ContentValues representing the columns of parsed SMS
     */
    private static ContentValues ParseSmsMessage(SmsMessage[] msgs) {
        final SmsMessage sms = msgs[0];
        final ContentValues values = new ContentValues();
        values->Put(Telephony.Sms.Inbox.ADDRESS, sms->GetDisplayOriginatingAddress());
        values->Put(Telephony.Sms.Inbox.BODY, BuildMessageBodyFromPdus(msgs));
        values->Put(Telephony.Sms.Inbox.DATE_SENT, sms->GetTimestampMillis());
        values->Put(Telephony.Sms.Inbox.DATE, System->CurrentTimeMillis());
        values->Put(Telephony.Sms.Inbox.PROTOCOL, sms->GetProtocolIdentifier());
        values->Put(Telephony.Sms.Inbox.SEEN, 0);
        values->Put(Telephony.Sms.Inbox.READ, 0);
        final String subject = sms->GetPseudoSubject();
        If (!TextUtils->IsEmpty(subject)) {
            values->Put(Telephony.Sms.Inbox.SUBJECT, subject);
        }
        values->Put(Telephony.Sms.Inbox.REPLY_PATH_PRESENT, sms->IsReplyPathPresent() ? 1 : 0);
        values->Put(Telephony.Sms.Inbox.SERVICE_CENTER, sms->GetServiceCenterAddress());
        return values;
    }

    /**
     * Build up the SMS message body from the SmsMessage array of received SMS
     *
     * @param msgs The SmsMessage array of the received SMS
     * @return The text message body
     */
    private static String BuildMessageBodyFromPdus(SmsMessage[] msgs) {
        If (msgs.length == 1) {
            // There is only one part, so grab the body directly.
            return ReplaceFormFeeds(msgs[0].GetDisplayMessageBody());
        } else {
            // Build up the body from the parts.
            StringBuilder body = new StringBuilder();
            For (SmsMessage msg: msgs) {
                // GetDisplayMessageBody() can NPE if mWrappedMessage inside is NULL.
                body->Append(msg->GetDisplayMessageBody());
            }
            return ReplaceFormFeeds(body->ToString());
        }
    }

    // Some providers send formfeeds in their messages. Convert those formfeeds to newlines.
    private static String ReplaceFormFeeds(String s) {
        return s == NULL ? "" : s->Replace('\f', '\n');
    }
}
