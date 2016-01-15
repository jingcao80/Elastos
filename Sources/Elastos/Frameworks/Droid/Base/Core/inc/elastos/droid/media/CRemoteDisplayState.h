#ifndef _ELASTOS_DROID_MEDIA_CREMOTEDISPLAYSTATE_H__
#define _ELASTOS_DROID_MEDIA_CREMOTEDISPLAYSTATE_H__

#include "_Elastos_Droid_Media_CRemoteDisplayState.h"
#include "elastos/droid/media/RemoteDisplayState.h"

namespace Elastos{
namespace Droid {
namespace Media {

CarClass(CRemoteDisplayState)
    , public RemoteDisplayState
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Elastos
} // namespace Droid
} // namespace Media

#endif // _ELASTOS_DROID_MEDIA_CREMOTEDISPLAYSTATE_H__