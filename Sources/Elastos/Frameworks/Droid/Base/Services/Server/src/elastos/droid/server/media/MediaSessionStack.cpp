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

#include "elastos/droid/server/media/MediaSessionStack.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Media::Session::IPlaybackState;
using Elastos::Droid::Media::Session::IMediaSession;
using Elastos::Droid::Media::Session::IMediaSessionHelper;
using Elastos::Droid::Media::Session::CMediaSessionHelper;
using Elastos::Droid::Media::Session::IMediaControllerPlaybackInfo;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

static AutoPtr<ArrayOf<Int32> > InitAlwaysPriorityStates()
{
    AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(4);
    (*states)[0] = IPlaybackState::STATE_FAST_FORWARDING;
    (*states)[1] = IPlaybackState::STATE_REWINDING;
    (*states)[2] = IPlaybackState::STATE_SKIPPING_TO_PREVIOUS;
    (*states)[3] = IPlaybackState::STATE_SKIPPING_TO_NEXT;
    return states;
}
const AutoPtr<ArrayOf<Int32> > MediaSessionStack::ALWAYS_PRIORITY_STATES = InitAlwaysPriorityStates();

static AutoPtr<ArrayOf<Int32> > InitTransitionPriorityStates()
{
    AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(3);
    (*states)[0] = IPlaybackState::STATE_BUFFERING;
    (*states)[1] = IPlaybackState::STATE_CONNECTING;
    (*states)[2] = IPlaybackState::STATE_PLAYING;
    return states;
}
const AutoPtr<ArrayOf<Int32> > MediaSessionStack::TRANSITION_PRIORITY_STATES = InitTransitionPriorityStates();

void MediaSessionStack::AddSession(
    /* [in] */ MediaSessionRecord* record)
{
    AutoPtr<MediaSessionRecord> msr(record);
    mSessions.PushBack(msr);
    ClearCache();
}

void MediaSessionStack::RemoveSession(
    /* [in] */ MediaSessionRecord* record)
{
    AutoPtr<MediaSessionRecord> msr(record);
    mSessions.Remove(msr);
    if (record == mGlobalPrioritySession.Get()) {
        mGlobalPrioritySession = NULL;
    }
    ClearCache();
}

Boolean MediaSessionStack::OnPlaystateChange(
    /* [in] */ MediaSessionRecord* record,
    /* [in] */ Int32 oldState,
    /* [in] */ Int32 newState)
{
    if (ShouldUpdatePriority(oldState, newState)) {
        AutoPtr<MediaSessionRecord> msr(record);
        mSessions.Remove(msr);
        mSessions.Insert(mSessions.Begin(), msr);
        ClearCache();
        return TRUE;
    }
    else {
        AutoPtr<IMediaSessionHelper> helper;
        CMediaSessionHelper::AcquireSingleton((IMediaSessionHelper**)&helper);
        Boolean isActiveState;
        if (helper->IsActiveState(newState, &isActiveState), !isActiveState) {
            // Just clear the volume cache when a state goes inactive
            mCachedVolumeDefault = NULL;
        }
    }
    return FALSE;
}

void MediaSessionStack::OnSessionStateChange(
    /* [in] */ MediaSessionRecord* record)
{
    if ((record->GetFlags() & IMediaSession::FLAG_EXCLUSIVE_GLOBAL_PRIORITY) != 0) {
        mGlobalPrioritySession = record;
    }
    // For now just clear the cache. Eventually we'll selectively clear
    // depending on what changed.
    ClearCache();
}

AutoPtr<List<AutoPtr<MediaSessionRecord> > > MediaSessionStack::GetActiveSessions(
    /* [in] */ Int32 userId)
{
    if (mCachedActiveList == NULL) {
        mCachedActiveList = GetPriorityListLocked(TRUE, 0, userId);
    }
    return mCachedActiveList;
}

AutoPtr<List<AutoPtr<MediaSessionRecord> > > MediaSessionStack::GetTransportControlSessions(
    /* [in] */ Int32 userId)
{
    if (mCachedTransportControlList == NULL) {
        mCachedTransportControlList = GetPriorityListLocked(TRUE,
            IMediaSession::FLAG_HANDLES_TRANSPORT_CONTROLS, userId);
    }
    return mCachedTransportControlList;
}

AutoPtr<MediaSessionRecord> MediaSessionStack::GetDefaultSession(
    /* [in] */ Int32 userId)
{
    if (mCachedDefault != NULL) {
        return mCachedDefault;
    }
    AutoPtr<List<AutoPtr<MediaSessionRecord> > > records = GetPriorityListLocked(TRUE, 0, userId);
    if (records->IsEmpty() == FALSE) {
        return *(records->Begin());
    }
    return NULL;
}

AutoPtr<MediaSessionRecord> MediaSessionStack::GetDefaultMediaButtonSession(
    /* [in] */ Int32 userId)
{
    if (mGlobalPrioritySession != NULL && mGlobalPrioritySession->IsActive()) {
        return mGlobalPrioritySession;
    }
    if (mCachedButtonReceiver != NULL) {
        return mCachedButtonReceiver;
    }
    AutoPtr<List<AutoPtr<MediaSessionRecord> > > records = GetPriorityListLocked(TRUE,
            IMediaSession::FLAG_HANDLES_MEDIA_BUTTONS, userId);
    if (records->IsEmpty() == FALSE) {
        mCachedButtonReceiver = *(records->Begin());
    }
    return mCachedButtonReceiver;
}

AutoPtr<MediaSessionRecord> MediaSessionStack::GetDefaultVolumeSession(
    /* [in] */ Int32 userId)
{
    if (mGlobalPrioritySession != NULL && mGlobalPrioritySession->IsActive()) {
        return mGlobalPrioritySession;
    }
    if (mCachedVolumeDefault != NULL) {
        return mCachedVolumeDefault;
    }
    AutoPtr<List<AutoPtr<MediaSessionRecord> > > records = GetPriorityListLocked(TRUE, 0, userId);
    List<AutoPtr<MediaSessionRecord> >::Iterator it = records->Begin();
    for (; it != records->End(); ++it) {
        AutoPtr<MediaSessionRecord> record = *it;
        if (record->IsPlaybackActive(FALSE)) {
            mCachedVolumeDefault = record;
            return record;
        }
    }
    return NULL;
}

AutoPtr<MediaSessionRecord> MediaSessionStack::GetDefaultRemoteSession(
    /* [in] */ Int32 userId)
{
    AutoPtr<List<AutoPtr<MediaSessionRecord> > > records = GetPriorityListLocked(TRUE, 0, userId);
    List<AutoPtr<MediaSessionRecord> >::Iterator it = records->Begin();
    for (; it != records->End(); ++it) {
        AutoPtr<MediaSessionRecord> record = *it;
        if (record->GetPlaybackType() == IMediaControllerPlaybackInfo::PLAYBACK_TYPE_REMOTE) {
            return record;
        }
    }
    return NULL;
}

Boolean MediaSessionStack::IsGlobalPriorityActive()
{
    return mGlobalPrioritySession == NULL ? FALSE : mGlobalPrioritySession->IsActive();
}

AutoPtr<List<AutoPtr<MediaSessionRecord> > > MediaSessionStack::GetPriorityListLocked(
    /* [in] */ Boolean activeOnly,
    /* [in] */ Int32 withFlags,
    /* [in] */ Int32 userId)
{
    AutoPtr<List<AutoPtr<MediaSessionRecord> > > result = new List<AutoPtr<MediaSessionRecord> >();
    Int32 lastLocalIndex = 0;
    Int32 lastActiveIndex = 0;
    Int32 lastPublishedIndex = 0;

    List<AutoPtr<MediaSessionRecord> >::Iterator it = mSessions.Begin();
    for (; it != mSessions.End(); ++it) {
        AutoPtr<MediaSessionRecord> session = *it;

        if (userId != IUserHandle::USER_ALL && userId != session->GetUserId()) {
            // Filter out sessions for the wrong user
            continue;
        }
        if ((session->GetFlags() & withFlags) != withFlags) {
            // Filter out sessions with the wrong flags
            continue;
        }
        if (!session->IsActive()) {
            if (!activeOnly) {
                // If we're getting unpublished as well always put them at
                // the end
                result->PushBack(session);
            }
            continue;
        }

        if (session->IsSystemPriority()) {
            // System priority sessions are special and always go at the
            // front. We expect there to only be one of these at a time.
            result->Insert(result->Begin(), session);
            lastLocalIndex++;
            lastActiveIndex++;
            lastPublishedIndex++;
        }
        else if (session->IsPlaybackActive(TRUE)) {
            // TODO this with real local route check
            if (TRUE) {
                // Active local sessions get top priority
                result->Insert(lastLocalIndex, session);
                lastLocalIndex++;
                lastActiveIndex++;
                lastPublishedIndex++;
            }
            else {
                // Then active remote sessions
                result->Insert(lastActiveIndex, session);
                lastActiveIndex++;
                lastPublishedIndex++;
            }
        }
        else {
            // inactive sessions go at the end in order of whoever last did
            // something.
            result->Insert(lastPublishedIndex, session);
            lastPublishedIndex++;
        }
    }

    return result;
}

Boolean MediaSessionStack::ShouldUpdatePriority(
    /* [in] */ Int32 oldState,
    /* [in] */ Int32 newState)
{
    if (ContainsState(newState, ALWAYS_PRIORITY_STATES)) {
        return TRUE;
    }
    if (!ContainsState(oldState, TRANSITION_PRIORITY_STATES)
            && ContainsState(newState, TRANSITION_PRIORITY_STATES)) {
        return TRUE;
    }
    return FALSE;
}

Boolean MediaSessionStack::ContainsState(
    /* [in] */ Int32 state,
    /* [in] */ ArrayOf<Int32>* states)
{
    for (Int32 i = 0; i < states->GetLength(); i++) {
        if ((*states)[i] == state) {
            return TRUE;
        }
    }
    return FALSE;
}

void MediaSessionStack::ClearCache()
{
    mCachedDefault = NULL;
    mCachedVolumeDefault = NULL;
    mCachedButtonReceiver = NULL;
    mCachedActiveList = NULL;
    mCachedTransportControlList = NULL;
}

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos
