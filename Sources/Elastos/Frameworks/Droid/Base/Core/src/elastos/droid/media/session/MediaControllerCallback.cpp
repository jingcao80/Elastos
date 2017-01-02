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

#include "elastos/droid/media/session/MediaControllerCallback.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CAR_INTERFACE_IMPL(MediaControllerCallback, Object, IMediaControllerCallback);
MediaControllerCallback::MediaControllerCallback()
{}

MediaControllerCallback::~MediaControllerCallback()
{}

ECode MediaControllerCallback::OnSessionDestroyed()
{
    return NOERROR;
}

ECode MediaControllerCallback::OnSessionEvent(
    /* [in] */ /*@NonNull*/ const String& event,
    /* [in] */ /*@Nullable*/ IBundle* extras)
{
    return NOERROR;
}

ECode MediaControllerCallback::OnPlaybackStateChanged(
    /* [in] */ /*@NonNull*/ IPlaybackState* state)
{
    return NOERROR;
}

ECode MediaControllerCallback::OnMetadataChanged(
    /* [in] */ /*@Nullable*/ IMediaMetadata* metadata)
{
    return NOERROR;
}

ECode MediaControllerCallback::OnQueueChanged(
    /* [in] */ /*@Nullable*/ IList/*<MediaSession.QueueItem>*/* queue)
{
    return NOERROR;
}

ECode MediaControllerCallback::OnQueueTitleChanged(
    /* [in] */ /*@Nullable*/ ICharSequence* title)
{
    return NOERROR;
}

ECode MediaControllerCallback::OnExtrasChanged(
    /* [in] */ /*@Nullable*/ IBundle* extras)
{
    return NOERROR;
}

ECode MediaControllerCallback::OnAudioInfoChanged(
    /* [in] */ IMediaControllerPlaybackInfo* info)
{
    return NOERROR;
}

ECode MediaControllerCallback::OnUpdateFolderInfoBrowsedPlayer(
    /* [in] */ const String& stringUri)
{
    return NOERROR;
}

ECode MediaControllerCallback::OnUpdateNowPlayingEntries(
    /* [in] */ ArrayOf<Int64>* playList)
{
    return NOERROR;
}

ECode MediaControllerCallback::OnUpdateNowPlayingContentChange()
{
    return NOERROR;
}

ECode MediaControllerCallback::OnPlayItemResponse(
    /* [in] */ Boolean success)
{
    return NOERROR;
}

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos
