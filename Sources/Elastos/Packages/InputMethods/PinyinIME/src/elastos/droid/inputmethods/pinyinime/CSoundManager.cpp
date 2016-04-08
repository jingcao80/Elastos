
#include "CSoundManager.h"

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

CAR_OBJECT_IMPL(CSoundManager);
CAR_INTERFACE_IMPL(CSoundManager, Object, ISoundManager);

CSoundManager::CSoundManager()
    : FX_VOLUME(-1.0f)
    , mSilentMode(FALSE)
{
}

ECode CSoundManager::constructor(
    /* [in] */ IContext* ctx)
{
    mContext = ctx;
    return UpdateRingerMode();
}

ECode CSoundManager::UpdateRingerMode()
{
    if (mAudioManager == NULL) {
        AutoPtr<IInterface> audioService;
        mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&audioService);
        mAudioManager = IAudioManager::Probe(audioService);
    }
    assert(mAudioManager != NULL);
    Int32 mode = 0;
    mAudioManager->GetRingerMode(&mode);
    mSilentMode = (mode != IAudioManager::RINGER_MODE_NORMAL);
    return NOERROR;
}

ECode CSoundManager::PlayKeyDown()
{
    if (mAudioManager == NULL) {
        UpdateRingerMode();
    }
    if (!mSilentMode) {
        Int32 sound = IAudioManager::FX_KEYPRESS_STANDARD;
        mAudioManager->PlaySoundEffect(sound, FX_VOLUME);
    }
    return NOERROR;
}

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos
