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

#ifndef __ELASTOS_DROID_SPEECH_TTS_AudioPlaybackQueueItem_H__
#define __ELASTOS_DROID_SPEECH_TTS_AudioPlaybackQueueItem_H__

#include "elastos/droid/speech/tts/PlaybackQueueItem.h"
#include "elastos/droid/speech/tts/AudioOutputParams.h"
#include "Elastos.Droid.Speech.h"

using Elastos::Core::IRunnable;
using Elastos::Droid::Content::IContext;
//using Elastos::Droid::media::IMediaPlayerOnErrorListener;
using Elastos::Droid::Net::IUri;
//using Elastos::Droid::Os::IConditionVariable;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

class AudioPlaybackQueueItem
    : public PlaybackQueueItem
{
private:
    class MediaPlayerOnErrorListener
        : public Object
//        , public IMediaPlayerOnErrorListener
    {
    public:
        CAR_INTERFACE_DECL()

        MediaPlayerOnErrorListener();

        virtual ~MediaPlayerOnErrorListener();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ AudioPlaybackQueueItem* apqi);
    public:
        //@Override
        CARAPI OnError(
//            /* [in] */ IMediaPlayer* mp,
            /* [in] */ Int32 what,
            /* [in] */ Int32 extra,
            /* [out] */ Boolean* ret);
    private:
        AutoPtr<AudioPlaybackQueueItem> mApqi;
    };

    friend class MediaPlayerOnErrorListener;

private:
    class MediaPlayerOnCompletionListener
        : public Object
//        , public IMediaPlayerOnCompletionListener
    {
    public:
        CAR_INTERFACE_DECL()

        MediaPlayerOnCompletionListener();

        virtual ~MediaPlayerOnCompletionListener();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ AudioPlaybackQueueItem* apqi);
    public:
        //@Override
        CARAPI OnCompletion(
//            /* [in] */ IMediaPlayer* mp
            );

    private:
        AutoPtr<AudioPlaybackQueueItem> mApqi;
    };

    friend class MediaPlayerOnCompletionListener;

public:
    CAR_INTERFACE_DECL()

    AudioPlaybackQueueItem();

    virtual ~AudioPlaybackQueueItem();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IUtteranceProgressDispatcher* dispatcher,
        /* [in] */ IInterface* callerIdentity,
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri,
        /* [in] */ AudioOutputParams* audioParams);

    //@Override
    CARAPI Run();

private:
    CARAPI_(void) Finish();

    CARAPI_(void) SetupVolume(
//        /* [in] */ IMediaPlayer *player,
        /* [in] */ Float volume,
        /* [in] */ Float pan);

    CARAPI_(Float) Clip(
            /* [in] */ Float value,
            /* [in] */ Float min,
            /* [in] */ Float max);

    //@Override
    CARAPI Stop(
        /* [in] */ Int32 errorCode);

private:
    static const String TAG;        // = "TTS.AudioQueueItem";

    AutoPtr<IContext> mContext;
    AutoPtr<IUri> mUri;
    Int32 mStreamType;
    AudioOutputParams *mAudioParams;

//    AutoPtr<IConditionVariable> mDone;
//    AutoPtr<IMediaPlayer> mPlayer;
    volatile Boolean mFinished;
};

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_AudioPlaybackQueueItem_H__
