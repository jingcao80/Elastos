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

#include "elastos/droid/teleservice/services/telephony/EmergencyTonePlayer.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CSystemVibrator;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Media::CToneGenerator;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Media::IAudioAttributesBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

const Int32 EmergencyTonePlayer::EMERGENCY_TONE_OFF = 0;
const Int32 EmergencyTonePlayer::EMERGENCY_TONE_ALERT = 1;
const Int32 EmergencyTonePlayer::EMERGENCY_TONE_VIBRATE = 2;

const Int32 EmergencyTonePlayer::ALERT_RELATIVE_VOLUME_PERCENT = 100;

const Int32 EmergencyTonePlayer::VIBRATE_LENGTH_MILLIS = 1000;
const Int32 EmergencyTonePlayer::VIBRATE_PAUSE_MILLIS = 1000;

AutoPtr<ArrayOf<Int64> > EmergencyTonePlayer::initVIBRATE_PATTERN()
{
    AutoPtr<ArrayOf<Int64> > array = ArrayOf<Int64>::Alloc(2);

    (*array)[0] = VIBRATE_LENGTH_MILLIS;
    (*array)[1] = VIBRATE_PAUSE_MILLIS;
    return array;
}

AutoPtr<ArrayOf<Int64> > EmergencyTonePlayer::VIBRATE_PATTERN = initVIBRATE_PATTERN();

static AutoPtr<IAudioAttributes> initVIBRATION_ATTRIBUTES()
{
    AutoPtr<IAudioAttributesBuilder> builder;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&builder);
    builder->SetContentType(IAudioAttributes::CONTENT_TYPE_SPEECH);
    builder->SetUsage(IAudioAttributes::USAGE_VOICE_COMMUNICATION);
    AutoPtr<IAudioAttributes> tmp;
    builder->Build((IAudioAttributes**)&tmp);
    return tmp;
}

AutoPtr<IAudioAttributes> EmergencyTonePlayer::VIBRATION_ATTRIBUTES = initVIBRATION_ATTRIBUTES();

EmergencyTonePlayer::EmergencyTonePlayer(
    /* [in] */ IContext* context)
    : mContext(context)
    , mSavedInCallVolume(0)
    , mIsVibrating(FALSE)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    mAudioManager = IAudioManager::Probe(obj);

    CSystemVibrator::New((IVibrator**)&mVibrator);
}

ECode EmergencyTonePlayer::Start()
{
    switch (GetToneSetting()) {
        case EMERGENCY_TONE_VIBRATE:
            StartVibrate();
            break;
        case EMERGENCY_TONE_ALERT:
            // Only start if we are not in silent mode.
            Int32 ringerMode;
            mAudioManager->GetRingerMode(&ringerMode);
            if (ringerMode == IAudioManager::RINGER_MODE_NORMAL) {
                StartAlert();
            }
            break;
        case EMERGENCY_TONE_OFF:
            // nothing;
            break;
    }
    return NOERROR;
}

ECode EmergencyTonePlayer::Stop()
{
    StopVibrate();
    StopAlert();
    return NOERROR;
}

void EmergencyTonePlayer::StartVibrate()
{
    if (!mIsVibrating) {
        mVibrator->Vibrate(VIBRATE_PATTERN, 0, VIBRATION_ATTRIBUTES);
        mIsVibrating = TRUE;
    }
}

void EmergencyTonePlayer::StopVibrate()
{
    if (mIsVibrating) {
        mVibrator->Cancel();
        mIsVibrating = FALSE;
    }
}

void EmergencyTonePlayer::StartAlert()
{
    if (mToneGenerator == NULL) {
        CToneGenerator::New(
                IAudioManager::STREAM_VOICE_CALL, ALERT_RELATIVE_VOLUME_PERCENT,
                (IToneGenerator**)&mToneGenerator);

        // Set the volume to max and save the old volume setting.
        mAudioManager->GetStreamVolume(IAudioManager::STREAM_VOICE_CALL, &mSavedInCallVolume);
        Int32 maxVolume;
        mAudioManager->GetStreamMaxVolume(IAudioManager::STREAM_VOICE_CALL, &maxVolume);
        mAudioManager->SetStreamVolume(
                IAudioManager::STREAM_VOICE_CALL,
                maxVolume,
                0);
        Boolean res;
        mToneGenerator->StartTone(IToneGenerator::TONE_CDMA_EMERGENCY_RINGBACK, &res);
    }
    else {
        Logger::D("EmergencyTonePlayer", "An alert is already running.");
    }
}

void EmergencyTonePlayer::StopAlert()
{
    if (mToneGenerator != NULL) {
        mToneGenerator->StopTone();
        mToneGenerator->ReleaseResources();
        mToneGenerator = NULL;

        mAudioManager->SetStreamVolume(IAudioManager::STREAM_VOICE_CALL, mSavedInCallVolume, 0);
        mSavedInCallVolume = 0;
    }
}

Int32 EmergencyTonePlayer::GetToneSetting()
{
    AutoPtr<ISettingsGlobal> helper;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);

    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    Int32 tmp;
    helper->GetInt32(contentResolver, ISettingsGlobal::EMERGENCY_TONE, EMERGENCY_TONE_OFF, &tmp);
    return tmp;
}

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos