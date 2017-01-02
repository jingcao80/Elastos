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

#ifndef __ELASTOS_DROID_SPEECH_TTS_SilencePlaybackQueueItem_H__
#define __ELASTOS_DROID_SPEECH_TTS_SilencePlaybackQueueItem_H__

#include "elastos/droid/speech/tts/PlaybackQueueItem.h"
#include "elastos/droid/os/ConditionVariable.h"
//
namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

class SilencePlaybackQueueItem
    : public PlaybackQueueItem
{
public:
    CAR_INTERFACE_DECL()

    SilencePlaybackQueueItem();

    virtual ~SilencePlaybackQueueItem();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IUtteranceProgressDispatcher* dispatcher,
        /* [in] */ IInterface* callerIdentity,
        /* [in] */ Int64 silenceDurationMs);

    //@Override
    CARAPI Run();

    //@Override
    CARAPI Stop(
        /* [in] */ Int32 errorCode);

private:
//    AutoPtr<IConditionVariable> mCondVar;// = new ConditionVariable();
    Int64 mSilenceDurationMs;
};

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_SilencePlaybackQueueItem_H__
