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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_PHONEBASE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_PHONEBASE_H__

#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/SmsStorageMonitor.h"
#include "elastos/droid/internal/telephony/SmsUsageMonitor.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/RegistrantList.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Internal::Telephony::DataConnection::IDcTrackerBase;
using Elastos::Droid::Internal::Telephony::ImsPhone::IImsPhone;
using Elastos::Droid::Internal::Telephony::Uicc::AppType;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IIsimRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::Uicc::IUsimServiceTable;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Os::RegistrantList;
using Elastos::Droid::Telephony::IDataConnectionRealTimeInfo;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Telephony::IVoLteServiceState;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IList;
using Elastos::Utility::Concurrent::Atomic::IAtomicReference;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * (<em>Not for SDK use</em>)
 * A base implementation for the com.android.internal.telephony.Phone interface.
 *
 * Note that implementations of Phone.java are expected to be used
 * from a single application thread. This should be the same thread that
 * originally called PhoneFactory to obtain the interface.
 *
 *  {@hide}
 *
 */
class PhoneBase
    : public Handler
    , public IPhone
    , public IPhoneBase
{
    friend class SMSDispatcher;
protected:
    /**
     * Small container class used to hold information relevant to
     * the carrier selection process. mOperatorNumeric can be ""
     * if we are looking for automatic selection. mOperatorAlphaLong is the
     * corresponding operator name.
     */
    class NetworkSelectMessage
        : public Object
        , public IPhoneBaseNetworkSelectMessage
    {
    public:
        CAR_INTERFACE_DECL()

    public:
        AutoPtr<IMessage> mMessage;
        String mOperatorNumeric;
        String mOperatorAlphaLong;
    };

private:
    class ImsIntentReceiver: public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("PhoneBase::ImsIntentReceiver")

        ImsIntentReceiver(
            /* [in] */ PhoneBase* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        PhoneBase* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    PhoneBase();

    virtual ~PhoneBase();

    // @Override
    CARAPI GetPhoneName(
        /* [out] */ String* result);

    /**
     * Return the ActionDetached string. When this action is received by components
     * they are to simulate detaching from the network.
     *
     * @return com.android.internal.telephony.{mName}.action_detached
     *          {mName} is GSM, CDMA ...
     */
    CARAPI GetActionDetached(
        /* [out] */ String* result);

    /**
     * Return the ActionAttached string. When this action is received by components
     * they are to simulate attaching to the network.
     *
     * @return com.android.internal.telephony.{mName}.action_detached
     *          {mName} is GSM, CDMA ...
     */
    CARAPI GetActionAttached(
        /* [out] */ String* result);

    /**
     * Set a system property, unless we're in unit test mode
     */
    // CAF_MSIM TODO this need to be replated with TelephonyManager API ?
    CARAPI SetSystemProperty(
        /* [in] */ const String& property,
        /* [in] */ const String& value);

    /**
     * Set a system property, unless we're in unit test mode
     */
    // CAF_MSIM TODO this need to be replated with TelephonyManager API ?
    CARAPI GetSystemProperty(
        /* [in] */ const String& property,
        /* [in] */ const String& defValue,
        /* [out] */ String* result);

    /**
     * Constructs a PhoneBase in normal (non-unit test) mode.
     *
     * @param notifier An instance of DefaultPhoneNotifier,
     * @param context IContext object from hosting application
     * unless unit testing.
     * @param ci the ICommandsInterface
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ IPhoneNotifier* notifier,
        /* [in] */ IContext* context,
        /* [in] */ ICommandsInterface* ci);

    /**
     * Constructs a PhoneBase in normal (non-unit test) mode.
     *
     * @param notifier An instance of DefaultPhoneNotifier,
     * @param context IContext object from hosting application
     * unless unit testing.
     * @param ci is ICommandsInterface
     * @param unitTestMode when TRUE, prevents notifications
     * of state change events
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ IPhoneNotifier* notifier,
        /* [in] */ IContext* context,
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ Boolean unitTestMode);

    /**
     * Constructs a PhoneBase in normal (non-unit test) mode.
     *
     * @param notifier An instance of DefaultPhoneNotifier,
     * @param context IContext object from hosting application
     * unless unit testing.
     * @param ci is ICommandsInterface
     * @param unitTestMode when TRUE, prevents notifications
     * of state change events
     * @param subscription is current phone subscription
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ IPhoneNotifier* notifier,
        /* [in] */ IContext* context,
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ Boolean unitTestMode,
        /* [in] */ Int32 phoneId);

    // @Override
    virtual CARAPI Dispose();

    // @Override
    virtual CARAPI RemoveReferences();

    /**
     * When overridden the derived class needs to call
     * super.handleMessage(msg) so this method has a
     * a chance to process the message.
     *
     * @param msg
     */
    // @Override
    virtual CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI GetContext(
        /* [out] */ IContext** ctx);

    /**
     * Disables the DNS check (i.e., allows "0.0.0.0").
     * Useful for lab testing environment.
     * @param b TRUE disables the check, FALSE enables.
     */
    // @Override
    virtual CARAPI DisableDnsCheck(
        /* [in] */ Boolean b);

    /**
     * Returns TRUE if the DNS check is currently disabled.
     */
    // @Override
    virtual CARAPI IsDnsCheckDisabled(
        /* [out] */ Boolean* result);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI RegisterForPreciseCallStateChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI UnregisterForPreciseCallStateChanged(
        /* [in] */ IHandler* h);

    // @Override
    virtual CARAPI RegisterForHandoverStateChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    virtual CARAPI UnregisterForHandoverStateChanged(
        /* [in] */ IHandler* h);

    /**
     * Subclasses of Phone probably want to replace this with a
     * version scoped to their packages
     */
    CARAPI NotifyHandoverStateChanged(
        /* [in] */ IConnection* cn);

    CARAPI MigrateFrom(
        /* [in] */ IPhoneBase* from);

    CARAPI Migrate(
        /* [in] */ IRegistrantList* to,
        /* [in] */ IRegistrantList* from);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI RegisterForUnknownConnection(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI UnregisterForUnknownConnection(
        /* [in] */ IHandler* h);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI RegisterForNewRingingConnection(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI UnregisterForNewRingingConnection(
        /* [in] */ IHandler* h);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI RegisterForVideoCapabilityChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI UnregisterForVideoCapabilityChanged(
        /* [in] */ IHandler* h);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI RegisterForInCallVoicePrivacyOn(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI UnregisterForInCallVoicePrivacyOn(
        /* [in] */ IHandler* h);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI RegisterForInCallVoicePrivacyOff(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI UnregisterForInCallVoicePrivacyOff(
        /* [in] */ IHandler* h);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI RegisterForIncomingRing(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI UnregisterForIncomingRing(
        /* [in] */ IHandler* h);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI RegisterForDisconnect(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI UnregisterForDisconnect(
        /* [in] */ IHandler* h);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI RegisterForSuppServiceFailed(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI UnregisterForSuppServiceFailed(
        /* [in] */ IHandler* h);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI RegisterForMmiInitiate(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI UnregisterForMmiInitiate(
        /* [in] */ IHandler* h);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI RegisterForMmiComplete(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI UnregisterForMmiComplete(
        /* [in] */ IHandler* h);

    CARAPI RegisterForSimRecordsLoaded(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForSimRecordsLoaded(
        /* [in] */ IHandler* h);

    // @Override
    virtual CARAPI SetNetworkSelectionModeAutomatic(
        /* [in] */ IMessage* response);

    // @Override
    virtual CARAPI SelectNetworkManually(
        /* [in] */ IOperatorInfo* network,
        /* [in] */ IMessage* response);

    /**
     * Method to restore the previously saved operator id, or reset to
     * automatic selection, all depending upon the value in the shared
     * preferences.
     */
    CARAPI RestoreSavedNetworkSelection(
        /* [in] */ IMessage* response);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI SetUnitTestMode(
        /* [in] */ Boolean f);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI GetUnitTestMode(
        /* [out] */ Boolean* result);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI RegisterForServiceStateChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI UnregisterForServiceStateChanged(
        /* [in] */ IHandler* h);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI RegisterForRingbackTone(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI UnregisterForRingbackTone(
        /* [in] */ IHandler* h);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI RegisterForOnHoldTone(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI UnregisterForOnHoldTone(
        /* [in] */ IHandler* h);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI RegisterForResendIncallMute(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // Inherited documentation suffices.
    // @Override
    virtual CARAPI UnregisterForResendIncallMute(
        /* [in] */ IHandler* h);

    // @Override
    virtual CARAPI SetEchoSuppressionEnabled();

    // Inherited documentation suffices.
    // @Override
    // CARAPI GetSimulatedRadioControl(
    //     /* [out] */ ISimulatedRadioControl** result);

    /**
     * Get state
     */
    // @Override
    virtual CARAPI GetState(
        /* [out] */ PhoneConstantsState* result) = 0;

    /**
     * Retrieves the IccFileHandler of the Phone instance
     */
    CARAPI GetIccFileHandler(
        /* [out] */ IIccFileHandler** result);

    /**
     * Retrieves the IccRecords of the Phone instance
     */
    CARAPI GetIccRecords(
        /* [out] */ IIccRecords** result);

    /*
     * Retrieves the Handler of the Phone instance
     */
    CARAPI GetHandler(
        /* [out] */ IHandler** result);

    // @Override
    virtual CARAPI UpdatePhoneObject(
        /* [in] */ Int32 voiceRadioTech);

    /**
    * Retrieves the ServiceStateTracker of the phone instance.
    */
    CARAPI GetServiceStateTracker(
        /* [out] */ IServiceStateTracker** result);

    /**
    * Get call tracker
    */
    CARAPI GetCallTracker(
        /* [out] */ ICallTracker** result);

    CARAPI GetCurrentUiccAppType(
        /* [out] */ AppType* result);

    // @Override
    virtual CARAPI GetIccCard(
        /* [out] */ IIccCard** result);

    // @Override
    virtual CARAPI GetIccSerialNumber(
        /* [out] */ String* result);

    // @Override
    virtual CARAPI GetIccRecordsLoaded(
        /* [out] */ Boolean* result);

    /**
     * @return all available cell information or NULL if none.
     */
    // @Override
    virtual CARAPI GetAllCellInfo(
        /* [out] */ IList** result);

    /**
     * {@inheritDoc}
     */
    // @Override
    virtual CARAPI SetCellInfoListRate(
        /* [in] */ Int32 rateInMillis);

    // @Override
    /** @return TRUE if there are messages waiting, FALSE otherwise. */
    virtual CARAPI GetMessageWaitingIndicator(
        /* [out] */ Boolean* result);

    // @Override
    virtual CARAPI GetCallForwardingIndicator(
        /* [out] */ Boolean* result);

    // @Override
    virtual CARAPI SetCallForwardingUncondTimerOption(
        /* [in] */ Int32 startHour,
        /* [in] */ Int32 startMinute,
        /* [in] */ Int32 endHour,
        /* [in] */ Int32 endMinute,
        /* [in] */ Int32 commandInterfaceCFReason,
        /* [in] */ Int32 commandInterfaceCFAction,
        /* [in] */ const String& dialingNumber,
        /* [in] */ IMessage* onComplete);

    // @Override
    virtual CARAPI GetCallForwardingUncondTimerOption(
        /* [in] */ Int32 commandInterfaceCFReason,
        /* [in] */ IMessage* onComplete);

    /**
     *  Query the status of the CDMA roaming preference
     */
    // @Override
    virtual CARAPI QueryCdmaRoamingPreference(
        /* [in] */ IMessage* response);

    /**
     * Get the signal strength
     */
    // @Override
    virtual CARAPI GetSignalStrength(
        /* [out] */ ISignalStrength** result);

    /**
     *  Set the status of the CDMA roaming preference
     */
    // @Override
    virtual CARAPI SetCdmaRoamingPreference(
        /* [in] */ Int32 cdmaRoamingType,
        /* [in] */ IMessage* response);

    /**
     *  Set the status of the CDMA subscription mode
     */
    // @Override
    virtual CARAPI SetCdmaSubscription(
        /* [in] */ Int32 cdmaSubscriptionType,
        /* [in] */ IMessage* response);

    /**
     *  Set the preferred Network Type: Global, CDMA only or GSM/UMTS only
     */
    // @Override
    virtual CARAPI SetPreferredNetworkType(
        /* [in] */ Int32 networkType,
        /* [in] */ IMessage* response);

    // @Override
    virtual CARAPI GetPreferredNetworkType(
        /* [in] */ IMessage* response);

    // @Override
    virtual CARAPI GetSmscAddress(
        /* [in] */ IMessage* result);

    // @Override
    virtual CARAPI SetSmscAddress(
        /* [in] */ const String& address,
        /* [in] */ IMessage* result);

    // @Override
    virtual CARAPI SetTTYMode(
        /* [in] */ Int32 ttyMode,
        /* [in] */ IMessage* onComplete);

    // @Override
    virtual CARAPI SetUiTTYMode(
        /* [in] */ Int32 uiTtyMode,
        /* [in] */ IMessage* onComplete);

    // @Override
    virtual CARAPI QueryTTYMode(
        /* [in] */ IMessage* onComplete);

    // @Override
    virtual CARAPI EnableEnhancedVoicePrivacy(
        /* [in] */ Boolean enable,
        /* [in] */ IMessage* onComplete);

    // @Override
    virtual CARAPI GetEnhancedVoicePrivacy(
        /* [in] */ IMessage* onComplete);

    // @Override
    virtual CARAPI SetBandMode(
        /* [in] */ Int32 bandMode,
        /* [in] */ IMessage* response);

    // @Override
    virtual CARAPI QueryAvailableBandMode(
        /* [in] */ IMessage* response);

    // @Override
    virtual CARAPI InvokeOemRilRequestRaw(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ IMessage* response);

    // @Override
    virtual CARAPI InvokeOemRilRequestStrings(
        /* [in] */ ArrayOf<String>* strings,
        /* [in] */ IMessage* response);

    // @Override
    virtual CARAPI NvReadItem(
        /* [in] */ Int32 itemID,
        /* [in] */ IMessage* response);

    // @Override
    virtual CARAPI NvWriteItem(
        /* [in] */ Int32 itemID,
        /* [in] */ const String& itemValue,
        /* [in] */ IMessage* response);

    // @Override
    virtual CARAPI NvWriteCdmaPrl(
        /* [in] */ ArrayOf<Byte>* preferredRoamingList,
        /* [in] */ IMessage* response);

    // @Override
    virtual CARAPI NvResetConfig(
        /* [in] */ Int32 resetType,
        /* [in] */ IMessage* response);

    // @Override
    virtual CARAPI NotifyDataActivity();

    CARAPI NotifyMessageWaitingIndicator();

    CARAPI NotifyDataConnection(
        /* [in] */ const String& reason,
        /* [in] */ const String& apnType,
        /* [in] */ PhoneConstantsDataState state);

    CARAPI NotifyDataConnection(
        /* [in] */ const String& reason,
        /* [in] */ const String& apnType);

    CARAPI NotifyDataConnection(
        /* [in] */ const String& reason);

    CARAPI NotifyOtaspChanged(
        /* [in] */ Int32 otaspMode);

    CARAPI NotifySignalStrength();

    CARAPI NotifyCellInfo(
        /* [in] */ IList/*<CellInfo>*/* cellInfo);

    CARAPI NotifyDataConnectionRealTimeInfo(
        /* [in] */ IDataConnectionRealTimeInfo* dcRtInfo);

    CARAPI NotifyVoLteServiceStateChanged(
        /* [in] */ IVoLteServiceState* lteState);

    /**
     * @return TRUE if a mobile originating emergency call is active
     */
    CARAPI IsInEmergencyCall(
        /* [out] */ Boolean* result);

    /**
     * @return TRUE if we are in the emergency call back mode. This is a period where
     * the phone should be using as little power as possible and be ready to receive an
     * incoming call from the emergency operator.
     */
    CARAPI IsInEcm(
        /* [out] */ Boolean* result);

    static CARAPI_(Int32) GetVideoState(
        /* [in] */ ICall* call);

    CARAPI IsImsVtCallPresent(
        /* [out] */ Boolean* result);

    // @Override
    virtual CARAPI GetPhoneType(
        /* [out] */ Int32* result) = 0;

    /** @hide */
    /** @return number of voicemails */
    // @Override
    virtual CARAPI GetVoiceMessageCount(
        /* [out] */ Int32* result);

    /** sets the voice mail count of the phone and notifies listeners. */
    CARAPI SetVoiceMessageCount(
        /* [in] */ Int32 countWaiting);

    /**
     * Returns the CDMA ERI icon index to display
     */
    // @Override
    virtual CARAPI GetCdmaEriIconIndex(
        /* [out] */ Int32* result);

    /**
     * Returns the CDMA ERI icon mode,
     * 0 - ON
     * 1 - FLASHING
     */
    // @Override
    virtual CARAPI GetCdmaEriIconMode(
        /* [out] */ Int32* result);

    /**
     * Returns the CDMA ERI text,
     */
    // @Override
    virtual CARAPI GetCdmaEriText(
        /* [out] */ String* result);

    // @Override
    virtual CARAPI GetCdmaMin(
        /* [out] */ String* result);

    // @Override
    virtual CARAPI IsMinInfoReady(
        /* [out] */ Boolean* result);

    // @Override
    virtual CARAPI GetCdmaPrlVersion(
        /* [out] */ String* result);

    // @Override
    virtual CARAPI SendBurstDtmf(
        /* [in] */ const String& dtmfString,
        /* [in] */ Int32 on,
        /* [in] */ Int32 off,
        /* [in] */ IMessage* onComplete);

    // @Override
    virtual CARAPI ExitEmergencyCallbackMode();

    // @Override
    virtual CARAPI RegisterForCdmaOtaStatusChange(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    virtual CARAPI UnregisterForCdmaOtaStatusChange(
        /* [in] */ IHandler* h);

    // @Override
    virtual CARAPI RegisterForSubscriptionInfoReady(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    virtual CARAPI UnregisterForSubscriptionInfoReady(
        /* [in] */ IHandler* h);

    /**
     * Returns TRUE if OTA Service Provisioning needs to be performed.
     * If not overridden return FALSE.
     */
    // @Override
    virtual CARAPI NeedsOtaServiceProvisioning(
        /* [out] */ Boolean* result);

    /**
     * Return TRUE if number is an OTASP number.
     * If not overridden return FALSE.
     */
    // @Override
    virtual CARAPI IsOtaSpNumber(
        /* [in] */ const String& dialStr,
        /* [out] */ Boolean* result);

    // @Override
    virtual CARAPI RegisterForCallWaiting(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    virtual CARAPI UnregisterForCallWaiting(
        /* [in] */ IHandler* h);

    // @Override
    virtual CARAPI RegisterForEcmTimerReset(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    virtual CARAPI UnregisterForEcmTimerReset(
        /* [in] */ IHandler* h);

    // @Override
    virtual CARAPI RegisterForSignalInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    virtual CARAPI UnregisterForSignalInfo(
        /* [in] */ IHandler* h);

    // @Override
    virtual CARAPI RegisterForDisplayInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

     // @Override
    virtual CARAPI UnregisterForDisplayInfo(
        /* [in] */ IHandler* h);

    // @Override
    virtual CARAPI RegisterForNumberInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    virtual CARAPI UnregisterForNumberInfo(
        /* [in] */ IHandler* h);

    // @Override
    virtual CARAPI RegisterForRedirectedNumberInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    virtual CARAPI UnregisterForRedirectedNumberInfo(
        /* [in] */ IHandler* h);

    // @Override
    virtual CARAPI RegisterForLineControlInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    virtual CARAPI UnregisterForLineControlInfo(
        /* [in] */ IHandler* h);

    // @Override
    virtual CARAPI RegisterFoT53ClirlInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    virtual CARAPI UnregisterForT53ClirInfo(
        /* [in] */ IHandler* h);

    // @Override
    virtual CARAPI RegisterForT53AudioControlInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    virtual CARAPI UnregisterForT53AudioControlInfo(
        /* [in] */ IHandler* h);

    // @Override
    virtual CARAPI SetOnEcbModeExitResponse(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    virtual CARAPI UnsetOnEcbModeExitResponse(
        /* [in] */ IHandler* h);

    // @Override
    virtual CARAPI GetActiveApnTypes(
        /* [out] */ ArrayOf<String>** types);

    // @Override
    virtual CARAPI GetActiveApnHost(
        /* [in] */ const String& apnType,
        /* [out] */ String* result);

    // @Override
    virtual CARAPI GetLinkProperties(
        /* [in] */ const String& apnType,
        /* [out] */ ILinkProperties** result);

    // @Override
    virtual CARAPI GetNetworkCapabilities(
        /* [in] */ const String& apnType,
        /* [out] */ INetworkCapabilities** result);

    // @Override
    virtual CARAPI IsDataConnectivityPossible(
        /* [out] */ Boolean* result);

    // @Override
    virtual CARAPI IsOnDemandDataPossible(
        /* [in] */ const String& apnType,
        /* [out] */ Boolean* result);

    // @Override
    virtual CARAPI IsDataConnectivityPossible(
        /* [in] */ const String& apnType,
        /* [out] */ Boolean* result);

    /**
     * Notify registrants of a new ringing Connection.
     * Subclasses of Phone probably want to replace this with a
     * version scoped to their packages
     */
    CARAPI NotifyNewRingingConnectionP(
        /* [in] */ IConnection* cn);


    /**
     * Notify registrants if phone is video capable.
     */
    CARAPI NotifyForVideoCapabilityChanged(
        /* [in] */ Boolean isVideoCallCapable);

    CARAPI IsManualNetSelAllowed(
        /* [out] */ Boolean* result);

    // @Override
    virtual CARAPI IsCspPlmnEnabled(
        /* [out] */ Boolean* result);

    // @Override
    virtual CARAPI GetIsimRecords(
        /* [out] */ IIsimRecords** result);

    // @Override
    virtual CARAPI GetMsisdn(
        /* [out] */ String* result);

    CARAPI GetDataConnectionState(
        /* [in] */ const String& apnType,
        /* [out] */ PhoneConstantsDataState* result) = 0;

    // @Override
    virtual CARAPI GetDataConnectionState(
        /* [out] */ PhoneConstantsDataState* result);

    // Called by SimRecords which is constructed with a PhoneBase instead of a GSMPhone.
    CARAPI NotifyCallForwardingIndicator();

    CARAPI NotifyDataConnectionFailed(
        /* [in] */ const String& reason,
        /* [in] */ const String& apnType);

    CARAPI NotifyPreciseDataConnectionFailed(
        /* [in] */ const String& reason,
        /* [in] */ const String& apnType,
        /* [in] */ const String& apn,
        /* [in] */ const String& failCause);

    /**
     * {@inheritDoc}
     */
    // @Override
    virtual CARAPI GetLteOnCdmaMode(
        /* [out] */ Int32* result);

    /**
     * {@hide}
     */
    // @Override
    virtual CARAPI GetLteOnGsmMode(
        /* [out] */ Int32* result);

    /**
     * Sets the SIM voice message waiting indicator records.
     * @param line GSM Subscriber Profile Number, one-based. Only '1' is supported
     * @param countWaiting The number of messages waiting, if known. Use
     *                     -1 to indicate that an unknown number of
     *                      messages are waiting
     */
    // @Override
    virtual CARAPI SetVoiceMessageWaiting(
        /* [in] */ Int32 line,
        /* [in] */ Int32 countWaiting);

    /**
     * Gets the USIM service table from the UICC, if present and available.
     * @return an interface to the UsimServiceTable record, or NULL if not available
     */
    // @Override
    virtual CARAPI GetUsimServiceTable(
        /* [out] */ IUsimServiceTable** result);

    /**
     * Gets the Uicc card corresponding to this phone.
     * @return the UiccCard object corresponding to the phone ID.
     */
    // @Override
    virtual CARAPI GetUiccCard(
        /* [out] */ IUiccCard** result);

    /**
     * Get P-CSCF address from PCO after data connection is established or modified.
     * @param apnType the apnType, "ims" for IMS APN, "emergency" for EMERGENCY APN
     */
    // @Override
    virtual CARAPI GetPcscfAddress(
        /* [in] */ const String& apnType,
        /* [out] */ ArrayOf<String>** result);

    /**
     * Set IMS registration state
     */
    // @Override
    virtual CARAPI SetImsRegistrationState(
        /* [in] */ Boolean registered);

    /**
     * Return an instance of a IMS phone
     */
    // @Override
    virtual CARAPI GetImsPhone(
        /* [out] */ IPhone** result);

    // @Override
    virtual CARAPI RelinquishOwnershipOfImsPhone(
        /* [out] */ IImsPhone** result);

    // @Override
    virtual CARAPI AcquireOwnershipOfImsPhone(
        /* [in] */ IImsPhone* imsPhone);

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    /**
     * Returns the subscription id.
     */
    CARAPI GetSubId(
        /* [out] */ Int64* result);

    /**
     * Returns the phone id.
     */
    CARAPI GetPhoneId(
        /* [out] */ Int32* result);

    //Gets Subscription information in the Phone Object
    CARAPI GetSubscriptionInfo(
        /* [out] */ ISubscription** result);

    /**
     * Return the service state of mImsPhone if it is STATE_IN_SERVICE
     * otherwise return the current voice service state
     */
    // @Override
    virtual CARAPI GetVoicePhoneServiceState(
        /* [out] */ Int32* result);

    // @Override
    virtual CARAPI SetOperatorBrandOverride(
        /* [in] */ const String& brand,
        /* [out] */ Boolean* result);

    // @Override
    virtual CARAPI IsRadioAvailable(
        /* [out] */ Boolean* result);

    // @Override
    virtual CARAPI ShutdownRadio();

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

    // @Override
    virtual CARAPI SetLocalCallHold(
        /* [in] */ Int32 lchStatus);

    // @Override
    virtual CARAPI DeflectCall(
        /* [in] */ const String& number) /*throws CallStateException*/;

    // @Override
    virtual CARAPI AddParticipant(
        /* [in] */ const String& dialString) /*throws CallStateException*/;

protected:
    // Set the Card into the Phone Book.
    virtual CARAPI_(void) SetCardInPhoneBook();

    // Will be called when icc changed
    virtual CARAPI OnUpdateIccAvailability() = 0;

    /**
     * Subclasses of Phone probably want to replace this with a
     * version scoped to their packages
     */
    virtual CARAPI_(void) NotifyPreciseCallStateChangedP();

    /**
     * To be invoked when a voice call Connection disconnects.
     *
     * Subclasses of Phone probably want to replace this with a
     * version scoped to their packages
     */
    virtual CARAPI_(void) NotifyDisconnectP(
        /* [in] */ IConnection* cn);

    /**
     * Subclasses of Phone probably want to replace this with a
     * version scoped to their packages
     */
    virtual CARAPI_(void) NotifyServiceStateChangedP(
        /* [in] */ IServiceState* ss);

    virtual CARAPI_(void) UpdateImsPhone();

    /**
     * Dials a number.
     *
     * @param dialString The number to dial.
     * @param uusInfo The UUSInfo.
     * @param videoState The video state for the call.
     * @return The Connection.
     * @throws CallStateException
     */
    virtual CARAPI DialInternal(
        /* [in] */ const String& dialString,
        /* [in] */ IUUSInfo* uusInfo,
        /* [in] */ Int32 videoState,
        /* [out] */ IConnection** result) /*throws CallStateException*/;

private:
    CARAPI_(void) HandleSrvccStateChanged(
        /* [in] */ ArrayOf<Int32>* ret);

    /**
     * Used to track the settings upon completion of the network change.
     */
    CARAPI_(void) HandleSetSelectNetwork(
        /* [in] */ AsyncResult* ar);

    /**
     * Method to retrieve the saved operator id from the Shared Preferences
     */
    CARAPI_(String) GetSavedNetworkSelection();

    /**
     * Verifies the current thread is the same as the thread originally
     * used in the initialization of this instance. Throws RuntimeException
     * if not.
     *
     * @exception RuntimeException if the current thread is not
     * the thread that originally obtained this PhoneBase instance.
     */
    CARAPI CheckCorrectThread(
        /* [in] */ IHandler* h);

    /**
     * Set the properties by matching the carrier string in
     * a string-array resource
     */
    CARAPI_(void) SetPropertiesByCarrier();

    /**
     * Clear CDMA base station lat/Int64 values if location setting is disabled.
     * @param cellInfoList the original cell info list from the RIL
     * @return the original list with CDMA lat/Int64 cleared if necessary
     */
    CARAPI_(AutoPtr<IList/*<CellInfo*/>) PrivatizeCellInfoList(
        /* [in] */ IList/*<CellInfo>*/* cellInfoList);

    CARAPI_(Boolean) IsImsVideoCall(
        /* [in] */ ICall* call);

    /**
     * Notify registrants of a RING event.
     */
    CARAPI_(void) NotifyIncomingRing();

    /**
     * Send the incoming call Ring notification if conditions are right.
     */
    CARAPI_(void) SendIncomingCallRingNotification(
        /* [in] */ Int32 token);

    /**
     * Common error logger method for unexpected calls to CDMA-only methods.
     */
    static CARAPI_(void) LogUnexpectedCdmaMethodCall(
        /* [in] */ const String& name);

    /**
     * Common error logger method for unexpected calls to GSM/WCDMA-only methods.
     */
    static CARAPI_(void) LogUnexpectedGsmMethodCall(
        /* [in] */ const String& name);

public:
    /* Instance Variables */
    AutoPtr<ICommandsInterface> mCi;
    AutoPtr<IDcTrackerBase> mDcTracker;
    Boolean mIsTheCurrentActivePhone;
    AutoPtr<IAtomicReference> mIccRecords;
    AutoPtr<SmsStorageMonitor> mSmsStorageMonitor;
    AutoPtr<SmsUsageMonitor> mSmsUsageMonitor;

protected:
    /* Event Constants */
    static const Int32 EVENT_RADIO_AVAILABLE             = 1;
    static const Int32 EVENT_SSN                         = 2;
    static const Int32 EVENT_SIM_RECORDS_LOADED          = 3;
    static const Int32 EVENT_MMI_DONE                    = 4;
    static const Int32 EVENT_RADIO_ON                    = 5;
    static const Int32 EVENT_GET_BASEBAND_VERSION_DONE   = 6;
    static const Int32 EVENT_USSD                        = 7;
    static const Int32 EVENT_RADIO_OFF_OR_NOT_AVAILABLE  = 8;
    static const Int32 EVENT_GET_IMEI_DONE               = 9;
    static const Int32 EVENT_GET_IMEISV_DONE             = 10;
    static const Int32 EVENT_GET_SIM_STATUS_DONE         = 11;
    static const Int32 EVENT_SET_CALL_FORWARD_DONE       = 12;
    static const Int32 EVENT_GET_CALL_FORWARD_DONE       = 13;
    static const Int32 EVENT_CALL_RING                   = 14;
    static const Int32 EVENT_CALL_RING_CONTINUE          = 15;
    static const Int32 EVENT_SET_NETWORK_MANUAL_COMPLETE    = 16;
    static const Int32 EVENT_SET_NETWORK_AUTOMATIC_COMPLETE = 17;
    static const Int32 EVENT_SET_CLIR_COMPLETE              = 18;
    static const Int32 EVENT_REGISTERED_TO_NETWORK          = 19;
    static const Int32 EVENT_SET_VM_NUMBER_DONE             = 20;
    static const Int32 EVENT_GET_DEVICE_IDENTITY_DONE       = 21;
    static const Int32 EVENT_RUIM_RECORDS_LOADED            = 22;
    static const Int32 EVENT_NV_READY                       = 23;
    static const Int32 EVENT_SET_ENHANCED_VP                = 24;
    static const Int32 EVENT_EMERGENCY_CALLBACK_MODE_ENTER  = 25;
    static const Int32 EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE = 26;
    static const Int32 EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED = 27;
    static const Int32 EVENT_SET_NETWORK_AUTOMATIC          = 28;
    static const Int32 EVENT_ICC_RECORD_EVENTS              = 29;
    static const Int32 EVENT_ICC_CHANGED                    = 30;
    static const Int32 EVENT_SRVCC_STATE_CHANGED            = 31;
    static const Int32 EVENT_INITIATE_SILENT_REDIAL         = 32;
    static const Int32 EVENT_UNSOL_OEM_HOOK_RAW             = 33;
    static const Int32 EVENT_SS                             = 34;
    static const Int32 EVENT_SET_CALL_FORWARD_TIMER_DONE    = 35;
    static const Int32 EVENT_GET_CALL_FORWARD_TIMER_DONE    = 36;
    static const Int32 EVENT_LAST                   = EVENT_GET_CALL_FORWARD_TIMER_DONE;

    Boolean mDnsCheckDisabled;

    Boolean mDoesRilSendMultipleCallRing;
    Int32 mCallRingContinueToken;
    Int32 mCallRingDelay;
    Boolean mIsVoiceCapable;

    AutoPtr<IUiccController> mUiccController;

    AutoPtr<IAtomicReference> mUiccApplication;

    // Holds the subscription information
    AutoPtr<ISubscription> mSubscriptionData;
    Int32 mPhoneId;
    AutoPtr<IImsPhone> mImsPhone;

    // Flag that indicates that Out Of Service is considered as data call disconnect
    Boolean mOosIsDisconnect;

    AutoPtr<RegistrantList> mPreciseCallStateRegistrants;

    AutoPtr<RegistrantList> mHandoverRegistrants;

    AutoPtr<RegistrantList> mNewRingingConnectionRegistrants;

    AutoPtr<RegistrantList> mIncomingRingRegistrants;

    AutoPtr<RegistrantList> mDisconnectRegistrants;

    AutoPtr<RegistrantList> mServiceStateRegistrants;

    AutoPtr<RegistrantList> mMmiCompleteRegistrants;

    AutoPtr<RegistrantList> mMmiRegistrants;

    AutoPtr<RegistrantList> mUnknownConnectionRegistrants;

    AutoPtr<RegistrantList> mSuppServiceFailedRegistrants;

    AutoPtr<RegistrantList> mSimRecordsLoadedRegistrants;

    AutoPtr<RegistrantList> mVideoCapabilityChangedRegistrants;


    AutoPtr<ILooper> mLooper; /* to insure registrants are in correct thread*/

    AutoPtr<IContext> mContext;

    /**
     * PhoneNotifier is an abstraction for all system-wide
     * state change notification. DefaultPhoneNotifier is
     * used here unless running we're inside a unit test.
     */
    AutoPtr<IPhoneNotifier> mNotifier;

    // AutoPtr<ISimulatedRadioControl> mSimulatedRadioControl;

    Boolean mUnitTestMode;

private:
    static const String TAG;

    AutoPtr<IBroadcastReceiver> mImsIntentReceiver;

    Int32 mVmCount;

    // Variable to cache the video capabilitity. In some cases, we lose this info and are unable
    // to recover from the state. So, we cache it and notify listeners when they register.
    Boolean mIsVideoCapable;

    AutoPtr<ITelephonyTester> mTelephonyTester;
    String mName;
    String mActionDetached;
    String mActionAttached;

    Object mImsLock/* = new Object()*/;
    Boolean mImsServiceReady;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_PHONEBASE_H__
