/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.server.media;

using Elastos::Droid::Media::Isession.MediaController.PlaybackInfo;
using Elastos::Droid::Media::Isession.PlaybackState;
using Elastos::Droid::Media::Isession.MediaSession;
using Elastos::Droid::Os::IUserHandle;

using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;

/**
 * Keeps track of media sessions and their priority for notifications, media
 * button dispatch, etc.
 */
public class MediaSessionStack {
    /**
     * These are states that usually indicate the user took an action and should
     * bump priority regardless of the old state.
     */
    private static const Int32[] ALWAYS_PRIORITY_STATES = {
            PlaybackState.STATE_FAST_FORWARDING,
            PlaybackState.STATE_REWINDING,
            PlaybackState.STATE_SKIPPING_TO_PREVIOUS,
            PlaybackState.STATE_SKIPPING_TO_NEXT };
    /**
     * These are states that usually indicate the user took an action if they
     * were entered from a non-priority state.
     */
    private static const Int32[] TRANSITION_PRIORITY_STATES = {
            PlaybackState.STATE_BUFFERING,
            PlaybackState.STATE_CONNECTING,
            PlaybackState.STATE_PLAYING };

    private final ArrayList<MediaSessionRecord> mSessions = new ArrayList<MediaSessionRecord>();

    private MediaSessionRecord mGlobalPrioritySession;

    private MediaSessionRecord mCachedButtonReceiver;
    private MediaSessionRecord mCachedDefault;
    private MediaSessionRecord mCachedVolumeDefault;
    private ArrayList<MediaSessionRecord> mCachedActiveList;
    private ArrayList<MediaSessionRecord> mCachedTransportControlList;

    /**
     * Add a record to the priority tracker.
     *
     * @param record The record to add.
     */
    CARAPI AddSession(MediaSessionRecord record) {
        mSessions->Add(record);
        ClearCache();
    }

    /**
     * Remove a record from the priority tracker.
     *
     * @param record The record to remove.
     */
    CARAPI RemoveSession(MediaSessionRecord record) {
        mSessions->Remove(record);
        if (record == mGlobalPrioritySession) {
            mGlobalPrioritySession = NULL;
        }
        ClearCache();
    }

    /**
     * Notify the priority tracker that a session's state changed.
     *
     * @param record The record that changed.
     * @param oldState Its old playback state.
     * @param newState Its new playback state.
     * @return TRUE if the priority order was updated, FALSE otherwise.
     */
    public Boolean OnPlaystateChange(MediaSessionRecord record, Int32 oldState, Int32 newState) {
        if (ShouldUpdatePriority(oldState, newState)) {
            mSessions->Remove(record);
            mSessions->Add(0, record);
            ClearCache();
            return TRUE;
        } else if (!MediaSession->IsActiveState(newState)) {
            // Just clear the volume cache when a state goes inactive
            mCachedVolumeDefault = NULL;
        }
        return FALSE;
    }

    /**
     * Handle any stack changes that need to occur in response to a session
     * state change. TODO add the old and new session state as params
     *
     * @param record The record that changed.
     */
    CARAPI OnSessionStateChange(MediaSessionRecord record) {
        if ((record->GetFlags() & MediaSession.FLAG_EXCLUSIVE_GLOBAL_PRIORITY) != 0) {
            mGlobalPrioritySession = record;
        }
        // For now just clear the cache. Eventually we'll selectively clear
        // depending on what changed.
        ClearCache();
    }

    /**
     * Get the current priority sorted list of active sessions. The most
     * important session is at index 0 and the least important at size - 1.
     *
     * @param userId The user to check.
     * @return All the active sessions in priority order.
     */
    public ArrayList<MediaSessionRecord> GetActiveSessions(Int32 userId) {
        if (mCachedActiveList == NULL) {
            mCachedActiveList = GetPriorityListLocked(TRUE, 0, userId);
        }
        return mCachedActiveList;
    }

    /**
     * Get the current priority sorted list of active sessions that use
     * transport controls. The most important session is at index 0 and the
     * least important at size -1.
     *
     * @param userId The user to check.
     * @return All the active sessions that handle transport controls in
     *         priority order.
     */
    public ArrayList<MediaSessionRecord> GetTransportControlSessions(Int32 userId) {
        if (mCachedTransportControlList == NULL) {
            mCachedTransportControlList = GetPriorityListLocked(TRUE,
                    MediaSession.FLAG_HANDLES_TRANSPORT_CONTROLS, userId);
        }
        return mCachedTransportControlList;
    }

    /**
     * Get the highest priority active session.
     *
     * @param userId The user to check.
     * @return The current highest priority session or NULL.
     */
    public MediaSessionRecord GetDefaultSession(Int32 userId) {
        if (mCachedDefault != NULL) {
            return mCachedDefault;
        }
        ArrayList<MediaSessionRecord> records = GetPriorityListLocked(TRUE, 0, userId);
        if (records->Size() > 0) {
            return records->Get(0);
        }
        return NULL;
    }

    /**
     * Get the highest priority session that can handle media buttons.
     *
     * @param userId The user to check.
     * @return The default media button session or NULL.
     */
    public MediaSessionRecord GetDefaultMediaButtonSession(Int32 userId) {
        if (mGlobalPrioritySession != NULL && mGlobalPrioritySession->IsActive()) {
            return mGlobalPrioritySession;
        }
        if (mCachedButtonReceiver != NULL) {
            return mCachedButtonReceiver;
        }
        ArrayList<MediaSessionRecord> records = GetPriorityListLocked(TRUE,
                MediaSession.FLAG_HANDLES_MEDIA_BUTTONS, userId);
        if (records->Size() > 0) {
            mCachedButtonReceiver = records->Get(0);
        }
        return mCachedButtonReceiver;
    }

    public MediaSessionRecord GetDefaultVolumeSession(Int32 userId) {
        if (mGlobalPrioritySession != NULL && mGlobalPrioritySession->IsActive()) {
            return mGlobalPrioritySession;
        }
        if (mCachedVolumeDefault != NULL) {
            return mCachedVolumeDefault;
        }
        ArrayList<MediaSessionRecord> records = GetPriorityListLocked(TRUE, 0, userId);
        Int32 size = records->Size();
        for (Int32 i = 0; i < size; i++) {
            MediaSessionRecord record = records->Get(i);
            if (record->IsPlaybackActive(FALSE)) {
                mCachedVolumeDefault = record;
                return record;
            }
        }
        return NULL;
    }

    public MediaSessionRecord GetDefaultRemoteSession(Int32 userId) {
        ArrayList<MediaSessionRecord> records = GetPriorityListLocked(TRUE, 0, userId);

        Int32 size = records->Size();
        for (Int32 i = 0; i < size; i++) {
            MediaSessionRecord record = records->Get(i);
            if (record->GetPlaybackType() == PlaybackInfo.PLAYBACK_TYPE_REMOTE) {
                return record;
            }
        }
        return NULL;
    }

    public Boolean IsGlobalPriorityActive() {
        return mGlobalPrioritySession == NULL ? FALSE : mGlobalPrioritySession->IsActive();
    }

    CARAPI Dump(PrintWriter pw, String prefix) {
        ArrayList<MediaSessionRecord> sortedSessions = GetPriorityListLocked(FALSE, 0,
                UserHandle.USER_ALL);
        Int32 count = sortedSessions->Size();
        pw->Println(prefix + "Global priority session is " + mGlobalPrioritySession);
        pw->Println(prefix + "Sessions Stack - have " + count + " sessions:");
        String indent = prefix + "  ";
        for (Int32 i = 0; i < count; i++) {
            MediaSessionRecord record = sortedSessions->Get(i);
            record->Dump(pw, indent);
            pw->Println();
        }
    }

    /**
     * Get a priority sorted list of sessions. Can filter to only return active
     * sessions or sessions with specific flags.
     *
     * @param activeOnly True to only return active sessions, FALSE to return
     *            all sessions.
     * @param withFlags Only return sessions with all the specified flags set. 0
     *            returns all sessions.
     * @param userId The user to get sessions for. {@link UserHandle#USER_ALL}
     *            will return sessions for all users.
     * @return The priority sorted list of sessions.
     */
    private ArrayList<MediaSessionRecord> GetPriorityListLocked(Boolean activeOnly, Int32 withFlags,
            Int32 userId) {
        ArrayList<MediaSessionRecord> result = new ArrayList<MediaSessionRecord>();
        Int32 lastLocalIndex = 0;
        Int32 lastActiveIndex = 0;
        Int32 lastPublishedIndex = 0;

        Int32 size = mSessions->Size();
        for (Int32 i = 0; i < size; i++) {
            final MediaSessionRecord session = mSessions->Get(i);

            if (userId != UserHandle.USER_ALL && userId != session->GetUserId()) {
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
                    result->Add(session);
                }
                continue;
            }

            if (session->IsSystemPriority()) {
                // System priority sessions are special and always go at the
                // front. We expect there to only be one of these at a time.
                result->Add(0, session);
                lastLocalIndex++;
                lastActiveIndex++;
                lastPublishedIndex++;
            } else if (session->IsPlaybackActive(TRUE)) {
                // TODO this with real local route check
                if (TRUE) {
                    // Active local sessions get top priority
                    result->Add(lastLocalIndex, session);
                    lastLocalIndex++;
                    lastActiveIndex++;
                    lastPublishedIndex++;
                } else {
                    // Then active remote sessions
                    result->Add(lastActiveIndex, session);
                    lastActiveIndex++;
                    lastPublishedIndex++;
                }
            } else {
                // inactive sessions go at the end in order of whoever last did
                // something.
                result->Add(lastPublishedIndex, session);
                lastPublishedIndex++;
            }
        }

        return result;
    }

    private Boolean ShouldUpdatePriority(Int32 oldState, Int32 newState) {
        if (ContainsState(newState, ALWAYS_PRIORITY_STATES)) {
            return TRUE;
        }
        if (!ContainsState(oldState, TRANSITION_PRIORITY_STATES)
                && ContainsState(newState, TRANSITION_PRIORITY_STATES)) {
            return TRUE;
        }
        return FALSE;
    }

    private Boolean ContainsState(Int32 state, Int32[] states) {
        for (Int32 i = 0; i < states.length; i++) {
            if (states[i] == state) {
                return TRUE;
            }
        }
        return FALSE;
    }

    private void ClearCache() {
        mCachedDefault = NULL;
        mCachedVolumeDefault = NULL;
        mCachedButtonReceiver = NULL;
        mCachedActiveList = NULL;
        mCachedTransportControlList = NULL;
    }
}
