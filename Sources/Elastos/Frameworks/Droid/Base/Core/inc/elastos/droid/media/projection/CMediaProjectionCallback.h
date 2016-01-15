#ifndef __ELASTOS_DROID_MEDIA_PROJECTION_CMEDIAPROJECTIONCALLBACK_H__
#define __ELASTOS_DROID_MEDIA_PROJECTION_CMEDIAPROJECTIONCALLBACK_H__

#include "_Elastos_Droid_Media_Projection_CMediaProjectionCallback.h"
#include "elastos/droid/media/projection/MediaProjectionCallback.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Projection {

/**
 * Callbacks for the projection session.
 */
CarClass(CMediaProjectionCallback)
    , public MediaProjectionCallback
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Projection
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_PROJECTION_CMEDIAPROJECTIONCALLBACK_H__
