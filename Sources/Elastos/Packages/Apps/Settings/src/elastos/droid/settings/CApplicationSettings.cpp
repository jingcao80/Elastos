//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Provider.h"
#include "elastos/droid/settings/CApplicationSettings.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
// #include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
// using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {

const String CApplicationSettings::KEY_TOGGLE_ADVANCED_SETTINGS("toggle_advanced_settings");
const String CApplicationSettings::KEY_APP_INSTALL_LOCATION("app_install_location");

const Int32 CApplicationSettings::APP_INSTALL_AUTO = 0;
const Int32 CApplicationSettings::APP_INSTALL_DEVICE = 1;
const Int32 CApplicationSettings::APP_INSTALL_SDCARD = 2;

const String CApplicationSettings::APP_INSTALL_DEVICE_ID("device");
const String CApplicationSettings::APP_INSTALL_SDCARD_ID("sdcard");
const String CApplicationSettings::APP_INSTALL_AUTO_ID("auto");

//===============================================================================
//                  CApplicationSettings::OnCreateDialogRunnable
//===============================================================================

CAR_INTERFACE_IMPL(CApplicationSettings::OnCreateOnPreferenceChangeListener, Object, IPreferenceOnPreferenceChangeListener)

CApplicationSettings::OnCreateOnPreferenceChangeListener::OnCreateOnPreferenceChangeListener(
    /* [in] */ CApplicationSettings* host)
    : mHost(host)
{}

CApplicationSettings::OnCreateOnPreferenceChangeListener::~OnCreateOnPreferenceChangeListener()
{}

ECode CApplicationSettings::OnCreateOnPreferenceChangeListener::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    String value;
    ICharSequence::Probe(newValue)->ToString(&value);
    mHost->HandleUpdateAppInstallLocation(value);
    *result = FALSE;
    return NOERROR;
}

//===============================================================================
//                  CApplicationSettings
//===============================================================================

CAR_OBJECT_IMPL(CApplicationSettings)

CApplicationSettings::CApplicationSettings()
{}

CApplicationSettings::~CApplicationSettings()
{}

ECode CApplicationSettings::constructor()
{
    return SettingsPreferenceFragment::constructor();
}

ECode CApplicationSettings::OnCreate(
    /* [in] */ IBundle* icicle)
{
    SettingsPreferenceFragment::OnCreate(icicle);

    AddPreferencesFromResource(R::xml::application_settings);

    AutoPtr<IPreference> pref;
    FindPreference(
            CoreUtils::Convert(KEY_TOGGLE_ADVANCED_SETTINGS), (IPreference**)&pref);
    mToggleAdvancedSettings = ICheckBoxPreference::Probe(pref);
    ITwoStatePreference::Probe(mToggleAdvancedSettings)->SetChecked(IsAdvancedSettingsEnabled());
    AutoPtr<IPreferenceScreen> _screen;
    GetPreferenceScreen((IPreferenceScreen**)&_screen);
    Boolean res;
    IPreferenceGroup* screen = IPreferenceGroup::Probe(_screen);
    screen->RemovePreference(IPreference::Probe(mToggleAdvancedSettings), &res);

    // not ready for prime time yet
    if (FALSE) {
        screen->RemovePreference(IPreference::Probe(mInstallLocation), &res);
    }

    AutoPtr<IPreference> installLocationPref;
    FindPreference(CoreUtils::Convert(KEY_APP_INSTALL_LOCATION), (IPreference**)&installLocationPref);
    mInstallLocation = IListPreference::Probe(installLocationPref);
    // Is app default install location set?
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int32 data;
    global->GetInt32(GetContentResolver(),
            ISettingsGlobal::SET_INSTALL_LOCATION, 0, &data);
    Boolean userSetInstLocation = (data != 0);
    if (!userSetInstLocation) {
        screen->RemovePreference(installLocationPref, &res);
    }
    else {
        mInstallLocation->SetValue(GetAppInstallLocation());
        AutoPtr<OnCreateOnPreferenceChangeListener> listener = new OnCreateOnPreferenceChangeListener(this);
        installLocationPref->SetOnPreferenceChangeListener(listener);
    }
    return NOERROR;
}

void CApplicationSettings::HandleUpdateAppInstallLocation(
    /* [in] */ const String& value)
{
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);

    AutoPtr<IContentResolver> resolver = GetContentResolver();
    Boolean res;
    if(APP_INSTALL_DEVICE_ID.Equals(value)) {
        global->PutInt32(resolver,
                ISettingsGlobal::DEFAULT_INSTALL_LOCATION, APP_INSTALL_DEVICE, &res);
    }
    else if (APP_INSTALL_SDCARD_ID.Equals(value)) {
        global->PutInt32(resolver,
                ISettingsGlobal::DEFAULT_INSTALL_LOCATION, APP_INSTALL_SDCARD, &res);
    }
    else if (APP_INSTALL_AUTO_ID.Equals(value)) {
        global->PutInt32(resolver,
                ISettingsGlobal::DEFAULT_INSTALL_LOCATION, APP_INSTALL_AUTO, &res);
    }
    else {
        // Should not happen, default to prompt...
        global->PutInt32(resolver,
                ISettingsGlobal::DEFAULT_INSTALL_LOCATION, APP_INSTALL_AUTO, &res);
    }
    mInstallLocation->SetValue(value);
}

ECode CApplicationSettings::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if (preference == IPreference::Probe(mToggleAdvancedSettings)) {
        Boolean value;
        ITwoStatePreference::Probe(mToggleAdvancedSettings)->IsChecked(&value);
        SetAdvancedSettingsEnabled(value);
    }

    return SettingsPreferenceFragment::OnPreferenceTreeClick(preferenceScreen, preference, res);
}

Boolean CApplicationSettings::IsAdvancedSettingsEnabled()
{
    AutoPtr<ISettingsSystem> sys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);
    Int32 data;
    sys->GetInt32(GetContentResolver(),
            ISettingsSystem::ADVANCED_SETTINGS,
            ISettingsSystem::ADVANCED_SETTINGS_DEFAULT, &data);
    return data > 0;
}

void CApplicationSettings::SetAdvancedSettingsEnabled(
    /* [in] */ Boolean enabled)
{
    Int32 value = enabled ? 1 : 0;
    // Change the system setting
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Boolean res;
    secure->PutInt32(GetContentResolver(), ISettingsSystem::ADVANCED_SETTINGS, value, &res);
    // TODO: the settings thing should broadcast this for thread safety purposes.
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_ADVANCED_SETTINGS_CHANGED, (IIntent**)&intent);
    intent->PutBooleanExtra(String("state"), value);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext::Probe(activity)->SendBroadcast(intent);
}

String CApplicationSettings::GetAppInstallLocation()
{
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);

    Int32 selectedLocation;
    global->GetInt32(GetContentResolver(),
            ISettingsGlobal::DEFAULT_INSTALL_LOCATION, APP_INSTALL_AUTO, &selectedLocation);
    if (selectedLocation == APP_INSTALL_DEVICE) {
        return APP_INSTALL_DEVICE_ID;
    }
    else if (selectedLocation == APP_INSTALL_SDCARD) {
        return APP_INSTALL_SDCARD_ID;
    }
    else  if (selectedLocation == APP_INSTALL_AUTO) {
        return APP_INSTALL_AUTO_ID;
    }
    else {
        // Default value, should not happen.
        return APP_INSTALL_AUTO_ID;
    }
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos