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

package com.android.internal.telephony.imsphone;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Telephony::ICellInfo;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDataConnection;
using Elastos::Droid::Internal::Telephony::Cdma::ICDMAPhone;
using Elastos::Droid::Internal::Telephony::Gsm::IGSMPhone;
using Elastos::Droid::Internal::Telephony::ICallManager;
using Elastos::Droid::Internal::Telephony::IIccCard;
using Elastos::Droid::Internal::Telephony::IIccPhoneBookInterfaceManager;
using Elastos::Droid::Internal::Telephony::IMmiCode;
using Elastos::Droid::Internal::Telephony::IOperatorInfo;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IPhoneNotifier;
using Elastos::Droid::Internal::Telephony::IPhoneSubInfo;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Internal::Telephony::IUUSInfo;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

abstract class ImsPhoneBase extends PhoneBase {
    private static const String LOG_TAG = "ImsPhoneBase";

    private RegistrantList mRingbackRegistrants = new RegistrantList();
    private RegistrantList mOnHoldRegistrants = new RegistrantList();
    private PhoneConstants.State mState = PhoneConstants.State.IDLE;

    public ImsPhoneBase(String name, Context context, PhoneNotifier notifier) {
        Super(name, notifier, context, new ImsPhoneCommandInterface(context), FALSE);
    }

    //@Override
    public Connection Dial(String dialString, UUSInfo uusInfo, Int32 videoState)
            throws CallStateException {
        // ignore UUSInfo
        return Dial(dialString, videoState);
    }

    //@Override
    CARAPI MigrateFrom(PhoneBase from) {
        super->MigrateFrom(from);
        Migrate(mRingbackRegistrants, ((ImsPhoneBase)from).mRingbackRegistrants);
    }

    //@Override
    CARAPI RegisterForRingbackTone(Handler h, Int32 what, Object obj) {
        mRingbackRegistrants->AddUnique(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForRingbackTone(Handler h) {
        mRingbackRegistrants->Remove(h);
    }

    protected void StartRingbackTone() {
        AsyncResult result = new AsyncResult(NULL, Boolean.TRUE, NULL);
        mRingbackRegistrants->NotifyRegistrants(result);
    }

    protected void StopRingbackTone() {
        AsyncResult result = new AsyncResult(NULL, Boolean.FALSE, NULL);
        mRingbackRegistrants->NotifyRegistrants(result);
    }

    //@Override
    CARAPI RegisterForOnHoldTone(Handler h, Int32 what, Object obj) {
        mOnHoldRegistrants->AddUnique(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForOnHoldTone(Handler h) {
        mOnHoldRegistrants->Remove(h);
    }

    protected void StartOnHoldTone() {
        AsyncResult result = new AsyncResult(NULL, Boolean.TRUE, NULL);
        mOnHoldRegistrants->NotifyRegistrants(result);
    }

    protected void StopOnHoldTone() {
        AsyncResult result = new AsyncResult(NULL, Boolean.FALSE, NULL);
        mOnHoldRegistrants->NotifyRegistrants(result);
    }

    //@Override
    public ServiceState GetServiceState() {
        // FIXME: we may need to provide this when data connectivity is lost
        // or when server is down
        ServiceState s = new ServiceState();
        s->SetState(ServiceState.STATE_IN_SERVICE);
        return s;
    }

    /**
     * @return all available cell information or NULL if none.
     */
    //@Override
    public List<CellInfo> GetAllCellInfo() {
        return GetServiceStateTracker()->GetAllCellInfo();
    }

    //@Override
    public CellLocation GetCellLocation() {
        return NULL;
    }

    //@Override
    public PhoneConstants.State GetState() {
        return mState;
    }

    //@Override
    public Int32 GetPhoneType() {
        return PhoneConstants.PHONE_TYPE_IMS;
    }

    //@Override
    public SignalStrength GetSignalStrength() {
        return new SignalStrength();
    }

    //@Override
    public Boolean GetMessageWaitingIndicator() {
        return FALSE;
    }

    //@Override
    public Boolean GetCallForwardingIndicator() {
        return FALSE;
    }

    //@Override
    public List<? extends MmiCode> GetPendingMmiCodes() {
        return new ArrayList<MmiCode>(0);
    }

    //@Override
    public PhoneConstants.DataState GetDataConnectionState() {
        return PhoneConstants.DataState.DISCONNECTED;
    }

    //@Override
    public PhoneConstants.DataState GetDataConnectionState(String apnType) {
        return PhoneConstants.DataState.DISCONNECTED;
    }

    //@Override
    public DataActivityState GetDataActivityState() {
        return DataActivityState.NONE;
    }

    /**
     * Notify any interested party of a Phone state change
     * {@link com.android.internal.telephony.PhoneConstants.State}
     */
    /* package */ void NotifyPhoneStateChanged() {
        mNotifier->NotifyPhoneState(this);
    }

    /**
     * Notify registrants of a change in the call state. This notifies changes in
     * {@link com.android.internal.telephony.Call.State}. Use this when changes
     * in the precise call state are needed, else use notifyPhoneStateChanged.
     */
    /* package */ void NotifyPreciseCallStateChanged() {
        /* we'd love it if this was package-scoped*/
        super->NotifyPreciseCallStateChangedP();
    }

    void NotifyDisconnect(Connection cn) {
        mDisconnectRegistrants->NotifyResult(cn);
    }

    void NotifyUnknownConnection() {
        mUnknownConnectionRegistrants->NotifyResult(this);
    }

    void NotifySuppServiceFailed(SuppService code) {
        mSuppServiceFailedRegistrants->NotifyResult(code);
    }

    void NotifyServiceStateChanged(ServiceState ss) {
        super->NotifyServiceStateChangedP(ss);
    }

    //@Override
    CARAPI NotifyCallForwardingIndicator() {
        mNotifier->NotifyCallForwardingChanged(this);
    }

    public Boolean CanDial() {
        Int32 serviceState = GetServiceState()->GetState();
        Rlog->V(LOG_TAG, "CanDial(): serviceState = " + serviceState);
        If (serviceState == ServiceState.STATE_POWER_OFF) return FALSE;

        String disableCall = SystemProperties->Get(
                TelephonyProperties.PROPERTY_DISABLE_CALL, "FALSE");
        Rlog->V(LOG_TAG, "CanDial(): disableCall = " + disableCall);
        If (disableCall->Equals("TRUE")) return FALSE;

        Rlog->V(LOG_TAG, "CanDial(): ringingCall: " + GetRingingCall()->GetState());
        Rlog->V(LOG_TAG, "CanDial(): foregndCall: " + GetForegroundCall()->GetState());
        Rlog->V(LOG_TAG, "CanDial(): backgndCall: " + GetBackgroundCall()->GetState());
        return !GetRingingCall()->IsRinging()
                && (!GetForegroundCall()->GetState().IsAlive()
                    || !GetBackgroundCall()->GetState().IsAlive());
    }

    //@Override
    public Boolean HandleInCallMmiCommands(String dialString) {
        return FALSE;
    }

    Boolean IsInCall() {
        Call.State foregroundCallState = GetForegroundCall()->GetState();
        Call.State backgroundCallState = GetBackgroundCall()->GetState();
        Call.State ringingCallState = GetRingingCall()->GetState();

       Return (foregroundCallState->IsAlive() || backgroundCallState->IsAlive()
               || ringingCallState->IsAlive());
    }

    //@Override
    public Boolean HandlePinMmi(String dialString) {
        return FALSE;
    }

    //@Override
    CARAPI SendUssdResponse(String ussdMessge) {
    }

    //@Override
    CARAPI RegisterForSuppServiceNotification(
            Handler h, Int32 what, Object obj) {
    }

    //@Override
    CARAPI UnregisterForSuppServiceNotification(Handler h) {
    }

    //@Override
    CARAPI SetRadioPower(Boolean power) {
    }

    //@Override
    public String GetVoiceMailNumber() {
        return NULL;
    }

    //@Override
    public String GetVoiceMailAlphaTag() {
        return NULL;
    }

    //@Override
    public String GetDeviceId() {
        return NULL;
    }

    //@Override
    public String GetDeviceSvn() {
        return NULL;
    }

    //@Override
    public String GetImei() {
        return NULL;
    }

    //@Override
    public String GetEsn() {
        Rlog->E(LOG_TAG, "[VoltePhone] GetEsn() is a CDMA method");
        return "0";
    }

    //@Override
    public String GetMeid() {
        Rlog->E(LOG_TAG, "[VoltePhone] GetMeid() is a CDMA method");
        return "0";
    }

    //@Override
    public String GetSubscriberId() {
        return NULL;
    }

    //@Override
    public String GetGroupIdLevel1() {
        return NULL;
    }

    //@Override
    public String GetIccSerialNumber() {
        return NULL;
    }

    //@Override
    public String GetLine1Number() {
        return NULL;
    }

    //@Override
    public String GetLine1AlphaTag() {
        return NULL;
    }

    //@Override
    CARAPI SetLine1Number(String alphaTag, String number, Message onComplete) {
        // FIXME: what to reply for Volte?
        AsyncResult->ForMessage(onComplete, NULL, NULL);
        onComplete->SendToTarget();
    }

    //@Override
    CARAPI SetVoiceMailNumber(String alphaTag, String voiceMailNumber,
            Message onComplete) {
        // FIXME: what to reply for Volte?
        AsyncResult->ForMessage(onComplete, NULL, NULL);
        onComplete->SendToTarget();
    }

    //@Override
    CARAPI GetCallForwardingOption(Int32 commandInterfaceCFReason, Message onComplete) {
    }

    //@Override
    CARAPI SetCallForwardingOption(Int32 commandInterfaceCFAction,
            Int32 commandInterfaceCFReason, String dialingNumber,
            Int32 timerSeconds, Message onComplete) {
    }

    //@Override
    CARAPI SetCallForwardingUncondTimerOption(Int32 startHour, Int32 startMinute,
            Int32 endHour, Int32 endMinute, Int32 commandInterfaceCFAction,
            Int32 commandInterfaceCFReason, String dialingNumber, Message onComplete) {
    }

    //@Override
    CARAPI GetOutgoingCallerIdDisplay(Message onComplete) {
        // FIXME: what to reply?
        AsyncResult->ForMessage(onComplete, NULL, NULL);
        onComplete->SendToTarget();
    }

    //@Override
    CARAPI SetOutgoingCallerIdDisplay(Int32 commandInterfaceCLIRMode,
            Message onComplete) {
        // FIXME: what's this for Volte?
        AsyncResult->ForMessage(onComplete, NULL, NULL);
        onComplete->SendToTarget();
    }

    //@Override
    CARAPI GetCallWaiting(Message onComplete) {
        AsyncResult->ForMessage(onComplete, NULL, NULL);
        onComplete->SendToTarget();
    }

    //@Override
    CARAPI SetCallWaiting(Boolean enable, Message onComplete) {
        Rlog->E(LOG_TAG, "call waiting not supported");
    }

    //@Override
    public Boolean GetIccRecordsLoaded() {
        return FALSE;
    }

    //@Override
    public IccCard GetIccCard() {
        return NULL;
    }

    //@Override
    CARAPI GetAvailableNetworks(Message response) {
    }

    //@Override
    CARAPI SetNetworkSelectionModeAutomatic(Message response) {
    }

    //@Override
    CARAPI SelectNetworkManually(
            OperatorInfo network,
            Message response) {
    }

    //@Override
    CARAPI GetNeighboringCids(Message response) {
    }

    //@Override
    CARAPI GetDataCallList(Message response) {
    }

    public List<DataConnection> GetCurrentDataConnectionList () {
        return NULL;
    }

    //@Override
    CARAPI UpdateServiceLocation() {
    }

    //@Override
    CARAPI EnableLocationUpdates() {
    }

    //@Override
    CARAPI DisableLocationUpdates() {
    }

    //@Override
    public Boolean GetDataRoamingEnabled() {
        return FALSE;
    }

    //@Override
    CARAPI SetDataRoamingEnabled(Boolean enable) {
    }

    //@Override
    public Boolean GetDataEnabled() {
        return FALSE;
    }

    //@Override
    CARAPI SetDataEnabled(Boolean enable) {
    }


    public Boolean EnableDataConnectivity() {
        return FALSE;
    }

    public Boolean DisableDataConnectivity() {
        return FALSE;
    }

    //@Override
    public Boolean IsDataConnectivityPossible() {
        return FALSE;
    }

    Boolean UpdateCurrentCarrierInProvider() {
        return FALSE;
    }

    CARAPI SaveClirSetting(Int32 commandInterfaceCLIRMode) {
    }

    //@Override
    public PhoneSubInfo GetPhoneSubInfo(){
        return NULL;
    }

    //@Override
    public IccPhoneBookInterfaceManager GetIccPhoneBookInterfaceManager(){
        return NULL;
    }

    //@Override
    public IccFileHandler GetIccFileHandler(){
        return NULL;
    }

    //@Override
    CARAPI ActivateCellBroadcastSms(Int32 activate, Message response) {
        Rlog->E(LOG_TAG, "Error! This functionality is not implemented for Volte.");
    }

    //@Override
    CARAPI GetCellBroadcastSmsConfig(Message response) {
        Rlog->E(LOG_TAG, "Error! This functionality is not implemented for Volte.");
    }

    //@Override
    CARAPI SetCellBroadcastSmsConfig(Int32[] configValuesArray, Message response){
        Rlog->E(LOG_TAG, "Error! This functionality is not implemented for Volte.");
    }

    //@Override
    //@Override
    public Boolean NeedsOtaServiceProvisioning() {
        // FIXME: what's this for Volte?
        return FALSE;
    }

    //@Override
    //@Override
    public LinkProperties GetLinkProperties(String apnType) {
        // FIXME: what's this for Volte?
        return NULL;
    }

    //@Override
    protected void OnUpdateIccAvailability() {
    }

    void UpdatePhoneState() {
        PhoneConstants.State oldState = mState;

        If (GetRingingCall()->IsRinging()) {
            mState = PhoneConstants.State.RINGING;
        } else If (GetForegroundCall()->IsIdle()
                && GetBackgroundCall()->IsIdle()) {
            mState = PhoneConstants.State.IDLE;
        } else {
            mState = PhoneConstants.State.OFFHOOK;
        }

        If (mState != oldState) {
            Rlog->D(LOG_TAG, " ^^^ new phone state: " + mState);
            NotifyPhoneStateChanged();
        }
    }
}
