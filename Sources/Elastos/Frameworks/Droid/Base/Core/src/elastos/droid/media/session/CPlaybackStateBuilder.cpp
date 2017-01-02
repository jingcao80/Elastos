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

#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/media/session/CPlaybackStateBuilder.h"
#include "elastos/droid/media/session/CPlaybackStateCustomAction.h"
#include "elastos/droid/media/session/CPlaybackState.h"
#include "elastos/droid/os/SystemClock.h"

using Elastos::Droid::Os::SystemClock;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CAR_INTERFACE_IMPL(CPlaybackStateBuilder, Object, IPlaybackStateBuilder)

CAR_OBJECT_IMPL(CPlaybackStateBuilder)

CPlaybackStateBuilder::CPlaybackStateBuilder()
    : mState(0)
    , mPosition(0)
    , mBufferedPosition(0)
    , mSpeed(0)
    , mActions(0)
    , mUpdateTime(0)
    , mActiveItemId(0)
{
    CArrayList::New((IArrayList**)&mCustomActions);
}

CPlaybackStateBuilder::~CPlaybackStateBuilder()
{
}

ECode CPlaybackStateBuilder::constructor()
{
    return NOERROR;
}

ECode CPlaybackStateBuilder::constructor(
    /* [in] */ IPlaybackState * from)
{
    if (from == NULL) {
        return NOERROR;
    }
    AutoPtr<CPlaybackStateBuilder> f = (CPlaybackStateBuilder*)from;
    mState = f->mState;
    mPosition = f->mPosition;
    mBufferedPosition = f->mBufferedPosition;
    mSpeed = f->mSpeed;
    mActions = f->mActions;
    if (f->mCustomActions != NULL) {
        mCustomActions->AddAll(ICollection::Probe(f->mCustomActions));
    }
    mErrorMessage = f->mErrorMessage;
    mUpdateTime = f->mUpdateTime;
    mActiveItemId = f->mActiveItemId;
    return NOERROR;
}

ECode CPlaybackStateBuilder::SetState(
    /* [in] */ Int32 state,
    /* [in] */ Int64 position,
    /* [in] */ Float playbackSpeed,
    /* [in] */ Int64 updateTime)
{
    mState = state;
    mPosition = position;
    mUpdateTime = updateTime;
    mSpeed = playbackSpeed;
    return NOERROR;
}

ECode CPlaybackStateBuilder::SetState(
    /* [in] */ Int32 state,
    /* [in] */ Int64 position,
    /* [in] */ Float playbackSpeed)
{
    return SetState(state, position, playbackSpeed, SystemClock::GetElapsedRealtime());
}

ECode CPlaybackStateBuilder::SetActions(
    /* [in] */ Int64 actions)
{
    mActions = actions;
    return NOERROR;
}

ECode CPlaybackStateBuilder::AddCustomAction(
    /* [in] */ const String& action,
    /* [in] */ const String& name,
    /* [in] */ Int32 icon)
{
    AutoPtr<ICharSequence> csq;
    CString::New(name, (ICharSequence**)&csq);
    AutoPtr<IPlaybackStateCustomAction> customAction;
    CPlaybackStateCustomAction::New(action, csq, icon, NULL
        , (IPlaybackStateCustomAction**)&customAction);
    return AddCustomAction(customAction);
}

ECode CPlaybackStateBuilder::AddCustomAction(
    /* [in] */ IPlaybackStateCustomAction * customAction)
{
    if (customAction == NULL) {
        // throw new IllegalArgumentException(
        //         "You may not add a null CustomAction to PlaybackState.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mCustomActions->Add(customAction);
    return NOERROR;
}

ECode CPlaybackStateBuilder::SetBufferedPosition(
    /* [in] */ Int64 bufferedPosition)
{
    mBufferedPosition = bufferedPosition;
    return NOERROR;
}

ECode CPlaybackStateBuilder::SetActiveQueueItemId(
    /* [in] */ Int64 id)
{
    mActiveItemId = id;
    return NOERROR;
}

ECode CPlaybackStateBuilder::SetErrorMessage(
    /* [in] */ ICharSequence * error)
{
    mErrorMessage = error;
    return NOERROR;
}

ECode CPlaybackStateBuilder::Build(
    /* [out] */ IPlaybackState ** result)
{
    VALIDATE_NOT_NULL(result)
    return CPlaybackState::New(mState, mPosition, mUpdateTime, mSpeed, mBufferedPosition,
            mActions, IList::Probe(mCustomActions), mActiveItemId, mErrorMessage, result);
}

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos
