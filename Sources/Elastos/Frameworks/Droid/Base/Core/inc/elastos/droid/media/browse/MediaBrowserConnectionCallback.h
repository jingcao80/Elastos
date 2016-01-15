#ifndef __ELASTOS_DROID_MEDIA_BROWSE_MEDIABROWSERCONNECTIONCALLBACK_H__
#define __ELASTOS_DROID_MEDIA_BROWSE_MEDIABROWSERCONNECTIONCALLBACK_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace Browse {

class MediaBrowserConnectionCallback
    : public Object
    , public IMediaBrowserConnectionCallback
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Invoked after {@link MediaBrowser#connect()} when the request has successfully completed.
     */
    CARAPI OnConnected();

    /**
     * Invoked when the client is disconnected from the media browser.
     */
    CARAPI OnConnectionSuspended();

    /**
     * Invoked when the connection to the media browser failed.
     */
    CARAPI OnConnectionFailed();

};

} // namespace Browse
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_BROWSE_MEDIABROWSERCONNECTIONCALLBACK_H__
