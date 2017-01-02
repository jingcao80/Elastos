//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/sip/SipCommandInterface.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Sip {

//=====================================================================
//                         SipCommandInterface
//=====================================================================
SipCommandInterface::SipCommandInterface()
{
}

ECode SipCommandInterface::constructor(
    /* [in] */ IContext* context)
{
    return BaseCommands::constructor(context);
}

ECode SipCommandInterface::SetOnNITZTime(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return NOERROR;
}

ECode SipCommandInterface::GetIccCardStatus(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::SupplyIccPin(
    /* [in] */ const String& pin,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::SupplyIccPuk(
    /* [in] */ const String& puk,
    /* [in] */ const String& newPin,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::SupplyIccPin2(
    /* [in] */ const String& pin,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::SupplyIccPuk2(
    /* [in] */ const String& puk,
    /* [in] */ const String& newPin2,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::ChangeIccPin(
    /* [in] */ const String& oldPin,
    /* [in] */ const String& newPin,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::ChangeIccPin2(
    /* [in] */ const String& oldPin2,
    /* [in] */ const String& newPin2,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::ChangeBarringPassword(
    /* [in] */ const String& facility,
    /* [in] */ const String& oldPwd,
    /* [in] */ const String& newPwd,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::SupplyDepersonalization(
    /* [in] */ const String& netpin,
    /* [in] */ const String& type,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::GetCurrentCalls(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::GetPDPContextList(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::GetDataCallList(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::Dial(
    /* [in] */ const String& address,
    /* [in] */ Int32 clirMode,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::Dial(
    /* [in] */ const String& address,
    /* [in] */ Int32 clirMode,
    /* [in] */ IUUSInfo* uusInfo,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::GetIMSI(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::GetIMSIForApp(
    /* [in] */ const String& aid,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::GetIMEI(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::GetIMEISV(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::HangupConnection(
    /* [in] */ Int32 gsmIndex,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::HangupWaitingOrBackground(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::HangupForegroundResumeBackground(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::SwitchWaitingOrHoldingAndActive(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::Conference(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::SetPreferredVoicePrivacy(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::GetPreferredVoicePrivacy(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::SeparateConnection(
    /* [in] */ Int32 gsmIndex,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::AcceptCall(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::RejectCall(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::ExplicitCallTransfer(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::GetLastCallFailCause(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::GetLastPdpFailCause(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::GetLastDataCallFailCause(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::SetMute(
    /* [in] */ Boolean enableMute,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::GetMute(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::GetSignalStrength(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::GetVoiceRegistrationState(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::GetDataRegistrationState(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::GetOperator(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::SendDtmf(
    /* [in] */ Char32 c,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::StartDtmf(
    /* [in] */ Char32 c,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::StopDtmf(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::SendBurstDtmf(
    /* [in] */ const String& dtmfString,
    /* [in] */ Int32 on,
    /* [in] */ Int32 off,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::SendSMS(
    /* [in] */ const String& smscPDU,
    /* [in] */ const String& pdu,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::SendSMSExpectMore(
    /* [in] */ const String& smscPDU,
    /* [in] */ const String& pdu,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::SendCdmaSms(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::SendImsGsmSms(
    /* [in] */ const String& smscPDU,
    /* [in] */ const String& pdu,
    /* [in] */ Int32 retry,
    /* [in] */ Int32 messageRef,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SendImsCdmaSms(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ Int32 retry,
    /* [in] */ Int32 messageRef,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::GetImsRegistrationState(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::DeleteSmsOnSim(
    /* [in] */ Int32 index,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::DeleteSmsOnRuim(
    /* [in] */ Int32 index,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::WriteSmsToSim(
    /* [in] */ Int32 status,
    /* [in] */ const String& smsc,
    /* [in] */ const String& pdu,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::WriteSmsToRuim(
    /* [in] */ Int32 status,
    /* [in] */ const String& pdu,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SetupDataCall(
    /* [in] */ const String& radioTechnology,
    /* [in] */ const String& profile,
    /* [in] */ const String& apn,
    /* [in] */ const String& user,
    /* [in] */ const String& password,
    /* [in] */ const String& authType,
    /* [in] */ const String& protocol,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::DeactivateDataCall(
    /* [in] */ Int32 cid,
    /* [in] */ Int32 reason,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::SetRadioPower(
    /* [in] */ Boolean on,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::SetSuppServiceNotifications(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::AcknowledgeLastIncomingGsmSms(
    /* [in] */ Boolean success,
    /* [in] */ Int32 cause,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::AcknowledgeLastIncomingCdmaSms(
    /* [in] */ Boolean success,
    /* [in] */ Int32 cause,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::AcknowledgeIncomingGsmSmsWithPdu(
    /* [in] */ Boolean success,
    /* [in] */ const String& ackPdu,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::IccIO(
    /* [in] */ Int32 command,
    /* [in] */ Int32 fileid,
    /* [in] */ const String& path,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ const String& data,
    /* [in] */ const String& pin2,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::IccIOForApp(
    /* [in] */ Int32 command,
    /* [in] */ Int32 fileid,
    /* [in] */ const String& path,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ const String& data,
    /* [in] */ const String& pin2,
    /* [in] */ const String& aid,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::GetCLIR(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::SetCLIR(
    /* [in] */ Int32 clirMode,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::QueryCallWaiting(
    /* [in] */ Int32 serviceClass,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SetCallWaiting(
    /* [in] */ Boolean enable,
    /* [in] */ Int32 serviceClass,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SetNetworkSelectionModeAutomatic(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SetNetworkSelectionModeManual(
    /* [in] */ const String& operatorNumeric,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::GetNetworkSelectionMode(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::GetAvailableNetworks(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SetCallForward(
    /* [in] */ Int32 action,
    /* [in] */ Int32 cfReason,
    /* [in] */ Int32 serviceClass,
    /* [in] */ const String& number,
    /* [in] */ Int32 timeSeconds,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::QueryCallForwardStatus(
    /* [in] */ Int32 cfReason,
    /* [in] */ Int32 serviceClass,
    /* [in] */ const String& number,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::QueryCLIP(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::GetBasebandVersion(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::QueryFacilityLock(
    /* [in] */ const String& facility,
    /* [in] */ const String& password,
    /* [in] */ Int32 serviceClass,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::QueryFacilityLockForApp(
    /* [in] */ const String& facility,
    /* [in] */ const String& password,
    /* [in] */ Int32 serviceClass,
    /* [in] */ const String& appId,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SetFacilityLock(
    /* [in] */ const String& facility,
    /* [in] */ Boolean lockState,
    /* [in] */ const String& password,
    /* [in] */ Int32 serviceClass,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SetFacilityLockForApp(
    /* [in] */ const String& facility,
    /* [in] */ Boolean lockState,
    /* [in] */ const String& password,
    /* [in] */ Int32 serviceClass,
    /* [in] */ const String& appId,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SendUSSD(
    /* [in] */ const String& ussdString,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::CancelPendingUssd(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::ResetRadio(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::InvokeOemRilRequestRaw(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::InvokeOemRilRequestStrings(
    /* [in] */ ArrayOf<String>* strings,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SetBandMode(
    /* [in] */ Int32 bandMode,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::QueryAvailableBandMode(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SendTerminalResponse(
    /* [in] */ const String& contents,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SendEnvelope(
    /* [in] */ const String& contents,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SendEnvelopeWithStatus(
    /* [in] */ const String& contents,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::HandleCallSetupRequestFromSim(
    /* [in] */ Boolean accept,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SetPreferredNetworkType(
    /* [in] */ Int32 networkType,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::GetPreferredNetworkType(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::GetNeighboringCids(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SetLocationUpdates(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::GetSmscAddress(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::SetSmscAddress(
    /* [in] */ const String& address,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::ReportSmsMemoryStatus(
    /* [in] */ Boolean available,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::ReportStkServiceIsRunning(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::GetCdmaSubscriptionSource(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::GetGsmBroadcastConfig(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SetGsmBroadcastConfig(
    /* [in] */ ArrayOf<ISmsBroadcastConfigInfo*>* config,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SetGsmBroadcastActivation(
    /* [in] */ Boolean activate,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::GetDeviceIdentity(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::GetCDMASubscription(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SetPhoneType(
    /* [in] */ Int32 phoneType)
{
    // ==================before translated======================
    // //Set by CDMAPhone and GSMPhone constructor
    return NOERROR;
}

ECode SipCommandInterface::QueryCdmaRoamingPreference(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SetCdmaRoamingPreference(
    /* [in] */ Int32 cdmaRoamingType,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SetCdmaSubscriptionSource(
    /* [in] */ Int32 cdmaSubscription,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::QueryTTYMode(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SetTTYMode(
    /* [in] */ Int32 ttyMode,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SendCDMAFeatureCode(
    /* [in] */ const String& FeatureCode,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::GetCdmaBroadcastConfig(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SetCdmaBroadcastConfig(
    /* [in] */ ArrayOf<ICdmaSmsBroadcastConfigInfo*>* configs,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SetCdmaBroadcastActivation(
    /* [in] */ Boolean activate,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::ExitEmergencyCallbackMode(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SupplyIccPinForApp(
    /* [in] */ const String& pin,
    /* [in] */ const String& aid,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SupplyIccPukForApp(
    /* [in] */ const String& puk,
    /* [in] */ const String& newPin,
    /* [in] */ const String& aid,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SupplyIccPin2ForApp(
    /* [in] */ const String& pin2,
    /* [in] */ const String& aid,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SupplyIccPuk2ForApp(
    /* [in] */ const String& puk2,
    /* [in] */ const String& newPin2,
    /* [in] */ const String& aid,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::ChangeIccPinForApp(
    /* [in] */ const String& oldPin,
    /* [in] */ const String& newPin,
    /* [in] */ const String& aidPtr,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::ChangeIccPin2ForApp(
    /* [in] */ const String& oldPin2,
    /* [in] */ const String& newPin2,
    /* [in] */ const String& aidPtr,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::RequestIsimAuthentication(
    /* [in] */ const String& nonce,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::RequestIccSimAuthentication(
    /* [in] */ Int32 authContext,
    /* [in] */ const String& data,
    /* [in] */ const String& aid,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::GetVoiceRadioTechnology(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::GetCellInfoList(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::SetCellInfoListRate(
    /* [in] */ Int32 rateInMillis,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::SetInitialAttachApn(
    /* [in] */ const String& apn,
    /* [in] */ const String& protocol,
    /* [in] */ Int32 authType,
    /* [in] */ const String& username,
    /* [in] */ const String& password,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::NvReadItem(
    /* [in] */ Int32 itemID,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::NvWriteItem(
    /* [in] */ Int32 itemID,
    /* [in] */ const String& itemValue,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::NvWriteCdmaPrl(
    /* [in] */ ArrayOf<Byte>* preferredRoamingList,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::NvResetConfig(
    /* [in] */ Int32 resetType,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipCommandInterface::GetHardwareConfig(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::RequestShutdown(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode SipCommandInterface::NeedsOldRilFeature(
    /* [in] */ const String& feature,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

} // namespace Sip
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
