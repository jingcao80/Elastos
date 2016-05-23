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

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Telephony::ICellInfo;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Utility::ITimeUtils;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Content::IContext;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

using Elastos::Droid::Internal::Telephony::Dataconnection::IDcTrackerBase;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppState;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;

/**
 * {@hide}
 */
public abstract class ServiceStateTracker extends Handler {
    protected  static const Boolean DBG = TRUE;
    protected static const Boolean VDBG = FALSE;

    protected static const String PROP_FORCE_ROAMING = "telephony.test.forceRoaming";

    protected CommandsInterface mCi;
    protected UiccController mUiccController = NULL;
    protected UiccCardApplication mUiccApplcation = NULL;
    protected IccRecords mIccRecords = NULL;

    protected PhoneBase mPhoneBase;

    protected Boolean mVoiceCapable;

    public ServiceState mSS = new ServiceState();
    protected ServiceState mNewSS = new ServiceState();

    private static const Int64 LAST_CELL_INFO_LIST_MAX_AGE_MS = 2000;
    protected Int64 mLastCellInfoListTime;
    protected List<CellInfo> mLastCellInfoList = NULL;

    // This is final as subclasses alias to a more specific type
    // so we don't want the reference to change.
    protected final CellInfo mCellInfo;

    protected SignalStrength mSignalStrength = new SignalStrength();

    // TODO - this should not be public, right now used externally GsmConnetion.
    public RestrictedState mRestrictedState = new RestrictedState();

    /* The otaspMode passed to PhoneStateListener#onOtaspChanged */
    static public final Int32 OTASP_UNINITIALIZED = 0;
    static public final Int32 OTASP_UNKNOWN = 1;
    static public final Int32 OTASP_NEEDED = 2;
    static public final Int32 OTASP_NOT_NEEDED = 3;

    /**
     * A unique identifier to track requests associated with a poll
     * and ignore stale responses.  The value is a count-down of
     * expected responses in this pollingContext.
     */
    protected Int32[] mPollingContext;
    protected Boolean mDesiredPowerState;

    /**
     * By default, strength polling is enabled.  However, if we're
     * getting unsolicited signal strength updates from the radio, set
     * value to TRUE and don't bother polling any more.
     */
    protected Boolean mDontPollSignalStrength = FALSE;

    protected RegistrantList mRoamingOnRegistrants = new RegistrantList();
    protected RegistrantList mRoamingOffRegistrants = new RegistrantList();
    protected RegistrantList mAttachedRegistrants = new RegistrantList();
    protected RegistrantList mDetachedRegistrants = new RegistrantList();
    protected RegistrantList mDataRegStateOrRatChangedRegistrants = new RegistrantList();
    protected RegistrantList mNetworkAttachedRegistrants = new RegistrantList();
    protected RegistrantList mPsRestrictEnabledRegistrants = new RegistrantList();
    protected RegistrantList mPsRestrictDisabledRegistrants = new RegistrantList();

    /* Radio power off pending flag and tag counter */
    protected Boolean mPendingRadioPowerOffAfterDataOff = FALSE;
    protected Int32 mPendingRadioPowerOffAfterDataOffTag = 0;

    /* Current rat is iwlan */
    protected Boolean mIwlanRatAvailable = FALSE;

    /** Signal strength poll rate. */
    protected static const Int32 POLL_PERIOD_MILLIS = 20 * 1000;

    /** Waiting period before recheck gprs and voice registration. */
    public static const Int32 DEFAULT_GPRS_CHECK_PERIOD_MILLIS = 60 * 1000;

    /** GSM events */
    protected static const Int32 EVENT_RADIO_STATE_CHANGED               = 1;
    protected static const Int32 EVENT_NETWORK_STATE_CHANGED             = 2;
    protected static const Int32 EVENT_GET_SIGNAL_STRENGTH               = 3;
    protected static const Int32 EVENT_POLL_STATE_REGISTRATION           = 4;
    protected static const Int32 EVENT_POLL_STATE_GPRS                   = 5;
    protected static const Int32 EVENT_POLL_STATE_OPERATOR               = 6;
    protected static const Int32 EVENT_POLL_SIGNAL_STRENGTH              = 10;
    protected static const Int32 EVENT_NITZ_TIME                         = 11;
    protected static const Int32 EVENT_SIGNAL_STRENGTH_UPDATE            = 12;
    protected static const Int32 EVENT_RADIO_AVAILABLE                   = 13;
    protected static const Int32 EVENT_POLL_STATE_NETWORK_SELECTION_MODE = 14;
    protected static const Int32 EVENT_GET_LOC_DONE                      = 15;
    protected static const Int32 EVENT_SIM_RECORDS_LOADED                = 16;
    protected static const Int32 EVENT_SIM_READY                         = 17;
    protected static const Int32 EVENT_LOCATION_UPDATES_ENABLED          = 18;
    protected static const Int32 EVENT_GET_PREFERRED_NETWORK_TYPE        = 19;
    protected static const Int32 EVENT_SET_PREFERRED_NETWORK_TYPE        = 20;
    protected static const Int32 EVENT_RESET_PREFERRED_NETWORK_TYPE      = 21;
    protected static const Int32 EVENT_CHECK_REPORT_GPRS                 = 22;
    protected static const Int32 EVENT_RESTRICTED_STATE_CHANGED          = 23;

    /** CDMA events */
    protected static const Int32 EVENT_POLL_STATE_REGISTRATION_CDMA      = 24;
    protected static const Int32 EVENT_POLL_STATE_OPERATOR_CDMA          = 25;
    protected static const Int32 EVENT_RUIM_READY                        = 26;
    protected static const Int32 EVENT_RUIM_RECORDS_LOADED               = 27;
    protected static const Int32 EVENT_POLL_SIGNAL_STRENGTH_CDMA         = 28;
    protected static const Int32 EVENT_GET_SIGNAL_STRENGTH_CDMA          = 29;
    protected static const Int32 EVENT_NETWORK_STATE_CHANGED_CDMA        = 30;
    protected static const Int32 EVENT_GET_LOC_DONE_CDMA                 = 31;
    //protected static const Int32 EVENT_UNUSED                            = 32;
    protected static const Int32 EVENT_NV_LOADED                         = 33;
    protected static const Int32 EVENT_POLL_STATE_CDMA_SUBSCRIPTION      = 34;
    protected static const Int32 EVENT_NV_READY                          = 35;
    protected static const Int32 EVENT_ERI_FILE_LOADED                   = 36;
    protected static const Int32 EVENT_OTA_PROVISION_STATUS_CHANGE       = 37;
    protected static const Int32 EVENT_SET_RADIO_POWER_OFF               = 38;
    protected static const Int32 EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED  = 39;
    protected static const Int32 EVENT_CDMA_PRL_VERSION_CHANGED          = 40;
    protected static const Int32 EVENT_RADIO_ON                          = 41;
    public static const Int32 EVENT_ICC_CHANGED                          = 42;
    protected static const Int32 EVENT_GET_CELL_INFO_LIST                = 43;
    protected static const Int32 EVENT_UNSOL_CELL_INFO_LIST              = 44;
    protected static const Int32 EVENT_CHANGE_IMS_STATE                  = 45;

    protected static const String TIMEZONE_PROPERTY = "persist.sys.timezone";

    /**
     * List of ISO codes for countries that can have an offset of
     * GMT+0 when not in daylight savings time.  This ignores some
     * small places such as the Canary Islands (Spain) and
     * Danmarkshavn (Denmark).  The list must be sorted by code.
    */
    protected static const String[] GMT_COUNTRY_CODES = {
        "bf", // Burkina Faso
        "ci", // Cote d'Ivoire
        "eh", // Western Sahara
        "fo", // Faroe Islands, Denmark
        "gb", // United Kingdom of Great Britain and Northern Ireland
        "gh", // Ghana
        "gm", // Gambia
        "gn", // Guinea
        "gw", // Guinea Bissau
        "ie", // Ireland
        "lr", // Liberia
        "is", // Iceland
        "ma", // Morocco
        "ml", // Mali
        "mr", // Mauritania
        "pt", // Portugal
        "sl", // Sierra Leone
        "sn", // Senegal
        "st", // Sao Tome and Principe
        "tg", // Togo
    };

    private class CellInfoResult {
        List<CellInfo> list;
        Object lockObj = new Object();
    }

    /** Reason for registration denial. */
    protected static const String REGISTRATION_DENIED_GEN  = "General";
    protected static const String REGISTRATION_DENIED_AUTH = "Authentication Failure";

    protected Boolean mImsRegistrationOnOff = FALSE;
    protected Boolean mAlarmSwitch = FALSE;
    protected IntentFilter mIntentFilter = NULL;
    protected PendingIntent mRadioOffIntent = NULL;
    protected static const String ACTION_RADIO_OFF = "android.intent.action.ACTION_RADIO_OFF";
    protected Boolean mPowerOffDelayNeed = TRUE;
    protected Boolean mDeviceShuttingDown = FALSE;


    protected ServiceStateTracker(PhoneBase phoneBase, CommandsInterface ci, CellInfo cellInfo) {
        mPhoneBase = phoneBase;
        mCellInfo = cellInfo;
        mCi = ci;
        mVoiceCapable = mPhoneBase->GetContext()->GetResources().GetBoolean(
                R.bool.config_voice_capable);
        mUiccController = UiccController->GetInstance();
        mUiccController->RegisterForIccChanged(this, EVENT_ICC_CHANGED, NULL);
        mCi->SetOnSignalStrengthUpdate(this, EVENT_SIGNAL_STRENGTH_UPDATE, NULL);
        mCi->RegisterForCellInfoList(this, EVENT_UNSOL_CELL_INFO_LIST, NULL);

        mPhoneBase->SetSystemProperty(TelephonyProperties.PROPERTY_DATA_NETWORK_TYPE,
            ServiceState->RilRadioTechnologyToString(ServiceState.RIL_RADIO_TECHNOLOGY_UNKNOWN));
    }

    void RequestShutdown() {
        If (mDeviceShuttingDown == TRUE) return;
        mDeviceShuttingDown = TRUE;
        mDesiredPowerState = FALSE;
        SetPowerStateToDesired();
    }

    CARAPI Dispose() {
        mCi->UnSetOnSignalStrengthUpdate(this);
        mUiccController->UnregisterForIccChanged(this);
        mCi->UnregisterForCellInfoList(this);
    }

    public Boolean GetDesiredPowerState() {
        return mDesiredPowerState;
    }

    private SignalStrength mLastSignalStrength = NULL;
    protected Boolean NotifySignalStrength() {
        Boolean notified = FALSE;
        {    AutoLock syncLock(mCellInfo);
            If (!mSignalStrength->Equals(mLastSignalStrength)) {
                try {
                    mPhoneBase->NotifySignalStrength();
                    notified = TRUE;
                } Catch (NullPointerException ex) {
                    Loge("UpdateSignalStrength() Phone already destroyed: " + ex
                            + "SignalStrength not notified");
                }
            }
        }
        return notified;
    }

    /**
     * Notify all mDataConnectionRatChangeRegistrants using an
     * AsyncResult in msg.obj where AsyncResult#result contains the
     * new RAT as an Integer Object.
     */
    protected void NotifyDataRegStateRilRadioTechnologyChanged() {
        Int32 rat = mSS->GetRilDataRadioTechnology();
        Int32 drs = mSS->GetDataRegState();
        If (DBG) Log("notifyDataRegStateRilRadioTechnologyChanged: drs=" + drs + " rat=" + rat);
        mPhoneBase->SetSystemProperty(TelephonyProperties.PROPERTY_DATA_NETWORK_TYPE,
                ServiceState->RilRadioTechnologyToString(rat));
        mDataRegStateOrRatChangedRegistrants->NotifyResult(new Pair<Integer, Integer>(drs, rat));
    }

    /**
     * Some operators have been known to report registration failure
     * data only devices, to fix that use DataRegState.
     */
    protected void UseDataRegStateForDataOnlyDevices() {
        If (mVoiceCapable == FALSE) {
            If (DBG) {
                Log("useDataRegStateForDataOnlyDevice: VoiceRegState=" + mNewSS->GetVoiceRegState()
                    + " DataRegState=" + mNewSS->GetDataRegState());
            }
            // TODO: Consider not lying and instead have callers know the difference.
            mNewSS->SetVoiceRegState(mNewSS->GetDataRegState());
        }
    }

    protected void UpdatePhoneObject() {
        If (mPhoneBase->GetContext()->GetResources().
                GetBoolean(R.bool.config_switch_phone_on_voice_reg_state_change)) {
            mPhoneBase->UpdatePhoneObject(mSS->GetRilVoiceRadioTechnology());
        }
    }

    /**
     * Registration point for combined roaming on
     * combined roaming is TRUE when roaming is TRUE and ONS differs SPN
     *
     * @param h handler to notify
     * @param what what code of message when delivered
     * @param obj placed in Message.obj
     */
    public  void RegisterForRoamingOn(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant(h, what, obj);
        mRoamingOnRegistrants->Add(r);

        If (mSS->GetRoaming()) {
            r->NotifyRegistrant();
        }
    }

    public  void UnregisterForRoamingOn(Handler h) {
        mRoamingOnRegistrants->Remove(h);
    }

    /**
     * Registration point for combined roaming off
     * combined roaming is TRUE when roaming is TRUE and ONS differs SPN
     *
     * @param h handler to notify
     * @param what what code of message when delivered
     * @param obj placed in Message.obj
     */
    public  void RegisterForRoamingOff(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant(h, what, obj);
        mRoamingOffRegistrants->Add(r);

        If (!mSS->GetRoaming()) {
            r->NotifyRegistrant();
        }
    }

    public  void UnregisterForRoamingOff(Handler h) {
        mRoamingOffRegistrants->Remove(h);
    }

    /**
     * Re-register network by toggling preferred network type.
     * This is a work-around to deregister and register network since there is
     * no ril api to set COPS=2 (deregister) only.
     *
     * @param onComplete is dispatched when this is complete.  it will be
     * an AsyncResult, and onComplete.obj.exception will be non-NULL
     * on failure.
     */
    CARAPI ReRegisterNetwork(Message onComplete) {
        mCi->GetPreferredNetworkType(
                ObtainMessage(EVENT_GET_PREFERRED_NETWORK_TYPE, onComplete));
    }

    CARAPI
    SetRadioPower(Boolean power) {
        mDesiredPowerState = power;

        SetPowerStateToDesired();
    }

    /**
     * These two flags manage the behavior of the cell lock -- the
     * lock should be held if either flag is TRUE.  The intention is
     * to allow temporary acquisition of the lock to get a single
     * update.  Such a lock grab and release can thus be made to not
     * interfere with more permanent lock holds -- in other words, the
     * lock will only be released if both flags are FALSE, and so
     * releases by temporary users will only affect the lock state if
     * there is no continuous user.
     */
    private Boolean mWantContinuousLocationUpdates;
    private Boolean mWantSingleLocationUpdate;

    CARAPI EnableSingleLocationUpdate() {
        If (mWantSingleLocationUpdate || mWantContinuousLocationUpdates) return;
        mWantSingleLocationUpdate = TRUE;
        mCi->SetLocationUpdates(TRUE, ObtainMessage(EVENT_LOCATION_UPDATES_ENABLED));
    }

    CARAPI EnableLocationUpdates() {
        If (mWantSingleLocationUpdate || mWantContinuousLocationUpdates) return;
        mWantContinuousLocationUpdates = TRUE;
        mCi->SetLocationUpdates(TRUE, ObtainMessage(EVENT_LOCATION_UPDATES_ENABLED));
    }

    protected void DisableSingleLocationUpdate() {
        mWantSingleLocationUpdate = FALSE;
        If (!mWantSingleLocationUpdate && !mWantContinuousLocationUpdates) {
            mCi->SetLocationUpdates(FALSE, NULL);
        }
    }

    CARAPI DisableLocationUpdates() {
        mWantContinuousLocationUpdates = FALSE;
        If (!mWantSingleLocationUpdate && !mWantContinuousLocationUpdates) {
            mCi->SetLocationUpdates(FALSE, NULL);
        }
    }

    //@Override
    CARAPI HandleMessage(Message msg) {
        Switch (msg.what) {
            case EVENT_SET_RADIO_POWER_OFF:
                {    AutoLock syncLock(this);
                    If (mPendingRadioPowerOffAfterDataOff &&
                            (msg.arg1 == mPendingRadioPowerOffAfterDataOffTag)) {
                        If (DBG) Log("EVENT_SET_RADIO_OFF, turn radio off now.");
                        HangupAndPowerOff();
                        mPendingRadioPowerOffAfterDataOffTag += 1;
                        mPendingRadioPowerOffAfterDataOff = FALSE;
                    } else {
                        Log("EVENT_SET_RADIO_OFF is stale arg1=" + msg.arg1 +
                                "!= tag=" + mPendingRadioPowerOffAfterDataOffTag);
                    }
                }
                break;

            case EVENT_ICC_CHANGED:
                OnUpdateIccAvailability();
                break;

            case EVENT_GET_CELL_INFO_LIST: {
                AsyncResult ar = (AsyncResult) msg.obj;
                CellInfoResult result = (CellInfoResult) ar.userObj;
                {    AutoLock syncLock(result.lockObj);
                    If (ar.exception != NULL) {
                        Log("EVENT_GET_CELL_INFO_LIST: error ret NULL, e=" + ar.exception);
                        result.list = NULL;
                    } else {
                        result.list = (List<CellInfo>) ar.result;

                        If (VDBG) {
                            Log("EVENT_GET_CELL_INFO_LIST: size=" + result.list->Size()
                                    + " list=" + result.list);
                        }
                    }
                    mLastCellInfoListTime = SystemClock->ElapsedRealtime();
                    mLastCellInfoList = result.list;
                    result.lockObj->Notify();
                }
                break;
            }

            case EVENT_UNSOL_CELL_INFO_LIST: {
                AsyncResult ar = (AsyncResult) msg.obj;
                If (ar.exception != NULL) {
                    Log("EVENT_UNSOL_CELL_INFO_LIST: error ignoring, e=" + ar.exception);
                } else {
                    List<CellInfo> list = (List<CellInfo>) ar.result;
                    If (DBG) {
                        Log("EVENT_UNSOL_CELL_INFO_LIST: size=" + list->Size()
                                + " list=" + list);
                    }
                    mLastCellInfoListTime = SystemClock->ElapsedRealtime();
                    mLastCellInfoList = list;
                    mPhoneBase->NotifyCellInfo(list);
                }
                break;
            }

            default:
                Log("Unhandled message with number: " + msg.what);
                break;
        }
    }

    protected abstract Phone GetPhone();
    protected abstract void HandlePollStateResult(Int32 what, AsyncResult ar);
    protected abstract void UpdateSpnDisplay();
    protected abstract void SetPowerStateToDesired();
    protected abstract void OnUpdateIccAvailability();
    protected abstract void Log(String s);
    protected abstract void Loge(String s);

    public abstract Int32 GetCurrentDataConnectionState();
    public abstract Boolean IsConcurrentVoiceAndDataAllowed();

    public abstract void SetImsRegistrationState(Boolean registered);
    public abstract void PollState();

    /**
     * Registration point for transition into DataConnection attached.
     * @param h handler to notify
     * @param what what code of message when delivered
     * @param obj placed in Message.obj
     */
    CARAPI RegisterForDataConnectionAttached(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant(h, what, obj);
        mAttachedRegistrants->Add(r);

        If (GetCurrentDataConnectionState() == ServiceState.STATE_IN_SERVICE) {
            r->NotifyRegistrant();
        }
    }
    CARAPI UnregisterForDataConnectionAttached(Handler h) {
        mAttachedRegistrants->Remove(h);
    }

    /**
     * Registration point for transition into DataConnection detached.
     * @param h handler to notify
     * @param what what code of message when delivered
     * @param obj placed in Message.obj
     */
    CARAPI RegisterForDataConnectionDetached(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant(h, what, obj);
        mDetachedRegistrants->Add(r);

        If (GetCurrentDataConnectionState() != ServiceState.STATE_IN_SERVICE) {
            r->NotifyRegistrant();
        }
    }
    CARAPI UnregisterForDataConnectionDetached(Handler h) {
        mDetachedRegistrants->Remove(h);
    }

    /**
     * Registration for DataConnection RIL Data Radio Technology changing. The
     * new radio technology will be returned AsyncResult#result as an Integer Object.
     * The AsyncResult will be in the notification Message#obj.
     *
     * @param h handler to notify
     * @param what what code of message when delivered
     * @param obj placed in Message.obj
     */
    CARAPI RegisterForDataRegStateOrRatChanged(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant(h, what, obj);
        mDataRegStateOrRatChangedRegistrants->Add(r);
        NotifyDataRegStateRilRadioTechnologyChanged();
    }
    CARAPI UnregisterForDataRegStateOrRatChanged(Handler h) {
        mDataRegStateOrRatChangedRegistrants->Remove(h);
    }

    /**
     * Registration point for transition into network attached.
     * @param h handler to notify
     * @param what what code of message when delivered
     * @param obj in Message.obj
     */
    CARAPI RegisterForNetworkAttached(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant(h, what, obj);

        mNetworkAttachedRegistrants->Add(r);
        If (mSS->GetVoiceRegState() == ServiceState.STATE_IN_SERVICE) {
            r->NotifyRegistrant();
        }
    }
    CARAPI UnregisterForNetworkAttached(Handler h) {
        mNetworkAttachedRegistrants->Remove(h);
    }

    /**
     * Registration point for transition into packet service restricted zone.
     * @param h handler to notify
     * @param what what code of message when delivered
     * @param obj placed in Message.obj
     */
    CARAPI RegisterForPsRestrictedEnabled(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant(h, what, obj);
        mPsRestrictEnabledRegistrants->Add(r);

        If (mRestrictedState->IsPsRestricted()) {
            r->NotifyRegistrant();
        }
    }

    CARAPI UnregisterForPsRestrictedEnabled(Handler h) {
        mPsRestrictEnabledRegistrants->Remove(h);
    }

    /**
     * Registration point for transition out of packet service restricted zone.
     * @param h handler to notify
     * @param what what code of message when delivered
     * @param obj placed in Message.obj
     */
    CARAPI RegisterForPsRestrictedDisabled(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant(h, what, obj);
        mPsRestrictDisabledRegistrants->Add(r);

        If (mRestrictedState->IsPsRestricted()) {
            r->NotifyRegistrant();
        }
    }

    CARAPI UnregisterForPsRestrictedDisabled(Handler h) {
        mPsRestrictDisabledRegistrants->Remove(h);
    }

    /**
     * Clean up existing voice and data connection then turn off radio power.
     *
     * Hang up the existing voice calls to decrease call drop rate.
     */
    CARAPI PowerOffRadioSafely(DcTrackerBase dcTracker) {
        {    AutoLock syncLock(this);
            If (!mPendingRadioPowerOffAfterDataOff) {
                // In some network, deactivate PDP connection cause releasing of RRC connection,
                // which MM/IMSI detaching request needs. Without this detaching, network can
                // not release the network resources previously attached.
                // So we are avoiding data detaching on these networks.
                String[] networkNotClearData = mPhoneBase->GetContext()->GetResources()
                        .GetStringArray(R.array.networks_not_clear_data);
                String currentNetwork = mSS->GetOperatorNumeric();
                If ((networkNotClearData != NULL) && (currentNetwork != NULL)) {
                    For (Int32 i = 0; i < networkNotClearData.length; i++) {
                        If (currentNetwork->Equals(networkNotClearData[i])) {
                            // Don't clear data connection for this carrier
                            If (DBG)
                                Log("Not disconnecting data for " + currentNetwork);
                            HangupAndPowerOff();
                            return;
                        }
                    }
                }
                // To minimize race conditions we call cleanUpAllConnections on
                // both if else paths instead of before this isDisconnected test.
                If (dcTracker->IsDisconnected()) {
                    // To minimize race conditions we do this after isDisconnected
                    dcTracker->CleanUpAllConnections(Phone.REASON_RADIO_TURNED_OFF);
                    If (DBG) Log("Data disconnected, turn off radio right away.");
                    HangupAndPowerOff();
                } else {
                    dcTracker->CleanUpAllConnections(Phone.REASON_RADIO_TURNED_OFF);
                    Message msg = Message->Obtain(this);
                    msg.what = EVENT_SET_RADIO_POWER_OFF;
                    msg.arg1 = ++mPendingRadioPowerOffAfterDataOffTag;
                    If (SendMessageDelayed(msg, 30000)) {
                        If (DBG) Log("Wait upto 30s for data to disconnect, then turn off radio.");
                        mPendingRadioPowerOffAfterDataOff = TRUE;
                    } else {
                        Log("Cannot send delayed Msg, turn off radio right away.");
                        HangupAndPowerOff();
                    }
                }
            }
        }
    }

    /**
     * process the pending request to turn radio off after data is disconnected
     *
     * return TRUE if there is pending request to process; FALSE otherwise.
     */
    public Boolean ProcessPendingRadioPowerOffAfterDataOff() {
        {    AutoLock syncLock(this);
            If (mPendingRadioPowerOffAfterDataOff) {
                If (DBG) Log("Process pending request to turn radio off.");
                mPendingRadioPowerOffAfterDataOffTag += 1;
                HangupAndPowerOff();
                mPendingRadioPowerOffAfterDataOff = FALSE;
                return TRUE;
            }
            return FALSE;
        }
    }

    /**
     * send signal-strength-changed notification if changed Called both for
     * solicited and unsolicited signal strength updates
     *
     * @return TRUE if the signal strength changed and a notification was sent.
     */
    protected Boolean OnSignalStrengthResult(AsyncResult ar, Boolean isGsm) {
        SignalStrength oldSignalStrength = mSignalStrength;

        // This signal is used for both voice and data radio signal so parse
        // all fields

        If ((ar.exception == NULL) && (ar.result != NULL)) {
            mSignalStrength = (SignalStrength) ar.result;
            mSignalStrength->ValidateInput();
            mSignalStrength->SetGsm(isGsm);
        } else {
            Log("OnSignalStrengthResult() Exception from RIL : " + ar.exception);
            mSignalStrength = new SignalStrength(isGsm);
        }

        return NotifySignalStrength();
    }

    /**
     * Hang up all voice call and turn off radio. Implemented by derived class.
     */
    protected abstract void HangupAndPowerOff();

    /** Cancel a Pending (if any) PollState() operation */
    protected void CancelPollState() {
        // This will effectively cancel the rest of the poll requests.
        mPollingContext = new Int32[1];
    }

    /**
     * Return TRUE if time zone needs fixing.
     *
     * @param phoneBase
     * @param operatorNumeric
     * @param prevOperatorNumeric
     * @param needToFixTimeZone
     * @return TRUE if time zone needs to be fixed
     */
    protected Boolean ShouldFixTimeZoneNow(PhoneBase phoneBase, String operatorNumeric,
            String prevOperatorNumeric, Boolean needToFixTimeZone) {
        // Return FALSE if the mcc isn't valid as we don't know where we are.
        // Return TRUE if we have an IccCard and the mcc changed or we
        // need to fix it because when the NITZ time came in we didn't
        // know the country code.

        // If mcc is invalid then we'll return FALSE
        Int32 mcc;
        try {
            mcc = Integer->ParseInt(operatorNumeric->Substring(0, 3));
        } Catch (Exception e) {
            If (DBG) {
                Log("shouldFixTimeZoneNow: no mcc, operatorNumeric=" + operatorNumeric +
                        " retVal=FALSE");
            }
            return FALSE;
        }

        // If prevMcc is invalid will make it different from mcc
        // so we'll return TRUE if the card exists.
        Int32 prevMcc;
        try {
            prevMcc = Integer->ParseInt(prevOperatorNumeric->Substring(0, 3));
        } Catch (Exception e) {
            prevMcc = mcc + 1;
        }

        // Determine if the Icc card exists
        Boolean iccCardExist = FALSE;
        If (mUiccApplcation != NULL) {
            iccCardExist = mUiccApplcation->GetState() != AppState.APPSTATE_UNKNOWN;
        }

        // Determine retVal
        Boolean retVal = ((iccCardExist && (mcc != prevMcc)) || needToFixTimeZone);
        If (DBG) {
            Int64 ctm = System->CurrentTimeMillis();
            Log("shouldFixTimeZoneNow: retVal=" + retVal +
                    " iccCardExist=" + iccCardExist +
                    " operatorNumeric=" + operatorNumeric + " mcc=" + mcc +
                    " prevOperatorNumeric=" + prevOperatorNumeric + " prevMcc=" + prevMcc +
                    " needToFixTimeZone=" + needToFixTimeZone +
                    " ltod=" + TimeUtils->LogTimeOfDay(ctm));
        }
        return retVal;
    }

    public String GetSystemProperty(String property, String defValue) {
        return TelephonyManager->GetTelephonyProperty(property, mPhoneBase->GetSubId(), defValue);
    }

    /**
     * @return all available cell information or NULL if none.
     */
    public List<CellInfo> GetAllCellInfo() {
        CellInfoResult result = new CellInfoResult();
        If (VDBG) Log("SST->GetAllCellInfo(): E");
        Int32 ver = mCi->GetRilVersion();
        If (ver >= 8) {
            If (IsCallerOnDifferentThread()) {
                If ((SystemClock->ElapsedRealtime() - mLastCellInfoListTime)
                        > LAST_CELL_INFO_LIST_MAX_AGE_MS) {
                    Message msg = ObtainMessage(EVENT_GET_CELL_INFO_LIST, result);
                    {    AutoLock syncLock(result.lockObj);
                        result.list = NULL;
                        mCi->GetCellInfoList(msg);
                        try {
                            result.lockObj->Wait(5000);
                        } Catch (InterruptedException e) {
                            e->PrintStackTrace();
                        }
                    }
                } else {
                    If (DBG) Log("SST->GetAllCellInfo(): return last, back to back calls");
                    result.list = mLastCellInfoList;
                }
            } else {
                If (DBG) Log("SST->GetAllCellInfo(): return last, same thread can't block");
                result.list = mLastCellInfoList;
            }
        } else {
            If (DBG) Log("SST->GetAllCellInfo(): not implemented");
            result.list = NULL;
        }
        {    AutoLock syncLock(result.lockObj);
            If (result.list != NULL) {
                If (DBG) Log("SST->GetAllCellInfo(): X size=" + result.list->Size()
                        + " list=" + result.list);
                return result.list;
            } else {
                If (DBG) Log("SST->GetAllCellInfo(): X size=0 list=NULL");
                return NULL;
            }
        }
    }

    /**
     * @return signal strength
     */
    public SignalStrength GetSignalStrength() {
        {    AutoLock syncLock(mCellInfo);
            return mSignalStrength;
        }
    }

    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("ServiceStateTracker:");
        pw->Println(" mSS=" + mSS);
        pw->Println(" mNewSS=" + mNewSS);
        pw->Println(" mCellInfo=" + mCellInfo);
        pw->Println(" mRestrictedState=" + mRestrictedState);
        pw->Println(" mPollingContext=" + mPollingContext);
        pw->Println(" mDesiredPowerState=" + mDesiredPowerState);
        pw->Println(" mDontPollSignalStrength=" + mDontPollSignalStrength);
        pw->Println(" mPendingRadioPowerOffAfterDataOff=" + mPendingRadioPowerOffAfterDataOff);
        pw->Println(" mPendingRadioPowerOffAfterDataOffTag=" + mPendingRadioPowerOffAfterDataOffTag);
    }

    /**
     * Verifies the current thread is the same as the thread originally
     * used in the initialization of this instance. Throws RuntimeException
     * if not.
     *
     * @exception RuntimeException if the current thread is not
     * the thread that originally obtained this PhoneBase instance.
     */
    protected void CheckCorrectThread() {
        If (Thread->CurrentThread() != GetLooper()->GetThread()) {
            throw new RuntimeException(
                    "ServiceStateTracker must be used from within one thread");
        }
    }

    protected Boolean IsCallerOnDifferentThread() {
        Boolean value = Thread->CurrentThread() != GetLooper()->GetThread();
        If (VDBG) Log("isCallerOnDifferentThread: " + value);
        return value;
    }

    protected void UpdateCarrierMccMncConfiguration(String newOp, String oldOp, Context context) {
        // if we have a change in operator, notify Wifi (even to/from none)
        If (((newOp == NULL) && (TextUtils->IsEmpty(oldOp) == FALSE)) ||
                ((newOp != NULL) && (newOp->Equals(oldOp) == FALSE))) {
            Log("update mccmnc=" + newOp + " fromServiceState=TRUE");
            MccTable->UpdateMccMncConfiguration(context, newOp, TRUE);
        }
    }

    protected Boolean IsIwlanFeatureAvailable() {
        Boolean iwlanAvailable = mPhoneBase->GetContext()->GetResources()
                .GetBoolean(R.bool.config_feature_iwlan_enabled);
        Log("Iwlan feature available = " + iwlanAvailable);
        return iwlanAvailable;
    }

    /* Consider the below usecase.
     * 1. WQE and IWLAN features are enabled and hence device can have wifi and mobile
     * connectivity simultaneously.
     * 2. Current available RATs are WWAN and IWLAN, both co-exists.
     * 3. RIL informs preferred RAT as WWAN. Telephony could have default and
     * non-default PDP activated. Since IWLAN is also available, non-default pdp
     * would be over IWLAN.
     * 4. WWAN goes to OOS.
     * 5. RIL informs that default PDP is Lost(unsol_data_call_list).
     * 6. Telephony attempts to retry the default APN context.
     * 7. RIL informs current preferred RAT is IWLAN.
     * 8. Telephony marks default/ia APN as "not available".
     * 9. Upon retry timer expiration, telephony does not bringup default APN
     * since is marked as unavailable. DC object moved to IDLE state.
     * 10 Later WWAN gets back in service.
     * 11. RIL informs preferred RAT as WWAN. So the RAT transition was as
     * follow.
     * IWLAN(Attached) -> WWAN(Attached)
     * 12. There is no trigger for telephony to initiate data call on
     * connnectable ApnContext after WWAN gets in service.
     * 13 Below method detects the transition from IWLAN to WWAN in attached
     * state and informs telephony that we are in WWAN attached state.
     * 14. Telephony would look into all the connectable APNs are would trigger
     * data call based on prevailing conditions.
     *
     */
    protected void ProcessIwlanToWwanTransition(ServiceState ss) {
        // Wifi Connected(iwlan feature on) AND a Valid(non-wlan) RAT present
        // AND attached AND previous RAT was iwlan.
        //
        // Notify that we are attached so that we can setup connectable
        // APNs.
        If (IsIwlanFeatureAvailable() &&
                (ss->GetRilDataRadioTechnology()
                 != ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN) &&
                (ss->GetRilDataRadioTechnology()
                 != ServiceState.RIL_RADIO_TECHNOLOGY_UNKNOWN) &&
                (ss->GetDataRegState() == ServiceState.STATE_IN_SERVICE) &&
                (mIwlanRatAvailable == TRUE)) {

            Log("pollStateDone: Wifi connected and moved out of iwlan " +
                    "and wwan is attached.");
            mAttachedRegistrants->NotifyRegistrants();
        }

    }

}
