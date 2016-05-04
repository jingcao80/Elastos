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
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;

using Elastos::Droid::Internal::Telephony::IBaseCommands;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IUUSInfo;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaSmsBroadcastConfigInfo;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDataProfile;
using Elastos::Droid::Internal::Telephony::Gsm::ISmsBroadcastConfigInfo;

/**
 * Volte doesn't need CommandsInterface. The class does nothing but made to work
 * with PhoneBase's constructor.
 */
class ImsPhoneCommandInterface extends BaseCommands implements CommandsInterface {
    ImsPhoneCommandInterface(Context context) {
        Super(context);
    }

    //@Override
    CARAPI SetOnNITZTime(Handler h, Int32 what, Object obj) {
    }

    //@Override
    CARAPI GetIccCardStatus(Message result) {
    }

    //@Override
    CARAPI SupplyIccPin(String pin, Message result) {
    }

    //@Override
    CARAPI SupplyIccPuk(String puk, String newPin, Message result) {
    }

    //@Override
    CARAPI SupplyIccPin2(String pin, Message result) {
    }

    //@Override
    CARAPI SupplyIccPuk2(String puk, String newPin2, Message result) {
    }

    //@Override
    CARAPI ChangeIccPin(String oldPin, String newPin, Message result) {
    }

    //@Override
    CARAPI ChangeIccPin2(String oldPin2, String newPin2, Message result) {
    }

    //@Override
    CARAPI ChangeBarringPassword(String facility, String oldPwd,
            String newPwd, Message result) {
    }

    //@Override
    CARAPI SupplyDepersonalization(String netpin, String type, Message result) {
    }

    //@Override
    CARAPI GetCurrentCalls(Message result) {
    }

    //@Override
    //@Deprecated
    CARAPI GetPDPContextList(Message result) {
    }

    //@Override
    CARAPI GetDataCallList(Message result) {
    }

    //@Override
    CARAPI Dial(String address, Int32 clirMode, Message result) {
    }

    //@Override
    CARAPI Dial(String address, Int32 clirMode, UUSInfo uusInfo,
            Message result) {
    }

    //@Override
    CARAPI GetIMSI(Message result) {
    }

    //@Override
    CARAPI GetIMSIForApp(String aid, Message result) {
    }

    //@Override
    CARAPI GetIMEI(Message result) {
    }

    //@Override
    CARAPI GetIMEISV(Message result) {
    }

    //@Override
    CARAPI HangupConnection (Int32 gsmIndex, Message result) {
    }

    //@Override
    CARAPI HangupWaitingOrBackground (Message result) {
    }

    //@Override
    CARAPI HangupForegroundResumeBackground (Message result) {
    }

    //@Override
    CARAPI SwitchWaitingOrHoldingAndActive (Message result) {
    }

    //@Override
    CARAPI Conference (Message result) {
    }

    //@Override
    CARAPI SetPreferredVoicePrivacy(Boolean enable, Message result) {
    }

    //@Override
    CARAPI GetPreferredVoicePrivacy(Message result) {
    }

    //@Override
    CARAPI SeparateConnection (Int32 gsmIndex, Message result) {
    }

    //@Override
    CARAPI AcceptCall (Message result) {
    }

    //@Override
    CARAPI RejectCall (Message result) {
    }

    //@Override
    CARAPI ExplicitCallTransfer (Message result) {
    }

    //@Override
    CARAPI GetLastCallFailCause (Message result) {
    }

    //@Deprecated
    //@Override
    CARAPI GetLastPdpFailCause (Message result) {
    }

    //@Override
    CARAPI GetLastDataCallFailCause (Message result) {
    }

    //@Override
    CARAPI SetMute (Boolean enableMute, Message response) {
    }

    //@Override
    CARAPI GetMute (Message response) {
    }

    //@Override
    CARAPI GetSignalStrength (Message result) {
    }

    //@Override
    CARAPI GetVoiceRegistrationState (Message result) {
    }

    //@Override
    CARAPI GetDataRegistrationState (Message result) {
    }

    //@Override
    CARAPI GetOperator(Message result) {
    }

    //@Override
    CARAPI SendDtmf(Char32 c, Message result) {
    }

    //@Override
    CARAPI StartDtmf(Char32 c, Message result) {
    }

    //@Override
    CARAPI StopDtmf(Message result) {
    }

    //@Override
    CARAPI SendBurstDtmf(String dtmfString, Int32 on, Int32 off,
            Message result) {
    }

    //@Override
    CARAPI SendSMS (String smscPDU, String pdu, Message result) {
    }

    //@Override
    CARAPI SendSMSExpectMore (String smscPDU, String pdu, Message result) {
    }

    //@Override
    CARAPI SendCdmaSms(Byte[] pdu, Message result) {
    }

    //@Override
    CARAPI SendImsGsmSms (String smscPDU, String pdu,
            Int32 retry, Int32 messageRef, Message response) {
    }

    //@Override
    CARAPI SendImsCdmaSms(Byte[] pdu, Int32 retry, Int32 messageRef,
            Message response) {
    }

    //@Override
    CARAPI GetImsRegistrationState (Message result) {
    }

    //@Override
    CARAPI DeleteSmsOnSim(Int32 index, Message response) {
    }

    //@Override
    CARAPI DeleteSmsOnRuim(Int32 index, Message response) {
    }

    //@Override
    CARAPI WriteSmsToSim(Int32 status, String smsc, String pdu, Message response) {
    }

    //@Override
    CARAPI WriteSmsToRuim(Int32 status, String pdu, Message response) {
    }

    //@Override
    CARAPI SetupDataCall(String radioTechnology, String profile,
            String apn, String user, String password, String authType,
            String protocol, Message result) {
    }

    //@Override
    CARAPI DeactivateDataCall(Int32 cid, Int32 reason, Message result) {
    }

    //@Override
    CARAPI SetRadioPower(Boolean on, Message result) {
    }

    //@Override
    CARAPI SetSuppServiceNotifications(Boolean enable, Message result) {
    }

    //@Override
    CARAPI AcknowledgeLastIncomingGsmSms(Boolean success, Int32 cause,
            Message result) {
    }

    //@Override
    CARAPI AcknowledgeLastIncomingCdmaSms(Boolean success, Int32 cause,
            Message result) {
    }

    //@Override
    CARAPI AcknowledgeIncomingGsmSmsWithPdu(Boolean success, String ackPdu,
            Message result) {
    }

    //@Override
    CARAPI IccIO (Int32 command, Int32 fileid, String path, Int32 p1, Int32 p2,
            Int32 p3, String data, String pin2, Message result) {
    }
    //@Override
    CARAPI IccIOForApp (Int32 command, Int32 fileid, String path, Int32 p1, Int32 p2,
            Int32 p3, String data, String pin2, String aid, Message result) {
    }

    //@Override
    CARAPI GetCLIR(Message result) {
    }

    //@Override
    CARAPI SetCLIR(Int32 clirMode, Message result) {
    }

    //@Override
    CARAPI QueryCallWaiting(Int32 serviceClass, Message response) {
    }

    //@Override
    CARAPI SetCallWaiting(Boolean enable, Int32 serviceClass,
            Message response) {
    }

    //@Override
    CARAPI SetNetworkSelectionModeAutomatic(Message response) {
    }

    //@Override
    CARAPI SetNetworkSelectionModeManual(
            String operatorNumeric, Message response) {
    }

    //@Override
    CARAPI GetNetworkSelectionMode(Message response) {
    }

    //@Override
    CARAPI GetAvailableNetworks(Message response) {
    }

    //@Override
    CARAPI SetCallForward(Int32 action, Int32 cfReason, Int32 serviceClass,
                String number, Int32 timeSeconds, Message response) {
    }

    //@Override
    CARAPI QueryCallForwardStatus(Int32 cfReason, Int32 serviceClass,
            String number, Message response) {
    }

    //@Override
    CARAPI QueryCLIP(Message response) {
    }

    //@Override
    CARAPI GetBasebandVersion (Message response) {
    }

    //@Override
    CARAPI QueryFacilityLock(String facility, String password,
            Int32 serviceClass, Message response) {
    }

    //@Override
    CARAPI QueryFacilityLockForApp(String facility, String password,
            Int32 serviceClass, String appId, Message response) {
    }

    //@Override
    CARAPI SetFacilityLock(String facility, Boolean lockState,
            String password, Int32 serviceClass, Message response) {
    }

    //@Override
    CARAPI SetFacilityLockForApp(String facility, Boolean lockState,
            String password, Int32 serviceClass, String appId, Message response) {
    }

    //@Override
    CARAPI SendUSSD (String ussdString, Message response) {
    }

    //@Override
    CARAPI CancelPendingUssd (Message response) {
    }

    //@Override
    CARAPI ResetRadio(Message result) {
    }

    //@Override
    CARAPI InvokeOemRilRequestRaw(Byte[] data, Message response) {
    }

    //@Override
    CARAPI InvokeOemRilRequestStrings(String[] strings, Message response) {
    }

    //@Override
    CARAPI SetBandMode (Int32 bandMode, Message response) {
    }

    //@Override
    CARAPI QueryAvailableBandMode (Message response) {
    }

    //@Override
    CARAPI SendTerminalResponse(String contents, Message response) {
    }

    //@Override
    CARAPI SendEnvelope(String contents, Message response) {
    }

    //@Override
    CARAPI SendEnvelopeWithStatus(String contents, Message response) {
    }

    //@Override
    CARAPI HandleCallSetupRequestFromSim(
            Boolean accept, Message response) {
    }

    //@Override
    CARAPI SetPreferredNetworkType(Int32 networkType , Message response) {
    }

    //@Override
    CARAPI GetPreferredNetworkType(Message response) {
    }

    //@Override
    CARAPI GetNeighboringCids(Message response) {
    }

    //@Override
    CARAPI SetLocationUpdates(Boolean enable, Message response) {
    }

    //@Override
    CARAPI GetSmscAddress(Message result) {
    }

    //@Override
    CARAPI SetSmscAddress(String address, Message result) {
    }

    //@Override
    CARAPI ReportSmsMemoryStatus(Boolean available, Message result) {
    }

    //@Override
    CARAPI ReportStkServiceIsRunning(Message result) {
    }

    //@Override
    CARAPI GetCdmaSubscriptionSource(Message response) {
    }

    //@Override
    CARAPI GetGsmBroadcastConfig(Message response) {
    }

    //@Override
    CARAPI SetGsmBroadcastConfig(SmsBroadcastConfigInfo[] config, Message response) {
    }

    //@Override
    CARAPI SetGsmBroadcastActivation(Boolean activate, Message response) {
    }

    // ***** Methods for CDMA support
    //@Override
    CARAPI GetDeviceIdentity(Message response) {
    }

    //@Override
    CARAPI GetCDMASubscription(Message response) {
    }

    //@Override
    CARAPI SetPhoneType(Int32 phoneType) { //Set by CDMAPhone and GSMPhone constructor
    }

    //@Override
    CARAPI QueryCdmaRoamingPreference(Message response) {
    }

    //@Override
    CARAPI SetCdmaRoamingPreference(Int32 cdmaRoamingType, Message response) {
    }

    //@Override
    CARAPI SetCdmaSubscriptionSource(Int32 cdmaSubscription , Message response) {
    }

    //@Override
    CARAPI QueryTTYMode(Message response) {
    }

    //@Override
    CARAPI SetTTYMode(Int32 ttyMode, Message response) {
    }

    //@Override
    CARAPI SendCDMAFeatureCode(String FeatureCode, Message response) {
    }

    //@Override
    CARAPI GetCdmaBroadcastConfig(Message response) {
    }

    //@Override
    CARAPI SetCdmaBroadcastConfig(CdmaSmsBroadcastConfigInfo[] configs, Message response) {
    }

    //@Override
    CARAPI SetCdmaBroadcastActivation(Boolean activate, Message response) {
    }

    //@Override
    CARAPI ExitEmergencyCallbackMode(Message response) {
    }

    //@Override
    CARAPI SupplyIccPinForApp(String pin, String aid, Message response) {
    }

    //@Override
    CARAPI SupplyIccPukForApp(String puk, String newPin, String aid, Message response) {
    }

    //@Override
    CARAPI SupplyIccPin2ForApp(String pin2, String aid, Message response) {
    }

    //@Override
    CARAPI SupplyIccPuk2ForApp(String puk2, String newPin2, String aid, Message response) {
    }

    //@Override
    CARAPI ChangeIccPinForApp(String oldPin, String newPin, String aidPtr, Message response) {
    }

    //@Override
    CARAPI ChangeIccPin2ForApp(String oldPin2, String newPin2, String aidPtr,
            Message response) {
    }

    //@Override
    CARAPI RequestIsimAuthentication(String nonce, Message response) {
    }

    //@Override
    CARAPI RequestIccSimAuthentication(Int32 authContext, String data, String aid, Message response) {
    }

    //@Override
    CARAPI GetVoiceRadioTechnology(Message result) {
    }

    //@Override
    CARAPI GetCellInfoList(Message result) {
    }

    //@Override
    CARAPI SetCellInfoListRate(Int32 rateInMillis, Message response) {
    }

    //@Override
    CARAPI SetInitialAttachApn(String apn, String protocol, Int32 authType, String username,
            String password, Message result) {
    }

    //@Override
    CARAPI SetDataProfile(DataProfile[] dps, Message result) {
    }

    //@Override
    CARAPI NvReadItem(Int32 itemID, Message response) {}

    //@Override
    CARAPI NvWriteItem(Int32 itemID, String itemValue, Message response) {}

    //@Override
    CARAPI NvWriteCdmaPrl(Byte[] preferredRoamingList, Message response) {}

    //@Override
    CARAPI NvResetConfig(Int32 resetType, Message response) {}

    //@Override
    CARAPI GetHardwareConfig(Message result) {}

    //@Override
    CARAPI RequestShutdown(Message result) {
    }

    public Boolean NeedsOldRilFeature(String feature) {
        return FALSE;
    }
}
