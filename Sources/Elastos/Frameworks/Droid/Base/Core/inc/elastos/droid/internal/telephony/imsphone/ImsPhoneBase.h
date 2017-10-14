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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONEBASE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONEBASE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/droid/os/RegistrantList.h"
#include "elastos/droid/internal/telephony/PhoneBase.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState;
using Elastos::Droid::Internal::Telephony::IPhoneNotifier;
using Elastos::Droid::Internal::Telephony::IUUSInfo;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::PhoneBase;
using Elastos::Droid::Internal::Telephony::IIccCard;
using Elastos::Droid::Internal::Telephony::IOperatorInfo;
using Elastos::Droid::Internal::Telephony::IPhoneSubInfo;
using Elastos::Droid::Internal::Telephony::IIccPhoneBookInterfaceManager;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Internal::Telephony::IPhoneDataActivityState;
using Elastos::Droid::Internal::Telephony::IPhoneSuppService;
using Elastos::Droid::Os::RegistrantList;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

class ImsPhoneBase
    : public PhoneBase
{
public:
    ImsPhoneBase();

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ IContext* context,
        /* [in] */ IPhoneNotifier* notifier);

    using PhoneBase::Dial;

    // @Override
    CARAPI Dial(
        /* [in] */ const String& dialString,
        /* [in] */ IUUSInfo* uusInfo,
        /* [in] */ Int32 videoState,
        /* [out] */ IConnection** result);

    // @Override
    CARAPI MigrateFrom(
        /* [in] */ IPhoneBase* from);

    // @Override
    CARAPI RegisterForRingbackTone(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForRingbackTone(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForOnHoldTone(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForOnHoldTone(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI GetServiceState(
        /* [out] */ IServiceState** result);

    /**
      * @return all available cell information or null if none.
      */
    // @Override
    CARAPI GetAllCellInfo(
        /* [out] */ IList** result);//CellInfo

    // @Override
    CARAPI GetCellLocation(
        /* [out] */ ICellLocation** result);

    // @Override
    CARAPI GetState(
        /* [out] */ PhoneConstantsState* result);

    // @Override
    CARAPI GetPhoneType(
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetSignalStrength(
        /* [out] */ ISignalStrength** result);

    // @Override
    CARAPI GetMessageWaitingIndicator(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetCallForwardingIndicator(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetPendingMmiCodes(
        /* [out] */ IList/*<? extends MmiCode>*/** result);

    // @Override
    CARAPI GetDataConnectionState(
        /* [out] */ PhoneConstantsDataState* result);

    // @Override
    CARAPI GetDataConnectionState(
        /* [in] */ const String& apnType,
        /* [out] */ PhoneConstantsDataState* result);

    // @Override
    CARAPI GetDataActivityState(
        /* [out] */ IPhoneDataActivityState* result);

    /**
      * Notify any interested party of a Phone state change
      * {@link com.android.internal.telephony.PhoneConstants.State}
      */
    /* package */
    virtual CARAPI NotifyPhoneStateChanged();

    /**
      * Notify registrants of a change in the call state. This notifies changes in
      * {@link com.android.internal.telephony.Call.State}. Use this when changes
      * in the precise call state are needed, else use notifyPhoneStateChanged.
      */
    /* package */
    virtual CARAPI NotifyPreciseCallStateChanged();

    virtual CARAPI NotifyDisconnect(
        /* [in] */ IConnection* cn);

    virtual CARAPI NotifyUnknownConnection();

    virtual CARAPI NotifySuppServiceFailed(
        /* [in] */ IPhoneSuppService code);

    virtual CARAPI NotifyServiceStateChanged(
        /* [in] */ IServiceState* ss);

    // @Override
    CARAPI NotifyCallForwardingIndicator();

    virtual CARAPI CanDial(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI HandleInCallMmiCommands(
        /* [in] */ const String& dialString,
        /* [out] */ Boolean* result);

    virtual CARAPI IsInCall(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI HandlePinMmi(
        /* [in] */ const String& dialString,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SendUssdResponse(
        /* [in] */ const String& ussdMessge);

    // @Override
    CARAPI RegisterForSuppServiceNotification(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForSuppServiceNotification(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI SetRadioPower(
        /* [in] */ Boolean power);

    // @Override
    CARAPI GetVoiceMailNumber(
        /* [out] */ String* result);

    // @Override
    CARAPI GetVoiceMailAlphaTag(
        /* [out] */ String* result);

    // @Override
    CARAPI GetDeviceId(
        /* [out] */ String* result);

    // @Override
    CARAPI GetDeviceSvn(
        /* [out] */ String* result);

    // @Override
    CARAPI GetImei(
        /* [out] */ String* result);

    // @Override
    CARAPI GetEsn(
        /* [out] */ String* result);

    // @Override
    CARAPI GetMeid(
        /* [out] */ String* result);

    // @Override
    CARAPI GetSubscriberId(
        /* [out] */ String* result);

    // @Override
    CARAPI GetGroupIdLevel1(
        /* [out] */ String* result);

    // @Override
    CARAPI GetIccSerialNumber(
        /* [out] */ String* result);

    // @Override
    CARAPI GetLine1Number(
        /* [out] */ String* result);

    // @Override
    CARAPI GetLine1AlphaTag(
        /* [out] */ String* result);

    // @Override
    CARAPI SetLine1Number(
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& number,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SetVoiceMailNumber(
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& voiceMailNumber,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI GetCallForwardingOption(
        /* [in] */ Int32 commandInterfaceCFReason,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SetCallForwardingOption(
        /* [in] */ Int32 commandInterfaceCFAction,
        /* [in] */ Int32 commandInterfaceCFReason,
        /* [in] */ const String& dialingNumber,
        /* [in] */ Int32 timerSeconds,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SetCallForwardingUncondTimerOption(
        /* [in] */ Int32 startHour,
        /* [in] */ Int32 startMinute,
        /* [in] */ Int32 endHour,
        /* [in] */ Int32 endMinute,
        /* [in] */ Int32 commandInterfaceCFAction,
        /* [in] */ Int32 commandInterfaceCFReason,
        /* [in] */ const String& dialingNumber,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI GetOutgoingCallerIdDisplay(
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SetOutgoingCallerIdDisplay(
        /* [in] */ Int32 commandInterfaceCLIRMode,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI GetCallWaiting(
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SetCallWaiting(
        /* [in] */ Boolean enable,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI GetIccRecordsLoaded(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetIccCard(
        /* [out] */ IIccCard** result);

    // @Override
    CARAPI GetAvailableNetworks(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetNetworkSelectionModeAutomatic(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SelectNetworkManually(
        /* [in] */ IOperatorInfo* network,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI GetNeighboringCids(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI GetDataCallList(
        /* [in] */ IMessage* response);

    virtual CARAPI GetCurrentDataConnectionList(
        /* [out] */ IList/*<DataConnection>*/** result);

    // @Override
    CARAPI UpdateServiceLocation();

    // @Override
    CARAPI EnableLocationUpdates();

    // @Override
    CARAPI DisableLocationUpdates();

    // @Override
    CARAPI GetDataRoamingEnabled(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetDataRoamingEnabled(
        /* [in] */ Boolean enable);

    // @Override
    CARAPI GetDataEnabled(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetDataEnabled(
        /* [in] */ Boolean enable);

    virtual CARAPI EnableDataConnectivity(
        /* [out] */ Boolean* result);

    virtual CARAPI DisableDataConnectivity(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsDataConnectivityPossible(
        /* [out] */ Boolean* result);

    virtual CARAPI UpdateCurrentCarrierInProvider(
        /* [out] */ Boolean* result);

    virtual CARAPI SaveClirSetting(
        /* [in] */ Int32 commandInterfaceCLIRMode);

    // @Override
    CARAPI GetPhoneSubInfo(
        /* [out] */ IPhoneSubInfo** result);

    // @Override
    CARAPI GetIccPhoneBookInterfaceManager(
        /* [out] */ IIccPhoneBookInterfaceManager** result);

    // @Override
    CARAPI GetIccFileHandler(
        /* [out] */ IIccFileHandler** result);

    // @Override
    CARAPI ActivateCellBroadcastSms(
        /* [in] */ Int32 activate,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI GetCellBroadcastSmsConfig(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetCellBroadcastSmsConfig(
        /* [in] */ ArrayOf<Int32>* configValuesArray,
        /* [in] */ IMessage* response);

    //@Override
    // @Override
    CARAPI NeedsOtaServiceProvisioning(
        /* [out] */ Boolean* result);

    //@Override
    // @Override
    CARAPI GetLinkProperties(
        /* [in] */ const String& apnType,
        /* [out] */ ILinkProperties** result);

    virtual CARAPI UpdatePhoneState();

    virtual CARAPI_(void) StartRingbackTone();

    virtual CARAPI_(void) StopRingbackTone();

    virtual CARAPI_(void) StartOnHoldTone();

    virtual CARAPI_(void) StopOnHoldTone();

    // @Override
    CARAPI OnUpdateIccAvailability();

private:
    static const String LOGTAG;
    AutoPtr<RegistrantList> mRingbackRegistrants;
    AutoPtr<RegistrantList> mOnHoldRegistrants;
    PhoneConstantsState mState;
};

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONEBASE_H__

