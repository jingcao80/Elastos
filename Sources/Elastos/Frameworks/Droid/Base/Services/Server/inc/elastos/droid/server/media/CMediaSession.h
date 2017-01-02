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

#ifndef __ELASTOS_DROID_SERVER_MEDIA_CMEDIASESSION_H__
#define __ELASTOS_DROID_SERVER_MEDIA_CMEDIASESSION_H__

#include "_Elastos_Droid_Server_Media_CMediaSession.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::Pm::IParceledListSlice;
using Elastos::Droid::Media::IMediaMetadata;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Media::Session::IISession;
using Elastos::Droid::Media::Session::IISessionController;
using Elastos::Droid::Media::Session::IPlaybackState;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

class MediaSessionRecord;

CarClass(CMediaSession)
    , public Object
    , public IISession
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IMediaSessionRecord* host);

    CARAPI Destroy();

    CARAPI SendEvent(
        /* [in] */ const String& event,
        /* [in] */ IBundle * data);

    CARAPI GetController(
        /* [out] */ IISessionController** result);

    CARAPI SetActive(
        /* [in] */ Boolean active);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    CARAPI SetMediaButtonReceiver(
        /* [in] */ IPendingIntent* mbr);

    CARAPI SetLaunchPendingIntent(
        /* [in] */ IPendingIntent* pi);

    // These commands are for the TransportPerformer
    CARAPI SetMetadata(
        /* [in] */ IMediaMetadata* metadata);

    CARAPI SetPlaybackState(
        /* [in] */ IPlaybackState* state);

    CARAPI SetQueue(
        /* [in] */ IParceledListSlice* queue);

    CARAPI SetQueueTitle(
        /* [in] */ ICharSequence* title);

    CARAPI SetExtras(
        /* [in] */ IBundle* extras);

    CARAPI SetRatingType(
        /* [in] */ Int32 type);

    CARAPI PlayItemResponse(
        /* [in] */ Boolean success);

    CARAPI UpdateNowPlayingEntries(
        /* [in] */ ArrayOf<Int64>* playList);

    CARAPI UpdateFolderInfoBrowsedPlayer(
        /* [in] */ const String& stringUri);

    CARAPI UpdateNowPlayingContentChange();

    CARAPI SetCurrentVolume(
        /* [in] */ Int32 currentVolume);

    // These commands relate to volume handling
    CARAPI SetPlaybackToLocal(
        /* [in] */ IAudioAttributes* attributes);

    CARAPI SetPlaybackToRemote(
        /* [in] */ Int32 control,
        /* [in] */ Int32 max);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    MediaSessionRecord* mHost;
};

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_MEDIA_CMEDIASESSION_H__
