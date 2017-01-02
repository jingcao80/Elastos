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

#include "elastos/droid/speech/tts/SilencePlaybackQueueItem.h"
//#include "elastos/droid/os/CConditionVariable.h"

using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

CAR_INTERFACE_IMPL(SilencePlaybackQueueItem, Object, IRunnable);

SilencePlaybackQueueItem::SilencePlaybackQueueItem()
{}

SilencePlaybackQueueItem::~SilencePlaybackQueueItem()
{}

ECode SilencePlaybackQueueItem::constructor()
{
    return NOERROR;
}

ECode SilencePlaybackQueueItem::constructor(
    /* [in] */ IUtteranceProgressDispatcher* dispatcher,
    /* [in] */ IInterface* callerIdentity,
    /* [in] */ Int64 silenceDurationMs)
{
    mSilenceDurationMs = silenceDurationMs;
//    CConditionVariable::New((IConditionVariable**)&mCondVar);
    PlaybackQueueItem::constructor(dispatcher, callerIdentity);

    return NOERROR;
}

ECode SilencePlaybackQueueItem::Run()
{
    GetDispatcher()->DispatchOnStart();
    Boolean wasStopped = FALSE;
    if (mSilenceDurationMs > 0) {
//      wasStopped = mCondVar->block(mSilenceDurationMs);
    }
    if (wasStopped) {
        GetDispatcher()->DispatchOnStop();
    } else {
        GetDispatcher()->DispatchOnSuccess();
    }

    return NOERROR;
}

ECode SilencePlaybackQueueItem::Stop(
    /* [in] */ Int32 errorCode)
{
//    mCondVar->Open();
    return NOERROR;
}

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos
