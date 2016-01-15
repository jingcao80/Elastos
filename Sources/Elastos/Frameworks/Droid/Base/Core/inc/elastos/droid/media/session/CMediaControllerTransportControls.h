#ifndef __ELASTOS_DROID_MEDIA_SESSION_CMEDIACONTROLLERTRANSPORTCONTROLS_H__
#define __ELASTOS_DROID_MEDIA_SESSION_CMEDIACONTROLLERTRANSPORTCONTROLS_H__

#include "_Elastos_Droid_Media_Session_CMediaControllerTransportControls.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/session/CMediaController.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Media::IRating;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

/**
 * Interface for controlling media playback on a session. This allows an app
 * to send media transport commands to the session.
 */
CarClass(CMediaControllerTransportControls)
    , public Object
    , public IMediaControllerTransportControls
{
public:
    CMediaControllerTransportControls();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CMediaControllerTransportControls();

    CARAPI constructor(
        /* [in] */ IMediaController * host);

    /**
     * Request that the player start its playback at its current position.
     */
    CARAPI Play();

    /**
     * Request that the player start playback for a specific {@link Uri}.
     *
     * @param mediaId The uri of the requested media.
     * @param extras Optional extras that can include extra information about the media item
     *               to be played.
     */
    CARAPI PlayFromMediaId(
        /* [in] */ const String& mediaId,
        /* [in] */ IBundle * extras);

    /**
     * Request that the player start playback for a specific search query.
     * An empty or null query should be treated as a request to play any
     * music.
     *
     * @param query The search query.
     * @param extras Optional extras that can include extra information
     *            about the query.
     */
    CARAPI PlayFromSearch(
        /* [in] */ const String& query,
        /* [in] */ IBundle * extras);

    /**
     * Play an item with a specific id in the play queue. If you specify an
     * id that is not in the play queue, the behavior is undefined.
     */
    CARAPI SkipToQueueItem(
        /* [in] */ Int64 id);

    /**
     * Request that the player pause its playback and stay at its current
     * position.
     */
    CARAPI Pause();

    /**
     * Request that the player stop its playback; it may clear its state in
     * whatever way is appropriate.
     */
    CARAPI Stop();

    /**
     * Move to a new location in the media stream.
     *
     * @param pos Position to move to, in milliseconds.
     */
    CARAPI SeekTo(
        /* [in] */ Int64 pos);

    /**
     * Start fast forwarding. If playback is already fast forwarding this
     * may increase the rate.
     */
    CARAPI FastForward();

    /**
     * Skip to the next item.
     */
    CARAPI SkipToNext();

    /**
     * Start rewinding. If playback is already rewinding this may increase
     * the rate.
     */
    CARAPI Rewind();

    /**
     * Skip to the previous item.
     */
    CARAPI SkipToPrevious();

    /**
     * Rate the current content. This will cause the rating to be set for
     * the current user. The Rating type must match the type returned by
     * {@link #getRatingType()}.
     *
     * @param rating The rating to set for the current content
     */
    CARAPI SetRating(
        /* [in] */ IRating * rating);

    /**
     * Send a custom action back for the {@link MediaSession} to perform.
     *
     * @param customAction The action to perform.
     * @param args Optional arguments to supply to the {@link MediaSession} for this
     *             custom action.
     */
    CARAPI SendCustomAction(
        /* [in] */ IPlaybackStateCustomAction * customAction,
        /* [in] */ IBundle * args);

    /**
     * Send the id and args from a custom action back for the {@link MediaSession} to perform.
     *
     * @see #sendCustomAction(PlaybackState.CustomAction action, Bundle args)
     * @param action The action identifier of the {@link PlaybackState.CustomAction} as
     *               specified by the {@link MediaSession}.
     * @param args Optional arguments to supply to the {@link MediaSession} for this
     *             custom action.
     */
    CARAPI SendCustomAction(
        /* [in] */ const String& action,
        /* [in] */ IBundle * args);

private:
    static String TAG;
    AutoPtr<CMediaController> mHost;
};

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_SESSION_CMEDIACONTROLLERTRANSPORTCONTROLS_H__
