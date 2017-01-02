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

#ifndef __ELASTOS_DROID_SERVER_MEDIA_MEDIASESSIONSTACK_H__
#define __ELASTOS_DROID_SERVER_MEDIA_MEDIASESSIONSTACK_H__

#include "elastos/droid/server/media/MediaSessionRecord.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

/**
 * Keeps track of media sessions and their priority for notifications, media
 * button dispatch, etc.
 */
class MediaSessionStack : public Object
{
public:
    /**
     * Add a record to the priority tracker.
     *
     * @param record The record to add.
     */
    CARAPI_(void) AddSession(
        /* [in] */ MediaSessionRecord* record);

    /**
     * Remove a record from the priority tracker.
     *
     * @param record The record to remove.
     */
    CARAPI_(void) RemoveSession(
        /* [in] */ MediaSessionRecord* record);

    /**
     * Notify the priority tracker that a session's state changed.
     *
     * @param record The record that changed.
     * @param oldState Its old playback state.
     * @param newState Its new playback state.
     * @return true if the priority order was updated, false otherwise.
     */
    CARAPI_(Boolean) OnPlaystateChange(
        /* [in] */ MediaSessionRecord* record,
        /* [in] */ Int32 oldState,
        /* [in] */ Int32 newState);

    /**
     * Handle any stack changes that need to occur in response to a session
     * state change. TODO add the old and new session state as params
     *
     * @param record The record that changed.
     */
    CARAPI_(void) OnSessionStateChange(
        /* [in] */ MediaSessionRecord* record);

    /**
     * Get the current priority sorted list of active sessions. The most
     * important session is at index 0 and the least important at size - 1.
     *
     * @param userId The user to check.
     * @return All the active sessions in priority order.
     */
    CARAPI_(AutoPtr<List<AutoPtr<MediaSessionRecord> > >) GetActiveSessions(
        /* [in] */ Int32 userId);

    /**
     * Get the current priority sorted list of active sessions that use
     * transport controls. The most important session is at index 0 and the
     * least important at size -1.
     *
     * @param userId The user to check.
     * @return All the active sessions that handle transport controls in
     *         priority order.
     */
    CARAPI_(AutoPtr<List<AutoPtr<MediaSessionRecord> > >) GetTransportControlSessions(
        /* [in] */ Int32 userId);

    /**
     * Get the highest priority active session.
     *
     * @param userId The user to check.
     * @return The current highest priority session or null.
     */
    CARAPI_(AutoPtr<MediaSessionRecord>) GetDefaultSession(
        /* [in] */ Int32 userId);

    /**
     * Get the highest priority session that can handle media buttons.
     *
     * @param userId The user to check.
     * @return The default media button session or null.
     */
    CARAPI_(AutoPtr<MediaSessionRecord>) GetDefaultMediaButtonSession(
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<MediaSessionRecord>) GetDefaultVolumeSession(
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<MediaSessionRecord>) GetDefaultRemoteSession(
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) IsGlobalPriorityActive();

    // CARAPI_(void) dump(PrintWriter pw, String prefix);

private:
    /**
     * Get a priority sorted list of sessions. Can filter to only return active
     * sessions or sessions with specific flags.
     *
     * @param activeOnly True to only return active sessions, false to return
     *            all sessions.
     * @param withFlags Only return sessions with all the specified flags set. 0
     *            returns all sessions.
     * @param userId The user to get sessions for. {@link UserHandle#USER_ALL}
     *            will return sessions for all users.
     * @return The priority sorted list of sessions.
     */
    CARAPI_(AutoPtr<List<AutoPtr<MediaSessionRecord> > >) GetPriorityListLocked(
        /* [in] */ Boolean activeOnly,
        /* [in] */ Int32 withFlags,
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) ShouldUpdatePriority(
        /* [in] */ Int32 oldState,
        /* [in] */ Int32 newState);

    CARAPI_(Boolean) ContainsState(
        /* [in] */ Int32 state,
        /* [in] */ ArrayOf<Int32>* states);

    CARAPI_(void) ClearCache();

private:
    /**
     * These are states that usually indicate the user took an action and should
     * bump priority regardless of the old state.
     */
    static const AutoPtr<ArrayOf<Int32> > ALWAYS_PRIORITY_STATES;
    /**
     * These are states that usually indicate the user took an action if they
     * were entered from a non-priority state.
     */
    static const AutoPtr<ArrayOf<Int32> > TRANSITION_PRIORITY_STATES;

    List<AutoPtr<MediaSessionRecord> > mSessions;

    AutoPtr<MediaSessionRecord> mGlobalPrioritySession;

    AutoPtr<MediaSessionRecord> mCachedButtonReceiver;
    AutoPtr<MediaSessionRecord> mCachedDefault;
    AutoPtr<MediaSessionRecord> mCachedVolumeDefault;
    AutoPtr<List<AutoPtr<MediaSessionRecord> > > mCachedActiveList;
    AutoPtr<List<AutoPtr<MediaSessionRecord> > > mCachedTransportControlList;
};

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_MEDIA_MEDIASESSIONSTACK_H__
