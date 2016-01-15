#ifndef __ELASTOS_DROID_MEDIA_BROWSE_MEDIABROWSERSUBSCRIPTIONCALLBACK_H__
#define __ELASTOS_DROID_MEDIA_BROWSE_MEDIABROWSERSUBSCRIPTIONCALLBACK_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Media.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Browse {

class MediaBrowserSubscriptionCallback
    : public Object
    , public IMediaBrowserSubscriptionCallback
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Called when the list of children is loaded or updated.
     */
    CARAPI OnChildrenLoaded(
        /* [in] */ const String& parentId,
        /* [in] */ IList * children);

    /**
     * Called when the id doesn't exist or other errors in subscribing.
     * <p>
     * If this is called, the subscription remains until {@link MediaBrowser#unsubscribe}
     * called, because some errors may heal themselves.
     * </p>
     */
    CARAPI OnError(
        /* [in] */ const String& id);

};

} // namespace Browse
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_BROWSE_MEDIABROWSERSUBSCRIPTIONCALLBACK_H__
