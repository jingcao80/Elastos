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
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneBase.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneCommandInterface.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/telephony/CServiceState.h"
#include "elastos/droid/telephony/CSignalStrength.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Telephony::CServiceState;
using Elastos::Droid::Telephony::CSignalStrength;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

//=====================================================================
//                             ImsPhoneBase
//=====================================================================
const String ImsPhoneBase::LOGTAG("ImsPhoneBase");

ImsPhoneBase::ImsPhoneBase()
    : mState(PhoneConstantsState_IDLE)
{
    mRingbackRegistrants = new RegistrantList();
    mOnHoldRegistrants = new RegistrantList();
}

ECode ImsPhoneBase::constructor(
    /* [in] */ const String& name,
    /* [in] */ IContext* context,
    /* [in] */ IPhoneNotifier* notifier)
{
    AutoPtr<ImsPhoneCommandInterface> ci = new ImsPhoneCommandInterface();
    ci->constructor(context);
    return PhoneBase::constructor(name, notifier, context,
            ICommandsInterface::Probe(ci), FALSE);
}

ECode ImsPhoneBase::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ IUUSInfo* uusInfo,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // ignore UUSInfo
    return Dial(dialString, videoState, result);
}

ECode ImsPhoneBase::MigrateFrom(
    /* [in] */ IPhoneBase* from)
{
    PhoneBase::MigrateFrom(from);
    Migrate(mRingbackRegistrants, ((ImsPhoneBase*)from)->mRingbackRegistrants);
    return NOERROR;
}

ECode ImsPhoneBase::RegisterForRingbackTone(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mRingbackRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

ECode ImsPhoneBase::UnregisterForRingbackTone(
    /* [in] */ IHandler* h)
{
    mRingbackRegistrants->Remove(h);
    return NOERROR;
}

ECode ImsPhoneBase::RegisterForOnHoldTone(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mOnHoldRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

ECode ImsPhoneBase::UnregisterForOnHoldTone(
    /* [in] */ IHandler* h)
{
    mOnHoldRegistrants->Remove(h);
    return NOERROR;
}

ECode ImsPhoneBase::GetServiceState(
    /* [out] */ IServiceState** result)
{
    VALIDATE_NOT_NULL(result);
    // FIXME: we may need to provide this when data connectivity is lost
    // or when server is down
    AutoPtr<IServiceState> s;
    CServiceState::New((IServiceState**)&s);
    s->SetState(IServiceState::STATE_IN_SERVICE);
    *result = s;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhoneBase::GetAllCellInfo(
    /* [out] */ IList/*<CellInfo>*/** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IServiceStateTracker> sst;
    GetServiceStateTracker((IServiceStateTracker**)&sst);
    sst->GetAllCellInfo(result);
    return NOERROR;
}

ECode ImsPhoneBase::GetCellLocation(
    /* [out] */ ICellLocation** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode ImsPhoneBase::GetState(
    /* [out] */ PhoneConstantsState* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mState;
    return NOERROR;
}

ECode ImsPhoneBase::GetPhoneType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IPhoneConstants::PHONE_TYPE_IMS;
    return NOERROR;
}

ECode ImsPhoneBase::GetSignalStrength(
    /* [out] */ ISignalStrength** result)
{
    VALIDATE_NOT_NULL(result);
    return CSignalStrength::New(result);
}

ECode ImsPhoneBase::GetMessageWaitingIndicator(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode ImsPhoneBase::GetCallForwardingIndicator(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode ImsPhoneBase::GetPendingMmiCodes(
    /* [out] */ IList/*<? extends MmiCode>*/** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IArrayList> list;
    CArrayList::New(0, (IArrayList**)&list);
    *result = IList::Probe(list);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhoneBase::GetDataConnectionState(
    /* [out] */ PhoneConstantsDataState* result)
{
    VALIDATE_NOT_NULL(result);
    *result = PhoneConstantsDataState_DISCONNECTED;
    return NOERROR;
}

ECode ImsPhoneBase::GetDataConnectionState(
    /* [in] */ const String& apnType,
    /* [out] */ PhoneConstantsDataState* result)
{
    VALIDATE_NOT_NULL(result);
    *result = PhoneConstantsDataState_DISCONNECTED;
    return NOERROR;
}

ECode ImsPhoneBase::GetDataActivityState(
    /* [out] */ IPhoneDataActivityState* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IPhoneDataActivityState_NONE;
    return NOERROR;
}

ECode ImsPhoneBase::NotifyPhoneStateChanged()
{
    return mNotifier->NotifyPhoneState(this);
}

ECode ImsPhoneBase::NotifyPreciseCallStateChanged()
{
    /* we'd love it if this was package-scoped*/
    PhoneBase::NotifyPreciseCallStateChangedP();
    return NOERROR;
}

ECode ImsPhoneBase::NotifyDisconnect(
    /* [in] */ IConnection* cn)
{
    mDisconnectRegistrants->NotifyResult(cn);
    return NOERROR;
}

ECode ImsPhoneBase::NotifyUnknownConnection()
{
    mUnknownConnectionRegistrants->NotifyResult(TO_IINTERFACE(this));
    return NOERROR;
}

ECode ImsPhoneBase::NotifySuppServiceFailed(
    /* [in] */ IPhoneSuppService code)
{
    mSuppServiceFailedRegistrants->NotifyResult(CoreUtils::Convert(code));
    return NOERROR;
}

ECode ImsPhoneBase::NotifyServiceStateChanged(
    /* [in] */ IServiceState* ss)
{
    PhoneBase::NotifyServiceStateChangedP(ss);
    return NOERROR;
}

ECode ImsPhoneBase::NotifyCallForwardingIndicator()
{
    return mNotifier->NotifyCallForwardingChanged(this);
}

ECode ImsPhoneBase::CanDial(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IServiceState> ss;
    GetServiceState((IServiceState**)&ss);
    Int32 serviceState;
    ss->GetState(&serviceState);
    Logger::V(LOGTAG, "canDial(): serviceState = %d", serviceState);
    if (serviceState == IServiceState::STATE_POWER_OFF) {
        *result = FALSE;
        return NOERROR;
    }

    String disableCall;
    SystemProperties::Get(
            ITelephonyProperties::PROPERTY_DISABLE_CALL, String("FALSE"), &disableCall);
    Logger::V(LOGTAG, "canDial(): disableCall = %s", disableCall.string());
    if (disableCall.Equals("TRUE")) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<ICall> fgc;
    GetForegroundCall((ICall**)&fgc);
    ICallState foregroundCallState;
    fgc->GetState(&foregroundCallState);

    AutoPtr<ICall> bgc;
    GetBackgroundCall((ICall**)&bgc);
    ICallState backgroundCallState;
    bgc->GetState(&backgroundCallState);

    AutoPtr<ICall> call;
    GetRingingCall((ICall**)&call);
    ICallState ringingCallState;
    call->GetState(&ringingCallState);

    Logger::V(LOGTAG, "canDial(): ringingCall: %d", ringingCallState);
    Logger::V(LOGTAG, "canDial(): foregndCall: %d", foregroundCallState);
    Logger::V(LOGTAG, "canDial(): backgndCall: %d", backgroundCallState);

    Boolean b1, b2, b3;
// TODO: Need ICallState::isAlive
    // *result = (call->IsRinging(&b1), !b1)
    //         && ((foregroundCallState->IsAlive(&b2), !b2)
    //             || (backgroundCallState->IsAlive(&b3), !b3));
    return NOERROR;
}

ECode ImsPhoneBase::HandleInCallMmiCommands(
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode ImsPhoneBase::IsInCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICall> fgc;
    GetForegroundCall((ICall**)&fgc);
    ICallState foregroundCallState;
    fgc->GetState(&foregroundCallState);

    AutoPtr<ICall> bgc;
    GetBackgroundCall((ICall**)&bgc);
    ICallState backgroundCallState;
    bgc->GetState(&backgroundCallState);

    AutoPtr<ICall> call;
    GetRingingCall((ICall**)&call);
    ICallState ringingCallState;
    call->GetState(&ringingCallState);

// TODO: Need ICallState::isAlive
    // *rsult = (foregroundCallState->IsAlive() || backgroundCallState->IsAlive()
    //         || ringingCallState->IsAlive());
    return NOERROR;
}

ECode ImsPhoneBase::HandlePinMmi(
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode ImsPhoneBase::SendUssdResponse(
    /* [in] */ const String& ussdMessge)
{
    return NOERROR;
}

ECode ImsPhoneBase::RegisterForSuppServiceNotification(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return NOERROR;
}

ECode ImsPhoneBase::UnregisterForSuppServiceNotification(
    /* [in] */ IHandler* h)
{
    return NOERROR;
}

ECode ImsPhoneBase::SetRadioPower(
    /* [in] */ Boolean power)
{
    return NOERROR;
}

ECode ImsPhoneBase::GetVoiceMailNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    return NOERROR;
}

ECode ImsPhoneBase::GetVoiceMailAlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    return NOERROR;
}

ECode ImsPhoneBase::GetDeviceId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    return NOERROR;
}

ECode ImsPhoneBase::GetDeviceSvn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    return NOERROR;
}

ECode ImsPhoneBase::GetImei(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    return NOERROR;
}

ECode ImsPhoneBase::GetEsn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::E(LOGTAG, "[VoltePhone] getEsn() is a CDMA method");
    *result = String("0");
    return NOERROR;
}

ECode ImsPhoneBase::GetMeid(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::E(LOGTAG, "[VoltePhone] getMeid() is a CDMA method");
    *result = String("0");
    return NOERROR;
}

ECode ImsPhoneBase::GetSubscriberId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    return NOERROR;
}

ECode ImsPhoneBase::GetGroupIdLevel1(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    return NOERROR;
}

ECode ImsPhoneBase::GetIccSerialNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    return NOERROR;
}

ECode ImsPhoneBase::GetLine1Number(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    return NOERROR;
}

ECode ImsPhoneBase::GetLine1AlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    return NOERROR;
}

ECode ImsPhoneBase::SetLine1Number(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number,
    /* [in] */ IMessage* onComplete)
{
    // FIXME: what to reply for Volte?
    AsyncResult::ForMessage(onComplete, NULL, NULL);
    onComplete->SendToTarget();
    return NOERROR;
}

ECode ImsPhoneBase::SetVoiceMailNumber(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& voiceMailNumber,
    /* [in] */ IMessage* onComplete)
{
    // FIXME: what to reply for Volte?
    AsyncResult::ForMessage(onComplete, NULL, NULL);
    onComplete->SendToTarget();
    return NOERROR;
}

ECode ImsPhoneBase::GetCallForwardingOption(
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ IMessage* onComplete)
{
    return NOERROR;
}

ECode ImsPhoneBase::SetCallForwardingOption(
    /* [in] */ Int32 commandInterfaceCFAction,
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ const String& dialingNumber,
    /* [in] */ Int32 timerSeconds,
    /* [in] */ IMessage* onComplete)
{
    return NOERROR;
}

ECode ImsPhoneBase::SetCallForwardingUncondTimerOption(
    /* [in] */ Int32 startHour,
    /* [in] */ Int32 startMinute,
    /* [in] */ Int32 endHour,
    /* [in] */ Int32 endMinute,
    /* [in] */ Int32 commandInterfaceCFAction,
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ const String& dialingNumber,
    /* [in] */ IMessage* onComplete)
{
    return NOERROR;
}

ECode ImsPhoneBase::GetOutgoingCallerIdDisplay(
    /* [in] */ IMessage* onComplete)
{
    // FIXME: what to reply?
    AsyncResult::ForMessage(onComplete, NULL, NULL);
    onComplete->SendToTarget();
    return NOERROR;
}

ECode ImsPhoneBase::SetOutgoingCallerIdDisplay(
    /* [in] */ Int32 commandInterfaceCLIRMode,
    /* [in] */ IMessage* onComplete)
{
    // FIXME: what's this for Volte?
    AsyncResult::ForMessage(onComplete, NULL, NULL);
    onComplete->SendToTarget();
    return NOERROR;
}

ECode ImsPhoneBase::GetCallWaiting(
    /* [in] */ IMessage* onComplete)
{
    AsyncResult::ForMessage(onComplete, NULL, NULL);
    onComplete->SendToTarget();
    return NOERROR;
}

ECode ImsPhoneBase::SetCallWaiting(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onComplete)
{
    Logger::E(LOGTAG, "call waiting not supported");
    return NOERROR;
}

ECode ImsPhoneBase::GetIccRecordsLoaded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode ImsPhoneBase::GetIccCard(
    /* [out] */ IIccCard** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode ImsPhoneBase::GetAvailableNetworks(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode ImsPhoneBase::SetNetworkSelectionModeAutomatic(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode ImsPhoneBase::SelectNetworkManually(
    /* [in] */ IOperatorInfo* network,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode ImsPhoneBase::GetNeighboringCids(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode ImsPhoneBase::GetDataCallList(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode ImsPhoneBase::GetCurrentDataConnectionList(
    /* [out] */ IList/*<DataConnection>*/** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode ImsPhoneBase::UpdateServiceLocation()
{
    return NOERROR;
}

ECode ImsPhoneBase::EnableLocationUpdates()
{
    return NOERROR;
}

ECode ImsPhoneBase::DisableLocationUpdates()
{
    return NOERROR;
}

ECode ImsPhoneBase::GetDataRoamingEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode ImsPhoneBase::SetDataRoamingEnabled(
    /* [in] */ Boolean enable)
{
    return NOERROR;
}

ECode ImsPhoneBase::GetDataEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode ImsPhoneBase::SetDataEnabled(
    /* [in] */ Boolean enable)
{
    return NOERROR;
}

ECode ImsPhoneBase::EnableDataConnectivity(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode ImsPhoneBase::DisableDataConnectivity(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode ImsPhoneBase::IsDataConnectivityPossible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode ImsPhoneBase::UpdateCurrentCarrierInProvider(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode ImsPhoneBase::SaveClirSetting(
    /* [in] */ Int32 commandInterfaceCLIRMode)
{
    return NOERROR;
}

ECode ImsPhoneBase::GetPhoneSubInfo(
    /* [out] */ IPhoneSubInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode ImsPhoneBase::GetIccPhoneBookInterfaceManager(
    /* [out] */ IIccPhoneBookInterfaceManager** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode ImsPhoneBase::GetIccFileHandler(
    /* [out] */ IIccFileHandler** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode ImsPhoneBase::ActivateCellBroadcastSms(
    /* [in] */ Int32 activate,
    /* [in] */ IMessage* response)
{
    Logger::E(LOGTAG, "Error! This functionality is not implemented for Volte.");
    return NOERROR;
}

ECode ImsPhoneBase::GetCellBroadcastSmsConfig(
    /* [in] */ IMessage* response)
{
    Logger::E(LOGTAG, "Error! This functionality is not implemented for Volte.");
    return NOERROR;
}

ECode ImsPhoneBase::SetCellBroadcastSmsConfig(
    /* [in] */ ArrayOf<Int32>* configValuesArray,
    /* [in] */ IMessage* response)
{
    Logger::E(LOGTAG, "Error! This functionality is not implemented for Volte.");
    return NOERROR;
}

ECode ImsPhoneBase::NeedsOtaServiceProvisioning(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // FIXME: what's this for Volte?
    *result = FALSE;
    return NOERROR;
}

ECode ImsPhoneBase::GetLinkProperties(
    /* [in] */ const String& apnType,
    /* [out] */ ILinkProperties** result)
{
    VALIDATE_NOT_NULL(result);
    // FIXME: what's this for Volte?
    *result = NULL;
    return NOERROR;
}

ECode ImsPhoneBase::UpdatePhoneState()
{
    PhoneConstantsState oldState = mState;

    AutoPtr<ICall> fgc;
    GetForegroundCall((ICall**)&fgc);

    AutoPtr<ICall> bgc;
    GetBackgroundCall((ICall**)&bgc);

    AutoPtr<ICall> call;
    GetRingingCall((ICall**)&call);

    Boolean b1, b2;
    if (call->IsRinging(&b1), b1) {
        mState = PhoneConstantsState_RINGING;
    }
    else if ((fgc->IsIdle(&b1), b1)
            && (bgc->IsIdle(&b2), b2)) {
        mState = PhoneConstantsState_IDLE;
    }
    else {
        mState = PhoneConstantsState_OFFHOOK;
    }

    if (mState != oldState) {
        Logger::D(LOGTAG, " ^^^ new phone state: %d", mState);
        NotifyPhoneStateChanged();
    }
    return NOERROR;
}

void ImsPhoneBase::StartRingbackTone()
{
    AutoPtr<AsyncResult> result = new AsyncResult(NULL, CoreUtils::Convert(TRUE), NULL);
    mRingbackRegistrants->NotifyRegistrants(result);
}

void ImsPhoneBase::StopRingbackTone()
{
    AutoPtr<AsyncResult> result = new AsyncResult(NULL, CoreUtils::Convert(FALSE), NULL);
    mRingbackRegistrants->NotifyRegistrants(result);
}

void ImsPhoneBase::StartOnHoldTone()
{
    AutoPtr<AsyncResult> result = new AsyncResult(NULL, CoreUtils::Convert(TRUE), NULL);
    mOnHoldRegistrants->NotifyRegistrants(result);
}

void ImsPhoneBase::StopOnHoldTone()
{
    AutoPtr<AsyncResult> result = new AsyncResult(NULL, CoreUtils::Convert(FALSE), NULL);
    mOnHoldRegistrants->NotifyRegistrants(result);
}

ECode ImsPhoneBase::OnUpdateIccAvailability()
{
    return NOERROR;
}

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
