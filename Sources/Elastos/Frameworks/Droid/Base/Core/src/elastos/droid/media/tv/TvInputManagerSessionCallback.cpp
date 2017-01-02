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

#include "Elastos.Droid.Media.h"
#include "elastos/droid/media/tv/TvInputManagerSessionCallback.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CAR_INTERFACE_IMPL(TvInputManagerSessionCallback, Object, ITvInputManagerSessionCallback)

TvInputManagerSessionCallback::TvInputManagerSessionCallback()
{
}

TvInputManagerSessionCallback::~TvInputManagerSessionCallback()
{
}

ECode TvInputManagerSessionCallback::constructor()
{
    return NOERROR;
}

ECode TvInputManagerSessionCallback::OnSessionCreated(
    /* [in] */ ITvInputManagerSession* session)
{
    return NOERROR;
}

ECode TvInputManagerSessionCallback::OnSessionReleased(
    /* [in] */ ITvInputManagerSession* session)
{
    return NOERROR;
}

ECode TvInputManagerSessionCallback::OnChannelRetuned(
    /* [in] */ ITvInputManagerSession* session,
    /* [in] */ IUri* channelUri)
{
    return NOERROR;
}

ECode TvInputManagerSessionCallback::OnTracksChanged(
    /* [in] */ ITvInputManagerSession* session,
    /* [in] */ IList* tracks)
{
    return NOERROR;
}

ECode TvInputManagerSessionCallback::OnTrackSelected(
    /* [in] */ ITvInputManagerSession* session,
    /* [in] */ Int32 type,
    /* [in] */ const String& trackId)
{
    return NOERROR;
}

ECode TvInputManagerSessionCallback::OnVideoAvailable(
    /* [in] */ ITvInputManagerSession* session)
{
    return NOERROR;
}

ECode TvInputManagerSessionCallback::OnVideoUnavailable(
    /* [in] */ ITvInputManagerSession* session,
    /* [in] */ Int32 reason)
{
    return NOERROR;
}

ECode TvInputManagerSessionCallback::OnContentAllowed(
    /* [in] */ ITvInputManagerSession* session)
{
    return NOERROR;
}

ECode TvInputManagerSessionCallback::OnContentBlocked(
    /* [in] */ ITvInputManagerSession* session,
    /* [in] */ ITvContentRating* rating)
{
    return NOERROR;
}

ECode TvInputManagerSessionCallback::OnLayoutSurface(
    /* [in] */ ITvInputManagerSession* session,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    return NOERROR;
}

ECode TvInputManagerSessionCallback::OnSessionEvent(
    /* [in] */ ITvInputManagerSession* session,
    /* [in] */ const String& eventType,
    /* [in] */ IBundle* eventArgs)
{
    return NOERROR;
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
