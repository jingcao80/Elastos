/*
 * Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 *
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

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Preference::IPreferenceManager;
using Elastos::Droid::Provider::ITelephony;
using Elastos::Droid::Telecom::IVideoProfile;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;

using Elastos::Droid::Ims::IImsManager;
using Elastos::Droid::Internal::Telephony::ICallTracker;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Telephony::IRlog;

using static com::Android::Internal::Telephony::CommandsInterface::ICF_ACTION_DISABLE;
using static com::Android::Internal::Telephony::CommandsInterface::ICF_ACTION_ENABLE;
using static com::Android::Internal::Telephony::CommandsInterface::ICF_ACTION_ERASURE;
using static com::Android::Internal::Telephony::CommandsInterface::ICF_ACTION_REGISTRATION;
using static com::Android::Internal::Telephony::CommandsInterface::ICF_REASON_ALL;
using static com::Android::Internal::Telephony::CommandsInterface::ICF_REASON_ALL_CONDITIONAL;
using static com::Android::Internal::Telephony::CommandsInterface::ICF_REASON_NO_REPLY;
using static com::Android::Internal::Telephony::CommandsInterface::ICF_REASON_NOT_REACHABLE;
using static com::Android::Internal::Telephony::CommandsInterface::ICF_REASON_BUSY;
using static com::Android::Internal::Telephony::CommandsInterface::ICF_REASON_UNCONDITIONAL;
using static com::Android::Internal::Telephony::CommandsInterface::ISERVICE_CLASS_VOICE;
using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_BASEBAND_VERSION;

using Elastos::Droid::Internal::Telephony::Dataconnection::IDcTracker;
using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::ICallForwardInfo;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::IIccPhoneBookInterfaceManager;
using Elastos::Droid::Internal::Telephony::IMmiCode;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::IPhoneNotifier;
using Elastos::Droid::Internal::Telephony::IPhoneProxy;
using Elastos::Droid::Internal::Telephony::IPhoneSubInfo;
using Elastos::Droid::Internal::Telephony::ISubscriptionController;

using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Internal::Telephony::IUUSInfo;
using Elastos::Droid::Internal::Telephony::Imsphone::IImsPhone;
using Elastos::Droid::Internal::Telephony::Test::ISimulatedRadioControl;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::IServiceStateTracker;
using Elastos::Droid::Internal::Telephony::Uicc::IIsimRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IIsimUiccRecords;


using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

using static com::Android::Internal::Telephony::PhoneConstants::IEVENT_SUBSCRIPTION_ACTIVATED;
using static com::Android::Internal::Telephony::PhoneConstants::IEVENT_SUBSCRIPTION_DEACTIVATED;

/**
 * {@hide}
 */
public class GSMPhone extends PhoneBase {
    // NOTE that LOG_TAG here is "GSM", which means that log messages
    // from this file will go into the radio log rather than the main
    // log.  (Use "adb logcat -b radio" to see them.)
    static const String LOG_TAG = "GSMPhone";
    private static const Boolean LOCAL_DEBUG = TRUE;
    private static const Boolean VDBG = FALSE; /* STOPSHIP if TRUE */

    // Key used to read/write current ciphering state
    public static const String CIPHERING_KEY = "ciphering_key";
    // Key used to read/write voice mail number
    public static const String VM_NUMBER = "vm_number_key";
    // Key used to read/write the SIM IMSI used for storing the imsi
    public static const String SIM_IMSI = "sim_imsi_key";
    // Key used to read/write if Call Forwarding is enabled
    public static const String CF_ENABLED = "cf_enabled_key";

    // Event constant for checking if Call Forwarding is enabled
    private static const Int32 CHECK_CALLFORWARDING_STATUS = 75;

    // Instance Variables
    GsmCallTracker mCT;
    GsmServiceStateTracker mSST;
    ArrayList <GsmMmiCode> mPendingMMIs = new ArrayList<GsmMmiCode>();
    protected SimPhoneBookInterfaceManager mSimPhoneBookIntManager;
    PhoneSubInfo mSubInfo;

    Registrant mPostDialHandler;

    /** List of Registrants to receive Supplementary Service Notifications. */
    RegistrantList mSsnRegistrants = new RegistrantList();

    // mEcmTimerResetRegistrants are informed after Ecm timer is canceled or re-started
    private final RegistrantList mEcmTimerResetRegistrants = new RegistrantList();

    private String mImei;
    private String mImeiSv;
    private String mVmNumber;

    private IsimUiccRecords mIsimUiccRecords;

    // Create Cfu (Call forward unconditional) so that dialling number &
    // MOnComplete (Message object passed by client) can be packed &
    // given as a single Cfu object as user data to RIL.
    private static class Cfu {
        final String mSetCfNumber;
        final Message mOnComplete;

        Cfu(String cfNumber, Message onComplete) {
            mSetCfNumber = cfNumber;
            mOnComplete = onComplete;
        }
    }

    // Constructors

    public
    GSMPhone (Context context, CommandsInterface ci, PhoneNotifier notifier) {
        This(context,ci,notifier, FALSE);
    }

    public
    GSMPhone (Context context, CommandsInterface ci, PhoneNotifier notifier, Boolean unitTestMode) {
        Super("GSM", notifier, context, ci, unitTestMode);

        If (ci instanceof SimulatedRadioControl) {
            mSimulatedRadioControl = (SimulatedRadioControl) ci;
        }

        mCi->SetPhoneType(PhoneConstants.PHONE_TYPE_GSM);
        mCT = new GsmCallTracker(this);

        mSST = new GsmServiceStateTracker(this);
        mDcTracker = new DcTracker(this);

        If (!unitTestMode) {
            mSimPhoneBookIntManager = new SimPhoneBookInterfaceManager(this);
            mSubInfo = new PhoneSubInfo(this);
        }

        mCi->RegisterForAvailable(this, EVENT_RADIO_AVAILABLE, NULL);
        mCi->RegisterForOffOrNotAvailable(this, EVENT_RADIO_OFF_OR_NOT_AVAILABLE, NULL);
        mCi->RegisterForOn(this, EVENT_RADIO_ON, NULL);
        mCi->SetOnUSSD(this, EVENT_USSD, NULL);
        mCi->SetOnSuppServiceNotification(this, EVENT_SSN, NULL);
        mSST->RegisterForNetworkAttached(this, EVENT_REGISTERED_TO_NETWORK, NULL);
        mCi->SetOnSs(this, EVENT_SS, NULL);
        SetProperties();
    }

    public
    GSMPhone (Context context, CommandsInterface ci, PhoneNotifier notifier, Int32 phoneId) {
        This(context, ci, notifier, FALSE, phoneId);
    }

    public
    GSMPhone (Context context, CommandsInterface ci,
            PhoneNotifier notifier, Boolean unitTestMode, Int32 phoneId) {
        Super("GSM", notifier, context, ci, unitTestMode, phoneId);

        If (ci instanceof SimulatedRadioControl) {
            mSimulatedRadioControl = (SimulatedRadioControl) ci;
        }

        mCi->SetPhoneType(PhoneConstants.PHONE_TYPE_GSM);
        mCT = new GsmCallTracker(this);

        mSST = new GsmServiceStateTracker(this);
        mDcTracker = new DcTracker(this);

        If (!unitTestMode) {
            mSimPhoneBookIntManager = new SimPhoneBookInterfaceManager(this);
            mSubInfo = new PhoneSubInfo(this);
        }

        mCi->RegisterForAvailable(this, EVENT_RADIO_AVAILABLE, NULL);
        mCi->RegisterForOffOrNotAvailable(this, EVENT_RADIO_OFF_OR_NOT_AVAILABLE, NULL);
        mCi->RegisterForOn(this, EVENT_RADIO_ON, NULL);
        mCi->SetOnUSSD(this, EVENT_USSD, NULL);
        mCi->SetOnSuppServiceNotification(this, EVENT_SSN, NULL);
        mSST->RegisterForNetworkAttached(this, EVENT_REGISTERED_TO_NETWORK, NULL);
        mCi->SetOnSs(this, EVENT_SS, NULL);
        SetProperties();

        Log("GSMPhone: constructor: sub = " + mPhoneId);

        SetProperties();
    }

    protected void SetProperties() {
        TelephonyManager->SetTelephonyProperty(TelephonyProperties.CURRENT_ACTIVE_PHONE,
                GetSubId(), new Integer(PhoneConstants.PHONE_TYPE_GSM).ToString());
    }

    //@Override
    CARAPI Dispose() {
        {    AutoLock syncLock(PhoneProxy.lockForRadioTechnologyChange);
            super->Dispose();

            //Unregister from all former registered events
            mCi->UnregisterForAvailable(this); //EVENT_RADIO_AVAILABLE
            UnregisterForSimRecordEvents();
            mCi->UnregisterForOffOrNotAvailable(this); //EVENT_RADIO_OFF_OR_NOT_AVAILABLE
            mCi->UnregisterForOn(this); //EVENT_RADIO_ON
            mSST->UnregisterForNetworkAttached(this); //EVENT_REGISTERED_TO_NETWORK
            mCi->UnSetOnUSSD(this);
            mCi->UnSetOnSuppServiceNotification(this);
            mCi->UnSetOnSs(this);

            mPendingMMIs->Clear();

            //Force all referenced classes to unregister their former registered events
            mCT->Dispose();
            mDcTracker->Dispose();
            mSST->Dispose();
            mSimPhoneBookIntManager->Dispose();
            mSubInfo->Dispose();
        }
    }

    //@Override
    CARAPI RemoveReferences() {
        Rlog->D(LOG_TAG, "removeReferences");
        mSimulatedRadioControl = NULL;
        mSimPhoneBookIntManager = NULL;
        mSubInfo = NULL;
        mCT = NULL;
        mSST = NULL;

        super->RemoveReferences();
    }

    //@Override
    protected void Finalize() {
        If(LOCAL_DEBUG) Rlog->D(LOG_TAG, "GSMPhone finalized");
    }


    private void OnSubscriptionActivated() {
        //mSubscriptionData = SubscriptionManager->GetCurrentSubscription(mSubscription);

        Log("SUBSCRIPTION ACTIVATED : slotId : " + mSubscriptionData.slotId
                + " appid : " + mSubscriptionData.m3gppIndex
                + " subId : " + mSubscriptionData.subId
                + " subStatus : " + mSubscriptionData.subStatus);

        // Make sure properties are set for proper subscription.
        SetProperties();

        OnUpdateIccAvailability();
        mSST->SendMessage(mSST->ObtainMessage(ServiceStateTracker.EVENT_ICC_CHANGED));
        ((DcTracker)mDcTracker).UpdateRecords();
    }

    private void OnSubscriptionDeactivated() {
        Log("SUBSCRIPTION DEACTIVATED");
        mSubscriptionData = NULL;
        ResetSubSpecifics();
    }

    //@Override
    public ServiceState
    GetServiceState() {
        If (mSST == NULL || mSST.mSS->GetState() != ServiceState.STATE_IN_SERVICE) {
            If (mImsPhone != NULL &&
                    mImsPhone->GetServiceState()->GetState() == ServiceState.STATE_IN_SERVICE) {
                return mImsPhone->GetServiceState();
            }
        }

        If (mSST != NULL) {
            return mSST.mSS;
        } else {
            // avoid potential NPE in EmergencyCallHelper during Phone switch
            return new ServiceState();
        }
    }

    //@Override
    public CellLocation GetCellLocation() {
        return mSST->GetCellLocation();
    }

    //@Override
    public PhoneConstants.State GetState() {
        return mCT.mState;
    }

    //@Override
    public Int32 GetPhoneType() {
        return PhoneConstants.PHONE_TYPE_GSM;
    }

    //@Override
    public ServiceStateTracker GetServiceStateTracker() {
        return mSST;
    }

    //@Override
    public CallTracker GetCallTracker() {
        return mCT;
    }

    // pending voice mail count updated after phone creation
    private void UpdateVoiceMail() {
        Int32 countVoiceMessages = 0;
        IccRecords r = mIccRecords->Get();
        If (r != NULL) {
            // get voice mail count from SIM
            countVoiceMessages = r->GetVoiceMessageCount();
        }
        If (countVoiceMessages == -1) {
            countVoiceMessages = GetStoredVoiceMessageCount();
        }
        Rlog->D(LOG_TAG, "updateVoiceMail countVoiceMessages = " + countVoiceMessages
                +" subId "+GetSubId());
        SetVoiceMessageCount(countVoiceMessages);
    }

    public Boolean GetCallForwardingIndicator() {
        Boolean cf = FALSE;
        IccRecords r = mIccRecords->Get();
        If (r != NULL && r->IsCallForwardStatusStored()) {
            cf = r->GetVoiceCallForwardingFlag();
        } else {
            cf = GetCallForwardingPreference();
        }
        return cf;
    }

    //@Override
    public List<? extends MmiCode>
    GetPendingMmiCodes() {
        return mPendingMMIs;
    }

    //@Override
    public PhoneConstants.DataState GetDataConnectionState(String apnType) {
        PhoneConstants.DataState ret = PhoneConstants.DataState.DISCONNECTED;

        If (mSST == NULL) {
            // Radio Technology Change is ongoning, Dispose() and RemoveReferences() have
            // already been called

            ret = PhoneConstants.DataState.DISCONNECTED;
        } else If (!apnType->Equals(PhoneConstants.APN_TYPE_EMERGENCY) &&
                mSST->GetCurrentDataConnectionState() != ServiceState.STATE_IN_SERVICE
                && mOosIsDisconnect) {
            // If we're out of service, open TCP sockets may still work
            // but no data will flow

            // Emergency APN is available even in Out Of Service
            // Pass the actual State of EPDN

            ret = PhoneConstants.DataState.DISCONNECTED;
            Rlog->D(LOG_TAG, "getDataConnectionState: Data is Out of Service. ret = " + ret);
        } else If (mDcTracker->IsApnTypeEnabled(apnType) == FALSE ||
                mDcTracker->IsApnTypeActive(apnType) == FALSE) {
            //TODO: IsApnTypeActive() is just checking whether ApnContext holds
            //      Dataconnection or not. Checking each ApnState below should
            //      provide the same state. Calling IsApnTypeActive() can be removed.
            ret = PhoneConstants.DataState.DISCONNECTED;
        } else { /* mSST.gprsState == ServiceState.STATE_IN_SERVICE */
            Switch (mDcTracker->GetState(apnType)) {
                case RETRYING:
                case FAILED:
                case IDLE:
                    ret = PhoneConstants.DataState.DISCONNECTED;
                break;

                case CONNECTED:
                case DISCONNECTING:
                    If ( mCT.mState != PhoneConstants.State.IDLE
                            && !mSST->IsConcurrentVoiceAndDataAllowed()) {
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

        return ret;
    }

    //@Override
    public DataActivityState GetDataActivityState() {
        DataActivityState ret = DataActivityState.NONE;

        If (mSST->GetCurrentDataConnectionState() == ServiceState.STATE_IN_SERVICE) {
            Switch (mDcTracker->GetActivity()) {
                case DATAIN:
                    ret = DataActivityState.DATAIN;
                break;

                case DATAOUT:
                    ret = DataActivityState.DATAOUT;
                break;

                case DATAINANDOUT:
                    ret = DataActivityState.DATAINANDOUT;
                break;

                case DORMANT:
                    ret = DataActivityState.DORMANT;
                break;

                default:
                    ret = DataActivityState.NONE;
                break;
            }
        }

        return ret;
    }

    /**
     * Notify any interested party of a Phone state change
     * {@link com.android.internal.telephony.PhoneConstants.State}
     */
    /*package*/ void NotifyPhoneStateChanged() {
        mNotifier->NotifyPhoneState(this);
    }

    /**
     * Notify registrants of a change in the call state. This notifies changes in
     * {@link com.android.internal.telephony.Call.State}. Use this when changes
     * in the precise call state are needed, else use notifyPhoneStateChanged.
     */
    /*package*/ void NotifyPreciseCallStateChanged() {
        /* we'd love it if this was package-scoped*/
        super->NotifyPreciseCallStateChangedP();
    }

    CARAPI NotifyNewRingingConnection(Connection c) {
        super->NotifyNewRingingConnectionP(c);
    }

    /*package*/ void
    NotifyDisconnect(Connection cn) {
        mDisconnectRegistrants->NotifyResult(cn);

        mNotifier->NotifyDisconnectCause(cn->GetDisconnectCause(), cn->GetPreciseDisconnectCause());
    }

    void NotifyUnknownConnection(Connection cn) {
        mUnknownConnectionRegistrants->NotifyResult(cn);
    }

    void NotifySuppServiceFailed(SuppService code) {
        mSuppServiceFailedRegistrants->NotifyResult(code);
    }

    /*package*/ void
    NotifyServiceStateChanged(ServiceState ss) {
        super->NotifyServiceStateChangedP(ss);
    }

    /*package*/
    void NotifyLocationChanged() {
        mNotifier->NotifyCellLocation(this);
    }

    //@Override
    CARAPI
    NotifyCallForwardingIndicator() {
        mNotifier->NotifyCallForwardingChanged(this);
    }

    // override for allowing access from other classes of this package
    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI
    SetSystemProperty(String property, String value) {
        TelephonyManager->SetTelephonyProperty(property, GetSubId(), value);
    }

    //@Override
    CARAPI RegisterForSuppServiceNotification(
            Handler h, Int32 what, Object obj) {
        mSsnRegistrants->AddUnique(h, what, obj);
        If (mSsnRegistrants->Size() == 1) mCi->SetSuppServiceNotifications(TRUE, NULL);
    }

    //@Override
    CARAPI UnregisterForSuppServiceNotification(Handler h) {
        mSsnRegistrants->Remove(h);
        If (mSsnRegistrants->Size() == 0) mCi->SetSuppServiceNotifications(FALSE, NULL);
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
    CARAPI
    AcceptCall(Int32 videoState) throws CallStateException {
        ImsPhone imsPhone = mImsPhone;
        If ( imsPhone != NULL && imsPhone->GetRingingCall()->IsRinging() ) {
            imsPhone->AcceptCall(videoState);
        } else {
            mCT->AcceptCall();
        }
    }

    //@Override
    CARAPI
    DeflectCall(String number) throws CallStateException {
        ImsPhone imsPhone = mImsPhone;
        If ( imsPhone != NULL && imsPhone->GetRingingCall()->IsRinging() ) {
            imsPhone->DeflectCall(number);
        } else {
            throw new CallStateException("Deflect call NOT supported in GSM!");
        }
    }

    //@Override
    CARAPI
    RejectCall() throws CallStateException {
        mCT->RejectCall();
    }

    //@Override
    CARAPI
    SwitchHoldingAndActive() throws CallStateException {
        mCT->SwitchWaitingOrHoldingAndActive();
    }

    //@Override
    public Boolean CanConference() {
        Boolean canImsConference = FALSE;
        If (mImsPhone != NULL) {
            canImsConference = mImsPhone->CanConference();
        }
        return mCT->CanConference() || canImsConference;
    }

    public Boolean CanDial() {
        return mCT->CanDial();
    }

    //@Override
    CARAPI Conference() {
        If (mImsPhone != NULL && mImsPhone->CanConference()) {
            Log("Conference() - delegated to IMS phone");
            mImsPhone->Conference();
            return;
        }
        mCT->Conference();
    }

    //@Override
    CARAPI ClearDisconnected() {
        mCT->ClearDisconnected();
    }

    //@Override
    public Boolean CanTransfer() {
        return mCT->CanTransfer();
    }

    //@Override
    CARAPI ExplicitCallTransfer() {
        mCT->ExplicitCallTransfer();
    }

    //@Override
    public GsmCall
    GetForegroundCall() {
        return mCT.mForegroundCall;
    }

    //@Override
    public GsmCall
    GetBackgroundCall() {
        return mCT.mBackgroundCall;
    }

    //@Override
    public Call GetRingingCall() {
        ImsPhone imsPhone = mImsPhone;
        If ( mCT.mRingingCall != NULL && mCT.mRingingCall->IsRinging() ) {
            return mCT.mRingingCall;
        } else If ( imsPhone != NULL ) {
            return imsPhone->GetRingingCall();
        }
        return mCT.mRingingCall;
    }

    private Boolean HandleCallDeflectionIncallSupplementaryService(
            String dialString) {
        If (dialString->Length() > 1) {
            return FALSE;
        }

        If (GetRingingCall()->GetState() != GsmCall.State.IDLE) {
            If (LOCAL_DEBUG) Rlog->D(LOG_TAG, "MmiCode 0: rejectCall");
            try {
                mCT->RejectCall();
            } Catch (CallStateException e) {
                If (LOCAL_DEBUG) Rlog->D(LOG_TAG,
                    "reject failed", e);
                NotifySuppServiceFailed(Phone.SuppService.REJECT);
            }
        } else If (GetBackgroundCall()->GetState() != GsmCall.State.IDLE) {
            If (LOCAL_DEBUG) Rlog->D(LOG_TAG,
                    "MmiCode 0: hangupWaitingOrBackground");
            mCT->HangupWaitingOrBackground();
        }

        return TRUE;
    }

    private Boolean HandleCallWaitingIncallSupplementaryService(
            String dialString) {
        Int32 len = dialString->Length();

        If (len > 2) {
            return FALSE;
        }

        GsmCall call = GetForegroundCall();

        try {
            If (len > 1) {
                Char32 ch = dialString->CharAt(1);
                Int32 callIndex = ch - '0';

                If (callIndex >= 1 && callIndex <= GsmCallTracker.MAX_CONNECTIONS) {
                    If (LOCAL_DEBUG) Rlog->D(LOG_TAG,
                            "MmiCode 1: hangupConnectionByIndex " +
                            callIndex);
                    mCT->HangupConnectionByIndex(call, callIndex);
                }
            } else {
                If (call->GetState() != GsmCall.State.IDLE) {
                    If (LOCAL_DEBUG) Rlog->D(LOG_TAG,
                            "MmiCode 1: hangup foreground");
                    //mCT->HangupForegroundResumeBackground();
                    mCT->Hangup(call);
                } else {
                    If (LOCAL_DEBUG) Rlog->D(LOG_TAG,
                            "MmiCode 1: switchWaitingOrHoldingAndActive");
                    mCT->SwitchWaitingOrHoldingAndActive();
                }
            }
        } Catch (CallStateException e) {
            If (LOCAL_DEBUG) Rlog->D(LOG_TAG,
                "hangup failed", e);
            NotifySuppServiceFailed(Phone.SuppService.HANGUP);
        }

        return TRUE;
    }

    private Boolean HandleCallHoldIncallSupplementaryService(String dialString) {
        Int32 len = dialString->Length();

        If (len > 2) {
            return FALSE;
        }

        GsmCall call = GetForegroundCall();

        If (len > 1) {
            try {
                Char32 ch = dialString->CharAt(1);
                Int32 callIndex = ch - '0';
                GsmConnection conn = mCT->GetConnectionByIndex(call, callIndex);

                // gsm index starts at 1, up to 5 connections in a call,
                If (conn != NULL && callIndex >= 1 && callIndex <= GsmCallTracker.MAX_CONNECTIONS) {
                    If (LOCAL_DEBUG) Rlog->D(LOG_TAG, "MmiCode 2: separate call "+
                            callIndex);
                    mCT->Separate(conn);
                } else {
                    If (LOCAL_DEBUG) Rlog->D(LOG_TAG, "separate: invalid call index "+
                            callIndex);
                    NotifySuppServiceFailed(Phone.SuppService.SEPARATE);
                }
            } Catch (CallStateException e) {
                If (LOCAL_DEBUG) Rlog->D(LOG_TAG,
                    "separate failed", e);
                NotifySuppServiceFailed(Phone.SuppService.SEPARATE);
            }
        } else {
            try {
                If (GetRingingCall()->GetState() != GsmCall.State.IDLE) {
                    If (LOCAL_DEBUG) Rlog->D(LOG_TAG,
                    "MmiCode 2: accept ringing call");
                    mCT->AcceptCall();
                } else {
                    If (LOCAL_DEBUG) Rlog->D(LOG_TAG,
                    "MmiCode 2: switchWaitingOrHoldingAndActive");
                    mCT->SwitchWaitingOrHoldingAndActive();
                }
            } Catch (CallStateException e) {
                If (LOCAL_DEBUG) Rlog->D(LOG_TAG,
                    "switch failed", e);
                NotifySuppServiceFailed(Phone.SuppService.SWITCH);
            }
        }

        return TRUE;
    }

    private Boolean HandleMultipartyIncallSupplementaryService(
            String dialString) {
        If (dialString->Length() > 1) {
            return FALSE;
        }

        If (LOCAL_DEBUG) Rlog->D(LOG_TAG, "MmiCode 3: merge calls");
        Conference();
        return TRUE;
    }

    private Boolean HandleEctIncallSupplementaryService(String dialString) {

        Int32 len = dialString->Length();

        If (len != 1) {
            return FALSE;
        }

        If (LOCAL_DEBUG) Rlog->D(LOG_TAG, "MmiCode 4: explicit call transfer");
        ExplicitCallTransfer();
        return TRUE;
    }

    private Boolean HandleCcbsIncallSupplementaryService(String dialString) {
        If (dialString->Length() > 1) {
            return FALSE;
        }

        Rlog->I(LOG_TAG, "MmiCode 5: CCBS not supported!");
        // Treat it as an "unknown" service.
        NotifySuppServiceFailed(Phone.SuppService.UNKNOWN);
        return TRUE;
    }

    //@Override
    public Boolean HandleInCallMmiCommands(String dialString) throws CallStateException {
        ImsPhone imsPhone = mImsPhone;
        If (imsPhone != NULL
                && imsPhone->GetServiceState()->GetState() == ServiceState.STATE_IN_SERVICE) {
            return imsPhone->HandleInCallMmiCommands(dialString);
        }

        If (!IsInCall()) {
            return FALSE;
        }

        If (TextUtils->IsEmpty(dialString)) {
            return FALSE;
        }

        Boolean result = FALSE;
        Char32 ch = dialString->CharAt(0);
        Switch (ch) {
            case '0':
                result = HandleCallDeflectionIncallSupplementaryService(
                        dialString);
                break;
            case '1':
                result = HandleCallWaitingIncallSupplementaryService(
                        dialString);
                break;
            case '2':
                result = HandleCallHoldIncallSupplementaryService(dialString);
                break;
            case '3':
                result = HandleMultipartyIncallSupplementaryService(dialString);
                break;
            case '4':
                result = HandleEctIncallSupplementaryService(dialString);
                break;
            case '5':
                result = HandleCcbsIncallSupplementaryService(dialString);
                break;
            default:
                break;
        }

        return result;
    }

    Boolean IsInCall() {
        GsmCall.State foregroundCallState = GetForegroundCall()->GetState();
        GsmCall.State backgroundCallState = GetBackgroundCall()->GetState();
        GsmCall.State ringingCallState = GetRingingCall()->GetState();

       Return (foregroundCallState->IsAlive() ||
                backgroundCallState->IsAlive() ||
                ringingCallState->IsAlive());
    }

    //@Override
    public Connection
    Dial(String dialString, Int32 videoState, Bundle extras) throws CallStateException {
        return Dial(dialString, NULL, videoState, extras);
    }

    //@Override
    public Connection
    Dial(String dialString, Int32 videoState) throws CallStateException {
        return Dial(dialString, NULL, videoState, NULL);
    }

    //@Override
    public Connection
    Dial (String dialString, UUSInfo uusInfo, Int32 videoState) throws CallStateException {
        return Dial(dialString, uusInfo, videoState, NULL);
    }

    public Connection
    Dial (String dialString, UUSInfo uusInfo, Int32 videoState, Bundle extras)
            throws CallStateException {
        ImsPhone imsPhone = mImsPhone;

        Boolean imsUseEnabled =
                ImsManager->IsEnhanced4gLteModeSettingEnabledByPlatform(mContext) &&
                ImsManager->IsEnhanced4gLteModeSettingEnabledByUser(mContext);
        If (!imsUseEnabled) {
            Rlog->W(LOG_TAG, "IMS is disabled: forced to CS");
        }

        If (imsUseEnabled && imsPhone != NULL
                && ((imsPhone->GetServiceState()->GetState() == ServiceState.STATE_IN_SERVICE
                && !PhoneNumberUtils->IsEmergencyNumber(dialString))
                || (PhoneNumberUtils->IsEmergencyNumber(dialString)
                && mContext->GetResources()->GetBoolean(
                        R.bool.useImsAlwaysForEmergencyCall))) ) {
            try {
                If (LOCAL_DEBUG) Rlog->D(LOG_TAG, "Trying IMS PS call");
                return imsPhone->Dial(dialString, videoState, extras);
            } Catch (CallStateException e) {
                If (LOCAL_DEBUG) Rlog->D(LOG_TAG, "IMS PS call exception " + e);
                If (!ImsPhone.CS_FALLBACK->Equals(e->GetMessage())) {
                    CallStateException ce = new CallStateException(e->GetMessage());
                    ce->SetStackTrace(e->GetStackTrace());
                    throw ce;
                }
            }
        }

        If (LOCAL_DEBUG) Rlog->D(LOG_TAG, "Trying (non-IMS) CS call");
        return DialInternal(dialString, NULL, VideoProfile.VideoState.AUDIO_ONLY);
    }

    //@Override
    protected Connection
    DialInternal (String dialString, UUSInfo uusInfo, Int32 videoState)
            throws CallStateException {

        // Need to make sure dialString gets parsed properly
        String newDialString = PhoneNumberUtils->StripSeparators(dialString);

        // handle in-call MMI first if applicable
        If (HandleInCallMmiCommands(newDialString)) {
            return NULL;
        }

        // Only look at the Network portion for mmi
        String networkPortion = PhoneNumberUtils->ExtractNetworkPortionAlt(newDialString);
        GsmMmiCode mmi =
                GsmMmiCode->NewFromDialString(networkPortion, this, mUiccApplication->Get());
        If (LOCAL_DEBUG) Rlog->D(LOG_TAG,
                               "dialing w/ mmi '" + mmi + "'...");

        If (mmi == NULL) {
            return mCT->Dial(newDialString, uusInfo);
        } else If (mmi->IsTemporaryModeCLIR()) {
            return mCT->Dial(mmi.mDialingNumber, mmi->GetCLIRMode(), uusInfo);
        } else {
            mPendingMMIs->Add(mmi);
            mMmiRegistrants->NotifyRegistrants(new AsyncResult(NULL, mmi, NULL));
            mmi->ProcessCode();

            // FIXME should this return NULL or something else?
            return NULL;
        }
    }

    //@Override
    CARAPI AddParticipant(String dialString) throws CallStateException {
        ImsPhone imsPhone = mImsPhone;
        Boolean imsUseEnabled =
                ImsManager->IsEnhanced4gLteModeSettingEnabledByPlatform(mContext) &&
                ImsManager->IsEnhanced4gLteModeSettingEnabledByUser(mContext);

        If (imsUseEnabled && imsPhone != NULL
                && imsPhone->GetServiceState()->GetState() == ServiceState.STATE_IN_SERVICE
                ) {
            try {
                If (LOCAL_DEBUG) Rlog->D(LOG_TAG, "Trying to add participant in IMS call");
                imsPhone->AddParticipant(dialString);
            } Catch (CallStateException e) {
                If (LOCAL_DEBUG) Rlog->D(LOG_TAG, "IMS PS call exception " + e);
            }
        } else {
            Rlog->E(LOG_TAG, "IMS is disabled so unable to add participant with IMS call");
        }
    }

    //@Override
    public Boolean HandlePinMmi(String dialString) {
        GsmMmiCode mmi = GsmMmiCode->NewFromDialString(dialString, this, mUiccApplication->Get());

        If (mmi != NULL && mmi->IsPinPukCommand()) {
            mPendingMMIs->Add(mmi);
            mMmiRegistrants->NotifyRegistrants(new AsyncResult(NULL, mmi, NULL));
            mmi->ProcessCode();
            return TRUE;
        }

        return FALSE;
    }

    //@Override
    CARAPI SendUssdResponse(String ussdMessge) {
        GsmMmiCode mmi = GsmMmiCode->NewFromUssdUserInput(ussdMessge, this, mUiccApplication->Get());
        mPendingMMIs->Add(mmi);
        mMmiRegistrants->NotifyRegistrants(new AsyncResult(NULL, mmi, NULL));
        mmi->SendUssd(ussdMessge);
    }

    //@Override
    CARAPI
    SendDtmf(Char32 c) {
        If (!PhoneNumberUtils->Is12Key(c)) {
            Rlog->E(LOG_TAG,
                    "sendDtmf called with invalid character '" + c + "'");
        } else {
            If (mCT.mState ==  PhoneConstants.State.OFFHOOK) {
                mCi->SendDtmf(c, NULL);
            }
        }
    }

    //@Override
    CARAPI
    StartDtmf(Char32 c) {
        If (!(PhoneNumberUtils->Is12Key(c) || (c >= 'A' && c <= 'D'))) {
            Rlog->E(LOG_TAG,
                "startDtmf called with invalid character '" + c + "'");
        } else {
            mCi->StartDtmf(c, NULL);
        }
    }

    //@Override
    CARAPI
    StopDtmf() {
        mCi->StopDtmf(NULL);
    }

    CARAPI
    SendBurstDtmf(String dtmfString) {
        Rlog->E(LOG_TAG, "[GSMPhone] SendBurstDtmf() is a CDMA method");
    }

    //@Override
    CARAPI
    SetRadioPower(Boolean power) {
        mSST->SetRadioPower(power);
    }

    private void StoreVoiceMailNumber(String number) {
        SharedPreferences sp = PreferenceManager->GetDefaultSharedPreferences(GetContext());
        SharedPreferences.Editor editor = sp->Edit();
        editor->PutString(VM_NUMBER + GetSubId(), number);
        editor->Apply();
        SetSimImsi(GetSubscriberId());
    }

    //@Override
    public String GetVoiceMailNumber() {
        // Read from the SIM. If its NULL, try reading from the shared preference area.
        IccRecords r = mIccRecords->Get();
        String number = (r != NULL) ? r->GetVoiceMailNumber() : "";
        If (TextUtils->IsEmpty(number)) {
            SharedPreferences sp = PreferenceManager->GetDefaultSharedPreferences(GetContext());
            number = sp->GetString(VM_NUMBER + GetSubId(), NULL);
        }

        If (TextUtils->IsEmpty(number)) {
            String[] listArray = GetContext()->GetResources()
                .GetStringArray(R.array.config_default_vm_number);
            If (listArray != NULL && listArray.length > 0) {
                For (Int32 i=0; i<listArray.length; i++) {
                    If (!TextUtils->IsEmpty(listArray[i])) {
                        String[] defaultVMNumberArray = listArray[i].Split(";");
                        If (defaultVMNumberArray != NULL && defaultVMNumberArray.length > 0) {
                            If (defaultVMNumberArray.length == 1) {
                                number = defaultVMNumberArray[0];
                            } else If (defaultVMNumberArray.length == 2 &&
                                    !TextUtils->IsEmpty(defaultVMNumberArray[1]) &&
                                    defaultVMNumberArray[1].EqualsIgnoreCase(GetGroupIdLevel1())) {
                                number = defaultVMNumberArray[0];
                                break;
                            }
                        }
                    }
                }
            }
        }
        return number;
    }

    private String GetSimImsi() {
        SharedPreferences sp = PreferenceManager->GetDefaultSharedPreferences(GetContext());
        return sp->GetString(SIM_IMSI + GetSubId(), NULL);
    }

    private void SetSimImsi(String imsi) {
        SharedPreferences sp = PreferenceManager->GetDefaultSharedPreferences(GetContext());
        SharedPreferences.Editor editor = sp->Edit();
        editor->PutString(SIM_IMSI + GetSubId(), imsi);
        editor->Apply();
    }

    //@Override
    public String GetVoiceMailAlphaTag() {
        String ret;
        IccRecords r = mIccRecords->Get();

        ret = (r != NULL) ? r->GetVoiceMailAlphaTag() : "";

        If (ret == NULL || ret->Length() == 0) {
            return mContext->GetText(
                R::string::defaultVoiceMailAlphaTag).ToString();
        }

        return ret;
    }

    //@Override
    public String GetDeviceId() {
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
    public String GetImei() {
        return mImei;
    }

    //@Override
    public String GetEsn() {
        Rlog->E(LOG_TAG, "[GSMPhone] GetEsn() is a CDMA method");
        return "0";
    }

    //@Override
    public String GetMeid() {
        Rlog->E(LOG_TAG, "[GSMPhone] GetMeid() is a CDMA method");
        return "0";
    }

    //@Override
    public String GetSubscriberId() {
        IccRecords r = mIccRecords->Get();
        Return (r != NULL) ? r->GetIMSI() : NULL;
    }

    //@Override
    public String GetGroupIdLevel1() {
        IccRecords r = mIccRecords->Get();
        Return (r != NULL) ? r->GetGid1() : NULL;
    }

    //@Override
    public String GetLine1Number() {
        IccRecords r = mIccRecords->Get();
        Return (r != NULL) ? r->GetMsisdnNumber() : NULL;
    }

    //@Override
    public String GetMsisdn() {
        IccRecords r = mIccRecords->Get();
        Return (r != NULL) ? r->GetMsisdnNumber() : NULL;
    }

    //@Override
    public String GetLine1AlphaTag() {
        IccRecords r = mIccRecords->Get();
        Return (r != NULL) ? r->GetMsisdnAlphaTag() : NULL;
    }

    //@Override
    CARAPI SetLine1Number(String alphaTag, String number, Message onComplete) {
        IccRecords r = mIccRecords->Get();
        If (r != NULL) {
            r->SetMsisdnNumber(alphaTag, number, onComplete);
        }
    }

    //@Override
    CARAPI SetVoiceMailNumber(String alphaTag,
                            String voiceMailNumber,
                            Message onComplete) {

        Message resp;
        mVmNumber = voiceMailNumber;
        resp = ObtainMessage(EVENT_SET_VM_NUMBER_DONE, 0, 0, onComplete);
        IccRecords r = mIccRecords->Get();
        If (r != NULL) {
            r->SetVoiceMailNumber(alphaTag, mVmNumber, resp);
        }
    }

    private Boolean IsValidCommandInterfaceCFReason (Int32 commandInterfaceCFReason) {
        Switch (commandInterfaceCFReason) {
        case CF_REASON_UNCONDITIONAL:
        case CF_REASON_BUSY:
        case CF_REASON_NO_REPLY:
        case CF_REASON_NOT_REACHABLE:
        case CF_REASON_ALL:
        case CF_REASON_ALL_CONDITIONAL:
            return TRUE;
        default:
            return FALSE;
        }
    }

    public String GetSystemProperty(String property, String defValue) {
        If(GetUnitTestMode()) {
            return NULL;
        }
        return TelephonyManager->GetTelephonyProperty(property, GetSubId(), defValue);
    }

    private Boolean IsValidCommandInterfaceCFAction (Int32 commandInterfaceCFAction) {
        Switch (commandInterfaceCFAction) {
        case CF_ACTION_DISABLE:
        case CF_ACTION_ENABLE:
        case CF_ACTION_REGISTRATION:
        case CF_ACTION_ERASURE:
            return TRUE;
        default:
            return FALSE;
        }
    }

    CARAPI UpdateDataConnectionTracker() {
        ((DcTracker)mDcTracker).Update();
    }

    protected  Boolean IsCfEnable(Int32 action) {
        Return (action == CF_ACTION_ENABLE) || (action == CF_ACTION_REGISTRATION);
    }

    //@Override
    CARAPI GetCallForwardingOption(Int32 commandInterfaceCFReason, Message onComplete) {
        ImsPhone imsPhone = mImsPhone;
        If ((imsPhone != NULL)
                && (imsPhone->GetServiceState()->GetState() == ServiceState.STATE_IN_SERVICE)) {
            imsPhone->GetCallForwardingOption(commandInterfaceCFReason, onComplete);
            return;
        }

        If (IsValidCommandInterfaceCFReason(commandInterfaceCFReason)) {
            If (LOCAL_DEBUG) Rlog->D(LOG_TAG, "requesting call forwarding query.");
            Message resp;
            If (commandInterfaceCFReason == CF_REASON_UNCONDITIONAL) {
                resp = ObtainMessage(EVENT_GET_CALL_FORWARD_DONE, onComplete);
            } else {
                resp = onComplete;
            }
            mCi->QueryCallForwardStatus(commandInterfaceCFReason,0,NULL,resp);
        }
    }

    //@Override
    CARAPI SetCallForwardingOption(Int32 commandInterfaceCFAction,
            Int32 commandInterfaceCFReason,
            String dialingNumber,
            Int32 timerSeconds,
            Message onComplete) {
        ImsPhone imsPhone = mImsPhone;
        If ((imsPhone != NULL)
                && (imsPhone->GetServiceState()->GetState() == ServiceState.STATE_IN_SERVICE)) {
            imsPhone->SetCallForwardingOption(commandInterfaceCFAction,
                    commandInterfaceCFReason, dialingNumber, timerSeconds, onComplete);
            return;
        }

        If (    (IsValidCommandInterfaceCFAction(commandInterfaceCFAction)) &&
                (IsValidCommandInterfaceCFReason(commandInterfaceCFReason))) {

            Message resp;
            If (commandInterfaceCFReason == CF_REASON_UNCONDITIONAL) {
                Cfu cfu = new Cfu(dialingNumber, onComplete);
                resp = ObtainMessage(EVENT_SET_CALL_FORWARD_DONE,
                        IsCfEnable(commandInterfaceCFAction) ? 1 : 0, 0, cfu);
            } else {
                resp = onComplete;
            }
            mCi->SetCallForward(commandInterfaceCFAction,
                    commandInterfaceCFReason,
                    CommandsInterface.SERVICE_CLASS_VOICE,
                    dialingNumber,
                    timerSeconds,
                    resp);
        }
    }

    //@Override
    CARAPI SetCallForwardingUncondTimerOption(Int32 startHour, Int32 startMinute,
            Int32 endHour, Int32 endMinute, Int32 commandInterfaceCFAction,
            Int32 commandInterfaceCFReason, String dialingNumber, Message onComplete) {

        ImsPhone imsPhone = mImsPhone;
        If (imsPhone != NULL) {
            imsPhone->SetCallForwardingUncondTimerOption(startHour, startMinute, endHour,
                    endMinute, commandInterfaceCFAction, commandInterfaceCFReason,
                    dialingNumber, onComplete);
        } else {
            If (onComplete != NULL) {
                AsyncResult->ForMessage(onComplete, NULL,
                        new CommandException(CommandException.Error.GENERIC_FAILURE));
                onComplete->SendToTarget();
           }
        }
    }

    //@Override
    CARAPI GetCallForwardingUncondTimerOption(Int32 commandInterfaceCFReason,
            Message onComplete) {

        ImsPhone imsPhone = mImsPhone;
        If (imsPhone != NULL) {
            imsPhone->GetCallForwardingOption(commandInterfaceCFReason, onComplete);
        } else {
            If (onComplete != NULL) {
                AsyncResult->ForMessage(onComplete, NULL,
                        new CommandException(CommandException.Error.GENERIC_FAILURE));
                onComplete->SendToTarget();
            }
        }
    }

    //@Override
    CARAPI GetOutgoingCallerIdDisplay(Message onComplete) {
        mCi->GetCLIR(onComplete);
    }

    //@Override
    CARAPI SetOutgoingCallerIdDisplay(Int32 commandInterfaceCLIRMode,
                                           Message onComplete) {
        mCi->SetCLIR(commandInterfaceCLIRMode,
                ObtainMessage(EVENT_SET_CLIR_COMPLETE, commandInterfaceCLIRMode, 0, onComplete));
    }

    //@Override
    CARAPI GetCallWaiting(Message onComplete) {
        ImsPhone imsPhone = mImsPhone;
        If ((imsPhone != NULL)
                && (imsPhone->GetServiceState()->GetState() == ServiceState.STATE_IN_SERVICE)) {
            imsPhone->GetCallWaiting(onComplete);
            return;
        }

        //As per 3GPP TS 24.083, section 1.6 UE doesn't need to send service
        //class parameter in call waiting interrogation  to network
        mCi->QueryCallWaiting(CommandsInterface.SERVICE_CLASS_NONE, onComplete);
    }

    //@Override
    CARAPI SetCallWaiting(Boolean enable, Message onComplete) {
        ImsPhone imsPhone = mImsPhone;
        If ((imsPhone != NULL)
                && (imsPhone->GetServiceState()->GetState() == ServiceState.STATE_IN_SERVICE)) {
            imsPhone->SetCallWaiting(enable, onComplete);
            return;
        }

        mCi->SetCallWaiting(enable, CommandsInterface.SERVICE_CLASS_VOICE, onComplete);
    }

    //@Override
    CARAPI
    GetAvailableNetworks(Message response) {
        mCi->GetAvailableNetworks(response);
    }

    //@Override
    CARAPI
    GetNeighboringCids(Message response) {
        mCi->GetNeighboringCids(response);
    }

    //@Override
    CARAPI SetOnPostDialCharacter(Handler h, Int32 what, Object obj) {
        mPostDialHandler = new Registrant(h, what, obj);
    }

    //@Override
    CARAPI SetUiTTYMode(Int32 uiTtyMode, Message onComplete) {
       If (mImsPhone != NULL) {
           mImsPhone->SetUiTTYMode(uiTtyMode, onComplete);
       }
    }

    //@Override
    CARAPI SetMute(Boolean muted) {
        mCT->SetMute(muted);
    }

    //@Override
    public Boolean GetMute() {
        return mCT->GetMute();
    }

    //@Override
    CARAPI GetDataCallList(Message response) {
        mCi->GetDataCallList(response);
    }

    //@Override
    CARAPI UpdateServiceLocation() {
        mSST->EnableSingleLocationUpdate();
    }

    //@Override
    CARAPI EnableLocationUpdates() {
        mSST->EnableLocationUpdates();
    }

    //@Override
    CARAPI DisableLocationUpdates() {
        mSST->DisableLocationUpdates();
    }

    //@Override
    public Boolean GetDataRoamingEnabled() {
        return mDcTracker->GetDataOnRoamingEnabled();
    }

    //@Override
    CARAPI SetDataRoamingEnabled(Boolean enable) {
        mDcTracker->SetDataOnRoamingEnabled(enable);
    }

    //@Override
    public Boolean GetDataEnabled() {
        return mDcTracker->GetDataEnabled();
    }

    //@Override
    CARAPI SetDataEnabled(Boolean enable) {
        mDcTracker->SetDataEnabled(enable);
    }

    /**
     * Removes the given MMI from the pending list and notifies
     * registrants that it is complete.
     * @param mmi MMI that is done
     */
    /*package*/ void
    OnMMIDone(GsmMmiCode mmi) {
        /* Only notify complete if it's on the pending list.
         * Otherwise, it's already been Handled (eg, previously canceled).
         * The exception is cancellation of an incoming USSD-REQUEST, which is
         * not on the list.
         */
        If (mPendingMMIs->Remove(mmi) || mmi->IsUssdRequest() || mmi->IsSsInfo()) {
            mMmiCompleteRegistrants->NotifyRegistrants(
                new AsyncResult(NULL, mmi, NULL));
        }
    }

    /**
     * This method stores the CF_ENABLED flag in preferences
     * @param enabled
     */
    protected void SetCallForwardingPreference(Boolean enabled) {
        If (LOCAL_DEBUG) Rlog->D(LOG_TAG, "Set callforwarding info to perferences");
        SharedPreferences sp = PreferenceManager->GetDefaultSharedPreferences(mContext);
        SharedPreferences.Editor edit = sp->Edit();
        edit->PutBoolean(CF_ENABLED + GetSubId(), enabled);
        edit->Commit();

        // set the sim imsi to be able to track when the sim card is changed.
        SetSimImsi(GetSubscriberId());
    }

    protected Boolean GetCallForwardingPreference() {
        If (LOCAL_DEBUG) Rlog->D(LOG_TAG, "Get callforwarding info from perferences");

        SharedPreferences sp = PreferenceManager->GetDefaultSharedPreferences(mContext);
        Boolean cf = sp->GetBoolean(CF_ENABLED + GetSubId(), FALSE);
        return cf;
    }

    /**
     * Used to check if Call Forwarding status is present on sim card. If not, a message is
     * sent so we can check if the CF status is stored as a Shared Preference.
     */
    private void UpdateCallForwardStatus() {
        If (LOCAL_DEBUG) Rlog->D(LOG_TAG, "updateCallForwardStatus got sim records");
        IccRecords r = mIccRecords->Get();
        If (r != NULL && r->IsCallForwardStatusStored()) {
            // The Sim card has the CF info
            If (LOCAL_DEBUG) Rlog->D(LOG_TAG, "Callforwarding info is present on sim");
            NotifyCallForwardingIndicator();
        } else {
            Message msg = ObtainMessage(CHECK_CALLFORWARDING_STATUS);
            SendMessage(msg);
        }
    }

    private void
    OnNetworkInitiatedUssd(GsmMmiCode mmi) {
        mMmiCompleteRegistrants->NotifyRegistrants(
            new AsyncResult(NULL, mmi, NULL));
    }


    /** ussdMode is one of CommandsInterface.USSD_MODE_* */
    private void
    OnIncomingUSSD (Int32 ussdMode, String ussdMessage) {
        Boolean isUssdError;
        Boolean isUssdRequest;

        isUssdRequest
            = (ussdMode == CommandsInterface.USSD_MODE_REQUEST);

        isUssdError
            = (ussdMode != CommandsInterface.USSD_MODE_NOTIFY
                && ussdMode != CommandsInterface.USSD_MODE_REQUEST);

        // See comments in GsmMmiCode.java
        // USSD requests aren't finished until one
        // of these two events happen
        GsmMmiCode found = NULL;
        For (Int32 i = 0, s = mPendingMMIs->Size() ; i < s; i++) {
            If(mPendingMMIs->Get(i).IsPendingUSSD()) {
                found = mPendingMMIs->Get(i);
                break;
            }
        }

        If (found != NULL) {
            // Complete pending USSD

            If (isUssdError) {
                found->OnUssdFinishedError();
            } else {
                found->OnUssdFinished(ussdMessage, isUssdRequest);
            }
        } else { // pending USSD not found
            // The network may initiate its own USSD request

            // ignore everything that isnt a Notify or a Request
            // also, discard if there is no message to present
            If (!isUssdError && ussdMessage != NULL) {
                GsmMmiCode mmi;
                mmi = GsmMmiCode->NewNetworkInitiatedUssd(ussdMessage,
                                                   isUssdRequest,
                                                   GSMPhone.this,
                                                   mUiccApplication->Get());
                OnNetworkInitiatedUssd(mmi);
            }
        }
    }

    /**
     * Make sure the network knows our preferred setting.
     */
    protected  void SyncClirSetting() {
        SharedPreferences sp = PreferenceManager->GetDefaultSharedPreferences(GetContext());
        Int32 clirSetting = sp->GetInt(CLIR_KEY + GetPhoneId(), -1);
        If (clirSetting >= 0) {
            mCi->SetCLIR(clirSetting, NULL);
        }
    }

    //@Override
    CARAPI HandleMessage (Message msg) {
        AsyncResult ar;
        Message onComplete;

        If (!mIsTheCurrentActivePhone) {
            Rlog->E(LOG_TAG, "Received message " + msg +
                    "[" + msg.what + "] while being destroyed. Ignoring.");
            return;
        }
        Switch (msg.what) {
            case EVENT_RADIO_AVAILABLE: {
                mCi->GetBasebandVersion(
                        ObtainMessage(EVENT_GET_BASEBAND_VERSION_DONE));

                mCi->GetIMEI(ObtainMessage(EVENT_GET_IMEI_DONE));
                mCi->GetIMEISV(ObtainMessage(EVENT_GET_IMEISV_DONE));
            }
            break;

            case EVENT_RADIO_ON:
                // do-nothing
                break;

            case EVENT_REGISTERED_TO_NETWORK:
                SyncClirSetting();
                break;

            case EVENT_SIM_RECORDS_LOADED:
                UpdateCurrentCarrierInProvider();

                // Check if this is a different SIM than the previous one. If so unset the
                // voice mail number and the call forwarding flag.
                String imsi = GetSimImsi();
                String imsiFromSIM = GetSubscriberId();
                If (imsi != NULL && imsiFromSIM != NULL && !imsiFromSIM->Equals(imsi)) {
                    StoreVoiceMailNumber(NULL);
                    SetCallForwardingPreference(FALSE);
                    SetSimImsi(NULL);
                    SubscriptionController controller =
                            SubscriptionController->GetInstance();
                    controller->RemoveStaleSubPreferences(VM_NUMBER);
                    controller->RemoveStaleSubPreferences(SIM_IMSI);
                    controller->RemoveStaleSubPreferences(CF_ENABLED);
                }

                mSimRecordsLoadedRegistrants->NotifyRegistrants();
                UpdateVoiceMail();
                UpdateCallForwardStatus();
            break;

            case EVENT_GET_BASEBAND_VERSION_DONE:
                ar = (AsyncResult)msg.obj;

                If (ar.exception != NULL) {
                    break;
                }

                If (LOCAL_DEBUG) Rlog->D(LOG_TAG, "Baseband version: " + ar.result);
                If (!"".Equals((String)ar.result)) {
                    SetSystemProperty(PROPERTY_BASEBAND_VERSION, (String)ar.result);
                }
            break;

            case EVENT_GET_IMEI_DONE:
                ar = (AsyncResult)msg.obj;

                If (ar.exception != NULL) {
                    break;
                }

                mImei = (String)ar.result;
            break;

            case EVENT_GET_IMEISV_DONE:
                ar = (AsyncResult)msg.obj;

                If (ar.exception != NULL) {
                    break;
                }

                mImeiSv = (String)ar.result;
            break;

            case EVENT_USSD:
                ar = (AsyncResult)msg.obj;

                String[] ussdResult = (String[]) ar.result;

                If (ussdResult.length > 1) {
                    try {
                        OnIncomingUSSD(Integer->ParseInt(ussdResult[0]), ussdResult[1]);
                    } Catch (NumberFormatException e) {
                        Rlog->W(LOG_TAG, "error parsing USSD");
                    }
                }
            break;

            case EVENT_RADIO_OFF_OR_NOT_AVAILABLE: {
                // Some MMI Requests (eg USSD) are not completed
                // within the course of a CommandsInterface request
                // If the radio shuts off or resets while one of these
                // is pending, we need to clean up.
                For (Int32 i = mPendingMMIs->Size() - 1; i >= 0; i--) {
                    If (mPendingMMIs->Get(i).IsPendingUSSD()) {
                        mPendingMMIs->Get(i).OnUssdFinishedError();
                    }
                }
                break;
            }

            case EVENT_SSN:
                ar = (AsyncResult)msg.obj;
                SuppServiceNotification not = (SuppServiceNotification) ar.result;
                mSsnRegistrants->NotifyRegistrants(ar);
            break;

            case EVENT_SET_CALL_FORWARD_DONE:
                ar = (AsyncResult)msg.obj;
                IccRecords r = mIccRecords->Get();
                Cfu cfu = (Cfu) ar.userObj;
                If (ar.exception == NULL && r != NULL) {
                    r->SetVoiceCallForwardingFlag(1, msg.arg1 == 1, cfu.mSetCfNumber);
                    SetCallForwardingPreference(msg.arg1 == 1);
                }
                If (cfu.mOnComplete != NULL) {
                    AsyncResult->ForMessage(cfu.mOnComplete, ar.result, ar.exception);
                    cfu.mOnComplete->SendToTarget();
                }
                break;

            case EVENT_SET_VM_NUMBER_DONE:
                ar = (AsyncResult)msg.obj;
                If (IccVmNotSupportedException.class->IsInstance(ar.exception)) {
                    StoreVoiceMailNumber(mVmNumber);
                    ar.exception = NULL;
                }
                onComplete = (Message) ar.userObj;
                If (onComplete != NULL) {
                    AsyncResult->ForMessage(onComplete, ar.result, ar.exception);
                    onComplete->SendToTarget();
                }
                break;


            case EVENT_GET_CALL_FORWARD_DONE:
                ar = (AsyncResult)msg.obj;
                If (ar.exception == NULL) {
                    HandleCfuQueryResult((CallForwardInfo[])ar.result);
                }
                onComplete = (Message) ar.userObj;
                If (onComplete != NULL) {
                    AsyncResult->ForMessage(onComplete, ar.result, ar.exception);
                    onComplete->SendToTarget();
                }
                break;

            case EVENT_SET_NETWORK_AUTOMATIC:
                // Automatic network selection from EF_CSP SIM record
                ar = (AsyncResult) msg.obj;
                If (mSST.mSS->GetIsManualSelection()) {
                    SetNetworkSelectionModeAutomatic((Message) ar.result);
                    Rlog->D(LOG_TAG, "SET_NETWORK_SELECTION_AUTOMATIC: set to automatic");
                } else {
                    // prevent duplicate request which will push current PLMN to low priority
                    Rlog->D(LOG_TAG, "SET_NETWORK_SELECTION_AUTOMATIC: already automatic, ignore");
                }
                break;

            case EVENT_ICC_RECORD_EVENTS:
                ar = (AsyncResult)msg.obj;
                ProcessIccRecordEvents((Integer)ar.result);
                break;

            case EVENT_SET_CLIR_COMPLETE:
                ar = (AsyncResult)msg.obj;
                If (ar.exception == NULL) {
                    SaveClirSetting(msg.arg1);
                }
                onComplete = (Message) ar.userObj;
                If (onComplete != NULL) {
                    AsyncResult->ForMessage(onComplete, ar.result, ar.exception);
                    onComplete->SendToTarget();
                }
                break;

            case EVENT_SUBSCRIPTION_ACTIVATED:
                Log("EVENT_SUBSCRIPTION_ACTIVATED");
                OnSubscriptionActivated();
                break;

            case EVENT_SUBSCRIPTION_DEACTIVATED:
                Log("EVENT_SUBSCRIPTION_DEACTIVATED");
                OnSubscriptionDeactivated();
                break;

            case EVENT_SS:
                ar = (AsyncResult)msg.obj;
                Rlog->D(LOG_TAG, "Event EVENT_SS received");
                // SS data is already being handled through MMI codes.
                // So, this result if processed as MMI response would help
                // in re-using the existing functionality.
                GsmMmiCode mmi = new GsmMmiCode(this, mUiccApplication->Get());
                mmi->ProcessSsData(ar);

            case CHECK_CALLFORWARDING_STATUS:
                Boolean cfEnabled = GetCallForwardingPreference();
                If (LOCAL_DEBUG) Rlog->D(LOG_TAG, "Callforwarding is " + cfEnabled);
                If (cfEnabled) {
                    NotifyCallForwardingIndicator();
                }
                break;

             default:
                 super->HandleMessage(msg);
        }
    }

    protected UiccCardApplication GetUiccCardApplication() {
            Return  ((UiccController) mUiccController).GetUiccCardApplication(mPhoneId,
                    UiccController.APP_FAM_3GPP);
    }

    // Set the Card into the Phone Book.
    //@Override
    protected void SetCardInPhoneBook() {
        If (mUiccController == NULL ) {
            return;
        }

        mSimPhoneBookIntManager->SetIccCard(mUiccController->GetUiccCard(mPhoneId));
    }

    //@Override
    protected void OnUpdateIccAvailability() {
        If (mUiccController == NULL ) {
            return;
        }

        // Get the latest info on the card and
        // send this to Phone Book
        SetCardInPhoneBook();

        UiccCardApplication newUiccApplication =
                mUiccController->GetUiccCardApplication(mPhoneId, UiccController.APP_FAM_IMS);
        IsimUiccRecords newIsimUiccRecords = NULL;

        If (newUiccApplication != NULL) {
            newIsimUiccRecords = (IsimUiccRecords)newUiccApplication->GetIccRecords();
            If (LOCAL_DEBUG) Log("New ISIM application found");
        }
        mIsimUiccRecords = newIsimUiccRecords;

        newUiccApplication = GetUiccCardApplication();

        UiccCardApplication app = mUiccApplication->Get();
        If (app != newUiccApplication) {
            If (app != NULL) {
                If (LOCAL_DEBUG) Log("Removing stale icc objects.");
                If (mIccRecords->Get() != NULL) {
                    UnregisterForSimRecordEvents();
                }
                mIccRecords->Set(NULL);
                mUiccApplication->Set(NULL);
            }
            If (newUiccApplication != NULL) {
                If (LOCAL_DEBUG) Log("New Uicc application found");
                mUiccApplication->Set(newUiccApplication);
                mIccRecords->Set(newUiccApplication->GetIccRecords());
                RegisterForSimRecordEvents();
            }
        }
    }

    private void ProcessIccRecordEvents(Int32 eventCode) {
        Switch (eventCode) {
            case IccRecords.EVENT_CFI:
                NotifyCallForwardingIndicator();
                break;
        }
    }

   /**
     * Sets the "current" field in the telephony provider according to the SIM's operator
     *
     * @return TRUE for success; FALSE otherwise.
     */
    public Boolean UpdateCurrentCarrierInProvider() {
        Int64 currentDds = SubscriptionManager->GetDefaultDataSubId();
        String operatorNumeric = GetOperatorNumeric();

        Log("updateCurrentCarrierInProvider: mSubId = " + GetSubId()
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

    /**
     * Saves CLIR setting so that we can re-apply it as necessary
     * (in case the RIL resets it across reboots).
     */
    CARAPI SaveClirSetting(Int32 commandInterfaceCLIRMode) {
        // open the shared preferences editor, and write the value.
        SharedPreferences sp = PreferenceManager->GetDefaultSharedPreferences(GetContext());
        SharedPreferences.Editor editor = sp->Edit();
        editor->PutInt(CLIR_KEY + GetPhoneId(), commandInterfaceCLIRMode);

        // commit and log the result.
        If (! editor->Commit()) {
            Rlog->E(LOG_TAG, "failed to commit CLIR preference");
        }
    }

    private void HandleCfuQueryResult(CallForwardInfo[] infos) {
        IccRecords r = mIccRecords->Get();
        If (r != NULL) {
            If (infos == NULL || infos.length == 0) {
                // Assume the default is not active
                // Set unconditional CFF in SIM to FALSE
                r->SetVoiceCallForwardingFlag(1, FALSE, NULL);
            } else {
                For (Int32 i = 0, s = infos.length; i < s; i++) {
                    If ((infos[i].serviceClass & SERVICE_CLASS_VOICE) != 0) {
                        SetCallForwardingPreference(infos[i].status == 1);
                        r->SetVoiceCallForwardingFlag(1, (infos[i].status == 1),
                            infos[i].number);
                        // should only have the one
                        break;
                    }
                }
            }
        }
    }

    /**
     * Retrieves the PhoneSubInfo of the GSMPhone
     */
    //@Override
    public PhoneSubInfo GetPhoneSubInfo(){
        return mSubInfo;
    }

    /**
     * Retrieves the IccPhoneBookInterfaceManager of the GSMPhone
     */
    //@Override
    public IccPhoneBookInterfaceManager GetIccPhoneBookInterfaceManager(){
        return mSimPhoneBookIntManager;
    }

    /**
     * Activate or deactivate cell broadcast SMS.
     *
     * @param activate 0 = activate, 1 = deactivate
     * @param response Callback message is empty on completion
     */
    //@Override
    CARAPI ActivateCellBroadcastSms(Int32 activate, Message response) {
        Rlog->E(LOG_TAG, "[GSMPhone] ActivateCellBroadcastSms() is obsolete; use SmsManager");
        response->SendToTarget();
    }

    /**
     * Query the current configuration of cdma cell broadcast SMS.
     *
     * @param response Callback message is empty on completion
     */
    //@Override
    CARAPI GetCellBroadcastSmsConfig(Message response) {
        Rlog->E(LOG_TAG, "[GSMPhone] GetCellBroadcastSmsConfig() is obsolete; use SmsManager");
        response->SendToTarget();
    }

    /**
     * Configure cdma cell broadcast SMS.
     *
     * @param response Callback message is empty on completion
     */
    //@Override
    CARAPI SetCellBroadcastSmsConfig(Int32[] configValuesArray, Message response) {
        Rlog->E(LOG_TAG, "[GSMPhone] SetCellBroadcastSmsConfig() is obsolete; use SmsManager");
        response->SendToTarget();
    }

    //@Override
    public Boolean IsCspPlmnEnabled() {
        IccRecords r = mIccRecords->Get();
        Return (r != NULL) ? r->IsCspPlmnEnabled() : FALSE;
    }

    public Boolean IsManualNetSelAllowed() {

        Int32 nwMode = Phone.PREFERRED_NT_MODE;

        nwMode = PhoneFactory->CalculatePreferredNetworkType(mContext, mPhoneId);

        Rlog->D(LOG_TAG, "isManualNetSelAllowed in mode = " + nwMode);
        /*
         *  For multimode targets in global mode manual network
         *  selection is disallowed
         */
        If (SystemProperties->GetBoolean(PhoneBase.PROPERTY_MULTIMODE_CDMA, FALSE)
                && ((nwMode == Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA)
                        || (nwMode == Phone.NT_MODE_GLOBAL)) ){
            Rlog->D(LOG_TAG, "Manual selection not supported in mode = " + nwMode);
            return FALSE;
        }

        /*
         *  Single mode phone with - GSM network modes/global mode
         *  LTE only for 3GPP
         *  LTE centric + 3GPP Legacy
         *  Note: the actual enabling/disabling manual selection for these
         *  cases will be controlled by csp
         */
        return TRUE;
    }

    private void RegisterForSimRecordEvents() {
        IccRecords r = mIccRecords->Get();
        If (r == NULL) {
            return;
        }
        r->RegisterForNetworkSelectionModeAutomatic(
                this, EVENT_SET_NETWORK_AUTOMATIC, NULL);
        r->RegisterForRecordsEvents(this, EVENT_ICC_RECORD_EVENTS, NULL);
        r->RegisterForRecordsLoaded(this, EVENT_SIM_RECORDS_LOADED, NULL);
    }

    private void UnregisterForSimRecordEvents() {
        IccRecords r = mIccRecords->Get();
        If (r == NULL) {
            return;
        }
        r->UnregisterForNetworkSelectionModeAutomatic(this);
        r->UnregisterForRecordsEvents(this);
        r->UnregisterForRecordsLoaded(this);
    }

    //@Override
    CARAPI ExitEmergencyCallbackMode() {
        If (mImsPhone != NULL) {
            mImsPhone->ExitEmergencyCallbackMode();
        }
    }

    //@Override
    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("GSMPhone extends:");
        super->Dump(fd, pw, args);
        pw->Println(" mCT=" + mCT);
        pw->Println(" mSST=" + mSST);
        pw->Println(" mPendingMMIs=" + mPendingMMIs);
        pw->Println(" mSimPhoneBookIntManager=" + mSimPhoneBookIntManager);
        pw->Println(" mSubInfo=" + mSubInfo);
        If (VDBG) pw->Println(" mImei=" + mImei);
        If (VDBG) pw->Println(" mImeiSv=" + mImeiSv);
        pw->Println(" mVmNumber=" + mVmNumber);
    }

    //@Override
    public Boolean SetOperatorBrandOverride(String brand) {
        If (mUiccController == NULL) {
            return FALSE;
        }

        UiccCard card = mUiccController->GetUiccCard();
        If (card == NULL) {
            return FALSE;
        }

        Boolean status = card->SetOperatorBrandOverride(brand);

        // Refresh.
        If (status) {
            IccRecords iccRecords = mIccRecords->Get();
            If (iccRecords != NULL) {
                SystemProperties->Set(TelephonyProperties.PROPERTY_ICC_OPERATOR_ALPHA,
                        iccRecords->GetServiceProviderName());
            }
            If (mSST != NULL) {
                mSST->PollState();
            }
        }
        return status;
    }

    /**
     * @return operator numeric.
     */
    public String GetOperatorNumeric() {
        String operatorNumeric = NULL;
        IccRecords r = mIccRecords->Get();
        If (r != NULL) {
            operatorNumeric = r->GetOperatorNumeric();
        }
        return operatorNumeric;
    }

    CARAPI RegisterForAllDataDisconnected(Handler h, Int32 what, Object obj) {
        ((DcTracker)mDcTracker)
                .RegisterForAllDataDisconnected(h, what, obj);
    }

    CARAPI UnregisterForAllDataDisconnected(Handler h) {
        ((DcTracker)mDcTracker).UnregisterForAllDataDisconnected(h);
    }

    CARAPI SetInternalDataEnabled(Boolean enable, Message onCompleteMsg) {
        ((DcTracker)mDcTracker)
                .SetInternalDataEnabled(enable, onCompleteMsg);
    }


    public Boolean SetInternalDataEnabledFlag(Boolean enable) {
        Return ((DcTracker)mDcTracker)
                .SetInternalDataEnabledFlag(enable);
    }

    CARAPI NotifyEcbmTimerReset(Boolean flag) {
        mEcmTimerResetRegistrants->NotifyResult(flag);
    }

    /**
     * Registration point for Ecm timer reset
     *
     * @param h handler to notify
     * @param what User-defined message code
     * @param obj placed in Message.obj
     */
    CARAPI RegisterForEcmTimerReset(Handler h, Int32 what, Object obj) {
        mEcmTimerResetRegistrants->AddUnique(h, what, obj);
    }

    CARAPI UnregisterForEcmTimerReset(Handler h) {
        mEcmTimerResetRegistrants->Remove(h);
    }

    CARAPI ResetSubSpecifics() {
    }

    /** gets the voice mail count from preferences */
    private Int32 GetStoredVoiceMessageCount() {
        Int32 countVoiceMessages = 0;
        SharedPreferences sp = PreferenceManager->GetDefaultSharedPreferences(mContext);
        String imsi = sp->GetString(VM_ID + GetSubId(), NULL);
        String currentImsi = GetSubscriberId();

        Rlog->D(LOG_TAG, "Voicemail count retrieval for Imsi = " + imsi +
                " current Imsi = " + currentImsi );

        If ((imsi != NULL) && (currentImsi != NULL)
                && (currentImsi->Equals(imsi))) {
            // get voice mail count from preferences
            countVoiceMessages = sp->GetInt(VM_COUNT + GetSubId(), 0);
            Rlog->D(LOG_TAG, "Voice Mail Count from preference = " + countVoiceMessages );
        }
        return countVoiceMessages;
    }

     /**
     * Sets the SIM voice message waiting indicator records.
     * @param line GSM Subscriber Profile Number, one-based. Only '1' is supported
     * @param countWaiting The number of messages waiting, if known. Use
     *                     -1 to indicate that an unknown number of
     *                      messages are waiting
     */
    //@Override
    CARAPI SetVoiceMessageWaiting(Int32 line, Int32 countWaiting) {
        IccRecords r = mIccRecords->Get();
        If (r != NULL) {
            r->SetVoiceMessageWaiting(line, countWaiting);
        } else {
            Log("SIM Records not found, MWI not updated");
        }
    }

    protected void Log(String s) {
        Rlog->D(LOG_TAG, "[GSMPhone] " + s);
    }

    private Boolean IsValidFacilityString(String facility) {
        If ((facility->Equals(CommandsInterface.CB_FACILITY_BAOC))
                || (facility->Equals(CommandsInterface.CB_FACILITY_BAOIC))
                || (facility->Equals(CommandsInterface.CB_FACILITY_BAOICxH))
                || (facility->Equals(CommandsInterface.CB_FACILITY_BAIC))
                || (facility->Equals(CommandsInterface.CB_FACILITY_BAICr))
                || (facility->Equals(CommandsInterface.CB_FACILITY_BA_ALL))
                || (facility->Equals(CommandsInterface.CB_FACILITY_BA_MO))
                || (facility->Equals(CommandsInterface.CB_FACILITY_BA_MT))
                || (facility->Equals(CommandsInterface.CB_FACILITY_BA_SIM))
                || (facility->Equals(CommandsInterface.CB_FACILITY_BA_FD))) {
            return TRUE;
        }
        Rlog->E(LOG_TAG, " Invalid facility String : "+facility);
        return FALSE;
    }

    CARAPI GetCallBarringOption(String facility, String password, Message onComplete) {
        If (IsValidFacilityString(facility)) {
            mCi->QueryFacilityLock(facility, password, CommandsInterface.SERVICE_CLASS_NONE,
                    onComplete);
        }
    }

    CARAPI SetCallBarringOption(String facility, Boolean lockState, String password,
            Message onComplete) {
        If (IsValidFacilityString(facility)) {
            mCi->SetFacilityLock(facility, lockState, password,
                    CommandsInterface.SERVICE_CLASS_VOICE, onComplete);
        }
    }

    CARAPI RequestChangeCbPsw(String facility, String oldPwd, String newPwd, Message result) {
        mCi->ChangeBarringPassword(facility, oldPwd, newPwd, result);
    }
}
