
#include "elastos/droid/systemui/statusbar/phone/PhoneStatusBarPolicy.h"
#include "../../R.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telecomm.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IAlarmClockInfo;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Bluetooth::CBluetoothAdapterHelper;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothAdapterHelper;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_ABSENT;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_CARD_IO_ERROR;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_PERSO_LOCKED;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_PIN_REQUIRED;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_PUK_REQUIRED;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_READY;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_UNKNOWN;
using Elastos::Droid::Internal::Telephony::IIccCardConstants;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_ICastControllerCallback;
using Elastos::Droid::SystemUI::StatusBar::Policy::ICastControllerCastDevice;
using Elastos::Droid::Telecomm::Telecom::ITelecomManager;
using Elastos::Utility::IIterator;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

PhoneStatusBarPolicy::IntentReceiver::IntentReceiver(
    /* [in] */ PhoneStatusBarPolicy* host)
    : mHost(host)
{}

ECode PhoneStatusBarPolicy::IntentReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IAlarmManager::ACTION_NEXT_ALARM_CLOCK_CHANGED)) {
        mHost->UpdateAlarm();
    }
    else if (action.Equals(IIntent::ACTION_SYNC_STATE_CHANGED)) {
        mHost->UpdateSyncState(intent);
    }
    else if (action.Equals(IBluetoothAdapter::ACTION_STATE_CHANGED) ||
            action.Equals(IBluetoothAdapter::ACTION_CONNECTION_STATE_CHANGED)) {
        mHost->UpdateBluetooth();
    }
    else if (action.Equals(IAudioManager::RINGER_MODE_CHANGED_ACTION)) {
        mHost->UpdateVolumeZen();
    }
    else if (action.Equals(ITelephonyIntents::ACTION_SIM_STATE_CHANGED)) {
        mHost->UpdateSimState(intent);
    }
    else if (action.Equals(ITelecomManager::ACTION_CURRENT_TTY_MODE_CHANGED)) {
        mHost->UpdateTTY(intent);
    }
    else if (action.Equals(IIntent::ACTION_USER_SWITCHED)) {
        mHost->UpdateAlarm();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(PhoneStatusBarPolicy::CastCallback, Object, ICastControllerCallback);
PhoneStatusBarPolicy::CastCallback::CastCallback(
    /* [in] */ PhoneStatusBarPolicy* host)
    : mHost(host)
{}

ECode PhoneStatusBarPolicy::CastCallback::OnCastDevicesChanged()
{
    mHost->UpdateCast();
    return NOERROR;
}

const String PhoneStatusBarPolicy::TAG("PhoneStatusBarPolicy");
const Boolean PhoneStatusBarPolicy::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);
const Boolean PhoneStatusBarPolicy::SHOW_SYNC_ICON = FALSE;
const String PhoneStatusBarPolicy::SLOT_SYNC_ACTIVE("sync_active");
const String PhoneStatusBarPolicy::SLOT_CAST("cast");
const String PhoneStatusBarPolicy::SLOT_BLUETOOTH("bluetooth");
const String PhoneStatusBarPolicy::SLOT_TTY("tty");
const String PhoneStatusBarPolicy::SLOT_ZEN("zen");
const String PhoneStatusBarPolicy::SLOT_VOLUME("volume");
const String PhoneStatusBarPolicy::SLOT_CDMA_ERI("cdma_eri");
const String PhoneStatusBarPolicy::SLOT_ALARM_CLOCK("alarm_clock");
PhoneStatusBarPolicy::PhoneStatusBarPolicy(
    /* [in] */ IContext* context,
    /* [in] */ ICastController* cast)
    : mSimState(IccCardConstantsState_READY)
    , mZenVisible(FALSE)
    , mVolumeVisible(FALSE)
    , mZen(0)
    , mBluetoothEnabled(FALSE)
{
    mIntentReceiver = new IntentReceiver(this);
    mCastCallback = new CastCallback(this);
    CHandler::New((IHandler**)&mHandler);
    mContext = context;
    mCast = cast;

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::STATUS_BAR_SERVICE, (IInterface**)&obj);
    mService = IStatusBarManager::Probe(obj);

    // listen for broadcasts
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IAlarmManager::ACTION_NEXT_ALARM_CLOCK_CHANGED);
    filter->AddAction(IIntent::ACTION_SYNC_STATE_CHANGED);
    filter->AddAction(IAudioManager::RINGER_MODE_CHANGED_ACTION);
    filter->AddAction(IBluetoothAdapter::ACTION_STATE_CHANGED);
    filter->AddAction(IBluetoothAdapter::ACTION_CONNECTION_STATE_CHANGED);
    filter->AddAction(ITelephonyIntents::ACTION_SIM_STATE_CHANGED);
    filter->AddAction(ITelecomManager::ACTION_CURRENT_TTY_MODE_CHANGED);
    filter->AddAction(IIntent::ACTION_USER_SWITCHED);

    AutoPtr<IIntent> i;
    mContext->RegisterReceiver(mIntentReceiver, filter, String(NULL), mHandler, (IIntent**)&i);

    // TTY status
    mService->SetIcon(SLOT_TTY,  R::drawable::stat_sys_tty_mode, 0, String(NULL));
    mService->SetIconVisibility(SLOT_TTY, FALSE);

    // Cdma Roaming Indicator, ERI
    mService->SetIcon(SLOT_CDMA_ERI, R::drawable::stat_sys_roaming_cdma_0, 0, String(NULL));
    mService->SetIconVisibility(SLOT_CDMA_ERI, FALSE);

    // bluetooth status
    UpdateBluetooth();

    // Alarm clock
    mService->SetIcon(SLOT_ALARM_CLOCK, R::drawable::stat_sys_alarm, 0, String(NULL));
    mService->SetIconVisibility(SLOT_ALARM_CLOCK, FALSE);

    // Sync state
    mService->SetIcon(SLOT_SYNC_ACTIVE, R::drawable::stat_sys_sync, 0, String(NULL));
    mService->SetIconVisibility(SLOT_SYNC_ACTIVE, FALSE);
    // "sync_failing" is obsolete: b/1297963

    // zen
    mService->SetIcon(SLOT_ZEN, R::drawable::stat_sys_zen_important, 0, String(NULL));
    mService->SetIconVisibility(SLOT_ZEN, FALSE);

    // volume
    mService->SetIcon(SLOT_VOLUME, R::drawable::stat_sys_ringer_vibrate, 0, String(NULL));
    mService->SetIconVisibility(SLOT_VOLUME, FALSE);
    UpdateVolumeZen();

    // cast
    mService->SetIcon(SLOT_CAST, R::drawable::stat_sys_cast, 0, String(NULL));
    mService->SetIconVisibility(SLOT_CAST, FALSE);
    mCast->AddCallback(mCastCallback);
}

void PhoneStatusBarPolicy::SetZenMode(
    /* [in] */ Int32 zen)
{
    mZen = zen;
    UpdateVolumeZen();
}

void PhoneStatusBarPolicy::UpdateAlarm()
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
    AutoPtr<IAlarmManager> alarmManager = IAlarmManager::Probe(obj);

    AutoPtr<IAlarmClockInfo> info;
    alarmManager->GetNextAlarmClock(IUserHandle::USER_CURRENT, (IAlarmClockInfo**)&info);
    Boolean alarmSet = info != NULL;
    mService->SetIconVisibility(SLOT_ALARM_CLOCK, alarmSet);
}

void PhoneStatusBarPolicy::UpdateSyncState(
    /* [in] */ IIntent* intent)
{
    if (!SHOW_SYNC_ICON) return;
    Boolean isActive = FALSE;
    intent->GetBooleanExtra(String("active"), FALSE, &isActive);
    mService->SetIconVisibility(SLOT_SYNC_ACTIVE, isActive);
}

void PhoneStatusBarPolicy::UpdateSimState(
    /* [in] */ IIntent* intent)
{
    String stateExtra;
    intent->GetStringExtra(IIccCardConstants::INTENT_KEY_ICC_STATE, &stateExtra);
    if (IIccCardConstants::INTENT_VALUE_ICC_ABSENT.Equals(stateExtra)) {
        mSimState = IccCardConstantsState_ABSENT;
    }
    else if (IIccCardConstants::INTENT_VALUE_ICC_CARD_IO_ERROR.Equals(stateExtra)) {
        mSimState = IccCardConstantsState_CARD_IO_ERROR;
    }
    else if (IIccCardConstants::INTENT_VALUE_ICC_READY.Equals(stateExtra)) {
        mSimState = IccCardConstantsState_READY;
    }
    else if (IIccCardConstants::INTENT_VALUE_ICC_LOCKED.Equals(stateExtra)) {
        String lockedReason;
        intent->GetStringExtra(IIccCardConstants::INTENT_KEY_LOCKED_REASON, &lockedReason);
        if (IIccCardConstants::INTENT_VALUE_LOCKED_ON_PIN.Equals(lockedReason)) {
            mSimState = IccCardConstantsState_PIN_REQUIRED;
        }
        else if (IIccCardConstants::INTENT_VALUE_LOCKED_ON_PUK.Equals(lockedReason)) {
            mSimState = IccCardConstantsState_PUK_REQUIRED;
        }
        else {
            mSimState = IccCardConstantsState_PERSO_LOCKED;
        }
    }
    else {
        mSimState = IccCardConstantsState_UNKNOWN;
    }
}

void PhoneStatusBarPolicy::UpdateVolumeZen()
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(obj);

    Boolean zenVisible = FALSE;
    Int32 zenIconId = 0;
    String zenDescription;

    Boolean volumeVisible = FALSE;
    Int32 volumeIconId = 0;
    String volumeDescription;

    if (mZen == ISettingsGlobal::ZEN_MODE_NO_INTERRUPTIONS) {
        zenVisible = TRUE;
        zenIconId = R::drawable::stat_sys_zen_none;
        mContext->GetString(R::string::zen_no_interruptions, &zenDescription);
    }
    else if (mZen == ISettingsGlobal::ZEN_MODE_IMPORTANT_INTERRUPTIONS) {
        zenVisible = TRUE;
        zenIconId = R::drawable::stat_sys_zen_important;
        mContext->GetString(R::string::zen_important_interruptions, &zenDescription);
    }

    Int32 mode = 0;
    audioManager->GetRingerMode(&mode);
    if (mZen != ISettingsGlobal::ZEN_MODE_NO_INTERRUPTIONS &&
            mode == IAudioManager::RINGER_MODE_VIBRATE) {
        volumeVisible = TRUE;
        volumeIconId = R::drawable::stat_sys_ringer_vibrate;
        mContext->GetString(R::string::accessibility_ringer_vibrate, &volumeDescription);
    }

    if (zenVisible) {
        mService->SetIcon(SLOT_ZEN, zenIconId, 0, zenDescription);
    }
    if (zenVisible != mZenVisible) {
        mService->SetIconVisibility(SLOT_ZEN, zenVisible);
        mZenVisible = zenVisible;
    }

    if (volumeVisible) {
        mService->SetIcon(SLOT_VOLUME, volumeIconId, 0, volumeDescription);
    }
    if (volumeVisible != mVolumeVisible) {
        mService->SetIconVisibility(SLOT_VOLUME, volumeVisible);
        mVolumeVisible = volumeVisible;
    }
}

void PhoneStatusBarPolicy::UpdateBluetooth()
{
    AutoPtr<IBluetoothAdapter> adapter;
    AutoPtr<IBluetoothAdapterHelper> helper;
    CBluetoothAdapterHelper::AcquireSingleton((IBluetoothAdapterHelper**)&helper);
    helper->GetDefaultAdapter((IBluetoothAdapter**)&adapter);
    Int32 iconId = R::drawable::stat_sys_data_bluetooth;
    String contentDescription;
    mContext->GetString(R::string::accessibility_bluetooth_disconnected, &contentDescription);
    if (adapter != NULL) {
        Int32 v = 0;
        adapter->GetState(&v);
        mBluetoothEnabled = (v == IBluetoothAdapter::STATE_ON);
        if ((adapter->GetConnectionState(&v), v) == IBluetoothAdapter::STATE_CONNECTED) {
            iconId = R::drawable::stat_sys_data_bluetooth_connected;
            mContext->GetString(R::string::accessibility_bluetooth_connected, &contentDescription);
        }
    }
    else {
        mBluetoothEnabled = FALSE;
    }

    mService->SetIcon(SLOT_BLUETOOTH, iconId, 0, contentDescription);
    mService->SetIconVisibility(SLOT_BLUETOOTH, mBluetoothEnabled);
}

void PhoneStatusBarPolicy::UpdateTTY(
    /* [in] */ IIntent* intent)
{
    Int32 currentTtyMode = 0;
    intent->GetInt32Extra(ITelecomManager::EXTRA_CURRENT_TTY_MODE,
            ITelecomManager::TTY_MODE_OFF, &currentTtyMode);
    Boolean enabled = currentTtyMode != ITelecomManager::TTY_MODE_OFF;

    if (DEBUG) Logger::V(TAG, "UpdateTTY: enabled: %d", enabled);

    if (enabled) {
        // TTY is on
        if (DEBUG) Logger::V(TAG, "UpdateTTY: set TTY on");
        String v;
        mContext->GetString(R::string::accessibility_tty_enabled, &v);
        mService->SetIcon(SLOT_TTY, R::drawable::stat_sys_tty_mode, 0, v);
        mService->SetIconVisibility(SLOT_TTY, TRUE);
    }
    else {
        // TTY is off
        if (DEBUG) Logger::V(TAG, "UpdateTTY: set TTY off");
        mService->SetIconVisibility(SLOT_TTY, FALSE);
    }
}

void PhoneStatusBarPolicy::UpdateCast()
{
    Boolean isCasting = FALSE;
    AutoPtr<ISet> values;
    mCast->GetCastDevices((ISet**)&values);
    AutoPtr<IIterator> ator;
    values->GetIterator((IIterator**)&ator);

    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);

        AutoPtr<ICastControllerCastDevice> device = ICastControllerCastDevice::Probe(obj);
        Int32 state = 0;
        device->GetState(&state);
        if (state == ICastControllerCastDevice::STATE_CONNECTING
                || state == ICastControllerCastDevice::STATE_CONNECTED) {
            isCasting = TRUE;
            break;
        }
    }

    if (DEBUG) Logger::V(TAG, "updateCast: isCasting: %d", isCasting);
    if (isCasting) {
        String v;
        mContext->GetString(R::string::accessibility_casting, &v);
        mService->SetIcon(SLOT_CAST, R::drawable::stat_sys_cast, 0, v);
    }
    mService->SetIconVisibility(SLOT_CAST, isCasting);
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
