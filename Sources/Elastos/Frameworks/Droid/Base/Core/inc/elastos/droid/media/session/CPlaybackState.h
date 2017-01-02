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

#ifndef __ELASTOS_DROID_MEDIA_SESSION_CPLAYBACKSTATE_H__
#define __ELASTOS_DROID_MEDIA_SESSION_CPLAYBACKSTATE_H__

#include "_Elastos_Droid_Media_Session_CPlaybackState.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

/**
 * Playback state for a {@link MediaSession}. This includes a state like
 * {@link PlaybackState#STATE_PLAYING}, the current playback position,
 * and the current control capabilities.
 */
CarClass(CPlaybackState)
    , public Object
    , public IPlaybackState
    , public IParcelable
{
public:
    CPlaybackState();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CPlaybackState();

    CARAPI constructor(
        /* [in] */ Int32 state,
        /* [in] */ Int64 position,
        /* [in] */ Int64 updateTime,
        /* [in] */ Float speed,
        /* [in] */ Int64 bufferedPosition,
        /* [in] */ Int64 transportControls,
        /* [in] */ IList * customActions,
        /* [in] */ Int64 activeItemId,
        /* [in] */ ICharSequence * error);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel * source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel * dest);

    CARAPI ToString(
        /* [out] */ String * result);

    /**
     * Get the current state of playback. One of the following:
     * <ul>
     * <li> {@link PlaybackState#STATE_NONE}</li>
     * <li> {@link PlaybackState#STATE_STOPPED}</li>
     * <li> {@link PlaybackState#STATE_PLAYING}</li>
     * <li> {@link PlaybackState#STATE_PAUSED}</li>
     * <li> {@link PlaybackState#STATE_FAST_FORWARDING}</li>
     * <li> {@link PlaybackState#STATE_REWINDING}</li>
     * <li> {@link PlaybackState#STATE_BUFFERING}</li>
     * <li> {@link PlaybackState#STATE_ERROR}</li>
     */
    CARAPI GetState(
        /* [out] */ Int32 * result);

    /**
     * Get the current playback position in ms.
     */
    CARAPI GetPosition(
        /* [out] */ Int64 * result);

    /**
     * Get the current buffered position in ms. This is the farthest playback
     * point that can be reached from the current position using only buffered
     * content.
     */
    CARAPI GetBufferedPosition(
        /* [out] */ Int64 * result);

    /**
     * Get the current playback speed as a multiple of normal playback. This
     * should be negative when rewinding. A value of 1 means normal playback and
     * 0 means paused.
     *
     * @return The current speed of playback.
     */
    CARAPI GetPlaybackSpeed(
        /* [out] */ Float * result);

    /**
     * Get the current actions available on this session. This should use a
     * bitmask of the available actions.
     * <ul>
     * <li> {@link PlaybackState#ACTION_SKIP_TO_PREVIOUS}</li>
     * <li> {@link PlaybackState#ACTION_REWIND}</li>
     * <li> {@link PlaybackState#ACTION_PLAY}</li>
     * <li> {@link PlaybackState#ACTION_PAUSE}</li>
     * <li> {@link PlaybackState#ACTION_STOP}</li>
     * <li> {@link PlaybackState#ACTION_FAST_FORWARD}</li>
     * <li> {@link PlaybackState#ACTION_SKIP_TO_NEXT}</li>
     * <li> {@link PlaybackState#ACTION_SEEK_TO}</li>
     * <li> {@link PlaybackState#ACTION_SET_RATING}</li>
     * </ul>
     */
    CARAPI GetActions(
        /* [out] */ Int64 * result);

    /**
     * Get the list of custom actions.
     */
    CARAPI GetCustomActions(
        /* [out] */ IList ** result);

    /**
     * Get a user readable error message. This should be set when the state is
     * {@link PlaybackState#STATE_ERROR}.
     */
    CARAPI GetErrorMessage(
        /* [out] */ ICharSequence ** result);

    /**
     * Get the elapsed real time at which position was last updated. If the
     * position has never been set this will return 0;
     *
     * @return The last time the position was updated.
     */
    CARAPI GetLastPositionUpdateTime(
        /* [out] */ Int64 * result);

    /**
     * Get the id of the currently active item in the queue. If there is no
     * queue or a queue is not supported by the session this will be
     * {@link MediaSession.QueueItem#UNKNOWN_ID}.
     *
     * @return The id of the currently active item in the queue or
     *         {@link MediaSession.QueueItem#UNKNOWN_ID}.
     */
    CARAPI GetActiveQueueItemId(
        /* [out] */ Int64 * result);

    /**
     * Get the {@link PlaybackState} state for the given
     * {@link RemoteControlClient} state.
     *
     * @param rccState The state used by {@link RemoteControlClient}.
     * @return The equivalent state used by {@link PlaybackState}.
     * @hide
     */
    static CARAPI GetStateFromRccState(
        /* [in] */ Int32 rccState,
        /* [out] */ Int32 * result);

    /**
     * Get the {@link RemoteControlClient} state for the given
     * {@link PlaybackState} state.
     *
     * @param state The state used by {@link PlaybackState}.
     * @return The equivalent state used by {@link RemoteControlClient}.
     * @hide
     */
    static CARAPI GetRccStateFromState(
        /* [in] */ Int32 state,
        /* [out] */ Int32 * result);

    /**
     * @hide
     */
    static CARAPI GetActionsFromRccControlFlags(
        /* [in] */ Int32 rccFlags,
        /* [out] */ Int64 * result);

    /**
     * @hide
     */
    static CARAPI GetRccControlFlagsFromActions(
        /* [in] */ Int64 actions,
        /* [out] */ Int32 * result);

private:
    static CARAPI_(Int64) GetActionForRccFlag(
        /* [in] */ Int32 flag);

    static CARAPI_(Int32) GetRccFlagForAction(
        /* [in] */ Int64 action);

private:
    static String TAG;

    Int32 mState;
    Int64 mPosition;
    Int64 mBufferedPosition;
    Float mSpeed;
    Int64 mActions;
    AutoPtr<IList> mCustomActions;
    AutoPtr<ICharSequence> mErrorMessage;
    Int64 mUpdateTime;
    Int64 mActiveItemId;
};

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_SESSION_CPLAYBACKSTATE_H__
