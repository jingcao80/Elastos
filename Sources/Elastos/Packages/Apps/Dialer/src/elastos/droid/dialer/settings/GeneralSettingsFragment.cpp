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

#include "elastos/droid/dialer/settings/GeneralSettingsFragment.h"
#include "R.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Core::ICharSequence;
using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Settings {

//=================================================================
// GeneralSettingsFragment::RingtoneLookupCompleteHandler
//=================================================================
GeneralSettingsFragment::RingtoneLookupCompleteHandler::RingtoneLookupCompleteHandler(
    /* [in] */ GeneralSettingsFragment* host)
    : mHost(host)
{}

ECode GeneralSettingsFragment::RingtoneLookupCompleteHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case 1: {        //MSG_UPDATE_RINGTONE_SUMMARY:
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->mRingtonePreference->SetSummary(ICharSequence::Probe(obj));
            break;
        }
    }
    return NOERROR;
}

ECode GeneralSettingsFragment::RingtoneLookupCompleteHandler::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    Object::ToString(str);
    return NOERROR;
}

//=================================================================
// GeneralSettingsFragment::RingtoneLookupRunnable
//=================================================================
GeneralSettingsFragment::RingtoneLookupRunnable::RingtoneLookupRunnable(
    /* [in] */ GeneralSettingsFragment* host)
    : mHost(host)
{}

ECode GeneralSettingsFragment::RingtoneLookupRunnable::Run()
{
    if (mHost->mRingtonePreference != NULL) {
        // TODO
        Logger::I("GeneralSettingsFragment::RingtoneLookupRunnable", "TODO Run");
        // SettingsUtil.updateRingtoneName(
        //         mHost->mContext,
        //         mHost->mRingtoneLookupComplete,
        //         IRingtoneManager::TYPE_RINGTONE,
        //         mHost->mRingtonePreference,
        //         MSG_UPDATE_RINGTONE_SUMMARY);
    }
    return NOERROR;
}

//=================================================================
// GeneralSettingsFragment
//=================================================================
const String GeneralSettingsFragment::CATEGORY_SOUNDS_KEY("dialer_general_sounds_category_key");
const String GeneralSettingsFragment::BUTTON_RINGTONE_KEY("button_ringtone_key");
const String GeneralSettingsFragment::BUTTON_VIBRATE_ON_RING("button_vibrate_on_ring");
const String GeneralSettingsFragment::BUTTON_PLAY_DTMF_TONE("button_play_dtmf_tone");
const String GeneralSettingsFragment::BUTTON_RESPOND_VIA_SMS_KEY("button_respond_via_sms_key");

const Int32 GeneralSettingsFragment::MSG_UPDATE_RINGTONE_SUMMARY = 1;

CAR_INTERFACE_IMPL_2(GeneralSettingsFragment, PreferenceFragment, IGeneralSettingsFragment, IPreferenceOnPreferenceChangeListener);

GeneralSettingsFragment::GeneralSettingsFragment()
{
    AutoPtr<RingtoneLookupCompleteHandler> p = new RingtoneLookupCompleteHandler(this);
    mRingtoneLookupComplete = IHandler::Probe(p);
}

ECode GeneralSettingsFragment::constructor()
{
    return PreferenceFragment::constructor();
}

ECode GeneralSettingsFragment::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    PreferenceFragment::OnCreate(savedInstanceState);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext::Probe(activity)->GetApplicationContext((IContext**)&mContext);

    AddPreferencesFromResource(R::xml::general_settings);

    FindPreference(CoreUtils::Convert(BUTTON_RINGTONE_KEY), (IPreference**)&mRingtonePreference);
    AutoPtr<IPreference> preference;
    FindPreference(CoreUtils::Convert(BUTTON_VIBRATE_ON_RING), (IPreference**)&preference);
    mVibrateWhenRinging = ICheckBoxPreference::Probe(preference);
    preference = NULL;
    FindPreference(CoreUtils::Convert(BUTTON_PLAY_DTMF_TONE), (IPreference**)&preference);
    mPlayDtmfTone = ICheckBoxPreference::Probe(preference);
    FindPreference(CoreUtils::Convert(BUTTON_RESPOND_VIA_SMS_KEY), (IPreference**)&mRespondViaSms);

    preference = NULL;
    FindPreference(CoreUtils::Convert(CATEGORY_SOUNDS_KEY), (IPreference**)&preference);
    AutoPtr<IPreferenceCategory> soundCategory = IPreferenceCategory::Probe(preference);
    if (mVibrateWhenRinging != NULL) {
        AutoPtr<IInterface> service;
        mContext->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&service);
        AutoPtr<IVibrator> vibrator = IVibrator::Probe(service);
        Boolean result = FALSE;
        Boolean hasVibrator = vibrator != NULL && (vibrator->HasVibrator(&result), result);
        if (hasVibrator) {
            IPreference::Probe(mVibrateWhenRinging)->SetOnPreferenceChangeListener(
                    IPreferenceOnPreferenceChangeListener::Probe(this));
        }
        else {
            Boolean res = FALSE;
            IPreferenceGroup::Probe(soundCategory)->RemovePreference(IPreference::Probe(mVibrateWhenRinging), &res);
            mVibrateWhenRinging = NULL;
        }
    }

    if (mPlayDtmfTone != NULL) {
        IPreference::Probe(mPlayDtmfTone)->SetOnPreferenceChangeListener(
                IPreferenceOnPreferenceChangeListener::Probe(this));
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<ISettingsSystem> settingsSys;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSys);
        Int32 value;
        settingsSys->GetInt32(resolver,
                ISettingsSystem::DTMF_TONE_WHEN_DIALING, 1, &value);
        ITwoStatePreference::Probe(mPlayDtmfTone)->SetChecked(value != 0);
    }

    AutoPtr<RingtoneLookupRunnable> run = new RingtoneLookupRunnable(this);
    mRingtoneLookupRunnable = IRunnable::Probe(run);
    return NOERROR;
}

ECode GeneralSettingsFragment::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* objValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean equals;
    if (IObject::Probe(preference)->Equals(mVibrateWhenRinging, &equals), equals) {
        Boolean doVibrate = FALSE;
        IBoolean::Probe(objValue)->GetValue(&doVibrate);
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<ISettingsSystem> settingsSys;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSys);
        Boolean res = FALSE;
        settingsSys->PutInt32(resolver,
                ISettingsSystem::VIBRATE_WHEN_RINGING, doVibrate ? 1 : 0, &res);
    }
    *result = TRUE;
    return NOERROR;
}

ECode GeneralSettingsFragment::OnPreferenceTreeClick(
    /*[in]*/ IPreferenceScreen* preferenceScreen,
    /*[in]*/ IPreference* preference,
    /*[out]*/ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (Object::Equals(preference, mPlayDtmfTone)) {
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<ISettingsSystem> settingsSys;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSys);
        Boolean isChecked = FALSE, res = FALSE;
        settingsSys->PutInt32(resolver,
                ISettingsSystem::DTMF_TONE_WHEN_DIALING,
                (ITwoStatePreference::Probe(mPlayDtmfTone)->IsChecked(&isChecked), isChecked) ? 1 : 0,
                &res);
    }
    else if (Object::Equals(preference, mRespondViaSms)) {
        // Needs to return false for the intent to launch.
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode GeneralSettingsFragment::OnResume()
{
    PreferenceFragment::OnResume();

    if (mVibrateWhenRinging != NULL) {
        // TODO
        Logger::I("GeneralSettingsFragment", "TODO OnResume mVibrateWhenRinging SetChecked");
        // mVibrateWhenRinging->SetChecked(SettingsUtil::GetVibrateWhenRingingSetting(mContext));
    }

    // Lookup the ringtone name asynchronously.
    AutoPtr<IThread> thread;
    CThread::New(mRingtoneLookupRunnable, (IThread**)&thread);
    thread->Start();
    return NOERROR;
}

} // Settings
} // Dialer
} // Droid
} // Elastos
