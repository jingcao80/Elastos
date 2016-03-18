#ifndef _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMEDIAPROVIDER_H__
#define _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMEDIAPROVIDER_H__

#include "_Elastos_Providers_MediaProvider_Media_CMediaProvider.h"
#include "elastos/providers/mediaprovider/media/MediaProvider.h"

namespace Elastos {
namespace Providers {
namespace MediaProvider {
namespace Media {

CarClass(CMediaProvider), public MediaProvider
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace MediaProvider
} // namespace Providers
} // namespace Elastos

#endif // _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMEDIAPROVIDER_H__