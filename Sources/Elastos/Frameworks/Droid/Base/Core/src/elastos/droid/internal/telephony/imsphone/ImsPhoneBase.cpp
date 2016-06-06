
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneBase.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

//=====================================================================
//                             ImsPhoneBase
//=====================================================================
const String ImsPhoneBase::LOGTAG("ImsPhoneBase");

ImsPhoneBase::ImsPhoneBase(
    /* [in] */ const String& name,
    /* [in] */ IContext* context,
    /* [in] */ IPhoneNotifier* notifier)
{
    // ==================before translated======================
    // super(name, notifier, context, new ImsPhoneCommandInterface(context), false);
}

ECode ImsPhoneBase::Dial(
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

ECode ImsPhoneBase::MigrateFrom(
    /* [in] */ IPhoneBase* from)
{
    // ==================before translated======================
    // super.migrateFrom(from);
    // migrate(mRingbackRegistrants, ((ImsPhoneBase)from).mRingbackRegistrants);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::RegisterForRingbackTone(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // mRingbackRegistrants.addUnique(h, what, obj);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::UnregisterForRingbackTone(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // mRingbackRegistrants.remove(h);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::RegisterForOnHoldTone(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // mOnHoldRegistrants.addUnique(h, what, obj);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::UnregisterForOnHoldTone(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // mOnHoldRegistrants.remove(h);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetServiceState(
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

ECode ImsPhoneBase::GetAllCellInfo(
    /* [out] */ IList/*<CellInfo>*/** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getServiceStateTracker().getAllCellInfo();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetCellLocation(
    /* [out] */ ICellLocation** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetState(
    /* [out] */ IPhoneConstants** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mState;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetPhoneType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return PhoneConstants.PHONE_TYPE_IMS;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetSignalStrength(
    /* [out] */ ISignalStrength** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return new SignalStrength();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetMessageWaitingIndicator(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetCallForwardingIndicator(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetPendingMmiCodes(
    /* [out] */ IList/*<? extends MmiCode>*/** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return new ArrayList<MmiCode>(0);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetDataConnectionState(
    /* [out] */ PhoneConstantsDataState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return PhoneConstants.DataState.DISCONNECTED;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetDataConnectionState(
    /* [in] */ const String& apnType,
    /* [out] */ PhoneConstantsDataState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return PhoneConstants.DataState.DISCONNECTED;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetDataActivityState(
    /* [out] */ IPhoneDataActivityState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return DataActivityState.NONE;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::NotifyPhoneStateChanged()
{
    // ==================before translated======================
    // mNotifier.notifyPhoneState(this);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::NotifyPreciseCallStateChanged()
{
    // ==================before translated======================
    // /* we'd love it if this was package-scoped*/
    // super.notifyPreciseCallStateChangedP();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::NotifyDisconnect(
    /* [in] */ IConnection* cn)
{
    // ==================before translated======================
    // mDisconnectRegistrants.notifyResult(cn);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::NotifyUnknownConnection()
{
    // ==================before translated======================
    // mUnknownConnectionRegistrants.notifyResult(this);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::NotifySuppServiceFailed(
    /* [in] */ IPhoneSuppService code)
{
    // ==================before translated======================
    // mSuppServiceFailedRegistrants.notifyResult(code);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::NotifyServiceStateChanged(
    /* [in] */ IServiceState* ss)
{
    // ==================before translated======================
    // super.notifyServiceStateChangedP(ss);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::NotifyCallForwardingIndicator()
{
    // ==================before translated======================
    // mNotifier.notifyCallForwardingChanged(this);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::CanDial(
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

ECode ImsPhoneBase::HandleInCallMmiCommands(
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::IsInCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    //  Call.State foregroundCallState = getForegroundCall().getState();
    //  Call.State backgroundCallState = getBackgroundCall().getState();
    //  Call.State ringingCallState = getRingingCall().getState();
    //
    // return (foregroundCallState.isAlive() || backgroundCallState.isAlive()
    //         || ringingCallState.isAlive());
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::HandlePinMmi(
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::SendUssdResponse(
    /* [in] */ const String& ussdMessge)
{
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::RegisterForSuppServiceNotification(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::UnregisterForSuppServiceNotification(
    /* [in] */ IHandler* h)
{
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::SetRadioPower(
    /* [in] */ Boolean power)
{
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetVoiceMailNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetVoiceMailAlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetDeviceId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetDeviceSvn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetImei(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetEsn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Rlog.e(LOGTAG, "[VoltePhone] getEsn() is a CDMA method");
    // return "0";
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetMeid(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Rlog.e(LOGTAG, "[VoltePhone] getMeid() is a CDMA method");
    // return "0";
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetSubscriberId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetGroupIdLevel1(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetIccSerialNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetLine1Number(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetLine1AlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::SetLine1Number(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // // FIXME: what to reply for Volte?
    // AsyncResult.forMessage(onComplete, null, null);
    // onComplete.sendToTarget();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::SetVoiceMailNumber(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& voiceMailNumber,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // // FIXME: what to reply for Volte?
    // AsyncResult.forMessage(onComplete, null, null);
    // onComplete.sendToTarget();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetCallForwardingOption(
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ IMessage* onComplete)
{
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::SetCallForwardingOption(
    /* [in] */ Int32 commandInterfaceCFAction,
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ const String& dialingNumber,
    /* [in] */ Int32 timerSeconds,
    /* [in] */ IMessage* onComplete)
{
    assert(0);
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
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetOutgoingCallerIdDisplay(
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // // FIXME: what to reply?
    // AsyncResult.forMessage(onComplete, null, null);
    // onComplete.sendToTarget();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::SetOutgoingCallerIdDisplay(
    /* [in] */ Int32 commandInterfaceCLIRMode,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // // FIXME: what's this for Volte?
    // AsyncResult.forMessage(onComplete, null, null);
    // onComplete.sendToTarget();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetCallWaiting(
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // AsyncResult.forMessage(onComplete, null, null);
    // onComplete.sendToTarget();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::SetCallWaiting(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "call waiting not supported");
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetIccRecordsLoaded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetIccCard(
    /* [out] */ IIccCard** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetAvailableNetworks(
    /* [in] */ IMessage* response)
{
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::SetNetworkSelectionModeAutomatic(
    /* [in] */ IMessage* response)
{
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::SelectNetworkManually(
    /* [in] */ IOperatorInfo* network,
    /* [in] */ IMessage* response)
{
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetNeighboringCids(
    /* [in] */ IMessage* response)
{
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetDataCallList(
    /* [in] */ IMessage* response)
{
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetCurrentDataConnectionList(
    /* [out] */ IList/*<DataConnection>*/** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::UpdateServiceLocation()
{
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::EnableLocationUpdates()
{
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::DisableLocationUpdates()
{
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetDataRoamingEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::SetDataRoamingEnabled(
    /* [in] */ Boolean enable)
{
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetDataEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::SetDataEnabled(
    /* [in] */ Boolean enable)
{
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::EnableDataConnectivity(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::DisableDataConnectivity(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::IsDataConnectivityPossible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::UpdateCurrentCarrierInProvider(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::SaveClirSetting(
    /* [in] */ Int32 commandInterfaceCLIRMode)
{
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetPhoneSubInfo(
    /* [out] */ IPhoneSubInfo** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetIccPhoneBookInterfaceManager(
    /* [out] */ IIccPhoneBookInterfaceManager** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetIccFileHandler(
    /* [out] */ IIccFileHandler** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return null;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::ActivateCellBroadcastSms(
    /* [in] */ Int32 activate,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "Error! This functionality is not implemented for Volte.");
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetCellBroadcastSmsConfig(
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "Error! This functionality is not implemented for Volte.");
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::SetCellBroadcastSmsConfig(
    /* [in] */ ArrayOf<Int32>* configValuesArray,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "Error! This functionality is not implemented for Volte.");
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::NeedsOtaServiceProvisioning(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // FIXME: what's this for Volte?
    // return false;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::GetLinkProperties(
    /* [in] */ const String& apnType,
    /* [out] */ ILinkProperties** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // FIXME: what's this for Volte?
    // return null;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneBase::UpdatePhoneState()
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

void ImsPhoneBase::StartRingbackTone()
{
    // ==================before translated======================
    // AsyncResult result = new AsyncResult(null, Boolean.TRUE, null);
    // mRingbackRegistrants.notifyRegistrants(result);
    assert(0);
}

void ImsPhoneBase::StopRingbackTone()
{
    // ==================before translated======================
    // AsyncResult result = new AsyncResult(null, Boolean.FALSE, null);
    // mRingbackRegistrants.notifyRegistrants(result);
    assert(0);
}

void ImsPhoneBase::StartOnHoldTone()
{
    // ==================before translated======================
    // AsyncResult result = new AsyncResult(null, Boolean.TRUE, null);
    // mOnHoldRegistrants.notifyRegistrants(result);
    assert(0);
}

void ImsPhoneBase::StopOnHoldTone()
{
    // ==================before translated======================
    // AsyncResult result = new AsyncResult(null, Boolean.FALSE, null);
    // mOnHoldRegistrants.notifyRegistrants(result);
    assert(0);
}

ECode ImsPhoneBase::OnUpdateIccAvailability()
{
    assert(0);
    return NOERROR;
}

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
