#ifndef __ELASTOS_DROID_MEDIA_SESSION_MEDIASESSIONCALLBACK_H__
#define __ELASTOS_DROID_MEDIA_SESSION_MEDIASESSIONCALLBACK_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IResultReceiver;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

/**
 * Receives media buttons, transport controls, and commands from controllers
 * and the system. A callback may be set using {@link #setCallback}.
 */
class MediaSessionCallback
    : public Object
    , public IMediaSessionCallback
{
public:
    CAR_INTERFACE_DECL()

    MediaSessionCallback();

    virtual ~MediaSessionCallback();

    CARAPI constructor();

    /**
     * Called when a controller has sent a command to this session.
     * The owner of the session may handle custom commands but is not
     * required to.
     *
     * @param command The command name.
     * @param args Optional parameters for the command, may be null.
     * @param cb A result receiver to which a result may be sent by the command, may be null.
     */
    CARAPI OnCommand(
        /* [in] */ const String& command,
        /* [in] */ IBundle * arg,
        /* [in] */ IResultReceiver * cb);

    /**
     * Called when a media button is pressed and this session has the
     * highest priority or a controller sends a media button event to the
     * session. The default behavior will call the relevant method if the
     * action for it was set.
     * <p>
     * The intent will be of type {@link Intent#ACTION_MEDIA_BUTTON} with a
     * KeyEvent in {@link Intent#EXTRA_KEY_EVENT}
     *
     * @param mediaButtonIntent an intent containing the KeyEvent as an
     *            extra
     * @return True if the event was handled, false otherwise.
     */
    CARAPI OnMediaButtonEvent(
        /* [in] */ IIntent * mediaButtonIntent,
        /* [out] */ Boolean * result);

    /**
     * Override to handle requests to begin playback.
     */
    CARAPI OnPlay();

    /**
     * Override to handle requests to play a specific mediaId that was
     * provided by your app's {@link MediaBrowserService}.
     */
    CARAPI OnPlayFromMediaId(
        /* [in] */ const String& mediaId,
        /* [in] */ IBundle * extras);

    /**
     * Override to handle requests to begin playback from a search query. An
     * empty query indicates that the app may play any music. The
     * implementation should attempt to make a smart choice about what to
     * play.
     */
    CARAPI OnPlayFromSearch(
        /* [in] */ const String& query,
        /* [in] */ IBundle * extras);

    /**
     * Override to handle requests to play an item with a given id from the
     * play queue.
     */
    CARAPI OnSkipToQueueItem(
        /* [in] */ Int64 id);

    /**
     * Override to handle requests to pause playback.
     */
    CARAPI OnPause();

    /**
     * Override to handle requests to skip to the next media item.
     */
    CARAPI OnSkipToNext();

    /**
     * Override to handle requests to skip to the previous media item.
     */
    CARAPI OnSkipToPrevious();

    /**
     * Override to handle requests to fast forward.
     */
    CARAPI OnFastForward();

    /**
     * Override to handle requests to rewind.
     */
    CARAPI OnRewind();

    /**
     * Override to handle requests to stop playback.
     */
    CARAPI OnStop();

    /**
     * Override to handle requests to seek to a specific position in ms.
     *
     * @param pos New position to move to, in milliseconds.
     */
    CARAPI OnSeekTo(
        /* [in] */ Int64 pos);

    /**
     * Override to handle the item being rated.
     *
     * @param rating
     */
    CARAPI OnSetRating(
        /* [in] */ IRating * rating);

    /**
     * Called when a {@link MediaController} wants a {@link PlaybackState.CustomAction} to be
     * performed.
     *
     * @param action The action that was originally sent in the
     *               {@link PlaybackState.CustomAction}.
     * @param extras Optional extras specified by the {@link MediaController}.
     */
    CARAPI OnCustomAction(
        /* [in] */ const String& action,
        /* [in] */ IBundle* extras);

    CARAPI SetBrowsedPlayer();

    CARAPI SetPlayItem(
        /* [in] */ Int32 scope,
        /* [in] */ Int64 uid);

    CARAPI GetNowPlayingEntries();

public:
    AutoPtr<IMediaSession> mSession;
};

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_SESSION_MEDIASESSIONCALLBACK_H__
