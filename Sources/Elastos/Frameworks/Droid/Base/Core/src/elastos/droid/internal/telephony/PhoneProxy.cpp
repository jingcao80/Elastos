/*
 * Copyright (C) 2008 The Android Open Source Project
 * Copyright (c) 2013, The Linux Foundation. All rights reserved.
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


using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Telephony::ICellInfo;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Internal::Telephony::Cdma::ICDMAPhone;
using Elastos::Droid::Internal::Telephony::Gsm::IGSMPhone;
using Elastos::Droid::Internal::Telephony::Imsphone::IImsPhone;
using Elastos::Droid::Internal::Telephony::Test::ISimulatedRadioControl;
using Elastos::Droid::Internal::Telephony::Cdma::ICDMALTEPhone;
using Elastos::Droid::Internal::Telephony::Uicc::IIccCardProxy;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Internal::Telephony::Uicc::IIsimRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUsimServiceTable;

using Elastos::Utility::IList;

public class PhoneProxy extends Handler implements Phone {
    public static const Object lockForRadioTechnologyChange = new Object();

    private Phone mActivePhone;
    private CommandsInterface mCommandsInterface;
    private IccSmsInterfaceManager mIccSmsInterfaceManager;
    private IccPhoneBookInterfaceManagerProxy mIccPhoneBookInterfaceManagerProxy;
    private PhoneSubInfoProxy mPhoneSubInfoProxy;
    private IccCardProxy mIccCardProxy;

    private Boolean mResetModemOnRadioTechnologyChange = FALSE;

    private Int32 mRilVersion;

    private static const Int32 EVENT_VOICE_RADIO_TECH_CHANGED = 1;
    private static const Int32 EVENT_RADIO_ON = 2;
    private static const Int32 EVENT_REQUEST_VOICE_RADIO_TECH_DONE = 3;
    private static const Int32 EVENT_RIL_CONNECTED = 4;
    private static const Int32 EVENT_UPDATE_PHONE_OBJECT = 5;
    private static const Int32 EVENT_SIM_RECORDS_LOADED = 6;

    private Int32 mPhoneId = 0;

    private static const String LOG_TAG = "PhoneProxy";

    //***** Class Methods
    public PhoneProxy(PhoneBase phone) {
        mActivePhone = phone;
        mResetModemOnRadioTechnologyChange = SystemProperties->GetBoolean(
                TelephonyProperties.PROPERTY_RESET_ON_RADIO_TECH_CHANGE, FALSE);
        mIccPhoneBookInterfaceManagerProxy = new IccPhoneBookInterfaceManagerProxy(
                phone->GetIccPhoneBookInterfaceManager());
        mPhoneSubInfoProxy = new PhoneSubInfoProxy(phone->GetPhoneSubInfo());
        mCommandsInterface = ((PhoneBase)mActivePhone).mCi;

        mCommandsInterface->RegisterForRilConnected(this, EVENT_RIL_CONNECTED, NULL);
        mCommandsInterface->RegisterForOn(this, EVENT_RADIO_ON, NULL);
        mCommandsInterface->RegisterForVoiceRadioTechChanged(
                             this, EVENT_VOICE_RADIO_TECH_CHANGED, NULL);
        mPhoneId = phone->GetPhoneId();
        mIccSmsInterfaceManager =
                new IccSmsInterfaceManager((PhoneBase)this.mActivePhone);
        mIccCardProxy = new IccCardProxy(mActivePhone->GetContext(),
                mCommandsInterface, mActivePhone->GetPhoneId());

        If (phone->GetPhoneType() == PhoneConstants.PHONE_TYPE_GSM) {
            // For the purpose of IccCardProxy we only care about the technology family
            mIccCardProxy->SetVoiceRadioTech(ServiceState.RIL_RADIO_TECHNOLOGY_UMTS);
        } else If (phone->GetPhoneType() == PhoneConstants.PHONE_TYPE_CDMA) {
            mIccCardProxy->SetVoiceRadioTech(ServiceState.RIL_RADIO_TECHNOLOGY_1xRTT);
        }
    }

    //@Override
    CARAPI HandleMessage(Message msg) {
        AsyncResult ar = (AsyncResult) msg.obj;
        Switch(msg.what) {
        case EVENT_RADIO_ON:
            /* Proactively query voice radio technologies */
            mCommandsInterface->GetVoiceRadioTechnology(
                    ObtainMessage(EVENT_REQUEST_VOICE_RADIO_TECH_DONE));
            break;

        case EVENT_RIL_CONNECTED:
            If (ar.exception == NULL && ar.result != NULL) {
                mRilVersion = (Integer) ar.result;
            } else {
                Logd("Unexpected exception on EVENT_RIL_CONNECTED");
                mRilVersion = -1;
            }
            break;

        case EVENT_VOICE_RADIO_TECH_CHANGED:
        case EVENT_REQUEST_VOICE_RADIO_TECH_DONE:
            String what = (msg.what == EVENT_VOICE_RADIO_TECH_CHANGED) ?
                    "EVENT_VOICE_RADIO_TECH_CHANGED" : "EVENT_REQUEST_VOICE_RADIO_TECH_DONE";
            If (ar.exception == NULL) {
                If ((ar.result != NULL) && (((Int32[]) ar.result).length != 0)) {
                    Int32 newVoiceTech = ((Int32[]) ar.result)[0];
                    Logd(what + ": newVoiceTech=" + newVoiceTech);
                    PhoneObjectUpdater(newVoiceTech);
                } else {
                    Loge(what + ": has no tech!");
                }
            } else {
                Loge(what + ": exception=" + ar.exception);
            }
            break;

        case EVENT_UPDATE_PHONE_OBJECT:
            PhoneObjectUpdater(msg.arg1);
            break;

        case EVENT_SIM_RECORDS_LOADED:
            // Only check for the voice radio tech if it not going to be updated by the voice
            // registration changes.
            If (!mActivePhone->GetContext()->GetResources().GetBoolean(
                    R.bool.config_switch_phone_on_voice_reg_state_change)) {
                mCommandsInterface->GetVoiceRadioTechnology(ObtainMessage(
                        EVENT_REQUEST_VOICE_RADIO_TECH_DONE));
            }
            break;

        default:
            Loge("Error! This handler was not registered for this message type. Message: "
                    + msg.what);
            break;
        }
        super->HandleMessage(msg);
    }

    private static void Logd(String msg) {
        Rlog->D(LOG_TAG, "[PhoneProxy] " + msg);
    }

    private void Loge(String msg) {
        Rlog->E(LOG_TAG, "[PhoneProxy] " + msg);
    }

    private void PhoneObjectUpdater(Int32 newVoiceRadioTech) {
        Logd("phoneObjectUpdater: newVoiceRadioTech=" + newVoiceRadioTech);

        If (mActivePhone != NULL) {
            // Check for a voice over lte replacement
            If ((newVoiceRadioTech == ServiceState.RIL_RADIO_TECHNOLOGY_LTE) ||
                    (newVoiceRadioTech == ServiceState.RIL_RADIO_TECHNOLOGY_UNKNOWN)) {
                Int32 volteReplacementRat = mActivePhone->GetContext()->GetResources().GetInteger(
                        R.integer.config_volte_replacement_rat);
                Logd("phoneObjectUpdater: volteReplacementRat=" + volteReplacementRat);
                If (volteReplacementRat != ServiceState.RIL_RADIO_TECHNOLOGY_UNKNOWN) {
                    newVoiceRadioTech = volteReplacementRat;
                }
            }

            If(mRilVersion == 6 && GetLteOnCdmaMode() == PhoneConstants.LTE_ON_CDMA_TRUE) {
                /*
                 * On v6 RIL, when LTE_ON_CDMA is TRUE, always create CDMALTEPhone
                 * irrespective of the voice radio tech reported.
                 */
                If (mActivePhone->GetPhoneType() == PhoneConstants.PHONE_TYPE_CDMA) {
                    Logd("phoneObjectUpdater: LTE ON CDMA property is set. Use CDMA Phone" +
                            " newVoiceRadioTech=" + newVoiceRadioTech +
                            " mActivePhone=" + mActivePhone->GetPhoneName());
                    return;
                } else {
                    Logd("phoneObjectUpdater: LTE ON CDMA property is set. Switch to CDMALTEPhone" +
                            " newVoiceRadioTech=" + newVoiceRadioTech +
                            " mActivePhone=" + mActivePhone->GetPhoneName());
                    newVoiceRadioTech = ServiceState.RIL_RADIO_TECHNOLOGY_1xRTT;
                }
            } else {
                If ((ServiceState->IsCdma(newVoiceRadioTech) &&
                        mActivePhone->GetPhoneType() == PhoneConstants.PHONE_TYPE_CDMA) ||
                        (ServiceState->IsGsm(newVoiceRadioTech) &&
                                mActivePhone->GetPhoneType() == PhoneConstants.PHONE_TYPE_GSM)) {
                    // Nothing changed. Keep phone as it is.
                    Logd("phoneObjectUpdater: No change ignore," +
                            " newVoiceRadioTech=" + newVoiceRadioTech +
                            " mActivePhone=" + mActivePhone->GetPhoneName());
                    return;
                }
            }
        }

        If (newVoiceRadioTech == ServiceState.RIL_RADIO_TECHNOLOGY_UNKNOWN) {
            // We need some voice phone object to be active always, so never
            // delete the phone without anything to replace it with!
            Logd("phoneObjectUpdater: Unknown rat ignore, "
                    + " newVoiceRadioTech=Unknown. mActivePhone=" + mActivePhone->GetPhoneName());
            return;
        }

        Boolean oldPowerState = FALSE; // old power state to off
        If (mResetModemOnRadioTechnologyChange) {
            If (mCommandsInterface->GetRadioState()->IsOn()) {
                oldPowerState = TRUE;
                Logd("phoneObjectUpdater: Setting Radio Power to Off");
                mCommandsInterface->SetRadioPower(FALSE, NULL);
            }
        }

        DeleteAndCreatePhone(newVoiceRadioTech);

        If (mResetModemOnRadioTechnologyChange && oldPowerState) { // restore power state
            Logd("phoneObjectUpdater: Resetting Radio");
            mCommandsInterface->SetRadioPower(oldPowerState, NULL);
        }

        // Set the new interfaces in the proxy's
        mIccSmsInterfaceManager->UpdatePhoneObject((PhoneBase) mActivePhone);
        mIccPhoneBookInterfaceManagerProxy->SetmIccPhoneBookInterfaceManager(mActivePhone
                .GetIccPhoneBookInterfaceManager());
        mPhoneSubInfoProxy->SetmPhoneSubInfo(mActivePhone->GetPhoneSubInfo());

        mCommandsInterface = ((PhoneBase)mActivePhone).mCi;
        mIccCardProxy->SetVoiceRadioTech(newVoiceRadioTech);

        // Send an Intent to the PhoneApp that we had a radio technology change
        Intent intent = new Intent(TelephonyIntents.ACTION_RADIO_TECHNOLOGY_CHANGED);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
        intent->PutExtra(PhoneConstants.PHONE_NAME_KEY, mActivePhone->GetPhoneName());
        SubscriptionManager->PutPhoneIdAndSubIdExtra(intent, mPhoneId);
        ActivityManagerNative->BroadcastStickyIntent(intent, NULL, UserHandle.USER_ALL);
    }

    private void DeleteAndCreatePhone(Int32 newVoiceRadioTech) {

        String outgoingPhoneName = "Unknown";
        Phone oldPhone = mActivePhone;
        ImsPhone imsPhone = NULL;

        If (oldPhone != NULL) {
            outgoingPhoneName = ((PhoneBase) oldPhone).GetPhoneName();
            oldPhone->UnregisterForSimRecordsLoaded(this);
        }

        Logd("Switching Voice Phone : " + outgoingPhoneName + " >>> "
                + (ServiceState->IsGsm(newVoiceRadioTech) ? "GSM" : "CDMA"));

        If (ServiceState->IsCdma(newVoiceRadioTech)) {
            mActivePhone = PhoneFactory->GetCdmaPhone(mPhoneId);
        } else If (ServiceState->IsGsm(newVoiceRadioTech)) {
            mActivePhone = PhoneFactory->GetGsmPhone(mPhoneId);
        }

        If (oldPhone != NULL) {
            imsPhone = oldPhone->RelinquishOwnershipOfImsPhone();
        }

        If(mActivePhone != NULL) {
            CallManager->GetInstance()->RegisterPhone(mActivePhone);
            If (imsPhone != NULL) {
                mActivePhone->AcquireOwnershipOfImsPhone(imsPhone);
            }
            mActivePhone->RegisterForSimRecordsLoaded(this, EVENT_SIM_RECORDS_LOADED, NULL);
        }

        If (oldPhone != NULL) {
            CallManager->GetInstance()->UnregisterPhone(oldPhone);
            Logd("Disposing old phone..");
            oldPhone->Dispose();
            // Potential GC issues: however, callers may have references to old
            // phone on which they perform hierarchical funcs: phone->GetA()->GetB()
            // HENCE: do not delete references.
            //oldPhone->RemoveReferences();
        }
        oldPhone = NULL;
    }

    public IccSmsInterfaceManager GetIccSmsInterfaceManager(){
        return mIccSmsInterfaceManager;
    }

    public PhoneSubInfoProxy GetPhoneSubInfoProxy(){
        return mPhoneSubInfoProxy;
    }

    public IccPhoneBookInterfaceManagerProxy GetIccPhoneBookInterfaceManagerProxy() {
        return mIccPhoneBookInterfaceManagerProxy;
    }

    public IccFileHandler GetIccFileHandler() {
        Return ((PhoneBase)mActivePhone).GetIccFileHandler();
    }

    public Boolean IsImsVtCallPresent() {
        return mActivePhone->IsImsVtCallPresent();
    }

    //@Override
    CARAPI UpdatePhoneObject(Int32 voiceRadioTech) {
        Logd("updatePhoneObject: radioTechnology=" + voiceRadioTech);
        SendMessage(ObtainMessage(EVENT_UPDATE_PHONE_OBJECT, voiceRadioTech, 0, NULL));
    }

    //@Override
    public ServiceState GetServiceState() {
        return mActivePhone->GetServiceState();
    }

    //@Override
    public CellLocation GetCellLocation() {
        return mActivePhone->GetCellLocation();
    }

    /**
     * @return all available cell information or NULL if none.
     */
    //@Override
    public List<CellInfo> GetAllCellInfo() {
        return mActivePhone->GetAllCellInfo();
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SetCellInfoListRate(Int32 rateInMillis) {
        mActivePhone->SetCellInfoListRate(rateInMillis);
    }

    //@Override
    public PhoneConstants.DataState GetDataConnectionState() {
        return mActivePhone->GetDataConnectionState(PhoneConstants.APN_TYPE_DEFAULT);
    }

    //@Override
    public PhoneConstants.DataState GetDataConnectionState(String apnType) {
        return mActivePhone->GetDataConnectionState(apnType);
    }

    //@Override
    public DataActivityState GetDataActivityState() {
        return mActivePhone->GetDataActivityState();
    }

    //@Override
    public Context GetContext() {
        return mActivePhone->GetContext();
    }

    //@Override
    CARAPI DisableDnsCheck(Boolean b) {
        mActivePhone->DisableDnsCheck(b);
    }

    //@Override
    public Boolean IsDnsCheckDisabled() {
        return mActivePhone->IsDnsCheckDisabled();
    }

    //@Override
    public PhoneConstants.State GetState() {
        return mActivePhone->GetState();
    }

    //@Override
    public String GetPhoneName() {
        return mActivePhone->GetPhoneName();
    }

    //@Override
    public Int32 GetPhoneType() {
        return mActivePhone->GetPhoneType();
    }

    //@Override
    public String[] GetActiveApnTypes() {
        return mActivePhone->GetActiveApnTypes();
    }

    //@Override
    public String GetActiveApnHost(String apnType) {
        return mActivePhone->GetActiveApnHost(apnType);
    }

    //@Override
    public LinkProperties GetLinkProperties(String apnType) {
        return mActivePhone->GetLinkProperties(apnType);
    }

    //@Override
    public NetworkCapabilities GetNetworkCapabilities(String apnType) {
        return mActivePhone->GetNetworkCapabilities(apnType);
    }

    //@Override
    public SignalStrength GetSignalStrength() {
        return mActivePhone->GetSignalStrength();
    }

    //@Override
    CARAPI RegisterForUnknownConnection(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForUnknownConnection(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForUnknownConnection(Handler h) {
        mActivePhone->UnregisterForUnknownConnection(h);
    }

    //@Override
    CARAPI RegisterForHandoverStateChanged(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForHandoverStateChanged(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForHandoverStateChanged(Handler h) {
        mActivePhone->UnregisterForHandoverStateChanged(h);
    }

    //@Override
    CARAPI RegisterForPreciseCallStateChanged(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForPreciseCallStateChanged(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForPreciseCallStateChanged(Handler h) {
        mActivePhone->UnregisterForPreciseCallStateChanged(h);
    }

    //@Override
    CARAPI RegisterForNewRingingConnection(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForNewRingingConnection(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForNewRingingConnection(Handler h) {
        mActivePhone->UnregisterForNewRingingConnection(h);
    }

    //@Override
    CARAPI RegisterForVideoCapabilityChanged(
            Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForVideoCapabilityChanged(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForVideoCapabilityChanged(Handler h) {
        mActivePhone->UnregisterForVideoCapabilityChanged(h);
    }

    //@Override
    CARAPI RegisterForIncomingRing(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForIncomingRing(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForIncomingRing(Handler h) {
        mActivePhone->UnregisterForIncomingRing(h);
    }

    //@Override
    CARAPI RegisterForDisconnect(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForDisconnect(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForDisconnect(Handler h) {
        mActivePhone->UnregisterForDisconnect(h);
    }

    //@Override
    CARAPI RegisterForMmiInitiate(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForMmiInitiate(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForMmiInitiate(Handler h) {
        mActivePhone->UnregisterForMmiInitiate(h);
    }

    //@Override
    CARAPI RegisterForMmiComplete(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForMmiComplete(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForMmiComplete(Handler h) {
        mActivePhone->UnregisterForMmiComplete(h);
    }

    //@Override
    public List<? extends MmiCode> GetPendingMmiCodes() {
        return mActivePhone->GetPendingMmiCodes();
    }

    //@Override
    CARAPI SendUssdResponse(String ussdMessge) {
        mActivePhone->SendUssdResponse(ussdMessge);
    }

    //@Override
    CARAPI RegisterForServiceStateChanged(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForServiceStateChanged(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForServiceStateChanged(Handler h) {
        mActivePhone->UnregisterForServiceStateChanged(h);
    }

    //@Override
    CARAPI RegisterForSuppServiceNotification(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForSuppServiceNotification(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForSuppServiceNotification(Handler h) {
        mActivePhone->UnregisterForSuppServiceNotification(h);
    }

    //@Override
    CARAPI RegisterForSuppServiceFailed(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForSuppServiceFailed(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForSuppServiceFailed(Handler h) {
        mActivePhone->UnregisterForSuppServiceFailed(h);
    }

    //@Override
    CARAPI RegisterForInCallVoicePrivacyOn(Handler h, Int32 what, Object obj){
        mActivePhone->RegisterForInCallVoicePrivacyOn(h,what,obj);
    }

    //@Override
    CARAPI UnregisterForInCallVoicePrivacyOn(Handler h){
        mActivePhone->UnregisterForInCallVoicePrivacyOn(h);
    }

    //@Override
    CARAPI RegisterForInCallVoicePrivacyOff(Handler h, Int32 what, Object obj){
        mActivePhone->RegisterForInCallVoicePrivacyOff(h,what,obj);
    }

    //@Override
    CARAPI UnregisterForInCallVoicePrivacyOff(Handler h){
        mActivePhone->UnregisterForInCallVoicePrivacyOff(h);
    }

    //@Override
    CARAPI RegisterForCdmaOtaStatusChange(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForCdmaOtaStatusChange(h,what,obj);
    }

    //@Override
    CARAPI UnregisterForCdmaOtaStatusChange(Handler h) {
         mActivePhone->UnregisterForCdmaOtaStatusChange(h);
    }

    //@Override
    CARAPI RegisterForSubscriptionInfoReady(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForSubscriptionInfoReady(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForSubscriptionInfoReady(Handler h) {
        mActivePhone->UnregisterForSubscriptionInfoReady(h);
    }

    //@Override
    CARAPI RegisterForEcmTimerReset(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForEcmTimerReset(h,what,obj);
    }

    //@Override
    CARAPI UnregisterForEcmTimerReset(Handler h) {
        mActivePhone->UnregisterForEcmTimerReset(h);
    }

    //@Override
    CARAPI RegisterForRingbackTone(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForRingbackTone(h,what,obj);
    }

    //@Override
    CARAPI UnregisterForRingbackTone(Handler h) {
        mActivePhone->UnregisterForRingbackTone(h);
    }

    //@Override
    CARAPI RegisterForOnHoldTone(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForOnHoldTone(h,what,obj);
    }

    //@Override
    CARAPI UnregisterForOnHoldTone(Handler h) {
        mActivePhone->UnregisterForOnHoldTone(h);
    }

    //@Override
    CARAPI RegisterForResendIncallMute(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForResendIncallMute(h,what,obj);
    }

    //@Override
    CARAPI UnregisterForResendIncallMute(Handler h) {
        mActivePhone->UnregisterForResendIncallMute(h);
    }

    //@Override
    CARAPI RegisterForSimRecordsLoaded(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForSimRecordsLoaded(h,what,obj);
    }

    CARAPI UnregisterForSimRecordsLoaded(Handler h) {
        mActivePhone->UnregisterForSimRecordsLoaded(h);
    }

    //@Override
    public Boolean GetIccRecordsLoaded() {
        return mIccCardProxy->GetIccRecordsLoaded();
    }

    //@Override
    public IccCard GetIccCard() {
        return mIccCardProxy;
    }

    //@Override
    CARAPI AcceptCall(Int32 videoState) throws CallStateException {
        mActivePhone->AcceptCall(videoState);
    }

    //@Override
    CARAPI DeflectCall(String number) throws CallStateException {
        mActivePhone->DeflectCall(number);
    }

    //@Override
    CARAPI RejectCall() throws CallStateException {
        mActivePhone->RejectCall();
    }

    //@Override
    CARAPI SwitchHoldingAndActive() throws CallStateException {
        mActivePhone->SwitchHoldingAndActive();
    }

    //@Override
    public Boolean CanConference() {
        return mActivePhone->CanConference();
    }

    //@Override
    CARAPI Conference() throws CallStateException {
        mActivePhone->Conference();
    }

    //@Override
    CARAPI EnableEnhancedVoicePrivacy(Boolean enable, Message onComplete) {
        mActivePhone->EnableEnhancedVoicePrivacy(enable, onComplete);
    }

    //@Override
    CARAPI GetEnhancedVoicePrivacy(Message onComplete) {
        mActivePhone->GetEnhancedVoicePrivacy(onComplete);
    }

    //@Override
    public Boolean CanTransfer() {
        return mActivePhone->CanTransfer();
    }

    //@Override
    CARAPI ExplicitCallTransfer() throws CallStateException {
        mActivePhone->ExplicitCallTransfer();
    }

    //@Override
    CARAPI ClearDisconnected() {
        mActivePhone->ClearDisconnected();
    }

    //@Override
    public Call GetForegroundCall() {
        return mActivePhone->GetForegroundCall();
    }

    //@Override
    public Call GetBackgroundCall() {
        return mActivePhone->GetBackgroundCall();
    }

    //@Override
    public Call GetRingingCall() {
        return mActivePhone->GetRingingCall();
    }

    //@Override
    public Connection Dial(String dialString, Int32 videoState) throws CallStateException {
        return mActivePhone->Dial(dialString, videoState);
    }

    //@Override
    public Connection Dial(String dialString, UUSInfo uusInfo, Int32 videoState) throws CallStateException {
        return mActivePhone->Dial(dialString, uusInfo, videoState);
    }

    //@Override
    CARAPI AddParticipant(String dialString) throws CallStateException {
        mActivePhone->AddParticipant(dialString);
    }

    //@Override
    public Boolean HandlePinMmi(String dialString) {
        return mActivePhone->HandlePinMmi(dialString);
    }

    //@Override
    public Boolean HandleInCallMmiCommands(String command) throws CallStateException {
        return mActivePhone->HandleInCallMmiCommands(command);
    }

    //@Override
    CARAPI SendDtmf(Char32 c) {
        mActivePhone->SendDtmf(c);
    }

    //@Override
    CARAPI StartDtmf(Char32 c) {
        mActivePhone->StartDtmf(c);
    }

    //@Override
    CARAPI StopDtmf() {
        mActivePhone->StopDtmf();
    }

    //@Override
    CARAPI SetRadioPower(Boolean power) {
        mActivePhone->SetRadioPower(power);
    }

    //@Override
    public Boolean GetMessageWaitingIndicator() {
        return mActivePhone->GetMessageWaitingIndicator();
    }

    //@Override
    public Boolean GetCallForwardingIndicator() {
        return mActivePhone->GetCallForwardingIndicator();
    }

    //@Override
    public String GetLine1Number() {
        return mActivePhone->GetLine1Number();
    }

    //@Override
    public String GetCdmaMin() {
        return mActivePhone->GetCdmaMin();
    }

    //@Override
    public Boolean IsMinInfoReady() {
        return mActivePhone->IsMinInfoReady();
    }

    //@Override
    public String GetCdmaPrlVersion() {
        return mActivePhone->GetCdmaPrlVersion();
    }

    //@Override
    public String GetLine1AlphaTag() {
        return mActivePhone->GetLine1AlphaTag();
    }

    //@Override
    CARAPI SetLine1Number(String alphaTag, String number, Message onComplete) {
        mActivePhone->SetLine1Number(alphaTag, number, onComplete);
    }

    //@Override
    public String GetVoiceMailNumber() {
        return mActivePhone->GetVoiceMailNumber();
    }

     /** @hide */
    //@Override
    public Int32 GetVoiceMessageCount(){
        return mActivePhone->GetVoiceMessageCount();
    }

    //@Override
    public String GetVoiceMailAlphaTag() {
        return mActivePhone->GetVoiceMailAlphaTag();
    }

    //@Override
    CARAPI SetVoiceMailNumber(String alphaTag,String voiceMailNumber,
            Message onComplete) {
        mActivePhone->SetVoiceMailNumber(alphaTag, voiceMailNumber, onComplete);
    }

    //@Override
    CARAPI GetCallForwardingOption(Int32 commandInterfaceCFReason,
            Message onComplete) {
        mActivePhone->GetCallForwardingOption(commandInterfaceCFReason,
                onComplete);
    }

    //@Override
    CARAPI GetCallForwardingUncondTimerOption(Int32 commandInterfaceCFReason,
            Message onComplete) {
        mActivePhone->GetCallForwardingUncondTimerOption(commandInterfaceCFReason,
                onComplete);
    }

    //@Override
    CARAPI SetCallForwardingOption(Int32 commandInterfaceCFReason,
            Int32 commandInterfaceCFAction, String dialingNumber,
            Int32 timerSeconds, Message onComplete) {
        mActivePhone->SetCallForwardingOption(commandInterfaceCFReason,
            commandInterfaceCFAction, dialingNumber, timerSeconds, onComplete);
    }

    //@Override
    CARAPI SetCallForwardingUncondTimerOption(Int32 startHour, Int32 startMinute,
             Int32 endHour, Int32 endMinute, Int32 commandInterfaceCFReason,
            Int32 commandInterfaceCFAction, String dialingNumber,
            Message onComplete) {
        mActivePhone->SetCallForwardingUncondTimerOption(startHour, startMinute,
            endHour, endMinute, commandInterfaceCFReason,
            commandInterfaceCFAction, dialingNumber, onComplete);
    }

    //@Override
    CARAPI GetOutgoingCallerIdDisplay(Message onComplete) {
        mActivePhone->GetOutgoingCallerIdDisplay(onComplete);
    }

    //@Override
    CARAPI SetOutgoingCallerIdDisplay(Int32 commandInterfaceCLIRMode,
            Message onComplete) {
        mActivePhone->SetOutgoingCallerIdDisplay(commandInterfaceCLIRMode,
                onComplete);
    }

    //@Override
    CARAPI GetCallWaiting(Message onComplete) {
        mActivePhone->GetCallWaiting(onComplete);
    }

    //@Override
    CARAPI SetCallWaiting(Boolean enable, Message onComplete) {
        mActivePhone->SetCallWaiting(enable, onComplete);
    }

    //@Override
    CARAPI GetAvailableNetworks(Message response) {
        mActivePhone->GetAvailableNetworks(response);
    }

    //@Override
    CARAPI SetNetworkSelectionModeAutomatic(Message response) {
        mActivePhone->SetNetworkSelectionModeAutomatic(response);
    }

    //@Override
    CARAPI SelectNetworkManually(OperatorInfo network, Message response) {
        mActivePhone->SelectNetworkManually(network, response);
    }

    //@Override
    CARAPI SetPreferredNetworkType(Int32 networkType, Message response) {
        mActivePhone->SetPreferredNetworkType(networkType, response);
    }

    //@Override
    CARAPI GetPreferredNetworkType(Message response) {
        mActivePhone->GetPreferredNetworkType(response);
    }

    //@Override
    CARAPI GetNeighboringCids(Message response) {
        mActivePhone->GetNeighboringCids(response);
    }

    //@Override
    CARAPI SetOnPostDialCharacter(Handler h, Int32 what, Object obj) {
        mActivePhone->SetOnPostDialCharacter(h, what, obj);
    }

    //@Override
    CARAPI SetMute(Boolean muted) {
        mActivePhone->SetMute(muted);
    }

    //@Override
    public Boolean GetMute() {
        return mActivePhone->GetMute();
    }

    //@Override
    CARAPI SetEchoSuppressionEnabled() {
        mActivePhone->SetEchoSuppressionEnabled();
    }

    //@Override
    CARAPI InvokeOemRilRequestRaw(Byte[] data, Message response) {
        mActivePhone->InvokeOemRilRequestRaw(data, response);
    }

    //@Override
    CARAPI InvokeOemRilRequestStrings(String[] strings, Message response) {
        mActivePhone->InvokeOemRilRequestStrings(strings, response);
    }

    //@Override
    CARAPI GetDataCallList(Message response) {
        mActivePhone->GetDataCallList(response);
    }

    //@Override
    CARAPI UpdateServiceLocation() {
        mActivePhone->UpdateServiceLocation();
    }

    //@Override
    CARAPI EnableLocationUpdates() {
        mActivePhone->EnableLocationUpdates();
    }

    //@Override
    CARAPI DisableLocationUpdates() {
        mActivePhone->DisableLocationUpdates();
    }

    //@Override
    CARAPI SetUnitTestMode(Boolean f) {
        mActivePhone->SetUnitTestMode(f);
    }

    //@Override
    public Boolean GetUnitTestMode() {
        return mActivePhone->GetUnitTestMode();
    }

    //@Override
    CARAPI SetBandMode(Int32 bandMode, Message response) {
        mActivePhone->SetBandMode(bandMode, response);
    }

    //@Override
    CARAPI QueryAvailableBandMode(Message response) {
        mActivePhone->QueryAvailableBandMode(response);
    }

    //@Override
    public Boolean GetDataRoamingEnabled() {
        return mActivePhone->GetDataRoamingEnabled();
    }

    //@Override
    CARAPI SetDataRoamingEnabled(Boolean enable) {
        mActivePhone->SetDataRoamingEnabled(enable);
    }

    //@Override
    public Boolean GetDataEnabled() {
        return mActivePhone->GetDataEnabled();
    }

    //@Override
    CARAPI SetDataEnabled(Boolean enable) {
        mActivePhone->SetDataEnabled(enable);
    }

    //@Override
    CARAPI QueryCdmaRoamingPreference(Message response) {
        mActivePhone->QueryCdmaRoamingPreference(response);
    }

    //@Override
    CARAPI SetCdmaRoamingPreference(Int32 cdmaRoamingType, Message response) {
        mActivePhone->SetCdmaRoamingPreference(cdmaRoamingType, response);
    }

    //@Override
    CARAPI SetCdmaSubscription(Int32 cdmaSubscriptionType, Message response) {
        mActivePhone->SetCdmaSubscription(cdmaSubscriptionType, response);
    }

    //@Override
    public SimulatedRadioControl GetSimulatedRadioControl() {
        return mActivePhone->GetSimulatedRadioControl();
    }

    //@Override
    public Boolean IsDataConnectivityPossible() {
        return mActivePhone->IsDataConnectivityPossible(PhoneConstants.APN_TYPE_DEFAULT);
    }

    //@Override
    public Boolean IsOnDemandDataPossible(String apnType) {
        return mActivePhone->IsOnDemandDataPossible(apnType);
    }

    //@Override
    public Boolean IsDataConnectivityPossible(String apnType) {
        return mActivePhone->IsDataConnectivityPossible(apnType);
    }

    //@Override
    public String GetDeviceId() {
        return mActivePhone->GetDeviceId();
    }

    //@Override
    public String GetDeviceSvn() {
        return mActivePhone->GetDeviceSvn();
    }

    //@Override
    public String GetSubscriberId() {
        return mActivePhone->GetSubscriberId();
    }

    //@Override
    public String GetGroupIdLevel1() {
        return mActivePhone->GetGroupIdLevel1();
    }

    //@Override
    public String GetIccSerialNumber() {
        return mActivePhone->GetIccSerialNumber();
    }

    //@Override
    public String GetEsn() {
        return mActivePhone->GetEsn();
    }

    //@Override
    public String GetMeid() {
        return mActivePhone->GetMeid();
    }

    //@Override
    public String GetMsisdn() {
        return mActivePhone->GetMsisdn();
    }

    //@Override
    public String GetImei() {
        return mActivePhone->GetImei();
    }

    //@Override
    public PhoneSubInfo GetPhoneSubInfo(){
        return mActivePhone->GetPhoneSubInfo();
    }

    //@Override
    public IccPhoneBookInterfaceManager GetIccPhoneBookInterfaceManager(){
        return mActivePhone->GetIccPhoneBookInterfaceManager();
    }

    //@Override
    CARAPI SetUiTTYMode(Int32 uiTtyMode, Message onComplete) {
        mActivePhone->SetUiTTYMode(uiTtyMode, onComplete);
    }

    //@Override
    CARAPI SetTTYMode(Int32 ttyMode, Message onComplete) {
        mActivePhone->SetTTYMode(ttyMode, onComplete);
    }

    //@Override
    CARAPI QueryTTYMode(Message onComplete) {
        mActivePhone->QueryTTYMode(onComplete);
    }

    //@Override
    CARAPI ActivateCellBroadcastSms(Int32 activate, Message response) {
        mActivePhone->ActivateCellBroadcastSms(activate, response);
    }

    //@Override
    CARAPI GetCellBroadcastSmsConfig(Message response) {
        mActivePhone->GetCellBroadcastSmsConfig(response);
    }

    //@Override
    CARAPI SetCellBroadcastSmsConfig(Int32[] configValuesArray, Message response) {
        mActivePhone->SetCellBroadcastSmsConfig(configValuesArray, response);
    }

    //@Override
    CARAPI NotifyDataActivity() {
         mActivePhone->NotifyDataActivity();
    }

    //@Override
    CARAPI GetSmscAddress(Message result) {
        mActivePhone->GetSmscAddress(result);
    }

    //@Override
    CARAPI SetSmscAddress(String address, Message result) {
        mActivePhone->SetSmscAddress(address, result);
    }

    //@Override
    public Int32 GetCdmaEriIconIndex() {
        return mActivePhone->GetCdmaEriIconIndex();
    }

    //@Override
    public String GetCdmaEriText() {
        return mActivePhone->GetCdmaEriText();
    }

    //@Override
    public Int32 GetCdmaEriIconMode() {
        return mActivePhone->GetCdmaEriIconMode();
    }

    public Phone GetActivePhone() {
        return mActivePhone;
    }

    //@Override
    CARAPI SendBurstDtmf(String dtmfString, Int32 on, Int32 off, Message onComplete){
        mActivePhone->SendBurstDtmf(dtmfString, on, off, onComplete);
    }

    //@Override
    CARAPI ExitEmergencyCallbackMode(){
        mActivePhone->ExitEmergencyCallbackMode();
    }

    //@Override
    public Boolean NeedsOtaServiceProvisioning(){
        return mActivePhone->NeedsOtaServiceProvisioning();
    }

    //@Override
    public Boolean IsOtaSpNumber(String dialStr){
        return mActivePhone->IsOtaSpNumber(dialStr);
    }

    //@Override
    CARAPI RegisterForCallWaiting(Handler h, Int32 what, Object obj){
        mActivePhone->RegisterForCallWaiting(h,what,obj);
    }

    //@Override
    CARAPI UnregisterForCallWaiting(Handler h){
        mActivePhone->UnregisterForCallWaiting(h);
    }

    //@Override
    CARAPI RegisterForSignalInfo(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForSignalInfo(h,what,obj);
    }

    //@Override
    CARAPI UnregisterForSignalInfo(Handler h) {
        mActivePhone->UnregisterForSignalInfo(h);
    }

    //@Override
    CARAPI RegisterForDisplayInfo(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForDisplayInfo(h,what,obj);
    }

    //@Override
    CARAPI UnregisterForDisplayInfo(Handler h) {
        mActivePhone->UnregisterForDisplayInfo(h);
    }

    //@Override
    CARAPI RegisterForNumberInfo(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForNumberInfo(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForNumberInfo(Handler h) {
        mActivePhone->UnregisterForNumberInfo(h);
    }

    //@Override
    CARAPI RegisterForRedirectedNumberInfo(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForRedirectedNumberInfo(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForRedirectedNumberInfo(Handler h) {
        mActivePhone->UnregisterForRedirectedNumberInfo(h);
    }

    //@Override
    CARAPI RegisterForLineControlInfo(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForLineControlInfo( h, what, obj);
    }

    //@Override
    CARAPI UnregisterForLineControlInfo(Handler h) {
        mActivePhone->UnregisterForLineControlInfo(h);
    }

    //@Override
    CARAPI RegisterFoT53ClirlInfo(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterFoT53ClirlInfo(h, what, obj);
    }

    //@Override
    CARAPI UnregisterForT53ClirInfo(Handler h) {
        mActivePhone->UnregisterForT53ClirInfo(h);
    }

    //@Override
    CARAPI RegisterForT53AudioControlInfo(Handler h, Int32 what, Object obj) {
        mActivePhone->RegisterForT53AudioControlInfo( h, what, obj);
    }

    //@Override
    CARAPI UnregisterForT53AudioControlInfo(Handler h) {
        mActivePhone->UnregisterForT53AudioControlInfo(h);
    }

    //@Override
    CARAPI SetOnEcbModeExitResponse(Handler h, Int32 what, Object obj){
        mActivePhone->SetOnEcbModeExitResponse(h,what,obj);
    }

    //@Override
    CARAPI UnsetOnEcbModeExitResponse(Handler h){
        mActivePhone->UnsetOnEcbModeExitResponse(h);
    }

    public Boolean IsManualNetSelAllowed() {
        return mActivePhone->IsManualNetSelAllowed();
    }

    //@Override
    public Boolean IsCspPlmnEnabled() {
        return mActivePhone->IsCspPlmnEnabled();
    }

    //@Override
    public IsimRecords GetIsimRecords() {
        return mActivePhone->GetIsimRecords();
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    public Int32 GetLteOnCdmaMode() {
        return mActivePhone->GetLteOnCdmaMode();
    }

    /**
     * {@hide}
     */
    //@Override
    public Int32 GetLteOnGsmMode() {
        return mActivePhone->GetLteOnGsmMode();
    }

    //@Override
    CARAPI SetVoiceMessageWaiting(Int32 line, Int32 countWaiting) {
        mActivePhone->SetVoiceMessageWaiting(line, countWaiting);
    }

    //@Override
    public UsimServiceTable GetUsimServiceTable() {
        return mActivePhone->GetUsimServiceTable();
    }

    //@Override
    public UiccCard GetUiccCard() {
        return mActivePhone->GetUiccCard();
    }

    //@Override
    CARAPI NvReadItem(Int32 itemID, Message response) {
        mActivePhone->NvReadItem(itemID, response);
    }

    //@Override
    CARAPI NvWriteItem(Int32 itemID, String itemValue, Message response) {
        mActivePhone->NvWriteItem(itemID, itemValue, response);
    }

    //@Override
    CARAPI NvWriteCdmaPrl(Byte[] preferredRoamingList, Message response) {
        mActivePhone->NvWriteCdmaPrl(preferredRoamingList, response);
    }

    //@Override
    CARAPI NvResetConfig(Int32 resetType, Message response) {
        mActivePhone->NvResetConfig(resetType, response);
    }

    //@Override
    CARAPI Dispose() {
        If (mActivePhone != NULL) {
            mActivePhone->UnregisterForSimRecordsLoaded(this);
        }
        mCommandsInterface->UnregisterForOn(this);
        mCommandsInterface->UnregisterForVoiceRadioTechChanged(this);
        mCommandsInterface->UnregisterForRilConnected(this);
    }

    //@Override
    CARAPI RemoveReferences() {
        mActivePhone = NULL;
        mCommandsInterface = NULL;
    }

    public Boolean UpdateCurrentCarrierInProvider() {
        If (mActivePhone instanceof CDMALTEPhone) {
            Return ((CDMALTEPhone)mActivePhone).UpdateCurrentCarrierInProvider();
        } else If (mActivePhone instanceof GSMPhone) {
            Return ((GSMPhone)mActivePhone).UpdateCurrentCarrierInProvider();
        } else {
           Loge("Phone object is not MultiSim. This should not hit!!!!");
           return FALSE;
        }
    }

    CARAPI UpdateDataConnectionTracker() {
        Logd("Updating Data Connection Tracker");
        If (mActivePhone instanceof CDMALTEPhone) {
            ((CDMALTEPhone)mActivePhone).UpdateDataConnectionTracker();
        } else If (mActivePhone instanceof GSMPhone) {
            ((GSMPhone)mActivePhone).UpdateDataConnectionTracker();
        } else {
           Loge("Phone object is not MultiSim. This should not hit!!!!");
        }
    }

    CARAPI SetInternalDataEnabled(Boolean enable) {
        SetInternalDataEnabled(enable, NULL);
    }

    public Boolean SetInternalDataEnabledFlag(Boolean enable) {
        Boolean flag = FALSE;
        If (mActivePhone instanceof CDMALTEPhone) {
            flag = ((CDMALTEPhone)mActivePhone).SetInternalDataEnabledFlag(enable);
        } else If (mActivePhone instanceof GSMPhone) {
            flag = ((GSMPhone)mActivePhone).SetInternalDataEnabledFlag(enable);
        } else {
           Loge("Phone object is not MultiSim. This should not hit!!!!");
        }
        return flag;
    }

    CARAPI SetInternalDataEnabled(Boolean enable, Message onCompleteMsg) {
        If (mActivePhone instanceof CDMALTEPhone) {
            ((CDMALTEPhone)mActivePhone).SetInternalDataEnabled(enable, onCompleteMsg);
        } else If (mActivePhone instanceof GSMPhone) {
            ((GSMPhone)mActivePhone).SetInternalDataEnabled(enable, onCompleteMsg);
        } else {
           Loge("Phone object is not MultiSim. This should not hit!!!!");
        }
    }

    CARAPI RegisterForAllDataDisconnected(Handler h, Int32 what, Object obj) {
        If (mActivePhone instanceof CDMALTEPhone) {
            ((CDMALTEPhone)mActivePhone).RegisterForAllDataDisconnected(h, what, obj);
        } else If (mActivePhone instanceof GSMPhone) {
            ((GSMPhone)mActivePhone).RegisterForAllDataDisconnected(h, what, obj);
        } else {
           Loge("Phone object is not MultiSim. This should not hit!!!!");
        }
    }

    CARAPI UnregisterForAllDataDisconnected(Handler h) {
        If (mActivePhone instanceof CDMALTEPhone) {
            ((CDMALTEPhone)mActivePhone).UnregisterForAllDataDisconnected(h);
        } else If (mActivePhone instanceof GSMPhone) {
            ((GSMPhone)mActivePhone).UnregisterForAllDataDisconnected(h);
        } else {
           Loge("Phone object is not MultiSim. This should not hit!!!!");
        }
    }


    public Int64 GetSubId() {
        return mActivePhone->GetSubId();
    }

    public Int32 GetPhoneId() {
        return mActivePhone->GetPhoneId();
    }

    //@Override
    public String[] GetPcscfAddress(String apnType) {
        return mActivePhone->GetPcscfAddress(apnType);
    }

    //@Override
    CARAPI SetImsRegistrationState(Boolean registered){
        Logd("setImsRegistrationState - registered: " + registered);

        mActivePhone->SetImsRegistrationState(registered);

        If ((mActivePhone->GetPhoneName()).Equals("GSM")) {
            GSMPhone GP = (GSMPhone)mActivePhone;
            GP->GetServiceStateTracker()->SetImsRegistrationState(registered);
        } else If ((mActivePhone->GetPhoneName()).Equals("CDMA")) {
            CDMAPhone CP = (CDMAPhone)mActivePhone;
            CP->GetServiceStateTracker()->SetImsRegistrationState(registered);
        }
    }

    //@Override
    public Phone GetImsPhone() {
        return mActivePhone->GetImsPhone();
    }

    //@Override
    public ImsPhone RelinquishOwnershipOfImsPhone() { return NULL; }

    //@Override
    CARAPI AcquireOwnershipOfImsPhone(ImsPhone imsPhone) { }

    //@Override
    public Int32 GetVoicePhoneServiceState() {
        return mActivePhone->GetVoicePhoneServiceState();
    }

    //@Override
    public Boolean SetOperatorBrandOverride(String brand) {
        return mActivePhone->SetOperatorBrandOverride(brand);
    }

    //@Override
    public Boolean IsRadioAvailable() {
        return mCommandsInterface->GetRadioState()->IsAvailable();
    }

    //@Override
    CARAPI ShutdownRadio() {
        mActivePhone->ShutdownRadio();
    }

    CARAPI GetCallBarringOption(String facility, String password, Message onComplete) {
        mActivePhone->GetCallBarringOption(facility, password, onComplete);
    }

    //@Override
    CARAPI SetCallBarringOption(String facility, Boolean lockState, String password,
            Message onComplete) {
        mActivePhone->SetCallBarringOption(facility, lockState, password, onComplete);
    }

    //@Override
    CARAPI RequestChangeCbPsw(String facility, String oldPwd, String newPwd, Message result) {
        mActivePhone->RequestChangeCbPsw(facility, oldPwd, newPwd, result);
    }

    //@Override
    CARAPI SetLocalCallHold(Int32 lchStatus) {
        mActivePhone->SetLocalCallHold(lchStatus);
    }

    //@Override
    public Connection Dial(String dialString, Int32 videoState, Bundle extras)
            throws CallStateException {
        return mActivePhone->Dial(dialString, videoState, extras);
    }
}
