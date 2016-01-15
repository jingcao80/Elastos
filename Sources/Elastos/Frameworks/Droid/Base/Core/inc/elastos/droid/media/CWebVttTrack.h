#ifndef __ELASTOS_DROID_MEDIA_CWEBVTTTRACK_H__
#define __ELASTOS_DROID_MEDIA_CWEBVTTTRACK_H__

#include "_Elastos_Droid_Media_CWebVttTrack.h"
#include "elastos/droid/media/WebVttTrack.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CWebVttTrack)
    , public WebVttTrack
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CWEBVTTTRACK_H__