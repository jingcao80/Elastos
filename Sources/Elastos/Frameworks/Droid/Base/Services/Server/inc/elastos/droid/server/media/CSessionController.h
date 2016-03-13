
#ifndef __ELASTOS_DROID_SERVER_MEDIA_CSESSIONCONTROLLER_H__
#define __ELASTOS_DROID_SERVER_MEDIA_CSESSIONCONTROLLER_H__

#include "_Elastos_Droid_Server_Media_CSessionController.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <elastos/core/Object.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::Pm::IParceledListSlice;
using Elastos::Droid::Media::IMediaMetadata;
using Elastos::Droid::Media::IRating;
using Elastos::Droid::Media::Session::IISessionController;
using Elastos::Droid::Media::Session::IISessionControllerCallback;
using Elastos::Droid::Media::Session::IPlaybackState;
using Elastos::Droid::Media::Session::IParcelableVolumeInfo;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IResultReceiver;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

class MediaSessionRecord;

CarClass(CSessionController)
    , public Object
    , public IISessionController
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 host);

    CARAPI SendCommand(
        /* [in] */ const String& command,
        /* [in] */ IBundle* args,
        /* [in] */ IResultReceiver* cb);

    CARAPI SendMediaButton(
        /* [in] */ IKeyEvent* mediaButton,
        /* [out] */ Boolean* result);

    CARAPI RegisterCallbackListener(
        /* [in] */ IISessionControllerCallback* cb);

    CARAPI UnregisterCallbackListener(
        /* [in] */ IISessionControllerCallback* cb);

    CARAPI GetPackageName(
        /* [out] */ String* result);

    CARAPI GetTag(
        /* [out] */ String* result);

    CARAPI GetLaunchPendingIntent(
        /* [out] */ IPendingIntent** result);

    CARAPI GetFlags(
        /* [out] */ Int64* result);

    CARAPI GetVolumeAttributes(
        /* [out] */ IParcelableVolumeInfo** result);

    CARAPI AdjustVolume(
        /* [in] */ Int32 direction,
        /* [in] */ Int32 flags,
        /* [in] */ const String& packageName);

    CARAPI SetVolumeTo(
        /* [in] */ Int32 value,
        /* [in] */ Int32 flags,
        /* [in] */ const String& packageName);

    // These commands are for the TransportControls
    CARAPI Play();

    CARAPI PlayFromMediaId(
        /* [in] */ const String& mediaId,
        /* [in] */ IBundle* extras);

    CARAPI PlayFromSearch(
        /* [in] */ const String& query,
        /* [in] */ IBundle* extras);

    CARAPI SkipToQueueItem(
        /* [in] */ Int64 id);

    CARAPI Pause();

    CARAPI Stop();

    CARAPI Next();

    CARAPI Previous();

    CARAPI FastForward();

    CARAPI Rewind();

    CARAPI SeekTo(
        /* [in] */ Int64 pos);

    CARAPI SetRemoteControlClientBrowsedPlayer();

    CARAPI SetRemoteControlClientPlayItem(
        /* [in] */ Int64 uid,
        /* [in] */ Int32 scope);

    CARAPI GetRemoteControlClientNowPlayingEntries();

    CARAPI Rate(
        /* [in] */ IRating* rating);

    CARAPI SendCustomAction(
        /* [in] */ const String& action,
        /* [in] */ IBundle* args);

    CARAPI GetMetadata(
        /* [out] */ IMediaMetadata** result);

    CARAPI GetPlaybackState(
        /* [out] */ IPlaybackState** result);

    CARAPI GetQueue(
        /* [out] */ IParceledListSlice** result);

    CARAPI GetQueueTitle(
        /* [out] */ ICharSequence** result);

    CARAPI GetExtras(
        /* [out] */ IBundle** result);

    CARAPI GetRatingType(
        /* [out] */ Int32* result);

    CARAPI IsTransportControlEnabled(
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    MediaSessionRecord* mHost;
};

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_MEDIA_CSESSIONCONTROLLER_H__
