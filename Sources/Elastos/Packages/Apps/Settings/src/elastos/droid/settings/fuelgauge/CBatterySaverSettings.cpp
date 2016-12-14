
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/settings/fuelgauge/CBatterySaverSettings.h"
#include "elastos/droid/settings/CSettingsActivity.h"
#include "elastos/droid/settings/widget/CSwitchBar.h"
#include "elastos/droid/os/AsyncTask.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Settings::Widget::EIID_ISwitchBarOnSwitchChangeListener;
using Elastos::Droid::Settings::Widget::CSwitchBar;
using Elastos::Droid::Settings::CSettingsActivity;

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Fuelgauge {

//===============================================================================
//                  CBatterySaverSettings::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL(CBatterySaverSettings::InnerListener, Object, ISwitchBarOnSwitchChangeListener)

CBatterySaverSettings::InnerListener::InnerListener(
    /* [in] */ CBatterySaverSettings* host)
    : mHost(host)
{}

ECode CBatterySaverSettings::InnerListener::OnSwitchChanged(
    /* [in] */ ISwitch* switchView,
    /* [in] */ Boolean isChecked)
{
    return mHost->OnSwitchChanged(switchView, isChecked);
}

//===============================================================================
//                  CBatterySaverSettings::OnActivityCreatedSettingPref
//===============================================================================

CBatterySaverSettings::OnActivityCreatedSettingPref::OnActivityCreatedSettingPref(
    /* [in] */ Int32 type,
    /* [in] */ const String& key,
    /* [in] */ const String& setting,
    /* [in] */ Int32 def,
    /* [in] */ ArrayOf<Int32>* values)
    : SettingPref(type, key, setting, def, values)
{}

ECode CBatterySaverSettings::OnActivityCreatedSettingPref::GetCaption(
    /* [in] */ IResources* res,
    /* [in] */ Int32 value,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if (value > 0 && value < 100) {
        AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(value));
        return res->GetString(R::string::battery_saver_turn_on_automatically_pct, args, result);
    }
    return res->GetString(R::string::battery_saver_turn_on_automatically_never, result);
}

//===============================================================================
//                  CBatterySaverSettings::MyRunnable
//===============================================================================

CBatterySaverSettings::MyRunnable::MyRunnable(
    /* [in] */ CBatterySaverSettings* host,
    /* [in] */ Int32 id)
    : mHost(host)
    , mId(id)
{}

ECode CBatterySaverSettings::MyRunnable::Run()
{
    switch (mId) {
        case 0:
            mHost->UpdateSwitch();
            break;
        case 1:
            AutoPtr<Runnable2> runnable = new Runnable2(mHost);
            AsyncTask::Execute(runnable);
            break;
    }
    return NOERROR;
}

//===============================================================================
//                  CBatterySaverSettings::Runnable2
//===============================================================================

CBatterySaverSettings::Runnable2::Runnable2(
    /* [in] */ CBatterySaverSettings* host)
    : mHost(host)
{}

ECode CBatterySaverSettings::Runnable2::Run()
{
    if (DEBUG) {
        Logger::D(TAG, "Starting low power mode from settings");
    }
    mHost->TrySetPowerSaveMode(TRUE);
    return NOERROR;
}

//===============================================================================
//                  CBatterySaverSettings::Receiver
//===============================================================================

CBatterySaverSettings::Receiver::Receiver(
    /* [in] */ CBatterySaverSettings* host)
    : mRegistered(FALSE)
    , mHost(host)
{}

ECode CBatterySaverSettings::Receiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (DEBUG) Logger::D(TAG, "Received %s", action.string());
    Boolean res;
    mHost->mHandler->Post(mHost->mUpdateSwitch, &res);
    return NOERROR;
}

ECode CBatterySaverSettings::Receiver::SetListening(
    /* [in] */ Boolean listening)
{
    if (listening && !mRegistered) {
        AutoPtr<IIntentFilter> filter;
        CIntentFilter::New(IPowerManager::ACTION_POWER_SAVE_MODE_CHANGING, (IIntentFilter**)&filter);
        AutoPtr<IIntent> intent;
        mHost->mContext->RegisterReceiver(this, filter, (IIntent**)&intent);
        mRegistered = TRUE;
    }
    else if (!listening && mRegistered) {
        mHost->mContext->UnregisterReceiver(this);
        mRegistered = FALSE;
    }
    return NOERROR;
}

//===============================================================================
//                  CBatterySaverSettings::SettingsObserver
//===============================================================================

CBatterySaverSettings::SettingsObserver::SettingsObserver(
    /* [in] */ CBatterySaverSettings* host)
    : mHost(host)
{}

ECode CBatterySaverSettings::SettingsObserver::constructor(
    /* [in] */ IHandler* handler)
{
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    global->GetUriFor(ISettingsGlobal::LOW_POWER_MODE_TRIGGER_LEVEL,
            (IUri**)&LOW_POWER_MODE_TRIGGER_LEVEL_URI);
    return ContentObserver::constructor(handler);
}

ECode CBatterySaverSettings::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    Boolean res;
    if (IObject::Probe(LOW_POWER_MODE_TRIGGER_LEVEL_URI)->Equals(uri, &res), res) {
        mHost->mTriggerPref->Update(mHost->mContext);
    }
    return NOERROR;
}

ECode CBatterySaverSettings::SettingsObserver::SetListening(
    /* [in] */ Boolean listening)
{
    AutoPtr<IContentResolver> cr = mHost->GetContentResolver();
    if (listening) {
        cr->RegisterContentObserver(LOW_POWER_MODE_TRIGGER_LEVEL_URI, FALSE, this);
    }
    else {
        cr->UnregisterContentObserver(this);
    }
    return NOERROR;
}

//===============================================================================
//                  CBatterySaverSettings
//===============================================================================

const String CBatterySaverSettings::TAG("CBatterySaverSettings");
const Boolean CBatterySaverSettings::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);
const String CBatterySaverSettings::KEY_TURN_ON_AUTOMATICALLY("turn_on_automatically");
const Int64 CBatterySaverSettings::WAIT_FOR_SWITCH_ANIM = 500;

CAR_OBJECT_IMPL(CBatterySaverSettings);

CBatterySaverSettings::CBatterySaverSettings()
    : mCreated(FALSE)
    , mValidListener(FALSE)
{}

CBatterySaverSettings::~CBatterySaverSettings()
{}

ECode CBatterySaverSettings::constructor()
{
    SettingsPreferenceFragment::constructor();

    CHandler::New((IHandler**)&mHandler);
    mSettingsObserver = new SettingsObserver(this);
    mSettingsObserver->constructor(mHandler);
    mReceiver = new Receiver(this);
    mUpdateSwitch = new MyRunnable(this, 0);
    mStartMode = new MyRunnable(this, 1);
    mListener = new InnerListener(this);
    return NOERROR;
}

ECode CBatterySaverSettings::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    SettingsPreferenceFragment::OnActivityCreated(savedInstanceState);
    if (mCreated) return NOERROR;
    mCreated = TRUE;
    AddPreferencesFromResource(R::xml::battery_saver_settings);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    mContext = IContext::Probe(activity);
    ((CSettingsActivity*)ISettingsActivity::Probe(activity))->GetSwitchBar((ISwitchBar**)&mSwitchBar);
    CSwitchBar* switchBar = (CSwitchBar*)mSwitchBar.Get();
    mSwitch = ISwitch::Probe(switchBar->GetSwitch());
    switchBar->Show();

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr< ArrayOf<Int32> > args;
    resources->GetInt32Array(R::array::battery_saver_trigger_values, (ArrayOf<Int32>**)&args);
    mTriggerPref = new OnActivityCreatedSettingPref(SettingPref::TYPE_GLOBAL, KEY_TURN_ON_AUTOMATICALLY,
            ISettingsGlobal::LOW_POWER_MODE_TRIGGER_LEVEL,
            0, /*default*/ args);
    mTriggerPref->Init(this);

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    mPowerManager = IPowerManager::Probe(obj);
    return NOERROR;
}

ECode CBatterySaverSettings::OnDestroyView()
{
    SettingsPreferenceFragment::OnDestroyView();
    ((CSwitchBar*)mSwitchBar.Get())->Hide();
    return NOERROR;
}

ECode CBatterySaverSettings::OnResume()
{
    SettingsPreferenceFragment::OnResume();
    mSettingsObserver->SetListening(TRUE);
    mReceiver->SetListening(TRUE);
    if (!mValidListener) {
        ((CSwitchBar*)mSwitchBar.Get())->AddOnSwitchChangeListener(mListener);
        mValidListener = TRUE;
    }
    UpdateSwitch();
    return NOERROR;
}

ECode CBatterySaverSettings::OnPause()
{
    SettingsPreferenceFragment::OnPause();
    mSettingsObserver->SetListening(FALSE);
    mReceiver->SetListening(FALSE);
    if (mValidListener) {
        ((CSwitchBar*)mSwitchBar.Get())->RemoveOnSwitchChangeListener(mListener);
        mValidListener = FALSE;
    }
    return NOERROR;
}

ECode CBatterySaverSettings::OnSwitchChanged(
    /* [in] */ ISwitch* switchView,
    /* [in] */ Boolean isChecked)
{
    mHandler->RemoveCallbacks(mStartMode);
    if (isChecked) {
        Boolean res;
        mHandler->PostDelayed(mStartMode, WAIT_FOR_SWITCH_ANIM, &res);
    }
    else {
        if (DEBUG) Logger::D(TAG, "Stopping low power mode from settings");
        TrySetPowerSaveMode(FALSE);
    }
    return NOERROR;
}

void CBatterySaverSettings::TrySetPowerSaveMode(
    /* [in] */ Boolean mode)
{
    Boolean res;
    if (mPowerManager->SetPowerSaveMode(mode, &res), !res) {
        if (DEBUG) Logger::D(TAG, "Setting mode failed, fallback to current value");
        mHandler->Post(mUpdateSwitch, &res);
    }
}

void CBatterySaverSettings::UpdateSwitch()
{
    Boolean mode;
    mPowerManager->IsPowerSaveMode(&mode);
    Boolean isChecked;
    ICheckable::Probe(mSwitch)->IsChecked(&isChecked);
    if (DEBUG) Logger::D(TAG, "updateSwitch: isChecked=%d mode=%d", isChecked, mode);
    if (mode == isChecked) return;

    // set listener to NULL so that that code below doesn't trigger OnCheckedChanged()
    if (mValidListener) {
        ((CSwitchBar*)mSwitchBar.Get())->RemoveOnSwitchChangeListener(mListener);
    }
    ICheckable::Probe(mSwitch)->SetChecked(mode);
    if (mValidListener) {
        ((CSwitchBar*)mSwitchBar.Get())->AddOnSwitchChangeListener(mListener);
    }
}

} // namespace Fuelgauge
} // namespace Settings
} // namespace Droid
} // namespace Elastos