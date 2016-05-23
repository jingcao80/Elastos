/*
 * Copyright (C) 2011 The Android Open Source Project
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

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Preference::IPreferenceManager;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Provider::ITelephony;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::IServiceState;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;

using Elastos::Droid::Telephony::ITelephonyManager;

using Elastos::Droid::Internal::Telephony::Dataconnection::IDcTracker;
using Elastos::Droid::Internal::Telephony::IMccTable;
using Elastos::Droid::Internal::Telephony::IOperatorInfo;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IPhoneNotifier;
using Elastos::Droid::Internal::Telephony::IPhoneProxy;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::IPhoneSubInfo;
using Elastos::Droid::Internal::Telephony::ISMSDispatcher;
using Elastos::Droid::Internal::Telephony::ISmsBroadcastUndelivered;
using Elastos::Droid::Internal::Telephony::ISubscription;
using Elastos::Droid::Internal::Telephony::ISubscriptionController;
using Elastos::Droid::Internal::Telephony::Gsm::IGsmSMSDispatcher;
using Elastos::Droid::Internal::Telephony::Gsm::ISmsMessage;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IIsimRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IIsimUiccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IRuimRecords;
using Elastos::Droid::Internal::Telephony::Uicc::ISIMRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::IServiceStateTracker;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_ICC_OPERATOR_ALPHA;
using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_ICC_OPERATOR_ISO_COUNTRY;
using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_ICC_OPERATOR_NUMERIC;
using static com::Android::Internal::Telephony::PhoneConstants::IEVENT_SUBSCRIPTION_ACTIVATED;
using static com::Android::Internal::Telephony::PhoneConstants::IEVENT_SUBSCRIPTION_DEACTIVATED;

public class CDMALTEPhone extends CDMAPhone {
    static const String LOG_LTE_TAG = "CDMALTEPhone";
    private static const Boolean DBG = TRUE;

    /** CdmaLtePhone in addition to RuimRecords available from
     * PhoneBase needs access to SIMRecords and IsimUiccRecords
     */
    private SIMRecords mSimRecords;
    private IsimUiccRecords mIsimUiccRecords;

    // Constructors
    public CDMALTEPhone(Context context, CommandsInterface ci, PhoneNotifier notifier,
            Int32 phoneId) {
        This(context, ci, notifier, FALSE, phoneId);
    }

    public CDMALTEPhone(Context context, CommandsInterface ci, PhoneNotifier notifier,
            Boolean unitTestMode, Int32 phoneId) {
        Super(context, ci, notifier, phoneId);

        Rlog->D(LOG_TAG, "CDMALTEPhone: constructor: sub = " + mPhoneId);

        mDcTracker = new DcTracker(this);

    }

    // Constructors
    public CDMALTEPhone(Context context, CommandsInterface ci, PhoneNotifier notifier) {
        Super(context, ci, notifier, FALSE);
    }

    //@Override
    protected void InitSstIcc() {
        mSST = new CdmaLteServiceStateTracker(this);
    }

    //@Override
    CARAPI Dispose() {
        {    AutoLock syncLock(PhoneProxy.lockForRadioTechnologyChange);
            If (mSimRecords != NULL) {
                mSimRecords->UnregisterForRecordsLoaded(this);
            }
            super->Dispose();
        }
    }

    //@Override
    CARAPI RemoveReferences() {
        super->RemoveReferences();
    }

    //@Override
    CARAPI HandleMessage(Message msg) {
        AsyncResult ar;
        Message onComplete;

        // messages to be handled whether or not the phone is being destroyed
        // should only include messages which are being re-directed and do not use
        // resources of the phone being destroyed
        Switch (msg.what) {
            // handle the select network completion callbacks.
            case EVENT_SET_NETWORK_MANUAL_COMPLETE:
            case EVENT_SET_NETWORK_AUTOMATIC_COMPLETE:
                super->HandleMessage(msg);
                return;
        }

        If (!mIsTheCurrentActivePhone) {
            Rlog->E(LOG_TAG, "Received message " + msg +
                    "[" + msg.what + "] while being destroyed. Ignoring.");
            return;
        }
        Switch(msg.what) {
            case EVENT_SIM_RECORDS_LOADED:
                mSimRecordsLoadedRegistrants->NotifyRegistrants();
                break;

            case EVENT_SUBSCRIPTION_ACTIVATED:
                Log("EVENT_SUBSCRIPTION_ACTIVATED");
                OnSubscriptionActivated();
                break;

            case EVENT_SUBSCRIPTION_DEACTIVATED:
                Log("EVENT_SUBSCRIPTION_DEACTIVATED");
                OnSubscriptionDeactivated();
                break;

            default:
                super->HandleMessage(msg);
        }
    }
    //@Override
    public PhoneConstants.DataState GetDataConnectionState(String apnType) {
        PhoneConstants.DataState ret = PhoneConstants.DataState.DISCONNECTED;

        If (mSST == NULL) {
            // Radio Technology Change is ongoing, Dispose() and
            // RemoveReferences() have already been called

            ret = PhoneConstants.DataState.DISCONNECTED;
        } else If (mSST->GetCurrentDataConnectionState() != ServiceState.STATE_IN_SERVICE &&
                            mOosIsDisconnect) {
            ret = PhoneConstants.DataState.DISCONNECTED;
            Log("getDataConnectionState: Data is Out of Service. ret = " + ret);
        } else If (mDcTracker->IsApnTypeEnabled(apnType) == FALSE) {
            ret = PhoneConstants.DataState.DISCONNECTED;
        } else {
            Switch (mDcTracker->GetState(apnType)) {
                case RETRYING:
                case FAILED:
                case IDLE:
                    ret = PhoneConstants.DataState.DISCONNECTED;
                    break;

                case CONNECTED:
                case DISCONNECTING:
                    If (mCT.mState != PhoneConstants.State.IDLE &&
                            !mSST->IsConcurrentVoiceAndDataAllowed()) {
                        ret = PhoneConstants.DataState.SUSPENDED;
                    } else {
                        ret = PhoneConstants.DataState.CONNECTED;
                    }
                    break;

                case CONNECTING:
                case SCANNING:
                    ret = PhoneConstants.DataState.CONNECTING;
                    break;
            }
        }

        Log("getDataConnectionState apnType=" + apnType + " ret=" + ret);
        return ret;
    }

    /**
     * Sets the "current" field in the telephony provider according to the
     * build-time operator numeric property
     *
     * @return TRUE for success; FALSE otherwise.
     */
    //@Override
    Boolean UpdateCurrentCarrierInProvider(String operatorNumeric) {
        Boolean retVal;
        If (mUiccController->GetUiccCardApplication(mPhoneId, UiccController.APP_FAM_3GPP) == NULL) {
            If (DBG) Log("updateCurrentCarrierInProvider APP_FAM_3GPP == NULL");
            retVal = super->UpdateCurrentCarrierInProvider(operatorNumeric);
        } else {
            If (DBG) Log("updateCurrentCarrierInProvider not updated");
            retVal = TRUE;
        }
        If (DBG) Log("updateCurrentCarrierInProvider X retVal=" + retVal);
        return retVal;
    }

    //@Override
    public Boolean UpdateCurrentCarrierInProvider() {
        Int64 currentDds = SubscriptionManager->GetDefaultDataSubId();
        String operatorNumeric = GetOperatorNumeric();

        Rlog->D(LOG_TAG, "updateCurrentCarrierInProvider: mSubscription = " + GetSubId()
                + " currentDds = " + currentDds + " operatorNumeric = " + operatorNumeric);

        If (!TextUtils->IsEmpty(operatorNumeric) && (GetSubId() == currentDds)) {
            try {
                Uri uri = Uri->WithAppendedPath(Telephony.Carriers.CONTENT_URI, "current");
                ContentValues map = new ContentValues();
                map->Put(Telephony.Carriers.NUMERIC, operatorNumeric);
                mContext->GetContentResolver()->Insert(uri, map);
                return TRUE;
            } Catch (SQLException e) {
                Rlog->E(LOG_TAG, "Can't store current operator", e);
            }
        }
        return FALSE;
    }

    // return IMSI from CSIM as subscriber ID if available, otherwise reads from USIM
    //@Override
    public String GetSubscriberId() {
        IccRecords r = (mIccRecords != NULL) ? mIccRecords->Get() : NULL;
        If (r != NULL) {
            String imsi = r->GetIMSI();
            If (!TextUtils->IsEmpty(imsi)) {
                Log("IMSI = " + imsi);
                return imsi;
            }
        }

        Log("IMSI undefined");
        return "";
    }


    // fix CTS test expecting IMEI to be used as device ID when in LteOnCdma mode
    //@Override
    public String GetDeviceId() {
        If (TelephonyManager->GetLteOnCdmaModeStatic() == PhoneConstants.LTE_ON_CDMA_TRUE) {
            return mImei;
        } else {
            return super->GetDeviceId();
        }
    }

    // return GID1 from USIM
    //@Override
    public String GetGroupIdLevel1() {
        Return (mSimRecords != NULL) ? mSimRecords->GetGid1() : "";
    }

    //@Override
    public String GetImei() {
        return mImei;
    }

    //@Override
    public String GetDeviceSvn() {
        return mImeiSv;
    }

    //@Override
    public IsimRecords GetIsimRecords() {
        return mIsimUiccRecords;
    }

    //@Override
    public String GetMsisdn() {
        Return (mSimRecords != NULL) ? mSimRecords->GetMsisdnNumber() : NULL;
    }

    //@Override
    CARAPI GetAvailableNetworks(Message response) {
        mCi->GetAvailableNetworks(response);
    }

    //@Override
    protected void OnUpdateIccAvailability() {
        If (mSimRecords != NULL) {
            mSimRecords->UnregisterForRecordsLoaded(this);
        }

        If (mUiccController == NULL ) {
            return;
        }

        // Update IsimRecords
        UiccCardApplication newUiccApplication =
                mUiccController->GetUiccCardApplication(mPhoneId, UiccController.APP_FAM_IMS);
        IsimUiccRecords newIsimUiccRecords = NULL;

        If (newUiccApplication != NULL) {
            newIsimUiccRecords = (IsimUiccRecords) newUiccApplication->GetIccRecords();
        }
        mIsimUiccRecords = newIsimUiccRecords;

        // Update UsimRecords
        newUiccApplication = mUiccController->GetUiccCardApplication(mPhoneId,
                UiccController.APP_FAM_3GPP);
        SIMRecords newSimRecords = NULL;
        If (newUiccApplication != NULL) {
            newSimRecords = (SIMRecords) newUiccApplication->GetIccRecords();
        }
        mSimRecords = newSimRecords;
        If (mSimRecords != NULL) {
            mSimRecords->RegisterForRecordsLoaded(this, EVENT_SIM_RECORDS_LOADED, NULL);
        }

        super->OnUpdateIccAvailability();
    }

    //@Override
    protected void Init(Context context, PhoneNotifier notifier) {
        mCi->SetPhoneType(PhoneConstants.PHONE_TYPE_CDMA);
        mCT = new CdmaCallTracker(this);
        mCdmaSSM = CdmaSubscriptionSourceManager->GetInstance(context, mCi, this,
                EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED, NULL);
        mRuimPhoneBookInterfaceManager = new RuimPhoneBookInterfaceManager(this);
        mSubInfo = new PhoneSubInfo(this);
        mEriManager = new EriManager(this, context, EriManager.ERI_FROM_XML);

        mCi->RegisterForAvailable(this, EVENT_RADIO_AVAILABLE, NULL);
        mCi->RegisterForOffOrNotAvailable(this, EVENT_RADIO_OFF_OR_NOT_AVAILABLE, NULL);
        mCi->RegisterForOn(this, EVENT_RADIO_ON, NULL);
        mCi->SetOnSuppServiceNotification(this, EVENT_SSN, NULL);
        mSST->RegisterForNetworkAttached(this, EVENT_REGISTERED_TO_NETWORK, NULL);
        mCi->SetEmergencyCallbackMode(this, EVENT_EMERGENCY_CALLBACK_MODE_ENTER, NULL);
        mCi->RegisterForExitEmergencyCallbackMode(this, EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE,
                NULL);

        PowerManager pm
            = (PowerManager) context->GetSystemService(Context.POWER_SERVICE);
        mWakeLock = pm->NewWakeLock(PowerManager.PARTIAL_WAKE_LOCK,LOG_TAG);

        // This is needed to handle phone process crashes
        String inEcm = SystemProperties->Get(TelephonyProperties.PROPERTY_INECM_MODE, "FALSE");
        mIsPhoneInEcmState = inEcm->Equals("TRUE");
        If (mIsPhoneInEcmState) {
            // Send a message which will invoke handleExitEmergencyCallbackMode
            mCi->ExitEmergencyCallbackMode(ObtainMessage(EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE));
        }

        // get the string that specifies the carrier OTA Sp number
        mCarrierOtaSpNumSchema = SystemProperties->Get(
                TelephonyProperties.PROPERTY_OTASP_NUM_SCHEMA,"");

        SetProperties();
    }

    private void OnSubscriptionActivated() {
//        mSubscriptionData = SubscriptionManager->GetCurrentSubscription(mSubscription);

        Log("SUBSCRIPTION ACTIVATED : slotId : " + mSubscriptionData.slotId
                + " appid : " + mSubscriptionData.m3gpp2Index
                + " subId : " + mSubscriptionData.subId
                + " subStatus : " + mSubscriptionData.subStatus);

        // Make sure properties are set for proper subscription.
        SetProperties();

        OnUpdateIccAvailability();
        mSST->SendMessage(mSST->ObtainMessage(ServiceStateTracker.EVENT_ICC_CHANGED));
        ((CdmaLteServiceStateTracker)mSST).UpdateCdmaSubscription();
        ((DcTracker)mDcTracker).UpdateRecords();
    }

    private void OnSubscriptionDeactivated() {
        Log("SUBSCRIPTION DEACTIVATED");
        // resetSubSpecifics
        mSubscriptionData = NULL;
    }

    // Set the properties per subscription
    private void SetProperties() {
        //Change the system property
        SetSystemProperty(TelephonyProperties.CURRENT_ACTIVE_PHONE,
                new Integer(PhoneConstants.PHONE_TYPE_CDMA).ToString());
        // Sets operator alpha property by retrieving from build-time system property
        String operatorAlpha = SystemProperties->Get("ro.cdma.home.operator.alpha");
        If (!TextUtils->IsEmpty(operatorAlpha)) {
            SetSystemProperty(PROPERTY_ICC_OPERATOR_ALPHA, operatorAlpha);
        }

        // Sets operator numeric property by retrieving from build-time system property
        String operatorNumeric = SystemProperties->Get(PROPERTY_CDMA_HOME_OPERATOR_NUMERIC);
        Log("update icc_operator_numeric=" + operatorNumeric);
        If (!TextUtils->IsEmpty(operatorNumeric)) {
            SetSystemProperty(PROPERTY_ICC_OPERATOR_NUMERIC, operatorNumeric);

            SubscriptionController->GetInstance()->SetMccMnc(operatorNumeric, GetSubId());
            // Sets iso country property by retrieving from build-time system property
            SetIsoCountryProperty(operatorNumeric);
            // Updates MCC MNC device configuration information
            Log("update mccmnc=" + operatorNumeric);
            MccTable->UpdateMccMncConfiguration(mContext, operatorNumeric, FALSE);
        }
        // Sets current entry in the telephony carrier table
        UpdateCurrentCarrierInProvider();
    }

    //@Override
    CARAPI SetSystemProperty(String property, String value) {
        If(GetUnitTestMode()) {
            return;
        }
        TelephonyManager->SetTelephonyProperty(property, GetSubId(), value);
    }

    public String GetSystemProperty(String property, String defValue) {
        If(GetUnitTestMode()) {
            return NULL;
        }
        return TelephonyManager->GetTelephonyProperty(property, GetSubId(), defValue);
    }

    CARAPI UpdateDataConnectionTracker() {
        ((DcTracker)mDcTracker).Update();
    }

    CARAPI SetInternalDataEnabled(Boolean enable, Message onCompleteMsg) {
        ((DcTracker)mDcTracker)
                .SetInternalDataEnabled(enable, onCompleteMsg);
    }

    public Boolean SetInternalDataEnabledFlag(Boolean enable) {
       Return ((DcTracker)mDcTracker)
                .SetInternalDataEnabledFlag(enable);
    }

    /**
     * @return operator numeric.
     */
    public String GetOperatorNumeric() {
        String operatorNumeric = NULL;
        IccRecords curIccRecords = NULL;
        If (mCdmaSubscriptionSource == CDMA_SUBSCRIPTION_NV) {
            operatorNumeric = SystemProperties->Get("ro.cdma.home.operator.numeric");
        } else If (mCdmaSubscriptionSource == CDMA_SUBSCRIPTION_RUIM_SIM) {
            curIccRecords = mSimRecords;
            If (curIccRecords != NULL) {
                operatorNumeric = curIccRecords->GetOperatorNumeric();
            } else {
                curIccRecords = mIccRecords->Get();
                If (curIccRecords != NULL && (curIccRecords instanceof RuimRecords)) {
                    RuimRecords csim = (RuimRecords) curIccRecords;
                    operatorNumeric = csim->GetOperatorNumeric();
                }
            }
        }
        If (operatorNumeric == NULL) {
            Rlog->E(LOG_TAG, "getOperatorNumeric: Cannot retrieve operatorNumeric:"
                    + " mCdmaSubscriptionSource = " + mCdmaSubscriptionSource + " mIccRecords = "
                    + ((curIccRecords != NULL) ? curIccRecords->GetRecordsLoaded() : NULL));
        }

        Rlog->D(LOG_TAG, "getOperatorNumeric: mCdmaSubscriptionSource = " + mCdmaSubscriptionSource
                + " operatorNumeric = " + operatorNumeric);

        return operatorNumeric;
    }
    CARAPI RegisterForAllDataDisconnected(Handler h, Int32 what, Object obj) {
        ((DcTracker)mDcTracker)
               .RegisterForAllDataDisconnected(h, what, obj);
    }

    CARAPI UnregisterForAllDataDisconnected(Handler h) {
        ((DcTracker)mDcTracker)
                .UnregisterForAllDataDisconnected(h);
    }

    //@Override
    CARAPI RegisterForSimRecordsLoaded(Handler h, Int32 what, Object obj) {
        mSimRecordsLoadedRegistrants->AddUnique(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForSimRecordsLoaded(Handler h) {
        mSimRecordsLoadedRegistrants->Remove(h);
    }


    //@Override
    protected void Log(String s) {
            Rlog->D(LOG_LTE_TAG, s);
    }

    protected void Loge(String s) {
            Rlog->E(LOG_LTE_TAG, s);
    }

    protected void Loge(String s, Throwable e) {
        Rlog->E(LOG_LTE_TAG, s, e);
    }

    //@Override
    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("CDMALTEPhone extends:");
        super->Dump(fd, pw, args);
    }
}
