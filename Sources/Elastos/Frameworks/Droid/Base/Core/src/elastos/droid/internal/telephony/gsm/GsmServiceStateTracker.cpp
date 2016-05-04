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

package com.android.internal.telephony.gsm;

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Telephony::ICellIdentityGsm;
using Elastos::Droid::Telephony::ICellIdentityLte;
using Elastos::Droid::Telephony::ICellIdentityWcdma;
using Elastos::Droid::Telephony::ICellInfo;
using Elastos::Droid::Telephony::ICellInfoGsm;
using Elastos::Droid::Telephony::ICellInfoLte;
using Elastos::Droid::Telephony::ICellInfoWcdma;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Telephony::Gsm::IGsmCellLocation;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::IEventLog;
using Elastos::Droid::Utility::ITimeUtils;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IEventLogTags;
using Elastos::Droid::Internal::Telephony::IMccTable;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IProxyController;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::IRILConstants;
using Elastos::Droid::Internal::Telephony::IRestrictedState;
using Elastos::Droid::Internal::Telephony::IServiceStateTracker;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDcTrackerBase;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppState;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::ISIMRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;
using Elastos::Utility::ICalendar;
using Elastos::Utility::IDate;
using Elastos::Utility::IList;
using Elastos::Utility::ITimeZone;

/**
 * {@hide}
 */
final class GsmServiceStateTracker extends ServiceStateTracker {
    static const String LOG_TAG = "GsmSST";
    static const Boolean VDBG = FALSE;
    //CAF_MSIM make it private ??
    private static const Int32 EVENT_ALL_DATA_DISCONNECTED = 1001;
    private GSMPhone mPhone;
    GsmCellLocation mCellLoc;
    GsmCellLocation mNewCellLoc;
    Int32 mPreferredNetworkType;

    private Int32 mMaxDataCalls = 1;
    private Int32 mNewMaxDataCalls = 1;
    private Int32 mReasonDataDenied = -1;
    private Int32 mNewReasonDataDenied = -1;

    /**
     * GSM roaming status solely based on TS 27.007 7.2 CREG. Only used by
     * handlePollStateResult to store CREG roaming result.
     */
    private Boolean mGsmRoaming = FALSE;

    /**
     * Data roaming status solely based on TS 27.007 10.1.19 CGREG. Only used by
     * handlePollStateResult to store CGREG roaming result.
     */
    private Boolean mDataRoaming = FALSE;

    /**
     * Mark when service state is in emergency call only mode
     */
    private Boolean mEmergencyOnly = FALSE;

    /**
     * Sometimes we get the NITZ time before we know what country we
     * are in. Keep the time zone information from the NITZ string so
     * we can fix the time zone once know the country.
     */
    private Boolean mNeedFixZoneAfterNitz = FALSE;
    private Int32 mZoneOffset;
    private Boolean mZoneDst;
    private Int64 mZoneTime;
    private Boolean mGotCountryCode = FALSE;
    private ContentResolver mCr;

    /** Boolean is TRUE is setTimeFromNITZString was called */
    private Boolean mNitzUpdatedTime = FALSE;

    String mSavedTimeZone;
    Int64 mSavedTime;
    Int64 mSavedAtTime;

    /** Started the recheck process after finding gprs should registered but not. */
    private Boolean mStartedGprsRegCheck = FALSE;

    /** Already sent the event-log for no gprs register. */
    private Boolean mReportedGprsNoReg = FALSE;

    /**
     * The Notification object given to the NotificationManager.
     */
    private Notification mNotification;

    /** Wake lock used while setting time of day. */
    private PowerManager.WakeLock mWakeLock;
    private static const String WAKELOCK_TAG = "ServiceStateTracker";

    /** Keep track of SPN display rules, so we only broadcast intent if something changes. */
    private String mCurSpn = NULL;
    private String mCurPlmn = NULL;
    private Boolean mCurShowPlmn = FALSE;
    private Boolean mCurShowSpn = FALSE;

    /** Notification type. */
    static const Int32 PS_ENABLED = 1001;            // Access Control blocks data service
    static const Int32 PS_DISABLED = 1002;           // Access Control enables data service
    static const Int32 CS_ENABLED = 1003;            // Access Control blocks all voice/sms service
    static const Int32 CS_DISABLED = 1004;           // Access Control enables all voice/sms service
    static const Int32 CS_NORMAL_ENABLED = 1005;     // Access Control blocks normal voice/sms service
    static const Int32 CS_EMERGENCY_ENABLED = 1006;  // Access Control blocks emergency call service

    /** Notification id. */
    static const Int32 PS_NOTIFICATION = 888;  // Id to update and cancel PS restricted
    static const Int32 CS_NOTIFICATION = 999;  // Id to update and cancel CS restricted

    private BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            If (!mPhone.mIsTheCurrentActivePhone) {
                Rlog->E(LOG_TAG, "Received Intent " + intent +
                        " while being destroyed. Ignoring.");
                return;
            }

            If (intent->GetAction()->Equals(IIntent::ACTION_LOCALE_CHANGED)) {
                // update emergency string whenever locale changed
                UpdateSpnDisplay();
            } else If (intent->GetAction()->Equals(ACTION_RADIO_OFF)) {
                mAlarmSwitch = FALSE;
                DcTrackerBase dcTracker = mPhone.mDcTracker;
                PowerOffRadioSafely(dcTracker);
            }
        }
    };

    private ContentObserver mAutoTimeObserver = new ContentObserver(new Handler()) {
        //@Override
        CARAPI OnChange(Boolean selfChange) {
            Rlog->I("GsmServiceStateTracker", "Auto time state changed");
            RevertToNitzTime();
        }
    };

    private ContentObserver mAutoTimeZoneObserver = new ContentObserver(new Handler()) {
        //@Override
        CARAPI OnChange(Boolean selfChange) {
            Rlog->I("GsmServiceStateTracker", "Auto time zone state changed");
            RevertToNitzTimeZone();
        }
    };

    public GsmServiceStateTracker(GSMPhone phone) {
        Super(phone, phone.mCi, new CellInfoGsm());

        mPhone = phone;
        mCellLoc = new GsmCellLocation();
        mNewCellLoc = new GsmCellLocation();

        PowerManager powerManager =
                (PowerManager)phone->GetContext()->GetSystemService(Context.POWER_SERVICE);
        mWakeLock = powerManager->NewWakeLock(PowerManager.PARTIAL_WAKE_LOCK, WAKELOCK_TAG);

        mCi->RegisterForAvailable(this, EVENT_RADIO_AVAILABLE, NULL);
        mCi->RegisterForRadioStateChanged(this, EVENT_RADIO_STATE_CHANGED, NULL);

        mCi->RegisterForVoiceNetworkStateChanged(this, EVENT_NETWORK_STATE_CHANGED, NULL);
        mCi->SetOnNITZTime(this, EVENT_NITZ_TIME, NULL);
        mCi->SetOnRestrictedStateChanged(this, EVENT_RESTRICTED_STATE_CHANGED, NULL);

        // system setting property AIRPLANE_MODE_ON is set in Settings.
        Int32 airplaneMode = Settings::Global::>GetInt(
                phone->GetContext()->GetContentResolver(),
                Settings::Global::AIRPLANE_MODE_ON, 0);
        mDesiredPowerState = ! (airplaneMode > 0);

        mCr = phone->GetContext()->GetContentResolver();
        mCr->RegisterContentObserver(
                Settings::Global::>GetUriFor(Settings::Global::AUTO_TIME), TRUE,
                mAutoTimeObserver);
        mCr->RegisterContentObserver(
                Settings::Global::>GetUriFor(Settings::Global::AUTO_TIME_ZONE), TRUE,
                mAutoTimeZoneObserver);

        SetSignalStrengthDefaultValues();

        // Monitor locale change
        IntentFilter filter = new IntentFilter();
        filter->AddAction(IIntent::ACTION_LOCALE_CHANGED);
        phone->GetContext()->RegisterReceiver(mIntentReceiver, filter);

        filter = new IntentFilter();
        Context context = phone->GetContext();
        filter->AddAction(ACTION_RADIO_OFF);
        context->RegisterReceiver(mIntentReceiver, filter);
    }

    //@Override
    CARAPI Dispose() {
        CheckCorrectThread();
        Log("ServiceStateTracker dispose");

        // Unregister for all events.
        mCi->UnregisterForAvailable(this);
        mCi->UnregisterForRadioStateChanged(this);
        mCi->UnregisterForVoiceNetworkStateChanged(this);
        If (mUiccApplcation != NULL) {mUiccApplcation->UnregisterForReady(this);}
        If (mIccRecords != NULL) {mIccRecords->UnregisterForRecordsLoaded(this);}
        mCi->UnSetOnRestrictedStateChanged(this);
        mCi->UnSetOnNITZTime(this);
        mCr->UnregisterContentObserver(mAutoTimeObserver);
        mCr->UnregisterContentObserver(mAutoTimeZoneObserver);
        mPhone->GetContext()->UnregisterReceiver(mIntentReceiver);
        super->Dispose();
    }

    //@Override
    protected void Finalize() {
        If(DBG) Log("finalize");
    }

    //@Override
    protected Phone GetPhone() {
        return mPhone;
    }

    //@Override
    CARAPI HandleMessage (Message msg) {
        AsyncResult ar;
        Int32[] ints;
        String[] strings;
        Message message;

        If (!mPhone.mIsTheCurrentActivePhone) {
            Rlog->E(LOG_TAG, "Received message " + msg +
                    "[" + msg.what + "] while being destroyed. Ignoring.");
            return;
        }
        Switch (msg.what) {
            case EVENT_RADIO_AVAILABLE:
                //this is unnecessary
                //SetPowerStateToDesired();
                break;

            case EVENT_SIM_READY:
                // Set the network type, in case the radio does not restore it.
                Int32 networkType = PhoneFactory->CalculatePreferredNetworkType(mPhone->GetContext(),
                        mPhone->GetPhoneId());
                mCi->SetPreferredNetworkType(networkType, NULL);

                Boolean skipRestoringSelection = mPhone->GetContext()->GetResources().GetBoolean(
                        R.bool.skip_restoring_network_selection);

                If (!skipRestoringSelection) {
                    // restore the previous network selection.
                    mPhone->RestoreSavedNetworkSelection(NULL);
                }
                PollState();
                // Signal strength polling stops when radio is off
                QueueNextSignalStrengthPoll();
                break;

            case EVENT_RADIO_STATE_CHANGED:
                // This will do nothing in the radio not
                // available case
                SetPowerStateToDesired();
                PollState();
                break;

            case EVENT_NETWORK_STATE_CHANGED:
                PollState();
                break;

            case EVENT_GET_SIGNAL_STRENGTH:
                // This callback is called when signal strength is polled
                // all by itself

                If (!(mCi->GetRadioState()->IsOn())) {
                    // Polling will continue when radio turns back on
                    return;
                }
                ar = (AsyncResult) msg.obj;
                OnSignalStrengthResult(ar, TRUE);
                QueueNextSignalStrengthPoll();

                break;

            case EVENT_GET_LOC_DONE:
                ar = (AsyncResult) msg.obj;

                If (ar.exception == NULL) {
                    String states[] = (String[])ar.result;
                    Int32 lac = -1;
                    Int32 cid = -1;
                    If (states.length >= 3) {
                        try {
                            If (states[1] != NULL && states[1].Length() > 0) {
                                lac = Integer->ParseInt(states[1], 16);
                            }
                            If (states[2] != NULL && states[2].Length() > 0) {
                                cid = Integer->ParseInt(states[2], 16);
                            }
                        } Catch (NumberFormatException ex) {
                            Rlog->W(LOG_TAG, "error parsing location: " + ex);
                        }
                    }
                    mCellLoc->SetLacAndCid(lac, cid);
                    mPhone->NotifyLocationChanged();
                }

                // Release any temporary cell lock, which could have been
                // acquired to allow a single-shot location update.
                DisableSingleLocationUpdate();
                break;

            case EVENT_POLL_STATE_REGISTRATION:
            case EVENT_POLL_STATE_GPRS:
            case EVENT_POLL_STATE_OPERATOR:
            case EVENT_POLL_STATE_NETWORK_SELECTION_MODE:
                ar = (AsyncResult) msg.obj;

                HandlePollStateResult(msg.what, ar);
                break;

            case EVENT_POLL_SIGNAL_STRENGTH:
                // Just poll signal strength...not part of PollState()

                mCi->GetSignalStrength(ObtainMessage(EVENT_GET_SIGNAL_STRENGTH));
                break;

            case EVENT_NITZ_TIME:
                ar = (AsyncResult) msg.obj;

                String nitzString = (String)((Object[])ar.result)[0];
                Int64 nitzReceiveTime = ((Long)((Object[])ar.result)[1]).LongValue();

                SetTimeFromNITZString(nitzString, nitzReceiveTime);
                break;

            case EVENT_SIGNAL_STRENGTH_UPDATE:
                // This is a notification from
                // CommandsInterface.setOnSignalStrengthUpdate

                ar = (AsyncResult) msg.obj;

                // The radio is telling us about signal strength changes
                // we don't have to ask it
                mDontPollSignalStrength = TRUE;

                OnSignalStrengthResult(ar, TRUE);
                break;

            case EVENT_SIM_RECORDS_LOADED:
                Log("EVENT_SIM_RECORDS_LOADED: what=" + msg.what);
                // Gsm doesn't support OTASP so its not needed
                mPhone->NotifyOtaspChanged(OTASP_NOT_NEEDED);

                UpdatePhoneObject();
                UpdateSpnDisplay();
                break;

            case EVENT_LOCATION_UPDATES_ENABLED:
                ar = (AsyncResult) msg.obj;

                If (ar.exception == NULL) {
                    mCi->GetVoiceRegistrationState(ObtainMessage(EVENT_GET_LOC_DONE, NULL));
                }
                break;

            case EVENT_SET_PREFERRED_NETWORK_TYPE:
                ar = (AsyncResult) msg.obj;
                // Don't care the result, only use for dereg Network (COPS=2)
                message = ObtainMessage(EVENT_RESET_PREFERRED_NETWORK_TYPE, ar.userObj);
                mCi->SetPreferredNetworkType(mPreferredNetworkType, message);
                break;

            case EVENT_RESET_PREFERRED_NETWORK_TYPE:
                ar = (AsyncResult) msg.obj;
                If (ar.userObj != NULL) {
                    AsyncResult->ForMessage(((Message) ar.userObj)).exception
                            = ar.exception;
                    ((Message) ar.userObj).SendToTarget();
                }
                break;

            case EVENT_GET_PREFERRED_NETWORK_TYPE:
                ar = (AsyncResult) msg.obj;

                If (ar.exception == NULL) {
                    mPreferredNetworkType = ((Int32[])ar.result)[0];
                } else {
                    mPreferredNetworkType = RILConstants.NETWORK_MODE_GLOBAL;
                }

                message = ObtainMessage(EVENT_SET_PREFERRED_NETWORK_TYPE, ar.userObj);
                Int32 toggledNetworkType = RILConstants.NETWORK_MODE_GLOBAL;

                mCi->SetPreferredNetworkType(toggledNetworkType, message);
                break;

            case EVENT_CHECK_REPORT_GPRS:
                If (mSS != NULL && !IsGprsConsistent(mSS->GetDataRegState(), mSS->GetVoiceRegState())) {

                    // Can't register data service while voice service is ok
                    // i.e. CREG is ok while CGREG is not
                    // possible a network or baseband side error
                    GsmCellLocation loc = ((GsmCellLocation)mPhone->GetCellLocation());
                    EventLog->WriteEvent(EventLogTags.DATA_NETWORK_REGISTRATION_FAIL,
                            mSS->GetOperatorNumeric(), loc != NULL ? loc->GetCid() : -1);
                    mReportedGprsNoReg = TRUE;
                }
                mStartedGprsRegCheck = FALSE;
                break;

            case EVENT_RESTRICTED_STATE_CHANGED:
                // This is a notification from
                // CommandsInterface.setOnRestrictedStateChanged

                If (DBG) Log("EVENT_RESTRICTED_STATE_CHANGED");

                ar = (AsyncResult) msg.obj;

                OnRestrictedStateChanged(ar);
                break;

            case EVENT_ALL_DATA_DISCONNECTED:
                Int64 dds = SubscriptionManager->GetDefaultDataSubId();
                ProxyController->GetInstance()->UnregisterForAllDataDisconnected(dds, this);
                Synchronized(this) {
                    If (mPendingRadioPowerOffAfterDataOff) {
                        If (DBG) Log("EVENT_ALL_DATA_DISCONNECTED, turn radio off now.");
                        HangupAndPowerOff();
                        mPendingRadioPowerOffAfterDataOff = FALSE;
                    } else {
                        Log("EVENT_ALL_DATA_DISCONNECTED is stale");
                    }
                }
                break;

            case EVENT_CHANGE_IMS_STATE:
                If (DBG) Log("EVENT_CHANGE_IMS_STATE:");

                SetPowerStateToDesired();
                break;

            default:
                super->HandleMessage(msg);
            break;
        }
    }

    //@Override
    protected void SetPowerStateToDesired() {

        If (DBG) {
            Log("mDeviceShuttingDown = " + mDeviceShuttingDown);
            Log("mDesiredPowerState = " + mDesiredPowerState);
            Log("getRadioState = " + mCi->GetRadioState());
            Log("mPowerOffDelayNeed = " + mPowerOffDelayNeed);
            Log("mAlarmSwitch = " + mAlarmSwitch);
        }

        If (mAlarmSwitch) {
            If(DBG) Log("mAlarmSwitch == TRUE");
            Context context = mPhone->GetContext();
            AlarmManager am = (AlarmManager) context->GetSystemService(Context.ALARM_SERVICE);
            am->Cancel(mRadioOffIntent);
            mAlarmSwitch = FALSE;
        }

        // If we want it on and it's off, turn it on
        If (mDesiredPowerState
                && mCi->GetRadioState() == CommandsInterface.RadioState.RADIO_OFF) {
            mCi->SetRadioPower(TRUE, NULL);
        } else If (!mDesiredPowerState && mCi->GetRadioState()->IsOn()) {
            // If it's on and available and we want it off gracefully
            If (mPowerOffDelayNeed) {
                If (mImsRegistrationOnOff && !mAlarmSwitch) {
                    If(DBG) Log("mImsRegistrationOnOff == TRUE");
                    Context context = mPhone->GetContext();
                    AlarmManager am = (AlarmManager) context->GetSystemService(Context.ALARM_SERVICE);

                    Intent intent = new IIntent::ACTION_RADIO_OFF);
                    mRadioOffIntent = PendingIntent->GetBroadcast(context, 0, intent, 0);

                    mAlarmSwitch = TRUE;
                    If (DBG) Log("Alarm setting");
                    am->Set(AlarmManager.ELAPSED_REALTIME_WAKEUP,
                            SystemClock->ElapsedRealtime() + 3000, mRadioOffIntent);
                } else {
                    DcTrackerBase dcTracker = mPhone.mDcTracker;
                    PowerOffRadioSafely(dcTracker);
                }
            } else {
                DcTrackerBase dcTracker = mPhone.mDcTracker;
                PowerOffRadioSafely(dcTracker);
            }
        } else If (mDeviceShuttingDown && mCi->GetRadioState()->IsAvailable()) {
            mCi->RequestShutdown(NULL);
        }
    }

    //@Override
    protected void HangupAndPowerOff() {
        // hang up all active voice calls
        If (mPhone->IsInCall()) {
            mPhone.mCT.mRingingCall->HangupIfAlive();
            mPhone.mCT.mBackgroundCall->HangupIfAlive();
            mPhone.mCT.mForegroundCall->HangupIfAlive();
        }

        mCi->SetRadioPower(FALSE, NULL);
    }

    //@Override
    protected void UpdateSpnDisplay() {
        // The values of plmn/showPlmn change in different scenarios.
        // 1) No service but emergency call allowed -> expected
        //    to show "Emergency call only"
        //    EXTRA_SHOW_PLMN = TRUE
        //    EXTRA_PLMN = "Emergency call only"

        // 2) No service at all --> expected to show "No service"
        //    EXTRA_SHOW_PLMN = TRUE
        //    EXTRA_PLMN = "No service"

        // 3) Normal operation in either home or roaming service
        //    EXTRA_SHOW_PLMN = depending on IccRecords rule
        //    EXTRA_PLMN = plmn

        // 4) No service due to power off, aka airplane mode
        //    EXTRA_SHOW_PLMN = FALSE
        //    EXTRA_PLMN = NULL

        IccRecords iccRecords = mIccRecords;
        String plmn = NULL;
        Boolean showPlmn = FALSE;
        Int32 rule = (iccRecords != NULL) ? iccRecords->GetDisplayRule(mSS->GetOperatorNumeric()) : 0;
        Int32 combinedRegState = GetCombinedRegState();
        If (combinedRegState == ServiceState.STATE_OUT_OF_SERVICE
                || combinedRegState == ServiceState.STATE_EMERGENCY_ONLY) {
            showPlmn = TRUE;
            If (mEmergencyOnly) {
                // No service but emergency call allowed
                plmn = Resources->GetSystem().
                        GetText(R::string::emergency_calls_only).ToString();
            } else {
                // No service at all
                plmn = Resources->GetSystem().
                        GetText(R::string::lockscreen_carrier_default).ToString();
            }
            If (DBG) Log("updateSpnDisplay: radio is on but out " +
                    "of service, set plmn='" + plmn + "'");
        } else If (combinedRegState == ServiceState.STATE_IN_SERVICE) {
            // In either home or roaming service
            plmn = mSS->GetOperatorAlphaLong();
            showPlmn = !TextUtils->IsEmpty(plmn) &&
                    ((rule & SIMRecords.SPN_RULE_SHOW_PLMN)
                            == SIMRecords.SPN_RULE_SHOW_PLMN);
        } else {
            // Power off state, such as airplane mode
            If (DBG) Log("updateSpnDisplay: radio is off w/ showPlmn="
                    + showPlmn + " plmn=" + plmn);
        }

        // The value of spn/showSpn are same in different scenarios.
        //    EXTRA_SHOW_SPN = depending on IccRecords rule
        //    EXTRA_SPN = spn
        String spn = (iccRecords != NULL) ? iccRecords->GetServiceProviderName() : "";
        Boolean showSpn = !TextUtils->IsEmpty(spn)
                && ((rule & SIMRecords.SPN_RULE_SHOW_SPN)
                        == SIMRecords.SPN_RULE_SHOW_SPN);

        // Update SPN_STRINGS_UPDATED_ACTION IFF any value changes
        If (showPlmn != mCurShowPlmn
                || showSpn != mCurShowSpn
                || !TextUtils->Equals(spn, mCurSpn)
                || !TextUtils->Equals(plmn, mCurPlmn)) {
            If (DBG) {
                Log(String->Format("updateSpnDisplay: changed" +
                        " sending intent rule=" + rule +
                        " showPlmn='%b' plmn='%s' showSpn='%b' spn='%s'",
                        showPlmn, plmn, showSpn, spn));
            }
            Intent intent = new Intent(TelephonyIntents.SPN_STRINGS_UPDATED_ACTION);
            intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
            intent->PutExtra(TelephonyIntents.EXTRA_SHOW_SPN, showSpn);
            intent->PutExtra(TelephonyIntents.EXTRA_SPN, spn);
            intent->PutExtra(TelephonyIntents.EXTRA_SHOW_PLMN, showPlmn);
            intent->PutExtra(TelephonyIntents.EXTRA_PLMN, plmn);
            SubscriptionManager->PutPhoneIdAndSubIdExtra(intent, mPhone->GetPhoneId());
            mPhone->GetContext()->SendStickyBroadcastAsUser(intent, UserHandle.ALL);
        }

        mCurShowSpn = showSpn;
        mCurShowPlmn = showPlmn;
        mCurSpn = spn;
        mCurPlmn = plmn;
    }

    /**
     * Consider dataRegState if voiceRegState is OOS to determine SPN to be
     * displayed
     */
    private Int32 GetCombinedRegState() {
        Int32 regState = mSS->GetVoiceRegState();
        Int32 dataRegState = mSS->GetDataRegState();

        If ((regState == ServiceState.STATE_OUT_OF_SERVICE)
                && (dataRegState == ServiceState.STATE_IN_SERVICE)) {
            Log("getCombinedRegState: return STATE_IN_SERVICE as Data is in service");
            regState = dataRegState;
        }

        return regState;
    }

    /**
     * Handle the result of one of the PollState()-related requests
     */
    //@Override
    protected void HandlePollStateResult (Int32 what, AsyncResult ar) {
        Int32 ints[];
        String states[];

        // Ignore stale requests from last poll
        If (ar.userObj != mPollingContext) return;

        If (ar.exception != NULL) {
            CommandException.Error err=NULL;

            If (ar.exception instanceof CommandException) {
                err = ((CommandException)(ar.exception)).GetCommandError();
            }

            If (err == CommandException.Error.RADIO_NOT_AVAILABLE) {
                // Radio has crashed or turned off
                CancelPollState();
                return;
            }

            If (err != CommandException.Error.OP_NOT_ALLOWED_BEFORE_REG_NW) {
                Loge("RIL implementation has returned an error where it must succeed" +
                        ar.exception);
            }
        } else try {
            Switch (what) {
                case EVENT_POLL_STATE_REGISTRATION: {
                    states = (String[])ar.result;
                    Int32 lac = -1;
                    Int32 cid = -1;
                    Int32 type = ServiceState.RIL_RADIO_TECHNOLOGY_UNKNOWN;
                    Int32 regState = ServiceState.RIL_REG_STATE_UNKNOWN;
                    Int32 reasonRegStateDenied = -1;
                    Int32 psc = -1;
                    Int32 cssIndicator = 0;
                    If (states.length > 0) {
                        try {
                            regState = Integer->ParseInt(states[0]);
                            If (states.length >= 3) {
                                If (states[1] != NULL && states[1].Length() > 0) {
                                    lac = Integer->ParseInt(states[1], 16);
                                }
                                If (states[2] != NULL && states[2].Length() > 0) {
                                    cid = Integer->ParseInt(states[2], 16);
                                }

                                // states[3] (if present) is the current radio technology
                                If (states.length >= 4 && states[3] != NULL) {
                                    type = Integer->ParseInt(states[3]);
                                }
                            }
                            If (states.length >= 7 && (states[7] != NULL)) {
                                cssIndicator = Integer->ParseInt(states[7]);
                            }
                            If (states.length > 14) {
                                If (states[14] != NULL && states[14].Length() > 0) {
                                    psc = Integer->ParseInt(states[14], 16);
                                }
                            }
                        } Catch (NumberFormatException ex) {
                            Loge("error parsing RegistrationState: " + ex);
                        }
                    }

                    mGsmRoaming = RegCodeIsRoaming(regState);
                    mNewSS->SetState(RegCodeToServiceState(regState));
                    mNewSS->SetRilVoiceRadioTechnology(type);
                    mNewSS->SetCssIndicator(cssIndicator);

                    If ((regState == ServiceState.RIL_REG_STATE_DENIED
                            || regState == ServiceState.RIL_REG_STATE_DENIED_EMERGENCY_CALL_ENABLED)
                            && (states.length >= 14)) {
                        try {
                            Int32 rejCode = Integer->ParseInt(states[13]);
                            // Check if rejCode is "Persistent location update reject",
                            If (rejCode == 10) {
                                Log(" Posting Managed roaming intent sub = "
                                        + mPhone->GetSubId());
                                Intent intent =
                                        new Intent(TelephonyIntents.ACTION_MANAGED_ROAMING_IND);
                                intent->PutExtra(PhoneConstants.SUBSCRIPTION_KEY,
                                        mPhone->GetSubId());
                                mPhone->GetContext()->SendBroadcast(intent);
                            }
                        } Catch (NumberFormatException ex) {
                            Loge("error parsing regCode: " + ex);
                        }
                    }

                    Boolean isVoiceCapable = mPhoneBase->GetContext()->GetResources()
                            .GetBoolean(R.bool.config_voice_capable);
                    If ((regState == ServiceState.RIL_REG_STATE_DENIED_EMERGENCY_CALL_ENABLED
                         || regState == ServiceState.RIL_REG_STATE_NOT_REG_EMERGENCY_CALL_ENABLED
                         || regState == ServiceState.RIL_REG_STATE_SEARCHING_EMERGENCY_CALL_ENABLED
                         || regState == ServiceState.RIL_REG_STATE_UNKNOWN_EMERGENCY_CALL_ENABLED)
                         && isVoiceCapable) {
                        mEmergencyOnly = TRUE;
                    } else {
                        mEmergencyOnly = FALSE;
                    }

                    // LAC and CID are -1 if not avail
                    mNewCellLoc->SetLacAndCid(lac, cid);
                    mNewCellLoc->SetPsc(psc);
                    break;
                }

                case EVENT_POLL_STATE_GPRS: {
                    states = (String[])ar.result;

                    Int32 type = 0;
                    Int32 regState = ServiceState.RIL_REG_STATE_UNKNOWN;
                    mNewReasonDataDenied = -1;
                    mNewMaxDataCalls = 1;
                    If (states.length > 0) {
                        try {
                            regState = Integer->ParseInt(states[0]);

                            // states[3] (if present) is the current radio technology
                            If (states.length >= 4 && states[3] != NULL) {
                                type = Integer->ParseInt(states[3]);
                            }
                            If ((states.length >= 5 ) &&
                                    (regState == ServiceState.RIL_REG_STATE_DENIED)) {
                                mNewReasonDataDenied = Integer->ParseInt(states[4]);
                            }
                            If (states.length >= 6) {
                                mNewMaxDataCalls = Integer->ParseInt(states[5]);
                            }
                        } Catch (NumberFormatException ex) {
                            Loge("error parsing GprsRegistrationState: " + ex);
                        }
                    }
                    Int32 dataRegState = RegCodeToServiceState(regState);
                    mNewSS->SetDataRegState(dataRegState);
                    mDataRoaming = RegCodeIsRoaming(regState);
                    mNewSS->SetRilDataRadioTechnology(type);
                    If (DBG) {
                        Log("handlPollStateResultMessage: GsmSST setDataRegState=" + dataRegState
                                + " regState=" + regState
                                + " dataRadioTechnology=" + type);
                    }
                    break;
                }

                case EVENT_POLL_STATE_OPERATOR: {
                    String opNames[] = (String[])ar.result;

                    If (opNames != NULL && opNames.length >= 3) {
                        String brandOverride = mUiccController->GetUiccCard() != NULL ?
                            mUiccController->GetUiccCard()->GetOperatorBrandOverride() : NULL;
                        If (brandOverride != NULL) {
                            mNewSS->SetOperatorName(brandOverride, brandOverride, opNames[2]);
                        } else {
                            mNewSS->SetOperatorName(opNames[0], opNames[1], opNames[2]);
                        }
                    }
                    break;
                }

                case EVENT_POLL_STATE_NETWORK_SELECTION_MODE: {
                    ints = (Int32[])ar.result;
                    mNewSS->SetIsManualSelection(ints[0] == 1);
                    If ((ints[0] == 1) && (!mPhone->IsManualNetSelAllowed())) {
                        /*
                         * modem is currently in manual selection but manual
                         * selection is not allowed in the current mode so
                         * switch to automatic registration
                         */
                        mPhone.SetNetworkSelectionModeAutomatic (NULL);
                        Log(" Forcing Automatic Network Selection, " +
                                "manual selection is not allowed");
                    }
                    break;
                }
            }

        } Catch (RuntimeException ex) {
            Loge("Exception while polling service state. Probably malformed RIL response." + ex);
        }

        mPollingContext[0]--;

        If (mPollingContext[0] == 0) {
            /**
             * Since the roaming state of gsm Service (from +CREG) and
             * data Service (from +CGREG) could be different, the new SS
             * is set to roaming when either is TRUE.
             *
             * There are exceptions for the above rule.
             * The new SS is not set as roaming while gsm service reports
             * roaming but indeed it is same operator.
             * And the operator is considered non roaming.
             *
             * The test for the operators is to handle special roaming
             * agreements and MVNO's.
             */
            Boolean roaming = (mGsmRoaming || mDataRoaming);
            If (mGsmRoaming && !IsOperatorConsideredRoaming(mNewSS) &&
                (IsSameNamedOperators(mNewSS) || IsOperatorConsideredNonRoaming(mNewSS))) {
                roaming = FALSE;
            }
            mNewSS->SetRoaming(roaming);
            mNewSS->SetEmergencyOnly(mEmergencyOnly);
            PollStateDone();
        }
    }

    private void SetSignalStrengthDefaultValues() {
        mSignalStrength = new SignalStrength(TRUE);
    }

    /**
     * A complete "service state" from our perspective is
     * composed of a handful of separate requests to the radio.
     *
     * We make all of these requests at once, but then abandon them
     * and start over again if the radio notifies us that some
     * event has changed
     */
    //@Override
    CARAPI PollState() {
        mPollingContext = new Int32[1];
        mPollingContext[0] = 0;

        Switch (mCi->GetRadioState()) {
            case RADIO_UNAVAILABLE:
                mNewSS->SetStateOutOfService();
                mNewCellLoc->SetStateInvalid();
                SetSignalStrengthDefaultValues();
                mGotCountryCode = FALSE;
                mNitzUpdatedTime = FALSE;
                PollStateDone();
            break;

            case RADIO_OFF:
                mNewSS->SetStateOff();
                mNewCellLoc->SetStateInvalid();
                SetSignalStrengthDefaultValues();
                mGotCountryCode = FALSE;
                mNitzUpdatedTime = FALSE;
                PollStateDone();

                /**
                 * If iwlan feature is enabled then we do get
                 * voice_network_change indication from RIL. At this moment we
                 * dont know the current RAT since we are in Airplane mode.
                 * We have to request for current registration state and hence
                 * fallthrough to default case only if iwlan feature is
                 * applicable.
                 */
                If (!IsIwlanFeatureAvailable()) {
                    /* fall-through */
                    break;
                }

            default:
                // Issue all poll-related commands at once
                // then count down the responses, which
                // are allowed to arrive out-of-order

                mPollingContext[0]++;
                mCi->GetOperator(
                    ObtainMessage(
                        EVENT_POLL_STATE_OPERATOR, mPollingContext));

                mPollingContext[0]++;
                mCi->GetDataRegistrationState(
                    ObtainMessage(
                        EVENT_POLL_STATE_GPRS, mPollingContext));

                mPollingContext[0]++;
                mCi->GetVoiceRegistrationState(
                    ObtainMessage(
                        EVENT_POLL_STATE_REGISTRATION, mPollingContext));

                mPollingContext[0]++;
                mCi->GetNetworkSelectionMode(
                    ObtainMessage(
                        EVENT_POLL_STATE_NETWORK_SELECTION_MODE, mPollingContext));
            break;
        }
    }

    private void PollStateDone() {
        If (DBG) {
            Log("Poll ServiceState done: " +
                " oldSS=[" + mSS + "] newSS=[" + mNewSS + "]" +
                " oldMaxDataCalls=" + mMaxDataCalls +
                " mNewMaxDataCalls=" + mNewMaxDataCalls +
                " oldReasonDataDenied=" + mReasonDataDenied +
                " mNewReasonDataDenied=" + mNewReasonDataDenied);
        }

        If (Build.IS_DEBUGGABLE && SystemProperties->GetBoolean(PROP_FORCE_ROAMING, FALSE)) {
            mNewSS->SetRoaming(TRUE);
        }

        UseDataRegStateForDataOnlyDevices();

        Boolean hasRegistered =
            mSS->GetVoiceRegState() != ServiceState.STATE_IN_SERVICE
            && mNewSS->GetVoiceRegState() == ServiceState.STATE_IN_SERVICE;

        Boolean hasDeregistered =
            mSS->GetVoiceRegState() == ServiceState.STATE_IN_SERVICE
            && mNewSS->GetVoiceRegState() != ServiceState.STATE_IN_SERVICE;

        Boolean hasGprsAttached =
                mSS->GetDataRegState() != ServiceState.STATE_IN_SERVICE
                && mNewSS->GetDataRegState() == ServiceState.STATE_IN_SERVICE;

        Boolean hasGprsDetached =
                mSS->GetDataRegState() == ServiceState.STATE_IN_SERVICE
                && mNewSS->GetDataRegState() != ServiceState.STATE_IN_SERVICE;

        Boolean hasDataRegStateChanged =
                mSS->GetDataRegState() != mNewSS->GetDataRegState();

        Boolean hasVoiceRegStateChanged =
                mSS->GetVoiceRegState() != mNewSS->GetVoiceRegState();

        Boolean hasRilVoiceRadioTechnologyChanged =
                mSS->GetRilVoiceRadioTechnology() != mNewSS->GetRilVoiceRadioTechnology();

        Boolean hasRilDataRadioTechnologyChanged =
                mSS->GetRilDataRadioTechnology() != mNewSS->GetRilDataRadioTechnology();

        Boolean hasChanged = !mNewSS->Equals(mSS);

        Boolean hasRoamingOn = !mSS->GetRoaming() && mNewSS->GetRoaming();

        Boolean hasRoamingOff = mSS->GetRoaming() && !mNewSS->GetRoaming();

        Boolean hasLocationChanged = !mNewCellLoc->Equals(mCellLoc);

        Boolean needNotifyData = (mSS->GetCssIndicator() != mNewSS->GetCssIndicator());

        If (mCi->GetRadioState() == CommandsInterface.RadioState.RADIO_OFF) {
            Boolean resetIwlanRatVal = FALSE;
            Log("set service state as POWER_OFF");
            If (IsIwlanFeatureAvailable()
                    && (ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN
                        == mNewSS->GetRilDataRadioTechnology())) {
                Log("pollStateDone: mNewSS = " + mNewSS);
                Log("pollStateDone: reset iwlan RAT value");
                resetIwlanRatVal = TRUE;
            }
            mNewSS->SetStateOff();
            If (resetIwlanRatVal) {
                mNewSS->SetRilDataRadioTechnology(ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN);
                Log("pollStateDone: mNewSS = " + mNewSS);
                resetIwlanRatVal = FALSE;
            }
        }

        // Add an event log when connection state changes
        If (hasVoiceRegStateChanged || hasDataRegStateChanged) {
            EventLog->WriteEvent(EventLogTags.GSM_SERVICE_STATE_CHANGE,
                mSS->GetVoiceRegState(), mSS->GetDataRegState(),
                mNewSS->GetVoiceRegState(), mNewSS->GetDataRegState());
        }

        // Add an event log when network type switched
        // TODO: we may add filtering to reduce the event logged,
        // i.e. check preferred network setting, only switch to 2G, etc
        If (hasRilVoiceRadioTechnologyChanged) {
            Int32 cid = -1;
            GsmCellLocation loc = mNewCellLoc;
            If (loc != NULL) cid = loc->GetCid();
            // NOTE: this code was previously located after mSS and mNewSS are swapped, so
            // existing logs were incorrectly using the new state for "network_from"
            // and STATE_OUT_OF_SERVICE for "network_to". To avoid confusion, use a new log tag
            // to record the correct states.
            EventLog->WriteEvent(EventLogTags.GSM_RAT_SWITCHED_NEW, cid,
                    mSS->GetRilVoiceRadioTechnology(),
                    mNewSS->GetRilVoiceRadioTechnology());
            If (DBG) {
                Log("RAT switched "
                        + ServiceState->RilRadioTechnologyToString(mSS->GetRilVoiceRadioTechnology())
                        + " -> "
                        + ServiceState->RilRadioTechnologyToString(
                                mNewSS->GetRilVoiceRadioTechnology()) + " at cell " + cid);
            }
        }

        // swap mSS and mNewSS to put new state in mSS
        ServiceState tss = mSS;
        mSS = mNewSS;
        mNewSS = tss;
        // clean slate for next time
        mNewSS->SetStateOutOfService();

        // swap mCellLoc and mNewCellLoc to put new state in mCellLoc
        GsmCellLocation tcl = mCellLoc;
        mCellLoc = mNewCellLoc;
        mNewCellLoc = tcl;

        mReasonDataDenied = mNewReasonDataDenied;
        mMaxDataCalls = mNewMaxDataCalls;

        If (hasRilVoiceRadioTechnologyChanged) {
            UpdatePhoneObject();
        }

        If (hasRilDataRadioTechnologyChanged) {
            mPhone->SetSystemProperty(TelephonyProperties.PROPERTY_DATA_NETWORK_TYPE,
                    ServiceState->RilRadioTechnologyToString(mSS->GetRilVoiceRadioTechnology()));

            If (IsIwlanFeatureAvailable()
                    && (ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN
                        == mSS->GetRilDataRadioTechnology())) {
                Log("pollStateDone: IWLAN enabled");
            }

        }

        If (hasRegistered) {
            mNetworkAttachedRegistrants->NotifyRegistrants();

            If (DBG) {
                Log("pollStateDone: registering current mNitzUpdatedTime=" +
                        mNitzUpdatedTime + " changing to FALSE");
            }
            mNitzUpdatedTime = FALSE;
        }

        If (hasChanged) {
            String operatorNumeric;

            UpdateSpnDisplay();

            mPhone->SetSystemProperty(TelephonyProperties.PROPERTY_OPERATOR_ALPHA,
                mSS->GetOperatorAlphaLong());

            String prevOperatorNumeric =
                    SystemProperties->Get(TelephonyProperties.PROPERTY_OPERATOR_NUMERIC, "");
            operatorNumeric = mSS->GetOperatorNumeric();
            mPhone->SetSystemProperty(TelephonyProperties.PROPERTY_OPERATOR_NUMERIC, operatorNumeric);
            UpdateCarrierMccMncConfiguration(operatorNumeric,
                    prevOperatorNumeric, mPhone->GetContext());
            If (operatorNumeric == NULL) {
                If (DBG) Log("operatorNumeric is NULL");
                mPhone->SetSystemProperty(TelephonyProperties.PROPERTY_OPERATOR_ISO_COUNTRY, "");
                mGotCountryCode = FALSE;
                mNitzUpdatedTime = FALSE;
            } else {
                String iso = "";
                String mcc = "";
                try{
                    mcc = operatorNumeric->Substring(0, 3);
                    iso = MccTable->CountryCodeForMcc(Integer->ParseInt(mcc));
                } Catch ( NumberFormatException ex){
                    Loge("pollStateDone: countryCodeForMcc error" + ex);
                } Catch ( StringIndexOutOfBoundsException ex) {
                    Loge("pollStateDone: countryCodeForMcc error" + ex);
                }

                mPhone->SetSystemProperty(TelephonyProperties.PROPERTY_OPERATOR_ISO_COUNTRY, iso);
                mGotCountryCode = TRUE;

                TimeZone zone = NULL;

                If (!mNitzUpdatedTime && !mcc->Equals("000") && !TextUtils->IsEmpty(iso) &&
                        GetAutoTimeZone()) {

                    // Test both paths if ignore nitz is TRUE
                    Boolean testOneUniqueOffsetPath = SystemProperties->GetBoolean(
                                TelephonyProperties.PROPERTY_IGNORE_NITZ, FALSE) &&
                                    ((SystemClock->UptimeMillis() & 1) == 0);

                    ArrayList<TimeZone> uniqueZones = TimeUtils->GetTimeZonesWithUniqueOffsets(iso);
                    If ((uniqueZones->Size() == 1) || testOneUniqueOffsetPath) {
                        zone = uniqueZones->Get(0);
                        If (DBG) {
                           Log("pollStateDone: no nitz but one TZ for iso-cc=" + iso +
                                   " with zone.getID=" + zone->GetID() +
                                   " testOneUniqueOffsetPath=" + testOneUniqueOffsetPath);
                        }
                        SetAndBroadcastNetworkSetTimeZone(zone->GetID());
                    } else {
                        If (DBG) {
                            Log("pollStateDone: there are " + uniqueZones->Size() +
                                " unique offsets for iso-cc='" + iso +
                                " testOneUniqueOffsetPath=" + testOneUniqueOffsetPath +
                                "', do nothing");
                        }
                    }
                }

                If (ShouldFixTimeZoneNow(mPhone, operatorNumeric, prevOperatorNumeric,
                        mNeedFixZoneAfterNitz)) {
                    // If the offset Is (0, FALSE) and the timezone property
                    // is set, use the timezone property rather than
                    // GMT.
                    String zoneName = SystemProperties->Get(TIMEZONE_PROPERTY);
                    If (DBG) {
                        Log("pollStateDone: fix time zone zoneName='" + zoneName +
                            "' mZoneOffset=" + mZoneOffset + " mZoneDst=" + mZoneDst +
                            " iso-cc='" + iso +
                            "' iso-cc-idx=" + Arrays->BinarySearch(GMT_COUNTRY_CODES, iso));
                    }

                    If ("".Equals(iso) && mNeedFixZoneAfterNitz) {
                        // Country code not found.  This is likely a test network.
                        // Get a TimeZone based only on the NITZ Parameters (best guess).
                        zone = GetNitzTimeZone(mZoneOffset, mZoneDst, mZoneTime);
                        If (DBG) Log("pollStateDone: using NITZ TimeZone");
                    } else
                    // "(mZoneOffset == 0) && (mZoneDst == FALSE) &&
                    //  (Arrays->BinarySearch(GMT_COUNTRY_CODES, iso) < 0)"
                    // means that we received a NITZ string telling
                    // it is in GMT+0 w/ DST time zone
                    // BUT iso tells is NOT, e.g, a wrong NITZ reporting
                    // local time w/ 0 offset.
                    If ((mZoneOffset == 0) && (mZoneDst == FALSE) &&
                        (zoneName != NULL) && (zoneName->Length() > 0) &&
                        (Arrays->BinarySearch(GMT_COUNTRY_CODES, iso) < 0)) {
                        zone = TimeZone->GetDefault();
                        If (mNeedFixZoneAfterNitz) {
                            // For wrong NITZ reporting local time w/ 0 offset,
                            // need adjust time to reflect default timezone setting
                            Int64 ctm = System->CurrentTimeMillis();
                            Int64 tzOffset = zone->GetOffset(ctm);
                            If (DBG) {
                                Log("pollStateDone: tzOffset=" + tzOffset + " ltod=" +
                                        TimeUtils->LogTimeOfDay(ctm));
                            }
                            If (GetAutoTime()) {
                                Int64 adj = ctm - tzOffset;
                                If (DBG) Log("pollStateDone: adj ltod=" +
                                        TimeUtils->LogTimeOfDay(adj));
                                SetAndBroadcastNetworkSetTime(adj);
                            } else {
                                // Adjust the saved NITZ time to account for tzOffset.
                                mSavedTime = mSavedTime - tzOffset;
                            }
                        }
                        If (DBG) Log("pollStateDone: using default TimeZone");
                    } else {
                        zone = TimeUtils->GetTimeZone(mZoneOffset, mZoneDst, mZoneTime, iso);
                        If (DBG) Log("pollStateDone: using GetTimeZone(off, dst, time, iso)");
                    }

                    mNeedFixZoneAfterNitz = FALSE;

                    If (zone != NULL) {
                        Log("pollStateDone: zone != NULL zone.getID=" + zone->GetID());
                        If (GetAutoTimeZone()) {
                            SetAndBroadcastNetworkSetTimeZone(zone->GetID());
                        }
                        SaveNitzTimeZone(zone->GetID());
                    } else {
                        Log("pollStateDone: zone == NULL");
                    }
                }
            }

            mPhone->SetSystemProperty(TelephonyProperties.PROPERTY_OPERATOR_ISROAMING,
                mSS->GetRoaming() ? "TRUE" : "FALSE");

            mPhone->NotifyServiceStateChanged(mSS);
        }

        // First notify detached, then rat changed, then attached - that's the way it
        // happens in the modem.
        // Behavior of Recipients (DcTracker, for instance) depends on this sequence
        // since DcTracker reloads profiles on "rat_changed" notification and sets up
        // data call on "attached" notification.
        If (hasGprsDetached) {
            mDetachedRegistrants->NotifyRegistrants();
        }

        If (hasDataRegStateChanged || hasRilDataRadioTechnologyChanged) {
            NotifyDataRegStateRilRadioTechnologyChanged();

            If (IsIwlanFeatureAvailable()
                    && (ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN
                        == mSS->GetRilDataRadioTechnology())) {
                mPhone->NotifyDataConnection(Phone.REASON_IWLAN_AVAILABLE);
                needNotifyData =  FALSE;
                mIwlanRatAvailable = TRUE;
            } else {
                ProcessIwlanToWwanTransition(mSS);
                needNotifyData = TRUE;
                mIwlanRatAvailable = FALSE;
            }
        }

        If (needNotifyData) {
            mPhone->NotifyDataConnection(NULL);
        }

        If (hasGprsAttached) {
            mAttachedRegistrants->NotifyRegistrants();
        }

        If (hasRoamingOn) {
            mRoamingOnRegistrants->NotifyRegistrants();
        }

        If (hasRoamingOff) {
            mRoamingOffRegistrants->NotifyRegistrants();
        }

        If (hasLocationChanged) {
            mPhone->NotifyLocationChanged();
        }

        If (! IsGprsConsistent(mSS->GetDataRegState(), mSS->GetVoiceRegState())) {
            If (!mStartedGprsRegCheck && !mReportedGprsNoReg) {
                mStartedGprsRegCheck = TRUE;

                Int32 check_period = Settings::Global::>GetInt(
                        mPhone->GetContext()->GetContentResolver(),
                        Settings::Global::GPRS_REGISTER_CHECK_PERIOD_MS,
                        DEFAULT_GPRS_CHECK_PERIOD_MILLIS);
                SendMessageDelayed(ObtainMessage(EVENT_CHECK_REPORT_GPRS),
                        check_period);
            }
        } else {
            mReportedGprsNoReg = FALSE;
        }
        // TODO: Add GsmCellIdenity updating, see CdmaLteServiceStateTracker.
    }

    /**
     * Check if GPRS got registered while voice is registered.
     *
     * @param dataRegState i.e. CGREG in GSM
     * @param voiceRegState i.e. CREG in GSM
     * @return FALSE if device only register to voice but not gprs
     */
    private Boolean IsGprsConsistent(Int32 dataRegState, Int32 voiceRegState) {
        return !((voiceRegState == ServiceState.STATE_IN_SERVICE) &&
                (dataRegState != ServiceState.STATE_IN_SERVICE));
    }

    /**
     * Returns a TimeZone object based only on parameters from the NITZ string.
     */
    private TimeZone GetNitzTimeZone(Int32 offset, Boolean dst, Int64 when) {
        TimeZone guess = FindTimeZone(offset, dst, when);
        If (guess == NULL) {
            // Couldn't find a proper timezone.  Perhaps the DST data is wrong.
            guess = FindTimeZone(offset, !dst, when);
        }
        If (DBG) Log("getNitzTimeZone returning " + (guess == NULL ? guess : guess->GetID()));
        return guess;
    }

    private TimeZone FindTimeZone(Int32 offset, Boolean dst, Int64 when) {
        Int32 rawOffset = offset;
        If (dst) {
            rawOffset -= 3600000;
        }
        String[] zones = TimeZone->GetAvailableIDs(rawOffset);
        TimeZone guess = NULL;
        Date d = new Date(when);
        For (String zone : zones) {
            TimeZone tz = TimeZone->GetTimeZone(zone);
            If (tz->GetOffset(when) == offset &&
                tz->InDaylightTime(d) == dst) {
                guess = tz;
                break;
            }
        }

        return guess;
    }

    private void QueueNextSignalStrengthPoll() {
        If (mDontPollSignalStrength) {
            // The radio is telling us about signal strength changes
            // we don't have to ask it
            return;
        }

        Message msg;

        msg = ObtainMessage();
        msg.what = EVENT_POLL_SIGNAL_STRENGTH;

        Int64 nextTime;

        // TODO Don't poll signal strength if screen is off
        SendMessageDelayed(msg, POLL_PERIOD_MILLIS);
    }

    /**
     * Set restricted state based on the OnRestrictedStateChanged notification
     * If any voice or packet restricted state changes, trigger a UI
     * notification and notify registrants when sim is ready.
     *
     * @param ar an Int32 value of RIL_RESTRICTED_STATE_*
     */
    private void OnRestrictedStateChanged(AsyncResult ar) {
        RestrictedState newRs = new RestrictedState();

        If (DBG) Log("onRestrictedStateChanged: E rs "+ mRestrictedState);

        If (ar.exception == NULL) {
            Int32[] ints = (Int32[])ar.result;
            Int32 state = ints[0];

            newRs->SetCsEmergencyRestricted(
                    ((state & RILConstants.RIL_RESTRICTED_STATE_CS_EMERGENCY) != 0) ||
                    ((state & RILConstants.RIL_RESTRICTED_STATE_CS_ALL) != 0) );
            //ignore the normal call and data restricted state before SIM READY
            If (mUiccApplcation != NULL && mUiccApplcation->GetState() == AppState.APPSTATE_READY) {
                newRs->SetCsNormalRestricted(
                        ((state & RILConstants.RIL_RESTRICTED_STATE_CS_NORMAL) != 0) ||
                        ((state & RILConstants.RIL_RESTRICTED_STATE_CS_ALL) != 0) );
                newRs->SetPsRestricted(
                        (state & RILConstants.RIL_RESTRICTED_STATE_PS_ALL)!= 0);
            }

            If (DBG) Log("onRestrictedStateChanged: new rs "+ newRs);

            If (!mRestrictedState->IsPsRestricted() && newRs->IsPsRestricted()) {
                mPsRestrictEnabledRegistrants->NotifyRegistrants();
                SetNotification(PS_ENABLED);
            } else If (mRestrictedState->IsPsRestricted() && !newRs->IsPsRestricted()) {
                mPsRestrictDisabledRegistrants->NotifyRegistrants();
                SetNotification(PS_DISABLED);
            }

            /**
             * There are two kind of cs restriction, normal and emergency. So
             * there are 4 x 4 combinations in current and new restricted states
             * and we only need to notify when state is changed.
             */
            If (mRestrictedState->IsCsRestricted()) {
                If (!newRs->IsCsRestricted()) {
                    // remove all restriction
                    SetNotification(CS_DISABLED);
                } else If (!newRs->IsCsNormalRestricted()) {
                    // remove normal restriction
                    SetNotification(CS_EMERGENCY_ENABLED);
                } else If (!newRs->IsCsEmergencyRestricted()) {
                    // remove emergency restriction
                    SetNotification(CS_NORMAL_ENABLED);
                }
            } else If (mRestrictedState->IsCsEmergencyRestricted() &&
                    !mRestrictedState->IsCsNormalRestricted()) {
                If (!newRs->IsCsRestricted()) {
                    // remove all restriction
                    SetNotification(CS_DISABLED);
                } else If (newRs->IsCsRestricted()) {
                    // enable all restriction
                    SetNotification(CS_ENABLED);
                } else If (newRs->IsCsNormalRestricted()) {
                    // remove emergency restriction and enable normal restriction
                    SetNotification(CS_NORMAL_ENABLED);
                }
            } else If (!mRestrictedState->IsCsEmergencyRestricted() &&
                    mRestrictedState->IsCsNormalRestricted()) {
                If (!newRs->IsCsRestricted()) {
                    // remove all restriction
                    SetNotification(CS_DISABLED);
                } else If (newRs->IsCsRestricted()) {
                    // enable all restriction
                    SetNotification(CS_ENABLED);
                } else If (newRs->IsCsEmergencyRestricted()) {
                    // remove normal restriction and enable emergency restriction
                    SetNotification(CS_EMERGENCY_ENABLED);
                }
            } else {
                If (newRs->IsCsRestricted()) {
                    // enable all restriction
                    SetNotification(CS_ENABLED);
                } else If (newRs->IsCsEmergencyRestricted()) {
                    // enable emergency restriction
                    SetNotification(CS_EMERGENCY_ENABLED);
                } else If (newRs->IsCsNormalRestricted()) {
                    // enable normal restriction
                    SetNotification(CS_NORMAL_ENABLED);
                }
            }

            mRestrictedState = newRs;
        }
        Log("onRestrictedStateChanged: X rs "+ mRestrictedState);
    }

    /** code is registration state 0-5 from TS 27.007 7.2 */
    private Int32 RegCodeToServiceState(Int32 code) {
        Switch (code) {
            case 0:
            case 2: // 2 is "searching"
            case 3: // 3 is "registration denied"
            case 4: // 4 is "unknown" no vaild in current baseband
            case 10:// same as 0, but indicates that emergency call is possible.
            case 12:// same as 2, but indicates that emergency call is possible.
            case 13:// same as 3, but indicates that emergency call is possible.
            case 14:// same as 4, but indicates that emergency call is possible.
                return ServiceState.STATE_OUT_OF_SERVICE;

            case 1:
                return ServiceState.STATE_IN_SERVICE;

            case 5:
                // in service, roam
                return ServiceState.STATE_IN_SERVICE;

            default:
                Loge("regCodeToServiceState: unexpected service state " + code);
                return ServiceState.STATE_OUT_OF_SERVICE;
        }
    }


    /**
     * code is registration state 0-5 from TS 27.007 7.2
     * returns TRUE if registered roam, FALSE otherwise
     */
    private Boolean RegCodeIsRoaming (Int32 code) {
        return ServiceState.RIL_REG_STATE_ROAMING == code;
    }

    /**
     * Set roaming state if operator mcc is the same as sim mcc
     * and ons is different from spn
     *
     * @param s ServiceState hold current ons
     * @return TRUE if same operator
     */
    private Boolean IsSameNamedOperators(ServiceState s) {
        String spn = GetSystemProperty(TelephonyProperties.PROPERTY_ICC_OPERATOR_ALPHA, "empty");

        String onsl = s->GetOperatorAlphaLong();
        String onss = s->GetOperatorAlphaShort();

        Boolean equalsOnsl = onsl != NULL && spn->Equals(onsl);
        Boolean equalsOnss = onss != NULL && spn->Equals(onss);

        return CurrentMccEqualsSimMcc(s) && (equalsOnsl || equalsOnss);
    }

    /**
     * Compare SIM MCC with Operator MCC
     *
     * @param s ServiceState hold current ons
     * @return TRUE if both are same
     */
    private Boolean CurrentMccEqualsSimMcc(ServiceState s) {
        String simNumeric = GetSystemProperty(
                TelephonyProperties.PROPERTY_ICC_OPERATOR_NUMERIC, "");
        String operatorNumeric = s->GetOperatorNumeric();
        Boolean equalsMcc = TRUE;

        try {
            equalsMcc = simNumeric->Substring(0, 3).
                    Equals(operatorNumeric->Substring(0, 3));
        } Catch (Exception e){
        }
        return equalsMcc;
    }

    /**
     * Do not set roaming state in case of oprators considered non-roaming.
     *
     + Can use mcc or mcc+mnc as item of config_operatorConsideredNonRoaming.
     * For example, 302 or 21407. If mcc or mcc+mnc match with operator,
     * don't set roaming state.
     *
     * @param s ServiceState hold current ons
     * @return FALSE for roaming state set
     */
    private Boolean IsOperatorConsideredNonRoaming(ServiceState s) {
        String operatorNumeric = s->GetOperatorNumeric();
        String[] numericArray = mPhone->GetContext()->GetResources().GetStringArray(
                    R.array.config_operatorConsideredNonRoaming);

        If (numericArray.length == 0 || operatorNumeric == NULL) {
            return FALSE;
        }

        For (String numeric : numericArray) {
            If (operatorNumeric->StartsWith(numeric)) {
                return TRUE;
            }
        }
        return FALSE;
    }

    private Boolean IsOperatorConsideredRoaming(ServiceState s) {
        String operatorNumeric = s->GetOperatorNumeric();
        String[] numericArray = mPhone->GetContext()->GetResources().GetStringArray(
                    R.array.config_sameNamedOperatorConsideredRoaming);

        If (numericArray.length == 0 || operatorNumeric == NULL) {
            return FALSE;
        }

        For (String numeric : numericArray) {
            If (operatorNumeric->StartsWith(numeric)) {
                return TRUE;
            }
        }
        return FALSE;
    }

    /**
     * @return The current GPRS state. IN_SERVICE is the same as "attached"
     * and OUT_OF_SERVICE is the same as detached.
     */
    //@Override
    public Int32 GetCurrentDataConnectionState() {
        return mSS->GetDataRegState();
    }

    /**
     * @return TRUE if phone is camping on a Technology (eg UMTS)
     * that could support voice and data simultaneously or
     * concurrent services support indicator is set to '1'.
     */
    //@Override
    public Boolean IsConcurrentVoiceAndDataAllowed() {
        If (mSS->GetRilDataRadioTechnology() >= ServiceState.RIL_RADIO_TECHNOLOGY_UMTS) {
            return TRUE;
        } else {
            return mSS->GetCssIndicator() == 1;
        }
    }

    /**
     * @return the current cell location information. Prefer Gsm location
     * information if available otherwise return LTE location information
     */
    public CellLocation GetCellLocation() {
        If ((mCellLoc->GetLac() >= 0) && (mCellLoc->GetCid() >= 0)) {
            If (DBG) Log("GetCellLocation(): X good mCellLoc=" + mCellLoc);
            return mCellLoc;
        } else {
            List<CellInfo> result = GetAllCellInfo();
            If (result != NULL) {
                // A hack to allow tunneling of LTE information via GsmCellLocation
                // so that older Network Location Providers can return some information
                // on LTE only networks, see bug 9228974.
                //
                // We'll search the return CellInfo array preferring GSM/WCDMA
                // data, but if there is none we'll tunnel the first LTE information
                // in the list.
                //
                // The tunnel'd LTE information is returned as follows:
                //   LAC = TAC field
                //   CID = CI field
                //   PSC = 0.
                GsmCellLocation cellLocOther = new GsmCellLocation();
                For (CellInfo ci : result) {
                    If (ci instanceof CellInfoGsm) {
                        CellInfoGsm cellInfoGsm = (CellInfoGsm)ci;
                        CellIdentityGsm cellIdentityGsm = cellInfoGsm->GetCellIdentity();
                        cellLocOther->SetLacAndCid(cellIdentityGsm->GetLac(),
                                cellIdentityGsm->GetCid());
                        cellLocOther->SetPsc(cellIdentityGsm->GetPsc());
                        If (DBG) Log("GetCellLocation(): X ret GSM info=" + cellLocOther);
                        return cellLocOther;
                    } else If (ci instanceof CellInfoWcdma) {
                        CellInfoWcdma cellInfoWcdma = (CellInfoWcdma)ci;
                        CellIdentityWcdma cellIdentityWcdma = cellInfoWcdma->GetCellIdentity();
                        cellLocOther->SetLacAndCid(cellIdentityWcdma->GetLac(),
                                cellIdentityWcdma->GetCid());
                        cellLocOther->SetPsc(cellIdentityWcdma->GetPsc());
                        If (DBG) Log("GetCellLocation(): X ret WCDMA info=" + cellLocOther);
                        return cellLocOther;
                    } else If ((ci instanceof CellInfoLte) &&
                            ((cellLocOther->GetLac() < 0) || (cellLocOther->GetCid() < 0))) {
                        // We'll return the first good LTE info we get if there is no better answer
                        CellInfoLte cellInfoLte = (CellInfoLte)ci;
                        CellIdentityLte cellIdentityLte = cellInfoLte->GetCellIdentity();
                        If ((cellIdentityLte->GetTac() != Integer.MAX_VALUE)
                                && (cellIdentityLte->GetCi() != Integer.MAX_VALUE)) {
                            cellLocOther->SetLacAndCid(cellIdentityLte->GetTac(),
                                    cellIdentityLte->GetCi());
                            cellLocOther->SetPsc(0);
                            If (DBG) {
                                Log("GetCellLocation(): possible LTE cellLocOther=" + cellLocOther);
                            }
                        }
                    }
                }
                If (DBG) {
                    Log("GetCellLocation(): X ret best answer cellLocOther=" + cellLocOther);
                }
                return cellLocOther;
            } else {
                If (DBG) {
                    Log("GetCellLocation(): X empty mCellLoc and CellInfo mCellLoc=" + mCellLoc);
                }
                return mCellLoc;
            }
        }
    }

    /**
     * nitzReceiveTime is time_t that the NITZ time was posted
     */
    private void SetTimeFromNITZString (String nitz, Int64 nitzReceiveTime) {
        // "yy/mm/dd,hh:mm:Ss(+/-)tz"
        // tz is in number of quarter-hours

        Int64 start = SystemClock->ElapsedRealtime();
        If (DBG) {Log("NITZ: " + nitz + "," + nitzReceiveTime +
                        " start=" + start + " delay=" + (start - nitzReceiveTime));
        }

        try {
            /* NITZ Time (hour:min:sec) will be in UTC but it supplies the timezone
             * offset as Well (which we won't worry about until later) */
            Calendar c = Calendar->GetInstance(TimeZone->GetTimeZone("GMT"));

            c->Clear();
            c->Set(Calendar.DST_OFFSET, 0);

            String[] nitzSubs = nitz->Split("[/:,+-]");

            Int32 year = 2000 + Integer->ParseInt(nitzSubs[0]);
            c->Set(Calendar.YEAR, year);

            // month is 0 based!
            Int32 month = Integer->ParseInt(nitzSubs[1]) - 1;
            c->Set(Calendar.MONTH, month);

            Int32 date = Integer->ParseInt(nitzSubs[2]);
            c->Set(Calendar.DATE, date);

            Int32 hour = Integer->ParseInt(nitzSubs[3]);
            c->Set(Calendar.HOUR, hour);

            Int32 minute = Integer->ParseInt(nitzSubs[4]);
            c->Set(Calendar.MINUTE, minute);

            Int32 second = Integer->ParseInt(nitzSubs[5]);
            c->Set(Calendar.SECOND, second);

            Boolean sign = (nitz->IndexOf('-') == -1);

            Int32 tzOffset = Integer->ParseInt(nitzSubs[6]);

            Int32 dst = (nitzSubs.length >= 8 ) ? Integer->ParseInt(nitzSubs[7])
                                              : 0;

            // The zone offset received from NITZ is for current local time,
            // so DST correction is already applied.  Don't add it again.
            //
            // tzOffset += dst * 4;
            //
            // We could unapply it if we wanted the raw offset.

            tzOffset = (sign ? 1 : -1) * tzOffset * 15 * 60 * 1000;

            TimeZone    zone = NULL;

            // As a special extension, the Android emulator appends the name of
            // the host computer's timezone to the nitz string. this is zoneinfo
            // timezone name of the form Area!Location or Area!Location!SubLocation
            // so we need to convert the ! into /
            If (nitzSubs.length >= 9) {
                String  tzname = nitzSubs[8].Replace('!','/');
                zone = TimeZone->GetTimeZone( tzname );
                // From luni's GetTimeZone() "We never return NULL; on failure we return the
                // equivalent of "GMT"." This is bad, since it'll force all invalid strings
                // to "GMT"... and all the NULL-zone checks below will fail, making tzOffset
                // irrelevant and GMT the active TZ. So tzOffset will take precedence if this
                // results in "GMT"
                If (TimeZone->GetTimeZone("GMT").Equals(zone) && tzOffset != 0) {
                    zone = NULL;
                }
            }

            String iso = GetSystemProperty(TelephonyProperties.PROPERTY_OPERATOR_ISO_COUNTRY, "");

            If (zone == NULL) {

                If (mGotCountryCode) {
                    If (iso != NULL && iso->Length() > 0) {
                        zone = TimeUtils->GetTimeZone(tzOffset, dst != 0,
                                c->GetTimeInMillis(),
                                iso);
                    } else {
                        // We don't have a valid iso country code.  This is
                        // most likely because we're on a test network that's
                        // using a bogus MCC (eg, "001"), so get a TimeZone
                        // based only on the NITZ parameters.
                        zone = GetNitzTimeZone(tzOffset, (dst != 0), c->GetTimeInMillis());
                    }
                }
            }

            If ((zone == NULL) || (mZoneOffset != tzOffset) || (mZoneDst != (dst != 0))){
                // We got the time before the country or the zone has changed
                // so we don't know how to identify the DST rules yet.  Save
                // the information and hope to fix it up later.

                mNeedFixZoneAfterNitz = TRUE;
                mZoneOffset  = tzOffset;
                mZoneDst     = dst != 0;
                mZoneTime    = c->GetTimeInMillis();
            }

            If (zone != NULL) {
                If (GetAutoTimeZone()) {
                    SetAndBroadcastNetworkSetTimeZone(zone->GetID());
                }
                SaveNitzTimeZone(zone->GetID());
            }

            String ignore = SystemProperties->Get("gsm.ignore-nitz");
            If (ignore != NULL && ignore->Equals("yes")) {
                Log("NITZ: Not setting clock because gsm.ignore-nitz is set");
                return;
            }

            try {
                mWakeLock->Acquire();

                If (GetAutoTime()) {
                    Int64 millisSinceNitzReceived
                            = SystemClock->ElapsedRealtime() - nitzReceiveTime;

                    If (millisSinceNitzReceived < 0) {
                        // Sanity check: something is wrong
                        If (DBG) {
                            Log("NITZ: not setting time, clock has rolled "
                                            + "backwards since NITZ time was received, "
                                            + nitz);
                        }
                        return;
                    }

                    If (millisSinceNitzReceived > Integer.MAX_VALUE) {
                        // If the time is this far off, something is wrong > 24 days!
                        If (DBG) {
                            Log("NITZ: not setting time, processing has taken "
                                        + (millisSinceNitzReceived / (1000 * 60 * 60 * 24))
                                        + " days");
                        }
                        return;
                    }

                    // Note: with range checks above, cast to Int32 is safe
                    c->Add(Calendar.MILLISECOND, (Int32)millisSinceNitzReceived);

                    If (DBG) {
                        Log("NITZ: Setting time of day to " + c->GetTime()
                            + " NITZ receive Delay(ms): " + millisSinceNitzReceived
                            + " Gained(ms): "
                            + (c->GetTimeInMillis() - System->CurrentTimeMillis())
                            + " from " + nitz);
                    }

                    SetAndBroadcastNetworkSetTime(c->GetTimeInMillis());
                    Rlog->I(LOG_TAG, "NITZ: after Setting time of day");
                }
                SystemProperties->Set("gsm.nitz.time", String->ValueOf(c->GetTimeInMillis()));
                SaveNitzTime(c->GetTimeInMillis());
                If (VDBG) {
                    Int64 end = SystemClock->ElapsedRealtime();
                    Log("NITZ: end=" + end + " dur=" + (end - start));
                }
                mNitzUpdatedTime = TRUE;
            } finally {
                mWakeLock->Release();
            }
        } Catch (RuntimeException ex) {
            Loge("NITZ: Parsing NITZ time " + nitz + " ex=" + ex);
        }
    }

    private Boolean GetAutoTime() {
        try {
            return Settings::Global::>GetInt(mPhone->GetContext()->GetContentResolver(),
                    Settings::Global::AUTO_TIME) > 0;
        } Catch (SettingNotFoundException snfe) {
            return TRUE;
        }
    }

    private Boolean GetAutoTimeZone() {
        try {
            return Settings::Global::>GetInt(mPhone->GetContext()->GetContentResolver(),
                    Settings::Global::AUTO_TIME_ZONE) > 0;
        } Catch (SettingNotFoundException snfe) {
            return TRUE;
        }
    }

    private void SaveNitzTimeZone(String zoneId) {
        mSavedTimeZone = zoneId;
    }

    private void SaveNitzTime(Int64 time) {
        mSavedTime = time;
        mSavedAtTime = SystemClock->ElapsedRealtime();
    }

    /**
     * Set the timezone and send out a sticky broadcast so the system can
     * determine if the timezone was set by the carrier.
     *
     * @param zoneId timezone set by carrier
     */
    private void SetAndBroadcastNetworkSetTimeZone(String zoneId) {
        If (DBG) Log("setAndBroadcastNetworkSetTimeZone: setTimeZone=" + zoneId);
        AlarmManager alarm =
            (AlarmManager) mPhone->GetContext()->GetSystemService(Context.ALARM_SERVICE);
        alarm->SetTimeZone(zoneId);
        Intent intent = new Intent(TelephonyIntents.ACTION_NETWORK_SET_TIMEZONE);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
        intent->PutExtra("time-zone", zoneId);
        mPhone->GetContext()->SendStickyBroadcastAsUser(intent, UserHandle.ALL);
        If (DBG) {
            Log("setAndBroadcastNetworkSetTimeZone: call alarm.setTimeZone and broadcast zoneId=" +
                zoneId);
        }
    }

    /**
     * Set the time and Send out a sticky broadcast so the system can determine
     * if the time was set by the carrier.
     *
     * @param time time set by network
     */
    private void SetAndBroadcastNetworkSetTime(Int64 time) {
        If (DBG) Log("setAndBroadcastNetworkSetTime: time=" + time + "ms");
        SystemClock->SetCurrentTimeMillis(time);
        Intent intent = new Intent(TelephonyIntents.ACTION_NETWORK_SET_TIME);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
        intent->PutExtra("time", time);
        mPhone->GetContext()->SendStickyBroadcastAsUser(intent, UserHandle.ALL);
    }

    private void RevertToNitzTime() {
        If (Settings::Global::>GetInt(mPhone->GetContext()->GetContentResolver(),
                Settings::Global::AUTO_TIME, 0) == 0) {
            return;
        }
        If (DBG) {
            Log("Reverting to NITZ Time: mSavedTime=" + mSavedTime
                + " mSavedAtTime=" + mSavedAtTime);
        }
        If (mSavedTime != 0 && mSavedAtTime != 0) {
            SetAndBroadcastNetworkSetTime(mSavedTime
                    + (SystemClock->ElapsedRealtime() - mSavedAtTime));
        }
    }

    private void RevertToNitzTimeZone() {
        If (Settings::Global::>GetInt(mPhone->GetContext()->GetContentResolver(),
                Settings::Global::AUTO_TIME_ZONE, 0) == 0) {
            return;
        }
        If (DBG) Log("Reverting to NITZ TimeZone: tz='" + mSavedTimeZone);
        If (mSavedTimeZone != NULL) {
            SetAndBroadcastNetworkSetTimeZone(mSavedTimeZone);
        }
    }

    /**
     * Post a notification to NotificationManager for restricted state
     *
     * @param notifyType is one state of PS/CS_*_ENABLE/DISABLE
     */
    private void SetNotification(Int32 notifyType) {

        If (DBG) Log("setNotification: create notification " + notifyType);
        Context context = mPhone->GetContext();

        mNotification = new Notification();
        mNotification.when = System->CurrentTimeMillis();
        mNotification.flags = Notification.FLAG_AUTO_CANCEL;
        mNotification.icon = R.drawable.stat_sys_warning;
        Intent intent = new Intent();
        mNotification.contentIntent = PendingIntent
        .GetActivity(context, 0, intent, PendingIIntent::FLAG_CANCEL_CURRENT);

        CharSequence details = "";
        CharSequence title = context->GetText(R::string::RestrictedChangedTitle);
        Int32 notificationId = CS_NOTIFICATION;

        Switch (notifyType) {
        case PS_ENABLED:
            notificationId = PS_NOTIFICATION;
            details = context->GetText(R::string::RestrictedOnData);
            break;
        case PS_DISABLED:
            notificationId = PS_NOTIFICATION;
            break;
        case CS_ENABLED:
            details = context->GetText(R::string::RestrictedOnAllVoice);
            break;
        case CS_NORMAL_ENABLED:
            details = context->GetText(R::string::RestrictedOnNormal);
            break;
        case CS_EMERGENCY_ENABLED:
            details = context->GetText(R::string::RestrictedOnEmergency);
            break;
        case CS_DISABLED:
            // do nothing and cancel the notification later
            break;
        }

        If (DBG) Log("setNotification: put notification " + title + " / " +details);
        mNotification.tickerText = title;
        mNotification.color = context->GetResources()->GetColor(
                R::Color::system_notification_accent_color);
        mNotification->SetLatestEventInfo(context, title, details,
                mNotification.contentIntent);

        NotificationManager notificationManager = (NotificationManager)
            context->GetSystemService(Context.NOTIFICATION_SERVICE);

        If (notifyType == PS_DISABLED || notifyType == CS_DISABLED) {
            // cancel previous post notification
            notificationManager->Cancel(notificationId);
        } else {
            // update restricted state notification
            notificationManager->Notify(notificationId, mNotification);
        }
    }

    private UiccCardApplication GetUiccCardApplication() {
            return  mUiccController->GetUiccCardApplication(mPhone->GetPhoneId(),
                    UiccController.APP_FAM_3GPP);
    }

    //@Override
    protected void OnUpdateIccAvailability() {
        If (mUiccController == NULL ) {
            return;
        }

        UiccCardApplication newUiccApplication = GetUiccCardApplication();

        If (mUiccApplcation != newUiccApplication) {
            If (mUiccApplcation != NULL) {
                Log("Removing stale icc objects.");
                mUiccApplcation->UnregisterForReady(this);
                If (mIccRecords != NULL) {
                    mIccRecords->UnregisterForRecordsLoaded(this);
                }
                mIccRecords = NULL;
                mUiccApplcation = NULL;
            }
            If (newUiccApplication != NULL) {
                Log("New card found");
                mUiccApplcation = newUiccApplication;
                mIccRecords = mUiccApplcation->GetIccRecords();
                mUiccApplcation->RegisterForReady(this, EVENT_SIM_READY, NULL);
                If (mIccRecords != NULL) {
                    mIccRecords->RegisterForRecordsLoaded(this, EVENT_SIM_RECORDS_LOADED, NULL);
                }
            }
        }
    }
    //@Override
    protected void Log(String s) {
        Rlog->D(LOG_TAG, "[GsmSST] " + s);
    }

    //@Override
    protected void Loge(String s) {
        Rlog->E(LOG_TAG, "[GsmSST] " + s);
    }

    //@Override
    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("GsmServiceStateTracker extends:");
        super->Dump(fd, pw, args);
        pw->Println(" mPhone=" + mPhone);
        pw->Println(" mSS=" + mSS);
        pw->Println(" mNewSS=" + mNewSS);
        pw->Println(" mCellLoc=" + mCellLoc);
        pw->Println(" mNewCellLoc=" + mNewCellLoc);
        pw->Println(" mPreferredNetworkType=" + mPreferredNetworkType);
        pw->Println(" mMaxDataCalls=" + mMaxDataCalls);
        pw->Println(" mNewMaxDataCalls=" + mNewMaxDataCalls);
        pw->Println(" mReasonDataDenied=" + mReasonDataDenied);
        pw->Println(" mNewReasonDataDenied=" + mNewReasonDataDenied);
        pw->Println(" mGsmRoaming=" + mGsmRoaming);
        pw->Println(" mDataRoaming=" + mDataRoaming);
        pw->Println(" mEmergencyOnly=" + mEmergencyOnly);
        pw->Println(" mNeedFixZoneAfterNitz=" + mNeedFixZoneAfterNitz);
        pw->Println(" mZoneOffset=" + mZoneOffset);
        pw->Println(" mZoneDst=" + mZoneDst);
        pw->Println(" mZoneTime=" + mZoneTime);
        pw->Println(" mGotCountryCode=" + mGotCountryCode);
        pw->Println(" mNitzUpdatedTime=" + mNitzUpdatedTime);
        pw->Println(" mSavedTimeZone=" + mSavedTimeZone);
        pw->Println(" mSavedTime=" + mSavedTime);
        pw->Println(" mSavedAtTime=" + mSavedAtTime);
        pw->Println(" mStartedGprsRegCheck=" + mStartedGprsRegCheck);
        pw->Println(" mReportedGprsNoReg=" + mReportedGprsNoReg);
        pw->Println(" mNotification=" + mNotification);
        pw->Println(" mWakeLock=" + mWakeLock);
        pw->Println(" mCurSpn=" + mCurSpn);
        pw->Println(" mCurShowSpn=" + mCurShowSpn);
        pw->Println(" mCurPlmn=" + mCurPlmn);
        pw->Println(" mCurShowPlmn=" + mCurShowPlmn);
    }


    /**
     * Clean up existing voice and data connection then turn off radio power.
     *
     * Hang up the existing voice calls to decrease call drop rate.
     */
    //@Override
    CARAPI PowerOffRadioSafely(DcTrackerBase dcTracker) {
        Synchronized (this) {
            If (!mPendingRadioPowerOffAfterDataOff) {
                Int64 dds = SubscriptionManager->GetDefaultDataSubId();
                // To minimize race conditions we call cleanUpAllConnections on
                // both if else paths instead of before this isDisconnected test.
                If (dcTracker->IsDisconnected()
                        && (dds == mPhone->GetSubId()
                            || (dds != mPhone->GetSubId()
                                && ProxyController->GetInstance()->IsDataDisconnected(dds)))) {
                    // To minimize race conditions we do this after isDisconnected
                    dcTracker->CleanUpAllConnections(Phone.REASON_RADIO_TURNED_OFF);
                    If (DBG) Log("Data disconnected, turn off radio right away.");
                    HangupAndPowerOff();
                } else {
                    dcTracker->CleanUpAllConnections(Phone.REASON_RADIO_TURNED_OFF);
                    If (dds != mPhone->GetSubId()
                            && !ProxyController->GetInstance()->IsDataDisconnected(dds)) {
                        If (DBG) Log("Data is active on DDS.  Wait for all data disconnect");
                        // Data is not disconnected on DDS. Wait for the data disconnect complete
                        // before sending the RADIO_POWER off.
                        ProxyController->GetInstance()->RegisterForAllDataDisconnected(dds, this,
                                EVENT_ALL_DATA_DISCONNECTED, NULL);
                        mPendingRadioPowerOffAfterDataOff = TRUE;
                    }
                    Message msg = Message->Obtain(this);
                    msg.what = EVENT_SET_RADIO_POWER_OFF;
                    msg.arg1 = ++mPendingRadioPowerOffAfterDataOffTag;
                    If (SendMessageDelayed(msg, 30000)) {
                        If (DBG) Log("Wait upto 30s for data to disconnect, then turn off radio.");
                        mPendingRadioPowerOffAfterDataOff = TRUE;
                    } else {
                        Log("Cannot send delayed Msg, turn off radio right away.");
                        HangupAndPowerOff();
                        mPendingRadioPowerOffAfterDataOff = FALSE;
                    }
                }
            }
        }

    }

    CARAPI SetImsRegistrationState(Boolean registered){
        If (mImsRegistrationOnOff && !registered) {
            If (mAlarmSwitch) {
                mImsRegistrationOnOff = registered;

                Context context = mPhone->GetContext();
                AlarmManager am = (AlarmManager) context->GetSystemService(Context.ALARM_SERVICE);
                am->Cancel(mRadioOffIntent);
                mAlarmSwitch = FALSE;

                SendMessage(ObtainMessage(EVENT_CHANGE_IMS_STATE));
                return;
            }
        }
        mImsRegistrationOnOff = registered;
    }
}
