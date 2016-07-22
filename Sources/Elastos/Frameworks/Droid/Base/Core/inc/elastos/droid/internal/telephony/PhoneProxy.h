
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_PHONEPROXY_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_PHONEPROXY_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Telephony.h"
#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Internal::Telephony::ImsPhone::IImsPhone;
// using Elastos::Droid::Internal::Telephony::Test::ISimulatedRadioControl;
using Elastos::Droid::Internal::Telephony::Uicc::IIccCardProxy;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Internal::Telephony::Uicc::IIsimRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUsimServiceTable;

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class PhoneProxy
    : public Handler
    , public IPhone
    , public IPhoneProxy
{
public:
    TO_STRING_IMPL("PhoneProxy")

    CAR_INTERFACE_DECL()

    //***** Class Methods
    PhoneProxy();

    CARAPI constructor(
        /* [in] */ IPhoneBase* phone);

    //@Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    CARAPI GetIccSmsInterfaceManager(
        /* [out] */ IIccSmsInterfaceManager** result);

    CARAPI GetPhoneSubInfoProxy(
        /* [out] */ IPhoneSubInfoProxy** result);

    CARAPI GetIccPhoneBookInterfaceManagerProxy(
        /* [out] */ IIccPhoneBookInterfaceManagerProxy** result);

    CARAPI GetIccFileHandler(
        /* [out] */ IIccFileHandler** result);

    CARAPI IsImsVtCallPresent(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI UpdatePhoneObject(
        /* [in] */ Int32 voiceRadioTech);

    //@Override
    CARAPI GetServiceState(
        /* [out] */ IServiceState** result);

    //@Override
    CARAPI GetCellLocation(
        /* [out] */ ICellLocation** result);

    /**
     * @return all available cell information or NULL if none.
     */
    //@Override
    CARAPI GetAllCellInfo(
        /* [out] */ IList** result);

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SetCellInfoListRate(
        /* [in] */ Int32 rateInMillis);

    //@Override
    CARAPI GetDataConnectionState(
        /* [out] */ PhoneConstantsDataState* result);

    //@Override
    CARAPI GetDataConnectionState(
        /* [in] */ const String& apnType,
        /* [out] */ PhoneConstantsDataState* result);

    //@Override
    CARAPI GetDataActivityState(
        /* [out] */ IPhoneDataActivityState* result);

    //@Override
    CARAPI GetContext(
        /* [out] */ IContext** result);

    //@Override
    CARAPI DisableDnsCheck(
        /* [in] */ Boolean b);

    //@Override
    CARAPI IsDnsCheckDisabled(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetState(
        /* [out] */ PhoneConstantsState* result);

    //@Override
    CARAPI GetPhoneName(
        /* [out] */ String* result);

    //@Override
    CARAPI GetPhoneType(
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetActiveApnTypes(
        /* [out] */ ArrayOf<String>** result);

    //@Override
    CARAPI GetActiveApnHost(
        /* [in] */ const String& apnType,
        /* [out] */ String* result);

    //@Override
    CARAPI GetLinkProperties(
        /* [in] */ const String& apnType,
        /* [out] */ ILinkProperties** result);

    //@Override
    CARAPI GetNetworkCapabilities(
        /* [in] */ const String& apnType,
        /* [out] */ INetworkCapabilities** result);

    //@Override
    CARAPI GetSignalStrength(
        /* [out] */ ISignalStrength** result);

    //@Override
    CARAPI RegisterForUnknownConnection(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForUnknownConnection(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForHandoverStateChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForHandoverStateChanged(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForPreciseCallStateChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForPreciseCallStateChanged(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForNewRingingConnection(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForNewRingingConnection(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForVideoCapabilityChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForVideoCapabilityChanged(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForIncomingRing(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForIncomingRing(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForDisconnect(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForDisconnect(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForMmiInitiate(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForMmiInitiate(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForMmiComplete(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForMmiComplete(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI GetPendingMmiCodes(
        /* [out] */ IList** result);

    //@Override
    CARAPI SendUssdResponse(
        /* [in] */ const String& ussdMessge);

    //@Override
    CARAPI RegisterForServiceStateChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForServiceStateChanged(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForSuppServiceNotification(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForSuppServiceNotification(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForSuppServiceFailed(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForSuppServiceFailed(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForInCallVoicePrivacyOn(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForInCallVoicePrivacyOn(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForInCallVoicePrivacyOff(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForInCallVoicePrivacyOff(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForCdmaOtaStatusChange(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForCdmaOtaStatusChange(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForSubscriptionInfoReady(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForSubscriptionInfoReady(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForEcmTimerReset(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForEcmTimerReset(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForRingbackTone(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForRingbackTone(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForOnHoldTone(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForOnHoldTone(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForResendIncallMute(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForResendIncallMute(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForSimRecordsLoaded(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForSimRecordsLoaded(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI GetIccRecordsLoaded(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetIccCard(
        /* [out] */ IIccCard** result);

    //@Override
    CARAPI AcceptCall(
        /* [in] */ Int32 videoState);

    //@Override
    CARAPI DeflectCall(
        /* [in] */ const String& number);

    //@Override
    CARAPI RejectCall();

    //@Override
    CARAPI SwitchHoldingAndActive();

    //@Override
    CARAPI CanConference(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI Conference();

    //@Override
    CARAPI EnableEnhancedVoicePrivacy(
        /* [in] */ Boolean enable,
        /* [in] */ IMessage* onComplete);

    //@Override
    CARAPI GetEnhancedVoicePrivacy(
        /* [in] */ IMessage* onComplete);

    //@Override
    CARAPI CanTransfer(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI ExplicitCallTransfer();

    //@Override
    CARAPI ClearDisconnected();

    //@Override
    CARAPI GetForegroundCall(
        /* [out] */ ICall** result);

    //@Override
    CARAPI GetBackgroundCall(
        /* [out] */ ICall** result);

    //@Override
    CARAPI GetRingingCall(
        /* [out] */ ICall** result);

    //@Override
    CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ Int32 videoState,
        /* [out] */ IConnection** result);

    //@Override
    CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ IUUSInfo* uusInfo,
        /* [in] */ Int32 videoState,
        /* [out] */ IConnection** result);

    //@Override
    CARAPI AddParticipant(
        /* [in] */ const String& dialString);

    //@Override
    CARAPI HandlePinMmi(
        /* [in] */ const String& dialString,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI HandleInCallMmiCommands(
        /* [in] */ const String& command,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI SendDtmf(
        /* [in] */ Char32 c);

    //@Override
    CARAPI StartDtmf(
        /* [in] */ Char32 c);

    //@Override
    CARAPI StopDtmf();

    //@Override
    CARAPI SetRadioPower(
        /* [in] */ Boolean power);

    //@Override
    CARAPI GetMessageWaitingIndicator(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetCallForwardingIndicator(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetLine1Number(
        /* [out] */ String* result);

    //@Override
    CARAPI GetCdmaMin(
        /* [out] */ String* result);

    //@Override
    CARAPI IsMinInfoReady(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetCdmaPrlVersion(
        /* [out] */ String* result);

    //@Override
    CARAPI GetLine1AlphaTag(
        /* [out] */ String* result);

    //@Override
    CARAPI SetLine1Number(
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& number,
        /* [in] */ IMessage* onComplete);

    //@Override
    CARAPI GetVoiceMailNumber(
        /* [out] */ String* result);

     /** @hide */
    //@Override
    CARAPI GetVoiceMessageCount(
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetVoiceMailAlphaTag(
        /* [out] */ String* result);

    //@Override
    CARAPI SetVoiceMailNumber(
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& voiceMailNumber,
        /* [in] */ IMessage* onComplete);

    //@Override
    CARAPI GetCallForwardingOption(
        /* [in] */ Int32 commandInterfaceCFReason,
        /* [in] */ IMessage* onComplete);

    //@Override
    CARAPI GetCallForwardingUncondTimerOption(
        /* [in] */ Int32 commandInterfaceCFReason,
        /* [in] */ IMessage* onComplete);

    //@Override
    CARAPI SetCallForwardingOption(
        /* [in] */ Int32 commandInterfaceCFReason,
        /* [in] */ Int32 commandInterfaceCFAction,
        /* [in] */ const String& dialingNumber,
        /* [in] */ Int32 timerSeconds,
        /* [in] */ IMessage* onComplete);

    //@Override
    CARAPI SetCallForwardingUncondTimerOption(
        /* [in] */ Int32 startHour,
        /* [in] */ Int32 startMinute,
        /* [in] */ Int32 endHour,
        /* [in] */ Int32 endMinute,
        /* [in] */ Int32 commandInterfaceCFReason,
        /* [in] */ Int32 commandInterfaceCFAction,
        /* [in] */ const String& dialingNumber,
        /* [in] */ IMessage* onComplete);

    //@Override
    CARAPI GetOutgoingCallerIdDisplay(
        /* [in] */ IMessage* onComplete);

    //@Override
    CARAPI SetOutgoingCallerIdDisplay(
        /* [in] */ Int32 commandInterfaceCLIRMode,
        /* [in] */ IMessage* onComplete);

    //@Override
    CARAPI GetCallWaiting(
        /* [in] */ IMessage* onComplete);

    //@Override
    CARAPI SetCallWaiting(
        /* [in] */ Boolean enable,
        /* [in] */ IMessage* onComplete);

    //@Override
    CARAPI GetAvailableNetworks(
        /* [in] */ IMessage* response);

    //@Override
    CARAPI SetNetworkSelectionModeAutomatic(
        /* [in] */ IMessage* response);

    //@Override
    CARAPI SelectNetworkManually(
        /* [in] */ IOperatorInfo* network,
        /* [in] */ IMessage* response);

    //@Override
    CARAPI SetPreferredNetworkType(
        /* [in] */ Int32 networkType,
        /* [in] */ IMessage* response);

    //@Override
    CARAPI GetPreferredNetworkType(
        /* [in] */ IMessage* response);

    //@Override
    CARAPI GetNeighboringCids(
        /* [in] */ IMessage* response);

    //@Override
    CARAPI SetOnPostDialCharacter(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI SetMute(
        /* [in] */ Boolean muted);

    //@Override
    CARAPI GetMute(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI SetEchoSuppressionEnabled();

    //@Override
    CARAPI InvokeOemRilRequestRaw(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ IMessage* response);

    //@Override
    CARAPI InvokeOemRilRequestStrings(
        /* [in] */ ArrayOf<String>* strings,
        /* [in] */ IMessage* response);

    //@Override
    CARAPI GetDataCallList(
        /* [in] */ IMessage* response);

    //@Override
    CARAPI UpdateServiceLocation();

    //@Override
    CARAPI EnableLocationUpdates();

    //@Override
    CARAPI DisableLocationUpdates();

    //@Override
    CARAPI SetUnitTestMode(
        /* [in] */ Boolean f);

    //@Override
    CARAPI GetUnitTestMode(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI SetBandMode(
        /* [in] */ Int32 bandMode,
        /* [in] */ IMessage* response);

    //@Override
    CARAPI QueryAvailableBandMode(
        /* [in] */ IMessage* response);

    //@Override
    CARAPI GetDataRoamingEnabled(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI SetDataRoamingEnabled(
        /* [in] */ Boolean enable);

    //@Override
    CARAPI GetDataEnabled(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI SetDataEnabled(
        /* [in] */ Boolean enable);

    //@Override
    CARAPI QueryCdmaRoamingPreference(
        /* [in] */ IMessage* response);

    //@Override
    CARAPI SetCdmaRoamingPreference(
        /* [in] */ Int32 cdmaRoamingType,
        /* [in] */ IMessage* response);

    //@Override
    CARAPI SetCdmaSubscription(
        /* [in] */ Int32 cdmaSubscriptionType,
        /* [in] */ IMessage* response);

    //@Override
    // CARAPI GetSimulatedRadioControl(
    //      [out]  ISimulatedRadioControl** result);

    //@Override
    CARAPI IsDataConnectivityPossible(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsOnDemandDataPossible(
        /* [in] */ const String& apnType,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsDataConnectivityPossible(
        /* [in] */ const String& apnType,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetDeviceId(
        /* [out] */ String* result);

    //@Override
    CARAPI GetDeviceSvn(
        /* [out] */ String* result);

    //@Override
    CARAPI GetSubscriberId(
        /* [out] */ String* result);

    //@Override
    CARAPI GetGroupIdLevel1(
        /* [out] */ String* result);

    //@Override
    CARAPI GetIccSerialNumber(
        /* [out] */ String* result);

    //@Override
    CARAPI GetEsn(
        /* [out] */ String* result);

    //@Override
    CARAPI GetMeid(
        /* [out] */ String* result);

    //@Override
    CARAPI GetMsisdn(
        /* [out] */ String* result);

    //@Override
    CARAPI GetImei(
        /* [out] */ String* result);

    //@Override
    CARAPI GetPhoneSubInfo(
        /* [out] */ IPhoneSubInfo** result);

    //@Override
    CARAPI GetIccPhoneBookInterfaceManager(
        /* [out] */ IIccPhoneBookInterfaceManager** result);

    //@Override
    CARAPI SetUiTTYMode(
        /* [in] */ Int32 uiTtyMode,
        /* [in] */ IMessage* onComplete);

    //@Override
    CARAPI SetTTYMode(
        /* [in] */ Int32 ttyMode,
        /* [in] */ IMessage* onComplete);

    //@Override
    CARAPI QueryTTYMode(
        /* [in] */ IMessage* onComplete);

    //@Override
    CARAPI ActivateCellBroadcastSms(
        /* [in] */ Int32 activate,
        /* [in] */ IMessage* response);

    //@Override
    CARAPI GetCellBroadcastSmsConfig(
        /* [in] */ IMessage* response);

    //@Override
    CARAPI SetCellBroadcastSmsConfig(
        /* [in] */ ArrayOf<Int32>* configValuesArray,
        /* [in] */ IMessage* response);

    //@Override
    CARAPI NotifyDataActivity();

    //@Override
    CARAPI GetSmscAddress(
        /* [in] */ IMessage* result);

    //@Override
    CARAPI SetSmscAddress(
        /* [in] */ const String& address,
        /* [in] */ IMessage* result);

    //@Override
    CARAPI GetCdmaEriIconIndex(
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetCdmaEriText(
        /* [out] */ String* result);

    //@Override
    CARAPI GetCdmaEriIconMode(
        /* [out] */ Int32* result);

    CARAPI GetActivePhone(
        /* [out] */ IPhone** result);

    //@Override
    CARAPI SendBurstDtmf(
        /* [in] */ const String& dtmfString,
        /* [in] */ Int32 on,
        /* [in] */ Int32 off,
        /* [in] */ IMessage* onComplete);

    //@Override
    CARAPI ExitEmergencyCallbackMode();

    //@Override
    CARAPI NeedsOtaServiceProvisioning(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsOtaSpNumber(
        /* [in] */ const String& dialStr,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI RegisterForCallWaiting(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForCallWaiting(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForSignalInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForSignalInfo(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForDisplayInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForDisplayInfo(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForNumberInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForNumberInfo(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForRedirectedNumberInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForRedirectedNumberInfo(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForLineControlInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForLineControlInfo(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterFoT53ClirlInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForT53ClirInfo(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI RegisterForT53AudioControlInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnregisterForT53AudioControlInfo(
        /* [in] */ IHandler* h);

    //@Override
    CARAPI SetOnEcbModeExitResponse(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //@Override
    CARAPI UnsetOnEcbModeExitResponse(
        /* [in] */ IHandler* h);

    CARAPI IsManualNetSelAllowed(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsCspPlmnEnabled(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetIsimRecords(
        /* [out] */ IIsimRecords** result);

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI GetLteOnCdmaMode(
        /* [out] */ Int32* result);

    /**
     * {@hide}
     */
    //@Override
    CARAPI GetLteOnGsmMode(
        /* [out] */ Int32* result);

    //@Override
    CARAPI SetVoiceMessageWaiting(
        /* [in] */ Int32 line,
        /* [in] */ Int32 countWaiting);

    //@Override
    CARAPI GetUsimServiceTable(
        /* [out] */ IUsimServiceTable** result);

    //@Override
    CARAPI GetUiccCard(
        /* [out] */ IUiccCard** result);

    //@Override
    CARAPI NvReadItem(
        /* [in] */ Int32 itemID,
        /* [in] */ IMessage* response);

    //@Override
    CARAPI NvWriteItem(
        /* [in] */ Int32 itemID,
        /* [in] */ const String& itemValue,
        /* [in] */ IMessage* response);

    //@Override
    CARAPI NvWriteCdmaPrl(
        /* [in] */ ArrayOf<Byte>* preferredRoamingList,
        /* [in] */ IMessage* response);

    //@Override
    CARAPI NvResetConfig(
        /* [in] */ Int32 resetType,
        /* [in] */ IMessage* response);

    //@Override
    CARAPI Dispose();

    //@Override
    CARAPI RemoveReferences();

    CARAPI UpdateCurrentCarrierInProvider(
        /* [out] */ Boolean* result);

    CARAPI UpdateDataConnectionTracker();

    CARAPI SetInternalDataEnabled(
        /* [in] */ Boolean enable);

    CARAPI SetInternalDataEnabledFlag(
        /* [in] */ Boolean enable,
        /* [out] */ Boolean* result);

    CARAPI SetInternalDataEnabled(
        /* [in] */ Boolean enable,
        /* [in] */ IMessage* onCompleteMsg);

    CARAPI RegisterForAllDataDisconnected(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForAllDataDisconnected(
        /* [in] */ IHandler* h);

    CARAPI GetSubId(
        /* [out] */ Int64* result);

    CARAPI GetPhoneId(
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetPcscfAddress(
        /* [in] */ const String& apnType,
        /* [out] */ ArrayOf<String>** result);

    //@Override
    CARAPI SetImsRegistrationState(
        /* [in] */ Boolean registered);

    //@Override
    CARAPI GetImsPhone(
        /* [out] */ IPhone** result);

    //@Override
    CARAPI RelinquishOwnershipOfImsPhone(
        /* [out] */ IImsPhone** result);

    //@Override
    CARAPI AcquireOwnershipOfImsPhone(
        /* [in] */ IImsPhone* imsPhone);

    //@Override
    CARAPI GetVoicePhoneServiceState(
        /* [out] */ Int32* result);

    //@Override
    CARAPI SetOperatorBrandOverride(
        /* [in] */ const String& brand,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsRadioAvailable(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI ShutdownRadio();

    CARAPI GetCallBarringOption(
        /* [in] */ const String& facility,
        /* [in] */ const String& password,
        /* [in] */ IMessage* onComplete);

    //@Override
    CARAPI SetCallBarringOption(
        /* [in] */ const String& facility,
        /* [in] */ Boolean lockState,
        /* [in] */ const String& password,
        /* [in] */ IMessage* onComplete);

    //@Override
    CARAPI RequestChangeCbPsw(
        /* [in] */ const String& facility,
        /* [in] */ const String& oldPwd,
        /* [in] */ const String& newPwd,
        /* [in] */ IMessage* result);

    //@Override
    CARAPI SetLocalCallHold(
        /* [in] */ Int32 lchStatus);

    //@Override
    CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ Int32 videoState,
        /* [in] */ IBundle* extras,
        /* [out] */ IConnection** result);

private:
    static CARAPI_(void) Logd(
        /* [in] */ String msg);

    CARAPI_(void) Loge(
        /* [in] */ String msg);

    CARAPI_(void) PhoneObjectUpdater(
        /* [in] */ Int32 newVoiceRadioTech);

    CARAPI_(void) DeleteAndCreatePhone(
        /* [in] */ Int32 newVoiceRadioTech);

public:
    static Object lockForRadioTechnologyChange;

private:
    AutoPtr<IPhone> mActivePhone;
    AutoPtr<ICommandsInterface> mCommandsInterface;
    AutoPtr<IIccSmsInterfaceManager> mIccSmsInterfaceManager;
    AutoPtr<IIccPhoneBookInterfaceManagerProxy> mIccPhoneBookInterfaceManagerProxy;
    AutoPtr<IPhoneSubInfoProxy> mPhoneSubInfoProxy;
    AutoPtr<IIccCardProxy> mIccCardProxy;

    Boolean mResetModemOnRadioTechnologyChange;

    Int32 mRilVersion;

    static const Int32 EVENT_VOICE_RADIO_TECH_CHANGED;
    static const Int32 EVENT_RADIO_ON;
    static const Int32 EVENT_REQUEST_VOICE_RADIO_TECH_DONE;
    static const Int32 EVENT_RIL_CONNECTED;
    static const Int32 EVENT_UPDATE_PHONE_OBJECT;
    static const Int32 EVENT_SIM_RECORDS_LOADED;

    Int32 mPhoneId;

    static const String LOGTAG;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_PHONEPROXY_H__
