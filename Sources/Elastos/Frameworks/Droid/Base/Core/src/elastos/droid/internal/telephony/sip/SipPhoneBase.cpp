
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/sip/SipPhoneBase.h"

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
{
}

ECode SipPhoneBase::constructor(
    /* [in] */ const String& name,
    /* [in] */ IContext* context,
    /* [in] */ IPhoneNotifier* notifier)
{
    // ==================before translated======================
    // super(name, notifier, context, new SipCommandInterface(context), false);
    return NOERROR;
}

ECode SipPhoneBase::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ IUUSInfo* uusInfo,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // ignore UUSInfo
    // return dial(dialString, videoState);
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::MigrateFrom(
    /* [in] */ ISipPhoneBase* from)
{
    // ==================before translated======================
    // super.migrateFrom(from);
    // migrate(mRingbackRegistrants, from.mRingbackRegistrants);
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::RegisterForRingbackTone(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // mRingbackRegistrants.addUnique(h, what, obj);
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::UnregisterForRingbackTone(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // mRingbackRegistrants.remove(h);
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetServiceState(
    /* [out] */ IServiceState** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // FIXME: we may need to provide this when data connectivity is lost
    // // or when server is down
    // ServiceState s = new ServiceState();
    // s.setState(ServiceState.STATE_IN_SERVICE);
    // return s;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetCellLocation(
    /* [out] */ ICellLocation** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetState(
    /* [out] */ PhoneConstantsState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mState;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetPhoneType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return PhoneConstants.PHONE_TYPE_SIP;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetSignalStrength(
    /* [out] */ ISignalStrength** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return new SignalStrength();
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetMessageWaitingIndicator(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetCallForwardingIndicator(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetPendingMmiCodes(
    /* [out] */ IList/*<? extends MmiCode>*/** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return new ArrayList<MmiCode>(0);
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetDataConnectionState(
    /* [out] */ PhoneConstantsDataState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return PhoneConstants.DataState.DISCONNECTED;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetDataConnectionState(
    /* [in] */ const String& apnType,
    /* [out] */ PhoneConstantsDataState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return PhoneConstants.DataState.DISCONNECTED;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetDataActivityState(
    /* [out] */ IPhoneDataActivityState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return DataActivityState.NONE;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::NotifyPhoneStateChanged()
{
    // ==================before translated======================
    // mNotifier.notifyPhoneState(this);
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::NotifyPreciseCallStateChanged()
{
    // ==================before translated======================
    // /* we'd love it if this was package-scoped*/
    // super.notifyPreciseCallStateChangedP();
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::NotifyNewRingingConnection(
    /* [in] */ IConnection* c)
{
    // ==================before translated======================
    // super.notifyNewRingingConnectionP(c);
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::NotifyDisconnect(
    /* [in] */ IConnection* cn)
{
    // ==================before translated======================
    // mDisconnectRegistrants.notifyResult(cn);
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::NotifyUnknownConnection()
{
    // ==================before translated======================
    // mUnknownConnectionRegistrants.notifyResult(this);
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::NotifySuppServiceFailed(
    /* [in] */ IPhoneSuppService code)
{
    // ==================before translated======================
    // mSuppServiceFailedRegistrants.notifyResult(code);
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::NotifyServiceStateChanged(
    /* [in] */ IServiceState* ss)
{
    // ==================before translated======================
    // super.notifyServiceStateChangedP(ss);
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::NotifyCallForwardingIndicator()
{
    // ==================before translated======================
    // mNotifier.notifyCallForwardingChanged(this);
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::CanDial(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // int serviceState = getServiceState().getState();
    // Rlog.v(LOGTAG, "canDial(): serviceState = " + serviceState);
    // if (serviceState == ServiceState.STATE_POWER_OFF) return false;
    //
    // String disableCall = SystemProperties.get(
    //         TelephonyProperties.PROPERTY_DISABLE_CALL, "false");
    // Rlog.v(LOGTAG, "canDial(): disableCall = " + disableCall);
    // if (disableCall.equals("true")) return false;
    //
    // Rlog.v(LOGTAG, "canDial(): ringingCall: " + getRingingCall().getState());
    // Rlog.v(LOGTAG, "canDial(): foregndCall: " + getForegroundCall().getState());
    // Rlog.v(LOGTAG, "canDial(): backgndCall: " + getBackgroundCall().getState());
    // return !getRingingCall().isRinging()
    //         && (!getForegroundCall().getState().isAlive()
    //             || !getBackgroundCall().getState().isAlive());
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::HandleInCallMmiCommands(
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::IsInCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    //  Call.State foregroundCallState = getForegroundCall().getState();
    //  Call.State backgroundCallState = getBackgroundCall().getState();
    //  Call.State ringingCallState = getRingingCall().getState();
    //
    // return (foregroundCallState.isAlive() || backgroundCallState.isAlive()
    //      || ringingCallState.isAlive());
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::HandlePinMmi(
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::SendUssdResponse(
    /* [in] */ const String& ussdMessge)
{
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::RegisterForSuppServiceNotification(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::UnregisterForSuppServiceNotification(
    /* [in] */ IHandler* h)
{
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::SetRadioPower(
    /* [in] */ Boolean power)
{
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetVoiceMailNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetVoiceMailAlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetDeviceId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetDeviceSvn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetImei(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetEsn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Rlog.e(LOGTAG, "[SipPhone] getEsn() is a CDMA method");
    // return "0";
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetMeid(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Rlog.e(LOGTAG, "[SipPhone] getMeid() is a CDMA method");
    // return "0";
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetSubscriberId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetGroupIdLevel1(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetIccSerialNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetLine1Number(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetLine1AlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::SetLine1Number(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // // FIXME: what to reply for SIP?
    // AsyncResult.forMessage(onComplete, null, null);
    // onComplete.sendToTarget();
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::SetVoiceMailNumber(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& voiceMailNumber,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // // FIXME: what to reply for SIP?
    // AsyncResult.forMessage(onComplete, null, null);
    // onComplete.sendToTarget();
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetCallForwardingOption(
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ IMessage* onComplete)
{
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::SetCallForwardingOption(
    /* [in] */ Int32 commandInterfaceCFAction,
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ const String& dialingNumber,
    /* [in] */ Int32 timerSeconds,
    /* [in] */ IMessage* onComplete)
{
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetOutgoingCallerIdDisplay(
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // // FIXME: what to reply?
    // AsyncResult.forMessage(onComplete, null, null);
    // onComplete.sendToTarget();
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::SetOutgoingCallerIdDisplay(
    /* [in] */ Int32 commandInterfaceCLIRMode,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // // FIXME: what's this for SIP?
    // AsyncResult.forMessage(onComplete, null, null);
    // onComplete.sendToTarget();
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetCallWaiting(
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // AsyncResult.forMessage(onComplete, null, null);
    // onComplete.sendToTarget();
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::SetCallWaiting(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "call waiting not supported");
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetIccRecordsLoaded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetIccCard(
    /* [out] */ IIccCard** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetAvailableNetworks(
    /* [in] */ IMessage* response)
{
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::SetNetworkSelectionModeAutomatic(
    /* [in] */ IMessage* response)
{
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::SelectNetworkManually(
    /* [in] */ IOperatorInfo* network,
    /* [in] */ IMessage* response)
{
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetNeighboringCids(
    /* [in] */ IMessage* response)
{
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::SetOnPostDialCharacter(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetDataCallList(
    /* [in] */ IMessage* response)
{
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetCurrentDataConnectionList(
    /* [out] */ IList/*<DataConnection>*/** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::UpdateServiceLocation()
{
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::EnableLocationUpdates()
{
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::DisableLocationUpdates()
{
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetDataRoamingEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::SetDataRoamingEnabled(
    /* [in] */ Boolean enable)
{
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetDataEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::SetDataEnabled(
    /* [in] */ Boolean enable)
{
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::EnableDataConnectivity(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::DisableDataConnectivity(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::IsDataConnectivityPossible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::UpdateCurrentCarrierInProvider(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::SaveClirSetting(
    /* [in] */ Int32 commandInterfaceCLIRMode)
{
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetPhoneSubInfo(
    /* [out] */ IPhoneSubInfo** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetIccPhoneBookInterfaceManager(
    /* [out] */ IIccPhoneBookInterfaceManager** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetIccFileHandler(
    /* [out] */ IIccFileHandler** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::ActivateCellBroadcastSms(
    /* [in] */ Int32 activate,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "Error! This functionality is not implemented for SIP.");
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetCellBroadcastSmsConfig(
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "Error! This functionality is not implemented for SIP.");
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::SetCellBroadcastSmsConfig(
    /* [in] */ ArrayOf<Int32>* configValuesArray,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "Error! This functionality is not implemented for SIP.");
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::NeedsOtaServiceProvisioning(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // FIXME: what's this for SIP?
    // return false;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::GetLinkProperties(
    /* [in] */ const String& apnType,
    /* [out] */ ILinkProperties** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // FIXME: what's this for SIP?
    // return null;
    assert(0);
    return NOERROR;
}

ECode SipPhoneBase::UpdatePhoneState()
{
    // ==================before translated======================
    // PhoneConstants.State oldState = mState;
    //
    // if (getRingingCall().isRinging()) {
    //     mState = PhoneConstants.State.RINGING;
    // } else if (getForegroundCall().isIdle()
    //         && getBackgroundCall().isIdle()) {
    //     mState = PhoneConstants.State.IDLE;
    // } else {
    //     mState = PhoneConstants.State.OFFHOOK;
    // }
    //
    // if (mState != oldState) {
    //     Rlog.d(LOGTAG, " ^^^ new phone state: " + mState);
    //     notifyPhoneStateChanged();
    // }
    assert(0);
    return NOERROR;
}

void SipPhoneBase::StartRingbackTone()
{
    // ==================before translated======================
    // AsyncResult result = new AsyncResult(null, Boolean.TRUE, null);
    // mRingbackRegistrants.notifyRegistrants(result);
    assert(0);
}

void SipPhoneBase::StopRingbackTone()
{
    // ==================before translated======================
    // AsyncResult result = new AsyncResult(null, Boolean.FALSE, null);
    // mRingbackRegistrants.notifyRegistrants(result);
    assert(0);
}

ECode SipPhoneBase::OnUpdateIccAvailability()
{
    assert(0);
    return NOERROR;
}

} // namespace Sip
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
