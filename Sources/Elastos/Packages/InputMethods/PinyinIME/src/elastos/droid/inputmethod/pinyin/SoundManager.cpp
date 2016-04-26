
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
