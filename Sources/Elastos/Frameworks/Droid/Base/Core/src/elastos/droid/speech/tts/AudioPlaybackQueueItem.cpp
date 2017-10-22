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

#include "elastos/droid/speech/tts/AudioPlaybackQueueItem.h"
//#include "elastos/droid/os/CConditionVariable.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringUtils.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"

using Elastos::Core::StringUtils;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Speech::Tts::IUtteranceProgressDispatcher;
using Elastos::Droid::Media::IMetadata;
//using Elastos::Droid::Media::CMediaPlayer;
//using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IMediaPlayerOnPreparedListener;
using Elastos::Droid::Media::IMediaPlayerOnCompletionListener;
using Elastos::Droid::Media::IMediaPlayerOnErrorListener;
using Elastos::Droid::Media::IMediaPlayerOnInfoListener;
using Elastos::Droid::Media::IMediaPlayerOnVideoSizeChangedListener;
using Elastos::Droid::Media::IMediaPlayerOnBufferingUpdateListener;
using Elastos::Droid::Media::EIID_IMediaPlayerOnPreparedListener;
using Elastos::Droid::Media::EIID_IMediaPlayerOnCompletionListener;
using Elastos::Droid::Media::EIID_IMediaPlayerOnErrorListener;
using Elastos::Droid::Media::EIID_IMediaPlayerOnInfoListener;
using Elastos::Droid::Media::EIID_IMediaPlayerOnVideoSizeChangedListener;
using Elastos::Droid::Media::EIID_IMediaPlayerOnBufferingUpdateListener;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

/******************
 * AudioPlaybackQueueItem::MediaPlayerOnErrorListener
 *******************************************************************************************************/

//CAR_INTERFACE_IMPL(AudioPlaybackQueueItem::MediaPlayerOnErrorListener, Object, IMediaPlayerOnErrorListener);

AudioPlaybackQueueItem::MediaPlayerOnErrorListener::MediaPlayerOnErrorListener()
{}

AudioPlaybackQueueItem::MediaPlayerOnErrorListener::~MediaPlayerOnErrorListener()
{}

ECode AudioPlaybackQueueItem::MediaPlayerOnErrorListener::constructor()
{
    return NOERROR;
}

ECode AudioPlaybackQueueItem::MediaPlayerOnErrorListener::constructor(
    /* [in] */ AudioPlaybackQueueItem* apqi)
{
    mApqi = apqi;

    return NOERROR;
}

ECode AudioPlaybackQueueItem::MediaPlayerOnErrorListener::OnError(
//    /* [in] */ IMediaPlayer* mp,
    /* [in] */ Int32 what,
    /* [in] */ Int32 extra,
    /* [out] */ Boolean* ret)
{
    //Java:    Log.w(TAG, "Audio playback error: " + what + ", " + extra);
    Logger::W(AudioPlaybackQueueItem::TAG, "Audio playback error: what:%d, extra:%d\n", what, extra);
//    (mApqi->mDone)->Open();
    *ret = TRUE;
    return NOERROR;
}

/******************
 * AudioPlaybackQueueItem::MediaPlayerOnCompletionListener
 *******************************************************************************************************/

//CAR_INTERFACE_IMPL(AudioPlaybackQueueItem::MediaPlayerOnCompletionListener, Object, IMediaPlayerOnCompletionListener);

AudioPlaybackQueueItem::MediaPlayerOnCompletionListener::MediaPlayerOnCompletionListener()
{}

AudioPlaybackQueueItem::MediaPlayerOnCompletionListener::~MediaPlayerOnCompletionListener()
{}

ECode AudioPlaybackQueueItem::MediaPlayerOnCompletionListener::constructor()
{
    return NOERROR;
}

ECode AudioPlaybackQueueItem::MediaPlayerOnCompletionListener::constructor(
    /* [in] */ AudioPlaybackQueueItem* apqi)
{
    mApqi = apqi;
    return NOERROR;
}

ECode AudioPlaybackQueueItem::MediaPlayerOnCompletionListener::OnCompletion(
//    /* [in] */ IMediaPlayer* mp
)
{
    mApqi->mFinished = TRUE;
//    (mApqi->mDone)->Open();
    return NOERROR;
}

/******************
 * AudioPlaybackQueueItem
 *******************************************************************************************************/

const String AudioPlaybackQueueItem::TAG("TTS.AudioQueueItem");

CAR_INTERFACE_IMPL(AudioPlaybackQueueItem, Object, IRunnable);

AudioPlaybackQueueItem::AudioPlaybackQueueItem()
{}

AudioPlaybackQueueItem::~AudioPlaybackQueueItem()
{}

ECode AudioPlaybackQueueItem::constructor()
{
    return NOERROR;
}

ECode AudioPlaybackQueueItem::constructor(
    /* [in] */ IUtteranceProgressDispatcher* dispatcher,
    /* [in] */ IInterface* callerIdentity,
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri,
    /* [in] */ AudioOutputParams *audioParams)
{
    mContext = context;
    mUri = uri;
    mAudioParams = audioParams;

    assert(0 && "TODO");
//    CConditionVariable::New((IConditionVariable**)&mDone);
//    mPlayer = NULL;
    mFinished = FALSE;
    PlaybackQueueItem::constructor(dispatcher, callerIdentity);

    return NOERROR;
}

ECode AudioPlaybackQueueItem::Run()
{
    AutoPtr<IUtteranceProgressDispatcher> dispatcher = GetDispatcher();

    Int32 sessionId = mAudioParams->mSessionId;

    dispatcher->DispatchOnStart();
    // TODO:
//    mPlayer = CMediaPlayer::Create(
//                mContext, mUri, null, mAudioParams.mAudioAttributes,
//                sessionId > 0 ? sessionId : AudioSystem.AUDIO_SESSION_ALLOCATE);
    if (/*mPlayer == NULL*/FALSE) {
        dispatcher->DispatchOnError(ITextToSpeech::ERROR_OUTPUT);
        return E_INVALID_PARAMETER_EXCEPTION;
    }

    AutoPtr<MediaPlayerOnErrorListener> mpoer = new MediaPlayerOnErrorListener();
//        mPlayer->SetOnErrorListener(mpoer);
    AutoPtr<MediaPlayerOnCompletionListener> mpocl = new MediaPlayerOnCompletionListener();

    if (mpoer == NULL || mpocl == NULL) {
        Logger::W(TAG, String("MediaPlayer failed\n"));
//        mDone->Open();

        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

//    SetupVolume(mPlayer, mAudioParams->mVolume, mAudioParams->mPan);
//    mPlayer->SetOnCompletionListener(mpocl);
//    mPlayer->SetAudioStreamType(mStreamType);
//    mPlayer->Start();
//    mDone->Block();
    Finish();

    if (mFinished) {
        dispatcher->DispatchOnSuccess();
    } else {
        dispatcher->DispatchOnStop();
    }
    return NOERROR;
}

void AudioPlaybackQueueItem::SetupVolume(
//    /* [in] */ IMediaPlayer *player,
    /* [in] */ Float volume,
    /* [in] */ Float pan)
{
    Float vol = Clip(volume, 0.0f, 1.0f);
    Float panning = Clip(pan, -1.0f, 1.0f);

    Float volLeft = vol, volRight = vol;
    if (panning > 0.0f) {
        volLeft *= (1.0f - panning);
    } else if (panning < 0.0f) {
        volRight *= (1.0f + panning);
    }
//    player->SetVolume(volLeft, volRight);
}

Float AudioPlaybackQueueItem::Clip(
    /* [in] */ Float value,
    /* [in] */ Float min,
    /* [in] */ Float max)
{
    return value < min ? min : (value < max ? value : max);
}


void AudioPlaybackQueueItem::Finish()
{
    assert(0 && "TODO");
    //try {
//        mPlayer->Stop();
    //} catch (IllegalStateException ex) {
        // Do nothing, the player is already stopped
    //}
//    mPlayer->Release();
}

ECode AudioPlaybackQueueItem::Stop(
    /* [in] */ Int32 errorCode)
{
    assert(0 && "TODO");
//    mDone->Open();
    return NOERROR;
}

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos
