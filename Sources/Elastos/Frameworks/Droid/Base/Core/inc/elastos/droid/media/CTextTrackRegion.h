#ifndef __ELASTOS_DROID_MEDIA_CTEXTTRACKREGION_H__
#define __ELASTOS_DROID_MEDIA_CTEXTTRACKREGION_H__

#include "_Elastos_Droid_Media_CTextTrackRegion.h"
#include "elastos/droid/media/TextTrackRegion.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CTextTrackRegion)
    , public TextTrackRegion
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CTEXTTRACKREGION_H__