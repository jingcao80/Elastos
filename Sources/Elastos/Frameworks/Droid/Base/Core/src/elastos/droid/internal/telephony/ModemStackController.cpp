/*
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

package com.android.internal.telephony;

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ISubInfoRecord;
using Elastos::Droid::Telephony::ITelephonyManager;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IRILConstants;
using Elastos::Droid::Internal::Telephony::Subscription::ISubscriptionStatus;
using Elastos::Droid::Internal::Telephony::ISubscriptionController;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::IRIL;
using Elastos::Droid::Internal::Telephony::Uicc::IIccUtils;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;

using Elastos::Nio::IByteBuffer;
using Elastos::Nio::IByteOrder;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;

/*
 * ModemStackController: Utility to provide the Stack capabilites and binding/unbinding
 * of Stacks based on client preference.
 * Includes:
 *    CurrentStackIds
 *    PreferredStackIds
 *    ModemCapabilities
 * Provides:
 *    Crossmapping (i.e. Binding/unbinding of stacks)
 *    RAT Capabilities of Stack for a sub.
 *    Current stack for a sub
 * Notifies:
 *    Stack Ready after update binding is completed to registrants.
 */

public class ModemStackController extends Handler {
    static const String LOG_TAG = "ModemStackController";

    //Utility class holding modem capabilities information per stack.
    public class ModemCapabilityInfo {
        private Int32 mStackId;
        //bit vector mask of all suported RATs
        private Int32 mSupportedRatBitMask;
        private Int32 mVoiceDataCap;
        private Int32 mMaxDataCap;

        public ModemCapabilityInfo(Int32 stackId, Int32 supportedRatBitMask,
                Int32 voiceCap, Int32 dataCap) {
            mStackId = stackId;
            mSupportedRatBitMask = supportedRatBitMask;
            mVoiceDataCap = voiceCap;
            mMaxDataCap = dataCap;
        }

        public Int32 GetSupportedRatBitMask() { return mSupportedRatBitMask;}

        public Int32 GetStackId() { return mStackId;}
        public Int32 GetMaxDataCap() { return mMaxDataCap;}

        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "[stack = " + mStackId + ", SuppRatBitMask = "
                + mSupportedRatBitMask + ", voiceDataCap = "
                + mVoiceDataCap + ", maxDataCap = "
                + mMaxDataCap +"]";
        }
    };

    //***** Events
    private static const Int32 CMD_DEACTIVATE_ALL_SUBS = 1;
    private static const Int32 EVENT_GET_MODEM_CAPS_DONE = 2;
    private static const Int32 CMD_TRIGGER_UNBIND = 3;
    private static const Int32 EVENT_UNBIND_DONE = 4;
    private static const Int32 CMD_TRIGGER_BIND = 5;
    private static const Int32 EVENT_BIND_DONE = 6;
    private static const Int32 EVENT_SET_PREF_MODE_DONE = 7;
    private  static const Int32 EVENT_SUB_DEACTIVATED = 8;
    private static const Int32 EVENT_RADIO_AVAILABLE = 9;
    private static const Int32 EVENT_MODEM_CAPABILITY_CHANGED = 10;

    //*****States
    private static const Int32 STATE_UNKNOWN = 1;
    private static const Int32 STATE_GOT_MODEM_CAPS = 2;
    private static const Int32 STATE_SUB_DEACT = 3;
    private static const Int32 STATE_UNBIND = 4;
    private static const Int32 STATE_BIND = 5;
    private static const Int32 STATE_SUB_ACT = 6;
    private static const Int32 STATE_SET_PREF_MODE = 7;

    //*****Constants
    private static const Int32 BIND_TO_STACK = 1;
    private static const Int32 UNBIND_TO_STACK = 0;
    private static const Int32 GET_MODEM_CAPS_BUFFER_LEN = 7;
    private static const Int32 SUCCESS = 1;
    private static const Int32 FAILURE = 0;
    private static const Int32 PRIMARY_STACK_ID = 0;
    private static const Int32 DEFAULT_MAX_DATA_ALLOWED = 1;

    //***** Class Variables
    private static ModemStackController sModemStackController;
    private CommandsInterface[] mCi;
    private Context mContext;
    private Int32 mNumPhones = TelephonyManager->GetDefault()->GetPhoneCount();
    private Int32 mActiveSubCount = 0;
    private Int32 mDeactivedSubCount = 0;
    private Int32[] mPreferredStackId = new Int32[mNumPhones];
    private Int32[] mCurrentStackId = new Int32[mNumPhones];
    private Int32[] mPrefNwMode = new Int32[mNumPhones];
    private Int32[] mSubState = new Int32[mNumPhones];
    private Boolean mIsStackReady = FALSE;
    private Boolean mIsRecoveryInProgress = FALSE;
    private Boolean mIsPhoneInEcbmMode = FALSE;
    private Boolean mModemRatCapabilitiesAvailable = FALSE;
    private Boolean mDeactivationInProgress = FALSE;
    private Boolean[] mCmdFailed = new Boolean[mNumPhones];
    private RegistrantList mStackReadyRegistrants = new RegistrantList();
    private RegistrantList mModemRatCapsAvailableRegistrants = new RegistrantList();
    private RegistrantList mModemDataCapsAvailableRegistrants = new RegistrantList();
    private Message mUpdateStackMsg;
    private HashMap<Integer, Integer> mSubcriptionStatus = new HashMap<Integer, Integer>();

    //Modem capabilities as per StackId
    private ModemCapabilityInfo[] mModemCapInfo = NULL;

    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            If (TelephonyIntents.ACTION_EMERGENCY_CALLBACK_MODE_CHANGED.
                    Equals(intent->GetAction())) {
                If (intent->GetBooleanExtra(PhoneConstants.PHONE_IN_ECM_STATE, FALSE)) {
                    Logd("Device is in ECBM Mode");
                    mIsPhoneInEcbmMode = TRUE;
                } else {
                    Logd("Device is out of ECBM Mode");
                    mIsPhoneInEcbmMode = FALSE;
                }
            } else If (TelephonyIntents.ACTION_SUBINFO_CONTENT_CHANGE->Equals(intent->GetAction())) {
                Int64 subId = intent->GetLongExtra(SubscriptionManager._ID,
                        SubscriptionManager.INVALID_SUB_ID);
                String column = intent->GetStringExtra(TelephonyIntents.EXTRA_COLUMN_NAME);
                Int32 intValue = intent->GetIntExtra(TelephonyIntents.EXTRA_INT_CONTENT, 0);
                Logd("Received ACTION_SUBINFO_CONTENT_CHANGE on subId: " + subId
                        + "for " + column + " intValue: " + intValue);

                If (mDeactivationInProgress && column != NULL
                        && column->Equals(SubscriptionManager.SUB_STATE)) {
                        Int32 phoneId = SubscriptionController->GetInstance()->GetPhoneId(subId);
                    If (intValue == SubscriptionManager.INACTIVE &&
                            mSubcriptionStatus->Get(phoneId)== SubscriptionManager.ACTIVE) {

                        //deactivated the activated sub
                        Message msg = ObtainMessage(EVENT_SUB_DEACTIVATED, new Integer(phoneId));
                        AsyncResult->ForMessage(msg, SubscriptionStatus.SUB_DEACTIVATED, NULL);
                        SendMessage(msg);
                    }
                }
            } else If (TelephonyIntents.ACTION_SUBSCRIPTION_SET_UICC_RESULT.
                    Equals(intent->GetAction())) {
                Int64 subId = intent->GetLongExtra(PhoneConstants.SUBSCRIPTION_KEY,
                        SubscriptionManager.INVALID_SUB_ID);
                Int32 phoneId = intent->GetIntExtra(PhoneConstants.PHONE_KEY,
                        PhoneConstants.PHONE_ID1);
                Int32 status = intent->GetIntExtra(TelephonyIntents.EXTRA_RESULT,
                        PhoneConstants.FAILURE);
                Logd("Received ACTION_SUBSCRIPTION_SET_UICC_RESULT on subId: " + subId
                        + "phoneId " + phoneId + " status: " + status);
                If (mDeactivationInProgress && (status == PhoneConstants.FAILURE)) {
                    // Sub deactivation failed
                    Message msg = ObtainMessage(EVENT_SUB_DEACTIVATED, new Integer(phoneId));
                    AsyncResult->ForMessage(msg, SubscriptionStatus.SUB_ACTIVATED, NULL);
                    SendMessage(msg);
                }
            }
        }};


    //***** Class Methods
    public static ModemStackController Make(Context context, UiccController uiccMgr,
            CommandsInterface[] ci) {
        Rlog->D(LOG_TAG, "getInstance");
        If (sModemStackController == NULL) {
            sModemStackController = new ModemStackController(context, uiccMgr, ci);
        } else {
            throw new RuntimeException("ModemStackController->Make() should only be called once");
        }
        return sModemStackController;
    }

    public static ModemStackController GetInstance() {
        If (sModemStackController == NULL) {
            throw new RuntimeException("ModemStackController.getInstance called before Make()");
        }
        return sModemStackController;
    }

    //***** Constructor
    private ModemStackController(Context context, UiccController uiccManager,
            CommandsInterface[] ci) {
        Logd("Constructor - Enter");

        mCi = ci;
        mContext = context;
        mModemCapInfo = new ModemCapabilityInfo[mNumPhones];

        For (Int32 i = 0; i < mCi.length; i++) {
            mCi[i].RegisterForAvailable(this, EVENT_RADIO_AVAILABLE, new Integer(i));
            mCi[i].RegisterForModemCapEvent(this, EVENT_MODEM_CAPABILITY_CHANGED, NULL);
        }

        For (Int32 i = 0; i < mNumPhones; i++) {
            mPreferredStackId[i] = i;
            mCurrentStackId[i] = i;
            mSubState[i] = STATE_UNKNOWN;
            mCmdFailed[i] = FALSE;
        }

        // In case of Single Sim, Stack is by default ready
        If (mNumPhones == 1) mIsStackReady = TRUE;

        IntentFilter filter =
                new IntentFilter(TelephonyIntents.ACTION_EMERGENCY_CALLBACK_MODE_CHANGED);
        filter->AddAction(TelephonyIntents.ACTION_SUBINFO_CONTENT_CHANGE);
        filter->AddAction(TelephonyIntents.ACTION_SUBSCRIPTION_SET_UICC_RESULT);
        mContext->RegisterReceiver(mReceiver, filter);
        Logd("Constructor - Exit");
    }

    //@Override
    CARAPI HandleMessage(Message msg) {
        AsyncResult ar;
        Integer phoneId;

        Switch(msg.what) {
            case EVENT_RADIO_AVAILABLE:
                ar = (AsyncResult)msg.obj;
                phoneId = (Integer)ar.userObj;
                Logd("EVENT_RADIO_AVAILABLE");
                ProcessRadioAvailable(ar, phoneId);
                break;

            case EVENT_GET_MODEM_CAPS_DONE:
                ar = (AsyncResult)msg.obj;
                phoneId = (Integer)ar.userObj;
                Logd("EVENT_GET_MODEM_CAPS_DONE");
                OnGetModemCapabilityDone(ar, (Byte[])ar.result, phoneId);
                break;

            case EVENT_MODEM_CAPABILITY_CHANGED:
                ar = (AsyncResult)msg.obj;
                Logd("EVENT_MODEM_CAPABILITY_CHANGED ar =" + ar);
                OnUnsolModemCapabilityChanged(ar);
                break;

            case CMD_DEACTIVATE_ALL_SUBS:
                Logd("CMD_DEACTIVATE_ALL_SUBS");
                DeactivateAllSubscriptions();
                break;

            case EVENT_SUB_DEACTIVATED:
                ar = (AsyncResult)msg.obj;
                phoneId = (Integer)ar.userObj;
                Logd("EVENT_SUB_DEACTIVATED");
                OnSubDeactivated(ar, phoneId);
                break;

            case CMD_TRIGGER_UNBIND:
                phoneId = (Integer)msg.obj;
                Logd("CMD_TRIGGER_UNBIND");
                UnbindStackOnSub(phoneId);
                break;

            case EVENT_UNBIND_DONE:
                ar = (AsyncResult)msg.obj;
                phoneId = (Integer)ar.userObj;
                Logd("EVENT_UNBIND_DONE");
                OnUnbindComplete(ar, phoneId);
                break;

            case CMD_TRIGGER_BIND:
                phoneId = (Integer)msg.obj;
                Logd("CMD_TRIGGER_BIND");
                BindStackOnSub(phoneId);
                break;

            case EVENT_BIND_DONE:
                ar = (AsyncResult)msg.obj;
                phoneId = (Integer)ar.userObj;
                Logd("EVENT_BIND_DONE");
                OnBindComplete(ar, phoneId);
                break;

            case EVENT_SET_PREF_MODE_DONE:
                ar = (AsyncResult)msg.obj;
                phoneId = (Integer)ar.userObj;
                Logd("EVENT_SET_PREF_MODE_DONE");
                OnSetPrefNwModeDone(ar, phoneId);
                break;

            default:
                break;
        }

    }

    private void ProcessRadioAvailable(AsyncResult ar, Int32 phoneId) {
        Logd("processRadioAvailable on phoneId = " + phoneId);

        If (phoneId >= 0 && phoneId < mNumPhones) {
            Message getModemCapsMsg = Message->Obtain(this, EVENT_GET_MODEM_CAPS_DONE,
                    new Integer(phoneId));

            mCi[phoneId].GetModemCapability(getModemCapsMsg);

        } else {
            Loge("Invalid Index!!!");
        }
    }

    private void OnGetModemCapabilityDone(AsyncResult ar, Byte[] result, Int32 phoneId) {

        If (result == NULL && ar.exception instanceof CommandException) {
            Loge("onGetModemCapabilityDone: EXIT!, result NULL or Exception =" + ar.exception);
            //On Modem Packages which do not support GetModemCaps RIl will return exception
            //On such Modem packages notify stack is ready so that SUB Activation can continue.
            NotifyStackReady(FALSE);
            return;
        }

        Logd("onGetModemCapabilityDone on phoneId[" + phoneId + "] result = " + result);

        If (phoneId >= 0 && phoneId < mNumPhones) {
            //Parse response and store it.
            mSubState[phoneId] = STATE_GOT_MODEM_CAPS;
            ParseGetModemCapabilityResponse(result, phoneId);

            //Wait till we get Modem Capabilities on all subs
            If (AreAllSubsinSameState(STATE_GOT_MODEM_CAPS)) {
                NotifyModemRatCapabilitiesAvailable();
            }
        } else {
            Loge("Invalid Index!!!");
        }
    }

    private void OnUnsolModemCapabilityChanged(AsyncResult ar) {
        Logd("onUnsolModemCapabilityChanged");
        RIL.UnsolOemHookBuffer unsolOemHookBuffer = (RIL.UnsolOemHookBuffer)ar.result;

        If(unsolOemHookBuffer == NULL && ar.exception instanceof CommandException) {
            Loge("onUnsolModemCapabilityChanged: EXIT!, result NULL or Exception =" + ar.exception);
            return;

        }
        Byte[] data = (Byte[])unsolOemHookBuffer->GetUnsolOemHookBuffer();
        Int32 phoneId = unsolOemHookBuffer->GetRilInstance();

        Logd("onUnsolModemCapabilityChanged on phoneId = " + phoneId);

        ParseGetModemCapabilityResponse(data, phoneId);
        NotifyModemDataCapabilitiesAvailable();

    }

    private void OnSubDeactivated(AsyncResult ar, Int32 phoneId) {
        SubscriptionStatus subStatus = (SubscriptionStatus)ar.result;
        If (subStatus == NULL ||
                (subStatus != NULL && SubscriptionStatus.SUB_DEACTIVATED != subStatus)) {
            Loge("onSubDeactivated on phoneId[" + phoneId + "] Failed!!!");
            mCmdFailed[phoneId] = TRUE;
        }

        Logd("onSubDeactivated on phoneId[" + phoneId + "] subStatus = " + subStatus);

        //avoid duplicate entries
        If (mSubState[phoneId] == STATE_SUB_DEACT) return;

        mSubState[phoneId] = STATE_SUB_DEACT;
        mDeactivedSubCount++;

        //Wait till we get Sub Deact response on all active subs
        If (mDeactivedSubCount == mActiveSubCount) {
            //if any deact failed notify registrants to activate any deactivated subs
            //and stop binding process. No need to recover here.
            If (IsAnyCmdFailed()) {
                NotifyStackReady(FALSE);
            } else {
                mDeactivationInProgress = FALSE;
                TriggerUnBindingOnAllSubs();
            }
        }
    }

    private void BindStackOnSub(Int32 phoneId) {
        Logd("bindStack " + mPreferredStackId[phoneId] + " On phoneId[" + phoneId + "]");
        Message msg = Message->Obtain(this, EVENT_BIND_DONE, new Integer(phoneId));
        mCi[phoneId].UpdateStackBinding(mPreferredStackId[phoneId], BIND_TO_STACK, msg);
    }

    private void UnbindStackOnSub(Int32 phoneId) {
        Logd("unbindStack " + mCurrentStackId[phoneId] + " On phoneId[" + phoneId + "]");
        Message msg = Message->Obtain(this, EVENT_UNBIND_DONE, new Integer(phoneId));
        mCi[phoneId].UpdateStackBinding(mCurrentStackId[phoneId], UNBIND_TO_STACK, msg);
    }

    private void OnUnbindComplete(AsyncResult ar, Int32 phoneId) {
        If (ar.exception instanceof CommandException) {
            mCmdFailed[phoneId] = TRUE;
            Loge("OnUnbindComplete(" + phoneId + "): got Exception =" + ar.exception);
        }

        mSubState[phoneId] = STATE_UNBIND;

        //Wait till we get UNBIND response on all subs
        If (AreAllSubsinSameState(STATE_UNBIND)) {
            If (IsAnyCmdFailed()) {
                RecoverToPrevState();
                return;
            }
            TriggerBindingOnAllSubs();
        }
    }

    private void OnBindComplete(AsyncResult ar, Int32 phoneId) {
        If (ar.exception instanceof CommandException) {
            mCmdFailed[phoneId] = TRUE;
            Loge("OnBindComplete(" + phoneId + "): got Exception =" + ar.exception);
        }

        mSubState[phoneId] = STATE_BIND;

        //Wait till we get BIND response on all subs
        If (AreAllSubsinSameState(STATE_BIND)) {
            If (IsAnyCmdFailed()) {
                RecoverToPrevState();
                return;
            }
            SetPrefNwTypeOnAllSubs();
        }
    }

    private void OnSetPrefNwModeDone(AsyncResult ar, Int32 phoneId) {
        If (ar.exception instanceof CommandException) {
            mCmdFailed[phoneId] = TRUE;
            Loge("OnSetPrefNwModeDone(SUB:" + phoneId + "): got Exception =" + ar.exception);
        }

        mSubState[phoneId] = STATE_SET_PREF_MODE;

        //Wait till we get Set Pref NwMode response on all subs
        If (AreAllSubsinSameState(STATE_SET_PREF_MODE)) {
            If (IsAnyCmdFailed()) {
                RecoverToPrevState();
                return;
            }

            If (mUpdateStackMsg != NULL) {
                SendResponseToTarget(mUpdateStackMsg, RILConstants.SUCCESS);
                mUpdateStackMsg = NULL;
            }
            UpdateNetworkSelectionMode();
            NotifyStackReady(TRUE);
        }
    }

    private void UpdateNetworkSelectionMode() {
        For (Int32 i = 0; i < mNumPhones; i++) {
            mCi[i].SetNetworkSelectionModeAutomatic(NULL);
        }
    }

    private void TriggerUnBindingOnAllSubs() {
        ResetSubStates();
        For (Int32 i = 0; i < mNumPhones; i++) {
            SendMessage(ObtainMessage(CMD_TRIGGER_UNBIND, new Integer(i)));
        }
    }

    private void TriggerBindingOnAllSubs() {
        ResetSubStates();
        For (Int32 i = 0; i < mNumPhones; i++) {
            SendMessage(ObtainMessage(CMD_TRIGGER_BIND, new Integer(i)));
        }
    }

    private void TriggerDeactivationOnAllSubs() {
        ResetSubStates();
        SendMessage(ObtainMessage(CMD_DEACTIVATE_ALL_SUBS));
    }


    private void SetPrefNwTypeOnAllSubs() {
        ResetSubStates();
        For (Int32 i = 0; i < mNumPhones; i++) {
            Message resp = ObtainMessage(EVENT_SET_PREF_MODE_DONE, new Integer(i));
            mCi[i].SetPreferredNetworkType(mPrefNwMode[i], resp);
        }
    }

    private Boolean AreAllSubsinSameState(Int32 state) {
        For (Int32 subState : mSubState) {
            Logd("areAllSubsinSameState state= "+state + " substate="+subState);
            If (subState != state) return FALSE;
        }
        return TRUE;
    }

    private void ResetSubStates() {
        For (Int32 i = 0; i < mNumPhones; i++) {
            mSubState[i] = STATE_UNKNOWN;
            mCmdFailed[i] = FALSE;
        }
    }

    private Boolean IsAnyCmdFailed() {
        Boolean result = FALSE;
        For (Int32 i = 0; i < mNumPhones; i++) {
            If (mCmdFailed[i] != FALSE) {
                result = TRUE;
            }
        }
        return result;
    }

    private void UpdateModemCapInfo(Int32 phoneId, Int32 stackId, Int32 supportedRatBitMask,
            Int32 voiceDataCap, Int32 maxDataCap) {

        mCurrentStackId[phoneId] = stackId;
        //Modem Capabilities are stored per StackId.
        mModemCapInfo[mCurrentStackId[phoneId]] = new ModemCapabilityInfo
                (mCurrentStackId[phoneId], supportedRatBitMask, voiceDataCap , maxDataCap);
        Logd("updateModemCapInfo: ModemCaps[" + phoneId +
                "]" + mModemCapInfo[mCurrentStackId[phoneId]]);


    }

    private void ParseGetModemCapabilityResponse(Byte[] result, Int32 phoneId) {
        Int32 supportedRatBitMask;
        Int32 stackId;
        Int32 voiceDataCap;
        Int32 maxDataCap;

        If (result.length != GET_MODEM_CAPS_BUFFER_LEN) {
            Loge("parseGetModemCapabilityResponse: EXIT!, result Length(" + result.length
                    + ") and Expected Length(" + GET_MODEM_CAPS_BUFFER_LEN + ") not matching.");
            return;
        }
        Logd("parseGetModemCapabilityResponse: buffer = "+IccUtils->BytesToHexString(result));
        ByteBuffer respBuffer = ByteBuffer->Wrap(result);
        respBuffer->Order(ByteOrder->NativeOrder());

        stackId = respBuffer->Get();

        If ( !(stackId >= 0 && stackId < mNumPhones) ) {
            Loge("Invalid Index!!!");
            return;
        }
        supportedRatBitMask = respBuffer->GetInt();
        voiceDataCap = respBuffer->Get();
        maxDataCap = respBuffer->Get();

        UpdateModemCapInfo(phoneId, stackId, supportedRatBitMask, voiceDataCap, maxDataCap);

    }

    private void SyncPreferredNwModeFromDB() {
        For (Int32 i = 0; i < mNumPhones; i++) {
            try {
                mPrefNwMode[i] = TelephonyManager->GetIntAtIndex(mContext->GetContentResolver(),
                        android.provider.Settings::Global::PREFERRED_NETWORK_MODE, i);
            } Catch (SettingNotFoundException snfe) {
                Loge("getPreferredNetworkMode: Could not find PREFERRED_NETWORK_MODE!!!");
                mPrefNwMode[i] = Phone.PREFERRED_NT_MODE;
            }
        }
    }

    private Boolean IsAnyCallsInProgress() {
        Boolean isCallInProgress = FALSE;
        For (Int32 i = 0; i < mNumPhones; i++) {
            If (TelephonyManager->GetDefault()->GetCallState(i)
                    != TelephonyManager.CALL_STATE_IDLE) {
                isCallInProgress = TRUE;
                break;
            }
        }
        return isCallInProgress;
    }

    public Boolean IsStackReady() {
        return mIsStackReady;
    }

    public Int32 GetMaxDataAllowed() {
        Logd("getMaxDataAllowed");
        Int32 ret = DEFAULT_MAX_DATA_ALLOWED;
        List<Integer> unsortedList = new ArrayList<Integer>();

        For (Int32 i = 0; i < mNumPhones; i++) {
            If (mModemCapInfo[i] != NULL) {
                unsortedList->Add(mModemCapInfo[i].GetMaxDataCap());
            }
        }
        Collections->Sort(unsortedList);
        Int32 listSize = unsortedList->Size();
        If (listSize > 0) {
            ret = unsortedList->Get(listSize - 1);
        }
        return ret;
    }

    public Int32 GetCurrentStackIdForPhoneId(Int32 phoneId) {
        return mCurrentStackId[phoneId];
    }

    public Int32 GetPrimarySub() {
        For (Int32 i = 0; i < mNumPhones; i++) {
            If (GetCurrentStackIdForPhoneId(i) == PRIMARY_STACK_ID) {
                return i;
            }
        }
        return 0;
    }

    public ModemCapabilityInfo GetModemRatCapsForPhoneId(Int32 phoneId) {
        return mModemCapInfo[mCurrentStackId[phoneId]];
    }

    public Int32 UpdateStackBinding(Int32[] prefStackIds, Boolean isBootUp, Message msg) {
        Boolean isUpdateRequired = FALSE;
        Boolean callInProgress = IsAnyCallsInProgress();

        If (mNumPhones == 1) {
            Loge("No need to update Stack Binding in case of Single Sim.");
            return FAILURE;
        }

        If (callInProgress || mIsPhoneInEcbmMode || (!mIsStackReady && !isBootUp)) {
            Loge("updateStackBinding: Calls is progress = " + callInProgress +
                    ", mIsPhoneInEcbmMode = " + mIsPhoneInEcbmMode + ", mIsStackReady = "
                    + mIsStackReady + ". So EXITING!!!");
            return FAILURE;
        }
        For (Int32 i = 0; i < mNumPhones; i++) {
            mPreferredStackId[i] = prefStackIds[i];
        }

        For (Int32 i = 0; i < mNumPhones; i++) {
            If (mPreferredStackId[i] != mCurrentStackId[i]) {
                //if preferred stackId is different from current, bindupdate is required.
                isUpdateRequired = TRUE;
                break;
            }
        }

        If (isUpdateRequired) {
            mIsStackReady = FALSE;
            //Store the msg object , so that result of updateStackbinding can be sent later.
            mUpdateStackMsg = msg;
            //Get Stored prefNwMode for all the subs and send request to RIL after update binding.
            SyncPreferredNwModeFromDB();
            If (isBootUp) {
                TriggerUnBindingOnAllSubs();
            } else {
                TriggerDeactivationOnAllSubs();
            }
        } else {
            //incase of bootup if cross binding is not required send stack ready notification.
            If (isBootUp) NotifyStackReady(FALSE);
            return FAILURE;
        }
        return SUCCESS;
    }

    private void DeactivateAllSubscriptions() {
        SubscriptionController subCtrlr = SubscriptionController->GetInstance();
        List<SubInfoRecord> subInfoList = subCtrlr->GetActiveSubInfoList();
        mActiveSubCount = 0;
        If (subInfoList == NULL) {
            //if getting sub info list is failed, abort cross mapping process.
            NotifyStackReady(FALSE);
            return;
        }
        For (SubInfoRecord subInfo : subInfoList) {
            Int32 subStatus = subCtrlr->GetSubState(subInfo.subId);
            If (subStatus == SubscriptionManager.ACTIVE) {
                mActiveSubCount++;
                subCtrlr->DeactivateSubId(subInfo.subId);
            }
            mSubcriptionStatus->Put(subInfo.slotId, subStatus);
        }
        If (mActiveSubCount > 0) {
            mDeactivedSubCount = 0;
            mDeactivationInProgress = TRUE;
        } else {
            mDeactivationInProgress = FALSE;
            TriggerUnBindingOnAllSubs();
        }
    }

    private void NotifyStackReady(Boolean isCrossMapDone) {
        Logd("notifyStackReady: Stack is READY!!!");
        mIsRecoveryInProgress = FALSE;
        mIsStackReady = TRUE;
        ResetSubStates();

        If (isCrossMapDone) {
            For (Int32 i = 0; i < mNumPhones; i++) {
                //update the current stackIds
                mCurrentStackId[i] = mPreferredStackId[i];
            }
        }
        //notify binding completed to all StackReady registrants.
        //including subscriptionManager which activates available subs on binding complete.
        mStackReadyRegistrants->NotifyRegistrants();
    }

    CARAPI RegisterForStackReady(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        If (mIsStackReady) {
            r->NotifyRegistrant();
        }
        Synchronized (mStackReadyRegistrants) {
            mStackReadyRegistrants->Add(r);
        }
    }

    private void NotifyModemRatCapabilitiesAvailable() {
           Logd("notifyGetRatCapabilitiesDone: Got RAT capabilities for all Stacks!!!");
           mModemRatCapabilitiesAvailable = TRUE;
           mModemRatCapsAvailableRegistrants->NotifyRegistrants();
    }

    private void NotifyModemDataCapabilitiesAvailable() {
           Logd("notifyGetDataCapabilitiesDone");
           mModemDataCapsAvailableRegistrants->NotifyRegistrants();
    }

    CARAPI RegisterForModemRatCapsAvailable(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        If (mModemRatCapabilitiesAvailable) {
            r->NotifyRegistrant();
        }
        Synchronized (mModemRatCapsAvailableRegistrants) {
            mModemRatCapsAvailableRegistrants->Add(r);
        }
    }

    CARAPI RegisterForModemDataCapsUpdate(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);

        Synchronized (mModemDataCapsAvailableRegistrants) {
            mModemDataCapsAvailableRegistrants->Add(r);
        }
    }

    private void RecoverToPrevState() {
        If (mIsRecoveryInProgress) {
            //Already in recovery process and exception occured again.
            //Get stored msg if available and send exception and stop recovery.
            If (mUpdateStackMsg != NULL) {
                SendResponseToTarget(mUpdateStackMsg, RILConstants.GENERIC_FAILURE);
                mUpdateStackMsg = NULL;
            }
            mIsRecoveryInProgress = FALSE;
            If(STATE_SET_PREF_MODE == mSubState[0]) {
                //Already recovery in progress, got failure in SetPrefNwmode. We are bailing out.
                //As Set Pref is failed, Binding is completed. so update and notify same.
                NotifyStackReady(TRUE);
            }
            return;
        }

        mIsRecoveryInProgress = TRUE;
        //Binding to Preferred Stack failed, recovery mode: fallback to Current stackIds
        For (Int32 i = 0; i < mNumPhones; i++) {
            mPreferredStackId[i] = mCurrentStackId[i];
        }
        TriggerUnBindingOnAllSubs();
    }

    private void SendResponseToTarget(Message response, Int32 responseCode) {
        Exception e = CommandException->FromRilErrno(responseCode);
        AsyncResult->ForMessage(response, NULL, e);
        response->SendToTarget();
    }

    private void Logd(String string) {
        Rlog->D(LOG_TAG, string);
    }

    private void Loge(String string) {
        Rlog->E(LOG_TAG, string);
    }
}
