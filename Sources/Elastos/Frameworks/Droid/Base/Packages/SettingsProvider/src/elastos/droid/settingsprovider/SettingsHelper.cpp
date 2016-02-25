
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Location.h>
#include <Elastos.Droid.Media.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/settingsprovider/SettingsHelper.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::Backup::IIBackupManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Media::CAudioManager;
using Elastos::Droid::Media::CRingtoneManagerHelper;
using Elastos::Droid::Media::IRingtoneManagerHelper;
using Elastos::Droid::Media::IRingtoneManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::IIPowerManager;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;

namespace Elastos {
namespace Droid {
namespace SettingsProvider {

const String SettingsHelper::SILENT_RINGTONE("_silent");

SettingsHelper::SettingsHelper(
    /* [in] */ IContext* context)
    : mContext(context)
{
    AutoPtr<IInterface> tempObj;
    mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&tempObj);
    mAudioManager = IAudioManager::Probe(tempObj);
}

Boolean SettingsHelper::RestoreValue(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    if (ISettingsSystem::SCREEN_BRIGHTNESS.Equals(name)) {
        SetBrightness(StringUtils::ParseInt32(value));
    }
    else if (ISettingsSystem::SOUND_EFFECTS_ENABLED.Equals(name)) {
        SetSoundEffects(StringUtils::ParseInt32(value) == 1);
    }
    else if (ISettingsSecure::LOCATION_PROVIDERS_ALLOWED.Equals(name)) {
        SetGpsLocation(value);
        return FALSE;
    }
    else if (ISettingsSecure::BACKUP_AUTO_RESTORE.Equals(name)) {
        SetAutoRestore(StringUtils::ParseInt32(value) == 1);
    }
    else if (IsAlreadyConfiguredCriticalAccessibilitySetting(name)) {
        return FALSE;
    }
    else if (ISettingsSystem::RINGTONE.Equals(name)
            || ISettingsSystem::NOTIFICATION_SOUND.Equals(name)) {
        SetRingtone(name, value);
        return FALSE;
    }

    return TRUE;
}

String SettingsHelper::OnBackupValue(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    // Special processing for backing up ringtones
    if (ISettingsSystem::RINGTONE.Equals(name)
            || ISettingsSystem::NOTIFICATION_SOUND.Equals(name)) {
        if (value.IsNull()) {
            // Silent ringtone
            return SILENT_RINGTONE;
        }
        else {
            return GetCanonicalRingtoneValue(value);
        }
    }
    // Return the original value
    return value;
}

void SettingsHelper::SetRingtone(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    // If it's null, don't change the default
    if (value.IsNull()) return;
    AutoPtr<IUri> ringtoneUri;
    if (SILENT_RINGTONE.Equals(value)) {
    }
    else {
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> canonicalUri;
        helper->Parse(value, (IUri**)&canonicalUri);
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        resolver->Uncanonicalize(canonicalUri, (IUri**)&ringtoneUri);
        if (ringtoneUri == NULL) {
            // Unrecognized or invalid Uri, don't restore
            return;
        }
    }
    const Int32 ringtoneType = ISettingsSystem::RINGTONE.Equals(name)
            ? IRingtoneManager::TYPE_RINGTONE : IRingtoneManager::TYPE_NOTIFICATION;

    AutoPtr<IRingtoneManagerHelper> helper;
    CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&helper);

    helper->SetActualDefaultRingtoneUri(mContext, ringtoneType, ringtoneUri);
}

String SettingsHelper::GetCanonicalRingtoneValue(
    /* [in] */ const String& value)
{
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> ringtoneUri;
    helper->Parse(value, (IUri**)&ringtoneUri);

    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IUri> canonicalUri;
    resolver->Canonicalize(ringtoneUri, (IUri**)&canonicalUri);

    String str;
    return canonicalUri == NULL ? String(NULL) : (IObject::Probe(canonicalUri)->ToString(&str), str);
}

Boolean SettingsHelper::IsAlreadyConfiguredCriticalAccessibilitySetting(
    /* [in] */ const String& name)
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
    }
    else if (ISettingsSecure::TOUCH_EXPLORATION_GRANTED_ACCESSIBILITY_SERVICES.Equals(name) ||
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
    AutoPtr<IInterface> service = ServiceManager::GetService(IContext::BACKUP_SERVICE);
    AutoPtr<IIBackupManager> backup = IIBackupManager::Probe(service);
    if (backup != NULL) {
        backup->SetAutoRestore(enabled);
    }
    //} catch (RemoteException doe) {
    //
    //}
}

void SettingsHelper::SetGpsLocation(
    /* [in] */ const String& value)
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    AutoPtr<IUserManager> um = IUserManager::Probe(obj);
    Boolean res;
    if (um->HasUserRestriction(IUserManager::DISALLOW_SHARE_LOCATION, &res), res) {
        return;
    }

    String GPS = ILocationManager::GPS_PROVIDER;
    Boolean enabled = GPS.Equals(value) ||
            value.StartWith(GPS + String(",")) ||
            value.EndWith(String(",") + GPS) ||
            value.Contains(String(",") + GPS + String(","));
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
    }
    else {
        mAudioManager->UnloadSoundEffects();
    }
}

void SettingsHelper::SetBrightness(
    /* [in] */ Int32 brightness)
{
    //try{
    AutoPtr<IInterface> service = ServiceManager::GetService(IContext::POWER_SERVICE);
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
        localeString += "-";
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
    // TODO: The following is not working as intended because the network is forcing a locale
    // change after registering. Need to find some other way to detect if the user manually
    // changed the locale
    Boolean temp;
    if (conf->IsUserSetLocale(&temp), temp) return;  // Don't change if user set it in the SetupWizard

    AutoPtr<IAssetManager> asset;
    mContext->GetAssets((IAssetManager**)&asset);
    AutoPtr<ArrayOf<String> > availableLocales;
    asset->GetLocales((ArrayOf<String>**)&availableLocales);
    // Replace "_" with "-" to deal with older backups.
    String localeCode = String((const char*)(data->GetPayload()), size);
    localeCode.Replace('_', '-');

    AutoPtr<ILocale> loc;
    for (Int32 i = 0; i < availableLocales->GetLength(); ++i) {
        if ((*availableLocales)[i].Equals(localeCode)) {
            AutoPtr<ILocaleHelper> helper;
            CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
            helper->ForLanguageTag(localeCode, (ILocale**)&loc);
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
} // namespace Droid
} // namespace Elastos
