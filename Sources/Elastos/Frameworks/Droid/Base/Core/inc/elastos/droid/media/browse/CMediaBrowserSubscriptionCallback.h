#ifndef __ELASTOS_DROID_MEDIA_BROWSE_CMEDIABROWSERSUBSCRIPTIONCALLBACK_H__
#define __ELASTOS_DROID_MEDIA_BROWSE_CMEDIABROWSERSUBSCRIPTIONCALLBACK_H__

#include "_Elastos_Droid_Media_Browse_CMediaBrowserSubscriptionCallback.h"
#include "elastos/droid/media/browse/MediaBrowserSubscriptionCallback.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Browse {

CarClass(CMediaBrowserSubscriptionCallback)
    , public MediaBrowserSubscriptionCallback
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Browse
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_BROWSE_CMEDIABROWSERSUBSCRIPTIONCALLBACK_H__
