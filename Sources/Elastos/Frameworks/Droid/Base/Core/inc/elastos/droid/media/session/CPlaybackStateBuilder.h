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

#ifndef __ELASTOS_DROID_MEDIA_SESSION_CPLAYBACKSTATEBUILDER_H__
#define __ELASTOS_DROID_MEDIA_SESSION_CPLAYBACKSTATEBUILDER_H__

#include "_Elastos_Droid_Media_Session_CPlaybackStateBuilder.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

/**
 * Builder for {@link PlaybackState} objects.
 */
CarClass(CPlaybackStateBuilder)
    , public Object
    , public IPlaybackStateBuilder
{
public:
    CPlaybackStateBuilder();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CPlaybackStateBuilder();

    /**
     * Creates an initially empty state builder.
     */
    CARAPI constructor();

    /**
     * Creates a builder with the same initial values as those in the from
     * state.
     *
     * @param from The state to use for initializing the builder.
     */
   CARAPI constructor(
        /* [in] */ IPlaybackState * from);

    /**
     * Set the current state of playback.
     * <p>
     * The position must be in ms and indicates the current playback
     * position within the item. If the position is unknown use
     * {@link #PLAYBACK_POSITION_UNKNOWN}. When not using an unknown
     * position the time at which the position was updated must be provided.
     * It is okay to use {@link SystemClock#elapsedRealtime()} if the
     * current position was just retrieved.
     * <p>
     * The speed is a multiple of normal playback and should be 0 when
     * paused and negative when rewinding. Normal playback speed is 1.0.
     * <p>
     * The state must be one of the following:
     * <ul>
     * <li> {@link PlaybackState#STATE_NONE}</li>
     * <li> {@link PlaybackState#STATE_STOPPED}</li>
     * <li> {@link PlaybackState#STATE_PLAYING}</li>
     * <li> {@link PlaybackState#STATE_PAUSED}</li>
     * <li> {@link PlaybackState#STATE_FAST_FORWARDING}</li>
     * <li> {@link PlaybackState#STATE_REWINDING}</li>
     * <li> {@link PlaybackState#STATE_BUFFERING}</li>
     * <li> {@link PlaybackState#STATE_ERROR}</li>
     * </ul>
     *
     * @param state The current state of playback.
     * @param position The position in the current item in ms.
     * @param playbackSpeed The current speed of playback as a multiple of
     *            normal playback.
     * @param updateTime The time in the {@link SystemClock#elapsedRealtime}
     *            timebase that the position was updated at.
     * @return this
     */
    CARAPI SetState(
        /* [in] */ Int32 state,
        /* [in] */ Int64 position,
        /* [in] */ Float playbackSpeed,
        /* [in] */ Int64 updateTime);

    /**
     * Set the current state of playback.
     * <p>
     * The position must be in ms and indicates the current playback
     * position within the item. If the position is unknown use
     * {@link #PLAYBACK_POSITION_UNKNOWN}. The update time will be set to
     * the current {@link SystemClock#elapsedRealtime()}.
     * <p>
     * The speed is a multiple of normal playback and should be 0 when
     * paused and negative when rewinding. Normal playback speed is 1.0.
     * <p>
     * The state must be one of the following:
     * <ul>
     * <li> {@link PlaybackState#STATE_NONE}</li>
     * <li> {@link PlaybackState#STATE_STOPPED}</li>
     * <li> {@link PlaybackState#STATE_PLAYING}</li>
     * <li> {@link PlaybackState#STATE_PAUSED}</li>
     * <li> {@link PlaybackState#STATE_FAST_FORWARDING}</li>
     * <li> {@link PlaybackState#STATE_REWINDING}</li>
     * <li> {@link PlaybackState#STATE_BUFFERING}</li>
     * <li> {@link PlaybackState#STATE_ERROR}</li>
     * </ul>
     *
     * @param state The current state of playback.
     * @param position The position in the current item in ms.
     * @param playbackSpeed The current speed of playback as a multiple of
     *            normal playback.
     * @return this
     */
    CARAPI SetState(
        /* [in] */ Int32 state,
        /* [in] */ Int64 position,
        /* [in] */ Float playbackSpeed);

    /**
     * Set the current actions available on this session. This should use a
     * bitmask of possible actions.
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
     *
     * @param actions The set of actions allowed.
     * @return this
     */
    CARAPI SetActions(
        /* [in] */ Int64 actions);

    /**
     * Add a custom action to the playback state. Actions can be used to
     * expose additional functionality to {@link MediaController
     * MediaControllers} beyond what is offered by the standard transport
     * controls.
     * <p>
     * e.g. start a radio station based on the current item or skip ahead by
     * 30 seconds.
     *
     * @param action An identifier for this action. It can be sent back to
     *            the {@link MediaSession} through
     *            {@link MediaController.TransportControls#sendCustomAction(String, Bundle)}.
     * @param name The display name for the action. If text is shown with
     *            the action or used for accessibility, this is what should
     *            be used.
     * @param icon The resource action of the icon that should be displayed
     *            for the action. The resource should be in the package of
     *            the {@link MediaSession}.
     * @return this
     */
    CARAPI AddCustomAction(
        /* [in] */ const String& action,
        /* [in] */ const String& name,
        /* [in] */ Int32 icon);

    /**
     * Add a custom action to the playback state. Actions can be used to expose additional
     * functionality to {@link MediaController MediaControllers} beyond what is offered by the
     * standard transport controls.
     * <p>
     * An example of an action would be to start a radio station based on the current item
     * or to skip ahead by 30 seconds.
     *
     * @param customAction The custom action to add to the {@link PlaybackState}.
     * @return this
     */
    CARAPI AddCustomAction(
        /* [in] */ IPlaybackStateCustomAction * customAction);

    /**
     * Set the current buffered position in ms. This is the farthest
     * playback point that can be reached from the current position using
     * only buffered content.
     *
     * @param bufferedPosition The position in ms that playback is buffered
     *            to.
     * @return this
     */
    CARAPI SetBufferedPosition(
        /* [in] */ Int64 bufferedPosition);

    /**
     * Set the active item in the play queue by specifying its id. The
     * default value is {@link MediaSession.QueueItem#UNKNOWN_ID}
     *
     * @param id The id of the active item.
     * @return this
     */
    CARAPI SetActiveQueueItemId(
        /* [in] */ Int64 id);

    /**
     * Set a user readable error message. This should be set when the state
     * is {@link PlaybackState#STATE_ERROR}.
     *
     * @param error The error message for display to the user.
     * @return this
     */
    CARAPI SetErrorMessage(
        /* [in] */ ICharSequence * error);

    /**
     * Build and return the {@link PlaybackState} instance with these values.
     *
     * @return A new state instance.
     */
    CARAPI Build(
        /* [out] */ IPlaybackState ** result);

private:
    AutoPtr<IArrayList> mCustomActions;

    Int32 mState;
    Int64 mPosition;
    Int64 mBufferedPosition;
    Float mSpeed;
    Int64 mActions;
    AutoPtr<ICharSequence> mErrorMessage;
    Int64 mUpdateTime;
    Int64 mActiveItemId;
};

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_SESSION_CPLAYBACKSTATEBUILDER_H__
