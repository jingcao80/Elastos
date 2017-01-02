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

#include "elastos/droid/speech/tts/PlaybackSynthesisCallback.h"
#include "elastos/droid/speech/tts/BlockingAudioTrack.h"
#include "elastos/droid/speech/tts/SynthesisPlaybackQueueItem.h"
#include "elastos/droid/speech/tts/TextToSpeech.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

const String PlaybackSynthesisCallback::TAG("PlaybackSynthesisRequest");
const Boolean PlaybackSynthesisCallback::DBG = FALSE;

const Int32 PlaybackSynthesisCallback::MIN_AUDIO_BUFFER_SIZE = 8192;

PlaybackSynthesisCallback::PlaybackSynthesisCallback(
    /* [in] */ AudioOutputParams* audioParams,
    /* [in] */ AudioPlaybackHandler* audioTrackHandler,
    /* [in] */ IUtteranceProgressDispatcher* dispatcher,
    /* [in] */ IInterface* callerIdentity,
    /* [in] */ EventLoggerV1* logger,
    /* [in] */ Boolean clientIsUsingV2)
{
    mAudioParams = audioParams;
    mAudioTrackHandler = audioTrackHandler;
    mDispatcher = dispatcher;
    mCallerIdentity = callerIdentity;
    mLogger = logger;

    mStatusCode = TextToSpeech::TTS_SUCCESS;

    mDone = FALSE;
}

ECode PlaybackSynthesisCallback::Stop()
{
    if (DBG) {
        //Java:    Log.d(TAG, "stop()");
        Logger::D(TAG, String("stop()\n"));
    }

    AutoPtr<SynthesisPlaybackQueueItem> item;
    {    AutoLock syncLock(mStateLock);
        if (mDone) {
            return NOERROR;
        }

        if (mStatusCode == TextToSpeech::STOPPED) {
            //Java:    Log.w(TAG, "stop() called twice");
            Logger::W(TAG, String("stop() called twice\n"));
            return NOERROR;
        }
        item = mItem;
        mStatusCode = TextToSpeech::STOPPED;
    }

    if (item != NULL) {
        // This might result in the synthesis thread being woken up, at which
        // point it will write an additional buffer to the item - but we
        // won't worry about that because the audio playback queue will be cleared
        // soon after (see SynthHandler#stop(String).
        item->Stop(TextToSpeech::STOPPED);
    }
    else {
        // This happens when stop() or error() were called before start() was.

        // In all other cases, mAudioTrackHandler.stop() will
        // result in onSynthesisDone being called, and we will
        // write data there.
        mLogger->OnCompleted(TextToSpeech::STOPPED);
        mDispatcher->DispatchOnStop();
    }

    return NOERROR;
}

ECode PlaybackSynthesisCallback::GetMaxBufferSize(
    /* [out] */ Int32* ret)
{
    // The AudioTrack buffer will be at least MIN_AUDIO_BUFFER_SIZE, so that should always be
    // a safe buffer size to pass in.
    *ret = MIN_AUDIO_BUFFER_SIZE;

    return NOERROR;
}

ECode PlaybackSynthesisCallback::HasStarted(
    /* [out] */ Boolean* ret)
{
    {    AutoLock syncLock(mStateLock);
        if (mItem != NULL) {
            *ret = TRUE;
        } else {
            *ret = FALSE;
        }
    }

    return NOERROR;
}

ECode PlaybackSynthesisCallback::HasFinished(
    /* [out] */ Boolean* ret)
{
    {    AutoLock syncLock(mStateLock);
        *ret = mDone;
    }

    return NOERROR;
}

ECode PlaybackSynthesisCallback::Start(
    /* [in] */ Int32 sampleRateInHz,
    /* [in] */ Int32 audioFormat,
    /* [in] */ Int32 channelCount,
    /* [out] */ Int32* ret)
{
    if (DBG) {
        //Java:    Log.d(TAG, "start(" + sampleRateInHz + "," + audioFormat + "," + channelCount + ")");
        Logger::D(TAG, "start(%d, %d, %d)\n", sampleRateInHz, audioFormat, channelCount);
    }

    {    AutoLock syncLock(mStateLock);
        Int32 channelConfig = BlockingAudioTrack::GetChannelConfig(channelCount);
        if (channelConfig == 0) {
            //Java:    Log.e(TAG, "Unsupported number of channels :" + channelCount);
            Logger::E(TAG, "Unsupported number of channels: %d\n", channelCount);
            mStatusCode = TextToSpeech::ERROR_OUTPUT;
            *ret = ITextToSpeech::TTS_ERROR;
            return NOERROR;
        }

        if (mStatusCode == TextToSpeech::STOPPED) {
            if (DBG) {
                Logger::D(TAG, "stop() called before start(), returning.");
            }

            ErrorCodeOnStop(ret);
            return NOERROR;
        }
        if (mStatusCode != TextToSpeech::TTS_SUCCESS) {
            if (DBG) {
                Logger::D(TAG, "Error was raised");
            }
            *ret = TextToSpeech::TTS_ERROR;
            return NOERROR;
        }
        if (mItem != NULL) {
            Logger::E(TAG, "Start called twice");
            *ret = TextToSpeech::TTS_ERROR;
            return NOERROR;
        }

        AutoPtr<SynthesisPlaybackQueueItem> item = new SynthesisPlaybackQueueItem();
        item->constructor(mAudioParams, sampleRateInHz, audioFormat, channelCount,
                    mDispatcher, mCallerIdentity, mLogger);
        mAudioTrackHandler->Enqueue(item);
        mItem = item;
    }

    *ret = ITextToSpeech::TTS_SUCCESS;
    return NOERROR;
}

ECode PlaybackSynthesisCallback::AudioAvailable(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int32* ret)
{
    if (DBG) {
        //Java:    Log.d(TAG, "audioAvailable(byte[" + buffer.length + "]," + offset + "," + length + ")");
        Logger::D(TAG, "audioAvailable(byte[%d], %d, %d)\n", buffer->GetLength(), offset, length);
    }

    Int32 i;
    GetMaxBufferSize(&i);
    if (length > i || length <= 0) {
        //Java:    throw new IllegalArgumentException("buffer is too large or of zero length (" + length + " bytes)");
        Logger::E(TAG, "buffer is too large or of zero length (%d) bytes)\n", length);
        *ret = 0;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<SynthesisPlaybackQueueItem> item = NULL;

    {
        AutoLock lock(mStateLock);
        if (mItem == NULL) {
            mStatusCode = TextToSpeech::ERROR_OUTPUT;
            *ret = ITextToSpeech::TTS_ERROR;
            return NOERROR;
        }
        if (mStatusCode != TextToSpeech::TTS_SUCCESS) {
            if (DBG) {
                Logger::D(TAG, "Error was raised");
            }
            *ret = TextToSpeech::TTS_ERROR;
            return NOERROR;
        }
        if (mStatusCode == TextToSpeech::STOPPED) {
            ErrorCodeOnStop(ret);
            return NOERROR;
        }

        item = mItem;
    }

    // Sigh, another copy.
    AutoPtr< ArrayOf<Byte> > bufferCopy = ArrayOf<Byte>::Alloc(length);
    bufferCopy->Copy(buffer, offset, length);    //Java:    System.arraycopy(buffer, offset, bufferCopy, 0, length);

    // Might block on mItem.this, if there are too many buffers waiting to
    // be consumed.
    //try {
        item->Put(bufferCopy);
    //} catch (InterruptedException ie) {
        //return ITextToSpeech::TTS_ERROR;
    //}

    mLogger->OnEngineDataReceived();

    *ret = ITextToSpeech::TTS_SUCCESS;
    return NOERROR;
}

ECode PlaybackSynthesisCallback::Done(
    /* [out] */ Int32* ret)
{
    if (DBG) {
        //Java:    Log.d(TAG, "done()");
        Logger::D(TAG, String("done()\n") );
    }

    AutoPtr<SynthesisPlaybackQueueItem> item = NULL;

    {
        AutoLock lock(mStateLock);
        if (mDone) {
            //Java:    Log.w(TAG, "Duplicate call to done()");
            Logger::W(TAG, String("Duplicate call to done()\n") );
            *ret = ITextToSpeech::TTS_ERROR;
            return NOERROR;
        }

        mDone = TRUE;

        if (mItem == NULL) {
            *ret = ITextToSpeech::TTS_ERROR;
            return NOERROR;
        }

        item = mItem;
    }

    item->Done();
    mLogger->OnEngineComplete();

    *ret = ITextToSpeech::TTS_SUCCESS;
    return NOERROR;
}

ECode PlaybackSynthesisCallback::Error()
{
    return Error(TextToSpeech::ERROR_SYNTHESIS);
}

ECode PlaybackSynthesisCallback::Error(
    /* [in] */ Int32 errorCode)
{
    if (DBG) {
        //Java:    Log.d(TAG, "error() [will call stop]");
        Logger::D(TAG, String("error() [will call stop]\n") );
    }
    {    AutoLock syncLock(mStateLock);
        if (mDone) {
            return NOERROR;
        }
        mStatusCode = errorCode;
    }
    return NOERROR;
}

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos
