
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Location.h"
#include "elastos/droid/settings/location/LocationSettingsBase.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Location {

//===============================================================================
//                  LocationSettingsBase::MyBroadcastReceiver
//===============================================================================

LocationSettingsBase::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ LocationSettingsBase* host)
    : mHost(host)
{}

LocationSettingsBase::MyBroadcastReceiver::~MyBroadcastReceiver()
{}

ECode LocationSettingsBase::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (Logger::IsLoggable(TAG, Logger::___DEBUG)) {
        Logger::D(TAG, "Received location mode change intent: %s", TO_CSTR(intent));
    }
    return mHost->RefreshLocationMode();
}

//===============================================================================
//                  LocationSettingsBase
//===============================================================================

const String LocationSettingsBase::TAG("LocationSettingsBase");
const String LocationSettingsBase::MODE_CHANGING_ACTION("com.android.settings.location.MODE_CHANGING");
const String LocationSettingsBase::CURRENT_MODE_KEY("CURRENT_MODE");
const String LocationSettingsBase::NEW_MODE_KEY("NEW_MODE");

LocationSettingsBase::LocationSettingsBase()
    : mCurrentMode(0)
    , mActive(FALSE)
{}

LocationSettingsBase::~LocationSettingsBase()
{}

ECode LocationSettingsBase::constructor()
{
    return SettingsPreferenceFragment::constructor();
}

ECode LocationSettingsBase::OnCreate(
    /* [in] */ IBundle* icicle)
{
    SettingsPreferenceFragment::OnCreate(icicle);
    mReceiver = new MyBroadcastReceiver(this);
    return NOERROR;
}

ECode LocationSettingsBase::OnResume()
{
    SettingsPreferenceFragment::OnResume();
    mActive = TRUE;
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(ILocationManager::MODE_CHANGED_ACTION);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    return IContext::Probe(activity)->RegisterReceiver(mReceiver, filter, (IIntent**)&intent);
}

ECode LocationSettingsBase::OnPause()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    // try {
    IContext::Probe(activity)->UnregisterReceiver(mReceiver);
    // } catch (RuntimeException e) {
    //     // Ignore exceptions caused by race condition
    // }
    SettingsPreferenceFragment::OnPause();
    mActive = FALSE;
    return NOERROR;
}

Boolean LocationSettingsBase::IsRestricted()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IInterface> obj;
    IContext::Probe(activity)->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    IUserManager* um = IUserManager::Probe(obj);
    Boolean res;
    um->HasUserRestriction(IUserManager::DISALLOW_SHARE_LOCATION, &res);
    return res;
}

ECode LocationSettingsBase::SetLocationMode(
    /* [in] */ Int32 mode)
{
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    if (IsRestricted()) {
        // Location toggling disabled by user restriction. Read the current location mode to
        // update the location master switch.
        if (Logger::IsLoggable(TAG, Logger::INFO)) {
            Logger::I(TAG, "Restricted user, not setting location mode");
        }
        secure->GetInt32(GetContentResolver(), ISettingsSecure::LOCATION_MODE,
                ISettingsSecure::LOCATION_MODE_OFF, &mode);
        if (mActive) {
            OnModeChanged(mode, TRUE);
        }
        return NOERROR;
    }
    AutoPtr<IIntent> intent;
    CIntent::New(MODE_CHANGING_ACTION, (IIntent**)&intent);
    intent->PutExtra(CURRENT_MODE_KEY, mCurrentMode);
    intent->PutExtra(NEW_MODE_KEY, mode);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext::Probe(activity)->SendBroadcast(intent, Manifest::permission::WRITE_SECURE_SETTINGS);
    Boolean res;
    secure->PutInt32(GetContentResolver(), ISettingsSecure::LOCATION_MODE, mode, &res);

    return RefreshLocationMode();
}

ECode LocationSettingsBase::RefreshLocationMode()
{
    if (mActive) {
        AutoPtr<ISettingsSecure> secure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
        Int32 mode;
        secure->GetInt32(GetContentResolver(), ISettingsSecure::LOCATION_MODE,
                ISettingsSecure::LOCATION_MODE_OFF, &mode);
        mCurrentMode = mode;
        if (Logger::IsLoggable(TAG, Logger::INFO)) {
            Logger::I(TAG, "Location mode has been changed");
        }
        OnModeChanged(mode, IsRestricted());
    }
    return NOERROR;
}

} // namespace Location
} // namespace Settings
} // namespace Droid
} // namespace Elastos