#include "SettingsHelper.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/core/StringUtils.h>

using Elastos::Utility::ILocale;
using Elastos::Utility::CLocale;
using Elastos::Core::StringUtils;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::Backup::IIBackupManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Media::CAudioManager;
using Elastos::Droid::Os::IIPowerManager;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSecure;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SettingsProvider {

SettingsHelper::SettingsHelper(
        /* [in] */ IContext* context)
    :mContext(context)
{
    AutoPtr<IInterface> tempObj;
    mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&tempObj);
    mAudioManager = IAudioManager::Probe(tempObj.Get());
}

Boolean SettingsHelper::RestoreValue(
    /* [in] */ String& name,
    /* [in] */ String& value)
{
    if (ISettingsSystem::SCREEN_BRIGHTNESS.Equals(name)) {
        SetBrightness(StringUtils::ParseInt32(value));
    } else if (ISettingsSystem::SOUND_EFFECTS_ENABLED.Equals(name)) {
        SetSoundEffects(StringUtils::ParseInt32(value) == 1);
    } else if (ISettingsSecure::LOCATION_PROVIDERS_ALLOWED.Equals(name)) {
        SetGpsLocation(value);
        return FALSE;
    } else if (ISettingsSecure::BACKUP_AUTO_RESTORE.Equals(name)) {
        SetAutoRestore(StringUtils::ParseInt32(value) == 1);
    } else if (IsAlreadyConfiguredCriticalAccessibilitySetting(name)) {
        return FALSE;
    }
    return TRUE;
}

Boolean SettingsHelper::IsAlreadyConfiguredCriticalAccessibilitySetting(
    /* [in] */ String& name)
{
    // These are the critical accessibility settings that are required for a
    // blind user to be able to interact with the device. If these settings are
    // already configured, we will not overwrite them. If they are already set,
    // it means that the user has performed a global gesture to enable accessibility
    // and definitely needs these features working after the restore.
    if (ISettingsSecure::ACCESSIBILITY_ENABLED.Equals(name) ||
        ISettingsSecure::ACCESSIBILITY_SCRIPT_INJECTION.Equals(name) ||
        ISettingsSecure::ACCESSIBILITY_SPEAK_PASSWORD.Equals(name) ||
        ISettingsSecure::TOUCH_EXPLORATION_ENABLED.Equals(name)) {

        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<ISettingsSecure> secure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
        Int32 temp;
        secure->GetInt32(resolver, name, 0, &temp);
        return temp != 0;
    } else if (ISettingsSecure::TOUCH_EXPLORATION_GRANTED_ACCESSIBILITY_SERVICES.Equals(name) ||
               ISettingsSecure::ENABLED_ACCESSIBILITY_SERVICES.Equals(name)) {

        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<ISettingsSecure> secure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
        String value;
        secure->GetString(resolver, name, &value);
        return !TextUtils::IsEmpty(value);
    }
    return FALSE;
}

void SettingsHelper::SetAutoRestore(
    /* [in] */ Boolean enabled)
{
    //try{
    AutoPtr<IInterface> service = ServiceManager::GetService(String(IContext::BACKUP_SERVICE));
    AutoPtr<IIBackupManager> backup = IIBackupManager::Probe(service);
    if (backup != NULL) {
        backup->SetAutoRestore(enabled);
    }
    //} catch (RemoteException doe) {
    //
    //}
}

void SettingsHelper::SetGpsLocation(
    /* [in] */ String& value)
{
    String GPS = ILocationManager::GPS_PROVIDER;
    Boolean enabled = GPS.Equals(value) ||
        value.StartWith(GPS + String(",")) ||
        value.EndWith(String(",") + GPS) ||
        value.Contains(String(",") + GPS + ",");
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    secure->SetLocationProviderEnabled(resolver, GPS, enabled);
}


void SettingsHelper::SetSoundEffects(
    /* [in] */ Boolean enable)
{
    if (enable) {
        mAudioManager->LoadSoundEffects();
    } else {
        mAudioManager->UnloadSoundEffects();
    }
}

void SettingsHelper::SetBrightness(
    /* [in] */ Int32 brightness)
{
    //try{
    AutoPtr<IInterface> service = ServiceManager::GetService(String(IContext::POWER_SERVICE));
    AutoPtr<IIPowerManager> power = IIPowerManager::Probe(service);
    if (power != NULL) {
        power->SetTemporaryScreenBrightnessSettingOverride(brightness);
    }
    //} catch (RemoteException doe) {
    //
    //}

}

AutoPtr<ArrayOf<Byte> > SettingsHelper::GetLocaleData()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IConfiguration> conf;
    res->GetConfiguration((IConfiguration**)&conf);
    AutoPtr<ILocale> loc;
    conf->GetLocale((ILocale**)&loc);
    String localeString;
    loc->GetLanguage(&localeString);
    String country;
    loc->GetCountry(&country);
    if (!TextUtils::IsEmpty(country)) {
        localeString += "_";
        localeString += country;
    }
    return localeString.GetBytes();
}

void SettingsHelper::SetLocaleData(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 size)
{
    // Check if locale was set by the user:
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IConfiguration> conf;
    res->GetConfiguration((IConfiguration**)&conf);
    AutoPtr<ILocale> loc;
    conf->GetLocale((ILocale**)&loc);
    // TODO: The following is not working as intended because the network is forcing a locale
    // change after registering. Need to find some other way to detect if the user manually
    // changed the locale
    Boolean temp;
    if (conf->IsUserSetLocale(&temp), temp) return;  // Don't change if user set it in the SetupWizard
    AutoPtr<IAssetManager> asset;
    mContext->GetAssets((IAssetManager**)&asset);
    AutoPtr<ArrayOf<String> > availableLocales;
    asset->GetLocales((ArrayOf<String>**)&availableLocales);
    String localeCode = String((const char*)(data->GetPayload()), size);
    String language = String((const char*)(data->GetPayload()), 2);
    String country("");
    if (size > 4) {
        const char* tempData = (const char*)(&(*data)[3]);
        country = String(tempData, 2);
    }
    loc = NULL;
    for (Int32 i = 0; i < availableLocales->GetLength(); ++i) {
        if ((*availableLocales)[i].Equals(localeCode)) {
            CLocale::New(language, country, (ILocale**)&loc);
            break;
        }
    }
    if (loc == NULL) return; // Couldn't find the saved locale in this version of the software
    //try {
    AutoPtr<IIActivityManager> defualtAM = ActivityManagerNative::GetDefault();
    AutoPtr<IConfiguration> config;
    defualtAM->GetConfiguration((IConfiguration**)&config);
    config->SetLocale(loc);
    config->SetUserSetLocale(TRUE);
    defualtAM->UpdateConfiguration(config);
    //} catch (RemoteException e) {
    //Intentionally left blank
    //}


}

void SettingsHelper::ApplyAudioSettings()
{
    AutoPtr<IAudioManager> am;
    CAudioManager::New(mContext, (IAudioManager**)&am);
    am->ReloadAudioSettings();
}

} // namespace SettingsProvider
} // namespace Packages
} // namespace Droid
} // namespace Elastos
