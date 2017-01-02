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

#ifndef __ELASTOS_DROID_SPEECH_TTS_PlaybackQueueItem_H__
#define __ELASTOS_DROID_SPEECH_TTS_PlaybackQueueItem_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/core/Object.h"
#include "Elastos.Droid.Speech.h"

using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

//abstract
class PlaybackQueueItem
    : public Object
    , public IRunnable
{
public:
    CAR_INTERFACE_DECL()

    PlaybackQueueItem();

    virtual ~PlaybackQueueItem();

    CARAPI constructor(
        /* [in] */ IUtteranceProgressDispatcher* dispatcher,
        /* [in] */ IInterface* callerIdentity);

    CARAPI_(AutoPtr<IInterface>) GetCallerIdentity();

public:
    virtual CARAPI Run() = 0;

    /**
     * Stop the playback.
     *
     * @param errorCode Cause of the stop. Can be either one of the error codes from
     *         {@link android.speech.tts.TextToSpeech} or
     *         {@link android.speech.tts.TextToSpeech#STOPPED}
     *         if stopped on a client request.
     */
     virtual CARAPI Stop(
        /* [in] */ Int32 errorCode) = 0;

protected:
    CARAPI_(AutoPtr<IUtteranceProgressDispatcher>) GetDispatcher();

private:
    AutoPtr<IUtteranceProgressDispatcher> mDispatcher;
    AutoPtr<IInterface> mCallerIdentity;
};

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_PlaybackQueueItem_H__
