#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"

#include "elastos/droid/internal/telephony/cdma/CDMAPhone.h"

using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                       CDMAPhone::InnerRunnable
//=====================================================================
CDMAPhone::InnerRunnable::InnerRunnable(
    /* [in] */ CDMAPhone* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode CDMAPhone::InnerRunnable::Run()
{
    // ==================before translated======================
    // exitEmergencyCallbackMode();
    assert(0);
    return NOERROR;
}

//=====================================================================
//                              CDMAPhone
//=====================================================================
CAR_INTERFACE_IMPL(CDMAPhone, PhoneBase, ICDMAPhone);

const String CDMAPhone::LOGTAG("CDMAPhone");
const Int32 CDMAPhone::RESTART_ECM_TIMER;
const Int32 CDMAPhone::CANCEL_ECM_TIMER;
const Boolean CDMAPhone::DBG = true;
const Boolean CDMAPhone::VDBG = false;
const Int32 CDMAPhone::DEFAULT_ECM_EXIT_TIMER_VALUE;
const String CDMAPhone::VM_NUMBER_CDMA("vm_number_key_cdma");
const String CDMAPhone::IS683A_FEATURE_CODE("*228");
const Int32 CDMAPhone::IS683A_FEATURE_CODE_NUM_DIGITS;
const Int32 CDMAPhone::IS683A_SYS_SEL_CODE_NUM_DIGITS;
const Int32 CDMAPhone::IS683A_SYS_SEL_CODE_OFFSET;
const Int32 CDMAPhone::IS683_CONST_800MHZ_A_BAND;
const Int32 CDMAPhone::IS683_CONST_800MHZ_B_BAND;
const Int32 CDMAPhone::IS683_CONST_1900MHZ_A_BLOCK;
const Int32 CDMAPhone::IS683_CONST_1900MHZ_B_BLOCK;
const Int32 CDMAPhone::IS683_CONST_1900MHZ_C_BLOCK;
const Int32 CDMAPhone::IS683_CONST_1900MHZ_D_BLOCK;
const Int32 CDMAPhone::IS683_CONST_1900MHZ_E_BLOCK;
const Int32 CDMAPhone::IS683_CONST_1900MHZ_F_BLOCK;
const Int32 CDMAPhone::INVALID_SYSTEM_SELECTION_CODE;

static AutoPtr<IPattern> InitPattern(
    /* [in] */ const String& regex)
{
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);

    AutoPtr<IPattern> pattern;
    helper->Compile(regex, (IPattern**)&pattern);
    return pattern;
}
AutoPtr<IPattern> CDMAPhone::pOtaSpNumSchema = InitPattern(String("[,\\s]+"));

CDMAPhone::CDMAPhone()
{
}

ECode CDMAPhone::constructor()
{
    return NOERROR;
}

ECode CDMAPhone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IPhoneNotifier* notifier)
{
    // ==================before translated======================
    // super("CDMA", notifier, context, ci, false);
    // initSstIcc();
    // init(context, notifier);
    return NOERROR;
}

ECode CDMAPhone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IPhoneNotifier* notifier,
    /* [in] */ Int32 phoneId)
{
    // ==================before translated======================
    // super("CDMA", notifier, context, ci, false, phoneId);
    // initSstIcc();
    // init(context, notifier);
    return NOERROR;
}

ECode CDMAPhone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IPhoneNotifier* notifier,
    /* [in] */ Boolean unitTestMode)
{
    // ==================before translated======================
    // super("CDMA", notifier, context, ci, unitTestMode);
    // initSstIcc();
    // init(context, notifier);
    return NOERROR;
}

ECode CDMAPhone::Dispose()
{
    // ==================before translated======================
    // synchronized(PhoneProxy.lockForRadioTechnologyChange) {
    //     super.dispose();
    //     log("dispose");
    //
    //     //Unregister from all former registered events
    //     unregisterForRuimRecordEvents();
    //     mCi.unregisterForAvailable(this); //EVENT_RADIO_AVAILABLE
    //     mCi.unregisterForOffOrNotAvailable(this); //EVENT_RADIO_OFF_OR_NOT_AVAILABLE
    //     mCi.unregisterForOn(this); //EVENT_RADIO_ON
    //     mSST.unregisterForNetworkAttached(this); //EVENT_REGISTERED_TO_NETWORK
    //     mCi.unSetOnSuppServiceNotification(this);
    //     mCi.unregisterForExitEmergencyCallbackMode(this);
    //     removeCallbacks(mExitEcmRunnable);
    //
    //     mPendingMmis.clear();
    //
    //     //Force all referenced classes to unregister their former registered events
    //     mCT.dispose();
    //     mDcTracker.dispose();
    //     mSST.dispose();
    //     mCdmaSSM.dispose(this);
    //     mRuimPhoneBookInterfaceManager.dispose();
    //     mSubInfo.dispose();
    //     mEriManager.dispose();
    // }
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::RemoveReferences()
{
    // ==================before translated======================
    // log("removeReferences");
    // mRuimPhoneBookInterfaceManager = null;
    // mSubInfo = null;
    // mCT = null;
    // mSST = null;
    // mEriManager = null;
    // mExitEcmRunnable = null;
    //
    // super.removeReferences();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetServiceState(
    /* [out] */ IServiceState** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // if (mSST == null || mSST.mSS.getState() != ServiceState.STATE_IN_SERVICE) {
    //     if (mImsPhone != null &&
    //             mImsPhone.getServiceState().getState() == ServiceState.STATE_IN_SERVICE) {
    //         return mImsPhone.getServiceState();
    //     }
    // }
    //
    // if (mSST != null) {
    //     return mSST.mSS;
    // } else {
    //     // avoid potential NPE in EmergencyCallHelper during Phone switch
    //     return new ServiceState();
    // }
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetCallTracker(
    /* [out] */ ICallTracker** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mCT;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetState(
    /* [out] */ PhoneConstantsState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCT.mState;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetServiceStateTracker(
    /* [out] */ IServiceStateTracker** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mSST;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetPhoneType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return PhoneConstants.PHONE_TYPE_CDMA;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::CanTransfer(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // Rlog.e(LOGTAG, "canTransfer: not possible in CDMA");
    // return false;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetRingingCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // ImsPhone imPhone = mImsPhone;
    // if ( mCT.mRingingCall != null && mCT.mRingingCall.isRinging() ) {
    //     return mCT.mRingingCall;
    // } else if ( imPhone != null ) {
    //     return imPhone.getRingingCall();
    // }
    // return mCT.mRingingCall;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SetUiTTYMode(
    /* [in] */ Int32 uiTtyMode,
    /* [in] */ IMessage* onComplete)
{
    VALIDATE_NOT_NULL(onComplete);
    // ==================before translated======================
    // if (mImsPhone != null) {
    //     mImsPhone.setUiTTYMode(uiTtyMode, onComplete);
    // }
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SetMute(
    /* [in] */ Boolean muted)
{
    // ==================before translated======================
    // mCT.setMute(muted);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mCT.getMute();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::Conference()
{
    // ==================before translated======================
    // if (mImsPhone != null && mImsPhone.canConference()) {
    //     log("conference() - delegated to IMS phone");
    //     mImsPhone.conference();
    //     return;
    // }
    // // three way calls in CDMA will be handled by feature codes
    // Rlog.e(LOGTAG, "conference: not possible in CDMA");
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::EnableEnhancedVoicePrivacy(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onComplete)
{
    VALIDATE_NOT_NULL(onComplete);
    // ==================before translated======================
    // mCi.setPreferredVoicePrivacy(enable, onComplete);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetEnhancedVoicePrivacy(
    /* [in] */ IMessage* onComplete)
{
    VALIDATE_NOT_NULL(onComplete);
    // ==================before translated======================
    // mCi.getPreferredVoicePrivacy(onComplete);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::ClearDisconnected()
{
    // ==================before translated======================
    // mCT.clearDisconnected();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetDataActivityState(
    /* [out] */ IPhoneDataActivityState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // DataActivityState ret = DataActivityState.NONE;
    //
    // if (mSST.getCurrentDataConnectionState() == ServiceState.STATE_IN_SERVICE) {
    //
    //     switch (mDcTracker.getActivity()) {
    //         case DATAIN:
    //             ret = DataActivityState.DATAIN;
    //         break;
    //
    //         case DATAOUT:
    //             ret = DataActivityState.DATAOUT;
    //         break;
    //
    //         case DATAINANDOUT:
    //             ret = DataActivityState.DATAINANDOUT;
    //         break;
    //
    //         case DORMANT:
    //             ret = DataActivityState.DORMANT;
    //         break;
    //
    //         default:
    //             ret = DataActivityState.NONE;
    //         break;
    //     }
    // }
    // return ret;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return (dial(dialString, videoState, null));
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [in] */ IBundle* extras,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // ImsPhone imsPhone = mImsPhone;
    //
    // boolean imsUseEnabled =
    //         ImsManager.isEnhanced4gLteModeSettingEnabledByPlatform(mContext) &&
    //         ImsManager.isEnhanced4gLteModeSettingEnabledByUser(mContext);
    // if (!imsUseEnabled) {
    //     Rlog.w(LOGTAG, "IMS is disabled: forced to CS");
    // }
    //
    // if (imsUseEnabled && imsPhone != null
    //         && ((imsPhone.getServiceState().getState() == ServiceState.STATE_IN_SERVICE
    //         && !PhoneNumberUtils.isEmergencyNumber(dialString))
    //         || (PhoneNumberUtils.isEmergencyNumber(dialString)
    //         && mContext.getResources().getBoolean(
    //                 com.android.internal.R.bool.useImsAlwaysForEmergencyCall))) ) {
    //     try {
    //         if (DBG) Rlog.d(LOGTAG, "Trying IMS PS call");
    //         return imsPhone.dial(dialString, videoState, extras);
    //     } catch (CallStateException e) {
    //         if (DBG) Rlog.d(LOGTAG, "IMS PS call exception " + e);
    //         if (!ImsPhone.CS_FALLBACK.equals(e.getMessage())) {
    //             CallStateException ce = new CallStateException(e.getMessage());
    //             ce.setStackTrace(e.getStackTrace());
    //             throw ce;
    //         }
    //     }
    // }
    //
    // if (DBG) Rlog.d(LOGTAG, "Trying (non-IMS) CS call");
    // return dialInternal(dialString, null, videoState);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ IUUSInfo* uusInfo,
    /* [in] */ Int32 videoState,
    /* [in] */ IBundle* extras,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(uusInfo);
    VALIDATE_NOT_NULL(extras);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return dial(dialString, uusInfo, videoState, null);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ IUUSInfo* uusInfo,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // throw new CallStateException("Sending UUS information NOT supported in CDMA!");
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetPendingMmiCodes(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mPendingMmis;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::RegisterForSuppServiceNotification(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    VALIDATE_NOT_NULL(h);
    VALIDATE_NOT_NULL(obj);
    // ==================before translated======================
    // Rlog.e(LOGTAG, "method registerForSuppServiceNotification is NOT supported in CDMA!");
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetBackgroundCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mCT.mBackgroundCall;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::HandleInCallMmiCommands(
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // Rlog.e(LOGTAG, "method handleInCallMmiCommands is NOT supported in CDMA!");
    // return false;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::IsInCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // CdmaCall.State foregroundCallState = getForegroundCall().getState();
    // CdmaCall.State backgroundCallState = getBackgroundCall().getState();
    // CdmaCall.State ringingCallState = getRingingCall().getState();
    //
    // return (foregroundCallState.isAlive() || backgroundCallState.isAlive() || ringingCallState
    //         .isAlive());
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SetNetworkSelectionModeAutomatic(
    /* [in] */ IMessage* response)
{
    VALIDATE_NOT_NULL(response);
    // ==================before translated======================
    // Rlog.e(LOGTAG, "method setNetworkSelectionModeAutomatic is NOT supported in CDMA!");
    // if (response != null) {
    //     Rlog.e(LOGTAG,
    //             "setNetworkSelectionModeAutomatic: not possible in CDMA- Posting exception");
    //     CommandException ce = new CommandException(
    //             CommandException.Error.REQUEST_NOT_SUPPORTED);
    //     AsyncResult.forMessage(response).exception = ce;
    //     response.sendToTarget();
    // }
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::UnregisterForSuppServiceNotification(
    /* [in] */ IHandler* h)
{
    VALIDATE_NOT_NULL(h);
    // ==================before translated======================
    // Rlog.e(LOGTAG, "method unregisterForSuppServiceNotification is NOT supported in CDMA!");
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::AcceptCall(
    /* [in] */ Int32 videoState)
{
    // ==================before translated======================
    // ImsPhone imsPhone = mImsPhone;
    // if ( imsPhone != null && imsPhone.getRingingCall().isRinging() ) {
    //     imsPhone.acceptCall(videoState);
    // } else {
    //     mCT.acceptCall();
    // }
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::DeflectCall(
    /* [in] */ const String& number)
{
    // ==================before translated======================
    // ImsPhone imsPhone = mImsPhone;
    // if ( imsPhone != null && imsPhone.getRingingCall().isRinging() ) {
    //     imsPhone.deflectCall(number);
    // } else {
    //     throw new CallStateException("Deflect call NOT supported in CDMA!");
    // }
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::RejectCall()
{
    // ==================before translated======================
    // mCT.rejectCall();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SwitchHoldingAndActive()
{
    // ==================before translated======================
    // mCT.switchWaitingOrHoldingAndActive();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetIccSerialNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // IccRecords r = mIccRecords.get();
    // if (r == null) {
    //     // to get ICCID form SIMRecords because it is on MF.
    //     r = mUiccController.getIccRecords(mPhoneId, UiccController.APP_FAM_3GPP);
    // }
    // return (r != null) ? r.getIccId() : null;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetLine1Number(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mSST.getMdnNumber();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetCdmaPrlVersion(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mSST.getPrlVersion();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetCdmaMin(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mSST.getCdmaMin();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::IsMinInfoReady(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mSST.isMinInfoReady();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetCallWaiting(
    /* [in] */ IMessage* onComplete)
{
    VALIDATE_NOT_NULL(onComplete);
    // ==================before translated======================
    // mCi.queryCallWaiting(CommandsInterface.SERVICE_CLASS_VOICE, onComplete);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SetRadioPower(
    /* [in] */ Boolean power)
{
    // ==================before translated======================
    // mSST.setRadioPower(power);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetEsn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mEsn;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetMeid(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mMeid;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetDeviceId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // String id = getMeid();
    // if ((id == null) || id.matches("^0*$")) {
    //     Rlog.d(LOGTAG, "getDeviceId(): MEID is not initialized use ESN");
    //     id = getEsn();
    // }
    // return id;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetDeviceSvn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // Rlog.d(LOGTAG, "getDeviceSvn(): return 0");
    // return "0";
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetSubscriberId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mSST.getImsi();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetGroupIdLevel1(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // Rlog.e(LOGTAG, "GID1 is not available in CDMA");
    // return null;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetImei(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // Rlog.e(LOGTAG, "getImei() called for CDMAPhone");
    // return mImei;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::CanConference(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // if (mImsPhone != null && mImsPhone.canConference()) {
    //     return true;
    // }
    // Rlog.e(LOGTAG, "canConference: not possible in CDMA");
    // return false;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetCellLocation(
    /* [out] */ ICellLocation** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // CdmaCellLocation loc = mSST.mCellLoc;
    //
    // int mode = Settings.Secure.getInt(getContext().getContentResolver(),
    //         Settings.Secure.LOCATION_MODE, Settings.Secure.LOCATION_MODE_OFF);
    // if (mode == Settings.Secure.LOCATION_MODE_OFF) {
    //     // clear lat/long values for location privacy
    //     CdmaCellLocation privateLoc = new CdmaCellLocation();
    //     privateLoc.setCellLocationData(loc.getBaseStationId(),
    //             CdmaCellLocation.INVALID_LAT_LONG,
    //             CdmaCellLocation.INVALID_LAT_LONG,
    //             loc.getSystemId(), loc.getNetworkId());
    //     loc = privateLoc;
    // }
    // return loc;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetForegroundCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mCT.mForegroundCall;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SelectNetworkManually(
    /* [in] */ IOperatorInfo* network,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "selectNetworkManually: not possible in CDMA");
    // if (response != null) {
    //     CommandException ce = new CommandException(
    //             CommandException.Error.REQUEST_NOT_SUPPORTED);
    //     AsyncResult.forMessage(response).exception = ce;
    //     response.sendToTarget();
    // }
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SetOnPostDialCharacter(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // mPostDialHandler = new Registrant(h, what, obj);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::HandlePinMmi(
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // CdmaMmiCode mmi = CdmaMmiCode.newFromDialString(dialString, this, mUiccApplication.get());
    //
    // if (mmi == null) {
    //     Rlog.e(LOGTAG, "Mmi is NULL!");
    //     return false;
    // } else if (mmi.isPinPukCommand()) {
    //     mPendingMmis.add(mmi);
    //     mMmiRegistrants.notifyRegistrants(new AsyncResult(null, mmi, null));
    //     mmi.processCode();
    //     return true;
    // }
    // Rlog.e(LOGTAG, "Unrecognized mmi!");
    // return false;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::OnMMIDone(
    /* [in] */ ICdmaMmiCode* mmi)
{
    // ==================before translated======================
    // /*
    //  * Only notify complete if it's on the pending list. Otherwise, it's
    //  * already been handled (eg, previously canceled).
    //  */
    // if (mPendingMmis.remove(mmi)) {
    //     mMmiCompleteRegistrants.notifyRegistrants(new AsyncResult(null, mmi, null));
    // }
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SetLine1Number(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "setLine1Number: not possible in CDMA");
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SetCallWaiting(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "method setCallWaiting is NOT supported in CDMA!");
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::UpdateServiceLocation()
{
    // ==================before translated======================
    // mSST.enableSingleLocationUpdate();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SetDataRoamingEnabled(
    /* [in] */ Boolean enable)
{
    // ==================before translated======================
    // mDcTracker.setDataOnRoamingEnabled(enable);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::RegisterForCdmaOtaStatusChange(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // mCi.registerForCdmaOtaProvision(h, what, obj);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::UnregisterForCdmaOtaStatusChange(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // mCi.unregisterForCdmaOtaProvision(h);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::RegisterForSubscriptionInfoReady(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // mSST.registerForSubscriptionInfoReady(h, what, obj);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::UnregisterForSubscriptionInfoReady(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // mSST.unregisterForSubscriptionInfoReady(h);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SetOnEcbModeExitResponse(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // mEcmExitRespRegistrant = new Registrant (h, what, obj);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::UnsetOnEcbModeExitResponse(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // mEcmExitRespRegistrant.clear();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::RegisterForCallWaiting(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // mCT.registerForCallWaiting(h, what, obj);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::UnregisterForCallWaiting(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // mCT.unregisterForCallWaiting(h);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetNeighboringCids(
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // /*
    //  * This is currently not implemented.  At least as of June
    //  * 2009, there is no neighbor cell information available for
    //  * CDMA because some party is resisting making this
    //  * information readily available.  Consequently, calling this
    //  * function can have no useful effect.  This situation may
    //  * (and hopefully will) change in the future.
    //  */
    // if (response != null) {
    //     CommandException ce = new CommandException(
    //             CommandException.Error.REQUEST_NOT_SUPPORTED);
    //     AsyncResult.forMessage(response).exception = ce;
    //     response.sendToTarget();
    // }
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetDataConnectionState(
    /* [in] */ const String& apnType,
    /* [out] */ PhoneConstantsState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // PhoneConstants.DataState ret = PhoneConstants.DataState.DISCONNECTED;
    //
    // if (mSST == null) {
    //      // Radio Technology Change is ongoning, dispose() and removeReferences() have
    //      // already been called
    //
    //      ret = PhoneConstants.DataState.DISCONNECTED;
    // } else if (mSST.getCurrentDataConnectionState() != ServiceState.STATE_IN_SERVICE
    //         && mOosIsDisconnect) {
    //     // If we're out of service, open TCP sockets may still work
    //     // but no data will flow
    //     ret = PhoneConstants.DataState.DISCONNECTED;
    //     log("getDataConnectionState: Data is Out of Service. ret = " + ret);
    // } else if (mDcTracker.isApnTypeEnabled(apnType) == false ||
    //         mDcTracker.isApnTypeActive(apnType) == false) {
    //     ret = PhoneConstants.DataState.DISCONNECTED;
    // } else {
    //     switch (mDcTracker.getState(apnType)) {
    //         case RETRYING:
    //         case FAILED:
    //         case IDLE:
    //             ret = PhoneConstants.DataState.DISCONNECTED;
    //         break;
    //
    //         case CONNECTED:
    //         case DISCONNECTING:
    //             if ( mCT.mState != PhoneConstants.State.IDLE
    //                     && !mSST.isConcurrentVoiceAndDataAllowed()) {
    //                 ret = PhoneConstants.DataState.SUSPENDED;
    //             } else {
    //                 ret = PhoneConstants.DataState.CONNECTED;
    //             }
    //         break;
    //
    //         case CONNECTING:
    //         case SCANNING:
    //             ret = PhoneConstants.DataState.CONNECTING;
    //         break;
    //     }
    // }
    //
    // log("getDataConnectionState apnType=" + apnType + " ret=" + ret);
    // return ret;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SendUssdResponse(
    /* [in] */ const String& ussdMessge)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "sendUssdResponse: not possible in CDMA");
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SendDtmf(
    /* [in] */ Char32 c)
{
    // ==================before translated======================
    // if (!PhoneNumberUtils.is12Key(c)) {
    //     Rlog.e(LOGTAG,
    //             "sendDtmf called with invalid character '" + c + "'");
    // } else {
    //     if (mCT.mState ==  PhoneConstants.State.OFFHOOK) {
    //         mCi.sendDtmf(c, null);
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::StartDtmf(
    /* [in] */ Char32 c)
{
    // ==================before translated======================
    // if (!PhoneNumberUtils.is12Key(c)) {
    //     Rlog.e(LOGTAG,
    //             "startDtmf called with invalid character '" + c + "'");
    // } else {
    //     mCi.startDtmf(c, null);
    // }
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::StopDtmf()
{
    // ==================before translated======================
    // mCi.stopDtmf(null);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SendBurstDtmf(
    /* [in] */ const String& dtmfString,
    /* [in] */ Int32 on,
    /* [in] */ Int32 off,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // boolean check = true;
    // for (int itr = 0;itr < dtmfString.length(); itr++) {
    //     if (!PhoneNumberUtils.is12Key(dtmfString.charAt(itr))) {
    //         Rlog.e(LOGTAG,
    //                 "sendDtmf called with invalid character '" + dtmfString.charAt(itr)+ "'");
    //         check = false;
    //         break;
    //     }
    // }
    // if ((mCT.mState ==  PhoneConstants.State.OFFHOOK)&&(check)) {
    //     mCi.sendBurstDtmf(dtmfString, on, off, onComplete);
    // }
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetAvailableNetworks(
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "getAvailableNetworks: not possible in CDMA");
    // if (response != null) {
    //     CommandException ce = new CommandException(
    //             CommandException.Error.REQUEST_NOT_SUPPORTED);
    //     AsyncResult.forMessage(response).exception = ce;
    //     response.sendToTarget();
    // }
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SetOutgoingCallerIdDisplay(
    /* [in] */ Int32 commandInterfaceCLIRMode,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "setOutgoingCallerIdDisplay: not possible in CDMA");
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::EnableLocationUpdates()
{
    // ==================before translated======================
    // mSST.enableLocationUpdates();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::DisableLocationUpdates()
{
    // ==================before translated======================
    // mSST.disableLocationUpdates();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetDataCallList(
    /* [in] */ IMessage* response)
{
    VALIDATE_NOT_NULL(response);
    // ==================before translated======================
    // mCi.getDataCallList(response);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetDataRoamingEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mDcTracker.getDataOnRoamingEnabled();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SetDataEnabled(
    /* [in] */ Boolean enable)
{
    // ==================before translated======================
    // mDcTracker.setDataEnabled(enable);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetDataEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mDcTracker.getDataEnabled();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SetVoiceMailNumber(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& voiceMailNumber,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // Message resp;
    // mVmNumber = voiceMailNumber;
    // resp = obtainMessage(EVENT_SET_VM_NUMBER_DONE, 0, 0, onComplete);
    // IccRecords r = mIccRecords.get();
    // if (r != null) {
    //     r.setVoiceMailNumber(alphaTag, mVmNumber, resp);
    // }
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetVoiceMailNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // String number = null;
    // SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(getContext());
    // number = sp.getString(VM_NUMBER_CDMA + getSubId(), null);
    // if (TextUtils.isEmpty(number)) {
    //     String[] listArray = getContext().getResources()
    //         .getStringArray(com.android.internal.R.array.config_default_vm_number);
    //     if (listArray != null && listArray.length > 0) {
    //         for (int i=0; i<listArray.length; i++) {
    //             if (!TextUtils.isEmpty(listArray[i])) {
    //                 String[] defaultVMNumberArray = listArray[i].split(";");
    //                 if (defaultVMNumberArray != null && defaultVMNumberArray.length > 0) {
    //                     if (defaultVMNumberArray.length == 1) {
    //                         number = defaultVMNumberArray[0];
    //                     } else if (defaultVMNumberArray.length == 2 &&
    //                             !TextUtils.isEmpty(defaultVMNumberArray[1]) &&
    //                             defaultVMNumberArray[1].equalsIgnoreCase(getGroupIdLevel1())) {
    //                         number = defaultVMNumberArray[0];
    //                         break;
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }
    // if (TextUtils.isEmpty(number)) {
    //     // Read platform settings for dynamic voicemail number
    //     if (getContext().getResources().getBoolean(com.android.internal
    //             .R.bool.config_telephony_use_own_number_for_voicemail)) {
    //         number = getLine1Number();
    //     } else {
    //         number = "*86";
    //     }
    // }
    // return number;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetVoiceMailAlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // // TODO: Where can we get this value has to be clarified with QC.
    // String ret = "";//TODO: Remove = "", if we know where to get this value.
    //
    // //ret = mSIMRecords.getVoiceMailAlphaTag();
    //
    // if (ret == null || ret.length() == 0) {
    //     return mContext.getText(
    //         com.android.internal.R.string.defaultVoiceMailAlphaTag).toString();
    // }
    //
    // return ret;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetCallForwardingOption(
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "getCallForwardingOption: not possible in CDMA");
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SetCallForwardingOption(
    /* [in] */ Int32 commandInterfaceCFAction,
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ const String& dialingNumber,
    /* [in] */ Int32 timerSeconds,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "setCallForwardingOption: not possible in CDMA");
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetCallForwardingUncondTimerOption(
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // ImsPhone imsPhone = mImsPhone;
    // if (imsPhone != null) {
    //     imsPhone.getCallForwardingOption(commandInterfaceCFReason, onComplete);
    // } else {
    //     if (onComplete != null) {
    //         AsyncResult.forMessage(onComplete, null,
    //                 new CommandException(CommandException.Error.GENERIC_FAILURE));
    //         onComplete.sendToTarget();
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SetCallForwardingUncondTimerOption(
    /* [in] */ Int32 startHour,
    /* [in] */ Int32 startMinute,
    /* [in] */ Int32 endHour,
    /* [in] */ Int32 endMinute,
    /* [in] */ Int32 commandInterfaceCFAction,
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ const String& dialingNumber,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // ImsPhone imsPhone = mImsPhone;
    // if (imsPhone != null) {
    //     imsPhone.setCallForwardingUncondTimerOption(startHour, startMinute, endHour,
    //             endMinute, commandInterfaceCFAction, commandInterfaceCFReason,
    //             dialingNumber, onComplete);
    // } else {
    //     if (onComplete != null) {
    //         AsyncResult.forMessage(onComplete, null,
    //                 new CommandException(CommandException.Error.GENERIC_FAILURE));
    //         onComplete.sendToTarget();
    //    }
    // }
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetOutgoingCallerIdDisplay(
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "getOutgoingCallerIdDisplay: not possible in CDMA");
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetCallForwardingIndicator(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // Rlog.e(LOGTAG, "getCallForwardingIndicator: not possible in CDMA");
    // return false;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::ExplicitCallTransfer()
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "explicitCallTransfer: not possible in CDMA");
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetLine1AlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // Rlog.e(LOGTAG, "getLine1AlphaTag: not possible in CDMA");
    // return null;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::NotifyPhoneStateChanged()
{
    // ==================before translated======================
    // mNotifier.notifyPhoneState(this);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::NotifyPreciseCallStateChanged()
{
    // ==================before translated======================
    // /* we'd love it if this was package-scoped*/
    // super.notifyPreciseCallStateChangedP();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::NotifyServiceStateChanged(
    /* [in] */ IServiceState* ss)
{
    VALIDATE_NOT_NULL(ss);
    // ==================before translated======================
    // super.notifyServiceStateChangedP(ss);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::NotifyLocationChanged()
{
    // ==================before translated======================
    // mNotifier.notifyCellLocation(this);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::NotifyNewRingingConnection(
    /* [in] */ IConnection* c)
{
    VALIDATE_NOT_NULL(c);
    // ==================before translated======================
    // super.notifyNewRingingConnectionP(c);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::NotifyDisconnect(
    /* [in] */ IConnection* cn)
{
    VALIDATE_NOT_NULL(cn);
    // ==================before translated======================
    // mDisconnectRegistrants.notifyResult(cn);
    //
    // mNotifier.notifyDisconnectCause(cn.getDisconnectCause(), cn.getPreciseDisconnectCause());
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::NotifyUnknownConnection(
    /* [in] */ IConnection* connection)
{
    VALIDATE_NOT_NULL(connection);
    // ==================before translated======================
    // mUnknownConnectionRegistrants.notifyResult(connection);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::IsInEmergencyCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mCT.isInEmergencyCall();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::IsInEcm(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mIsPhoneInEcmState;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SendEmergencyCallbackModeChange()
{
    // ==================before translated======================
    // //Send an Intent
    // Intent intent = new Intent(TelephonyIntents.ACTION_EMERGENCY_CALLBACK_MODE_CHANGED);
    // intent.putExtra(PhoneConstants.PHONE_IN_ECM_STATE, mIsPhoneInEcmState);
    // SubscriptionManager.putPhoneIdAndSubIdExtra(intent, getPhoneId());
    // ActivityManagerNative.broadcastStickyIntent(intent,null,UserHandle.USER_ALL);
    // if (DBG) Rlog.d(LOGTAG, "sendEmergencyCallbackModeChange");
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::ExitEmergencyCallbackMode()
{
    // ==================before translated======================
    // if (mWakeLock.isHeld()) {
    //     mWakeLock.release();
    // }
    // // Send a message which will invoke handleExitEmergencyCallbackMode
    // mCi.exitEmergencyCallbackMode(obtainMessage(EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE));
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::HandleTimerInEmergencyCallbackMode(
    /* [in] */ Int32 action)
{
    // ==================before translated======================
    // switch(action) {
    // case CANCEL_ECM_TIMER:
    //     removeCallbacks(mExitEcmRunnable);
    //     mEcmTimerResetRegistrants.notifyResult(Boolean.TRUE);
    //     break;
    // case RESTART_ECM_TIMER:
    //     long delayInMillis = SystemProperties.getLong(
    //             TelephonyProperties.PROPERTY_ECM_EXIT_TIMER, DEFAULT_ECM_EXIT_TIMER_VALUE);
    //     postDelayed(mExitEcmRunnable, delayInMillis);
    //     mEcmTimerResetRegistrants.notifyResult(Boolean.FALSE);
    //     break;
    // default:
    //     Rlog.e(LOGTAG, "handleTimerInEmergencyCallbackMode, unsupported action " + action);
    // }
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::NotifyEcbmTimerReset(
    /* [in] */ Boolean flag)
{
    // ==================before translated======================
    // mEcmTimerResetRegistrants.notifyResult(flag);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::RegisterForEcmTimerReset(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // mEcmTimerResetRegistrants.addUnique(h, what, obj);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::UnregisterForEcmTimerReset(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // mEcmTimerResetRegistrants.remove(h);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // AsyncResult ar;
    // Message     onComplete;
    //
    // if (!mIsTheCurrentActivePhone) {
    //     Rlog.e(LOGTAG, "Received message " + msg +
    //             "[" + msg.what + "] while being destroyed. Ignoring.");
    //     return;
    // }
    // switch(msg.what) {
    //     case EVENT_RADIO_AVAILABLE: {
    //         mCi.getBasebandVersion(obtainMessage(EVENT_GET_BASEBAND_VERSION_DONE));
    //
    //         mCi.getDeviceIdentity(obtainMessage(EVENT_GET_DEVICE_IDENTITY_DONE));
    //     }
    //     break;
    //
    //     case EVENT_GET_BASEBAND_VERSION_DONE:{
    //         ar = (AsyncResult)msg.obj;
    //
    //         if (ar.exception != null) {
    //             break;
    //         }
    //
    //         if (DBG) Rlog.d(LOGTAG, "Baseband version: " + ar.result);
    //         if (!"".equals((String)ar.result)) {
    //             setSystemProperty(TelephonyProperties.PROPERTY_BASEBAND_VERSION,
    //                               (String)ar.result);
    //         }
    //     }
    //     break;
    //
    //     case EVENT_GET_DEVICE_IDENTITY_DONE:{
    //         ar = (AsyncResult)msg.obj;
    //
    //         if (ar.exception != null) {
    //             break;
    //         }
    //         String[] respId = (String[])ar.result;
    //         mImei = respId[0];
    //         mImeiSv = respId[1];
    //         mEsn  =  respId[2];
    //         mMeid =  respId[3];
    //     }
    //     break;
    //
    //     case EVENT_EMERGENCY_CALLBACK_MODE_ENTER:{
    //         handleEnterEmergencyCallbackMode(msg);
    //     }
    //     break;
    //
    //     case  EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE:{
    //         handleExitEmergencyCallbackMode(msg);
    //     }
    //     break;
    //
    //     case EVENT_RUIM_RECORDS_LOADED:{
    //         Rlog.d(LOGTAG, "Event EVENT_RUIM_RECORDS_LOADED Received");
    //         updateCurrentCarrierInProvider();
    //         // Notify voicemails.
    //         log("notifyMessageWaitingChanged");
    //         mNotifier.notifyMessageWaitingChanged(this);
    //         updateVoiceMail();
    //     }
    //     break;
    //
    //     case EVENT_RADIO_OFF_OR_NOT_AVAILABLE:{
    //         Rlog.d(LOGTAG, "Event EVENT_RADIO_OFF_OR_NOT_AVAILABLE Received");
    //     }
    //     break;
    //
    //     case EVENT_RADIO_ON:{
    //         Rlog.d(LOGTAG, "Event EVENT_RADIO_ON Received");
    //         handleCdmaSubscriptionSource(mCdmaSSM.getCdmaSubscriptionSource());
    //     }
    //     break;
    //
    //     case EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED:{
    //         Rlog.d(LOGTAG, "EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED");
    //         handleCdmaSubscriptionSource(mCdmaSSM.getCdmaSubscriptionSource());
    //     }
    //     break;
    //
    //     case EVENT_SSN:{
    //         Rlog.d(LOGTAG, "Event EVENT_SSN Received");
    //     }
    //     break;
    //
    //     case EVENT_REGISTERED_TO_NETWORK:{
    //         Rlog.d(LOGTAG, "Event EVENT_REGISTERED_TO_NETWORK Received");
    //     }
    //     break;
    //
    //     case EVENT_NV_READY:{
    //         Rlog.d(LOGTAG, "Event EVENT_NV_READY Received");
    //         prepareEri();
    //         // Notify voicemails.
    //         log("notifyMessageWaitingChanged");
    //         mNotifier.notifyMessageWaitingChanged(this);
    //         updateVoiceMail();
    //         SubInfoRecordUpdater subInfoRecordUpdater = PhoneFactory.getSubInfoRecordUpdater();
    //         if (subInfoRecordUpdater != null) {
    //             subInfoRecordUpdater.updateSubIdForNV (mPhoneId);
    //         }
    //     }
    //     break;
    //
    //     case EVENT_SET_VM_NUMBER_DONE:{
    //         ar = (AsyncResult)msg.obj;
    //         if (IccException.class.isInstance(ar.exception)) {
    //             storeVoiceMailNumber(mVmNumber);
    //             ar.exception = null;
    //         }
    //         onComplete = (Message) ar.userObj;
    //         if (onComplete != null) {
    //             AsyncResult.forMessage(onComplete, ar.result, ar.exception);
    //             onComplete.sendToTarget();
    //         }
    //     }
    //     break;
    //
    //     default:{
    //         super.handleMessage(msg);
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetPhoneSubInfo(
    /* [out] */ IPhoneSubInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mSubInfo;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetIccPhoneBookInterfaceManager(
    /* [out] */ IIccPhoneBookInterfaceManager** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mRuimPhoneBookInterfaceManager;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::RegisterForEriFileLoaded(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // Registrant r = new Registrant (h, what, obj);
    // mEriFileLoadedRegistrants.add(r);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::UnregisterForEriFileLoaded(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // mEriFileLoadedRegistrants.remove(h);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SetSystemProperty(
    /* [in] */ const String& property,
    /* [in] */ const String& value)
{
    // ==================before translated======================
    // super.setSystemProperty(property, value);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetSystemProperty(
    /* [in] */ const String& property,
    /* [in] */ const String& defValue,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return super.getSystemProperty(property, defValue);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::ActivateCellBroadcastSms(
    /* [in] */ Int32 activate,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "[CDMAPhone] activateCellBroadcastSms() is obsolete; use SmsManager");
    // response.sendToTarget();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetCellBroadcastSmsConfig(
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "[CDMAPhone] getCellBroadcastSmsConfig() is obsolete; use SmsManager");
    // response.sendToTarget();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SetCellBroadcastSmsConfig(
    /* [in] */ ArrayOf<Int32>* configValuesArray,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "[CDMAPhone] setCellBroadcastSmsConfig() is obsolete; use SmsManager");
    // response.sendToTarget();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::NeedsOtaServiceProvisioning(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mSST.getOtasp() != ServiceStateTracker.OTASP_NOT_NEEDED;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::IsOtaSpNumber(
    /* [in] */ const String& dialStr,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // boolean isOtaSpNum = false;
    // String dialableStr = PhoneNumberUtils.extractNetworkPortionAlt(dialStr);
    // if (dialableStr != null) {
    //     isOtaSpNum = isIs683OtaSpDialStr(dialableStr);
    //     if (isOtaSpNum == false) {
    //         isOtaSpNum = isCarrierOtaSpNum(dialableStr);
    //     }
    // }
    // if (DBG) Rlog.d(LOGTAG, "isOtaSpNumber " + isOtaSpNum);
    // return isOtaSpNum;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetCdmaEriIconIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getServiceState().getCdmaEriIconIndex();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetCdmaEriIconMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getServiceState().getCdmaEriIconMode();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::GetCdmaEriText(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // int roamInd = getServiceState().getCdmaRoamingIndicator();
    // int defRoamInd = getServiceState().getCdmaDefaultRoamingIndicator();
    // return mEriManager.getCdmaEriText(roamInd, defRoamInd);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::UpdateCurrentCarrierInProvider(
    /* [in] */ const String& operatorNumeric,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // log("CDMAPhone: updateCurrentCarrierInProvider called");
    // if (!TextUtils.isEmpty(operatorNumeric)) {
    //     try {
    //         Uri uri = Uri.withAppendedPath(Telephony.Carriers.CONTENT_URI, "current");
    //         ContentValues map = new ContentValues();
    //         map.put(Telephony.Carriers.NUMERIC, operatorNumeric);
    //         log("updateCurrentCarrierInProvider from system: numeric=" + operatorNumeric);
    //         getContext().getContentResolver().insert(uri, map);
    //
    //         // Updates MCC MNC device configuration information
    //         log("update mccmnc=" + operatorNumeric);
    //         MccTable.updateMccMncConfiguration(mContext, operatorNumeric, false);
    //
    //         return true;
    //     } catch (SQLException e) {
    //         Rlog.e(LOGTAG, "Can't store current operator", e);
    //     }
    // }
    // return false;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::UpdateCurrentCarrierInProvider(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return true;
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::PrepareEri()
{
    // ==================before translated======================
    // if (mEriManager == null) {
    //     Rlog.e(LOGTAG, "PrepareEri: Trying to access stale objects");
    //     return;
    // }
    // mEriManager.loadEriFile();
    // if(mEriManager.isEriFileLoaded()) {
    //     // when the ERI file is loaded
    //     log("ERI read, notify registrants");
    //     mEriFileLoadedRegistrants.notifyRegistrants();
    // }
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::IsEriFileLoaded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mEriManager.isEriFileLoaded();
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SetVoiceMessageWaiting(
    /* [in] */ Int32 line,
    /* [in] */ Int32 countWaiting)
{
    // ==================before translated======================
    // setVoiceMessageCount(countWaiting);
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    // ==================before translated======================
    // pw.println("CDMAPhone extends:");
    // super.dump(fd, pw, args);
    // pw.println(" mVmNumber=" + mVmNumber);
    // pw.println(" mCT=" + mCT);
    // pw.println(" mSST=" + mSST);
    // pw.println(" mCdmaSSM=" + mCdmaSSM);
    // pw.println(" mPendingMmis=" + mPendingMmis);
    // pw.println(" mRuimPhoneBookInterfaceManager=" + mRuimPhoneBookInterfaceManager);
    // pw.println(" mCdmaSubscriptionSource=" + mCdmaSubscriptionSource);
    // pw.println(" mSubInfo=" + mSubInfo);
    // pw.println(" mEriManager=" + mEriManager);
    // pw.println(" mWakeLock=" + mWakeLock);
    // pw.println(" mIsPhoneInEcmState=" + mIsPhoneInEcmState);
    // if (VDBG) pw.println(" mImei=" + mImei);
    // if (VDBG) pw.println(" mImeiSv=" + mImeiSv);
    // if (VDBG) pw.println(" mEsn=" + mEsn);
    // if (VDBG) pw.println(" mMeid=" + mMeid);
    // pw.println(" mCarrierOtaSpNumSchema=" + mCarrierOtaSpNumSchema);
    // pw.println(" getCdmaEriIconIndex()=" + getCdmaEriIconIndex());
    // pw.println(" getCdmaEriIconMode()=" + getCdmaEriIconMode());
    // pw.println(" getCdmaEriText()=" + getCdmaEriText());
    // pw.println(" isMinInfoReady()=" + isMinInfoReady());
    // pw.println(" isCspPlmnEnabled()=" + isCspPlmnEnabled());
    assert(0);
    return NOERROR;
}

ECode CDMAPhone::SetOperatorBrandOverride(
    /* [in] */ const String& brand,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // if (mUiccController == null) {
    //     return false;
    // }
    //
    // UiccCard card = mUiccController.getUiccCard();
    // if (card == null) {
    //     return false;
    // }
    //
    // boolean status = card.setOperatorBrandOverride(brand);
    //
    // // Refresh.
    // if (status) {
    //     IccRecords iccRecords = mIccRecords.get();
    //     if (iccRecords != null) {
    //         SystemProperties.set(TelephonyProperties.PROPERTY_ICC_OPERATOR_ALPHA,
    //                 iccRecords.getServiceProviderName());
    //     }
    //     if (mSST != null) {
    //         mSST.pollState();
    //     }
    // }
    // return status;
    assert(0);
    return NOERROR;
}

void CDMAPhone::InitSstIcc()
{
    // ==================before translated======================
    // mSST = new CdmaServiceStateTracker(this);
    assert(0);
}

void CDMAPhone::Init(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneNotifier* notifier)
{
    // ==================before translated======================
    // mCi.setPhoneType(PhoneConstants.PHONE_TYPE_CDMA);
    // mCT = new CdmaCallTracker(this);
    // mCdmaSSM = CdmaSubscriptionSourceManager.getInstance(context, mCi, this,
    //         EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED, null);
    // mDcTracker = new DcTracker(this);
    // mRuimPhoneBookInterfaceManager = new RuimPhoneBookInterfaceManager(this);
    // mSubInfo = new PhoneSubInfo(this);
    // mEriManager = new EriManager(this, context, EriManager.ERI_FROM_XML);
    //
    // mCi.registerForAvailable(this, EVENT_RADIO_AVAILABLE, null);
    // mCi.registerForOffOrNotAvailable(this, EVENT_RADIO_OFF_OR_NOT_AVAILABLE, null);
    // mCi.registerForOn(this, EVENT_RADIO_ON, null);
    // mCi.setOnSuppServiceNotification(this, EVENT_SSN, null);
    // mSST.registerForNetworkAttached(this, EVENT_REGISTERED_TO_NETWORK, null);
    // mCi.setEmergencyCallbackMode(this, EVENT_EMERGENCY_CALLBACK_MODE_ENTER, null);
    // mCi.registerForExitEmergencyCallbackMode(this, EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE,
    //         null);
    //
    // PowerManager pm
    //     = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
    // mWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK,LOGTAG);
    //
    // //Change the system setting
    // SystemProperties.set(TelephonyProperties.CURRENT_ACTIVE_PHONE,
    //         Integer.toString(PhoneConstants.PHONE_TYPE_CDMA));
    //
    // // This is needed to handle phone process crashes
    // String inEcm=SystemProperties.get(TelephonyProperties.PROPERTY_INECM_MODE, "false");
    // mIsPhoneInEcmState = inEcm.equals("true");
    // if (mIsPhoneInEcmState) {
    //     // Send a message which will invoke handleExitEmergencyCallbackMode
    //     mCi.exitEmergencyCallbackMode(obtainMessage(EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE));
    // }
    //
    // // get the string that specifies the carrier OTA Sp number
    // mCarrierOtaSpNumSchema = SystemProperties.get(
    //         TelephonyProperties.PROPERTY_OTASP_NUM_SCHEMA,"");
    //
    // // Sets operator properties by retrieving from build-time system property
    // String operatorAlpha = SystemProperties.get("ro.cdma.home.operator.alpha");
    // String operatorNumeric = SystemProperties.get(PROPERTY_CDMA_HOME_OPERATOR_NUMERIC);
    // log("init: operatorAlpha='" + operatorAlpha
    //         + "' operatorNumeric='" + operatorNumeric + "'");
    // if (mUiccController.getUiccCardApplication(mPhoneId, UiccController.APP_FAM_3GPP) == null) {
    //     log("init: APP_FAM_3GPP == NULL");
    //     if (!TextUtils.isEmpty(operatorAlpha)) {
    //         log("init: set 'gsm.sim.operator.alpha' to operator='" + operatorAlpha + "'");
    //         setSystemProperty(PROPERTY_ICC_OPERATOR_ALPHA, operatorAlpha);
    //     }
    //     if (!TextUtils.isEmpty(operatorNumeric)) {
    //         log("init: set 'gsm.sim.operator.numeric' to operator='" + operatorNumeric + "'");
    //         log("update icc_operator_numeric=" + operatorNumeric);
    //         setSystemProperty(PROPERTY_ICC_OPERATOR_NUMERIC, operatorNumeric);
    //
    //         SubscriptionController.getInstance().setMccMnc(operatorNumeric, getSubId());
    //     }
    //     setIsoCountryProperty(operatorNumeric);
    // }
    //
    // // Sets current entry in the telephony carrier table
    // updateCurrentCarrierInProvider(operatorNumeric);
    assert(0);
}

void CDMAPhone::Finalize()
{
    // ==================before translated======================
    // if(DBG) Rlog.d(LOGTAG, "CDMAPhone finalized");
    // if (mWakeLock.isHeld()) {
    //     Rlog.e(LOGTAG, "UNEXPECTED; mWakeLock is held when finalizing.");
    //     mWakeLock.release();
    // }
    assert(0);
}

AutoPtr<IConnection> CDMAPhone::DialInternal(
    /* [in] */ const String& dialString,
    /* [in] */ IUUSInfo* uusInfo,
    /* [in] */ Int32 videoState)
{
    // ==================before translated======================
    // // Need to make sure dialString gets parsed properly
    // String newDialString = PhoneNumberUtils.stripSeparators(dialString);
    // return mCT.dial(newDialString);
    assert(0);
    AutoPtr<IConnection> empty;
    return empty;
}

AutoPtr<IUiccCardApplication> CDMAPhone::GetUiccCardApplication()
{
    // ==================before translated======================
    // return  mUiccController.getUiccCardApplication(mPhoneId, UiccController.APP_FAM_3GPP2);
    assert(0);
    AutoPtr<IUiccCardApplication> empty;
    return empty;
}

void CDMAPhone::SetCardInPhoneBook()
{
    // ==================before translated======================
    // if (mUiccController == null ) {
    //     return;
    // }
    //
    // mRuimPhoneBookInterfaceManager.setIccCard(mUiccController.getUiccCard(mPhoneId));
    assert(0);
}

ECode CDMAPhone::OnUpdateIccAvailability()
{
    // ==================before translated======================
    // if (mUiccController == null ) {
    //     return;
    // }
    //
    // // Get the latest info on the card and
    // // send this to Phone Book
    // setCardInPhoneBook();
    //
    // UiccCardApplication newUiccApplication = getUiccCardApplication();
    //
    // if (newUiccApplication == null) {
    //     log("can't find 3GPP2 application; trying APP_FAM_3GPP");
    //     newUiccApplication =
    //             mUiccController.getUiccCardApplication(mPhoneId, UiccController.APP_FAM_3GPP);
    // }
    //
    // UiccCardApplication app = mUiccApplication.get();
    // if (app != newUiccApplication) {
    //     if (app != null) {
    //         log("Removing stale icc objects.");
    //         if (mIccRecords.get() != null) {
    //             unregisterForRuimRecordEvents();
    //         }
    //         mIccRecords.set(null);
    //         mUiccApplication.set(null);
    //     }
    //     if (newUiccApplication != null) {
    //         log("New Uicc application found");
    //         mUiccApplication.set(newUiccApplication);
    //         mIccRecords.set(newUiccApplication.getIccRecords());
    //         registerForRuimRecordEvents();
    //     }
    // }
    assert(0);
    return NOERROR;
}

void CDMAPhone::SetIsoCountryProperty(
    /* [in] */ const String& operatorNumeric)
{
    // ==================before translated======================
    // if (TextUtils.isEmpty(operatorNumeric)) {
    //     log("setIsoCountryProperty: clear 'gsm.sim.operator.iso-country'");
    //     setSystemProperty(PROPERTY_ICC_OPERATOR_ISO_COUNTRY, "");
    // } else {
    //     String iso = "";
    //     try {
    //         iso = MccTable.countryCodeForMcc(Integer.parseInt(
    //                 operatorNumeric.substring(0,3)));
    //     } catch (NumberFormatException ex) {
    //         loge("setIsoCountryProperty: countryCodeForMcc error", ex);
    //     } catch (StringIndexOutOfBoundsException ex) {
    //         loge("setIsoCountryProperty: countryCodeForMcc error", ex);
    //     }
    //
    //     log("setIsoCountryProperty: set 'gsm.sim.operator.iso-country' to iso=" + iso);
    //     setSystemProperty(PROPERTY_ICC_OPERATOR_ISO_COUNTRY, iso);
    // }
    assert(0);
}

void CDMAPhone::RegisterForRuimRecordEvents()
{
    // ==================before translated======================
    // IccRecords r = mIccRecords.get();
    // if (r == null) {
    //     return;
    // }
    // r.registerForRecordsLoaded(this, EVENT_RUIM_RECORDS_LOADED, null);
    assert(0);
}

void CDMAPhone::UnregisterForRuimRecordEvents()
{
    // ==================before translated======================
    // IccRecords r = mIccRecords.get();
    // if (r == null) {
    //     return;
    // }
    // r.unregisterForRecordsLoaded(this);
    assert(0);
}

void CDMAPhone::Log(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // if (DBG)
    //     Rlog.d(LOGTAG, s);
    assert(0);
}

//void CDMAPhone::Loge(
//    /* [in] */ const String& s,
//    /* [in] */ Exception* e)
//{
//    // ==================before translated======================
//    // if (DBG)
//    //     Rlog.e(LOGTAG, s, e);
//    assert(0);
//}

void CDMAPhone::UpdateVoiceMail()
{
    // ==================before translated======================
    // setVoiceMessageCount(getStoredVoiceMessageCount());
    assert(0);
}

Int32 CDMAPhone::GetStoredVoiceMessageCount()
{
    // ==================before translated======================
    // SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(mContext);
    // return (sp.getInt(VM_COUNT + getSubId(), 0));
    assert(0);
    return 0;
}

void CDMAPhone::HandleEnterEmergencyCallbackMode(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // if (DBG) {
    //     Rlog.d(LOGTAG, "handleEnterEmergencyCallbackMode,mIsPhoneInEcmState= "
    //             + mIsPhoneInEcmState);
    // }
    // // if phone is not in Ecm mode, and it's changed to Ecm mode
    // if (mIsPhoneInEcmState == false) {
    //     mIsPhoneInEcmState = true;
    //     // notify change
    //     sendEmergencyCallbackModeChange();
    //     super.setSystemProperty(TelephonyProperties.PROPERTY_INECM_MODE, "true");
    //
    //     // Post this runnable so we will automatically exit
    //     // if no one invokes exitEmergencyCallbackMode() directly.
    //     long delayInMillis = SystemProperties.getLong(
    //             TelephonyProperties.PROPERTY_ECM_EXIT_TIMER, DEFAULT_ECM_EXIT_TIMER_VALUE);
    //     postDelayed(mExitEcmRunnable, delayInMillis);
    //     // We don't want to go to sleep while in Ecm
    //     mWakeLock.acquire();
    // }
    assert(0);
}

void CDMAPhone::HandleExitEmergencyCallbackMode(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // AsyncResult ar = (AsyncResult)msg.obj;
    // if (DBG) {
    //     Rlog.d(LOGTAG, "handleExitEmergencyCallbackMode,ar.exception , mIsPhoneInEcmState "
    //             + ar.exception + mIsPhoneInEcmState);
    // }
    // // Remove pending exit Ecm runnable, if any
    // removeCallbacks(mExitEcmRunnable);
    //
    // if (mEcmExitRespRegistrant != null) {
    //     mEcmExitRespRegistrant.notifyRegistrant(ar);
    // }
    // // if exiting ecm success
    // if (ar.exception == null) {
    //     if (mIsPhoneInEcmState) {
    //         mIsPhoneInEcmState = false;
    //         super.setSystemProperty(TelephonyProperties.PROPERTY_INECM_MODE, "false");
    //     }
    //     // send an Intent
    //     sendEmergencyCallbackModeChange();
    //     // Re-initiate data connection
    //     mDcTracker.setInternalDataEnabled(true);
    // }
    assert(0);
}

void CDMAPhone::HandleCdmaSubscriptionSource(
    /* [in] */ Int32 newSubscriptionSource)
{
    // ==================before translated======================
    // if (newSubscriptionSource != mCdmaSubscriptionSource) {
    //      mCdmaSubscriptionSource = newSubscriptionSource;
    //      if (newSubscriptionSource == CDMA_SUBSCRIPTION_NV) {
    //          // NV is ready when subscription source is NV
    //          sendMessage(obtainMessage(EVENT_NV_READY));
    //      }
    // }
    assert(0);
}

Boolean CDMAPhone::IsIs683OtaSpDialStr(
    /* [in] */ const String& dialStr)
{
    // ==================before translated======================
    // int sysSelCodeInt;
    // boolean isOtaspDialString = false;
    // int dialStrLen = dialStr.length();
    //
    // if (dialStrLen == IS683A_FEATURE_CODE_NUM_DIGITS) {
    //     if (dialStr.equals(IS683A_FEATURE_CODE)) {
    //         isOtaspDialString = true;
    //     }
    // } else {
    //     sysSelCodeInt = extractSelCodeFromOtaSpNum(dialStr);
    //     switch (sysSelCodeInt) {
    //         case IS683_CONST_800MHZ_A_BAND:
    //         case IS683_CONST_800MHZ_B_BAND:
    //         case IS683_CONST_1900MHZ_A_BLOCK:
    //         case IS683_CONST_1900MHZ_B_BLOCK:
    //         case IS683_CONST_1900MHZ_C_BLOCK:
    //         case IS683_CONST_1900MHZ_D_BLOCK:
    //         case IS683_CONST_1900MHZ_E_BLOCK:
    //         case IS683_CONST_1900MHZ_F_BLOCK:
    //             isOtaspDialString = true;
    //             break;
    //         default:
    //             break;
    //     }
    // }
    // return isOtaspDialString;
    assert(0);
    return FALSE;
}

Int32 CDMAPhone::ExtractSelCodeFromOtaSpNum(
    /* [in] */ const String& dialStr)
{
    // ==================before translated======================
    // int dialStrLen = dialStr.length();
    // int sysSelCodeInt = INVALID_SYSTEM_SELECTION_CODE;
    //
    // if ((dialStr.regionMatches(0, IS683A_FEATURE_CODE,
    //                            0, IS683A_FEATURE_CODE_NUM_DIGITS)) &&
    //     (dialStrLen >= (IS683A_FEATURE_CODE_NUM_DIGITS +
    //                     IS683A_SYS_SEL_CODE_NUM_DIGITS))) {
    //         // Since we checked the condition above, the system selection code
    //         // extracted from dialStr will not cause any exception
    //         sysSelCodeInt = Integer.parseInt (
    //                         dialStr.substring (IS683A_FEATURE_CODE_NUM_DIGITS,
    //                         IS683A_FEATURE_CODE_NUM_DIGITS + IS683A_SYS_SEL_CODE_NUM_DIGITS));
    // }
    // if (DBG) Rlog.d(LOGTAG, "extractSelCodeFromOtaSpNum " + sysSelCodeInt);
    // return sysSelCodeInt;
    assert(0);
    return 0;
}

Boolean CDMAPhone::CheckOtaSpNumBasedOnSysSelCode(
    /* [in] */ Int32 sysSelCodeInt,
    /* [in] */ ArrayOf<String>* sch)
{
    // ==================before translated======================
    // boolean isOtaSpNum = false;
    // try {
    //     // Get how many number of system selection code ranges
    //     int selRc = Integer.parseInt(sch[1]);
    //     for (int i = 0; i < selRc; i++) {
    //         if (!TextUtils.isEmpty(sch[i+2]) && !TextUtils.isEmpty(sch[i+3])) {
    //             int selMin = Integer.parseInt(sch[i+2]);
    //             int selMax = Integer.parseInt(sch[i+3]);
    //             // Check if the selection code extracted from the dial string falls
    //             // within any of the range pairs specified in the schema.
    //             if ((sysSelCodeInt >= selMin) && (sysSelCodeInt <= selMax)) {
    //                 isOtaSpNum = true;
    //                 break;
    //             }
    //         }
    //     }
    // } catch (NumberFormatException ex) {
    //     // If the carrier ota sp number schema is not correct, we still allow dial
    //     // and only log the error:
    //     Rlog.e(LOGTAG, "checkOtaSpNumBasedOnSysSelCode, error", ex);
    // }
    // return isOtaSpNum;
    assert(0);
    return FALSE;
}

Boolean CDMAPhone::IsCarrierOtaSpNum(
    /* [in] */ const String& dialStr)
{
    // ==================before translated======================
    // boolean isOtaSpNum = false;
    // int sysSelCodeInt = extractSelCodeFromOtaSpNum(dialStr);
    // if (sysSelCodeInt == INVALID_SYSTEM_SELECTION_CODE) {
    //     return isOtaSpNum;
    // }
    // // mCarrierOtaSpNumSchema is retrieved from PROPERTY_OTASP_NUM_SCHEMA:
    // if (!TextUtils.isEmpty(mCarrierOtaSpNumSchema)) {
    //     Matcher m = pOtaSpNumSchema.matcher(mCarrierOtaSpNumSchema);
    //     if (DBG) {
    //         Rlog.d(LOGTAG, "isCarrierOtaSpNum,schema" + mCarrierOtaSpNumSchema);
    //     }
    //
    //     if (m.find()) {
    //         String sch[] = pOtaSpNumSchema.split(mCarrierOtaSpNumSchema);
    //         // If carrier uses system selection code mechanism
    //         if (!TextUtils.isEmpty(sch[0]) && sch[0].equals("SELC")) {
    //             if (sysSelCodeInt!=INVALID_SYSTEM_SELECTION_CODE) {
    //                 isOtaSpNum=checkOtaSpNumBasedOnSysSelCode(sysSelCodeInt,sch);
    //             } else {
    //                 if (DBG) {
    //                     Rlog.d(LOGTAG, "isCarrierOtaSpNum,sysSelCodeInt is invalid");
    //                 }
    //             }
    //         } else if (!TextUtils.isEmpty(sch[0]) && sch[0].equals("FC")) {
    //             int fcLen =  Integer.parseInt(sch[1]);
    //             String fc = sch[2];
    //             if (dialStr.regionMatches(0,fc,0,fcLen)) {
    //                 isOtaSpNum = true;
    //             } else {
    //                 if (DBG) Rlog.d(LOGTAG, "isCarrierOtaSpNum,not otasp number");
    //             }
    //         } else {
    //             if (DBG) {
    //                 Rlog.d(LOGTAG, "isCarrierOtaSpNum,ota schema not supported" + sch[0]);
    //             }
    //         }
    //     } else {
    //         if (DBG) {
    //             Rlog.d(LOGTAG, "isCarrierOtaSpNum,ota schema pattern not right" +
    //                   mCarrierOtaSpNumSchema);
    //         }
    //     }
    // } else {
    //     if (DBG) Rlog.d(LOGTAG, "isCarrierOtaSpNum,ota schema pattern empty");
    // }
    // return isOtaSpNum;
    assert(0);
    return FALSE;
}

void CDMAPhone::StoreVoiceMailNumber(
    /* [in] */ const String& number)
{
    // ==================before translated======================
    // // Update the preference value of voicemail number
    // SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(getContext());
    // SharedPreferences.Editor editor = sp.edit();
    // editor.putString(VM_NUMBER_CDMA + getSubId(), number);
    // editor.apply();
    assert(0);
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
