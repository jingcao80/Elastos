
#include "elastos/droid/media/CMediaActionSound.h"
#include "elastos/droid/media/CSoundPool.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using namespace Elastos::Core;

namespace Elastos {
namespace Droid {
namespace Media {

const String CMediaActionSound::TAG("MediaActionSound");
const String CMediaActionSound::SOUND_FILES[] = {
    String("/system/media/audio/ui/camera_click.ogg"),
    String("/system/media/audio/ui/camera_focus.ogg"),
    String("/system/media/audio/ui/VideoRecord.ogg"),
    String("/system/media/audio/ui/VideoRecord.ogg")
};

const Int32 CMediaActionSound::NUM_MEDIA_SOUND_STREAMS;
const Int32 CMediaActionSound::SOUND_NOT_LOADED;

//=====================================================================
//          CMediaActionSound::SoundPoolOnLoadCompleteListener
//=====================================================================
CAR_INTERFACE_IMPL(CMediaActionSound::SoundPoolOnLoadCompleteListener, Object, ISoundPoolOnLoadCompleteListener)

CMediaActionSound::SoundPoolOnLoadCompleteListener::SoundPoolOnLoadCompleteListener(
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{
}

ECode CMediaActionSound::SoundPoolOnLoadCompleteListener::OnLoadComplete(
    /* [in] */ ISoundPool* soundPool,
    /* [in] */ Int32 sampleId,
    /* [in] */ Int32 status)
{
    if (status == 0) {
        AutoPtr<IMediaActionSound> strongObj;
        mWeakHost->Resolve(EIID_IMediaActionSound, (IInterface**)&strongObj);
        if (strongObj == NULL) {
            return NOERROR;
        }

        CMediaActionSound* mHost = (CMediaActionSound*)strongObj.Get();

        if (mHost->mSoundIdToPlay == sampleId) {
            Int32 result;
            soundPool->Play(sampleId, 1.0f, 1.0f, 0, 0, 1.0f, &result);
            mHost->mSoundIdToPlay = CMediaActionSound::SOUND_NOT_LOADED;
        }
    } else {
        Logger::E(CMediaActionSound::TAG,
            "Unable to load sound for playback (status: %d)", status);
    }
    return NOERROR;
}

//=====================================================================
//          CMediaActionSound
//=====================================================================

CAR_INTERFACE_IMPL(CMediaActionSound, Object, IMediaActionSound)

CAR_OBJECT_IMPL(CMediaActionSound)

CMediaActionSound::CMediaActionSound()
    : mSoundIdToPlay(0)
{
}

CMediaActionSound::~CMediaActionSound()
{
}

ECode CMediaActionSound::constructor()
{
    AutoPtr<IWeakReference> weakHost;
    GetWeakReference((IWeakReference**)&weakHost);
    mLoadCompleteListener = new SoundPoolOnLoadCompleteListener(weakHost);

    CSoundPool::New(NUM_MEDIA_SOUND_STREAMS, IAudioManager::STREAM_SYSTEM_ENFORCED, 0, (ISoundPool**)&mSoundPool);
    mSoundPool->SetOnLoadCompleteListener(mLoadCompleteListener);

    mSoundIds = ArrayOf<Int32>::Alloc(sizeof(SOUND_FILES));
    for (Int32 i = 0; i < mSoundIds->GetLength(); i++) {
        mSoundIds->Set(i, SOUND_NOT_LOADED);
    }
    mSoundIdToPlay = SOUND_NOT_LOADED;
    return NOERROR;
}

ECode CMediaActionSound::Load(
    /* [in] */ Int32 soundName)
{
    if (soundName < 0 || soundName >= (Int32)sizeof(SOUND_FILES)) {
        Logger::E(TAG, "Unknown sound requested: %d", soundName);
        return E_RUNTIME_EXCEPTION;
    }

    if ((*mSoundIds)[soundName] == SOUND_NOT_LOADED) {
        mSoundPool->Load(SOUND_FILES[soundName], 1, &(*mSoundIds)[soundName]);
    }
    return NOERROR;
}

ECode CMediaActionSound::Play(
    /* [in] */ Int32 soundName)
{
    if (soundName < 0 || soundName >= (Int32)sizeof(SOUND_FILES)) {
        Logger::E(TAG, "Unknown sound requested: %d", soundName);
        return E_RUNTIME_EXCEPTION;
    }

    if ((*mSoundIds)[soundName] == SOUND_NOT_LOADED) {
        mSoundPool->Load(SOUND_FILES[soundName], 1, &mSoundIdToPlay);
        (*mSoundIds)[soundName] = mSoundIdToPlay;
    }
    else {
        Int32 val;
        mSoundPool->Play((*mSoundIds)[soundName], 1.0f, 1.0f, 0, 0, 1.0f, &val);
    }
    return NOERROR;
}

ECode CMediaActionSound::ReleaseResources()
{
    if (mSoundPool != NULL) {
        mSoundPool->ReleaseResources();
        mSoundPool = NULL;
    }
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
