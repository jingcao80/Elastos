#ifndef _ELASTOS_DROID_MEDIA_CSRTRENDERER_H_
#define _ELASTOS_DROID_MEDIA_CSRTRENDERER_H_

#include "_Elastos_Droid_Media_CSRTRenderer.h"
#include "elastos/droid/media/SRTRenderer.h"

namespace Elastos{
namespace Droid {
namespace Media {

CarClass(CSRTRenderer)
    , public SRTRenderer
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Elastos
} // namespace Droid
} // namespace Media

#endif // _ELASTOS_DROID_MEDIA_CSRTRENDERER_H_