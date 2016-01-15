#include "Elastos.Droid.Media.h"
#include "elastos/droid/media/browse/MediaBrowserConnectionCallback.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Browse {

CAR_INTERFACE_IMPL(MediaBrowserConnectionCallback, Object, IMediaBrowserConnectionCallback)

ECode MediaBrowserConnectionCallback::OnConnected()
{
    return NOERROR;
}

ECode MediaBrowserConnectionCallback::OnConnectionSuspended()
{
    return NOERROR;
}

ECode MediaBrowserConnectionCallback::OnConnectionFailed()
{
    return NOERROR;
}

} // namespace Browse
} // namespace Media
} // namepsace Droid
} // namespace Elastos
