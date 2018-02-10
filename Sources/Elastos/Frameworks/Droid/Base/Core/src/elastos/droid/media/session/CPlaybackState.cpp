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
#include "elastos/droid/media/session/CPlaybackState.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Objects.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CAR_INTERFACE_IMPL_2(CPlaybackState, Object, IPlaybackState, IParcelable)

CAR_OBJECT_IMPL(CPlaybackState)

CPlaybackState::CPlaybackState()
    : mState(0)
    , mPosition(0)
    , mBufferedPosition(0)
    , mSpeed(0)
    , mActions(0)
    , mUpdateTime(0)
    , mActiveItemId(0)
{
}

CPlaybackState::~CPlaybackState()
{
}

ECode CPlaybackState::constructor(
    /* [in] */ Int32 state,
    /* [in] */ Int64 position,
    /* [in] */ Int64 updateTime,
    /* [in] */ Float speed,
    /* [in] */ Int64 bufferedPosition,
    /* [in] */ Int64 transportControls,
    /* [in] */ IList * customActions,
    /* [in] */ Int64 activeItemId,
    /* [in] */ ICharSequence * error)
{
    mState = state;
    mPosition = position;
    mSpeed = speed;
    mUpdateTime = updateTime;
    mBufferedPosition = bufferedPosition;
    mActions = transportControls;
    AutoPtr<IArrayList> arrayList;
    CArrayList::New(ICollection::Probe(customActions), (IArrayList**)&arrayList);
    mCustomActions = IList::Probe(arrayList);
    mActiveItemId = activeItemId;
    mErrorMessage = error;
    return NOERROR;
}

ECode CPlaybackState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mState);
    source->ReadInt64(&mPosition);
    source->ReadInt64(&mBufferedPosition);
    source->ReadFloat(&mSpeed);
    source->ReadInt64(&mActions);
    source->ReadInt64(&mUpdateTime);
    source->ReadInt64(&mActiveItemId);

    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((HANDLE*)(IInterface**)&obj);
    mCustomActions = IList::Probe(obj);
    obj = NULL;
    source->ReadInterfacePtr((HANDLE*)(IInterface**)&obj);
    mErrorMessage = ICharSequence::Probe(obj);
    return NOERROR;
}

ECode CPlaybackState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mState);
    dest->WriteInt64(mPosition);
    dest->WriteInt64(mBufferedPosition);
    dest->WriteFloat(mSpeed);
    dest->WriteInt64(mActions);
    dest->WriteInt64(mUpdateTime);
    dest->WriteInt64(mActiveItemId);
    dest->WriteInterfacePtr(mCustomActions);
    dest->WriteInterfacePtr(mErrorMessage);
    return NOERROR;
}

ECode CPlaybackState::ToString(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder bob("PlaybackState {");
    bob.Append("state=");
    bob.Append(mState);
    bob.Append(", position=");
    bob.Append(mPosition);
    bob.Append(", buffered position=");
    bob.Append(mBufferedPosition);
    bob.Append(", speed=");
    bob.Append(mSpeed);
    bob.Append(", updated=");
    bob.Append(mUpdateTime);
    bob.Append(", actions=");
    bob.Append(mActions);
    bob.Append(", custom actions=");
    bob.Append(mCustomActions);
    bob.Append(", active item id=");
    bob.Append(mActiveItemId);
    bob.Append(", error=");
    bob.Append(mErrorMessage);
    bob.Append("}");
    bob.ToString(result);
    return NOERROR;
}

ECode CPlaybackState::GetState(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mState;
    return NOERROR;
}

ECode CPlaybackState::GetPosition(
    /* [out] */ Int64 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPosition;
    return NOERROR;
}

ECode CPlaybackState::GetBufferedPosition(
    /* [out] */ Int64 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBufferedPosition;
    return NOERROR;
}

ECode CPlaybackState::GetPlaybackSpeed(
    /* [out] */ Float * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSpeed;
    return NOERROR;
}

ECode CPlaybackState::GetActions(
    /* [out] */ Int64 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mActions;
    return NOERROR;
}

ECode CPlaybackState::GetCustomActions(
    /* [out] */ IList ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCustomActions;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CPlaybackState::GetErrorMessage(
    /* [out] */ ICharSequence ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mErrorMessage;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CPlaybackState::GetLastPositionUpdateTime(
    /* [out] */ Int64 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUpdateTime;
    return NOERROR;
}

ECode CPlaybackState::GetActiveQueueItemId(
    /* [out] */ Int64 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mActiveItemId;
    return NOERROR;
}

ECode CPlaybackState::GetStateFromRccState(
    /* [in] */ Int32 rccState,
    /* [out] */ Int32 * result)
{
    switch (rccState) {
        case IRemoteControlClient::PLAYSTATE_BUFFERING:
            *result = IPlaybackState::STATE_BUFFERING;
            return NOERROR;
        case IRemoteControlClient::PLAYSTATE_ERROR:
            *result = IPlaybackState::STATE_ERROR;
            return NOERROR;
        case IRemoteControlClient::PLAYSTATE_FAST_FORWARDING:
            *result = IPlaybackState::STATE_FAST_FORWARDING;
            return NOERROR;
        case IRemoteControlClient::PLAYSTATE_NONE:
            *result = IPlaybackState::STATE_NONE;
            return NOERROR;
        case IRemoteControlClient::PLAYSTATE_PAUSED:
            *result = IPlaybackState::STATE_PAUSED;
            return NOERROR;
        case IRemoteControlClient::PLAYSTATE_PLAYING:
            *result = IPlaybackState::STATE_PLAYING;
            return NOERROR;
        case IRemoteControlClient::PLAYSTATE_REWINDING:
            *result = IPlaybackState::STATE_REWINDING;
            return NOERROR;
        case IRemoteControlClient::PLAYSTATE_SKIPPING_BACKWARDS:
            *result = IPlaybackState::STATE_SKIPPING_TO_PREVIOUS;
            return NOERROR;
        case IRemoteControlClient::PLAYSTATE_SKIPPING_FORWARDS:
            *result = IPlaybackState::STATE_SKIPPING_TO_NEXT;
            return NOERROR;
        case IRemoteControlClient::PLAYSTATE_STOPPED:
            *result = IPlaybackState::STATE_STOPPED;
            return NOERROR;
        default:
            *result = -1;
            return NOERROR;
    }
}

ECode CPlaybackState::GetRccStateFromState(
    /* [in] */ Int32 state,
    /* [out] */ Int32 * result)
{
    switch (state) {
        case IPlaybackState::STATE_BUFFERING:
            *result = IRemoteControlClient::PLAYSTATE_BUFFERING;
            return NOERROR;
        case IPlaybackState::STATE_ERROR:
            *result = IRemoteControlClient::PLAYSTATE_ERROR;
            return NOERROR;
        case IPlaybackState::STATE_FAST_FORWARDING:
            *result = IRemoteControlClient::PLAYSTATE_FAST_FORWARDING;
            return NOERROR;
        case IPlaybackState::STATE_NONE:
            *result = IRemoteControlClient::PLAYSTATE_NONE;
            return NOERROR;
        case IPlaybackState::STATE_PAUSED:
            *result = IRemoteControlClient::PLAYSTATE_PAUSED;
            return NOERROR;
        case IPlaybackState::STATE_PLAYING:
            *result = IRemoteControlClient::PLAYSTATE_PLAYING;
            return NOERROR;
        case IPlaybackState::STATE_REWINDING:
            *result = IRemoteControlClient::PLAYSTATE_REWINDING;
            return NOERROR;
        case IPlaybackState::STATE_SKIPPING_TO_PREVIOUS:
            *result = IRemoteControlClient::PLAYSTATE_SKIPPING_BACKWARDS;
            return NOERROR;
        case IPlaybackState::STATE_SKIPPING_TO_NEXT:
            *result = IRemoteControlClient::PLAYSTATE_SKIPPING_FORWARDS;
            return NOERROR;
        case IPlaybackState::STATE_STOPPED:
            *result = IRemoteControlClient::PLAYSTATE_STOPPED;
            return NOERROR;
        default:
            *result = -1;
            return NOERROR;
    }
}

ECode CPlaybackState::GetActionsFromRccControlFlags(
    /* [in] */ Int32 rccFlags,
    /* [out] */ Int64 * result)
{
    VALIDATE_NOT_NULL(result)

    Int64 actions = 0;
    Int64 flag = 1;
    while (flag <= rccFlags) {
        if ((flag & rccFlags) != 0) {
            actions |= GetActionForRccFlag((Int32) flag);
        }
        flag = flag << 1;
    }
    *result = actions;
    return NOERROR;
}

ECode CPlaybackState::GetRccControlFlagsFromActions(
    /* [in] */ Int64 actions,
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)

    Int32 rccFlags = 0;
    Int64 action = 1;
    while (action <= actions && action < Elastos::Core::Math::INT32_MAX_VALUE) {
        if ((action & actions) != 0) {
            rccFlags |= GetRccFlagForAction(action);
        }
        action = action << 1;
    }
    *result = rccFlags;
    return NOERROR;
}

Int64 CPlaybackState::GetActionForRccFlag(
    /* [in] */ Int32 flag)
{
    switch (flag) {
        case IRemoteControlClient::FLAG_KEY_MEDIA_PREVIOUS:
            return IPlaybackState::ACTION_SKIP_TO_PREVIOUS;
        case IRemoteControlClient::FLAG_KEY_MEDIA_REWIND:
            return IPlaybackState::ACTION_REWIND;
        case IRemoteControlClient::FLAG_KEY_MEDIA_PLAY:
            return IPlaybackState::ACTION_PLAY;
        case IRemoteControlClient::FLAG_KEY_MEDIA_PLAY_PAUSE:
            return IPlaybackState::ACTION_PLAY_PAUSE;
        case IRemoteControlClient::FLAG_KEY_MEDIA_PAUSE:
            return IPlaybackState::ACTION_PAUSE;
        case IRemoteControlClient::FLAG_KEY_MEDIA_STOP:
            return IPlaybackState::ACTION_STOP;
        case IRemoteControlClient::FLAG_KEY_MEDIA_FAST_FORWARD:
            return IPlaybackState::ACTION_FAST_FORWARD;
        case IRemoteControlClient::FLAG_KEY_MEDIA_NEXT:
            return IPlaybackState::ACTION_SKIP_TO_NEXT;
        case IRemoteControlClient::FLAG_KEY_MEDIA_POSITION_UPDATE:
            return IPlaybackState::ACTION_SEEK_TO;
        case IRemoteControlClient::FLAG_KEY_MEDIA_RATING:
            return IPlaybackState::ACTION_SET_RATING;
    }
    return 0;
}

Int32 CPlaybackState::GetRccFlagForAction(
    /* [in] */ Int64 action)
{
    // We only care about the lower set of actions that can map to rcc
    // flags.
    Int32 testAction = action < Elastos::Core::Math::INT32_MAX_VALUE ? (Int32) action : 0;

    if (testAction == IPlaybackState::ACTION_SKIP_TO_PREVIOUS) {
        return IRemoteControlClient::FLAG_KEY_MEDIA_PREVIOUS;
    }
    if (testAction == IPlaybackState::ACTION_REWIND) {
        return IRemoteControlClient::FLAG_KEY_MEDIA_REWIND;
    }
    if (testAction == IPlaybackState::ACTION_PLAY) {
        return IRemoteControlClient::FLAG_KEY_MEDIA_PLAY;
    }
    if (testAction == IPlaybackState::ACTION_PLAY_PAUSE) {
        return IRemoteControlClient::FLAG_KEY_MEDIA_PLAY_PAUSE;
    }
    if (testAction == IPlaybackState::ACTION_PAUSE) {
        return IRemoteControlClient::FLAG_KEY_MEDIA_PAUSE;
    }
    if (testAction == IPlaybackState::ACTION_STOP) {
        return IRemoteControlClient::FLAG_KEY_MEDIA_STOP;
    }
    if (testAction == IPlaybackState::ACTION_FAST_FORWARD) {
        return IRemoteControlClient::FLAG_KEY_MEDIA_FAST_FORWARD;
    }
    if (testAction == IPlaybackState::ACTION_SKIP_TO_NEXT) {
        return IRemoteControlClient::FLAG_KEY_MEDIA_NEXT;
    }
    if (testAction == IPlaybackState::ACTION_SEEK_TO) {
        return IRemoteControlClient::FLAG_KEY_MEDIA_POSITION_UPDATE;
    }
    if (testAction == IPlaybackState::ACTION_SET_RATING) {
        return IRemoteControlClient::FLAG_KEY_MEDIA_RATING;
    }

    return 0;
}

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos
