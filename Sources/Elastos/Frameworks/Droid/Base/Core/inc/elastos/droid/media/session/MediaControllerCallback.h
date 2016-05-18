
#ifndef __ELASTOS_DROID_MEDIA_SESSION_MEDIACONTROLLERCALLBACK_H__
#define __ELASTOS_DROID_MEDIA_SESSION_MEDIACONTROLLERCALLBACK_H__

#include "Elastos.Droid.Media.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

/**
 * Callback for receiving updates on from the session. A Callback can be
 * registered using {@link #registerCallback}
 */
class ECO_PUBLIC MediaControllerCallback
    : public Object
    , public IMediaControllerCallback
{
public:
    CAR_INTERFACE_DECL()

    MediaControllerCallback();

    virtual ~MediaControllerCallback() = 0;

    /**
     * Override to handle the session being destroyed. The session is no
     * longer valid after this call and calls to it will be ignored.
     */
    CARAPI OnSessionDestroyed();

    /**
     * Override to handle custom events sent by the session owner without a
     * specified interface. Controllers should only handle these for
     * sessions they own.
     *
     * @param event The event from the session.
     * @param extras Optional parameters for the event, may be null.
     */
    CARAPI OnSessionEvent(
        /* [in] */ /*@NonNull*/ const String& event,
        /* [in] */ /*@Nullable*/ IBundle* extras);

    /**
     * Override to handle changes in playback state.
     *
     * @param state The new playback state of the session
     */
    CARAPI OnPlaybackStateChanged(
        /* [in] */ /*@NonNull*/ IPlaybackState* state);

    /**
     * Override to handle changes to the current metadata.
     *
     * @param metadata The current metadata for the session or null if none.
     * @see MediaMetadata
     */
    CARAPI OnMetadataChanged(
        /* [in] */ /*@Nullable*/ IMediaMetadata* metadata);

    /**
     * Override to handle changes to items in the queue.
     *
     * @param queue A list of items in the current play queue. It should
     *            include the currently playing item as well as previous and
     *            upcoming items if applicable.
     * @see MediaSession.QueueItem
     */
    CARAPI OnQueueChanged(
        /* [in] */ /*@Nullable*/ IList/*<MediaSession.QueueItem>*/* queue);

    /**
     * Override to handle changes to the queue title.
     *
     * @param title The title that should be displayed along with the play queue such as
     *              "Now Playing". May be null if there is no such title.
     */
    CARAPI OnQueueTitleChanged(
        /* [in] */ /*@Nullable*/ ICharSequence* title);

    /**
     * Override to handle changes to the {@link MediaSession} extras.
     *
     * @param extras The extras that can include other information associated with the
     *               {@link MediaSession}.
     */
    CARAPI OnExtrasChanged(
        /* [in] */ /*@Nullable*/ IBundle* extras);

    /**
     * Override to handle changes to the audio info.
     *
     * @param info The current audio info for this session.
     */
    CARAPI OnAudioInfoChanged(
        /* [in] */ IMediaControllerPlaybackInfo* info);

    /**
     * @hide
     */
    CARAPI OnUpdateFolderInfoBrowsedPlayer(
        /* [in] */ const String& stringUri);

    /**
     * @hide
     */
    CARAPI OnUpdateNowPlayingEntries(
        /* [in] */ ArrayOf<Int64>* playList);

    /**
     * @hide
     */
    CARAPI OnUpdateNowPlayingContentChange();

    /**
     * @hide
     */
    CARAPI OnPlayItemResponse(
        /* [in] */ Boolean success);
};

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_SESSION_MEDIACONTROLLERCALLBACK_H__
