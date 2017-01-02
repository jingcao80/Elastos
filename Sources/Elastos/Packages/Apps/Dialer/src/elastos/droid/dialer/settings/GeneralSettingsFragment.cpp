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

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Core::ICharSequence;
using Elastos::Core::IThread;
using Elastos::Core::CThread;

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
        case MSG_UPDATE_RINGTONE_SUMMARY:
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->mRingtonePreference->SetSummary(ICharSequence::Probe(obj));
            break;
    }
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
        assert(0 && "TODO");
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

CAR_INTERFACE_IMPL(GeneralSettingsFragment, PreferenceFragment,
        IGeneralSettingsFragment, IPreferenceOnPreferenceChangeListener);

GeneralSettingsFragment::GeneralSettingsFragment()
{
    mRingtoneLookupComplete = (IHandler*)new RingtoneLookupCompleteHandler(this);
}

ECode GeneralSettingsFragment::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    PreferenceFragment::OnCreate(savedInstanceState);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    activity->GetApplicationContext((IContext**)&mContext);

    AddPreferencesFromResource(R::xml::general_settings);

    FindPreference(BUTTON_RINGTONE_KEY, (IPreference**)&mRingtonePreference);
    AutoPtr<IPreference> preference;
    FindPreference(BUTTON_VIBRATE_ON_RING, (IPreference**)&preference);
    mVibrateWhenRinging = ICheckBoxPreference::Probe(preference);
    preference = NULL;
    FindPreference(BUTTON_PLAY_DTMF_TONE, (IPreference**)&preference);
    mPlayDtmfTone = ICheckBoxPreference::Probe(preference);
    FindPreference(BUTTON_RESPOND_VIA_SMS_KEY, (IPreference**)&mRespondViaSms);

    AutoPtr<IPreference> soundCategory;
    FindPreference(CATEGORY_SOUNDS_KEY, (IPreference**)&soundCategory);
    if (mVibrateWhenRinging != NULL) {
        AutoPtr<IInterface> service;
        mContext->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&service);
        IVibrator* vibrator = IVibrator::Probe(service);
        Boolean result;
        if (vibrator != NULL && vibrator->HasVibrator(&result), result) {
            mVibrateWhenRinging->SetOnPreferenceChangeListener(
                    (IPreferenceOnPreferenceChangeListener*)this);
        }
        else {
            IPreferenceCategory::Probe(soundCategory)->RemovePreference(mVibrateWhenRinging);
            mVibrateWhenRinging = NULL;
        }
    }

    if (mPlayDtmfTone != NULL) {
        mPlayDtmfTone->SetOnPreferenceChangeListener(
                (IPreferenceOnPreferenceChangeListener*)this);
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<ISettingsSystem> settingsSys;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSys);
        Int32 value;
        settingsSys->GetInt32(resolver,
                ISettingsSystem::DTMF_TONE_WHEN_DIALING, 1, &value);
        mPlayDtmfTone->SetChecked(value != 0);
    }

    mRingtoneLookupRunnable = (IRunnable*)new RingtoneLookupRunnable(this);
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
        Boolean doVibrate = CoreUtils::Unbox(objValue);
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<ISettingsSystem> settingsSys;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSys);
        settingsSys->PutInt32(resolver,
                ISettingsSystem::VIBRATE_WHEN_RINGING, doVibrate ? 1 : 0);
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
    if (preference == mPlayDtmfTone) {
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<ISettingsSystem> settingsSys;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSys);
        Boolean isChecked;
        settingsSys->PutInt32(resolver,
                ISettingsSystem::DTMF_TONE_WHEN_DIALING,
                mPlayDtmfTone->IsChecked(&isChecked), isChecked ? 1 : 0);
    }
    else if (preference == mRespondViaSms) {
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
        assert(0 && "TODO");
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
