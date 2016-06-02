/*
 * Copyright (C) 2012 The Android Open Source Project
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

package com.android.internal.telephony.cdma;

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Telephony::ICellInfo;
using Elastos::Droid::Telephony::ICellInfoCdma;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::Cdma::ICdmaCellLocation;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::IEventLog;
using Elastos::Droid::Utility::ITimeUtils;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::CommandsInterface::IRadioState;
using Elastos::Droid::Internal::Telephony::IEventLogTags;
using Elastos::Droid::Internal::Telephony::IMccTable;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::IServiceStateTracker;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDcTrackerBase;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::IHbpcdUtils;
using Elastos::Droid::Internal::Telephony::Uicc::IRuimRecords;
using Elastos::Droid::Internal::Telephony::IOperators;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrays;
using Elastos::Utility::ICalendar;
using Elastos::Utility::IDate;
using Elastos::Utility::IList;
using Elastos::Utility::ITimeZone;

/**
 * {@hide}
 */
public class CdmaServiceStateTracker extends ServiceStateTracker {
    static const String LOG_TAG = "CdmaSST";

    CDMAPhone mPhone;
    CdmaCellLocation mCellLoc;
    CdmaCellLocation mNewCellLoc;

    // Min values used to by GetOtasp()
    private static const String UNACTIVATED_MIN2_VALUE = "000000";
    private static const String UNACTIVATED_MIN_VALUE = "1111110111";

    private static const Int32 MS_PER_HOUR = 60 * 60 * 1000;

    // Current Otasp value
    Int32 mCurrentOtaspMode = OTASP_UNINITIALIZED;

     /** if time between NITZ updates is less than mNitzUpdateSpacing the update may be ignored. */
    private static const Int32 NITZ_UPDATE_SPACING_DEFAULT = 1000 * 60 * 10;
    private Int32 mNitzUpdateSpacing = SystemProperties->GetInt("ro.nitz_update_spacing",
            NITZ_UPDATE_SPACING_DEFAULT);

    /** If mNitzUpdateSpacing hasn't been exceeded but update is > mNitzUpdate do the update */
    private static const Int32 NITZ_UPDATE_DIFF_DEFAULT = 2000;
    private Int32 mNitzUpdateDiff = SystemProperties->GetInt("ro.nitz_update_diff",
            NITZ_UPDATE_DIFF_DEFAULT);

    private Boolean mCdmaRoaming = FALSE;
    protected Boolean mDataRoaming = FALSE;
    private Int32 mRoamingIndicator = EriInfo.ROAMING_INDICATOR_OFF;
    private Boolean mIsInPrl;
    private Int32 mDefaultRoamingIndicator = EriInfo.ROAMING_INDICATOR_OFF;

    /**
     * Initially assume no data connection.
     */
    protected Int32 mRegistrationState = -1;
    protected RegistrantList mCdmaForSubscriptionInfoReadyRegistrants = new RegistrantList();

    /**
     * Sometimes we get the NITZ time before we know what country we
     * are in. Keep the time zone information from the NITZ string so
     * we can fix the time zone once know the country.
     */
    protected Boolean mNeedFixZone = FALSE;
    private Int32 mZoneOffset;
    private Boolean mZoneDst;
    private Int64 mZoneTime;
    protected Boolean mGotCountryCode = FALSE;
    String mSavedTimeZone;
    Int64 mSavedTime;
    Int64 mSavedAtTime;

    /** Wake lock used while setting time of day. */
    private PowerManager.WakeLock mWakeLock;
    private static const String WAKELOCK_TAG = "ServiceStateTracker";

    /** Contains the name of the registered network in CDMA (either ONS or ERI text). */
    protected String mCurPlmn = NULL;
    protected String mCurSpn = NULL;
    protected Boolean mCurShowPlmn = FALSE;
    protected Boolean mCurShowSpn = FALSE;

    protected String mMdn;
    protected Int32 mHomeSystemId[] = NULL;
    protected Int32 mHomeNetworkId[] = NULL;
    protected String mMin;
    protected String mPrlVersion;
    protected Boolean mIsMinInfoReady = FALSE;

    private Boolean mIsEriTextLoaded = FALSE;
    protected Boolean mIsSubscriptionFromRuim = FALSE;
    private CdmaSubscriptionSourceManager mCdmaSSM;

    protected static const String INVALID_MCC = "000";
    protected static const String DEFAULT_MNC = "00";

    protected HbpcdUtils mHbpcdUtils = NULL;

    /* Used only for debugging purposes. */
    private String mRegistrationDeniedReason;

    private ContentResolver mCr;
    private String mCurrentCarrier = NULL;

    private ContentObserver mAutoTimeObserver = new ContentObserver(new Handler()) {
        //@Override
        CARAPI OnChange(Boolean selfChange) {
            If (DBG) Log("Auto time state changed");
            RevertToNitzTime();
        }
    };

    private ContentObserver mAutoTimeZoneObserver = new ContentObserver(new Handler()) {
        //@Override
        CARAPI OnChange(Boolean selfChange) {
            If (DBG) Log("Auto time zone state changed");
            RevertToNitzTimeZone();
        }
    };

    public CdmaServiceStateTracker(CDMAPhone phone) {
        This(phone, new CellInfoCdma());
    }

    protected CdmaServiceStateTracker(CDMAPhone phone, CellInfo cellInfo) {
        Super(phone, phone.mCi, cellInfo);

        mPhone = phone;
        mCr = phone->GetContext()->GetContentResolver();
        mCellLoc = new CdmaCellLocation();
        mNewCellLoc = new CdmaCellLocation();

        mCdmaSSM = CdmaSubscriptionSourceManager->GetInstance(phone->GetContext(), mCi, this,
                EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED, NULL);
        mIsSubscriptionFromRuim = (mCdmaSSM->GetCdmaSubscriptionSource() ==
                          CdmaSubscriptionSourceManager.SUBSCRIPTION_FROM_RUIM);

        PowerManager powerManager =
                (PowerManager)phone->GetContext()->GetSystemService(Context.POWER_SERVICE);
        mWakeLock = powerManager->NewWakeLock(PowerManager.PARTIAL_WAKE_LOCK, WAKELOCK_TAG);

        mCi->RegisterForRadioStateChanged(this, EVENT_RADIO_STATE_CHANGED, NULL);

        mCi->RegisterForVoiceNetworkStateChanged(this, EVENT_NETWORK_STATE_CHANGED_CDMA, NULL);
        mCi->SetOnNITZTime(this, EVENT_NITZ_TIME, NULL);

        mCi->RegisterForCdmaPrlChanged(this, EVENT_CDMA_PRL_VERSION_CHANGED, NULL);
        phone->RegisterForEriFileLoaded(this, EVENT_ERI_FILE_LOADED, NULL);
        mCi->RegisterForCdmaOtaProvision(this,EVENT_OTA_PROVISION_STATUS_CHANGE, NULL);

        // System setting property AIRPLANE_MODE_ON is set in Settings.
        Int32 airplaneMode = Settings::Global::>GetInt(mCr, Settings::Global::AIRPLANE_MODE_ON, 0);
        mDesiredPowerState = ! (airplaneMode > 0);

        mCr->RegisterContentObserver(
                Settings::Global::>GetUriFor(Settings::Global::AUTO_TIME), TRUE,
                mAutoTimeObserver);
        mCr->RegisterContentObserver(
            Settings::Global::>GetUriFor(Settings::Global::AUTO_TIME_ZONE), TRUE,
            mAutoTimeZoneObserver);
        SetSignalStrengthDefaultValues();

        mHbpcdUtils = new HbpcdUtils(phone->GetContext());

        // Reset OTASP state in case previously set by another service
        phone->NotifyOtaspChanged(OTASP_UNINITIALIZED);
    }

    //@Override
    CARAPI Dispose() {
        CheckCorrectThread();
        Log("ServiceStateTracker dispose");

        // Unregister for all events.
        mCi->UnregisterForRadioStateChanged(this);
        mCi->UnregisterForVoiceNetworkStateChanged(this);
        mCi->UnregisterForCdmaOtaProvision(this);
        mPhone->UnregisterForEriFileLoaded(this);
        UnregisterForRuimEvents();
        mCi->UnSetOnNITZTime(this);
        mCr->UnregisterContentObserver(mAutoTimeObserver);
        mCr->UnregisterContentObserver(mAutoTimeZoneObserver);
        mCdmaSSM->Dispose(this);
        mCi->UnregisterForCdmaPrlChanged(this);
        super->Dispose();
    }

    //@Override
    protected void Finalize() {
        If (DBG) Log("CdmaServiceStateTracker finalized");
    }

    /**
     * Registration point for subscription info ready
     * @param h handler to notify
     * @param what what code of message when delivered
     * @param obj placed in Message.obj
     */
    CARAPI RegisterForSubscriptionInfoReady(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant(h, what, obj);
        mCdmaForSubscriptionInfoReadyRegistrants->Add(r);

        If (IsMinInfoReady()) {
            r->NotifyRegistrant();
        }
    }

    CARAPI UnregisterForSubscriptionInfoReady(Handler h) {
        mCdmaForSubscriptionInfoReadyRegistrants->Remove(h);
    }

    /**
     * Save current source of cdma subscription
     * @param source - 1 for NV, 0 for RUIM
     */
    private void SaveCdmaSubscriptionSource(Int32 source) {
        Log("Storing cdma subscription source: " + source);
        Settings::Global::>PutInt(mPhone->GetContext()->GetContentResolver(),
                Settings::Global::CDMA_SUBSCRIPTION_MODE,
                source );
    }

    private void GetSubscriptionInfoAndStartPollingThreads() {
        mCi->GetCDMASubscription(ObtainMessage(EVENT_POLL_STATE_CDMA_SUBSCRIPTION));

        // Get Registration Information
        PollState();
    }

    //@Override
    CARAPI HandleMessage (Message msg) {
        AsyncResult ar;
        Int32[] ints;
        String[] strings;

        If (!mPhone.mIsTheCurrentActivePhone) {
            Loge("Received message " + msg + "[" + msg.what + "]" +
                    " while being destroyed. Ignoring.");
            return;
        }

        Switch (msg.what) {
        case EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED:
            HandleCdmaSubscriptionSource(mCdmaSSM->GetCdmaSubscriptionSource());
            break;

        case EVENT_RUIM_READY:
            Int32 networkType = PhoneFactory->CalculatePreferredNetworkType(mPhone->GetContext(),
                    mPhone->GetPhoneId());
            mCi->SetPreferredNetworkType(networkType, NULL);

            If (DBG) Log("Receive EVENT_RUIM_READY");
            PollState();

            Boolean skipRestoringSelection = mPhone->GetContext()->GetResources().GetBoolean(
                    R.bool.skip_restoring_network_selection);
            If (!skipRestoringSelection) {
                 // Only support automatic selection mode in CDMA.
                 mPhone->SetNetworkSelectionModeAutomatic(NULL);
            }

            mPhone->PrepareEri();
            break;

        case EVENT_NV_READY:
            UpdatePhoneObject();

            // Only support automatic selection mode in CDMA.
            mPhone->SetNetworkSelectionModeAutomatic(NULL);

            // For Non-RUIM phones, the subscription information is stored in
            // Non Volatile. Here when Non-Volatile is ready, we can poll the CDMA
            // subscription info.
            GetSubscriptionInfoAndStartPollingThreads();
            break;

        case EVENT_RADIO_STATE_CHANGED:
            If(mCi->GetRadioState() == RadioState.RADIO_ON) {
                HandleCdmaSubscriptionSource(mCdmaSSM->GetCdmaSubscriptionSource());

                // Signal strength polling stops when radio is off.
                QueueNextSignalStrengthPoll();
            }
            // This will do nothing in the 'radio not available' case.
            SetPowerStateToDesired();
            PollState();
            break;

        case EVENT_NETWORK_STATE_CHANGED_CDMA:
            PollState();
            break;

        case EVENT_GET_SIGNAL_STRENGTH:
            // This callback is called when signal strength is polled
            // all by itself.

            If (!(mCi->GetRadioState()->IsOn())) {
                // Polling will continue when radio turns back on.
                return;
            }
            ar = (AsyncResult) msg.obj;
            OnSignalStrengthResult(ar, FALSE);
            QueueNextSignalStrengthPoll();

            break;

        case EVENT_GET_LOC_DONE_CDMA:
            ar = (AsyncResult) msg.obj;

            If (ar.exception == NULL) {
                String states[] = (String[])ar.result;
                Int32 baseStationId = -1;
                Int32 baseStationLatitude = CdmaCellLocation.INVALID_LAT_LONG;
                Int32 baseStationLongitude = CdmaCellLocation.INVALID_LAT_LONG;
                Int32 systemId = -1;
                Int32 networkId = -1;

                If (states.length > 9) {
                    try {
                        If (states[4] != NULL) {
                            baseStationId = Integer->ParseInt(states[4]);
                        }
                        If (states[5] != NULL) {
                            baseStationLatitude = Integer->ParseInt(states[5]);
                        }
                        If (states[6] != NULL) {
                            baseStationLongitude = Integer->ParseInt(states[6]);
                        }
                        // Some carriers only return lat-lngs of 0,0
                        If (baseStationLatitude == 0 && baseStationLongitude == 0) {
                            baseStationLatitude  = CdmaCellLocation.INVALID_LAT_LONG;
                            baseStationLongitude = CdmaCellLocation.INVALID_LAT_LONG;
                        }
                        If (states[8] != NULL) {
                            systemId = Integer->ParseInt(states[8]);
                        }
                        If (states[9] != NULL) {
                            networkId = Integer->ParseInt(states[9]);
                        }
                    } Catch (NumberFormatException ex) {
                        Loge("error parsing cell location data: " + ex);
                    }
                }

                mCellLoc->SetCellLocationData(baseStationId, baseStationLatitude,
                        baseStationLongitude, systemId, networkId);
                mPhone->NotifyLocationChanged();
            }

            // Release any temporary cell lock, which could have been
            // acquired to allow a single-shot location update.
            DisableSingleLocationUpdate();
            break;

        case EVENT_POLL_STATE_REGISTRATION_CDMA:
        case EVENT_POLL_STATE_OPERATOR_CDMA:
        case EVENT_POLL_STATE_GPRS:
            ar = (AsyncResult) msg.obj;
            HandlePollStateResult(msg.what, ar);
            break;

        case EVENT_POLL_STATE_CDMA_SUBSCRIPTION: // Handle RIL_CDMA_SUBSCRIPTION
            ar = (AsyncResult) msg.obj;

            If (ar.exception == NULL) {
                String cdmaSubscription[] = (String[])ar.result;
                If (cdmaSubscription != NULL && cdmaSubscription.length >= 5) {
                    If (NULL != cdmaSubscription[0]) {
                        mMdn = cdmaSubscription[0];
                    }
                    ParseSidNid(cdmaSubscription[1], cdmaSubscription[2]);

                    If (NULL != cdmaSubscription[3]) {
                        mMin = cdmaSubscription[3];
                    }
                    If (NULL != cdmaSubscription[4]) {
                        mPrlVersion = cdmaSubscription[4];
                    }
                    If (DBG) Log("GET_CDMA_SUBSCRIPTION: MDN=" + mMdn);

                    mIsMinInfoReady = TRUE;

                    UpdateOtaspState();
                    If (!mIsSubscriptionFromRuim && mIccRecords != NULL) {
                        If (DBG) {
                            Log("GET_CDMA_SUBSCRIPTION set imsi in mIccRecords");
                        }
                        mIccRecords->SetImsi(GetImsi());
                    } else {
                        If (DBG) {
                            Log("GET_CDMA_SUBSCRIPTION either mIccRecords is NULL  or NV type device" +
                                    " - not setting Imsi in mIccRecords");
                        }
                    }
                } else {
                    If (DBG) {
                        Log("GET_CDMA_SUBSCRIPTION: error parsing cdmaSubscription params num="
                            + cdmaSubscription.length);
                    }
                }
            }
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
            // This is a notification from CommandsInterface.setOnSignalStrengthUpdate.

            ar = (AsyncResult) msg.obj;

            // The radio is telling us about signal strength changes,
            // so we don't have to ask it.
            mDontPollSignalStrength = TRUE;

            OnSignalStrengthResult(ar, FALSE);
            break;

        case EVENT_RUIM_RECORDS_LOADED:
            Log("EVENT_RUIM_RECORDS_LOADED: what=" + msg.what);
            UpdatePhoneObject();
            RuimRecords ruim = (RuimRecords)mIccRecords;
            If ((ruim != NULL) && ruim->IsProvisioned()) {
                mMdn = ruim->GetMdn();
                mMin = ruim->GetMin();
                ParseSidNid(ruim->GetSid(), ruim->GetNid());
                mPrlVersion = ruim->GetPrlVersion();
                mIsMinInfoReady = TRUE;
                UpdateOtaspState();
            }
            // SID/NID/PRL is loaded. Poll service state
            // again to update to the roaming state with
            // the latest variables.
            GetSubscriptionInfoAndStartPollingThreads();
            break;

        case EVENT_LOCATION_UPDATES_ENABLED:
            ar = (AsyncResult) msg.obj;

            If (ar.exception == NULL) {
                mCi->GetVoiceRegistrationState(ObtainMessage(EVENT_GET_LOC_DONE_CDMA, NULL));
            }
            break;

        case EVENT_ERI_FILE_LOADED:
            // Repoll the state once the ERI file has been loaded.
            If (DBG) Log("[CdmaServiceStateTracker] ERI file has been loaded, repolling.");
            PollState();
            break;

        case EVENT_OTA_PROVISION_STATUS_CHANGE:
            ar = (AsyncResult)msg.obj;
            If (ar.exception == NULL) {
                ints = (Int32[]) ar.result;
                Int32 otaStatus = ints[0];
                If (otaStatus == Phone.CDMA_OTA_PROVISION_STATUS_COMMITTED
                    || otaStatus == Phone.CDMA_OTA_PROVISION_STATUS_OTAPA_STOPPED) {
                    If (DBG) Log("EVENT_OTA_PROVISION_STATUS_CHANGE: Complete, Reload MDN");
                    mCi->GetCDMASubscription( ObtainMessage(EVENT_POLL_STATE_CDMA_SUBSCRIPTION));
                }
            }
            break;

        case EVENT_CDMA_PRL_VERSION_CHANGED:
            ar = (AsyncResult)msg.obj;
            If (ar.exception == NULL) {
                ints = (Int32[]) ar.result;
                mPrlVersion = Integer->ToString(ints[0]);
            }
            break;

        case EVENT_CHANGE_IMS_STATE:
            If (DBG) Log("EVENT_CHANGE_IMS_STATE");
            SetPowerStateToDesired();
            break;

        default:
            super->HandleMessage(msg);
        break;
        }
    }

    //***** Private Instance Methods

    private void HandleCdmaSubscriptionSource(Int32 newSubscriptionSource) {
        Log("Subscription Source : " + newSubscriptionSource);
        mIsSubscriptionFromRuim =
            (newSubscriptionSource == CdmaSubscriptionSourceManager.SUBSCRIPTION_FROM_RUIM);
        SaveCdmaSubscriptionSource(newSubscriptionSource);
        If (!mIsSubscriptionFromRuim) {
            // Unregister from any previous RUIM events if registered
            // (switching from RUIM/SIM to NV)
            UnregisterForRuimEvents();
            // NV is ready when subscription source is NV
            SendMessage(ObtainMessage(EVENT_NV_READY));
        } else {
            RegisterForRuimEvents();
        }
    }

    //@Override
    protected void SetPowerStateToDesired() {
        // If we want it on and it's off, turn it on
        If (mDesiredPowerState
            && mCi->GetRadioState() == CommandsInterface.RadioState.RADIO_OFF) {
            mCi->SetRadioPower(TRUE, NULL);
        } else If (!mDesiredPowerState && mCi->GetRadioState()->IsOn()) {
            DcTrackerBase dcTracker = mPhone.mDcTracker;

            // If it's on and available and we want it off gracefully
            PowerOffRadioSafely(dcTracker);
        } else If (mDeviceShuttingDown && mCi->GetRadioState()->IsAvailable()) {
            mCi->RequestShutdown(NULL);
        }
    }

    //@Override
    protected void UpdateSpnDisplay() {
        // mOperatorAlphaLong contains the ERI text
        String plmn = NULL;
        String spn = NULL;
        Boolean showPlmn = FALSE;
        Boolean showSpn = FALSE;
        Int32 rule = 0;
        Int32 combinedRegState;

        rule = (mIccRecords != NULL) ? mIccRecords->GetDisplayRule(mSS->GetOperatorNumeric()) : 0;
        combinedRegState = GetCombinedRegState();

        If (combinedRegState == ServiceState.STATE_OUT_OF_SERVICE) {
            // display out of service
            showPlmn = TRUE;
            plmn = Resources->GetSystem()->GetText(com.android.internal.
                    R::string::lockscreen_carrier_default).ToString();
            If (DBG) Log("updateSpnDisplay: radio is on but out " +
                    "of service, set plmn='" + plmn + "'");
        } else If (combinedRegState == ServiceState.STATE_IN_SERVICE) {
            plmn = TextUtils->IsEmpty(mSS->GetOperatorAlphaLong()) ? SystemProperties->Get(
                            "ro.cdma.home.operator.alpha", "") : mSS->GetOperatorAlphaLong();
            // depends on the rule and whether plmn or spn is NULL
            showPlmn = ( !TextUtils->IsEmpty(plmn)) &&
                    ((rule & RuimRecords.SPN_RULE_SHOW_PLMN) == RuimRecords.SPN_RULE_SHOW_PLMN);
            spn = (mIccRecords != NULL) ? mIccRecords->GetServiceProviderName() : "";
            showSpn = (!TextUtils->IsEmpty(spn)) &&
                    ((rule & RuimRecords.SPN_RULE_SHOW_SPN) == RuimRecords.SPN_RULE_SHOW_SPN);
        } else {
            // power off State (airplane mode), show nothing
        }

        // Update if any value changes
        If (showPlmn != mCurShowPlmn
                || showSpn != mCurShowSpn
                || !TextUtils->Equals(spn, mCurSpn)
                || !TextUtils->Equals(plmn, mCurPlmn)) {

            If (DBG) {
                Log(String->Format("updateSpnDisplay: changed sending intent" +
                            " showPlmn='%b' plmn='%s'", showPlmn, plmn));
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

    //@Override
    protected Phone GetPhone() {
        return mPhone;
    }

    /**
    * Hanlde the PollStateResult message
    */
    protected void HandlePollStateResultMessage(Int32 what, AsyncResult ar){
        Int32 ints[];
        String states[];
        Switch (what) {
            case EVENT_POLL_STATE_GPRS:
                states = (String[])ar.result;
                If (DBG) {
                    Log("handlePollStateResultMessage: EVENT_POLL_STATE_GPRS states.length=" +
                            states.length + " states=" + states);
                }

                Int32 regState = ServiceState.RIL_REG_STATE_UNKNOWN;
                Int32 dataRadioTechnology = 0;

                If (states.length > 0) {
                    try {
                        regState = Integer->ParseInt(states[0]);

                        // states[3] (if present) is the current radio technology
                        If (states.length >= 4 && states[3] != NULL) {
                            dataRadioTechnology = Integer->ParseInt(states[3]);
                        }
                    } Catch (NumberFormatException ex) {
                        Loge("handlePollStateResultMessage: error parsing GprsRegistrationState: "
                                        + ex);
                    }
                }

                Int32 dataRegState = RegCodeToServiceState(regState);
                mNewSS->SetDataRegState(dataRegState);
                mNewSS->SetRilDataRadioTechnology(dataRadioTechnology);
                If (DBG) {
                    Log("handlPollStateResultMessage: cdma setDataRegState=" + dataRegState
                            + " regState=" + regState
                            + " dataRadioTechnology=" + dataRadioTechnology);
                }
                break;

            case EVENT_POLL_STATE_REGISTRATION_CDMA: // Handle RIL_REQUEST_REGISTRATION_STATE.
                states = (String[])ar.result;

                Int32 registrationState = 4;     //[0] registrationState
                Int32 radioTechnology = -1;      //[3] radioTechnology
                Int32 baseStationId = -1;        //[4] baseStationId
                //[5] baseStationLatitude
                Int32 baseStationLatitude = CdmaCellLocation.INVALID_LAT_LONG;
                //[6] baseStationLongitude
                Int32 baseStationLongitude = CdmaCellLocation.INVALID_LAT_LONG;
                Int32 cssIndicator = 0;          //[7] init with 0, because it is treated as a Boolean
                Int32 systemId = 0;              //[8] systemId
                Int32 networkId = 0;             //[9] networkId
                Int32 roamingIndicator = EriInfo.ROAMING_INDICATOR_OFF;     //[10] Roaming indicator
                Int32 systemIsInPrl = 0;         //[11] Indicates if current system is in PRL
                Int32 defaultRoamingIndicator = EriInfo.ROAMING_INDICATOR_OFF;  //[12] def RI from PRL
                Int32 reasonForDenial = 0;       //[13] Denial reason if registrationState = 3

                If (states.length >= 14) {
                    try {
                        If (states[0] != NULL) {
                            registrationState = Integer->ParseInt(states[0]);
                        }
                        If (states[3] != NULL) {
                            radioTechnology = Integer->ParseInt(states[3]);
                        }
                        If (states[4] != NULL) {
                            baseStationId = Integer->ParseInt(states[4]);
                        }
                        If (states[5] != NULL) {
                            baseStationLatitude = Integer->ParseInt(states[5]);
                        }
                        If (states[6] != NULL) {
                            baseStationLongitude = Integer->ParseInt(states[6]);
                        }
                        // Some carriers only return lat-lngs of 0,0
                        If (baseStationLatitude == 0 && baseStationLongitude == 0) {
                            baseStationLatitude  = CdmaCellLocation.INVALID_LAT_LONG;
                            baseStationLongitude = CdmaCellLocation.INVALID_LAT_LONG;
                        }
                        If (states[7] != NULL) {
                            cssIndicator = Integer->ParseInt(states[7]);
                        }
                        If (states[8] != NULL) {
                            systemId = Integer->ParseInt(states[8]);
                        }
                        If (states[9] != NULL) {
                            networkId = Integer->ParseInt(states[9]);
                        }
                        If (states[10] != NULL) {
                            roamingIndicator = Integer->ParseInt(states[10]);
                        }
                        If (states[11] != NULL) {
                            systemIsInPrl = Integer->ParseInt(states[11]);
                        }
                        If (states[12] != NULL) {
                            defaultRoamingIndicator = Integer->ParseInt(states[12]);
                        }
                        If (states[13] != NULL) {
                            reasonForDenial = Integer->ParseInt(states[13]);
                        }
                    } Catch (NumberFormatException ex) {
                        Loge("EVENT_POLL_STATE_REGISTRATION_CDMA: error parsing: " + ex);
                    }
                } else {
                    throw new RuntimeException("Warning! Wrong number of parameters returned from "
                                         + "RIL_REQUEST_REGISTRATION_STATE: expected 14 or more "
                                         + "strings and got " + states.length + " strings");
                }

                mRegistrationState = registrationState;
                // When registration state is roaming and TSB58
                // roaming indicator is not in the carrier-specified
                // list of ERIs for home system, mCdmaRoaming is TRUE.
                mCdmaRoaming =
                        RegCodeIsRoaming(registrationState) && !IsRoamIndForHomeSystem(states[10]);
                mCdmaRoaming = mCdmaRoaming || mDataRoaming;
                mNewSS.SetState (RegCodeToServiceState(registrationState));

                mNewSS->SetRilVoiceRadioTechnology(radioTechnology);

                mNewSS->SetCssIndicator(cssIndicator);
                mNewSS->SetSystemAndNetworkId(systemId, networkId);
                mRoamingIndicator = roamingIndicator;
                mIsInPrl = (systemIsInPrl == 0) ? FALSE : TRUE;
                mDefaultRoamingIndicator = defaultRoamingIndicator;


                // Values are -1 if not available.
                mNewCellLoc->SetCellLocationData(baseStationId, baseStationLatitude,
                        baseStationLongitude, systemId, networkId);

                If (reasonForDenial == 0) {
                    mRegistrationDeniedReason = ServiceStateTracker.REGISTRATION_DENIED_GEN;
                } else If (reasonForDenial == 1) {
                    mRegistrationDeniedReason = ServiceStateTracker.REGISTRATION_DENIED_AUTH;
                } else {
                    mRegistrationDeniedReason = "";
                }

                If (mRegistrationState == 3) {
                    If (DBG) Log("Registration denied, " + mRegistrationDeniedReason);
                }
                break;

            case EVENT_POLL_STATE_OPERATOR_CDMA: // Handle RIL_REQUEST_OPERATOR
                String opNames[] = (String[])ar.result;

                If (opNames != NULL && opNames.length >= 3) {
                    // TODO: Do we care about overriding in this case.
                    // If the NUMERIC field isn't valid use PROPERTY_CDMA_HOME_OPERATOR_NUMERIC
                    If ((opNames[2] == NULL) || (opNames[2].Length() < 5)
                            || ("00000".Equals(opNames[2]))) {
                        opNames[2] = SystemProperties->Get(
                                CDMAPhone.PROPERTY_CDMA_HOME_OPERATOR_NUMERIC, "00000");
                        If (DBG) {
                            Log("RIL_REQUEST_OPERATOR.response[2], the numeric, " +
                                    " is bad. Using SystemProperties '" +
                                            CDMAPhone.PROPERTY_CDMA_HOME_OPERATOR_NUMERIC +
                                    "'= " + opNames[2]);
                        }
                    }

                    If (!mIsSubscriptionFromRuim) {
                        // In CDMA in case on NV, the ss.mOperatorAlphaLong is set later with the
                        // ERI text, so here it is ignored what is coming from the modem.
                        mNewSS->SetOperatorName(NULL, opNames[1], opNames[2]);
                    } else {
                        String brandOverride = mUiccController->GetUiccCard() != NULL ?
                            mUiccController->GetUiccCard()->GetOperatorBrandOverride() : NULL;
                        If (brandOverride != NULL) {
                            mNewSS->SetOperatorName(brandOverride, brandOverride, opNames[2]);
                        } else If (SystemProperties->GetBoolean("ro.cdma.force_plmn_lookup",
                                FALSE)) {
                            mNewSS->SetOperatorName(
                                Operators->OperatorReplace(opNames[2]),
                                opNames[1],
                                opNames[2]);
                        } else {
                            mNewSS->SetOperatorName(opNames[0], opNames[1], opNames[2]);
                        }
                    }
                } else {
                    If (DBG) Log("EVENT_POLL_STATE_OPERATOR_CDMA: error parsing opNames");
                }
                break;

            default:
                Loge("handlePollStateResultMessage: RIL response handle in wrong phone!"
                        + " Expected CDMA RIL request and get GSM RIL request.");
                break;
        }
    }

    /**
     * Handle the result of one of the PollState() - related requests
     */
    //@Override
    protected void HandlePollStateResult(Int32 what, AsyncResult ar) {
        // Ignore stale requests from last poll.
        If (ar.userObj != mPollingContext) return;

        If (ar.exception != NULL) {
            CommandException.Error err=NULL;

            If (ar.exception instanceof CommandException) {
                err = ((CommandException)(ar.exception)).GetCommandError();
            }

            If (err == CommandException.Error.RADIO_NOT_AVAILABLE) {
                // Radio has crashed or turned off.
                CancelPollState();
                return;
            }


            If (err != CommandException.Error.OP_NOT_ALLOWED_BEFORE_REG_NW) {
                Loge("handlePollStateResult: RIL returned an error where it must succeed"
                        + ar.exception);
            }
        } else try {
            HandlePollStateResultMessage(what, ar);
        } Catch (RuntimeException ex) {
            Loge("handlePollStateResult: Exception while polling service state. "
                    + "Probably malformed RIL response." + ex);
        }

        mPollingContext[0]--;

        If (mPollingContext[0] == 0) {
            Boolean namMatch = FALSE;
            If (!IsSidsAllZeros() && IsHomeSid(mNewSS->GetSystemId())) {
                namMatch = TRUE;
            }

            // Setting SS Roaming (general)
            If (mIsSubscriptionFromRuim) {
                mNewSS->SetRoaming(IsRoamingBetweenOperators(mCdmaRoaming, mNewSS));
            } else {
                mNewSS->SetRoaming(mCdmaRoaming);
            }

            // Setting SS CdmaRoamingIndicator and CdmaDefaultRoamingIndicator
            mNewSS->SetCdmaDefaultRoamingIndicator(mDefaultRoamingIndicator);
            mNewSS->SetCdmaRoamingIndicator(mRoamingIndicator);
            Boolean isPrlLoaded = TRUE;
            If (TextUtils->IsEmpty(mPrlVersion)) {
                isPrlLoaded = FALSE;
            }
            If (!isPrlLoaded || (mNewSS->GetRilVoiceRadioTechnology()
                                        == ServiceState.RIL_RADIO_TECHNOLOGY_UNKNOWN)) {
                Log("Turn off roaming indicator if !isPrlLoaded or voice RAT is unknown");
                mNewSS->SetCdmaRoamingIndicator(EriInfo.ROAMING_INDICATOR_OFF);
            } else If (!IsSidsAllZeros()) {
                If (!namMatch && !mIsInPrl) {
                    // Use default
                    mNewSS->SetCdmaRoamingIndicator(mDefaultRoamingIndicator);
                } else If (namMatch && !mIsInPrl) {
                    // TODO this will be removed when we handle roaming on LTE on CDMA+LTE phones
                    If (mNewSS->GetRilVoiceRadioTechnology()
                            == ServiceState.RIL_RADIO_TECHNOLOGY_LTE) {
                        Log("Turn off roaming indicator as voice is LTE");
                        mNewSS->SetCdmaRoamingIndicator(EriInfo.ROAMING_INDICATOR_OFF);
                    } else {
                        mNewSS->SetCdmaRoamingIndicator(EriInfo.ROAMING_INDICATOR_FLASH);
                    }
                } else If (!namMatch && mIsInPrl) {
                    // Use the one from PRL/ERI
                    mNewSS->SetCdmaRoamingIndicator(mRoamingIndicator);
                } else {
                    // It means namMatch && mIsInPrl
                    If ((mRoamingIndicator <= 2)) {
                        mNewSS->SetCdmaRoamingIndicator(EriInfo.ROAMING_INDICATOR_OFF);
                    } else {
                        // Use the one from PRL/ERI
                        mNewSS->SetCdmaRoamingIndicator(mRoamingIndicator);
                    }
                }
            }

            Int32 roamingIndicator = mNewSS->GetCdmaRoamingIndicator();
            mNewSS->SetCdmaEriIconIndex(mPhone.mEriManager->GetCdmaEriIconIndex(roamingIndicator,
                    mDefaultRoamingIndicator));
            mNewSS->SetCdmaEriIconMode(mPhone.mEriManager->GetCdmaEriIconMode(roamingIndicator,
                    mDefaultRoamingIndicator));

            // NOTE: Some operator may require overriding mCdmaRoaming
            // (set by the modem), depending on the mRoamingIndicator.

            If (DBG) {
                Log("Set CDMA Roaming Indicator to: " + mNewSS->GetCdmaRoamingIndicator()
                    + ". mCdmaRoaming = " + mCdmaRoaming + ", isPrlLoaded = " + isPrlLoaded
                    + ". namMatch = " + namMatch + " , mIsInPrl = " + mIsInPrl
                    + ", mRoamingIndicator = " + mRoamingIndicator
                    + ", mDefaultRoamingIndicator= " + mDefaultRoamingIndicator);
            }
            PollStateDone();
        }

    }

    protected void SetSignalStrengthDefaultValues() {
        mSignalStrength = new SignalStrength( FALSE);
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

            PollStateDone();
            break;

        case RADIO_OFF:
            mNewSS->SetStateOff();
            mNewCellLoc->SetStateInvalid();
            SetSignalStrengthDefaultValues();
            mGotCountryCode = FALSE;

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
            // Issue all poll-related commands at once, then count
            // down the responses which are allowed to arrive
            // out-of-order.

            mPollingContext[0]++;
            // RIL_REQUEST_OPERATOR is necessary for CDMA
            mCi->GetOperator(
                    ObtainMessage(EVENT_POLL_STATE_OPERATOR_CDMA, mPollingContext));

            mPollingContext[0]++;
            // RIL_REQUEST_VOICE_REGISTRATION_STATE is necessary for CDMA
            mCi->GetVoiceRegistrationState(
                    ObtainMessage(EVENT_POLL_STATE_REGISTRATION_CDMA, mPollingContext));

            mPollingContext[0]++;
            // RIL_REQUEST_DATA_REGISTRATION_STATE
            mCi->GetDataRegistrationState(ObtainMessage(EVENT_POLL_STATE_GPRS,
                                        mPollingContext));
            break;
        }
    }

    protected void FixTimeZone(String isoCountryCode) {
        TimeZone zone = NULL;
        // If the offset Is (0, FALSE) and the time zone property
        // is set, use the time zone property rather than GMT.
        String zoneName = SystemProperties->Get(TIMEZONE_PROPERTY);
        If (DBG) {
            Log("fixTimeZone zoneName='" + zoneName +
                "' mZoneOffset=" + mZoneOffset + " mZoneDst=" + mZoneDst +
                " iso-cc='" + isoCountryCode +
                "' iso-cc-idx=" + Arrays->BinarySearch(GMT_COUNTRY_CODES, isoCountryCode));
        }
        If ((mZoneOffset == 0) && (mZoneDst == FALSE) && (zoneName != NULL)
                && (zoneName->Length() > 0)
                && (Arrays->BinarySearch(GMT_COUNTRY_CODES, isoCountryCode) < 0)) {
            // For NITZ string without time zone,
            // need adjust time to reflect default time zone setting
            zone = TimeZone->GetDefault();
            If (mNeedFixZone) {
                Int64 ctm = System->CurrentTimeMillis();
                Int64 tzOffset = zone->GetOffset(ctm);
                If (DBG) {
                    Log("fixTimeZone: tzOffset=" + tzOffset +
                            " ltod=" + TimeUtils->LogTimeOfDay(ctm));
                }
                If (GetAutoTime()) {
                    Int64 adj = ctm - tzOffset;
                    If (DBG) Log("fixTimeZone: adj ltod=" + TimeUtils->LogTimeOfDay(adj));
                    SetAndBroadcastNetworkSetTime(adj);
                } else {
                    // Adjust the saved NITZ time to account for tzOffset.
                    mSavedTime = mSavedTime - tzOffset;
                    If (DBG) Log("fixTimeZone: adj mSavedTime=" + mSavedTime);
                }
            }
            If (DBG) Log("fixTimeZone: using default TimeZone");
        } else If (isoCountryCode->Equals("")) {
            // Country code not found. This is likely a test network.
            // Get a TimeZone based only on the NITZ Parameters (best guess).
            zone = GetNitzTimeZone(mZoneOffset, mZoneDst, mZoneTime);
            If (DBG) Log("fixTimeZone: using NITZ TimeZone");
        } else {
            zone = TimeUtils->GetTimeZone(mZoneOffset, mZoneDst, mZoneTime, isoCountryCode);
            If (DBG) Log("fixTimeZone: using GetTimeZone(off, dst, time, iso)");
        }

        mNeedFixZone = FALSE;

        If (zone != NULL) {
            Log("fixTimeZone: zone != NULL zone.getID=" + zone->GetID());
            If (GetAutoTimeZone()) {
                SetAndBroadcastNetworkSetTimeZone(zone->GetID());
            } else {
                Log("fixTimeZone: skip changing zone as getAutoTimeZone was FALSE");
            }
            SaveNitzTimeZone(zone->GetID());
        } else {
            Log("fixTimeZone: zone == NULL, do nothing for zone");
        }
    }

    protected void PollStateDone() {
        If (DBG) Log("pollStateDone: cdma oldSS=[" + mSS + "] newSS=[" + mNewSS + "]");

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

        Boolean hasCdmaDataConnectionAttached =
            mSS->GetDataRegState() != ServiceState.STATE_IN_SERVICE
            && mNewSS->GetDataRegState() == ServiceState.STATE_IN_SERVICE;

        Boolean hasCdmaDataConnectionDetached =
            mSS->GetDataRegState() == ServiceState.STATE_IN_SERVICE
            && mNewSS->GetDataRegState() != ServiceState.STATE_IN_SERVICE;

        Boolean hasCdmaDataConnectionChanged =
                       mSS->GetDataRegState() != mNewSS->GetDataRegState();

        Boolean hasRilVoiceRadioTechnologyChanged =
                mSS->GetRilVoiceRadioTechnology() != mNewSS->GetRilVoiceRadioTechnology();

        Boolean hasRilDataRadioTechnologyChanged =
                mSS->GetRilDataRadioTechnology() != mNewSS->GetRilDataRadioTechnology();

        Boolean hasChanged = !mNewSS->Equals(mSS);

        Boolean hasRoamingOn = !mSS->GetRoaming() && mNewSS->GetRoaming();

        Boolean hasRoamingOff = mSS->GetRoaming() && !mNewSS->GetRoaming();

        Boolean hasLocationChanged = !mNewCellLoc->Equals(mCellLoc);

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
        If (mSS->GetVoiceRegState() != mNewSS->GetVoiceRegState() ||
                mSS->GetDataRegState() != mNewSS->GetDataRegState()) {
            EventLog->WriteEvent(EventLogTags.CDMA_SERVICE_STATE_CHANGE,
                    mSS->GetVoiceRegState(), mSS->GetDataRegState(),
                    mNewSS->GetVoiceRegState(), mNewSS->GetDataRegState());
        }

        ServiceState tss;
        tss = mSS;
        mSS = mNewSS;
        mNewSS = tss;
        // clean slate for next time
        mNewSS->SetStateOutOfService();

        CdmaCellLocation tcl = mCellLoc;
        mCellLoc = mNewCellLoc;
        mNewCellLoc = tcl;

        If (hasRilVoiceRadioTechnologyChanged) {
            UpdatePhoneObject();
        }

        If (hasRilDataRadioTechnologyChanged) {
            mPhone->SetSystemProperty(TelephonyProperties.PROPERTY_DATA_NETWORK_TYPE,
                    ServiceState->RilRadioTechnologyToString(mSS->GetRilDataRadioTechnology()));

            If (IsIwlanFeatureAvailable()
                    && (ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN
                        == mSS->GetRilDataRadioTechnology())) {
                Log("pollStateDone: IWLAN enabled");
            }
        }

        If (hasRegistered) {
            mNetworkAttachedRegistrants->NotifyRegistrants();
        }

        If (hasChanged) {
            If ((mCi->GetRadioState()->IsOn()) && (!mIsSubscriptionFromRuim)) {
                String eriText;
                // Now the CDMAPhone sees the new ServiceState so it can get the new ERI text
                If (mSS->GetVoiceRegState() == ServiceState.STATE_IN_SERVICE) {
                    eriText = mPhone->GetCdmaEriText();
                } else {
                    // Note that ServiceState.STATE_OUT_OF_SERVICE is valid used for
                    // mRegistrationState 0,2,3 and 4
                    eriText = mPhone->GetContext()->GetText(
                            R::string::roamingTextSearching).ToString();
                }
                mSS->SetOperatorAlphaLong(eriText);
            }

            String operatorNumeric;

            mPhone->SetSystemProperty(TelephonyProperties.PROPERTY_OPERATOR_ALPHA,
                    mSS->GetOperatorAlphaLong());

            String prevOperatorNumeric =
                    SystemProperties->Get(TelephonyProperties.PROPERTY_OPERATOR_NUMERIC, "");
            operatorNumeric = mSS->GetOperatorNumeric();

            // try to fix the invalid Operator Numeric
            If (IsInvalidOperatorNumeric(operatorNumeric)) {
                Int32 sid = mSS->GetSystemId();
                operatorNumeric = FixUnknownMcc(operatorNumeric, sid);
            }

            mPhone->SetSystemProperty(TelephonyProperties.PROPERTY_OPERATOR_NUMERIC, operatorNumeric);
            UpdateCarrierMccMncConfiguration(operatorNumeric,
                    prevOperatorNumeric, mPhone->GetContext());

            If (IsInvalidOperatorNumeric(operatorNumeric)) {
                If (DBG) Log("operatorNumeric "+ operatorNumeric +"is invalid");
                mPhone->SetSystemProperty(TelephonyProperties.PROPERTY_OPERATOR_ISO_COUNTRY, "");
                mGotCountryCode = FALSE;
            } else {
                String isoCountryCode = "";
                String mcc = operatorNumeric->Substring(0, 3);
                try{
                    isoCountryCode = MccTable->CountryCodeForMcc(Integer->ParseInt(
                            operatorNumeric->Substring(0,3)));
                } Catch ( NumberFormatException ex){
                    Loge("pollStateDone: countryCodeForMcc error" + ex);
                } Catch ( StringIndexOutOfBoundsException ex) {
                    Loge("pollStateDone: countryCodeForMcc error" + ex);
                }

                mPhone->SetSystemProperty(TelephonyProperties.PROPERTY_OPERATOR_ISO_COUNTRY,
                        isoCountryCode);
                mGotCountryCode = TRUE;

                SetOperatorIdd(operatorNumeric);

                If (ShouldFixTimeZoneNow(mPhone, operatorNumeric, prevOperatorNumeric,
                        mNeedFixZone)) {
                    FixTimeZone(isoCountryCode);
                }
            }

            mPhone->SetSystemProperty(TelephonyProperties.PROPERTY_OPERATOR_ISROAMING,
                    mSS->GetRoaming() ? "TRUE" : "FALSE");

            UpdateSpnDisplay();
            mPhone->NotifyServiceStateChanged(mSS);
        }

        // First notify detached, then rat changed, then attached - that's the way it
        // happens in the modem.
        // Behavior of Recipients (DcTracker, for instance) depends on this sequence
        // since DcTracker reloads profiles on "rat_changed" notification and sets up
        // data call on "attached" notification.
        If (hasCdmaDataConnectionDetached) {
            mDetachedRegistrants->NotifyRegistrants();
        }

        If (hasCdmaDataConnectionChanged || hasRilDataRadioTechnologyChanged) {
            NotifyDataRegStateRilRadioTechnologyChanged();
            If (IsIwlanFeatureAvailable()
                    && (ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN
                        == mSS->GetRilDataRadioTechnology())) {
                mPhone->NotifyDataConnection(Phone.REASON_IWLAN_AVAILABLE);
                mIwlanRatAvailable = TRUE;
            } else {
                ProcessIwlanToWwanTransition(mSS);

                mPhone->NotifyDataConnection(NULL);
                mIwlanRatAvailable = FALSE;
            }
        }

        If (hasCdmaDataConnectionAttached) {
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
        // TODO: Add CdmaCellIdenity updating, see CdmaLteServiceStateTracker.
    }

    protected Boolean IsInvalidOperatorNumeric(String operatorNumeric) {
        return operatorNumeric == NULL || operatorNumeric->Length() < 5 ||
                    operatorNumeric->StartsWith(INVALID_MCC);
    }

    protected String FixUnknownMcc(String operatorNumeric, Int32 sid) {
        If (sid <= 0) {
            // no cdma information is available, do nothing
            return operatorNumeric;
        }

        // resolve the mcc from sid;
        // if mSavedTimeZone is NULL, TimeZone would get the default timeZone,
        // and the fixTimeZone couldn't help, because it depends on operator Numeric;
        // if the sid is conflict and timezone is unavailable, the mcc may be not right.
        Boolean isNitzTimeZone = FALSE;
        Int32 timeZone = 0;
        TimeZone tzone = NULL;
        If (mSavedTimeZone != NULL) {
             timeZone =
                     TimeZone->GetTimeZone(mSavedTimeZone).GetRawOffset()/MS_PER_HOUR;
             isNitzTimeZone = TRUE;
        } else {
             tzone = GetNitzTimeZone(mZoneOffset, mZoneDst, mZoneTime);
             If (tzone != NULL)
                     timeZone = tzone->GetRawOffset()/MS_PER_HOUR;
        }

        Int32 mcc = mHbpcdUtils->GetMcc(sid,
                timeZone, (mZoneDst ? 1 : 0), isNitzTimeZone);
        If (mcc > 0) {
            operatorNumeric = Integer->ToString(mcc) + DEFAULT_MNC;
        }
        return operatorNumeric;
    }

    protected void SetOperatorIdd(String operatorNumeric) {
        // Retrieve the current country information
        // with the MCC got from opeatorNumeric.
        String idd = mHbpcdUtils->GetIddByMcc(
                Integer->ParseInt(operatorNumeric->Substring(0,3)));
        If (idd != NULL && !idd->IsEmpty()) {
            mPhone->SetSystemProperty(TelephonyProperties.PROPERTY_OPERATOR_IDP_STRING,
                     idd);
        } else {
            // use default "+", since we don't know the current IDP
            mPhone->SetSystemProperty(TelephonyProperties.PROPERTY_OPERATOR_IDP_STRING, "+");
        }
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
            rawOffset -= MS_PER_HOUR;
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

    /**
     * TODO: This code is exactly the same as in GsmServiceStateTracker
     * and has a TODO to not poll signal strength if screen is off.
     * This code should probably be hoisted to the base class so
     * the fix, when added, works for both.
     */
    private void
    QueueNextSignalStrengthPoll() {
        If (mDontPollSignalStrength) {
            // The radio is telling us about signal strength changes
            // we don't have to ask it
            return;
        }

        Message msg;

        msg = ObtainMessage();
        msg.what = EVENT_POLL_SIGNAL_STRENGTH;

        // TODO Don't poll signal strength if screen is off
        SendMessageDelayed(msg, POLL_PERIOD_MILLIS);
    }

    protected Int32 RadioTechnologyToDataServiceState(Int32 code) {
        Int32 retVal = ServiceState.STATE_OUT_OF_SERVICE;
        Switch(code) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            break;
        case 6: // RADIO_TECHNOLOGY_1xRTT
        case 7: // RADIO_TECHNOLOGY_EVDO_0
        case 8: // RADIO_TECHNOLOGY_EVDO_A
        case 12: // RADIO_TECHNOLOGY_EVDO_B
        case 13: // RADIO_TECHNOLOGY_EHRPD
            retVal = ServiceState.STATE_IN_SERVICE;
            break;
        default:
            Loge("radioTechnologyToDataServiceState: Wrong radioTechnology code.");
        break;
        }
        Return(retVal);
    }

    /** code is registration state 0-5 from TS 27.007 7.2 */
    protected Int32
    RegCodeToServiceState(Int32 code) {
        Switch (code) {
        case 0: // Not searching and not registered
            return ServiceState.STATE_OUT_OF_SERVICE;
        case 1:
            return ServiceState.STATE_IN_SERVICE;
        case 2: // 2 is "searching", fall through
        case 3: // 3 is "registration denied", fall through
        case 4: // 4 is "unknown", not valid in current baseband
            return ServiceState.STATE_OUT_OF_SERVICE;
        case 5:// 5 is "Registered, roaming"
            return ServiceState.STATE_IN_SERVICE;

        default:
            Loge("regCodeToServiceState: unexpected service state " + code);
        return ServiceState.STATE_OUT_OF_SERVICE;
        }
    }

    //@Override
    public Int32 GetCurrentDataConnectionState() {
        return mSS->GetDataRegState();
    }

    /**
     * code is registration state 0-5 from TS 27.007 7.2
     * returns TRUE if registered roam, FALSE otherwise
     */
    protected Boolean
    RegCodeIsRoaming (Int32 code) {
        // 5 is  "in service -- roam"
        return 5 == code;
    }

    /**
     * Determine whether a roaming indicator is in the carrier-specified list of ERIs for
     * home system
     *
     * @param roamInd roaming indicator in String
     * @return TRUE if the roamInd is in the carrier-specified list of ERIs for home network
     */
    private Boolean IsRoamIndForHomeSystem(String roamInd) {
        // retrieve the carrier-specified list of ERIs for home system
        String[] homeRoamIndicators = mPhone->GetContext()->GetResources()
                .GetStringArray(R.array.config_cdma_home_system);

        If (homeRoamIndicators != NULL) {
            // searches through the comma-separated list for a match,
            // return TRUE if one is found.
            For (String homeRoamInd : homeRoamIndicators) {
                If (homeRoamInd->Equals(roamInd)) {
                    return TRUE;
                }
            }
            // no matches found against the list!
            return FALSE;
        }

        // no system property found for the roaming indicators for home system
        return FALSE;
    }

    /**
     * Set roaming state when cdmaRoaming is TRUE and ons is different from spn
     * @param cdmaRoaming TS 27.007 7.2 CREG registered roaming
     * @param s ServiceState hold current ons
     * @return TRUE for roaming state set
     */
    private
    Boolean IsRoamingBetweenOperators(Boolean cdmaRoaming, ServiceState s) {
        String spn = GetSystemProperty(TelephonyProperties.PROPERTY_ICC_OPERATOR_ALPHA, "empty");

        // NOTE: in case of RUIM we should completely ignore the ERI data file and
        // mOperatorAlphaLong is set from RIL_REQUEST_OPERATOR response 0 (alpha ONS)
        String onsl = s->GetOperatorAlphaLong();
        String onss = s->GetOperatorAlphaShort();

        Boolean equalsOnsl = onsl != NULL && spn->Equals(onsl);
        Boolean equalsOnss = onss != NULL && spn->Equals(onss);

        return cdmaRoaming && !(equalsOnsl || equalsOnss);
    }


    /**
     * nitzReceiveTime is time_t that the NITZ time was posted
     */

    private
    void SetTimeFromNITZString (String nitz, Int64 nitzReceiveTime)
    {
        // "yy/mm/dd,hh:mm:Ss(+/-)tz"
        // tz is in number of quarter-hours

        Int64 start = SystemClock->ElapsedRealtime();
        If (DBG) {
            Log("NITZ: " + nitz + "," + nitzReceiveTime +
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

                mNeedFixZone = TRUE;
                mZoneOffset  = tzOffset;
                mZoneDst     = dst != 0;
                mZoneTime    = c->GetTimeInMillis();
            }
            If (DBG) {
                Log("NITZ: tzOffset=" + tzOffset + " dst=" + dst + " zone=" +
                        (zone!=NULL ? zone->GetID() : "NULL") +
                        " iso=" + iso + " mGotCountryCode=" + mGotCountryCode +
                        " mNeedFixZone=" + mNeedFixZone);
            }

            If (zone != NULL) {
                If (GetAutoTimeZone()) {
                    SetAndBroadcastNetworkSetTimeZone(zone->GetID());
                }
                SaveNitzTimeZone(zone->GetID());
            }

            String ignore = SystemProperties->Get("gsm.ignore-nitz");
            If (ignore != NULL && ignore->Equals("yes")) {
                If (DBG) Log("NITZ: Not setting clock because gsm.ignore-nitz is set");
                return;
            }

            try {
                mWakeLock->Acquire();

                /**
                 * Correct the NITZ time by how Int64 its taken to get here.
                 */
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

                If (GetAutoTime()) {
                    /**
                     * Update system time automatically
                     */
                    Int64 gained = c->GetTimeInMillis() - System->CurrentTimeMillis();
                    Int64 timeSinceLastUpdate = SystemClock->ElapsedRealtime() - mSavedAtTime;
                    Int32 nitzUpdateSpacing = Settings::Global::>GetInt(mCr,
                            Settings::Global::NITZ_UPDATE_SPACING, mNitzUpdateSpacing);
                    Int32 nitzUpdateDiff = Settings::Global::>GetInt(mCr,
                            Settings::Global::NITZ_UPDATE_DIFF, mNitzUpdateDiff);

                    If ((mSavedAtTime == 0) || (timeSinceLastUpdate > nitzUpdateSpacing)
                            || (Math->Abs(gained) > nitzUpdateDiff)) {
                        If (DBG) {
                            Log("NITZ: Auto updating time of day to " + c->GetTime()
                                + " NITZ receive delay=" + millisSinceNitzReceived
                                + "ms gained=" + gained + "ms from " + nitz);
                        }

                        SetAndBroadcastNetworkSetTime(c->GetTimeInMillis());
                    } else {
                        If (DBG) {
                            Log("NITZ: ignore, a previous update was "
                                + timeSinceLastUpdate + "ms ago and gained=" + gained + "ms");
                        }
                        return;
                    }
                }

                /**
                 * Update properties and save the time we did the update
                 */
                If (DBG) Log("NITZ: update nitz time property");
                SystemProperties->Set("gsm.nitz.time", String->ValueOf(c->GetTimeInMillis()));
                mSavedTime = c->GetTimeInMillis();
                mSavedAtTime = SystemClock->ElapsedRealtime();
            } finally {
                Int64 end = SystemClock->ElapsedRealtime();
                If (DBG) Log("NITZ: end=" + end + " dur=" + (end - start));
                mWakeLock->ReleaseLock();
            }
        } Catch (RuntimeException ex) {
            Loge("NITZ: Parsing NITZ time " + nitz + " ex=" + ex);
        }
    }

    private Boolean GetAutoTime() {
        try {
            return Settings::Global::>GetInt(mCr, Settings::Global::AUTO_TIME) > 0;
        } Catch (SettingNotFoundException snfe) {
            return TRUE;
        }
    }

    private Boolean GetAutoTimeZone() {
        try {
            return Settings::Global::>GetInt(mCr, Settings::Global::AUTO_TIME_ZONE) > 0;
        } Catch (SettingNotFoundException snfe) {
            return TRUE;
        }
    }

    private void SaveNitzTimeZone(String zoneId) {
        mSavedTimeZone = zoneId;
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
        If (Settings::Global::>GetInt(mCr, Settings::Global::AUTO_TIME, 0) == 0) {
            return;
        }
        If (DBG) {
            Log("revertToNitzTime: mSavedTime=" + mSavedTime + " mSavedAtTime=" + mSavedAtTime);
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
        If (DBG) Log("revertToNitzTimeZone: tz='" + mSavedTimeZone);
        If (mSavedTimeZone != NULL) {
            SetAndBroadcastNetworkSetTimeZone(mSavedTimeZone);
        }
    }

    protected Boolean IsSidsAllZeros() {
        If (mHomeSystemId != NULL) {
            For (Int32 i=0; i < mHomeSystemId.length; i++) {
                If (mHomeSystemId[i] != 0) {
                    return FALSE;
                }
            }
        }
        return TRUE;
    }

    /**
     * Check whether a specified system ID that matches one of the home system IDs.
     */
    private Boolean IsHomeSid(Int32 sid) {
        If (mHomeSystemId != NULL) {
            For (Int32 i=0; i < mHomeSystemId.length; i++) {
                If (sid == mHomeSystemId[i]) {
                    return TRUE;
                }
            }
        }
        return FALSE;
    }

    /**
     * @return TRUE if phone is camping on a technology
     * that could support voice and data simultaneously.
     */
    //@Override
    public Boolean IsConcurrentVoiceAndDataAllowed() {
        // Note: it needs to be confirmed which CDMA network types
        // can support voice and data calls concurrently.
        // For the time-being, the return value will be FALSE.
        return FALSE;
    }

    public String GetMdnNumber() {
        return mMdn;
    }

    public String GetCdmaMin() {
         return mMin;
    }

    /** Returns NULL if NV is not yet ready */
    public String GetPrlVersion() {
        return mPrlVersion;
    }

    /**
     * Returns IMSI as MCC + MNC + MIN
     */
    String GetImsi() {
        // TODO: When RUIM is enabled, IMSI will come from RUIM not build-time props.
        String operatorNumeric = GetSystemProperty(
                TelephonyProperties.PROPERTY_ICC_OPERATOR_NUMERIC, "");

        If (!TextUtils->IsEmpty(operatorNumeric) && GetCdmaMin() != NULL) {
            Return (operatorNumeric + GetCdmaMin());
        } else {
            return NULL;
        }
    }

    /**
     * Check if subscription data has been assigned to mMin
     *
     * return TRUE if MIN info is ready; FALSE otherwise.
     */
    public Boolean IsMinInfoReady() {
        return mIsMinInfoReady;
    }

    /**
     * Returns OTASP_UNKNOWN, OTASP_NEEDED or OTASP_NOT_NEEDED
     */
    Int32 GetOtasp() {
        Int32 provisioningState;
        // for ruim, min is NULL means require otasp.
        If (mIsSubscriptionFromRuim && mMin == NULL) {
            return OTASP_NEEDED;
        }
        If (mMin == NULL || (mMin->Length() < 6)) {
            If (DBG) Log("getOtasp: bad mMin='" + mMin + "'");
            provisioningState = OTASP_UNKNOWN;
        } else {
            If ((mMin->Equals(UNACTIVATED_MIN_VALUE)
                    || mMin->Substring(0,6).Equals(UNACTIVATED_MIN2_VALUE))
                    || SystemProperties->GetBoolean("test_cdma_setup", FALSE)) {
                provisioningState = OTASP_NEEDED;
            } else {
                provisioningState = OTASP_NOT_NEEDED;
            }
        }
        If (DBG) Log("getOtasp: state=" + provisioningState);
        return provisioningState;
    }

    //@Override
    protected void HangupAndPowerOff() {
        // hang up all active voice calls
        mPhone.mCT.mRingingCall->HangupIfAlive();
        mPhone.mCT.mBackgroundCall->HangupIfAlive();
        mPhone.mCT.mForegroundCall->HangupIfAlive();
        mCi->SetRadioPower(FALSE, NULL);
    }

    protected void ParseSidNid (String sidStr, String nidStr) {
        If (sidStr != NULL) {
            String[] sid = sidStr->Split(",");
            mHomeSystemId = new Int32[sid.length];
            For (Int32 i = 0; i < sid.length; i++) {
                try {
                    mHomeSystemId[i] = Integer->ParseInt(sid[i]);
                } Catch (NumberFormatException ex) {
                    Loge("error parsing system id: " + ex);
                }
            }
        }
        If (DBG) Log("CDMA_SUBSCRIPTION: SID=" + sidStr);

        If (nidStr != NULL) {
            String[] nid = nidStr->Split(",");
            mHomeNetworkId = new Int32[nid.length];
            For (Int32 i = 0; i < nid.length; i++) {
                try {
                    mHomeNetworkId[i] = Integer->ParseInt(nid[i]);
                } Catch (NumberFormatException ex) {
                    Loge("CDMA_SUBSCRIPTION: error parsing network id: " + ex);
                }
            }
        }
        If (DBG) Log("CDMA_SUBSCRIPTION: NID=" + nidStr);
    }

    protected void UpdateOtaspState() {
        Int32 otaspMode = GetOtasp();
        Int32 oldOtaspMode = mCurrentOtaspMode;
        mCurrentOtaspMode = otaspMode;

        // Notify apps subscription info is ready
        If (mCdmaForSubscriptionInfoReadyRegistrants != NULL) {
            If (DBG) Log("CDMA_SUBSCRIPTION: call NotifyRegistrants()");
            mCdmaForSubscriptionInfoReadyRegistrants->NotifyRegistrants();
        }
        If (oldOtaspMode != mCurrentOtaspMode) {
            If (DBG) {
                Log("CDMA_SUBSCRIPTION: call notifyOtaspChanged old otaspMode=" +
                    oldOtaspMode + " new otaspMode=" + mCurrentOtaspMode);
            }
            mPhone->NotifyOtaspChanged(mCurrentOtaspMode);
        }
    }

    private void RegisterForRuimEvents() {
        Log("registerForRuimEvents");
        If (mUiccApplcation != NULL) {
            mUiccApplcation->RegisterForReady(this, EVENT_RUIM_READY, NULL);
        }
        If (mIccRecords != NULL) {
            mIccRecords->RegisterForRecordsLoaded(this, EVENT_RUIM_RECORDS_LOADED, NULL);
        }
    }

    private void UnregisterForRuimEvents() {
        Log("unregisterForRuimEvents");
        If (mUiccApplcation != NULL) {
            mUiccApplcation->UnregisterForReady(this);
        }
        If (mIccRecords != NULL) {
            mIccRecords->UnregisterForRecordsLoaded(this);
        }
    }

    protected UiccCardApplication GetUiccCardApplication() {
            return  mUiccController->GetUiccCardApplication(mPhone->GetPhoneId(),
                    UiccController.APP_FAM_3GPP2);
    }

    //@Override
    protected void OnUpdateIccAvailability() {
        If (mUiccController == NULL ) {
            return;
        }

        UiccCardApplication newUiccApplication = GetUiccCardApplication();

        If (mUiccApplcation != newUiccApplication) {
            Log("Removing stale icc objects.");
            UnregisterForRuimEvents();
            mIccRecords = NULL;
            mUiccApplcation = NULL;
            If (newUiccApplication != NULL) {
                Log("New card found");
                mUiccApplcation = newUiccApplication;
                mIccRecords = mUiccApplcation->GetIccRecords();
                If (mIsSubscriptionFromRuim) {
                    RegisterForRuimEvents();
                }
            }
        }
    }

    //@Override
    protected void Log(String s) {
        Rlog->D(LOG_TAG, "[CdmaSST] " + s);
    }

    //@Override
    protected void Loge(String s) {
        Rlog->E(LOG_TAG, "[CdmaSST] " + s);
    }

    //@Override
    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("CdmaServiceStateTracker extends:");
        super->Dump(fd, pw, args);
        pw->Println(" mPhone=" + mPhone);
        pw->Println(" mSS=" + mSS);
        pw->Println(" mNewSS=" + mNewSS);
        pw->Println(" mCellLoc=" + mCellLoc);
        pw->Println(" mNewCellLoc=" + mNewCellLoc);
        pw->Println(" mCurrentOtaspMode=" + mCurrentOtaspMode);
        pw->Println(" mCdmaRoaming=" + mCdmaRoaming);
        pw->Println(" mRoamingIndicator=" + mRoamingIndicator);
        pw->Println(" mIsInPrl=" + mIsInPrl);
        pw->Println(" mDefaultRoamingIndicator=" + mDefaultRoamingIndicator);
        pw->Println(" mRegistrationState=" + mRegistrationState);
        pw->Println(" mNeedFixZone=" + mNeedFixZone);
        pw->Println(" mZoneOffset=" + mZoneOffset);
        pw->Println(" mZoneDst=" + mZoneDst);
        pw->Println(" mZoneTime=" + mZoneTime);
        pw->Println(" mGotCountryCode=" + mGotCountryCode);
        pw->Println(" mSavedTimeZone=" + mSavedTimeZone);
        pw->Println(" mSavedTime=" + mSavedTime);
        pw->Println(" mSavedAtTime=" + mSavedAtTime);
        pw->Println(" mWakeLock=" + mWakeLock);
        pw->Println(" mCurPlmn=" + mCurPlmn);
        pw->Println(" mCurShowPmn=" + mCurShowPlmn);
        pw->Println(" mCurSpn=" + mCurSpn);
        pw->Println(" mCurShowSpn=" + mCurShowSpn);
        pw->Println(" mMdn=" + mMdn);
        pw->Println(" mHomeSystemId=" + mHomeSystemId);
        pw->Println(" mHomeNetworkId=" + mHomeNetworkId);
        pw->Println(" mMin=" + mMin);
        pw->Println(" mPrlVersion=" + mPrlVersion);
        pw->Println(" mIsMinInfoReady=" + mIsMinInfoReady);
        pw->Println(" mIsEriTextLoaded=" + mIsEriTextLoaded);
        pw->Println(" mIsSubscriptionFromRuim=" + mIsSubscriptionFromRuim);
        pw->Println(" mCdmaSSM=" + mCdmaSSM);
        pw->Println(" mRegistrationDeniedReason=" + mRegistrationDeniedReason);
        pw->Println(" mCurrentCarrier=" + mCurrentCarrier);
    }

    //@Override
    CARAPI SetImsRegistrationState(Boolean registered) {
        Log("ImsRegistrationState - registered : " + registered);

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
