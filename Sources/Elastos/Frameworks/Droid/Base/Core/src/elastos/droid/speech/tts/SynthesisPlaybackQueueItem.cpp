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

#include "elastos/droid/speech/tts/SynthesisPlaybackQueueItem.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/speech/tts/BlockingAudioTrack.h"

using Elastos::Utility::Logging::Logger;
using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

/******************************SynthesisPlaybackQueueItem::ListEntry*************************/
SynthesisPlaybackQueueItem::ListEntry::ListEntry(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    mBytes = bytes;
}

/******************************SynthesisPlaybackQueueItem*************************/
const String SynthesisPlaybackQueueItem::TAG("TTS.SynthQueueItem");
const Boolean SynthesisPlaybackQueueItem::DBG = FALSE;

const Int64 SynthesisPlaybackQueueItem::MAX_UNCONSUMED_AUDIO_MS = 500;

CAR_INTERFACE_IMPL(SynthesisPlaybackQueueItem, Object, IRunnable);

SynthesisPlaybackQueueItem::SynthesisPlaybackQueueItem()
{}

SynthesisPlaybackQueueItem::~SynthesisPlaybackQueueItem()
{}

ECode SynthesisPlaybackQueueItem::constructor()
{
    return NOERROR;
}

ECode SynthesisPlaybackQueueItem::constructor(
    /* [in] */ AudioOutputParams* audioParams,
    /* [in] */ Int32 sampleRate,
    /* [in] */ Int32 audioFormat,
    /* [in] */ Int32 channelCount,
    /* [in] */ IUtteranceProgressDispatcher* dispatcher,
    /* [in] */ IInterface* callerIdentity,
    /* [in] */ AbstractEventLogger* logger)

{
    mUnconsumedBytes = 0;

    mStopped = FALSE;
    mDone = FALSE;
    mStatusCode = ITextToSpeech::TTS_SUCCESS;

    mAudioTrack = new BlockingAudioTrack();
    mAudioTrack->constructor(audioParams, sampleRate, audioFormat, channelCount);
    mLogger = logger;

    PlaybackQueueItem::constructor(dispatcher, callerIdentity);

    return NOERROR;
}

ECode SynthesisPlaybackQueueItem::Run()
{
    const AutoPtr<IUtteranceProgressDispatcher> dispatcher = GetDispatcher();
    dispatcher->DispatchOnStart();

    Boolean b;
    mAudioTrack->Init(&b);
    if (!b) {
        dispatcher->DispatchOnError(ITextToSpeech::ERROR_OUTPUT);
        return NOERROR;
    }

    //try {
        AutoPtr< ArrayOf<Byte> > buffer = NULL;

        // take() will block until:
        //
        // (a) there is a buffer available to tread. In which case
        // a non null value is returned.
        // OR (b) stop() is called in which case it will return null.
        // OR (c) done() is called in which case it will return null.
        while ( (buffer = Take(), buffer) != NULL) {
            Int32 i;
            mAudioTrack->Write(buffer, &i);
            mLogger->OnAudioDataWritten();
        }

    //} catch (InterruptedException ie) {
        /*
        if (DBG){
            //Java:    Log.d(TAG, "Interrupted waiting for buffers, cleaning up.");
            Logger::D(TAG, String("Interrupted waiting for buffers, cleaning up.\n"));
        }
        */
    //}

    mAudioTrack->WaitAndRelease();

    if (mStatusCode == ITextToSpeech::TTS_SUCCESS) {
        dispatcher->DispatchOnSuccess();
    }
    else if(mStatusCode == ITextToSpeech::STOPPED) {
        dispatcher->DispatchOnStop();
    }
    else {
        dispatcher->DispatchOnError(mStatusCode);
    }

    mLogger->OnCompleted(mStatusCode);

    return NOERROR;
}

ECode SynthesisPlaybackQueueItem::Stop(
    /* [in] */ Int32 errorCode)
{
#if 0
    //try {
        //Java:    mListLock.lock();
        {
            AutoLock lock(mListLock);
            // Update our internal state.
            mStopped = TRUE;
            mIsError = isError;

            // Wake up the audio playback thread if it was waiting on take().
            // take() will return null since mStopped was true, and will then
            // break out of the data write loop.
            //Java:    mReadReady.signal();

            // Wake up the synthesis thread if it was waiting on put(). Its
            // buffers will no longer be copied since mStopped is true. The
            // PlaybackSynthesisCallback that this synthesis corresponds to
            // would also have been stopped, and so all calls to
            // Callback.onDataAvailable( ) will return errors too.
            //Java:    mNotFull.signal();
        }
    //} finally {
        //Java:    mListLock.unlock();
    //}
#endif

    // Stop the underlying audio track. This will stop sending
    // data to the mixer and discard any pending buffers that the
    // track holds.
    mAudioTrack->Stop();
    return NOERROR;
}

void SynthesisPlaybackQueueItem::Done()
{
#if 0
    //try {
        //mListLock.lock();
        AutoLock lock(mListLock);

        // Update state.
        mDone = TRUE;

        // Unblocks the audio playback thread if it was waiting on take()
        // after having consumed all available buffers. It will then return
        // null and leave the write loop.
        //Java:    mReadReady.signal();

        // Just so that engines that try to queue buffers after
        // calling done() don't block the synthesis thread forever. Ideally
        // this should be called from the same thread as put() is, and hence
        // this call should be pointless.
        //Java:    mNotFull.signal();

    //} finally {
        //mListLock.unlock();
    //}
#endif
}

void SynthesisPlaybackQueueItem::Put(
    /* [in] */ ArrayOf<Byte>* buffer)// throws InterruptedException
{
    #if 0
    //try {
        //Java:    mListLock.lock();
        AutoLock lock(mListLock);
        Int64 unconsumedAudioMs = 0;

        while ((unconsumedAudioMs = mAudioTrack->GetAudioLengthMs(mUnconsumedBytes)) >
                MAX_UNCONSUMED_AUDIO_MS && !mStopped) {
            //Java:    mNotFull.await();
        }

        // Don't bother queueing the buffer if we've stopped. The playback thread
        // would have woken up when stop() is called (if it was blocked) and will
        // proceed to leave the write loop since take() will return null when
        // stopped.
        if (mStopped) {
            return;
        }

        AutoPtr<ListEntry> listEntryT = new ListEntry(buffer);

        mDataBufferList.PushBack(listEntryT);
        mUnconsumedBytes += buffer->GetLength();
        //Java:    mReadReady.signal();
    //} finally {
        //Java:    mListLock.unlock();
    //}
#endif
}

AutoPtr< ArrayOf<Byte> > SynthesisPlaybackQueueItem::Take()
{
#if 0
    //try {
        //Java:    mListLock.lock();
        AutoLock lock(mListLock);

        // Block if there are no available buffers, and stop() has not
        // been called and done() has not been called.
        while (mDataBufferList.IsEmpty() && !mStopped && !mDone) {
            //Java:    mReadReady.await();
        }

        // If stopped, return null so that we can exit the playback loop
        // as soon as possible.
        if (mStopped) {
            return NULL;
        }

            // Remove the first entry from the queue.
            AutoPtr<ListEntry> entry = mDataBufferList.GetFront();
            mDataBufferList.Remove(0);

            // This is the normal playback loop exit case, when done() was
            // called. (mDone will be true at this point).
            if (entry == NULL) {
                return NULL;
            }

            mUnconsumedBytes -= (entry->mBytes)->GetLength();
            // Unblock the waiting writer. We use signal() and not signalAll()
            // because there will only be one thread waiting on this (the
            // Synthesis thread).
            //Java:    mNotFull.signal();

            return entry->mBytes;
    //} finally {
        //Java:    mListLock.unlock();
    //}
#endif
    return NULL;
}

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos
