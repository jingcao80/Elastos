#ifndef __ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONCALLBACKSTUB_H__
#define __ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONCALLBACKSTUB_H__

#include "_Elastos_Droid_Media_Session_CMediaSessionCallbackStub.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IResultReceiver;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

/**
 * @hide
 */
CarClass(CMediaSessionCallbackStub)
    , public Object
    , public IISessionCallback
    , public IBinder
{
public:
    CMediaSessionCallbackStub();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CMediaSessionCallbackStub();

    CARAPI constructor(
        /* [in] */ IMediaSession * session);

    CARAPI OnCommand(
        /* [in] */ const String& command,
        /* [in] */ IBundle * args,
        /* [in] */ IResultReceiver * cb);

    CARAPI OnMediaButton(
        /* [in] */ IIntent * mediaButtonIntent,
        /* [in] */ Int32 sequenceNumber,
        /* [in] */ IResultReceiver * cb);

    // These callbacks are for the TransportPerformer
    CARAPI OnPlay();

    CARAPI OnPlayFromMediaId(
        /* [in] */ const String& mediaId,
        /* [in] */ IBundle * extras);

    CARAPI OnPlayFromSearch(
        /* [in] */ const String& query,
        /* [in] */ IBundle * extras);

    CARAPI OnSkipToTrack(
        /* [in] */ Int64 id);

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnNext();

    CARAPI OnPrevious();

    CARAPI OnFastForward();

    CARAPI OnRewind();

    CARAPI OnSeekTo(
        /* [in] */ Int64 pos);

    CARAPI OnRate(
        /* [in] */ IRating * rating);

    // @Override
    CARAPI SetRemoteControlClientBrowsedPlayer();

    // @Override
    CARAPI SetRemoteControlClientPlayItem(
        /* [in] */ Int64 uid,
        /* [in] */ Int32 scope);

    // @Override
    CARAPI GetRemoteControlClientNowPlayingEntries();

    CARAPI OnCustomAction(
        /* [in] */ const String& action,
        /* [in] */ IBundle * args);

    // These callbacks are for volume handling
    CARAPI OnAdjustVolume(
        /* [in] */ Int32 direction);

    CARAPI OnSetVolumeTo(
        /* [in] */ Int32 value);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    AutoPtr<IWeakReference> mMediaSession;
    static const String TAG;
};

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONCALLBACKSTUB_H__
