#ifndef __ELASTOS_DROID_MEDIA_BROWSE_CMEDIABROWSERCONNECTIONCALLBACK_H__
#define __ELASTOS_DROID_MEDIA_BROWSE_CMEDIABROWSERCONNECTIONCALLBACK_H__

#include "_Elastos_Droid_Media_Browse_CMediaBrowserConnectionCallback.h"
#include "elastos/droid/media/browse/MediaBrowserConnectionCallback.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Browse {

CarClass(CMediaBrowserConnectionCallback)
    , public MediaBrowserConnectionCallback
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Browse
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_BROWSE_CMEDIABROWSERCONNECTIONCALLBACK_H__
