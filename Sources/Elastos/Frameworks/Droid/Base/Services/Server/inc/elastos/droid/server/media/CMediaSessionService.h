#ifndef __ELASTOS_DROID_SERVER_MEDIA_CMEDIASESSIONSERVICE_H__
#define __ELASTOS_DROID_SERVER_MEDIA_CMEDIASESSIONSERVICE_H__

#include "_Elastos_Droid_Server_Media_CMediaSessionService.h"
#include "elastos/droid/server/media/MediaSessionService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

CarClass(CMediaSessionService)
    , public MediaSessionService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_MEDIA_CMEDIASESSIONSERVICE_H__
