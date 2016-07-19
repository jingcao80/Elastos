
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitor.h"
#include "Elastos.Droid.View.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode KeyguardUpdateMonitor::BatteryStatus::IsPluggedIn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mPlugged == IBatteryManager::BATTERY_PLUGGED_AC
            || mPlugged == IBatteryManager::BATTERY_PLUGGED_USB
            || mPlugged == IBatteryManager::BATTERY_PLUGGED_WIRELESS;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::BatteryStatus::isCharged(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mStatus == BATTERY_STATUS_FULL || mLevel >= 100;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::BatteryStatus::IsBatteryLow(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mLevel < LOW_BATTERY_THRESHOLD;
    return NOERROR;
}

KeyguardUpdateMonitor::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ KeyguardUpdateMonitor* host)
    : mHost(host)
{
    BroadcastReceiver::constructor();
}

ECode KeyguardUpdateMonitor::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (DEBUG) Logger::D(TAG, "received broadcast %s", action.string());

    if (IIntent::ACTION_TIME_TICK.Equals(action)
            || IIntent::ACTION_TIME_CHANGED.Equals(action)
            || IIntent::ACTION_TIMEZONE_CHANGED.Equals(action)) {
        mHost->mHandler->SendEmptyMessage(MSG_TIME_UPDATE);
    }
    else if (ITelephonyIntents::SPN_STRINGS_UPDATED_ACTION.Equals(action)) {
        mHost->mTelephonyPlmn = mHost->GetTelephonyPlmnFrom(intent);
        mHost->mTelephonySpn = mHost->GetTelephonySpnFrom(intent);
        mHost->mHandler->SendEmptyMessage(MSG_CARRIER_INFO_UPDATE);
    }
    else if (IIntent::ACTION_BATTERY_CHANGED.equals(action)) {
        Int32 status;
        intent->GetInt32Extra(EXTRA_STATUS, BATTERY_STATUS_UNKNOWN, &status);
        Int32 plugged;
        intent->GetInt32Extra(EXTRA_PLUGGED, 0, &plugged);
        Int32 level;
        intent->GetInt32Extra(EXTRA_LEVEL, 0, &level);
        Int32 health;
        intent->GetInt32Extra(EXTRA_HEALTH, BATTERY_HEALTH_UNKNOWN, &health);
        AutoPtr<BatteryStatus> status = new BatteryStatus(status, level, plugged, health);
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_BATTERY_UPDATE, status, (IMessage**)&msg);
        mHost->mHandler->SendMessage(msg);
    }
    else if (ITelephonyIntents::ACTION_SIM_STATE_CHANGED.Equals(action)) {
        // if (DEBUG_SIM_STATES) {
        //     Log.v(TAG, "action " + action + " state" +
        //         intent.getStringExtra(IccCardConstants.INTENT_KEY_ICC_STATE));
        // }
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_SIM_STATE_CHANGE, SimArgs::FromIntent(intent), (IMessage**)&msg);
        mHost->mHandler->SendMessage(msg);
    }
    else if (IAudioManager::RINGER_MODE_CHANGED_ACTION.Equals(action)) {
        Int32 extra;
        intent->GetInt32Extra(IAudioManager::EXTRA_RINGER_MODE, -1, &extra);
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_RINGER_MODE_CHANGED, extra, 0, (IMessage**)&msg);
        mHost->mHandler->SendMessage(msg);
    }
    else if (ITelephonyManager::ACTION_PHONE_STATE_CHANGED.Equals(action)) {
        String state;
        intent->GetStringExtra(ITelephonyManager::EXTRA_STATE, &state);
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_PHONE_STATE_CHANGED, state, (IMessage**)&msg);
        mHost->mHandler->SendMessage(msg);
    }
    else if (IIntent::ACTION_USER_REMOVED.Equals(action)) {
        Int32 extra;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, 0, &extra);
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_USER_REMOVED, extra, 0, (IMessage**)&msg);
        mHost->mHandler->SendMessage(msg);
    }
    else if (IIntent::ACTION_BOOT_COMPLETED.Equals(action)) {
        mHost->DispatchBootCompleted();
    }
    return NOERROR;
}

KeyguardUpdateMonitor::MyBroadcastReceiver2::MyBroadcastReceiver(
    /* [in] */ KeyguardUpdateMonitor* host)
    : mHost(host)
{
    BroadcastReceiver::constructor();
}

ECode KeyguardUpdateMonitor::MyBroadcastReceiver2::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IAlarmManager::ACTION_NEXT_ALARM_CLOCK_CHANGED.Equals(action)) {
        mHost->mHandler->SendEmptyMessage(MSG_TIME_UPDATE);
    }
    else if (IIntent::ACTION_USER_INFO_CHANGED.Equals(action)) {
        Int32 id;
        GetSendingUserId(&id);
        Int32 extra;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, id, &extra);
        AutoPtr<IMessage> massage;
        mHost->mHandler->ObtainMessage(MSG_USER_INFO_CHANGED, extra, 0, (IMessage**)&massage);
        massage->SendMessage(massage);
    }
    else if (ACTION_FACE_UNLOCK_STARTED.Equals(action)) {
        Int32 id;
        GetSendingUserId(&id);
        AutoPtr<IMessage> massage;
        mHost->mHandler->ObtainMessage(MSG_FACE_UNLOCK_STATE_CHANGED, 1, id, (IMessage**)&massage);
        mHost->mHandler->SendMessage(massage);
    }
    else if (ACTION_FACE_UNLOCK_STOPPED.Equals(action)) {
        Int32 id;
        GetSendingUserId(&id);
        AutoPtr<IMessage> massage;
        mHost->mHandler->ObtainMessage(MSG_FACE_UNLOCK_STATE_CHANGED, 0, id, (IMessage**)&massage);
        mHost->mHandler->SendMessage(massage);
    }
    else if (IDevicePolicyManager::ACTION_DEVICE_POLICY_MANAGER_STATE_CHANGED
            .Equals(action)) {
        mHost->mHandler->SendEmptyMessage(MSG_DPM_STATE_CHANGED);
    }
    return NOERROR;
}

KeyguardUpdateMonitor::MyContentObserver::MyContentObserver(
    /* [in] */ KeyguardUpdateMonitor* host)
    : mHost(host)
{
    ContentObserver::constructor();
}

ECode KeyguardUpdateMonitor::MyContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    ContentObserver::OnChange(selfChange);
    mHost->mDeviceProvisioned = IsDeviceProvisionedInSettingsDb();
    if (mDeviceProvisioned) {
        mHost->mHandler->SendEmptyMessage(MSG_DEVICE_PROVISIONED);
    }
    if (DEBUG) Logger::D(TAG, "DEVICE_PROVISIONED state = %d", mHost->mDeviceProvisioned);
    return NOERROR;
}

KeyguardUpdateMonitor::MyHandler::MyHandler(
    /* [in] */ KeyguardUpdateMonitor* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode KeyguardUpdateMonitor::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_TIME_UPDATE:
            mHost->HandleTimeUpdate();
            break;
        case MSG_BATTERY_UPDATE:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->HandleBatteryUpdate(IBatteryStatus::Probe(obj));
            break;
        }
        case MSG_CARRIER_INFO_UPDATE:
            mHost->HandleCarrierInfoUpdate();
            break;
        case MSG_SIM_STATE_CHANGE:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->HandleSimStateChange((SimArgs*)IObject::Probe(obj));
            break;
        }
        case MSG_RINGER_MODE_CHANGED:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleRingerModeChange(arg1);
            break;
        }
        case MSG_PHONE_STATE_CHANGED:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<ICharSequence> cchar = ICharSequence::Probe(obj);
            String str;
            cchar->ToString(&str);
            mHost->HandlePhoneStateChanged(str);
            break;
        }
        case MSG_CLOCK_VISIBILITY_CHANGED:
            mHost->HandleClockVisibilityChanged();
            break;
        case MSG_DEVICE_PROVISIONED:
            mHost->HandleDeviceProvisioned();
            break;
        case MSG_DPM_STATE_CHANGED:
            mHost->HandleDevicePolicyManagerStateChanged();
            break;
        case MSG_USER_SWITCHING:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->HandleUserSwitching(arg1, IIRemoteCallback::Probe(obj));
            break;
        }
        case MSG_USER_SWITCH_COMPLETE:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleUserSwitchComplete(arg1);
            break;
        }
        case MSG_USER_REMOVED:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleUserRemoved(arg1);
            break;
        }
        case MSG_KEYGUARD_VISIBILITY_CHANGED:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleKeyguardVisibilityChanged(arg1);
            break;
        }
        case MSG_KEYGUARD_BOUNCER_CHANGED:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleKeyguardBouncerChanged(arg1);
            break;
        }
        case MSG_BOOT_COMPLETED:
            mHost->HandleBootCompleted();
            break;
        case MSG_USER_INFO_CHANGED:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleUserInfoChanged(arg1);
            break;
        }
        case MSG_REPORT_EMERGENCY_CALL_ACTION:
            mHost->HandleReportEmergencyCallAction();
            break;
        case MSG_SCREEN_TURNED_OFF:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleScreenTurnedOff(arg1);
            break;
        }
        case MSG_SCREEN_TURNED_ON:
            mHost->HandleScreenTurnedOn();
            break;
        case MSG_FINGERPRINT_ACQUIRED:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleFingerprintAcquired(arg1);
            break;
        }
        case MSG_FINGERPRINT_PROCESSED:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleFingerprintProcessed(arg1);
            break;
        }
        case MSG_FACE_UNLOCK_STATE_CHANGED:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            Int32 arg2;
            msg->GetArg2(&arg2);
            mHost->HandleFaceUnlockStateChanged(arg1 != 0, arg2);
            break;
        }
    }
    return NOERROR;
}

ECode KeyguardUpdateMonitor::SimArgs::FromIntent(
    /* [in] */ IIntent* intent,
    /* [out] */ SimArgs** args)
{
    VALIDATE_NOT_NULL(args)

    IccCardConstantsState state;
    Int32 action;
    intent->GetAction(&action);
    if (!ITelephonyIntents::ACTION_SIM_STATE_CHANGED.Equals(action)) {
        //throw new IllegalArgumentException("only handles intent ACTION_SIM_STATE_CHANGED");
        return IllegalArgumentException;
    }
    String stateExtra;
    intent->GetStringExtra(IIccCardConstants::INTENT_KEY_ICC_STATE, &stateExtra);
    if (IIccCardConstants::INTENT_VALUE_ICC_ABSENT.Equals(stateExtra)) {
        String absentReason;
        intent->GetStringExtra(IIccCardConstants::INTENT_KEY_LOCKED_REASON, &absentReason);

        if (IIccCardConstants::INTENT_VALUE_ABSENT_ON_PERM_DISABLED.Equals(
                absentReason)) {
            state = IccCardConstantsState_PERM_DISABLED;
        }
        else {
            state = IccCardConstantsState_ABSENT;
        }
    }
    else if (IIccCardConstants::INTENT_VALUE_ICC_READY.Equals(stateExtra)) {
        state = IccCardConstantsState_READY;
    }
    else if (IIccCardConstants::INTENT_VALUE_ICC_LOCKED.Equals(stateExtra)) {
        String lockedReason;
        intent->GetStringExtra(IIccCardConstants::INTENT_KEY_LOCKED_REASON, &lockedReason);
        if (IIccCardConstants::INTENT_VALUE_LOCKED_ON_PIN.Equals(lockedReason)) {
            state = IccCardConstantsState_PIN_REQUIRED;
        }
        else if (IIccCardConstants::INTENT_VALUE_LOCKED_ON_PUK.Equals(lockedReason)) {
            state = IccCardConstantsState_PUK_REQUIRED;
        }
        else {
            state = IccCardConstantsState_UNKNOWN;
        }
    }
    else if (IIccCardConstants::INTENT_VALUE_LOCKED_NETWORK.Equals(stateExtra)) {
        state = IccCardConstantsState_NETWORK_LOCKED;
    }
    else if (IIccCardConstants::INTENT_VALUE_ICC_LOADED.Equals(stateExtra)
                || IccCardConstants.INTENT_VALUE_ICC_IMSI.Equals(stateExtra)) {
        // This is required because telephony doesn't return to "READY" after
        // these state transitions. See bug 7197471.
        state = IccCardConstantsState_READY;
    }
    else {
        state = IccCardConstantsState_UNKNOWN;
    }
    AutoPtr<SimArgs> _args new SimArgs(state);
    *args = _args;
    REFCOUNT_ADD(*args);
    return NOERROR;
}

ECode KeyguardUpdateMonitor::SimArgs::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    assert(0);
    //return mSimState.toString();
    return NOERROR;
}

KeyguardUpdateMonitor::MyFingerprintManagerReceiver::MyFingerprintManagerReceiver(
    /* [in] */ KeyguardUpdateMonitor* host)
    : mHost(host)
{
    FingerprintManagerReceiver::constructor();
}

ECode KeyguardUpdateMonitor::MyFingerprintManagerReceiver::OnProcessed(
    /* [in] */ Int32 fingerprintId)
{
    AutoPtr<IMessage> m;
    mHost->mHandler->ObtainMessage(MSG_FINGERPRINT_PROCESSED, fingerprintId, 0, (IMessage**)&m);
    return m->SendToTarget();
}

ECode KeyguardUpdateMonitor::MyFingerprintManagerReceiver::OnAcquired(
    /* [in] */ Int32 info)
{
    AutoPtr<IMessage> m;
    mHost->mHandler->ObtainMessage(MSG_FINGERPRINT_ACQUIRED, info, 0, (IMessage**)&m);
    return m->SendToTarget();
}

ECode KeyguardUpdateMonitor::MyFingerprintManagerReceiver::OnError(
    /* [in] */ Int32 error)
{
    if (DEBUG) Logger::W(TAG, "FingerprintManager reported error: %d", error);
    return NOERROR;
}

const String KeyguardUpdateMonitor::TAG("KeyguardUpdateMonitor");
const Boolean KeyguardUpdateMonitor::DEBUG = IKeyguardConstants::DEBUG;
const Boolean KeyguardUpdateMonitor::DEBUG_SIM_STATES = DEBUG || FALSE;
const Int32 KeyguardUpdateMonitor::FAILED_BIOMETRIC_UNLOCK_ATTEMPTS_BEFORE_BACKUP = 3;
const Int32 KeyguardUpdateMonitor::LOW_BATTERY_THRESHOLD = 20;

const String KeyguardUpdateMonitor::ACTION_FACE_UNLOCK_STARTED("com.android.facelock.FACE_UNLOCK_STARTED");
const String KeyguardUpdateMonitor::ACTION_FACE_UNLOCK_STOPPED("com.android.facelock.FACE_UNLOCK_STOPPED");

// Callback messages
const Int32 KeyguardUpdateMonitor::MSG_TIME_UPDATE = 301;
const Int32 KeyguardUpdateMonitor::MSG_BATTERY_UPDATE = 302;
const Int32 KeyguardUpdateMonitor::MSG_CARRIER_INFO_UPDATE = 303;
const Int32 KeyguardUpdateMonitor::MSG_SIM_STATE_CHANGE = 304;
const Int32 KeyguardUpdateMonitor::MSG_RINGER_MODE_CHANGED = 305;
const Int32 KeyguardUpdateMonitor::MSG_PHONE_STATE_CHANGED = 306;
const Int32 KeyguardUpdateMonitor::MSG_CLOCK_VISIBILITY_CHANGED = 307;
const Int32 KeyguardUpdateMonitor::MSG_DEVICE_PROVISIONED = 308;
const Int32 KeyguardUpdateMonitor::MSG_DPM_STATE_CHANGED = 309;
const Int32 KeyguardUpdateMonitor::MSG_USER_SWITCHING = 310;
const Int32 KeyguardUpdateMonitor::MSG_USER_REMOVED = 311;
const Int32 KeyguardUpdateMonitor::MSG_KEYGUARD_VISIBILITY_CHANGED = 312;
const Int32 KeyguardUpdateMonitor::MSG_BOOT_COMPLETED = 313;
const Int32 KeyguardUpdateMonitor::MSG_USER_SWITCH_COMPLETE = 314;
const Int32 KeyguardUpdateMonitor::MSG_SET_CURRENT_CLIENT_ID = 315;
const Int32 KeyguardUpdateMonitor::MSG_SET_PLAYBACK_STATE = 316;
const Int32 KeyguardUpdateMonitor::MSG_USER_INFO_CHANGED = 317;
const Int32 KeyguardUpdateMonitor::MSG_REPORT_EMERGENCY_CALL_ACTION = 318;
const Int32 KeyguardUpdateMonitor::MSG_SCREEN_TURNED_ON = 319;
const Int32 KeyguardUpdateMonitor::MSG_SCREEN_TURNED_OFF = 320;
const Int32 KeyguardUpdateMonitor::MSG_KEYGUARD_BOUNCER_CHANGED = 322;
const Int32 KeyguardUpdateMonitor::MSG_FINGERPRINT_PROCESSED = 323;
const Int32 KeyguardUpdateMonitor::MSG_FINGERPRINT_ACQUIRED = 324;
const Int32 KeyguardUpdateMonitor::MSG_FACE_UNLOCK_STATE_CHANGED = 325;

AutoPtr<KeyguardUpdateMonitor> KeyguardUpdateMonitor::sInstance;

CAR_INTERFACE_IMPL(KeyguardUpdateMonitor, Object, ITrustListener)

KeyguardUpdateMonitor::KeyguardUpdateMonitor()
    : mSimState(IccCardConstantsState_READY)
    , mRingMode(0)
    , mPhoneState(0)
    , mKeyguardIsVisible(FALSE)
    , mBouncer(FALSE)
    , mBootCompleted(FALSE)
    , mDeviceProvisioned(FALSE)
    , mFailedAttempts(0)
    , mFailedBiometricUnlockAttempts(0)
    , mAlternateUnlockEnabled(0)
    , mClockVisible(0)
    , mSwitchingUser(FALSE)
    , mScreenOn(FALSE)
{
    CArrayList::New((IList**)&mCallbacks);

    mHandler = new MyHandler(this);

    CSparseBooleanArray::New((ISparseBooleanArray**)&mUserHasTrust);
    CSparseBooleanArray::New((ISparseBooleanArray**)&mUserTrustIsManaged);
    CSparseBooleanArray::New((ISparseBooleanArray**)&mUserFingerprintRecognized);
    CSparseBooleanArray::New((ISparseBooleanArray**)&mUserFaceUnlockRunning);

    mDisplayClientState = new DisplayClientState();

    CKeyguardUpdateMonitorBroadcastReceiver1::New((IBroadcastReceiver**)&mBroadcastReceiver);

    CKeyguardUpdateMonitorBroadcastReceiver2::New((IBroadcastReceiver**)&mBroadcastAllReceiver);
}

ECode KeyguardUpdateMonitor::OnTrustChanged(
    /* [in] */ Boolean enabled,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean initiatedByUser)
{
    mUserHasTrust->Put(userId, enabled);

    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnTrustChanged(userId);
            if (enabled && initiatedByUser) {
                cb->OnTrustInitiatedByUser(userId);
            }
        }
    }
    return NOERROR;
}

ECode KeyguardUpdateMonitor::OnTrustManagedChanged(
    /* [in] */ Boolean managed,
    /* [in] */ Int32 userId)
{
    mUserTrustIsManaged->Put(userId, managed);

    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnTrustManagedChanged(userId);
        }
    }
    return NOERROR;
}

void KeyguardUpdateMonitor::OnFingerprintRecognized(
    /* [in] */ Int32 userId)
{
    mUserFingerprintRecognized->Put(userId, TRUE);

    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnFingerprintRecognized(userId);
        }
    }
}

void KeyguardUpdateMonitor::HandleFingerprintProcessed(
    /* [in] */ Int32 fingerprintId);
{
    if (fingerprintId == 0) return; // not a valid fingerprint

    Int32 userId;
    //try {
    ECode ec = NOERROR;
    AutoPtr<IIActivityManager> manager = ActivityManagerNative::GetDefault();
    AutoPtr<IUserInfo> user;
    FAIL_GOTO(ec = manager->GetCurrentUser((IUserInfo**)&user), ERROR)
    FAIL_GOTO(ec = user->GetId(&userId), ERROR)
    //} catch (RemoteException e) {
ERROR:
    if (ec == (ECode)RemoteException) {
        Logger::E(TAG, "Failed to get current user id: %d", ec);
        return;
    }
    if (IsFingerprintDisabled(userId)) {
        Logger::D(TAG, "Fingerprint disabled by DPM for userId: %d", userId);
        return;
    }
    AutoPtr<IContentResolver> res;
    mContext->GetContentResolver((IContentResolver**)&res);
    AutoPtr<IFingerprintUtils> helper;
    CFingerprintUtils::AcquireSingleton((IFingerprintUtils**)&helper);
    AutoPtr<ArrayOf<Int32> > ids;
    helper->GetFingerprintIdsForUser(res, userId, (ArrayOf<Int32>**)&ids);
    for (Int32 i = 0; i < ids->GetLength(); i++) {
        if ((*ids)[i] == fingerprintId) {
            OnFingerprintRecognized(userId);
        }
    }
}

void KeyguardUpdateMonitor::HandleFingerprintAcquired(
    /* [in] */ Int32 info)
{
    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnFingerprintAcquired(info);
        }
    }
}

void KeyguardUpdateMonitor::HandleFaceUnlockStateChanged(
    /* [in] */ Boolean running,
    /* [in] */ Int32 userId)
{
    mUserFaceUnlockRunning->Put(userId, running);

    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnFaceUnlockStateChanged(running, userId);
        }
    }
}

ECode KeyguardUpdateMonitor::IsFaceUnlockRunning(
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return mUserFaceUnlockRunning->Get(userId, result);
}

Boolean KeyguardUpdateMonitor::IsTrustDisabled(
    /* [in] */ Int32 userId)
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::DEVICE_POLICY_SERVICE, (IInterface**)&obj);
    AutoPtr<IDevicePolicyManager> dpm = IDevicePolicyManager::Probe(obj);
    if (dpm != NULL) {
            // TODO once UI is finalized
            Boolean disabledByGlobalActions = FALSE;
            Boolean disabledBySettings = FALSE;

            // Don't allow trust agent if device is secured with a SIM PIN. This is here
            // mainly because there's no other way to prompt the user to enter their SIM PIN
            // once they get past the keyguard screen.
            Boolean disabledBySimPin = IsSimPinSecure();

            Int32 which;
            dpm->GetKeyguardDisabledFeatures(NULL, userId, &which);
            Boolean disabledByDpm = (which & IDevicePolicyManager::KEYGUARD_DISABLE_TRUST_AGENTS) != 0;
            return disabledByDpm || disabledByGlobalActions || disabledBySettings
                    || disabledBySimPin;
    }
    return FALSE;
}

Boolean KeyguardUpdateMonitor::IsFingerprintDisabled(
    /* [in] */ Int32 userId)
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::DEVICE_POLICY_SERVICE, (IInterface**)&obj);
    AutoPtr<IDevicePolicyManager> dpm = IDevicePolicyManager::Probe(obj);

    Int32 which;
    return dpm != NULL && ((dpm->GetKeyguardDisabledFeatures(NULL, userId, &which), which)
                & IDevicePolicyManager::KEYGUARD_DISABLE_FINGERPRINT) != 0;
}

ECode KeyguardUpdateMonitor::GetUserHasTrust(
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = !IsTrustDisabled(userId) && mUserHasTrust->Get(userId)
                || mUserFingerprintRecognized->Get(userId);
    return NOERROR;
}

ECode KeyguardUpdateMonitor::GetUserTrustIsManaged(
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mUserTrustIsManaged->Get(userId) && !IsTrustDisabled(userId);
    return NOERROR;
}

AutoPtr<KeyguardUpdateMonitor> KeyguardUpdateMonitor::GetInstance(
    /* [in] */ IContext* context)
{
    if (sInstance == NULL) {
        sInstance = new KeyguardUpdateMonitor(context);
    }
    return sInstance;
}

ECode KeyguardUpdateMonitor::HandleScreenTurnedOn()
{
    Int32 count;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnScreenTurnedOn();
        }
    }
    return NOERROR;
}

ECode KeyguardUpdateMonitor::HandleScreenTurnedOff(
    /* [in] */ Int32 arg1)
{
    ClearFingerprintRecognized();
    Int32 count;
    mCallbacks->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnScreenTurnedOff(arg1);
        }
    }
    return NOERROR;
}

ECode KeyguardUpdateMonitor::DispatchSetBackground(
    /* [in] */ IBitmap* bmp)
{
    if (DEBUG) Logger::D(TAG, "dispatchSetBackground");
    Int32 count;
    mCallbacks->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnSetBackground(bmp);
        }
    }
    return NOERROR;
}

ECode KeyguardUpdateMonitor::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    IsDeviceProvisionedInSettingsDb(&mDeviceProvisioned);
    // Since device can't be un-provisioned, we only need to register a content observer
    // to update mDeviceProvisioned when we are...
    if (!mDeviceProvisioned) {
        WatchForDeviceProvisioning();
    }

    // Take a guess at initial SIM state, battery status and PLMN until we get an update
    mSimState = IccCardConstantsState_NOT_READY;
    mBatteryStatus = new BatteryStatus(BATTERY_STATUS_UNKNOWN, 100, 0, 0);
    mTelephonyPlmn = GetDefaultPlmn();

    // Watch for interesting updates
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_TIME_TICK);
    filter->AddAction(IIntent::ACTION_TIME_CHANGED);
    filter->AddAction(IIntent::ACTION_BATTERY_CHANGED);
    filter->AddAction(IIntent::ACTION_TIMEZONE_CHANGED);
    filter->AddAction(ITelephonyIntents::ACTION_SIM_STATE_CHANGED);
    filter->AddAction(ITelephonyManager::ACTION_PHONE_STATE_CHANGED);
    filter->AddAction(ITelephonyIntents::SPN_STRINGS_UPDATED_ACTION);
    filter->AddAction(IAudioManager::RINGER_MODE_CHANGED_ACTION);
    filter->AddAction(IIntent::ACTION_USER_REMOVED);
    context->RegisterReceiver(mBroadcastReceiver, filter);

    AutoPtr<IIntentFilter> bootCompleteFilter;
    CIntentFilter::New((IIntentFilter**)&bootCompleteFilter);
    bootCompleteFilter->SetPriority(IIntentFilter::SYSTEM_HIGH_PRIORITY);
    bootCompleteFilter->AddAction(IIntent::ACTION_BOOT_COMPLETED);
    context->RegisterReceiver(mBroadcastReceiver, bootCompleteFilter);

    AutoPtr<IIntentFilter> allUserFilter;
    CIntentFilter::New((IIntentFilter**)&allUserFilter);
    allUserFilter->AddAction(IIntent::ACTION_USER_INFO_CHANGED);
    allUserFilter->AddAction(IAlarmManager::ACTION_NEXT_ALARM_CLOCK_CHANGED);
    allUserFilter->AddAction(ACTION_FACE_UNLOCK_STARTED);
    allUserFilter->AddAction(ACTION_FACE_UNLOCK_STOPPED);
    allUserFilter->AddAction(IDevicePolicyManager::ACTION_DEVICE_POLICY_MANAGER_STATE_CHANGED);
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> all;
    helper->GetALL((IUserHandle**)&all);
    context->RegisterReceiverAsUser(mBroadcastAllReceiver, all, allUserFilter,
            NULL, NULL);

    //try {
    AutoPtr<IIUserSwitchObserver> observer;
    CKeyguardUpdateMonitorUserSwitchObserver::New(this, (IIUserSwitchObserver**)&observer);
    AutoPtr<IIActivityManager> manager = ActivityManagerNative::GetDefault();
    manager->RegisterUserSwitchObserver(observer);
    // } catch (RemoteException e) {
    //     // TODO Auto-generated catch block
    //     e.printStackTrace();
    // }

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::TRUST_SERVICE, (IInterface**)&obj);
    AutoPtr<ITrustManager> trustManager = ITrustManager::Probe(obj);
    trustManager->RegisterTrustListener(this);

    AutoPtr<IFingerprintManager> fpm;
    AutoPtr<IInterface> obj2;
    context->GetSystemService(IContext::FINGERPRINT_SERVICE, (IInterface**)&obj2);
    fpm = IFingerprintManager::Probe(obj2);
    return fpm->StartListening(mFingerprintManagerReceiver);
}

Boolean KeyguardUpdateMonitor::IsDeviceProvisionedInSettingsDb()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);

    AutoPtr<ISettingsGlobal> helper;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
    Int32 num;
    helper->GetInt32(cr, ISettingsGlobal::DEVICE_PROVISIONED, 0, &num);
    return num != 0;
}

void KeyguardUpdateMonitor::WatchForDeviceProvisioning()
{
    CKeyguardUpdateMonitorContentObserver::New((IContentObserver**)&mDeviceProvisionedObserver);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> helper;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
    AutoPtr<IUri> value;
    helper->GetUriFor(ISettingsGlobal::DEVICE_PROVISIONED, (IUri**)&value);
    cr->RegisterContentObserver(value, FALSE, mDeviceProvisionedObserver);

    // prevent a race condition between where we check the flag and where we register the
    // observer by grabbing the value once again...
    Boolean provisioned = IsDeviceProvisionedInSettingsDb();
    if (provisioned != mDeviceProvisioned) {
        mDeviceProvisioned = provisioned;
        if (mDeviceProvisioned) {
            mHandler->SendEmptyMessage(MSG_DEVICE_PROVISIONED);
        }
    }
}

ECode KeyguardUpdateMonitor::HandleDevicePolicyManagerStateChanged()
{
    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnDevicePolicyManagerStateChanged();
        }
    }
    return NOERROR;
}


ECode KeyguardUpdateMonitor::HandleUserSwitching(
    /* [in] */ Int32 userId,
    /* [in] */ IIRemoteCallback* reply)
{
    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnUserSwitching(userId);
        }
    }
    //try {
    return reply->SendResult(NULL);
    // } catch (RemoteException e) {
    // }
}

ECode KeyguardUpdateMonitor::HandleUserSwitchComplete(
    /* [in] */ Int32 userId)
{
    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnUserSwitchComplete(userId);
        }
    }
    return NOERROR;
}

ECode KeyguardUpdateMonitor::DispatchBootCompleted()
{
    return mHandler->SendEmptyMessage(MSG_BOOT_COMPLETED);
}

ECode KeyguardUpdateMonitor::HandleBootCompleted()
{
    if (mBootCompleted) return NOERROR;
    mBootCompleted = TRUE;

    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnBootCompleted();
        }
    }
    return NOERROR;
}

ECode KeyguardUpdateMonitor::HasBootCompleted(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mBootCompleted;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::HandleUserRemoved(
    /* [in] */ Int32 userId)
{
    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnUserRemoved(userId);
        }
    }
    return NOERROR;
}

ECode KeyguardUpdateMonitor::HandleDeviceProvisioned()
{
    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnDeviceProvisioned();
        }
    }
    if (mDeviceProvisionedObserver != NULL) {
        // We don't need the observer anymore...
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        cr->UnregisterContentObserver(mDeviceProvisionedObserver);
        mDeviceProvisionedObserver = NULL;
    }
    return NOERROR;
}

ECode KeyguardUpdateMonitor::HandlePhoneStateChanged(
    /* [in] */ const String& newState)
{
    if (DEBUG) Logger::D(TAG, "handlePhoneStateChanged(%s)", newState.string());
    if (ITelephonyManager::EXTRA_STATE_IDLE.Equals(newState)) {
        mPhoneState = ITelephonyManager::CALL_STATE_IDLE;
    }
    else if (ITelephonyManager::EXTRA_STATE_OFFHOOK.Equals(newState)) {
        mPhoneState = ITelephonyManager::CALL_STATE_OFFHOOK;
    }
    else if (ITelephonyManager::EXTRA_STATE_RINGING.Equals(newState)) {
        mPhoneState = ITelephonyManager::CALL_STATE_RINGING;
    }

    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnPhoneStateChanged(mPhoneState);
        }
    }
    return NOERROR;
}

ECode KeyguardUpdateMonitor::HandleRingerModeChange(
    /* [in] */ Int32 mode)
{
    if (DEBUG) Logger::D(TAG, "handleRingerModeChange(%d)", mode);
    mRingMode = mode;

    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnRingerModeChanged(mode);
        }
    }
    return NOERROR;
}

void KeyguardUpdateMonitor::HandleTimeUpdate()
{
    if (DEBUG) Logger::D(TAG, "handleTimeUpdate");

    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnTimeChanged();
        }
    }
}

void KeyguardUpdateMonitor::HandleBatteryUpdate(
    /* [in] */ BatteryStatus* status)
{
    if (DEBUG) Logger::D(TAG, "handleBatteryUpdate");
    Boolean batteryUpdateInteresting = IsBatteryUpdateInteresting(mBatteryStatus, status);
    mBatteryStatus = status;
    if (batteryUpdateInteresting) {
        Int32 size;
        mCallbacks->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mCallbacks->Get(i, (IInterface**)&obj);
            AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
            AutoPtr<IInterface> callbackObj;
            w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
            AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
            if (cb != NULL) {
                cb->OnRefreshBatteryInfo(status);
            }
        }
    }
}

void KeyguardUpdateMonitor::HandleCarrierInfoUpdate()
{
    if (DEBUG) Logger::D(TAG, "handleCarrierInfoUpdate: plmn = %s, spn = %s",
            TO_CSTR(mTelephonyPlmn), TO_CSTR(mTelephonySpn));

    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnRefreshCarrierInfo(mTelephonyPlmn, mTelephonySpn);
        }
    }
}voi KeyguardUpdateMonitor::::HandleSimStateChange(
    /* [in] */ SimArgs* simArgs)
{
    IccCardConstantsState state = simArgs->mSimState;

    if (DEBUG) {
        // Logger::D(TAG, "handleSimStateChange: intentValue = " + simArgs + " "
        //         + "state resolved to " + state.toString());
    }

    if (state != IccCardConstantsState_UNKNOWN && state != mSimState) {
        mSimState = state;

        Int32 size;
        mCallbacks->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mCallbacks->Get(i, (IInterface**)&obj);
            AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
            AutoPtr<IInterface> callbackObj;
            w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
            AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
            if (cb != NULL) {
                cb->OnSimStateChanged(state);
            }
        }
    }
}

void KeyguardUpdateMonitor::HandleClockVisibilityChanged()
{
    if (DEBUG) Logger::D(TAG, "handleClockVisibilityChanged()");

    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnClockVisibilityChanged();
        }
    }
}
void KeyguardUpdateMonitor::HandleKeyguardVisibilityChanged(
    /* [in] */ Int32 showing)
{
    if (DEBUG) Logger::D(TAG, "handleKeyguardVisibilityChanged(%d)", showing);
    Boolean isShowing = (showing == 1);
    mKeyguardIsVisible = isShowing;

    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnKeyguardVisibilityChangedRaw(isShowing);
        }
    }
}

void KeyguardUpdateMonitor::HandleKeyguardBouncerChanged(
    /* [in] */ Int32 bouncer)
{
    if (DEBUG) Logger::D(TAG, "handleKeyguardBouncerChanged(%d)", bouncer);
    Boolean isBouncer = (bouncer == 1);
    mBouncer = isBouncer;

    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnKeyguardBouncerChanged(isBouncer);
        }
    }
}

void KeyguardUpdateMonitor::HandleReportEmergencyCallAction()
{
    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (cb != NULL) {
            cb->OnEmergencyCallAction();
        }
    }
}

ECode KeyguardUpdateMonitor::IsKeyguardVisible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mKeyguardIsVisible;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::IsKeyguardBouncer(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mBouncer;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::IsSwitchingUser(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mSwitchingUser;
    return NOERROR;
}

Boolean KeyguardUpdateMonitor::IsBatteryUpdateInteresting(
    /* [in] */ BatteryStatus* old,
    /* [in] */ BatteryStatus* current)
{
    Boolean nowPluggedIn;
    current->IsPluggedIn(&nowPluggedIn);
    Boolean wasPluggedIn;
    old->IsPluggedIn(&wasPluggedIn);
    Boolean stateChangedWhilePluggedIn =
        wasPluggedIn == TRUE && nowPluggedIn == TRUE
        && (old->mStatus != current->mStatus);

    // change in plug state is always interesting
    if (wasPluggedIn != nowPluggedIn || stateChangedWhilePluggedIn) {
        return TRUE;
    }

    // change in battery level while plugged in
    if (nowPluggedIn && old->mLevel != current->mLevel) {
        return TRUE;
    }

    // change where battery needs charging
    Boolean res;
    if (!nowPluggedIn && (current->IsBatteryLow(&res), res) && current->mLevel != old->mLevel) {
        return TRUE;
    }
    return FALSE;
}

AutoPtr<ICharSequence> KeyguardUpdateMonitor::GetTelephonyPlmnFrom(
    /* [in] */ IIntent* intent)
{
    Boolean res;
    intent->GetBooleanExtra(ITelephonyIntents::EXTRA_SHOW_PLMN, FALSE, &res);
    if (res) {
        String plmn;
        intent->GetStringExtra(ITelephonyIntents::EXTRA_PLMN, &plmn);
        return (plmn != NULL) ? CoreUtils::Convert(plmn) : GetDefaultPlmn();
    }
    return NULL;
}

AutoPtr<ICharSequence> KeyguardUpdateMonitor::GetDefaultPlmn()
{
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> cchar;
    resources->GetText(R::string::keyguard_carrier_default, (ICharSequence**)&cchar);
    return cchar;
}

AutoPtr<ICharSequence> KeyguardUpdateMonitor::GetTelephonySpnFrom(
    /* [in] */ IIntent* intent)
{
    Boolean res;
    intent->GetBooleanExtra(ITelephonyIntents::EXTRA_SHOW_SPN, FALSE, &res);
    if (res) {
        String spn;
        intent->GetStringExtra(ITelephonyIntents::EXTRA_SPN, &spn);
        if (!spn.IsNull()) {
            return CoreUtils::Convert(spn);
        }
    }
    return NULL;
}

ECode KeyguardUpdateMonitor::RemoveCallback(
    /* [in] */ IKeyguardUpdateMonitorCallback* callback)
{
    if (DEBUG) Logger::V(TAG, "*** unregister callback for %s", TO_CSTR(callback));

    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (TO_IINTERFACE(cb) == TO_IINTERFACE(callback)) {
            mCallbacks->Remove(i);
        }
    }
    return NOERROR;
}

ECode KeyguardUpdateMonitor::RegisterCallback(
    /* [in] */ IKeyguardUpdateMonitorCallback* callback)
{
    if (DEBUG) Logger::V(TAG, "*** register callback for %s", TO_CSTR(callback));
    // Prevent adding duplicate callbacks
    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<IWeakReference> w = IWeakReference::Probe(obj);
        AutoPtr<IInterface> callbackObj;
        w->Resolve(EIID_IInterface, (IInterface**)&callbackObj);
        AutoPtr<IKeyguardUpdateMonitorCallback> cb = IKeyguardUpdateMonitorCallback::Probe(callbackObj);
        if (TO_IINTERFACE(cb) == TO_IINTERFACE(callback)) {
            // if (DEBUG) Logger::E(TAG, "Object tried to add another callback",
            //         new Exception("Called by"));
            return NOERROR;
        }
    }

    AutoPtr<IWeakReference> wr;
    IWeakReferenceSource::Probe(callback)->GetWeakReference((IWeakReference**)&wr);
    mCallbacks->Add(TO_IINTERFACE(wr));
    RemoveCallback(NULL); // remove unused references
    return SendUpdates(callback);
}

void KeyguardUpdateMonitor::SendUpdates(
    /* [in] */ IKeyguardUpdateMonitorCallback* callback)
{
    // Notify listener of the current state
    callback->OnRefreshBatteryInfo(mBatteryStatus);
    callback->OnTimeChanged();
    callback->OnRingerModeChanged(mRingMode);
    callback->OnPhoneStateChanged(mPhoneState);
    callback->OnRefreshCarrierInfo(mTelephonyPlmn, mTelephonySpn);
    callback->OnClockVisibilityChanged();
    callback->OnSimStateChanged(mSimState);
}

ECode KeyguardUpdateMonitor::SendKeyguardVisibilityChanged(
    /* [in] */ Boolean showing)
{
    if (DEBUG) Logger::D(TAG, "sendKeyguardVisibilityChanged(%d)", showing);
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(MSG_KEYGUARD_VISIBILITY_CHANGED, (IMessage**)&message);
    message->SetArg1(showing ? 1 : 0);
    return message->SendToTarget();
}

ECode KeyguardUpdateMonitor::SendKeyguardBouncerChanged(
    /* [in] */ Boolean showingBouncer)
{
    if (DEBUG) Logger::D(TAG, "sendKeyguardBouncerChanged(%d)", showingBouncer);
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(MSG_KEYGUARD_BOUNCER_CHANGED, (IMessage**)&message);
    message->SetArg1(showingBouncer ? 1 : 0);
    return message->SendToTarget();
}

ECode KeyguardUpdateMonitor::ReportClockVisible(
    /* [in] */ Boolean visible)
{
    mClockVisible = visible;
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(MSG_CLOCK_VISIBILITY_CHANGED, (IMessage**)&message);
    return message->SendToTarget();
}

ECode KeyguardUpdateMonitor::GetSimState(
    /* [out] */ IccCardConstantsState* state)
{
    VALIDATE_NOT_NULL(state)

    *state = mSimState;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::ReportSimUnlocked()
{
    AutoPtr<SimArgs> args = new SimArgs(IccCardConstantsState_READY);
    HandleSimStateChange(args);
    return NOERROR;
}

ECode KeyguardUpdateMonitor::ReportEmergencyCallAction(
    /* [in] */ Boolean bypassHandler)
{
    if (!bypassHandler) {
        AutoPtr<IMessage> message;
        mHandler->ObtainMessage(MSG_REPORT_EMERGENCY_CALL_ACTION,(IMessage**)&message);
        message->SendToTarget();
    }
    else {
        HandleReportEmergencyCallAction();
    }
    return NOERROR;
}

ECode KeyguardUpdateMonitor::GetTelephonyPlmn(
    /* [out] */ ICharSequence** cchar)
{
    VALIDATE_NOT_NULL(cchar)

    *cchar = mTelephonyPlmn;
    REFCOUNT_ADD(*cchar)
    return NOERROR;
}

ECode KeyguardUpdateMonitor::GetTelephonySpn(
    /* [out] */ ICharSequence** cchar)
{
    VALIDATE_NOT_NULL(cchar)

    *cchar = mTelephonySpn;
    REFCOUNT_ADD(*cchar)
    return NOERROR;
}

ECode KeyguardUpdateMonitor::IsDeviceProvisioned(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL()

    *result = mDeviceProvisioned;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::GetFailedUnlockAttempts(
    /* [out] */ Int32* attempts)
{
    VALIDATE_NOT_NULL(attempts)

    *attempts = mFailedAttempts;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::ClearFailedUnlockAttempts()
{
    mFailedAttempts = 0;
    mFailedBiometricUnlockAttempts = 0;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::ClearFingerprintRecognized()
{
    return mUserFingerprintRecognized->Clear();
}

ECode KeyguardUpdateMonitor::ReportFailedUnlockAttempt()
{
    mFailedAttempts++;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::IsClockVisible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mClockVisible;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::GetPhoneState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state)

    *state = mPhoneState;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::ReportFailedBiometricUnlockAttempt()
{
    mFailedBiometricUnlockAttempts++;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::GetMaxBiometricUnlockAttemptsReached(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mFailedBiometricUnlockAttempts >= FAILED_BIOMETRIC_UNLOCK_ATTEMPTS_BEFORE_BACKUP;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::IsAlternateUnlockEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mAlternateUnlockEnabled;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::SetAlternateUnlockEnabled(
    /* [in] */ Boolean enabled)
{
    mAlternateUnlockEnabled = enabled;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::IsSimLocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return IsSimLocked(mSimState, result);
}

ECode KeyguardUpdateMonitor::IsSimLocked(
    /* [in] */ IccCardConstantsState state,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = state == IccCardConstantsState_PIN_REQUIRED
        || state == IccCardConstantsState_PUK_REQUIRED
        || state == IccCardConstantsState_PERM_DISABLED;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::IsSimPinSecure(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return IsSimPinSecure(mSimState, result);
}

ECode KeyguardUpdateMonitor::IsSimPinSecure(
    /* [in] */ IccCardConstantsState state,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    IccCardConstantsState simState = state;
    *result = (simState == IccCardConstantsState_PIN_REQUIRED
            || simState == IccCardConstantsState_PUK_REQUIRED
            || simState == IccCardConstantsState_PERM_DISABLED);
    return NOERROR;
}

ECode KeyguardUpdateMonitor::GetCachedDisplayClientState(
    /* [out] */ DisplayClientState** state)
{
    VALIDATE_NOT_NULL(state)

    *state = mDisplayClientState;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::DispatchScreenTurnedOn()
{
    {
        AutoLock syncLock(this);
        mScreenOn = TRUE;
    }
    return mHandler->SendEmptyMessage(MSG_SCREEN_TURNED_ON);
}

ECode KeyguardUpdateMonitor::DispatchScreenTurndOff(
    /* [in] */ Int3 why)
{
    {
        AutoLock syncLock(this);
        mScreenOn = FALSE;
    }
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(MSG_SCREEN_TURNED_OFF, why, 0, (IMessage**)&message);
    return mHandler->SendMessage(message);
}

ECode KeyguardUpdateMonitor::IsScreenOn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mScreenOn;
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
