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

#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/settings/notification/COtherSoundSettings.h"
#include "elastos/droid/settings/Utils.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Settings::Search::EIID_IIndexable;

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Provider::CSearchIndexableResource;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Core::CoreUtils;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

const String COtherSoundSettings::TAG("COtherSoundSettings");

const Int32 COtherSoundSettings::DEFAULT_ON = 1;

const Int32 COtherSoundSettings::EMERGENCY_TONE_SILENT = 0;
const Int32 COtherSoundSettings::EMERGENCY_TONE_ALERT = 1;
const Int32 COtherSoundSettings::EMERGENCY_TONE_VIBRATE = 2;
const Int32 COtherSoundSettings::DEFAULT_EMERGENCY_TONE = EMERGENCY_TONE_SILENT;

const Int32 COtherSoundSettings::DOCK_AUDIO_MEDIA_DISABLED = 0;
const Int32 COtherSoundSettings::DOCK_AUDIO_MEDIA_ENABLED = 1;
const Int32 COtherSoundSettings::DEFAULT_DOCK_AUDIO_MEDIA = DOCK_AUDIO_MEDIA_DISABLED;

const String COtherSoundSettings::KEY_DIAL_PAD_TONES("dial_pad_tones");
const String COtherSoundSettings::KEY_SCREEN_LOCKING_SOUNDS("screen_locking_sounds");
const String COtherSoundSettings::KEY_DOCKING_SOUNDS("docking_sounds");
const String COtherSoundSettings::KEY_TOUCH_SOUNDS("touch_sounds");
const String COtherSoundSettings::KEY_VIBRATE_ON_TOUCH("vibrate_on_touch");
const String COtherSoundSettings::KEY_DOCK_AUDIO_MEDIA("dock_audio_media");
const String COtherSoundSettings::KEY_EMERGENCY_TONE("emergency_tone");

AutoPtr<ISettingPref> COtherSoundSettings::PREF_DIAL_PAD_TONES;

AutoPtr<ISettingPref> COtherSoundSettings::PREF_SCREEN_LOCKING_SOUNDS;

AutoPtr<ISettingPref> COtherSoundSettings::PREF_DOCKING_SOUNDS;

AutoPtr<ISettingPref> COtherSoundSettings::PREF_TOUCH_SOUNDS;

AutoPtr<ISettingPref> COtherSoundSettings::PREF_VIBRATE_ON_TOUCH;

AutoPtr<ISettingPref> COtherSoundSettings::PREF_DOCK_AUDIO_MEDIA;

AutoPtr<ISettingPref> COtherSoundSettings::PREF_EMERGENCY_TONE;

AutoPtr< ArrayOf<ISettingPref*> > COtherSoundSettings::PREFS;

Boolean COtherSoundSettings::InitStatic()
{
    if (PREF_DIAL_PAD_TONES == NULL) {
        PREF_DIAL_PAD_TONES = new MySettingPref(0, SettingPref::TYPE_SYSTEM,
                KEY_DIAL_PAD_TONES, ISettingsSystem::DTMF_TONE_WHEN_DIALING, DEFAULT_ON, NULL);
    }

    if (PREF_SCREEN_LOCKING_SOUNDS == NULL) {
        PREF_SCREEN_LOCKING_SOUNDS = new MySettingPref(1, SettingPref::TYPE_SYSTEM,
                KEY_SCREEN_LOCKING_SOUNDS, ISettingsSystem::LOCKSCREEN_SOUNDS_ENABLED, DEFAULT_ON, NULL);
    }

    if (PREF_DOCKING_SOUNDS == NULL) {
        PREF_DOCKING_SOUNDS = new MySettingPref(2, SettingPref::TYPE_GLOBAL,
                KEY_DOCKING_SOUNDS, ISettingsGlobal::DOCK_SOUNDS_ENABLED, DEFAULT_ON, NULL);
    }

    if (PREF_TOUCH_SOUNDS == NULL) {
        PREF_TOUCH_SOUNDS = new MySettingPref(3, SettingPref::TYPE_SYSTEM,
                KEY_TOUCH_SOUNDS, ISettingsSystem::SOUND_EFFECTS_ENABLED, DEFAULT_ON, NULL);
    }

    if (PREF_VIBRATE_ON_TOUCH == NULL) {
        PREF_VIBRATE_ON_TOUCH = new MySettingPref(4, SettingPref::TYPE_SYSTEM,
                KEY_VIBRATE_ON_TOUCH, ISettingsSystem::HAPTIC_FEEDBACK_ENABLED, DEFAULT_ON, NULL);
    }

    if (PREF_DOCK_AUDIO_MEDIA == NULL) {
        AutoPtr< ArrayOf<Int32> > args = ArrayOf<Int32>::Alloc(2);
        (*args)[0] = DOCK_AUDIO_MEDIA_DISABLED;
        (*args)[1] = DOCK_AUDIO_MEDIA_ENABLED;

        PREF_DOCK_AUDIO_MEDIA = new MySettingPref(5, SettingPref::TYPE_GLOBAL, KEY_DOCK_AUDIO_MEDIA,
                 ISettingsGlobal::DOCK_AUDIO_MEDIA_ENABLED, DEFAULT_DOCK_AUDIO_MEDIA, args);
    }

    if (PREF_EMERGENCY_TONE == NULL) {
        AutoPtr< ArrayOf<Int32> > args = ArrayOf<Int32>::Alloc(3);
        (*args)[0] = EMERGENCY_TONE_ALERT;
        (*args)[1] = EMERGENCY_TONE_VIBRATE;
        (*args)[2] = EMERGENCY_TONE_SILENT;

        PREF_EMERGENCY_TONE = new MySettingPref(6, SettingPref::TYPE_GLOBAL, KEY_EMERGENCY_TONE,
                ISettingsGlobal::EMERGENCY_TONE, DEFAULT_EMERGENCY_TONE, args);
    }

    if (PREFS == NULL) {
        PREFS = ArrayOf<ISettingPref*>::Alloc(7);
        PREFS->Set(0, PREF_DIAL_PAD_TONES);
        PREFS->Set(1, PREF_SCREEN_LOCKING_SOUNDS);
        PREFS->Set(2, PREF_DOCKING_SOUNDS);
        PREFS->Set(3, PREF_TOUCH_SOUNDS);
        PREFS->Set(4, PREF_VIBRATE_ON_TOUCH);
        PREFS->Set(5, PREF_DOCK_AUDIO_MEDIA);
        PREFS->Set(6, PREF_EMERGENCY_TONE);
    }

    return TRUE;
}

const Boolean COtherSoundSettings::sInitStatic = InitStatic();

AutoPtr<IIndexableSearchIndexProvider> COtherSoundSettings::SEARCH_INDEX_DATA_PROVIDER;

AutoPtr<IIndexableSearchIndexProvider> COtherSoundSettings::GetSEARCH_INDEX_DATA_PROVIDER()
{
    if (SEARCH_INDEX_DATA_PROVIDER == NULL) {
        SEARCH_INDEX_DATA_PROVIDER = new COtherSoundSettings::MyBaseSearchIndexProvider();
    }

    return SEARCH_INDEX_DATA_PROVIDER;
}

//===============================================================================
//                  COtherSoundSettings::MyBaseSearchIndexProvider
//===============================================================================

COtherSoundSettings::MyBaseSearchIndexProvider::MyBaseSearchIndexProvider()
{}

COtherSoundSettings::MyBaseSearchIndexProvider::~MyBaseSearchIndexProvider()
{}

ECode COtherSoundSettings::MyBaseSearchIndexProvider::GetXmlResourcesToIndex(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<ISearchIndexableResource> sir;
    CSearchIndexableResource::New(context, (ISearchIndexableResource**)&sir);
    sir->SetXmlResId(R::xml::other_sound_settings);

    AutoPtr< ArrayOf<ISearchIndexableResource*> > args = ArrayOf<ISearchIndexableResource*>::Alloc(1);
    args->Set(0, sir);

    return Arrays::AsList(args, result);
}

ECode COtherSoundSettings::MyBaseSearchIndexProvider::GetNonIndexableKeys(
    /* [in] */ IContext* context,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IArrayList> rt;
    CArrayList::New((IArrayList**)&rt);
    for (Int32 i = 0; i < PREFS->GetLength(); i++) {
        SettingPref* pref = (SettingPref*)(*PREFS)[i];
        if (!pref->IsApplicable(context)) {
            rt->Add(CoreUtils::Convert(pref->GetKey()));
        }
    }
    *result = IList::Probe(rt);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//===============================================================================
//                 COtherSoundSettings::SettingsObserver
//===============================================================================

COtherSoundSettings::SettingsObserver::SettingsObserver(
    /* [in] */ COtherSoundSettings* host)
    : mHost(host)
{}

ECode COtherSoundSettings::SettingsObserver::constructor()
{
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    return ContentObserver::constructor(handler);
}

ECode COtherSoundSettings::SettingsObserver::Register(
    /* [in] */ Boolean _register)
{
    AutoPtr<IContentResolver> cr = mHost->GetContentResolver();
    if (_register) {
        for (Int32 i = 0; i < PREFS->GetLength(); i++) {
            SettingPref* pref = (SettingPref*)(*PREFS)[i];
            cr->RegisterContentObserver(pref->GetUri(), FALSE, this);
        }
    }
    else {
        cr->UnregisterContentObserver(this);
    }
    return NOERROR;
}

ECode COtherSoundSettings::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    ContentObserver::OnChange(selfChange, uri);
    for (Int32 i = 0; i < PREFS->GetLength(); i++) {
        SettingPref* pref = (SettingPref*)(*PREFS)[i];
        Boolean res;
        IObject::Probe(pref->GetUri())->Equals(uri, &res);
        if (res) {
            pref->Update(mHost->mContext);
            return NOERROR;
        }
    }
    return NOERROR;
}

//===============================================================================
//                 COtherSoundSettings::MySettingPref
//===============================================================================

COtherSoundSettings::MySettingPref::MySettingPref(
    /* [in] */ Int32 id,
    /* [in] */ Int32 type,
    /* [in] */ const String& key,
    /* [in] */ const String& setting,
    /* [in] */ Int32 def,
    /* [in] */ ArrayOf<Int32>* values)
    : SettingPref::SettingPref(type, key, setting, def, values)
    , mId(id)
{}

COtherSoundSettings::MySettingPref::~MySettingPref()
{}

Boolean COtherSoundSettings::MySettingPref::IsApplicable(
    /* [in] */ IContext* context)
{
    switch (mId) {
        case 0:
            return Utils::IsVoiceCapable(context);
        case 1:
        case 3:
            return SettingPref::IsApplicable(context);
        case 2:
        case 5:
            return HasDockSettings(context);
        case 4:
            return HasHaptic(context);
        case 6: {
            AutoPtr<ITelephonyManagerHelper> helper;
            CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&helper);
            AutoPtr<ITelephonyManager> tm;
            helper->GetDefault((ITelephonyManager**)&tm);
            Int32 activePhoneType;
            tm->GetCurrentPhoneType(&activePhoneType);
            return activePhoneType == ITelephonyManager::PHONE_TYPE_CDMA;
        }
    }
    return FALSE;
}

ECode COtherSoundSettings::MySettingPref::GetCaption(
    /* [in] */ IResources* res,
    /* [in] */ Int32 value,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    switch (mId) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            return SettingPref::GetCaption(res, value, result);
        case 5: {
            switch (value) {
                case DOCK_AUDIO_MEDIA_DISABLED:
                    return res->GetString(R::string::dock_audio_media_disabled, result);
                case DOCK_AUDIO_MEDIA_ENABLED:
                    return res->GetString(R::string::dock_audio_media_enabled, result);
                default:
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }

        case 6: {
            switch (value) {
                case EMERGENCY_TONE_SILENT:
                    return res->GetString(R::string::emergency_tone_silent, result);
                case EMERGENCY_TONE_ALERT:
                    return res->GetString(R::string::emergency_tone_alert, result);
                case EMERGENCY_TONE_VIBRATE:
                    return res->GetString(R::string::emergency_tone_vibrate, result);
                default:
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
    }
    return NOERROR;
}

ECode COtherSoundSettings::MySettingPref::SetSetting(
    /* [in] */ IContext* context,
    /* [in] */ Int32 value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    switch (mId) {
        case 0:
        case 1:
        case 2:
        case 4:
        case 5:
        case 6:
            return SettingPref::SetSetting(context, value, result);

        case 3: {
            AutoPtr<IInterface> obj;
            context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
            IAudioManager* am = IAudioManager::Probe(obj);
            if (value != 0) {
                am->LoadSoundEffects();
            }
            else {
                am->UnloadSoundEffects();
            }
            return SettingPref::SetSetting(context, value, result);
        }
    }
    return NOERROR;
}

//===============================================================================
//                 COtherSoundSettings
//===============================================================================

CAR_INTERFACE_IMPL(COtherSoundSettings, SettingsPreferenceFragment, IIndexable)

CAR_OBJECT_IMPL(COtherSoundSettings)

COtherSoundSettings::COtherSoundSettings()
{}

COtherSoundSettings::~COtherSoundSettings()
{}

ECode COtherSoundSettings::constructor()
{
    mSettingsObserver = new SettingsObserver(this);

    return SettingsPreferenceFragment::constructor();
}

ECode COtherSoundSettings::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    SettingsPreferenceFragment::OnCreate(savedInstanceState);

    AddPreferencesFromResource(R::xml::other_sound_settings);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    mContext = IContext::Probe(activity);

    for (Int32 i = 0; i < PREFS->GetLength(); i++) {
        SettingPref* pref = (SettingPref*)(*PREFS)[i];
        pref->Init(this);
    }

    return NOERROR;
}

ECode COtherSoundSettings::OnResume()
{
    SettingsPreferenceFragment::OnResume();
    return mSettingsObserver->Register(TRUE);
}

ECode COtherSoundSettings::OnPause()
{
    SettingsPreferenceFragment::OnPause();
    return mSettingsObserver->Register(FALSE);
}

Boolean COtherSoundSettings::HasDockSettings(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    Boolean res;
    resources->GetBoolean(R::bool_::has_dock_settings, &res);
    return res;
}

Boolean COtherSoundSettings::HasHaptic(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&obj);
    IVibrator* vibrator = IVibrator::Probe(obj);
    Boolean res;
    return vibrator != NULL && (vibrator->HasVibrator(&res), res);
}

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos