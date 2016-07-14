#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONECOMMANDINTERFACE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONECOMMANDINTERFACE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/droid/internal/telephony/BaseCommands.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::BaseCommands;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IUUSInfo;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaSmsBroadcastConfigInfo;
using Elastos::Droid::Internal::Telephony::DataConnection::IDataProfile;
using Elastos::Droid::Internal::Telephony::Gsm::ISmsBroadcastConfigInfo;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

/**
  * Volte doesn't need CommandsInterface. The class does nothing but made to work
  * with PhoneBase's constructor.
  */
class ImsPhoneCommandInterface
    : public BaseCommands
{
public:
    ImsPhoneCommandInterface();

    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI SetOnNITZTime(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI GetIccCardStatus(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI SupplyIccPin(
        /* [in] */ const String& pin,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI SupplyIccPuk(
        /* [in] */ const String& puk,
        /* [in] */ const String& newPin,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI SupplyIccPin2(
        /* [in] */ const String& pin,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI SupplyIccPuk2(
        /* [in] */ const String& puk,
        /* [in] */ const String& newPin2,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI ChangeIccPin(
        /* [in] */ const String& oldPin,
        /* [in] */ const String& newPin,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI ChangeIccPin2(
        /* [in] */ const String& oldPin2,
        /* [in] */ const String& newPin2,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI ChangeBarringPassword(
        /* [in] */ const String& facility,
        /* [in] */ const String& oldPwd,
        /* [in] */ const String& newPwd,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI SupplyDepersonalization(
        /* [in] */ const String& netpin,
        /* [in] */ const String& type,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI GetCurrentCalls(
        /* [in] */ IMessage* result);

    // @Override
    // @Deprecated
    virtual CARAPI GetPDPContextList(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI GetDataCallList(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI Dial(
        /* [in] */ const String& address,
        /* [in] */ Int32 clirMode,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI Dial(
        /* [in] */ const String& address,
        /* [in] */ Int32 clirMode,
        /* [in] */ IUUSInfo* uusInfo,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI GetIMSI(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI GetIMSIForApp(
        /* [in] */ const String& aid,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI GetIMEI(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI GetIMEISV(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI HangupConnection(
        /* [in] */ Int32 gsmIndex,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI HangupWaitingOrBackground(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI HangupForegroundResumeBackground(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI SwitchWaitingOrHoldingAndActive(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI Conference(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI SetPreferredVoicePrivacy(
        /* [in] */ Boolean enable,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI GetPreferredVoicePrivacy(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI SeparateConnection(
        /* [in] */ Int32 gsmIndex,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI AcceptCall(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI RejectCall(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI ExplicitCallTransfer(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI GetLastCallFailCause(
        /* [in] */ IMessage* result);

    // @Deprecated
    // @Override
    CARAPI GetLastPdpFailCause(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI GetLastDataCallFailCause(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI SetMute(
        /* [in] */ Boolean enableMute,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI GetMute(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI GetSignalStrength(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI GetVoiceRegistrationState(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI GetDataRegistrationState(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI GetOperator(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI SendDtmf(
        /* [in] */ Char32 c,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI StartDtmf(
        /* [in] */ Char32 c,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI StopDtmf(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI SendBurstDtmf(
        /* [in] */ const String& dtmfString,
        /* [in] */ Int32 on,
        /* [in] */ Int32 off,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI SendSMS(
        /* [in] */ const String& smscPDU,
        /* [in] */ const String& pdu,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI SendSMSExpectMore(
        /* [in] */ const String& smscPDU,
        /* [in] */ const String& pdu,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI SendCdmaSms(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI SendImsGsmSms(
        /* [in] */ const String& smscPDU,
        /* [in] */ const String& pdu,
        /* [in] */ Int32 retry,
        /* [in] */ Int32 messageRef,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SendImsCdmaSms(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ Int32 retry,
        /* [in] */ Int32 messageRef,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI GetImsRegistrationState(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI DeleteSmsOnSim(
        /* [in] */ Int32 index,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI DeleteSmsOnRuim(
        /* [in] */ Int32 index,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI WriteSmsToSim(
        /* [in] */ Int32 status,
        /* [in] */ const String& smsc,
        /* [in] */ const String& pdu,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI WriteSmsToRuim(
        /* [in] */ Int32 status,
        /* [in] */ const String& pdu,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetupDataCall(
        /* [in] */ const String& radioTechnology,
        /* [in] */ const String& profile,
        /* [in] */ const String& apn,
        /* [in] */ const String& user,
        /* [in] */ const String& password,
        /* [in] */ const String& authType,
        /* [in] */ const String& protocol,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI DeactivateDataCall(
        /* [in] */ Int32 cid,
        /* [in] */ Int32 reason,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI SetRadioPower(
        /* [in] */ Boolean on,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI SetSuppServiceNotifications(
        /* [in] */ Boolean enable,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI AcknowledgeLastIncomingGsmSms(
        /* [in] */ Boolean success,
        /* [in] */ Int32 cause,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI AcknowledgeLastIncomingCdmaSms(
        /* [in] */ Boolean success,
        /* [in] */ Int32 cause,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI AcknowledgeIncomingGsmSmsWithPdu(
        /* [in] */ Boolean success,
        /* [in] */ const String& ackPdu,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI IccIO(
        /* [in] */ Int32 command,
        /* [in] */ Int32 fileid,
        /* [in] */ const String& path,
        /* [in] */ Int32 p1,
        /* [in] */ Int32 p2,
        /* [in] */ Int32 p3,
        /* [in] */ const String& data,
        /* [in] */ const String& pin2,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI IccIOForApp(
        /* [in] */ Int32 command,
        /* [in] */ Int32 fileid,
        /* [in] */ const String& path,
        /* [in] */ Int32 p1,
        /* [in] */ Int32 p2,
        /* [in] */ Int32 p3,
        /* [in] */ const String& data,
        /* [in] */ const String& pin2,
        /* [in] */ const String& aid,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI GetCLIR(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI SetCLIR(
        /* [in] */ Int32 clirMode,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI QueryCallWaiting(
        /* [in] */ Int32 serviceClass,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetCallWaiting(
        /* [in] */ Boolean enable,
        /* [in] */ Int32 serviceClass,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetNetworkSelectionModeAutomatic(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetNetworkSelectionModeManual(
        /* [in] */ const String& operatorNumeric,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI GetNetworkSelectionMode(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI GetAvailableNetworks(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetCallForward(
        /* [in] */ Int32 action,
        /* [in] */ Int32 cfReason,
        /* [in] */ Int32 serviceClass,
        /* [in] */ const String& number,
        /* [in] */ Int32 timeSeconds,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI QueryCallForwardStatus(
        /* [in] */ Int32 cfReason,
        /* [in] */ Int32 serviceClass,
        /* [in] */ const String& number,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI QueryCLIP(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI GetBasebandVersion(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI QueryFacilityLock(
        /* [in] */ const String& facility,
        /* [in] */ const String& password,
        /* [in] */ Int32 serviceClass,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI QueryFacilityLockForApp(
        /* [in] */ const String& facility,
        /* [in] */ const String& password,
        /* [in] */ Int32 serviceClass,
        /* [in] */ const String& appId,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetFacilityLock(
        /* [in] */ const String& facility,
        /* [in] */ Boolean lockState,
        /* [in] */ const String& password,
        /* [in] */ Int32 serviceClass,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetFacilityLockForApp(
        /* [in] */ const String& facility,
        /* [in] */ Boolean lockState,
        /* [in] */ const String& password,
        /* [in] */ Int32 serviceClass,
        /* [in] */ const String& appId,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SendUSSD(
        /* [in] */ const String& ussdString,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI CancelPendingUssd(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI ResetRadio(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI InvokeOemRilRequestRaw(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI InvokeOemRilRequestStrings(
        /* [in] */ ArrayOf<String>* strings,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetBandMode(
        /* [in] */ Int32 bandMode,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI QueryAvailableBandMode(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SendTerminalResponse(
        /* [in] */ const String& contents,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SendEnvelope(
        /* [in] */ const String& contents,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SendEnvelopeWithStatus(
        /* [in] */ const String& contents,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI HandleCallSetupRequestFromSim(
        /* [in] */ Boolean accept,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetPreferredNetworkType(
        /* [in] */ Int32 networkType,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI GetPreferredNetworkType(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI GetNeighboringCids(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetLocationUpdates(
        /* [in] */ Boolean enable,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI GetSmscAddress(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI SetSmscAddress(
        /* [in] */ const String& address,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI ReportSmsMemoryStatus(
        /* [in] */ Boolean available,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI ReportStkServiceIsRunning(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI GetCdmaSubscriptionSource(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI GetGsmBroadcastConfig(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetGsmBroadcastConfig(
        /* [in] */ ArrayOf<ISmsBroadcastConfigInfo*>* config,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetGsmBroadcastActivation(
        /* [in] */ Boolean activate,
        /* [in] */ IMessage* response);

    // ***** Methods for CDMA support
    // @Override
    CARAPI GetDeviceIdentity(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI GetCDMASubscription(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetPhoneType(
        /* [in] */ Int32 phoneType);

    // @Override
    CARAPI QueryCdmaRoamingPreference(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetCdmaRoamingPreference(
        /* [in] */ Int32 cdmaRoamingType,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetCdmaSubscriptionSource(
        /* [in] */ Int32 cdmaSubscription,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI QueryTTYMode(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetTTYMode(
        /* [in] */ Int32 ttyMode,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SendCDMAFeatureCode(
        /* [in] */ const String& FeatureCode,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI GetCdmaBroadcastConfig(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetCdmaBroadcastConfig(
        /* [in] */ ArrayOf<ICdmaSmsBroadcastConfigInfo*>* configs,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetCdmaBroadcastActivation(
        /* [in] */ Boolean activate,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI ExitEmergencyCallbackMode(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SupplyIccPinForApp(
        /* [in] */ const String& pin,
        /* [in] */ const String& aid,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SupplyIccPukForApp(
        /* [in] */ const String& puk,
        /* [in] */ const String& newPin,
        /* [in] */ const String& aid,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SupplyIccPin2ForApp(
        /* [in] */ const String& pin2,
        /* [in] */ const String& aid,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SupplyIccPuk2ForApp(
        /* [in] */ const String& puk2,
        /* [in] */ const String& newPin2,
        /* [in] */ const String& aid,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI ChangeIccPinForApp(
        /* [in] */ const String& oldPin,
        /* [in] */ const String& newPin,
        /* [in] */ const String& aidPtr,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI ChangeIccPin2ForApp(
        /* [in] */ const String& oldPin2,
        /* [in] */ const String& newPin2,
        /* [in] */ const String& aidPtr,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI RequestIsimAuthentication(
        /* [in] */ const String& nonce,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI RequestIccSimAuthentication(
        /* [in] */ Int32 authContext,
        /* [in] */ const String& data,
        /* [in] */ const String& aid,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI GetVoiceRadioTechnology(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI GetCellInfoList(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI SetCellInfoListRate(
        /* [in] */ Int32 rateInMillis,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetInitialAttachApn(
        /* [in] */ const String& apn,
        /* [in] */ const String& protocol,
        /* [in] */ Int32 authType,
        /* [in] */ const String& username,
        /* [in] */ const String& password,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI SetDataProfile(
        /* [in] */ ArrayOf<IDataProfile*>* dps,
        /* [in] */ IMessage* result);

    // @Override
    CARAPI NvReadItem(
        /* [in] */ Int32 itemID,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI NvWriteItem(
        /* [in] */ Int32 itemID,
        /* [in] */ const String& itemValue,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI NvWriteCdmaPrl(
        /* [in] */ ArrayOf<Byte>* preferredRoamingList,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI NvResetConfig(
        /* [in] */ Int32 resetType,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI GetHardwareConfig(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI RequestShutdown(
        /* [in] */ IMessage* result);

    virtual CARAPI NeedsOldRilFeature(
        /* [in] */ const String& feature,
        /* [out] */ Boolean* result);
};

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONECOMMANDINTERFACE_H__
