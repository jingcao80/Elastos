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

#ifndef __ELASTOS_DROID_MEDIA_TV_TVINPUTMANAGERSESSIONCALLBACK_H__
#define __ELASTOS_DROID_MEDIA_TV_TVINPUTMANAGERSESSIONCALLBACK_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/**
 * Interface used to receive the created session.
 * @hide
 */
// @SystemApi
class TvInputManagerSessionCallback
    : public Object
    , public ITvInputManagerSessionCallback
{
public:
    TvInputManagerSessionCallback();

    virtual ~TvInputManagerSessionCallback();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * This is called after {@link TvInputManager#createSession} has been processed.
     *
     * @param session A {@link TvInputManager.Session} instance created. This can be
     *            {@code null} if the creation request failed.
     */
    CARAPI OnSessionCreated(
        /* [in] */ ITvInputManagerSession* session);

    /**
     * This is called when {@link TvInputManager.Session} is released.
     * This typically happens when the process hosting the session has crashed or been killed.
     *
     * @param session A {@link TvInputManager.Session} instance released.
     */
    CARAPI OnSessionReleased(
        /* [in] */ ITvInputManagerSession* session);

    /**
     * This is called when the channel of this session is changed by the underlying TV input
     * with out any {@link TvInputManager.Session#tune(Uri)} request.
     *
     * @param session A {@link TvInputManager.Session} associated with this callback.
     * @param channelUri The URI of a channel.
     */
    CARAPI OnChannelRetuned(
        /* [in] */ ITvInputManagerSession* session,
        /* [in] */ IUri* channelUri);

    /**
     * This is called when the track information of the session has been changed.
     *
     * @param session A {@link TvInputManager.Session} associated with this callback.
     * @param tracks A list which includes track information.
     */
    CARAPI OnTracksChanged(
        /* [in] */ ITvInputManagerSession* session,
        /* [in] */ IList* tracks);

    /**
     * This is called when a track for a given type is selected.
     *
     * @param session A {@link TvInputManager.Session} associated with this callback
     * @param type The type of the selected track. The type can be
     *            {@link TvTrackInfo#TYPE_AUDIO}, {@link TvTrackInfo#TYPE_VIDEO} or
     *            {@link TvTrackInfo#TYPE_SUBTITLE}.
     * @param trackId The ID of the selected track. When {@code null} the currently selected
     *            track for a given type should be unselected.
     */
    CARAPI OnTrackSelected(
        /* [in] */ ITvInputManagerSession* session,
        /* [in] */ Int32 type,
        /* [in] */ const String& trackId);

    /**
     * This is called when the video is available, so the TV input starts the playback.
     *
     * @param session A {@link TvInputManager.Session} associated with this callback.
     */
    CARAPI OnVideoAvailable(
        /* [in] */ ITvInputManagerSession* session);

    /**
     * This is called when the video is not available, so the TV input stops the playback.
     *
     * @param session A {@link TvInputManager.Session} associated with this callback
     * @param reason The reason why the TV input stopped the playback:
     * <ul>
     * <li>{@link TvInputManager#VIDEO_UNAVAILABLE_REASON_UNKNOWN}
     * <li>{@link TvInputManager#VIDEO_UNAVAILABLE_REASON_TUNING}
     * <li>{@link TvInputManager#VIDEO_UNAVAILABLE_REASON_WEAK_SIGNAL}
     * <li>{@link TvInputManager#VIDEO_UNAVAILABLE_REASON_BUFFERING}
     * </ul>
     */
    CARAPI OnVideoUnavailable(
        /* [in] */ ITvInputManagerSession* session,
        /* [in] */ Int32 reason);

    /**
     * This is called when the current program content turns out to be allowed to watch since
     * its content rating is not blocked by parental controls.
     *
     * @param session A {@link TvInputManager.Session} associated with this callback
     */
    CARAPI OnContentAllowed(
        /* [in] */ ITvInputManagerSession* session);

    /**
     * This is called when the current program content turns out to be not allowed to watch
     * since its content rating is blocked by parental controls.
     *
     * @param session A {@link TvInputManager.Session} associated with this callback
     * @param rating The content ration of the blocked program.
     */
    CARAPI OnContentBlocked(
        /* [in] */ ITvInputManagerSession* session,
        /* [in] */ ITvContentRating* rating);

    /**
     * This is called when {@link TvInputService.Session#layoutSurface} is called to change the
     * layout of surface.
     *
     * @param session A {@link TvInputManager.Session} associated with this callback
     * @param left Left position.
     * @param top Top position.
     * @param right Right position.
     * @param bottom Bottom position.
     * @hide
     */
    CARAPI OnLayoutSurface(
        /* [in] */ ITvInputManagerSession* session,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    /**
     * This is called when a custom event has been sent from this session.
     *
     * @param session A {@link TvInputManager.Session} associated with this callback
     * @param eventType The type of the event.
     * @param eventArgs Optional arguments of the event.
     * @hide
     */
    CARAPI OnSessionEvent(
        /* [in] */ ITvInputManagerSession* session,
        /* [in] */ const String& eventType,
        /* [in] */ IBundle* eventArgs);
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_TVINPUTMANAGERSESSIONCALLBACK_H__
