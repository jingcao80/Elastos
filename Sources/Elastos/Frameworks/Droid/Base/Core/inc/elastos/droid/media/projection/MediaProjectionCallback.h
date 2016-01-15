#ifndef __ELASTOS_DROID_MEDIA_PROJECTION_MEDIAPROJECTIONCALLBACK_H__
#define __ELASTOS_DROID_MEDIA_PROJECTION_MEDIAPROJECTIONCALLBACK_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace Projection {

class MediaProjectionCallback
    : public Object
    , public IMediaProjectionCallback
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Called when the MediaProjection session is no longer valid.
     * <p>
     * Once a MediaProjection has been stopped, it's up to the application to release any
     * resources it may be holding (e.g. {@link android.hardware.display.VirtualDisplay}s).
     * </p>
     */
    CARAPI OnStop();

};

} // namespace Projection
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_PROJECTION_MEDIAPROJECTIONCALLBACK_H__
