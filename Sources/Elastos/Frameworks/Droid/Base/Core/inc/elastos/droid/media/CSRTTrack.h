#ifndef _ELASTOS_DROID_MEDIA_CSRTTRACK_H__
#define _ELASTOS_DROID_MEDIA_CSRTTRACK_H__

#include "_Elastos_Droid_Media_CSRTTrack.h"
#include "elastos/droid/media/SRTTrack.h"

namespace Elastos{
namespace Droid {
namespace Media {

CarClass(CSRTTrack)
    , public SRTTrack
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Elastos
} // namespace Droid
} // namespace Media

#endif // _ELASTOS_DROID_MEDIA_CSRTTRACK_H__