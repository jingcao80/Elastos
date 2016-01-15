#include "elastos/droid/media/browse/MediaBrowserSubscriptionCallback.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Browse {

CAR_INTERFACE_IMPL(MediaBrowserSubscriptionCallback, Object, IMediaBrowserSubscriptionCallback)

ECode MediaBrowserSubscriptionCallback::OnChildrenLoaded(
    /* [in] */ const String& parentId,
    /* [in] */ IList * children)
{
    return NOERROR;
}

ECode MediaBrowserSubscriptionCallback::OnError(
    /* [in] */ const String& id)
{
    return NOERROR;
}

} // namespace Browse
} // namespace Media
} // namepsace Droid
} // namespace Elastos
