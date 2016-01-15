#ifndef _ELASTOS_DROID_MEDIA_CMEDIAROUTERCLIENTSTATE_H__
#define _ELASTOS_DROID_MEDIA_CMEDIAROUTERCLIENTSTATE_H__

#include "_Elastos_Droid_Media_CMediaRouterClientState.h"
#include "elastos/droid/media/MediaRouterClientState.h"

namespace Elastos{
namespace Droid {
namespace Media {

CarClass(CMediaRouterClientState)
    , public MediaRouterClientState
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Elastos
} // namespace Droid
} // namespace Media

#endif // _ELASTOS_DROID_MEDIA_CMEDIAROUTERCLIENTSTATE_H__