
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
