#ifndef _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIAPROVIDER_H__
#define _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIAPROVIDER_H__

#include "_Elastos_Droid_Providers_Media_CMediaProvider.h"
#include "elastos/droid/providers/media/MediaProvider.h"

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

CarClass(CMediaProvider), public MediaProvider
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIAPROVIDER_H__