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

#include "elastos/droid/speech/tts/AudioPlaybackHandler.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Core::CThread;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

const String AudioPlaybackHandler::TAG("TTS.AudioPlaybackHandler");
const Boolean AudioPlaybackHandler::DBG = FALSE;

/******************
 * AudioPlaybackHandler::AudioPlaybackHandlerMessageLoop
 *******************************************************************************************************/

CAR_INTERFACE_IMPL(AudioPlaybackHandler::AudioPlaybackHandlerMessageLoop, Object, IRunnable);

AudioPlaybackHandler::AudioPlaybackHandlerMessageLoop::AudioPlaybackHandlerMessageLoop()
{}

AudioPlaybackHandler::AudioPlaybackHandlerMessageLoop::~AudioPlaybackHandlerMessageLoop()
{}

ECode AudioPlaybackHandler::AudioPlaybackHandlerMessageLoop::constructor()
{
    return NOERROR;
}

ECode AudioPlaybackHandler::AudioPlaybackHandlerMessageLoop::constructor(
    /* [in] */ AudioPlaybackHandler* aph)
{
    mAph = aph;
    return NOERROR;
}

ECode AudioPlaybackHandler::AudioPlaybackHandlerMessageLoop::Run()
{
    while (TRUE) {
        AutoPtr<PlaybackQueueItem> item = NULL;
        //try {
            item = (mAph->mQueue).GetFront();
            //(mAph->mQueue).Pop();
            (mAph->mQueue).PopFront();
        //} catch (InterruptedException ie) {
            /*
            if (DBG){
                //Java:    Log.d(TAG, "MessageLoop : Shutting down (interrupted)");
                Logger::D(TAG, String("MessageLoop : Shutting down (interrupted)\n"));
            }
            return E_INTERRUPTED_EXCEPTION;
            */
        //}

        // If stop() or stopForApp() are called between mQueue.take()
        // returning and mCurrentWorkItem being set, the current work item
        // will be run anyway.

        mAph->mCurrentWorkItem = item;
        item->Run();
        mAph->mCurrentWorkItem = NULL;
    }

    return NOERROR;
}

/******************************AudioPlaybackHandler*************************/

AudioPlaybackHandler::AudioPlaybackHandler()
{
    //Java:    mHandlerThread = new Thread(new MessageLoop(), "TTS.AudioPlaybackThread");
    AutoPtr<AudioPlaybackHandlerMessageLoop> aphMl = new AudioPlaybackHandlerMessageLoop();
    aphMl->constructor(this);
    CThread::New(aphMl, String("TTS.AudioPlaybackThread"), (IThread**)&mHandlerThread);
}

void AudioPlaybackHandler::Start()
{
    mHandlerThread->Start();
}

void AudioPlaybackHandler::Stop(
    /* [in] */ PlaybackQueueItem* item)
{
    if (item == NULL) {
        return;
    }

    item->Stop(ITextToSpeech::STOPPED);
}

void AudioPlaybackHandler::Enqueue(
    /* [in] */ PlaybackQueueItem* item)
{
    //try {
        //mQueue.Push(item);
        mQueue.PushBack(item);
    //} catch (InterruptedException ie) {
        // This exception will never be thrown, since we allow our queue
        // to be have an unbounded size. put() will therefore never block.
    //}
}

void AudioPlaybackHandler::StopForApp(
    /* [in] */ IInterface* callerIdentity)
{
    if (DBG){
        //Java:    Log.d(TAG, "Removing all callback items for : " + callerIdentity);
        Logger::D(TAG, "Removing all callback items for : \n");
    }
    RemoveWorkItemsFor(callerIdentity);

    const AutoPtr<PlaybackQueueItem> current = mCurrentWorkItem;
    if (current != NULL && ((current->GetCallerIdentity()).Get() == callerIdentity)) {
        Stop(current);
    }
}

void AudioPlaybackHandler::Stop()
{
    if (DBG){
        //Java:    Log.d(TAG, "Stopping all items");
        Logger::D(TAG, "Stopping all items\n");
    }
    RemoveAllMessages();

    Stop(mCurrentWorkItem);
}

Boolean AudioPlaybackHandler::IsSpeaking()
{
    return (mQueue.GetFront() != NULL) || (mCurrentWorkItem != NULL);
}

void AudioPlaybackHandler::Quit()
{
    RemoveAllMessages();
    Stop(mCurrentWorkItem);
    mHandlerThread->Interrupt();
}

void AudioPlaybackHandler::RemoveAllMessages()
{
    /*
    while( mQueue.IsEmpty() == FALSE )
    {
        mQueue.Pop();
    }
    */
    mQueue.Clear();
}

void AudioPlaybackHandler::RemoveWorkItemsFor(
    /* [in] */ IInterface* callerIdentity)
{
    Int32 queueLen = mQueue.GetSize();
    for(Int32 i = 0; i<queueLen; i++) {
        AutoPtr<PlaybackQueueItem> item = mQueue.GetFront();
        AutoPtr<IInterface> ci;
        ci = item->GetCallerIdentity();
        if (ci.Get() != callerIdentity) {
            //mQueue.Push(item);
            mQueue.PushBack(item);
        }
        //mQueue.Pop();
        mQueue.PopFront();
    }
}

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos
