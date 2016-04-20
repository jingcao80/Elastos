#ifndef _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIATHUMBREQUEST_H__
#define _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIATHUMBREQUEST_H__

#include "_Elastos_Droid_Providers_Media_CMediaThumbRequest.h"
#include "elastos/droid/providers/media/MediaThumbRequest.h"

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

CarClass(CMediaThumbRequest), public MediaThumbRequest
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIATHUMBREQUEST_H__