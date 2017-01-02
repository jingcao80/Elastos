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

#include "elastos/droid/inputmethod/pinyin/SoundManager.h"

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

AutoPtr<SoundManager> SoundManager::sInstance;

SoundManager::SoundManager(
    /* [in] */ IContext* ctx)
    : mContext(ctx)
    , FX_VOLUME(-1.0f)
    , mSilentMode(FALSE)
{
    UpdateRingerMode();
}

void SoundManager::UpdateRingerMode()
{
    if (mAudioManager == NULL) {
        AutoPtr<IInterface> audioService;
        mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&audioService);
        mAudioManager = IAudioManager::Probe(audioService);
    }
    Int32 mode = 0;
    mAudioManager->GetRingerMode(&mode);
    mSilentMode = (mode != IAudioManager::RINGER_MODE_NORMAL);
}

AutoPtr<SoundManager> SoundManager::GetInstance(
    /* [in] */ IContext* context)
{
    if (NULL == sInstance) {
        if (NULL != context) {
            sInstance = new SoundManager(context);
        }
    }
    return sInstance;
}

void SoundManager::PlayKeyDown()
{
    if (mAudioManager == NULL) {
        UpdateRingerMode();
    }
    if (!mSilentMode) {
        Int32 sound = IAudioManager::FX_KEYPRESS_STANDARD;
        mAudioManager->PlaySoundEffect(sound, FX_VOLUME);
    }
}

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos
