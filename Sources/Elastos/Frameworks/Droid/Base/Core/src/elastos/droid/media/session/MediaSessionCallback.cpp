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

#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "elastos/droid/media/session/MediaSessionCallback.h"
#include "elastos/droid/media/session/CMediaSession.h"

using Elastos::Droid::View::IKeyEvent;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CAR_INTERFACE_IMPL(MediaSessionCallback, Object, IMediaSessionCallback)

MediaSessionCallback::MediaSessionCallback()
{
}

MediaSessionCallback::~MediaSessionCallback()
{
}

ECode MediaSessionCallback::constructor()
{
    return NOERROR;
}

ECode MediaSessionCallback::OnCommand(
    /* [in] */ const String& command,
    /* [in] */ IBundle * arg,
    /* [in] */ IResultReceiver * cb)
{
    return NOERROR;
}

ECode MediaSessionCallback::OnMediaButtonEvent(
    /* [in] */ IIntent * mediaButtonIntent,
    /* [out] */ Boolean * result)
{
    String action;
    mediaButtonIntent->GetAction(&action);
    if (mSession != NULL && action.Equals(IIntent::ACTION_MEDIA_BUTTON)) {
        AutoPtr<IParcelable> parcelable;
        mediaButtonIntent->GetParcelableExtra(IIntent::EXTRA_KEY_EVENT, (IParcelable**)&parcelable);
        AutoPtr<IKeyEvent> ke = IKeyEvent::Probe(parcelable);
        if (ke != NULL) {
            Int32 ac;
            ke->GetAction(&ac);
            if (ac == IKeyEvent::ACTION_DOWN) {
                AutoPtr<IPlaybackState> state = ((CMediaSession*)mSession.Get())->mPlaybackState;
                Int64 validActions;
                if(state == NULL) {
                    validActions = 0;
                } else {
                    state->GetActions(&validActions);
                }
                Int32 keyCode;
                ke->GetKeyCode(&keyCode);
                switch (keyCode) {
                    case IKeyEvent::KEYCODE_MEDIA_PLAY:
                        if ((validActions & IPlaybackState::ACTION_PLAY) != 0) {
                            OnPlay();
                            *result = TRUE;
                            return NOERROR;
                        }
                        break;
                    case IKeyEvent::KEYCODE_MEDIA_PAUSE:
                        if ((validActions & IPlaybackState::ACTION_PAUSE) != 0) {
                            OnPause();
                            *result = TRUE;
                            return NOERROR;
                        }
                        break;
                    case IKeyEvent::KEYCODE_MEDIA_NEXT:
                        if ((validActions & IPlaybackState::ACTION_SKIP_TO_NEXT) != 0) {
                            OnSkipToNext();
                            *result = TRUE;
                            return NOERROR;
                        }
                        break;
                    case IKeyEvent::KEYCODE_MEDIA_PREVIOUS:
                        if ((validActions & IPlaybackState::ACTION_SKIP_TO_PREVIOUS) != 0) {
                            OnSkipToPrevious();
                            *result = TRUE;
                            return NOERROR;
                        }
                        break;
                    case IKeyEvent::KEYCODE_MEDIA_STOP:
                        if ((validActions & IPlaybackState::ACTION_STOP) != 0) {
                            OnStop();
                            *result = TRUE;
                            return NOERROR;
                        }
                        break;
                    case IKeyEvent::KEYCODE_MEDIA_FAST_FORWARD:
                        if ((validActions & IPlaybackState::ACTION_FAST_FORWARD) != 0) {
                            OnFastForward();
                            *result = TRUE;
                            return NOERROR;
                        }
                        break;
                    case IKeyEvent::KEYCODE_MEDIA_REWIND:
                        if ((validActions & IPlaybackState::ACTION_REWIND) != 0) {
                            OnRewind();
                            *result = TRUE;
                            return NOERROR;
                        }
                        break;
                    case IKeyEvent::KEYCODE_MEDIA_PLAY_PAUSE:
                    case IKeyEvent::KEYCODE_HEADSETHOOK: {
                        Int32 ss;
                        state->GetState(&ss);

                        Boolean isPlaying = state == NULL ? FALSE
                                : ss == IPlaybackState::STATE_PLAYING;
                        Boolean canPlay = (validActions & (IPlaybackState::ACTION_PLAY_PAUSE
                                | IPlaybackState::ACTION_PLAY)) != 0;
                        Boolean canPause = (validActions & (IPlaybackState::ACTION_PLAY_PAUSE
                                | IPlaybackState::ACTION_PAUSE)) != 0;
                        if (isPlaying && canPause) {
                            OnPause();
                            *result = TRUE;
                            return NOERROR;
                        }
                        else if (!isPlaying && canPlay) {
                            OnPlay();
                            *result = TRUE;
                            return NOERROR;
                        }
                        break;
                    }
                }
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode MediaSessionCallback::OnPlay()
{
    return NOERROR;
}

ECode MediaSessionCallback::OnPlayFromMediaId(
    /* [in] */ const String& mediaId,
    /* [in] */ IBundle * extras)
{
    return NOERROR;
}

ECode MediaSessionCallback::OnPlayFromSearch(
    /* [in] */ const String& query,
    /* [in] */ IBundle * extras)
{
    return NOERROR;
}

ECode MediaSessionCallback::OnSkipToQueueItem(
    /* [in] */ Int64 id)
{
    return NOERROR;
}

ECode MediaSessionCallback::OnPause()
{
    return NOERROR;
}

ECode MediaSessionCallback::OnSkipToNext()
{
    return NOERROR;
}

ECode MediaSessionCallback::OnSkipToPrevious()
{
    return NOERROR;
}

ECode MediaSessionCallback::OnFastForward()
{
    return NOERROR;
}

ECode MediaSessionCallback::OnRewind()
{
    return NOERROR;
}

ECode MediaSessionCallback::OnStop()
{
    return NOERROR;
}

ECode MediaSessionCallback::OnSeekTo(
    /* [in] */ Int64 pos)
{
    return NOERROR;
}

ECode MediaSessionCallback::OnSetRating(
    /* [in] */ IRating * rating)
{
    return NOERROR;
}

ECode MediaSessionCallback::OnCustomAction(
    /* [in] */ const String& action,
    /* [in] */ IBundle* extras)
{
    return NOERROR;
}

ECode MediaSessionCallback::SetBrowsedPlayer()
{
    return NOERROR;
}

ECode MediaSessionCallback::SetPlayItem(
    /* [in] */ Int32 scope,
    /* [in] */ Int64 uid)
{
    return NOERROR;
}

ECode MediaSessionCallback::GetNowPlayingEntries()
{
    return NOERROR;
}

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos
