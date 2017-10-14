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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMPHONE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMPHONE_H__

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telephony.h"
#include "_Elastos_Droid_Internal_Telephony_Gsm_CGSMPhone.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/telephony/PhoneBase.h"
#include "elastos/droid/os/RegistrantList.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::ICallForwardInfo;
using Elastos::Droid::Internal::Telephony::ICallTracker;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::IIccPhoneBookInterfaceManager;
using Elastos::Droid::Internal::Telephony::IPhoneDataActivityState;
using Elastos::Droid::Internal::Telephony::IPhoneNotifier;
using Elastos::Droid::Internal::Telephony::IPhoneSubInfo;
using Elastos::Droid::Internal::Telephony::IServiceStateTracker;
using Elastos::Droid::Internal::Telephony::IUUSInfo;
using Elastos::Droid::Internal::Telephony::PhoneBase;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState;
using Elastos::Droid::Internal::Telephony::PhoneConstantsDataState;
using Elastos::Droid::Internal::Telephony::Uicc::IIsimRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IIsimUiccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::RegistrantList;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

/**
 * {@hide}
 */
CarClass(CGSMPhone)
    , public PhoneBase
    , public IGSMPhone
{
private:
    // Create Cfu (Call forward unconditional) so that dialling number &
    // mOnComplete (Message object passed by client) can be packed &
    // given as a single Cfu object as user data to RIL.
    class Cfu
        : public Object
    {
    public:
        Cfu(
            /* [in] */ const String& cfNumber,
            /* [in] */ IMessage* onComplete);

    public:
        String mSetCfNumber;
        AutoPtr<IMessage> mOnComplete;
    };

public:
    friend class CGsmConnection;
    friend class CGsmMmiCode;

    CGSMPhone();

    virtual ~CGSMPhone();

    TO_STRING_IMPL("CGSMPhone")

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ IPhoneNotifier* notifier);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ IPhoneNotifier* notifier,
        /* [in] */ Boolean unitTestMode);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ IPhoneNotifier* notifier,
        /* [in] */ Int32 phoneId);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ IPhoneNotifier* notifier,
        /* [in] */ Boolean unitTestMode,
        /* [in] */ Int32 phoneId);

    CARAPI Dispose();

    CARAPI RemoveReferences();

    CARAPI GetServiceState(
        /* [out] */ IServiceState** result);

    CARAPI GetCellLocation(
        /* [out] */ ICellLocation** result);

    CARAPI GetState(
        /* [out] */ PhoneConstantsState* result);

    CARAPI GetPhoneType(
        /* [out] */ Int32* result);

    CARAPI GetServiceStateTracker(
        /* [out] */ IServiceStateTracker** result);

    CARAPI GetCallTracker(
        /* [out] */ ICallTracker** result);

    CARAPI GetCallForwardingIndicator(
        /* [out] */ Boolean* result);

    CARAPI GetPendingMmiCodes(
        /* [out] */ IList** result);

    CARAPI GetDataConnectionState(
        /* [in] */ const String& apnType,
        /* [out] */ PhoneConstantsDataState* result);

    CARAPI GetDataActivityState(
        /* [out] */ IPhoneDataActivityState* result);

    /**
     * Notify any interested party of a Phone state change
     * {@link com.android.internal.telephony.PhoneConstants.State}
     */
    /*package*/
    CARAPI NotifyPhoneStateChanged();

    /**
     * Notify registrants of a change in the call state. This notifies changes in
     * {@link com.android.internal.telephony.Call.State}. Use this when changes
     * in the precise call state are needed, else use notifyPhoneStateChanged.
     */
    /*package*/
    CARAPI NotifyPreciseCallStateChanged();

    CARAPI NotifyNewRingingConnection(
        /* [in] */ IConnection* c);

    /*package*/
    CARAPI NotifyDisconnect(
        /* [in] */ IConnection* cn);

    CARAPI NotifyUnknownConnection(
        /* [in] */ IConnection* cn);

    CARAPI NotifySuppServiceFailed(
        /* [in] */ IPhoneSuppService code);

    /*package*/
    CARAPI NotifyServiceStateChanged(
        /* [in] */ IServiceState* ss);

    /*package*/
    CARAPI NotifyLocationChanged();

    CARAPI NotifyCallForwardingIndicator();

    CARAPI SetSystemProperty(
        /* [in] */ const String& property,
        /* [in] */ const String& value);

    CARAPI RegisterForSuppServiceNotification(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForSuppServiceNotification(
        /* [in] */ IHandler* h);

    CARAPI RegisterForSimRecordsLoaded(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForSimRecordsLoaded(
        /* [in] */ IHandler* h);

    CARAPI AcceptCall(
        /* [in] */ Int32 videoState);

    CARAPI DeflectCall(
        /* [in] */ const String& number);

    CARAPI RejectCall();

    CARAPI SwitchHoldingAndActive();

    CARAPI CanConference(
        /* [out] */ Boolean* result);

    CARAPI CanDial(
        /* [out] */ Boolean* result);

    CARAPI Conference();

    CARAPI ClearDisconnected();

    CARAPI CanTransfer(
        /* [out] */ Boolean* result);

    CARAPI ExplicitCallTransfer();

    CARAPI GetForegroundCall(
        /* [out] */ ICall** result);

    CARAPI GetBackgroundCall(
        /* [out] */ ICall** result);

    CARAPI GetRingingCall(
        /* [out] */ ICall** result);

    CARAPI HandleInCallMmiCommands(
        /* [in] */ const String& dialString,
        /* [out] */ Boolean* result);

    CARAPI IsInCall(
        /* [out] */ Boolean* result);

    CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ Int32 videoState,
        /* [in] */ IBundle* extras,
        /* [out] */ IConnection** result);

    CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ Int32 videoState,
        /* [out] */ IConnection** result);

    CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ IUUSInfo* uusInfo,
        /* [in] */ Int32 videoState,
        /* [out] */ IConnection** result);

    CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ IUUSInfo* uusInfo,
        /* [in] */ Int32 videoState,
        /* [in] */ IBundle* extras,
        /* [out] */ IConnection** result);

    CARAPI AddParticipant(
        /* [in] */ const String& dialString);

    CARAPI HandlePinMmi(
        /* [in] */ const String& dialString,
        /* [out] */ Boolean* result);

    CARAPI SendUssdResponse(
        /* [in] */ const String& ussdMessge);

    CARAPI SendDtmf(
        /* [in] */ Char32 c);

    CARAPI StartDtmf(
        /* [in] */ Char32 c);

    CARAPI StopDtmf();

    using PhoneBase::SendBurstDtmf;

    CARAPI SendBurstDtmf(
        /* [in] */ const String& dtmfString);

    CARAPI SetRadioPower(
        /* [in] */ Boolean power);

    CARAPI GetVoiceMailNumber(
        /* [out] */ String* result);

    CARAPI GetVoiceMailAlphaTag(
        /* [out] */ String* result);

    CARAPI GetDeviceId(
        /* [out] */ String* result);

    CARAPI GetDeviceSvn(
        /* [out] */ String* result);

    CARAPI GetIsimRecords(
        /* [out] */ IIsimRecords** result);

    CARAPI GetImei(
        /* [out] */ String* result);

    CARAPI GetEsn(
        /* [out] */ String* result);

    CARAPI GetMeid(
        /* [out] */ String* result);

    CARAPI GetSubscriberId(
        /* [out] */ String* result);

    CARAPI GetGroupIdLevel1(
        /* [out] */ String* result);

    CARAPI GetLine1Number(
        /* [out] */ String* result);

    CARAPI GetMsisdn(
        /* [out] */ String* result);

    CARAPI GetLine1AlphaTag(
        /* [out] */ String* result);

    CARAPI SetLine1Number(
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& number,
        /* [in] */ IMessage* onComplete);

    CARAPI SetVoiceMailNumber(
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& voiceMailNumber,
        /* [in] */ IMessage* onComplete);

    CARAPI GetSystemProperty(
        /* [in] */ const String& property,
        /* [in] */ const String& defValue,
        /* [out] */ String* result);

    CARAPI UpdateDataConnectionTracker();

    CARAPI GetCallForwardingOption(
        /* [in] */ Int32 commandInterfaceCFReason,
        /* [in] */ IMessage* onComplete);

    CARAPI SetCallForwardingOption(
        /* [in] */ Int32 commandInterfaceCFAction,
        /* [in] */ Int32 commandInterfaceCFReason,
        /* [in] */ const String& dialingNumber,
        /* [in] */ Int32 timerSeconds,
        /* [in] */ IMessage* onComplete);

    CARAPI SetCallForwardingUncondTimerOption(
        /* [in] */ Int32 startHour,
        /* [in] */ Int32 startMinute,
        /* [in] */ Int32 endHour,
        /* [in] */ Int32 endMinute,
        /* [in] */ Int32 commandInterfaceCFAction,
        /* [in] */ Int32 commandInterfaceCFReason,
        /* [in] */ const String& dialingNumber,
        /* [in] */ IMessage* onComplete);

    CARAPI GetCallForwardingUncondTimerOption(
        /* [in] */ Int32 commandInterfaceCFReason,
        /* [in] */ IMessage* onComplete);

    CARAPI GetOutgoingCallerIdDisplay(
        /* [in] */ IMessage* onComplete);

    CARAPI SetOutgoingCallerIdDisplay(
        /* [in] */ Int32 commandInterfaceCLIRMode,
        /* [in] */ IMessage* onComplete);

    CARAPI GetCallWaiting(
        /* [in] */ IMessage* onComplete);

    CARAPI SetCallWaiting(
        /* [in] */ Boolean enable,
        /* [in] */ IMessage* onComplete);

    CARAPI GetAvailableNetworks(
        /* [in] */ IMessage* response);

    CARAPI GetNeighboringCids(
        /* [in] */ IMessage* response);

    CARAPI SetOnPostDialCharacter(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI SetUiTTYMode(
        /* [in] */ Int32 uiTtyMode,
        /* [in] */ IMessage* onComplete);

    CARAPI SetMute(
        /* [in] */ Boolean muted);

    CARAPI GetMute(
        /* [out] */ Boolean* result);

    CARAPI GetDataCallList(
        /* [in] */ IMessage* response);

    CARAPI UpdateServiceLocation();

    CARAPI EnableLocationUpdates();

    CARAPI DisableLocationUpdates();

    CARAPI GetDataRoamingEnabled(
        /* [out] */ Boolean* result);

    CARAPI SetDataRoamingEnabled(
        /* [in] */ Boolean enable);

    CARAPI GetDataEnabled(
        /* [out] */ Boolean* result);

    CARAPI SetDataEnabled(
        /* [in] */ Boolean enable);

    /**
     * Removes the given MMI from the pending list and notifies
     * registrants that it is complete.
     * @param mmi MMI that is done
     */
    /*package*/
    CARAPI OnMMIDone(
        /* [in] */ IGsmMmiCode* mmi);

    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    /**
     * Sets the "current" field in the telephony provider according to the SIM's operator
     *
     * @return true for success; false otherwise.
     */
    CARAPI UpdateCurrentCarrierInProvider(
        /* [out] */ Boolean* result);

    /**
     * Saves CLIR setting so that we can re-apply it as necessary
     * (in case the RIL resets it across reboots).
     */
    CARAPI SaveClirSetting(
        /* [in] */ Int32 commandInterfaceCLIRMode);

    /**
     * Retrieves the PhoneSubInfo of the GSMPhone
     */
    CARAPI GetPhoneSubInfo(
        /* [out] */ IPhoneSubInfo** result);

    /**
     * Retrieves the IccPhoneBookInterfaceManager of the GSMPhone
     */
    CARAPI GetIccPhoneBookInterfaceManager(
        /* [out] */ IIccPhoneBookInterfaceManager** result);

    /**
     * Activate or deactivate cell broadcast SMS.
     *
     * @param activate 0 = activate, 1 = deactivate
     * @param response Callback message is empty on completion
     */
    CARAPI ActivateCellBroadcastSms(
        /* [in] */ Int32 activate,
        /* [in] */ IMessage* response);

    /**
     * Query the current configuration of cdma cell broadcast SMS.
     *
     * @param response Callback message is empty on completion
     */
    CARAPI GetCellBroadcastSmsConfig(
        /* [in] */ IMessage* response);

    /**
     * Configure cdma cell broadcast SMS.
     *
     * @param response Callback message is empty on completion
     */
    CARAPI SetCellBroadcastSmsConfig(
        /* [in] */ ArrayOf<Int32>* configValuesArray,
        /* [in] */ IMessage* response);

    CARAPI IsCspPlmnEnabled(
        /* [out] */ Boolean* result);

    CARAPI IsManualNetSelAllowed(
        /* [out] */ Boolean* result);

    CARAPI ExitEmergencyCallbackMode();

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI SetOperatorBrandOverride(
        /* [in] */ const String& brand,
        /* [out] */ Boolean* result);

    /**
     * @return operator numeric.
     */
    CARAPI GetOperatorNumeric(
        /* [out] */ String* result);

    CARAPI RegisterForAllDataDisconnected(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForAllDataDisconnected(
        /* [in] */ IHandler* h);

    CARAPI SetInternalDataEnabled(
        /* [in] */ Boolean enable,
        /* [in] */ IMessage* onCompleteMsg);

    CARAPI SetInternalDataEnabledFlag(
        /* [in] */ Boolean enable,
        /* [out] */ Boolean* result);

    CARAPI NotifyEcbmTimerReset(
        /* [in] */ Boolean flag);

    /**
     * Registration point for Ecm timer reset
     *
     * @param h handler to notify
     * @param what User-defined message code
     * @param obj placed in Message.obj
     */
    CARAPI RegisterForEcmTimerReset(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForEcmTimerReset(
        /* [in] */ IHandler* h);

    CARAPI ResetSubSpecifics();

     /**
     * Sets the SIM voice message waiting indicator records.
     * @param line GSM Subscriber Profile Number, one-based. Only '1' is supported
     * @param countWaiting The number of messages waiting, if known. Use
     *                     -1 to indicate that an unknown number of
     *                      messages are waiting
     */
    CARAPI SetVoiceMessageWaiting(
        /* [in] */ Int32 line,
        /* [in] */ Int32 countWaiting);

    CARAPI GetCallBarringOption(
        /* [in] */ const String& facility,
        /* [in] */ const String& password,
        /* [in] */ IMessage* onComplete);

    CARAPI SetCallBarringOption(
        /* [in] */ const String& facility,
        /* [in] */ Boolean lockState,
        /* [in] */ const String& password,
        /* [in] */ IMessage* onComplete);

    CARAPI RequestChangeCbPsw(
        /* [in] */ const String& facility,
        /* [in] */ const String& oldPwd,
        /* [in] */ const String& newPwd,
        /* [in] */ IMessage* result);

protected:
    CARAPI_(void) SetProperties();

    CARAPI DialInternal(
        /* [in] */ const String& dialString,
        /* [in] */ IUUSInfo* uusInfo,
        /* [in] */ Int32 videoState,
        /* [out] */ IConnection** connection);

    CARAPI_(Boolean) IsCfEnable(
        /* [in] */ Int32 action);

    /**
     * This method stores the CF_ENABLED flag in preferences
     * @param enabled
     */
    CARAPI_(void) SetCallForwardingPreference(
        /* [in] */ Boolean enabled);

    CARAPI_(Boolean) GetCallForwardingPreference();

    /**
     * Make sure the network knows our preferred setting.
     */
    CARAPI_(void) SyncClirSetting();

    CARAPI_(AutoPtr<IUiccCardApplication>) GetUiccCardApplication();

    // Set the Card into the Phone Book.
    CARAPI_(void) SetCardInPhoneBook();

    CARAPI OnUpdateIccAvailability();

    CARAPI_(void) Log(
        /* [in] */ const String& s);

private:
    CARAPI_(void) OnSubscriptionActivated();

    CARAPI_(void) OnSubscriptionDeactivated();

    // pending voice mail count updated after phone creation
    CARAPI_(void) UpdateVoiceMail();

    CARAPI_(Boolean) HandleCallDeflectionIncallSupplementaryService(
        /* [in] */ const String& dialString);

    CARAPI_(Boolean) HandleCallWaitingIncallSupplementaryService(
        /* [in] */ const String& dialString);

    CARAPI_(Boolean) HandleCallHoldIncallSupplementaryService(
        /* [in] */ const String& dialString);

    CARAPI_(Boolean) HandleMultipartyIncallSupplementaryService(
        /* [in] */ const String& dialString);

    CARAPI_(Boolean) HandleEctIncallSupplementaryService(
        /* [in] */ const String& dialString);

    CARAPI_(Boolean) HandleCcbsIncallSupplementaryService(
        /* [in] */ const String& dialString);

    CARAPI_(void) StoreVoiceMailNumber(
        /* [in] */ const String& number);

    CARAPI_(String) GetSimImsi();

    CARAPI_(void) SetSimImsi(
        /* [in] */ const String& imsi);

    CARAPI_(Boolean) IsValidCommandInterfaceCFReason (
        /* [in] */ Int32 commandInterfaceCFReason);

    CARAPI_(Boolean) IsValidCommandInterfaceCFAction (
        /* [in] */ Int32 commandInterfaceCFAction);

    /**
     * Used to check if Call Forwarding status is present on sim card. If not, a message is
     * sent so we can check if the CF status is stored as a Shared Preference.
     */
    CARAPI_(void) UpdateCallForwardStatus();

    CARAPI_(void) OnNetworkInitiatedUssd(
        /* [in] */ IGsmMmiCode* mmi);

    /** ussdMode is one of CommandsInterface.USSD_MODE_* */
    CARAPI_(void) OnIncomingUSSD (
        /* [in] */ Int32 ussdMode,
        /* [in] */ const String& ussdMessage);

    CARAPI_(void) ProcessIccRecordEvents(
        /* [in] */ Int32 eventCode);

    CARAPI_(void) HandleCfuQueryResult(
        /* [in] */ ArrayOf<ICallForwardInfo*>* infos);

    CARAPI_(void) RegisterForSimRecordEvents();

    CARAPI_(void) UnregisterForSimRecordEvents();

    /** gets the voice mail count from preferences */
    CARAPI_(Int32) GetStoredVoiceMessageCount();

    CARAPI_(Boolean) IsValidFacilityString(
        /* [in] */ const String& facility);

public:
    // NOTE that LOG_TAG here is "GSM", which means that log messages
    // from this file will go into the radio log rather than the main
    // log.  (Use "adb logcat -b radio" to see them.)
    static const String TAG; // = "GSMPhone";

    // Instance Variables
    AutoPtr<IGsmCallTracker> mCT;
    AutoPtr<IGsmServiceStateTracker> mSST;
    AutoPtr<IArrayList> mPendingMMIs; // = new ArrayList<GsmMmiCode>();
    AutoPtr<IPhoneSubInfo> mSubInfo;

    AutoPtr<IRegistrant> mPostDialHandler;

    /** List of Registrants to receive Supplementary Service Notifications. */
    AutoPtr<RegistrantList> mSsnRegistrants; // = new RegistrantList();

protected:
    AutoPtr<ISimPhoneBookInterfaceManager> mSimPhoneBookIntManager;

private:
    static const Boolean LOCAL_DEBUG = TRUE;
    static const Boolean VDBG = FALSE; /* STOPSHIP if true */
    // Event constant for checking if Call Forwarding is enabled
    static const Int32 CHECK_CALLFORWARDING_STATUS = 75;

    // mEcmTimerResetRegistrants are informed after Ecm timer is canceled or re-started
    AutoPtr<RegistrantList> mEcmTimerResetRegistrants; // = new RegistrantList();

    String mImei;
    String mImeiSv;
    String mVmNumber;

    AutoPtr<IIsimUiccRecords> mIsimUiccRecords;
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMPHONE_H__
