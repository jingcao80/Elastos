#ifndef _ELASTOS_DROID_MEDIA_CTTMLRENDERER_H__
#define _ELASTOS_DROID_MEDIA_CTTMLRENDERER_H__

#include "_Elastos_Droid_Media_CTtmlRenderer.h"
#include "elastos/droid/media/TtmlRenderer.h"

namespace Elastos{
namespace Droid {
namespace Media {

CarClass(CTtmlRenderer)
    , public TtmlRenderer
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Elastos
} // namespace Droid
} // namespace Media

#endif // _ELASTOS_DROID_MEDIA_CTTMLRENDERER_H__