
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/sip/SipCommandInterface.h"
#include "elastos/droid/internal/telephony/sip/SipPhoneBase.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/CRegistrantList.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/telephony/CServiceState.h"
#include "elastos/droid/telephony/CSignalStrength.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::CRegistrantList;
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
namespace Sip {

//=====================================================================
//                             SipPhoneBase
//=====================================================================
CAR_INTERFACE_IMPL(SipPhoneBase, PhoneBase, ISipPhoneBase);

const String SipPhoneBase::LOGTAG("SipPhoneBase");

SipPhoneBase::SipPhoneBase()
    : mState(PhoneConstantsState_IDLE)
{
    CRegistrantList::New((IRegistrantList**)&mRingbackRegistrants);
}

ECode SipPhoneBase::constructor(
    /* [in] */ const String& name,
    /* [in] */ IContext* context,
    /* [in] */ IPhoneNotifier* notifier)
{
    AutoPtr<SipCommandInterface> sci = new SipCommandInterface();
    sci->constructor(context);
    return PhoneBase::constructor(name, notifier, context, ICommandsInterface::Probe(sci), FALSE);
}

ECode SipPhoneBase::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ IUUSInfo* uusInfo,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // ignore UUSInfo
// TODO: Need Dial
    // return Dial(dialString, videoState, result);
    return NOERROR;
}

ECode SipPhoneBase::MigrateFrom(
    /* [in] */ ISipPhoneBase* from)
{
    PhoneBase::MigrateFrom(IPhoneBase::Probe(from));
    Migrate(mRingbackRegistrants, ((SipPhoneBase*)from)->mRingbackRegistrants);
    return NOERROR;
}

ECode SipPhoneBase::RegisterForRingbackTone(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    ((CRegistrantList*)mRingbackRegistrants.Get())->AddUnique(h, what, obj);
    return NOERROR;
}

ECode SipPhoneBase::UnregisterForRingbackTone(
    /* [in] */ IHandler* h)
{
    ((CRegistrantList*)mRingbackRegistrants.Get())->Remove(h);
    return NOERROR;
}

ECode SipPhoneBase::GetServiceState(
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

ECode SipPhoneBase::GetCellLocation(
    /* [out] */ ICellLocation** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode SipPhoneBase::GetState(
    /* [out] */ PhoneConstantsState* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mState;
    return NOERROR;
}

ECode SipPhoneBase::GetPhoneType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IPhoneConstants::PHONE_TYPE_SIP;
    return NOERROR;
}

ECode SipPhoneBase::GetSignalStrength(
    /* [out] */ ISignalStrength** result)
{
    VALIDATE_NOT_NULL(result);
    return CSignalStrength::New(result);
}

ECode SipPhoneBase::GetMessageWaitingIndicator(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode SipPhoneBase::GetCallForwardingIndicator(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode SipPhoneBase::GetPendingMmiCodes(
    /* [out] */ IList/*<? extends MmiCode>*/** result)
{
    VALIDATE_NOT_NULL(result);
    return CArrayList::New(0, result);
}

ECode SipPhoneBase::GetDataConnectionState(
    /* [out] */ PhoneConstantsDataState* result)
{
    VALIDATE_NOT_NULL(result);
    *result = PhoneConstantsDataState_DISCONNECTED;
    return NOERROR;
}

ECode SipPhoneBase::GetDataConnectionState(
    /* [in] */ const String& apnType,
    /* [out] */ PhoneConstantsDataState* result)
{
    VALIDATE_NOT_NULL(result);
    *result = PhoneConstantsDataState_DISCONNECTED;
    return NOERROR;
}

ECode SipPhoneBase::GetDataActivityState(
    /* [out] */ IPhoneDataActivityState* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IPhoneDataActivityState_NONE;
    return NOERROR;
}

ECode SipPhoneBase::NotifyPhoneStateChanged()
{
    return mNotifier->NotifyPhoneState(this);
}

ECode SipPhoneBase::NotifyPreciseCallStateChanged()
{
    // /* we'd love it if this was package-scoped*/
    PhoneBase::NotifyPreciseCallStateChangedP();
    return NOERROR;
}

ECode SipPhoneBase::NotifyNewRingingConnection(
    /* [in] */ IConnection* c)
{
    return PhoneBase::NotifyNewRingingConnectionP(c);
}

ECode SipPhoneBase::NotifyDisconnect(
    /* [in] */ IConnection* cn)
{
    mDisconnectRegistrants->NotifyResult(cn);
    return NOERROR;
}

ECode SipPhoneBase::NotifyUnknownConnection()
{
    mUnknownConnectionRegistrants->NotifyResult(ISipPhoneBase::Probe(this));
    return NOERROR;
}

ECode SipPhoneBase::NotifySuppServiceFailed(
    /* [in] */ IPhoneSuppService code)
{
    mSuppServiceFailedRegistrants->NotifyResult(CoreUtils::Convert(code));
    return NOERROR;
}

ECode SipPhoneBase::NotifyServiceStateChanged(
    /* [in] */ IServiceState* ss)
{
    PhoneBase::NotifyServiceStateChangedP(ss);
    return NOERROR;
}

ECode SipPhoneBase::NotifyCallForwardingIndicator()
{
    return mNotifier->NotifyCallForwardingChanged(this);
}

ECode SipPhoneBase::CanDial(
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
            ITelephonyProperties::PROPERTY_DISABLE_CALL, String("false"), &disableCall);
    Logger::V(LOGTAG, "canDial(): disableCall = %s", disableCall.string());
    if (disableCall.Equals("true")) {
        *result = FALSE;
        return NOERROR;
    }

    ICallState foregroundCallState;
    ICallState backgroundCallState;
    ICallState ringingCallState;

    AutoPtr<ICall> call;
    GetRingingCall((ICall**)&call);
    call->GetState(&ringingCallState);

    AutoPtr<ICall> fCall;
    GetForegroundCall((ICall**)&fCall);
    fCall->GetState(&foregroundCallState);

    AutoPtr<ICall> bCall;
    GetBackgroundCall((ICall**)&bCall);
    bCall->GetState(&backgroundCallState);

    Logger::V(LOGTAG, "canDial(): ringingCall: %d" , ringingCallState);
    Logger::V(LOGTAG, "canDial(): foregndCall: %d" , foregroundCallState);
    Logger::V(LOGTAG, "canDial(): backgndCall: %d" , backgroundCallState);

// TODO: Need ICallState::isAlive
    // *result = !getRingingCall().isRinging()
    //         && (!getForegroundCall().getState().isAlive()
    //             || !getBackgroundCall().getState().isAlive());
    return NOERROR;
}

ECode SipPhoneBase::HandleInCallMmiCommands(
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode SipPhoneBase::IsInCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    ICallState foregroundCallState;
    ICallState backgroundCallState;
    ICallState ringingCallState;

    AutoPtr<ICall> call;
    GetRingingCall((ICall**)&call);
    call->GetState(&ringingCallState);

    AutoPtr<ICall> fCall;
    GetForegroundCall((ICall**)&fCall);
    fCall->GetState(&foregroundCallState);

    AutoPtr<ICall> bCall;
    GetBackgroundCall((ICall**)&bCall);
    bCall->GetState(&backgroundCallState);

// TODO: Need ICallState::isAlive
    // *result = (foregroundCallState.isAlive() || backgroundCallState.isAlive()
    //      || ringingCallState.isAlive());
    return NOERROR;
}

ECode SipPhoneBase::HandlePinMmi(
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode SipPhoneBase::SendUssdResponse(
    /* [in] */ const String& ussdMessge)
{
    return NOERROR;
}

ECode SipPhoneBase::RegisterForSuppServiceNotification(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return NOERROR;
}

ECode SipPhoneBase::UnregisterForSuppServiceNotification(
    /* [in] */ IHandler* h)
{
    return NOERROR;
}

ECode SipPhoneBase::SetRadioPower(
    /* [in] */ Boolean power)
{
    return NOERROR;
}

ECode SipPhoneBase::GetVoiceMailNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode SipPhoneBase::GetVoiceMailAlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode SipPhoneBase::GetDeviceId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode SipPhoneBase::GetDeviceSvn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode SipPhoneBase::GetImei(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode SipPhoneBase::GetEsn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::E(LOGTAG, "[SipPhone] getEsn() is a CDMA method");
    *result = String("0");
    return NOERROR;
}

ECode SipPhoneBase::GetMeid(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::E(LOGTAG, "[SipPhone] getMeid() is a CDMA method");
    *result = String("0");
    return NOERROR;
}

ECode SipPhoneBase::GetSubscriberId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode SipPhoneBase::GetGroupIdLevel1(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode SipPhoneBase::GetIccSerialNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode SipPhoneBase::GetLine1Number(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode SipPhoneBase::GetLine1AlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode SipPhoneBase::SetLine1Number(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number,
    /* [in] */ IMessage* onComplete)
{
    // FIXME: what to reply for SIP?
    AsyncResult::ForMessage(onComplete, NULL, NULL);
    return onComplete->SendToTarget();
}

ECode SipPhoneBase::SetVoiceMailNumber(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& voiceMailNumber,
    /* [in] */ IMessage* onComplete)
{
    // FIXME: what to reply for SIP?
    AsyncResult::ForMessage(onComplete, NULL, NULL);
    return onComplete->SendToTarget();
}

ECode SipPhoneBase::GetCallForwardingOption(
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ IMessage* onComplete)
{
    return NOERROR;
}

ECode SipPhoneBase::SetCallForwardingOption(
    /* [in] */ Int32 commandInterfaceCFAction,
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ const String& dialingNumber,
    /* [in] */ Int32 timerSeconds,
    /* [in] */ IMessage* onComplete)
{
    return NOERROR;
}

ECode SipPhoneBase::GetOutgoingCallerIdDisplay(
    /* [in] */ IMessage* onComplete)
{
    // FIXME: what to reply?
    AsyncResult::ForMessage(onComplete, NULL, NULL);
    return onComplete->SendToTarget();
}

ECode SipPhoneBase::SetOutgoingCallerIdDisplay(
    /* [in] */ Int32 commandInterfaceCLIRMode,
    /* [in] */ IMessage* onComplete)
{
    // FIXME: what's this for SIP?
    AsyncResult::ForMessage(onComplete, NULL, NULL);
    return onComplete->SendToTarget();
}

ECode SipPhoneBase::GetCallWaiting(
    /* [in] */ IMessage* onComplete)
{
    AsyncResult::ForMessage(onComplete, NULL, NULL);
    return onComplete->SendToTarget();
}

ECode SipPhoneBase::SetCallWaiting(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onComplete)
{
    Logger::E(LOGTAG, "call waiting not supported");
    return NOERROR;
}

ECode SipPhoneBase::GetIccRecordsLoaded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode SipPhoneBase::GetIccCard(
    /* [out] */ IIccCard** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode SipPhoneBase::GetAvailableNetworks(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipPhoneBase::SetNetworkSelectionModeAutomatic(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipPhoneBase::SelectNetworkManually(
    /* [in] */ IOperatorInfo* network,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipPhoneBase::GetNeighboringCids(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipPhoneBase::SetOnPostDialCharacter(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return NOERROR;
}

ECode SipPhoneBase::GetDataCallList(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode SipPhoneBase::GetCurrentDataConnectionList(
    /* [out] */ IList/*<DataConnection>*/** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode SipPhoneBase::UpdateServiceLocation()
{
    return NOERROR;
}

ECode SipPhoneBase::EnableLocationUpdates()
{
    return NOERROR;
}

ECode SipPhoneBase::DisableLocationUpdates()
{
    return NOERROR;
}

ECode SipPhoneBase::GetDataRoamingEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode SipPhoneBase::SetDataRoamingEnabled(
    /* [in] */ Boolean enable)
{
    return NOERROR;
}

ECode SipPhoneBase::GetDataEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode SipPhoneBase::SetDataEnabled(
    /* [in] */ Boolean enable)
{
    return NOERROR;
}

ECode SipPhoneBase::EnableDataConnectivity(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode SipPhoneBase::DisableDataConnectivity(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode SipPhoneBase::IsDataConnectivityPossible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode SipPhoneBase::UpdateCurrentCarrierInProvider(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode SipPhoneBase::SaveClirSetting(
    /* [in] */ Int32 commandInterfaceCLIRMode)
{
    return NOERROR;
}

ECode SipPhoneBase::GetPhoneSubInfo(
    /* [out] */ IPhoneSubInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode SipPhoneBase::GetIccPhoneBookInterfaceManager(
    /* [out] */ IIccPhoneBookInterfaceManager** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode SipPhoneBase::GetIccFileHandler(
    /* [out] */ IIccFileHandler** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode SipPhoneBase::ActivateCellBroadcastSms(
    /* [in] */ Int32 activate,
    /* [in] */ IMessage* response)
{
    Logger::E(LOGTAG, "Error! This functionality is not implemented for SIP.");
    return NOERROR;
}

ECode SipPhoneBase::GetCellBroadcastSmsConfig(
    /* [in] */ IMessage* response)
{
    Logger::E(LOGTAG, "Error! This functionality is not implemented for SIP.");
    return NOERROR;
}

ECode SipPhoneBase::SetCellBroadcastSmsConfig(
    /* [in] */ ArrayOf<Int32>* configValuesArray,
    /* [in] */ IMessage* response)
{
    Logger::E(LOGTAG, "Error! This functionality is not implemented for SIP.");
    return NOERROR;
}

ECode SipPhoneBase::NeedsOtaServiceProvisioning(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // FIXME: what's this for SIP?
    *result = FALSE;
    return NOERROR;
}

ECode SipPhoneBase::GetLinkProperties(
    /* [in] */ const String& apnType,
    /* [out] */ ILinkProperties** result)
{
    VALIDATE_NOT_NULL(result);
    // FIXME: what's this for SIP?
    *result = NULL;
    return NOERROR;
}

ECode SipPhoneBase::UpdatePhoneState()
{
    PhoneConstantsState oldState = mState;

    AutoPtr<ICall> call;
    GetRingingCall((ICall**)&call);
    Boolean b;
    call->IsRinging(&b);
    AutoPtr<ICall> fCall;
    GetForegroundCall((ICall**)&fCall);
    Boolean b1;
    fCall->IsIdle(&b1);
    AutoPtr<ICall> bCall;
    GetBackgroundCall((ICall**)&bCall);
    Boolean b2;
    bCall->IsIdle(&b2);

    if (b) {
        mState = PhoneConstantsState_RINGING;
    }
    else if (b1 && b2) {
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

void SipPhoneBase::StartRingbackTone()
{
    AutoPtr<AsyncResult> result = new AsyncResult(NULL, CoreUtils::Convert(TRUE), NULL);
    ((CRegistrantList*)mRingbackRegistrants.Get())->NotifyRegistrants(result);
}

void SipPhoneBase::StopRingbackTone()
{
    AutoPtr<AsyncResult> result = new AsyncResult(NULL, CoreUtils::Convert(FALSE), NULL);
    ((CRegistrantList*)mRingbackRegistrants.Get())->NotifyRegistrants(result);
}

ECode SipPhoneBase::OnUpdateIccAvailability()
{
    return NOERROR;
}

} // namespace Sip
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
