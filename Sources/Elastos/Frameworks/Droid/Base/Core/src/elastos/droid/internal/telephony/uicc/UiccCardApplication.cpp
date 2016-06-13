
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/UiccCardApplication.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                  UiccCardApplication::InnerHandler
//=====================================================================
UiccCardApplication::InnerHandler::InnerHandler(
    /* [in] */ UiccCardApplication* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode UiccCardApplication::InnerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // AsyncResult ar;
    //
    // if (mDestroyed) {
    //     loge("Received message " + msg + "[" + msg.what
    //             + "] while being destroyed. Ignoring.");
    //     return;
    // }
    //
    // switch (msg.what) {
    //     case EVENT_PIN1_PUK1_DONE:
    //     case EVENT_PIN2_PUK2_DONE:
    //     case EVENT_CHANGE_PIN1_DONE:
    //     case EVENT_CHANGE_PIN2_DONE:
    //         // a PIN/PUK/PIN2/PUK2 complete
    //         // request has completed. ar.userObj is the response Message
    //         int attemptsRemaining = -1;
    //         ar = (AsyncResult)msg.obj;
    //         if (ar.result != null) {
    //             attemptsRemaining = parsePinPukErrorResult(ar);
    //         }
    //         Message response = (Message)ar.userObj;
    //         AsyncResult.forMessage(response).exception = ar.exception;
    //         response.arg1 = attemptsRemaining;
    //         response.sendToTarget();
    //         break;
    //     case EVENT_QUERY_FACILITY_FDN_DONE:
    //         ar = (AsyncResult)msg.obj;
    //         onQueryFdnEnabled(ar);
    //         break;
    //     case EVENT_CHANGE_FACILITY_FDN_DONE:
    //         ar = (AsyncResult)msg.obj;
    //         onChangeFdnDone(ar);
    //         break;
    //     case EVENT_QUERY_FACILITY_LOCK_DONE:
    //         ar = (AsyncResult)msg.obj;
    //         onQueryFacilityLock(ar);
    //         break;
    //     case EVENT_CHANGE_FACILITY_LOCK_DONE:
    //         ar = (AsyncResult)msg.obj;
    //         onChangeFacilityLock(ar);
    //         break;
    //     default:
    //         loge("Unknown Event " + msg.what);
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                         UiccCardApplication
//=====================================================================
CAR_INTERFACE_IMPL(UiccCardApplication, Object, IUiccCardApplication);

const String UiccCardApplication::LOGTAG("UiccCardApplication");
const Boolean UiccCardApplication::DBG = TRUE;
const Int32 UiccCardApplication::EVENT_PIN1_PUK1_DONE;
const Int32 UiccCardApplication::EVENT_CHANGE_PIN1_DONE;
const Int32 UiccCardApplication::EVENT_CHANGE_PIN2_DONE;
const Int32 UiccCardApplication::EVENT_QUERY_FACILITY_FDN_DONE;
const Int32 UiccCardApplication::EVENT_CHANGE_FACILITY_FDN_DONE;
const Int32 UiccCardApplication::EVENT_QUERY_FACILITY_LOCK_DONE;
const Int32 UiccCardApplication::EVENT_CHANGE_FACILITY_LOCK_DONE;
const Int32 UiccCardApplication::EVENT_PIN2_PUK2_DONE;

UiccCardApplication::UiccCardApplication()
{
}

ECode UiccCardApplication::constructor(
    /* [in] */ IUiccCard* uiccCard,
    /* [in] */ IIccCardApplicationStatus* as,
    /* [in] */ IContext* c,
    /* [in] */ ICommandsInterface* ci)
{
    // ==================before translated======================
    // if (DBG) log("Creating UiccApp: " + as);
    // mUiccCard = uiccCard;
    // mAppState = as.app_state;
    // mAppType = as.app_type;
    // mAuthContext = getAuthContext(mAppType);
    // mPersoSubState = as.perso_substate;
    // mAid = as.aid;
    // mAppLabel = as.app_label;
    // mPin1Replaced = (as.pin1_replaced != 0);
    // mPin1State = as.pin1;
    // mPin2State = as.pin2;
    //
    // mContext = c;
    // mCi = ci;
    //
    // mIccFh = createIccFileHandler(as.app_type);
    // mIccRecords = createIccRecords(as.app_type, mContext, mCi);
    // if (mAppState == AppState.APPSTATE_READY) {
    //     queryFdn();
    //     queryPin1State();
    // }
    return NOERROR;
}

ECode UiccCardApplication::Update(
    /* [in] */ IIccCardApplicationStatus* as,
    /* [in] */ IContext* c,
    /* [in] */ ICommandsInterface* ci)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (mDestroyed) {
    //         loge("Application updated after destroyed! Fix me!");
    //         return;
    //     }
    //
    //     if (DBG) log(mAppType + " update. New " + as);
    //     mContext = c;
    //     mCi = ci;
    //     AppType oldAppType = mAppType;
    //     AppState oldAppState = mAppState;
    //     PersoSubState oldPersoSubState = mPersoSubState;
    //     mAppType = as.app_type;
    //     mAuthContext = getAuthContext(mAppType);
    //     mAppState = as.app_state;
    //     mPersoSubState = as.perso_substate;
    //     mAid = as.aid;
    //     mAppLabel = as.app_label;
    //     mPin1Replaced = (as.pin1_replaced != 0);
    //     mPin1State = as.pin1;
    //     mPin2State = as.pin2;
    //
    //     if (mAppType != oldAppType) {
    //         if (mIccFh != null) { mIccFh.dispose();}
    //         if (mIccRecords != null) { mIccRecords.dispose();}
    //         mIccFh = createIccFileHandler(as.app_type);
    //         mIccRecords = createIccRecords(as.app_type, c, ci);
    //     }
    //
    //     if (mPersoSubState != oldPersoSubState &&
    //             isPersoLocked()) {
    //         notifyPersoLockedRegistrantsIfNeeded(null);
    //     }
    //
    //     if (mAppState != oldAppState) {
    //         if (DBG) log(oldAppType + " changed state: " + oldAppState + " -> " + mAppState);
    //         // If the app state turns to APPSTATE_READY, then query FDN status,
    //         //as it might have failed in earlier attempt.
    //         if (mAppState == AppState.APPSTATE_READY) {
    //             queryFdn();
    //             queryPin1State();
    //         }
    //         notifyPinLockedRegistrantsIfNeeded(null);
    //         notifyReadyRegistrantsIfNeeded(null);
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::Dispose()
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (DBG) log(mAppType + " being Disposed");
    //     mDestroyed = true;
    //     if (mIccRecords != null) { mIccRecords.dispose();}
    //     if (mIccFh != null) { mIccFh.dispose();}
    //     mIccRecords = null;
    //     mIccFh = null;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::QueryFdn()
{
    // ==================before translated======================
    // //This shouldn't change run-time. So needs to be called only once.
    // int serviceClassX;
    //
    // serviceClassX = CommandsInterface.SERVICE_CLASS_VOICE +
    //                 CommandsInterface.SERVICE_CLASS_DATA +
    //                 CommandsInterface.SERVICE_CLASS_FAX;
    // mCi.queryFacilityLockForApp (
    //         CommandsInterface.CB_FACILITY_BA_FD, "", serviceClassX,
    //         mAid, mHandler.obtainMessage(EVENT_QUERY_FACILITY_FDN_DONE));
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::OnRefresh(
    /* [in] */ IIccRefreshResponse* refreshResponse)
{
    // ==================before translated======================
    // if (refreshResponse == null) {
    //     loge("onRefresh received without input");
    //     return;
    // }
    //
    // if (refreshResponse.aid == null ||
    //         refreshResponse.aid.equals(mAid)) {
    //     log("refresh for app " + refreshResponse.aid);
    // } else {
    //  // This is for a different app. Ignore.
    //     return;
    // }
    //
    // switch (refreshResponse.refreshResult) {
    //     case IccRefreshResponse.REFRESH_RESULT_INIT:
    //     case IccRefreshResponse.REFRESH_RESULT_RESET:
    //         log("onRefresh: Setting app state to unknown");
    //         // Move our state to Unknown as soon as we know about a refresh
    //         // so that anyone interested does not get a stale state.
    //         mAppState = AppState.APPSTATE_UNKNOWN;
    //         break;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::RegisterForReady(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     for (int i = mReadyRegistrants.size() - 1; i >= 0 ; i--) {
    //         Registrant  r = (Registrant) mReadyRegistrants.get(i);
    //         Handler rH = r.getHandler();
    //
    //         if (rH != null && rH == h) {
    //             return;
    //         }
    //     }
    //     Registrant r = new Registrant (h, what, obj);
    //     mReadyRegistrants.add(r);
    //     notifyReadyRegistrantsIfNeeded(r);
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::UnregisterForReady(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     mReadyRegistrants.remove(h);
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::RegisterForLocked(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     Registrant r = new Registrant (h, what, obj);
    //     mPinLockedRegistrants.add(r);
    //     notifyPinLockedRegistrantsIfNeeded(r);
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::UnregisterForLocked(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     mPinLockedRegistrants.remove(h);
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::RegisterForPersoLocked(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     Registrant r = new Registrant (h, what, obj);
    //     mPersoLockedRegistrants.add(r);
    //     notifyPersoLockedRegistrantsIfNeeded(r);
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::UnregisterForPersoLocked(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     mPersoLockedRegistrants.remove(h);
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::GetState(
    /* [out] */ AppState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     return mAppState;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::GetType(
    /* [out] */ AppType* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     return mAppType;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::GetAuthContext(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     return mAuthContext;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::GetPersoSubState(
    /* [out] */ PersoSubState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     return mPersoSubState;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::GetAid(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     return mAid;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::GetAppLabel(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAppLabel;
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::GetPin1State(
    /* [out] */ PinState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (mPin1Replaced) {
    //         return mUiccCard.getUniversalPinState();
    //     }
    //     return mPin1State;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::GetIccFileHandler(
    /* [out] */ IIccFileHandler** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     return mIccFh;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::GetIccRecords(
    /* [out] */ IIccRecords** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     return mIccRecords;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::IsPersoLocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // switch (mPersoSubState) {
    //     case PERSOSUBSTATE_UNKNOWN:
    //     case PERSOSUBSTATE_IN_PROGRESS:
    //     case PERSOSUBSTATE_READY:
    //         return false;
    //     default:
    //         return true;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::SupplyPin(
    /* [in] */ const String& pin,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     mCi.supplyIccPinForApp(pin, mAid, mHandler.obtainMessage(EVENT_PIN1_PUK1_DONE,
    //             onComplete));
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::SupplyPuk(
    /* [in] */ const String& puk,
    /* [in] */ const String& newPin,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // synchronized (mLock) {
    // mCi.supplyIccPukForApp(puk, newPin, mAid,
    //         mHandler.obtainMessage(EVENT_PIN1_PUK1_DONE, onComplete));
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::SupplyPin2(
    /* [in] */ const String& pin2,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     mCi.supplyIccPin2ForApp(pin2, mAid,
    //             mHandler.obtainMessage(EVENT_PIN2_PUK2_DONE, onComplete));
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::SupplyPuk2(
    /* [in] */ const String& puk2,
    /* [in] */ const String& newPin2,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     mCi.supplyIccPuk2ForApp(puk2, newPin2, mAid,
    //             mHandler.obtainMessage(EVENT_PIN2_PUK2_DONE, onComplete));
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::SupplyDepersonalization(
    /* [in] */ const String& pin,
    /* [in] */ const String& type,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (DBG) log("Network Despersonalization: pin = **** , type = " + type);
    //     mCi.supplyDepersonalization(pin, type, onComplete);
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::GetIccLockEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIccLockEnabled;
    // /* STOPSHIP: Remove line above and all code associated with setting
    //    mIccLockEanbled once all RIL correctly sends the pin1 state.
    // // Use getPin1State to take into account pin1Replaced flag
    // PinState pinState = getPin1State();
    // return pinState == PinState.PINSTATE_ENABLED_NOT_VERIFIED ||
    //        pinState == PinState.PINSTATE_ENABLED_VERIFIED ||
    //        pinState == PinState.PINSTATE_ENABLED_BLOCKED ||
    //        pinState == PinState.PINSTATE_ENABLED_PERM_BLOCKED;*/
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::GetIccFdnEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     return mIccFdnEnabled;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::GetIccFdnAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIccFdnAvailable;
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::SetIccLockEnabled(
    /* [in] */ Boolean enabled,
    /* [in] */ const String& password,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     int serviceClassX;
    //     serviceClassX = CommandsInterface.SERVICE_CLASS_VOICE +
    //             CommandsInterface.SERVICE_CLASS_DATA +
    //             CommandsInterface.SERVICE_CLASS_FAX;
    //
    //     mDesiredPinLocked = enabled;
    //
    //     mCi.setFacilityLockForApp(CommandsInterface.CB_FACILITY_BA_SIM,
    //             enabled, password, serviceClassX, mAid,
    //             mHandler.obtainMessage(EVENT_CHANGE_FACILITY_LOCK_DONE, onComplete));
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::SetIccFdnEnabled(
    /* [in] */ Boolean enabled,
    /* [in] */ const String& password,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     int serviceClassX;
    //     serviceClassX = CommandsInterface.SERVICE_CLASS_VOICE +
    //             CommandsInterface.SERVICE_CLASS_DATA +
    //             CommandsInterface.SERVICE_CLASS_FAX +
    //             CommandsInterface.SERVICE_CLASS_SMS;
    //
    //     mDesiredFdnEnabled = enabled;
    //
    //     mCi.setFacilityLockForApp(CommandsInterface.CB_FACILITY_BA_FD,
    //             enabled, password, serviceClassX, mAid,
    //             mHandler.obtainMessage(EVENT_CHANGE_FACILITY_FDN_DONE, onComplete));
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::ChangeIccLockPassword(
    /* [in] */ const String& oldPassword,
    /* [in] */ const String& newPassword,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (DBG) log("changeIccLockPassword");
    //     mCi.changeIccPinForApp(oldPassword, newPassword, mAid,
    //             mHandler.obtainMessage(EVENT_CHANGE_PIN1_DONE, onComplete));
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::ChangeIccFdnPassword(
    /* [in] */ const String& oldPassword,
    /* [in] */ const String& newPassword,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (DBG) log("changeIccFdnPassword");
    //     mCi.changeIccPin2ForApp(oldPassword, newPassword, mAid,
    //             mHandler.obtainMessage(EVENT_CHANGE_PIN2_DONE, onComplete));
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::GetIccPin2Blocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     return mPin2State == PinState.PINSTATE_ENABLED_BLOCKED;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::GetIccPuk2Blocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     return mPin2State == PinState.PINSTATE_ENABLED_PERM_BLOCKED;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::GetUICCConfig(
    /* [out] */ IUICCConfig** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mUiccCard.getUICCConfig();
    assert(0);
    return NOERROR;
}

ECode UiccCardApplication::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    // ==================before translated======================
    // pw.println("UiccCardApplication: " + this);
    // pw.println(" mUiccCard=" + mUiccCard);
    // pw.println(" mAppState=" + mAppState);
    // pw.println(" mAppType=" + mAppType);
    // pw.println(" mPersoSubState=" + mPersoSubState);
    // pw.println(" mAid=" + mAid);
    // pw.println(" mAppLabel=" + mAppLabel);
    // pw.println(" mPin1Replaced=" + mPin1Replaced);
    // pw.println(" mPin1State=" + mPin1State);
    // pw.println(" mPin2State=" + mPin2State);
    // pw.println(" mIccFdnEnabled=" + mIccFdnEnabled);
    // pw.println(" mDesiredFdnEnabled=" + mDesiredFdnEnabled);
    // pw.println(" mIccLockEnabled=" + mIccLockEnabled);
    // pw.println(" mDesiredPinLocked=" + mDesiredPinLocked);
    // pw.println(" mCi=" + mCi);
    // pw.println(" mIccRecords=" + mIccRecords);
    // pw.println(" mIccFh=" + mIccFh);
    // pw.println(" mDestroyed=" + mDestroyed);
    // pw.println(" mReadyRegistrants: size=" + mReadyRegistrants.size());
    // for (int i = 0; i < mReadyRegistrants.size(); i++) {
    //     pw.println("  mReadyRegistrants[" + i + "]="
    //             + ((Registrant)mReadyRegistrants.get(i)).getHandler());
    // }
    // pw.println(" mPinLockedRegistrants: size=" + mPinLockedRegistrants.size());
    // for (int i = 0; i < mPinLockedRegistrants.size(); i++) {
    //     pw.println("  mPinLockedRegistrants[" + i + "]="
    //             + ((Registrant)mPinLockedRegistrants.get(i)).getHandler());
    // }
    // pw.println(" mPersoLockedRegistrants: size=" + mPersoLockedRegistrants.size());
    // for (int i = 0; i < mPersoLockedRegistrants.size(); i++) {
    //     pw.println("  mPersoLockedRegistrants[" + i + "]="
    //             + ((Registrant)mPersoLockedRegistrants.get(i)).getHandler());
    // }
    // pw.flush();
    assert(0);
    return NOERROR;
}

AutoPtr<IUiccCard> UiccCardApplication::GetUiccCard()
{
    // ==================before translated======================
    // return mUiccCard;
    assert(0);
    AutoPtr<IUiccCard> empty;
    return empty;
}

AutoPtr<IIccRecords> UiccCardApplication::CreateIccRecords(
    /* [in] */ AppType type,
    /* [in] */ IContext* c,
    /* [in] */ ICommandsInterface* ci)
{
    // ==================before translated======================
    // if (type == AppType.APPTYPE_USIM || type == AppType.APPTYPE_SIM) {
    //     return new SIMRecords(this, c, ci);
    // } else if (type == AppType.APPTYPE_RUIM || type == AppType.APPTYPE_CSIM){
    //     return new RuimRecords(this, c, ci);
    // } else if (type == AppType.APPTYPE_ISIM) {
    //     return new IsimUiccRecords(this, c, ci);
    // } else {
    //     // Unknown app type (maybe detection is still in progress)
    //     return null;
    // }
    assert(0);
    AutoPtr<IIccRecords> empty;
    return empty;
}

AutoPtr<IIccFileHandler> UiccCardApplication::CreateIccFileHandler(
    /* [in] */ AppType type)
{
    // ==================before translated======================
    // switch (type) {
    //     case APPTYPE_SIM:
    //         return new SIMFileHandler(this, mAid, mCi);
    //     case APPTYPE_RUIM:
    //         return new RuimFileHandler(this, mAid, mCi);
    //     case APPTYPE_USIM:
    //         return new UsimFileHandler(this, mAid, mCi);
    //     case APPTYPE_CSIM:
    //         return new CsimFileHandler(this, mAid, mCi);
    //     case APPTYPE_ISIM:
    //         return new IsimFileHandler(this, mAid, mCi);
    //     default:
    //         return null;
    // }
    assert(0);
    AutoPtr<IIccFileHandler> empty;
    return empty;
}

void UiccCardApplication::OnQueryFdnEnabled(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (ar.exception != null) {
    //         if (DBG) log("Error in querying facility lock:" + ar.exception);
    //         return;
    //     }
    //
    //     int[] result = (int[])ar.result;
    //     if(result.length != 0) {
    //         //0 - Available & Disabled, 1-Available & Enabled, 2-Unavailable.
    //         if (result[0] == 2) {
    //             mIccFdnEnabled = false;
    //             mIccFdnAvailable = false;
    //         } else {
    //             mIccFdnEnabled = (result[0] == 1) ? true : false;
    //             mIccFdnAvailable = true;
    //         }
    //         log("Query facility FDN : FDN service available: "+ mIccFdnAvailable
    //                 +" enabled: "  + mIccFdnEnabled);
    //     } else {
    //         loge("Bogus facility lock response");
    //     }
    // }
    assert(0);
}

void UiccCardApplication::OnChangeFdnDone(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     int attemptsRemaining = -1;
    //
    //     if (ar.exception == null) {
    //         mIccFdnEnabled = mDesiredFdnEnabled;
    //         if (DBG) log("EVENT_CHANGE_FACILITY_FDN_DONE: " +
    //                 "mIccFdnEnabled=" + mIccFdnEnabled);
    //     } else {
    //         attemptsRemaining = parsePinPukErrorResult(ar);
    //         loge("Error change facility fdn with exception " + ar.exception);
    //     }
    //     Message response = (Message)ar.userObj;
    //     response.arg1 = attemptsRemaining;
    //     AsyncResult.forMessage(response).exception = ar.exception;
    //     response.sendToTarget();
    // }
    assert(0);
}

void UiccCardApplication::QueryPin1State()
{
    // ==================before translated======================
    // int serviceClassX = CommandsInterface.SERVICE_CLASS_VOICE +
    //         CommandsInterface.SERVICE_CLASS_DATA +
    //         CommandsInterface.SERVICE_CLASS_FAX;
    // mCi.queryFacilityLockForApp (
    //     CommandsInterface.CB_FACILITY_BA_SIM, "", serviceClassX,
    //     mAid, mHandler.obtainMessage(EVENT_QUERY_FACILITY_LOCK_DONE));
    assert(0);
}

void UiccCardApplication::OnQueryFacilityLock(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if(ar.exception != null) {
    //         if (DBG) log("Error in querying facility lock:" + ar.exception);
    //         return;
    //     }
    //
    //     int[] ints = (int[])ar.result;
    //     if(ints.length != 0) {
    //         if (DBG) log("Query facility lock : "  + ints[0]);
    //
    //         mIccLockEnabled = (ints[0] != 0);
    //
    //         if (mIccLockEnabled) {
    //             mPinLockedRegistrants.notifyRegistrants();
    //         }
    //
    //         // Sanity check: we expect mPin1State to match mIccLockEnabled.
    //         // When mPin1State is DISABLED mIccLockEanbled should be false.
    //         // When mPin1State is ENABLED mIccLockEnabled should be true.
    //         //
    //         // Here we validate these assumptions to assist in identifying which ril/radio's
    //         // have not correctly implemented GET_SIM_STATUS
    //         switch (mPin1State) {
    //             case PINSTATE_DISABLED:
    //                 if (mIccLockEnabled) {
    //                     loge("QUERY_FACILITY_LOCK:enabled GET_SIM_STATUS.Pin1:disabled."
    //                             + " Fixme");
    //                 }
    //                 break;
    //             case PINSTATE_ENABLED_NOT_VERIFIED:
    //             case PINSTATE_ENABLED_VERIFIED:
    //             case PINSTATE_ENABLED_BLOCKED:
    //             case PINSTATE_ENABLED_PERM_BLOCKED:
    //                 if (!mIccLockEnabled) {
    //                     loge("QUERY_FACILITY_LOCK:disabled GET_SIM_STATUS.Pin1:enabled."
    //                             + " Fixme");
    //                 }
    //             case PINSTATE_UNKNOWN:
    //             default:
    //                 if (DBG) log("Ignoring: pin1state=" + mPin1State);
    //                 break;
    //         }
    //     } else {
    //         loge("Bogus facility lock response");
    //     }
    // }
    assert(0);
}

void UiccCardApplication::OnChangeFacilityLock(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     int attemptsRemaining = -1;
    //
    //     if (ar.exception == null) {
    //         mIccLockEnabled = mDesiredPinLocked;
    //         if (DBG) log( "EVENT_CHANGE_FACILITY_LOCK_DONE: mIccLockEnabled= "
    //                 + mIccLockEnabled);
    //     } else {
    //         attemptsRemaining = parsePinPukErrorResult(ar);
    //         loge("Error change facility lock with exception " + ar.exception);
    //     }
    //     Message response = (Message)ar.userObj;
    //     AsyncResult.forMessage(response).exception = ar.exception;
    //     response.arg1 = attemptsRemaining;
    //     response.sendToTarget();
    // }
    assert(0);
}

Int32 UiccCardApplication::ParsePinPukErrorResult(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // int[] result = (int[]) ar.result;
    // if (result == null) {
    //     return -1;
    // } else {
    //     int length = result.length;
    //     int attemptsRemaining = -1;
    //     if (length > 0) {
    //         attemptsRemaining = result[0];
    //     }
    //     log("parsePinPukErrorResult: attemptsRemaining=" + attemptsRemaining);
    //     return attemptsRemaining;
    // }
    assert(0);
    return 0;
}

void UiccCardApplication::NotifyReadyRegistrantsIfNeeded(
    /* [in] */ Registrant* r)
{
    // ==================before translated======================
    // if (mDestroyed) {
    //     return;
    // }
    // if (mAppState == AppState.APPSTATE_READY) {
    //     if (mPin1State == PinState.PINSTATE_ENABLED_NOT_VERIFIED ||
    //             mPin1State == PinState.PINSTATE_ENABLED_BLOCKED ||
    //             mPin1State == PinState.PINSTATE_ENABLED_PERM_BLOCKED) {
    //         loge("Sanity check failed! APPSTATE is ready while PIN1 is not verified!!!");
    //         // Don't notify if application is in insane state
    //         return;
    //     }
    //     if (r == null) {
    //         if (DBG) log("Notifying registrants: READY");
    //         mReadyRegistrants.notifyRegistrants();
    //     } else {
    //         if (DBG) log("Notifying 1 registrant: READY");
    //         r.notifyRegistrant(new AsyncResult(null, null, null));
    //     }
    // }
    assert(0);
}

void UiccCardApplication::NotifyPinLockedRegistrantsIfNeeded(
    /* [in] */ Registrant* r)
{
    // ==================before translated======================
    // if (mDestroyed) {
    //     return;
    // }
    //
    // if (mAppState == AppState.APPSTATE_PIN ||
    //         mAppState == AppState.APPSTATE_PUK) {
    //     if (mPin1State == PinState.PINSTATE_ENABLED_VERIFIED ||
    //             mPin1State == PinState.PINSTATE_DISABLED) {
    //         loge("Sanity check failed! APPSTATE is locked while PIN1 is not!!!");
    //         //Don't notify if application is in insane state
    //         return;
    //     }
    //     if (r == null) {
    //         if (DBG) log("Notifying registrants: LOCKED");
    //         mPinLockedRegistrants.notifyRegistrants();
    //     } else {
    //         if (DBG) log("Notifying 1 registrant: LOCKED");
    //         r.notifyRegistrant(new AsyncResult(null, null, null));
    //     }
    // }
    assert(0);
}

void UiccCardApplication::NotifyPersoLockedRegistrantsIfNeeded(
    /* [in] */ Registrant* r)
{
    // ==================before translated======================
    // if (mDestroyed) {
    //     return;
    // }
    //
    // if (mAppState == AppState.APPSTATE_SUBSCRIPTION_PERSO &&
    //         isPersoLocked()) {
    //     AsyncResult ar = new AsyncResult(null, mPersoSubState.ordinal(), null);
    //     if (r == null) {
    //         if (DBG) log("Notifying registrants: PERSO_LOCKED");
    //         mPersoLockedRegistrants.notifyRegistrants(ar);
    //     } else {
    //         if (DBG) log("Notifying 1 registrant: PERSO_LOCKED");
    //         r.notifyRegistrant(ar);
    //     }
    // }
    assert(0);
}

Int32 UiccCardApplication::GetAuthContext(
    /* [in] */ AppType appType)
{
    // ==================before translated======================
    // int authContext;
    //
    // switch (appType) {
    //     case APPTYPE_SIM:
    //         authContext = AUTH_CONTEXT_EAP_SIM;
    //         break;
    //
    //     case APPTYPE_USIM:
    //         authContext = AUTH_CONTEXT_EAP_AKA;
    //         break;
    //
    //     default:
    //         authContext = AUTH_CONTEXT_UNDEFINED;
    //         break;
    // }
    //
    // return authContext;
    assert(0);
    return 0;
}

void UiccCardApplication::Log(
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, msg);
    assert(0);
}

void UiccCardApplication::Loge(
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, msg);
    assert(0);
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
