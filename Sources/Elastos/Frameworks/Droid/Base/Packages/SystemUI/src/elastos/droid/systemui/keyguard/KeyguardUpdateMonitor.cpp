
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitor.h"
#include "elastos/droid/systemui/keyguard/CKeyguardUpdateMonitorContentObserver.h"
#include "elastos/droid/systemui/keyguard/CKeyguardUpdateMonitorBroadcastReceiver1.h"
#include "elastos/droid/systemui/keyguard/CKeyguardUpdateMonitorBroadcastReceiver2.h"
#include "elastos/droid/systemui/keyguard/CKeyguardUpdateMonitorUserSwitchObserver.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/provider/Settings.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/droid/app/ActivityManagerNative.h>
#include "../R.h"
#include <elastos/droid/R.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IIUserSwitchObserver;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::EIID_IIUserSwitchObserver;;
using Elastos::Droid::App::Trust::EIID_ITrustListener;
using Elastos::Droid::App::Trust::ITrustManager;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IBatteryManager;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Service::Fingerprint::CFingerprintUtils;
using Elastos::Droid::Service::Fingerprint::IFingerprintManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::INativeTextHelper;
using Elastos::Droid::Utility::CNativeTextHelper;
using Elastos::Droid::Utility::CSparseBooleanArray;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::CSubscriptionManager;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IIccCardConstants;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_UNKNOWN;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_ABSENT;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_PIN_REQUIRED;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_PUK_REQUIRED;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_PERSO_LOCKED;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_READY;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_NOT_READY;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_PERM_DISABLED;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_CARD_IO_ERROR;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Service::Fingerprint::IFingerprintUtils;
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

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


//========================================================================
// KeyguardUpdateMonitor::UserSwitchObserver
//========================================================================

CAR_INTERFACE_IMPL_2(KeyguardUpdateMonitor::UserSwitchObserver, Object, IIUserSwitchObserver, IBinder)

ECode KeyguardUpdateMonitor::UserSwitchObserver::constructor(
    /* [in] */ IKeyguardUpdateMonitor* monitor)
{
    mHost = (KeyguardUpdateMonitor*)monitor;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::UserSwitchObserver::OnUserSwitching(
    /* [in] */ Int32 newUserId,
    /* [in] */ IIRemoteCallback* reply)
{
    Boolean bval;
    AutoPtr<IMessage> m;
    mHost->mHandler->ObtainMessage(KeyguardUpdateMonitor::MSG_USER_SWITCHING, newUserId, 0, reply, (IMessage**)&m);
    mHost->mHandler->SendMessage(m, &bval);
    mHost->mSwitchingUser = TRUE;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::UserSwitchObserver::OnUserSwitchComplete(
    /* [in] */ Int32 newUserId)
{
    Boolean bval;
    AutoPtr<IMessage> m;
    mHost->mHandler->ObtainMessage(KeyguardUpdateMonitor::MSG_USER_SWITCH_COMPLETE, newUserId, 0, (IMessage**)&m);
    mHost->mHandler->SendMessage(m, &bval);
    mHost->mSwitchingUser = FALSE;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::UserSwitchObserver::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

//========================================================================
// KeyguardUpdateMonitor::DisplayClientState
//========================================================================
CAR_INTERFACE_IMPL(KeyguardUpdateMonitor::DisplayClientState, Object, IKeyguardUpdateMonitorDisplayClientState)

KeyguardUpdateMonitor::DisplayClientState::DisplayClientState()
    : mClientGeneration(0)
    , mClearing(FALSE)
    , mPlaybackState(0)
    , mPlaybackEventTime(0)
{}

//========================================================================
// KeyguardUpdateMonitor::BatteryStatus
//========================================================================
CAR_INTERFACE_IMPL(KeyguardUpdateMonitor::BatteryStatus, Object, IKeyguardUpdateMonitorBatteryStatus)

ECode KeyguardUpdateMonitor::BatteryStatus::SetStatus(
    /* [in] */ Int32 value)
{
    mStatus = value;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::BatteryStatus::GetStatus(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mStatus;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::BatteryStatus::SetLevel(
    /* [in] */ Int32 value)
{
    mLevel = value;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::BatteryStatus::GetLevel(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mLevel;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::BatteryStatus::SetPlugged(
    /* [in] */ Int32 value)
{
    mPlugged = value;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::BatteryStatus::GetPlugged(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mPlugged;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::BatteryStatus::SetHealth(
    /* [in] */ Int32 value)
{
    mHealth = value;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::BatteryStatus::GetHealth(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mHealth;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::BatteryStatus::IsPluggedIn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mPlugged == IBatteryManager::BATTERY_PLUGGED_AC
            || mPlugged == IBatteryManager::BATTERY_PLUGGED_USB
            || mPlugged == IBatteryManager::BATTERY_PLUGGED_WIRELESS;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::BatteryStatus::IsCharged(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mStatus == IBatteryManager::BATTERY_STATUS_FULL || mLevel >= 100;
    return NOERROR;
}

ECode KeyguardUpdateMonitor::BatteryStatus::IsBatteryLow(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mLevel < LOW_BATTERY_THRESHOLD;
    return NOERROR;
}

//========================================================================
// KeyguardUpdateMonitor::MyBroadcastReceiver
//========================================================================
ECode KeyguardUpdateMonitor::MyBroadcastReceiver::constructor(
    /* [in] */ IKeyguardUpdateMonitor* host)
{
    mHost = (KeyguardUpdateMonitor*)host;
    return BroadcastReceiver::constructor();
}

ECode KeyguardUpdateMonitor::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Boolean bval;
    String action;
    intent->GetAction(&action);
    if (DEBUG) Logger::D(TAG, "received broadcast %s", action.string());

    if (IIntent::ACTION_TIME_TICK.Equals(action)
            || IIntent::ACTION_TIME_CHANGED.Equals(action)
            || IIntent::ACTION_TIMEZONE_CHANGED.Equals(action)) {
        mHost->mHandler->SendEmptyMessage(MSG_TIME_UPDATE, &bval);
    }
    else if (ITelephonyIntents::SPN_STRINGS_UPDATED_ACTION.Equals(action)) {
        mHost->mTelephonyPlmn = mHost->GetTelephonyPlmnFrom(intent);
        mHost->mTelephonySpn = mHost->GetTelephonySpnFrom(intent);
        mHost->mHandler->SendEmptyMessage(MSG_CARRIER_INFO_UPDATE, &bval);
    }
    else if (IIntent::ACTION_BATTERY_CHANGED.Equals(action)) {
        Int32 status, plugged, level, health;
        intent->GetInt32Extra(IBatteryManager::EXTRA_STATUS, IBatteryManager::BATTERY_STATUS_UNKNOWN, &status);
        intent->GetInt32Extra(IBatteryManager::EXTRA_PLUGGED, 0, &plugged);
        intent->GetInt32Extra(IBatteryManager::EXTRA_LEVEL, 0, &level);
        intent->GetInt32Extra(IBatteryManager::EXTRA_HEALTH, IBatteryManager::BATTERY_HEALTH_UNKNOWN, &health);
        AutoPtr<BatteryStatus> bsStatus = new BatteryStatus(status, level, plugged, health);
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_BATTERY_UPDATE,
            (IKeyguardUpdateMonitorBatteryStatus*)bsStatus.Get(), (IMessage**)&msg);
        mHost->mHandler->SendMessage(msg, &bval);
    }
    else if (ITelephonyIntents::ACTION_SIM_STATE_CHANGED.Equals(action)) {
        // if (DEBUG_SIM_STATES) {
        //     Log.v(TAG, "action " + action + " state" +
        //         intent.getStringExtra(IccCardConstants.INTENT_KEY_ICC_STATE));
        // }
        AutoPtr<SimArgs> args;
        SimArgs::FromIntent(intent, (SimArgs**)&args);
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_SIM_STATE_CHANGE, (IObject*)args.Get(), (IMessage**)&msg);
        mHost->mHandler->SendMessage(msg, &bval);
    }
    else if (IAudioManager::RINGER_MODE_CHANGED_ACTION.Equals(action)) {
        Int32 extra;
        intent->GetInt32Extra(IAudioManager::EXTRA_RINGER_MODE, -1, &extra);
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_RINGER_MODE_CHANGED, extra, 0, (IMessage**)&msg);
        mHost->mHandler->SendMessage(msg, &bval);
    }
    else if (ITelephonyManager::ACTION_PHONE_STATE_CHANGED.Equals(action)) {
        String state;
        intent->GetStringExtra(ITelephonyManager::EXTRA_STATE, &state);
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_PHONE_STATE_CHANGED, CoreUtils::Convert(state), (IMessage**)&msg);
        mHost->mHandler->SendMessage(msg, &bval);
    }
    else if (IIntent::ACTION_USER_REMOVED.Equals(action)) {
        Int32 extra;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, 0, &extra);
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_USER_REMOVED, extra, 0, (IMessage**)&msg);
        mHost->mHandler->SendMessage(msg, &bval);
    }
    else if (IIntent::ACTION_BOOT_COMPLETED.Equals(action)) {
        mHost->DispatchBootCompleted();
    }
    return NOERROR;
}

//========================================================================
// KeyguardUpdateMonitor::MyBroadcastReceiver2
//========================================================================

ECode KeyguardUpdateMonitor::MyBroadcastReceiver2::constructor(
    /* [in] */ IKeyguardUpdateMonitor* host)
{
    mHost = (KeyguardUpdateMonitor*)host;
    return BroadcastReceiver::constructor();
}

ECode KeyguardUpdateMonitor::MyBroadcastReceiver2::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Boolean bval;
    String action;
    intent->GetAction(&action);
    if (IAlarmManager::ACTION_NEXT_ALARM_CLOCK_CHANGED.Equals(action)) {
        mHost->mHandler->SendEmptyMessage(MSG_TIME_UPDATE, &bval);
    }
    else if (IIntent::ACTION_USER_INFO_CHANGED.Equals(action)) {
        Int32 id;
        GetSendingUserId(&id);
        Int32 extra;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, id, &extra);
        AutoPtr<IMessage> massage;
        mHost->mHandler->ObtainMessage(MSG_USER_INFO_CHANGED, extra, 0, (IMessage**)&massage);
        mHost->mHandler->SendMessage(massage, &bval);
    }
    else if (ACTION_FACE_UNLOCK_STARTED.Equals(action)) {
        Int32 id;
        GetSendingUserId(&id);
        AutoPtr<IMessage> massage;
        mHost->mHandler->ObtainMessage(KeyguardUpdateMonitor::MSG_FACE_UNLOCK_STATE_CHANGED,
            1, id, (IMessage**)&massage);
        mHost->mHandler->SendMessage(massage, &bval);
    }
    else if (ACTION_FACE_UNLOCK_STOPPED.Equals(action)) {
        Int32 id;
        GetSendingUserId(&id);
        AutoPtr<IMessage> massage;
        mHost->mHandler->ObtainMessage(KeyguardUpdateMonitor::MSG_FACE_UNLOCK_STATE_CHANGED,
            0, id, (IMessage**)&massage);
        mHost->mHandler->SendMessage(massage, &bval);
    }
    else if (IDevicePolicyManager::ACTION_DEVICE_POLICY_MANAGER_STATE_CHANGED
            .Equals(action)) {
        mHost->mHandler->SendEmptyMessage(MSG_DPM_STATE_CHANGED, &bval);
    }
    return NOERROR;
}

//========================================================================
// KeyguardUpdateMonitor::MyContentObserver
//========================================================================

ECode KeyguardUpdateMonitor::MyContentObserver::constructor(
    /* [in] */ IKeyguardUpdateMonitor* host)
{
    mHost = (KeyguardUpdateMonitor*)host;
    return ContentObserver::constructor();
}

ECode KeyguardUpdateMonitor::MyContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    ContentObserver::OnChange(selfChange);
    mHost->mDeviceProvisioned = mHost->IsDeviceProvisionedInSettingsDb();
    if (mHost->mDeviceProvisioned) {
        Boolean bval;
        mHost->mHandler->SendEmptyMessage(MSG_DEVICE_PROVISIONED, &bval);
    }
    if (DEBUG) Logger::D(TAG, "DEVICE_PROVISIONED state = %d", mHost->mDeviceProvisioned);
    return NOERROR;
}

//========================================================================
// KeyguardUpdateMonitor::MyHandler
//========================================================================
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
        case KeyguardUpdateMonitor::MSG_TIME_UPDATE:
            mHost->HandleTimeUpdate();
            break;
        case KeyguardUpdateMonitor::MSG_BATTERY_UPDATE:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->HandleBatteryUpdate(IKeyguardUpdateMonitorBatteryStatus::Probe(obj));
            break;
        }
        case KeyguardUpdateMonitor::MSG_CARRIER_INFO_UPDATE:
            mHost->HandleCarrierInfoUpdate();
            break;
        case KeyguardUpdateMonitor::MSG_SIM_STATE_CHANGE:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->HandleSimStateChange((SimArgs*)IObject::Probe(obj));
            break;
        }
        case KeyguardUpdateMonitor::MSG_RINGER_MODE_CHANGED:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleRingerModeChange(arg1);
            break;
        }
        case KeyguardUpdateMonitor::MSG_PHONE_STATE_CHANGED:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<ICharSequence> cchar = ICharSequence::Probe(obj);
            String str;
            cchar->ToString(&str);
            mHost->HandlePhoneStateChanged(str);
            break;
        }
        case KeyguardUpdateMonitor::MSG_CLOCK_VISIBILITY_CHANGED:
            mHost->HandleClockVisibilityChanged();
            break;
        case KeyguardUpdateMonitor::MSG_DEVICE_PROVISIONED:
            mHost->HandleDeviceProvisioned();
            break;
        case KeyguardUpdateMonitor::MSG_DPM_STATE_CHANGED:
            mHost->HandleDevicePolicyManagerStateChanged();
            break;
        case KeyguardUpdateMonitor::MSG_USER_SWITCHING:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->HandleUserSwitching(arg1, IIRemoteCallback::Probe(obj));
            break;
        }
        case KeyguardUpdateMonitor::MSG_USER_SWITCH_COMPLETE:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleUserSwitchComplete(arg1);
            break;
        }
        case KeyguardUpdateMonitor::MSG_USER_REMOVED:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleUserRemoved(arg1);
            break;
        }
        case KeyguardUpdateMonitor::MSG_KEYGUARD_VISIBILITY_CHANGED:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleKeyguardVisibilityChanged(arg1);
            break;
        }
        case KeyguardUpdateMonitor::MSG_KEYGUARD_BOUNCER_CHANGED:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleKeyguardBouncerChanged(arg1);
            break;
        }
        case KeyguardUpdateMonitor::MSG_BOOT_COMPLETED:
            mHost->HandleBootCompleted();
            break;
        case KeyguardUpdateMonitor::MSG_USER_INFO_CHANGED:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleUserInfoChanged(arg1);
            break;
        }
        case KeyguardUpdateMonitor::MSG_REPORT_EMERGENCY_CALL_ACTION:
            mHost->HandleReportEmergencyCallAction();
            break;
        case KeyguardUpdateMonitor::MSG_SCREEN_TURNED_OFF:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleScreenTurnedOff(arg1);
            break;
        }
        case KeyguardUpdateMonitor::MSG_SCREEN_TURNED_ON:
            mHost->HandleScreenTurnedOn();
            break;
        case KeyguardUpdateMonitor::MSG_FINGERPRINT_ACQUIRED:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleFingerprintAcquired(arg1);
            break;
        }
        case KeyguardUpdateMonitor::MSG_FINGERPRINT_PROCESSED:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleFingerprintProcessed(arg1);
            break;
        }
        case KeyguardUpdateMonitor::MSG_FACE_UNLOCK_STATE_CHANGED:
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

//========================================================================
// KeyguardUpdateMonitor::SimArgs
//========================================================================
KeyguardUpdateMonitor::SimArgs::SimArgs(
    /* [in] */ IccCardConstantsState state)
    : mSimState(state)
{}

ECode KeyguardUpdateMonitor::SimArgs::FromIntent(
    /* [in] */ IIntent* intent,
    /* [out] */ SimArgs** args)
{
    VALIDATE_NOT_NULL(args)
    *args = NULL;

    IccCardConstantsState state;
    String action;
    intent->GetAction(&action);
    if (!ITelephonyIntents::ACTION_SIM_STATE_CHANGED.Equals(action)) {
        Logger::E(TAG, "only handles intent ACTION_SIM_STATE_CHANGED");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String stateExtra;
    intent->GetStringExtra(IIccCardConstants::INTENT_KEY_ICC_STATE, &stateExtra);
    if (IIccCardConstants::INTENT_VALUE_ICC_ABSENT.Equals(stateExtra)) {
        String absentReason;
        intent->GetStringExtra(IIccCardConstants::INTENT_KEY_LOCKED_REASON, &absentReason);

        if (IIccCardConstants::INTENT_VALUE_ABSENT_ON_PERM_DISABLED.Equals(absentReason)) {
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
    else if (IIccCardConstants::INTENT_VALUE_LOCKED_PERSO.Equals(stateExtra)) {
        state = IccCardConstantsState_PERSO_LOCKED;
    }
    else if (IIccCardConstants::INTENT_VALUE_ICC_LOADED.Equals(stateExtra)
                || IIccCardConstants::INTENT_VALUE_ICC_IMSI.Equals(stateExtra)) {
        // This is required because telephony doesn't return to "READY" after
        // these state transitions. See bug 7197471.
        state = IccCardConstantsState_READY;
    }
    else {
        state = IccCardConstantsState_UNKNOWN;
    }
    AutoPtr<SimArgs> result  = new SimArgs(state);
    *args = result;
    REFCOUNT_ADD(*args);
    return NOERROR;
}

static const String StateToString(
    /* [in] */ IccCardConstantsState state)
{
    switch (state) {
        case IccCardConstantsState_ABSENT:
            return String("ABSENT");
        case IccCardConstantsState_PIN_REQUIRED:
            return String("PIN_REQUIRED");
        case IccCardConstantsState_PUK_REQUIRED:
            return String("PUK_REQUIRED");
        case IccCardConstantsState_PERSO_LOCKED:
            return String("PERSO_LOCKED");
        case IccCardConstantsState_READY:
            return String("READY");
        case IccCardConstantsState_NOT_READY:
            return String("NOT_READY");
        case IccCardConstantsState_PERM_DISABLED:
            return String("PERM_DISABLED");
        case IccCardConstantsState_CARD_IO_ERROR:
            return String("CARD_IO_ERROR");
        case IccCardConstantsState_UNKNOWN:
        default:
            return String("UNKNOWN");
    }
    return String("UNKNOWN");
}

ECode KeyguardUpdateMonitor::SimArgs::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = StateToString(mSimState);
    return NOERROR;
}

//========================================================================
// KeyguardUpdateMonitor::MyFingerprintManagerReceiver
//========================================================================
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

//========================================================================
// KeyguardUpdateMonitor::TrustListener
//========================================================================
CAR_INTERFACE_IMPL(KeyguardUpdateMonitor::TrustListener, Object, ITrustListener)

ECode KeyguardUpdateMonitor::TrustListener::OnTrustChanged(
    /* [in] */ Boolean enabled,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean initiatedByUser)
{
    mHost->mUserHasTrust->Put(userId, enabled);

    Int32 size;
    mHost->mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mHost->mCallbacks->Get(i, (IInterface**)&obj);
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

ECode KeyguardUpdateMonitor::TrustListener::OnTrustManagedChanged(
    /* [in] */ Boolean managed,
    /* [in] */ Int32 userId)
{
    mHost->mUserTrustIsManaged->Put(userId, managed);

    Int32 size;
    mHost->mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mHost->mCallbacks->Get(i, (IInterface**)&obj);
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

//========================================================================
// KeyguardUpdateMonitor::KeyguardUpdateMonitor
//========================================================================
const String KeyguardUpdateMonitor::TAG("KeyguardUpdateMonitor");
const Boolean KeyguardUpdateMonitor::DEBUG = TRUE;
const Boolean KeyguardUpdateMonitor::DEBUG_SIM_STATES = DEBUG || FALSE;
const Int32 KeyguardUpdateMonitor::FAILED_BIOMETRIC_UNLOCK_ATTEMPTS_BEFORE_BACKUP = 3;
const Int32 KeyguardUpdateMonitor::LOW_BATTERY_THRESHOLD = 20;

const String KeyguardUpdateMonitor::ACTION_FACE_UNLOCK_STARTED("com.android.facelock.FACE_UNLOCK_STARTED");
const String KeyguardUpdateMonitor::ACTION_FACE_UNLOCK_STOPPED("com.android.facelock.FACE_UNLOCK_STOPPED");

const Int64 KeyguardUpdateMonitor::INVALID_SUBID = ISubscriptionManager::INVALID_SUB_ID;

AutoPtr<IKeyguardUpdateMonitor> KeyguardUpdateMonitor::sInstance;

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
    /* [in] */ Int32 fingerprintId)
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
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
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
            Boolean disabledBySimPin;
            IsSimPinSecure(&disabledBySimPin);

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

    Boolean bval, bval2;
    *result = (!IsTrustDisabled(userId) && (mUserHasTrust->Get(userId, &bval), bval))
        || (mUserFingerprintRecognized->Get(userId, &bval2), bval2);
    return NOERROR;
}

ECode KeyguardUpdateMonitor::GetUserTrustIsManaged(
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean bval;
    mUserTrustIsManaged->Get(userId, &bval);
    *result = bval && !IsTrustDisabled(userId);
    return NOERROR;
}

AutoPtr<IKeyguardUpdateMonitor> KeyguardUpdateMonitor::GetInstance(
    /* [in] */ IContext* context)
{
    if (sInstance == NULL) {
        AutoPtr<KeyguardUpdateMonitor> monitor = new KeyguardUpdateMonitor();
        monitor->constructor(context);
        sInstance = (IKeyguardUpdateMonitor*)monitor.Get();
    }
    return sInstance;
}

ECode KeyguardUpdateMonitor::HandleScreenTurnedOn()
{
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

void KeyguardUpdateMonitor::HandleUserInfoChanged(
    /* [in] */ Int32 userId)
{
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
            cb->OnUserInfoChanged(userId);
        }
    }
}

ECode KeyguardUpdateMonitor::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;

    mHandler = new MyHandler(this);
    mDisplayClientState = new DisplayClientState();

    CArrayList::New((IList**)&mCallbacks);
    CSparseBooleanArray::New((ISparseBooleanArray**)&mUserHasTrust);
    CSparseBooleanArray::New((ISparseBooleanArray**)&mUserTrustIsManaged);
    CSparseBooleanArray::New((ISparseBooleanArray**)&mUserFingerprintRecognized);
    CSparseBooleanArray::New((ISparseBooleanArray**)&mUserFaceUnlockRunning);

    CKeyguardUpdateMonitorBroadcastReceiver1::New(this, (IBroadcastReceiver**)&mBroadcastReceiver);
    CKeyguardUpdateMonitorBroadcastReceiver2::New(this, (IBroadcastReceiver**)&mBroadcastAllReceiver);

    mDeviceProvisioned = IsDeviceProvisionedInSettingsDb();
    // Since device can't be un-provisioned, we only need to register a content observer
    // to update mDeviceProvisioned when we are...
    if (!mDeviceProvisioned) {
        WatchForDeviceProvisioning();
    }

    // Take a guess at initial SIM state, battery status and PLMN until we get an update
    mSimState = IccCardConstantsState_NOT_READY;
    mTelephonyPlmn = GetDefaultPlmn();

    // Take a guess at initial SIM state, battery status and PLMN until we get an update
    mBatteryStatus = new BatteryStatus(IBatteryManager::BATTERY_STATUS_UNKNOWN, 100, 0, 0);

    AutoPtr<IIntent> stickyIntent;
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
    context->RegisterReceiver(mBroadcastReceiver, filter, (IIntent**)&stickyIntent);

    AutoPtr<IIntentFilter> bootCompleteFilter;
    CIntentFilter::New((IIntentFilter**)&bootCompleteFilter);
    bootCompleteFilter->SetPriority(IIntentFilter::SYSTEM_HIGH_PRIORITY);
    bootCompleteFilter->AddAction(IIntent::ACTION_BOOT_COMPLETED);
    stickyIntent = NULL;
    context->RegisterReceiver(mBroadcastReceiver, bootCompleteFilter, (IIntent**)&stickyIntent);

    AutoPtr<IIntentFilter> allUserFilter;
    CIntentFilter::New((IIntentFilter**)&allUserFilter);
    allUserFilter->AddAction(IIntent::ACTION_USER_INFO_CHANGED);
    allUserFilter->AddAction(IAlarmManager::ACTION_NEXT_ALARM_CLOCK_CHANGED);
    allUserFilter->AddAction(ACTION_FACE_UNLOCK_STARTED);
    allUserFilter->AddAction(ACTION_FACE_UNLOCK_STOPPED);
    allUserFilter->AddAction(IDevicePolicyManager::ACTION_DEVICE_POLICY_MANAGER_STATE_CHANGED);
    stickyIntent = NULL;
    context->RegisterReceiverAsUser(mBroadcastAllReceiver, UserHandle::ALL, allUserFilter,
            String(NULL), NULL, (IIntent**)&stickyIntent);

    //try {
    AutoPtr<IIUserSwitchObserver> observer;
    CKeyguardUpdateMonitorUserSwitchObserver::New(this, (IIUserSwitchObserver**)&observer);
    AutoPtr<IIActivityManager> manager = ActivityManagerNative::GetDefault();
    manager->RegisterUserSwitchObserver(observer);
    // } catch (RemoteException e) {
    //     // TODO Auto-generated catch block
    //     e.printStackTrace();
    // }

    AutoPtr<ITrustListener> listener = new TrustListener(this);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::TRUST_SERVICE, (IInterface**)&obj);
    AutoPtr<ITrustManager> trustManager = ITrustManager::Probe(obj);
    trustManager->RegisterTrustListener(listener);

    mFingerprintManagerReceiver = new MyFingerprintManagerReceiver(this);
    obj = NULL;
    context->GetSystemService(IContext::FINGERPRINT_SERVICE, (IInterface**)&obj);
    AutoPtr<IFingerprintManager> fpm = IFingerprintManager::Probe(obj);
    return fpm->StartListening(mFingerprintManagerReceiver);
}

Boolean KeyguardUpdateMonitor::IsDeviceProvisionedInSettingsDb()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 num;
    Elastos::Droid::Provider::Settings::Global::GetInt32(cr, ISettingsGlobal::DEVICE_PROVISIONED, 0, &num);
    return num != 0;
}

void KeyguardUpdateMonitor::WatchForDeviceProvisioning()
{
    mDeviceProvisionedObserver = NULL;
    CKeyguardUpdateMonitorContentObserver::New(this, (IContentObserver**)&mDeviceProvisionedObserver);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IUri> value;
    Elastos::Droid::Provider::Settings::Global::GetUriFor(ISettingsGlobal::DEVICE_PROVISIONED, (IUri**)&value);
    cr->RegisterContentObserver(value, FALSE, mDeviceProvisionedObserver);

    // prevent a race condition between where we check the flag and where we register the
    // observer by grabbing the value once again...
    Boolean provisioned = IsDeviceProvisionedInSettingsDb();
    if (provisioned != mDeviceProvisioned) {
        mDeviceProvisioned = provisioned;
        if (mDeviceProvisioned) {
            Boolean bval;
            mHandler->SendEmptyMessage(MSG_DEVICE_PROVISIONED, &bval);
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
    Boolean bval;
    return mHandler->SendEmptyMessage(MSG_BOOT_COMPLETED, &bval);
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
    /* [in] */ IKeyguardUpdateMonitorBatteryStatus* status)
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
    // if (DEBUG) Log.d(TAG, "handleCarrierInfoUpdate: plmn = " + mPlmn
    //     + ", spn = " + mSpn + ", subId = " + subId);

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
}

void KeyguardUpdateMonitor::HandleSimStateChange(
    /* [in] */ SimArgs* simArgs)
{
    IccCardConstantsState state = simArgs->mSimState;

    if (DEBUG) {
        Logger::D(TAG, "handleSimStateChange: intentValue = %s "
            "state resolved to %s", TO_CSTR(simArgs), StateToString(state).string());
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
    /* [in] */ IKeyguardUpdateMonitorBatteryStatus* oldObj,
    /* [in] */ IKeyguardUpdateMonitorBatteryStatus* currentObj)
{
    BatteryStatus* old = (BatteryStatus*)oldObj;
    BatteryStatus* current = (BatteryStatus*)currentObj;
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
        if (callback == IKeyguardUpdateMonitorCallback::Probe(callbackObj)) {
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
        if (callback == IKeyguardUpdateMonitorCallback::Probe(callbackObj)) {
            if (DEBUG)  {
                Logger::E(TAG, "Object tried to add another callback");
            }
            return NOERROR;
        }
    }

    AutoPtr<IWeakReference> wr;
    IWeakReferenceSource::Probe(callback)->GetWeakReference((IWeakReference**)&wr);
    mCallbacks->Add(TO_IINTERFACE(wr));
    RemoveCallback(NULL); // remove unused references
    return SendUpdates(callback);
}

ECode KeyguardUpdateMonitor::SendUpdates(
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
    return NOERROR;
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
    VALIDATE_NOT_NULL(result)

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
    /* [out] */ IKeyguardUpdateMonitorDisplayClientState** state)
{
    VALIDATE_NOT_NULL(state)
    *state = (IKeyguardUpdateMonitorDisplayClientState*)mDisplayClientState.Get();
    REFCOUNT_ADD(*state)
    return NOERROR;
}

ECode KeyguardUpdateMonitor::DispatchScreenTurnedOn()
{
    {
        AutoLock syncLock(this);
        mScreenOn = TRUE;
    }
    Boolean bval;
    return mHandler->SendEmptyMessage(MSG_SCREEN_TURNED_ON, &bval);
}

ECode KeyguardUpdateMonitor::DispatchScreenTurndOff(
    /* [in] */ Int32 why)
{
    {
        AutoLock syncLock(this);
        mScreenOn = FALSE;
    }
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(MSG_SCREEN_TURNED_OFF, why, 0, (IMessage**)&message);
    Boolean bval;
    return mHandler->SendMessage(message, &bval);
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
