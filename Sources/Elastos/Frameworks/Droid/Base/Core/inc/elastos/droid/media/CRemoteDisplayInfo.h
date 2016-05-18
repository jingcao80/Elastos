#ifndef _ELASTOS_DROID_MEDIA_CREMOTEDISPLAYINFO_H__
#define _ELASTOS_DROID_MEDIA_CREMOTEDISPLAYINFO_H__

#include "_Elastos_Droid_Media_CRemoteDisplayInfo.h"
#include "elastos/droid/media/RemoteDisplayState.h"

namespace Elastos{
namespace Droid {
namespace Media {

CarClass(CRemoteDisplayInfo)
    , public RemoteDisplayState::RemoteDisplayInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Elastos
} // namespace Droid
} // namespace Media

#endif // _ELASTOS_DROID_MEDIA_CREMOTEDISPLAYINFO_H__