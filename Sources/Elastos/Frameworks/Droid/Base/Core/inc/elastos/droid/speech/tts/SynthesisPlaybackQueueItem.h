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

#ifndef __ELASTOS_DROID_SPEECH_TTS_SynthesisPlaybackQueueItem_H__
#define __ELASTOS_DROID_SPEECH_TTS_SynthesisPlaybackQueueItem_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/core/Object.h"
#include "elastos/droid/speech/tts/PlaybackQueueItem.h"
#include "elastos/droid/speech/tts/BlockingAudioTrack.h"
#include "elastos/droid/speech/tts/AbstractEventLogger.h"
#include "elastos/droid/speech/tts/AudioOutputParams.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

/**
 * Manages the playback of a list of byte arrays representing audio data
 * that are queued by the engine to an audio track.
 */
//final
class SynthesisPlaybackQueueItem
    : public PlaybackQueueItem
{
public:
    //static final
    class ListEntry
        : public Object
    {
    public:
        //final
        CARAPI_(AutoPtr< ArrayOf<Byte> >) mBytes;
    public:
        ListEntry(
            /* [in] */ ArrayOf<Byte>* bytes);
    };

public:
    CAR_INTERFACE_DECL()

    SynthesisPlaybackQueueItem();

    virtual ~SynthesisPlaybackQueueItem();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ AudioOutputParams* audioParams,
        /* [in] */ Int32 sampleRate,
        /* [in] */ Int32 audioFormat,
        /* [in] */ Int32 channelCount,
        /* [in] */ IUtteranceProgressDispatcher* dispatcher,
        /* [in] */ IInterface* callerIdentity,
        /* [in] */ AbstractEventLogger* logger);

    //@Override
    CARAPI Run();

    //@Override
    CARAPI Stop(
        /* [in] */ Int32 errorCode);

    CARAPI_(void) Done();

    CARAPI_(void) Put(
        /* [in] */ ArrayOf<Byte>* buffer);      // throws InterruptedException

private:
    AutoPtr<ArrayOf<Byte> > Take();             // throws InterruptedException

private:
    static const String TAG;        // = "TTS.SynthQueueItem";
    static const Boolean DBG;       // = FALSE;

    /**
     * Maximum length of audio we leave unconsumed by the audio track.
     * Calls to {@link #put(byte[])} will block until we have less than
     * this amount of audio left to play back.
     */
    static const Int64 MAX_UNCONSUMED_AUDIO_MS;     // = 500;

    /**
     * Guards accesses to mDataBufferList and mUnconsumedBytes.
     */
    //private final Lock mListLock = new ReentrantLock();
    //private final Condition mReadReady = mListLock.newCondition();
    //private final Condition mNotFull = mListLock.newCondition();
    Object mListLock;

    // Guarded by mListLock.
    List<AutoPtr<ListEntry> > mDataBufferList;
    // Guarded by mListLock.
    Int32 mUnconsumedBytes;// = 0;

    /*
     * While mStopped and mIsError can be written from any thread, mDone is written
     * only from the synthesis thread. All three variables are read from the
     * audio playback thread.
     */
    /* volatile */ Boolean mStopped;
    /* volatile */ Boolean mDone;
    /* volatile */ Int32 mStatusCode;

    AutoPtr<BlockingAudioTrack> mAudioTrack;
    AutoPtr<AbstractEventLogger> mLogger;
};

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_SynthesisPlaybackQueueItem_H__
