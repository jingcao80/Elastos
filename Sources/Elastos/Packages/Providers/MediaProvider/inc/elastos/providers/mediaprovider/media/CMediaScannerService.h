#ifndef _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMEDIASCANNERSERVICE_H__
#define _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMEDIASCANNERSERVICE_H__

#include "_Elastos_Providers_MediaProvider_Media_CMediaScannerService.h"
#include "elastos/providers/mediaprovider/media/MediaScannerService.h"

namespace Elastos {
namespace Providers {
namespace MediaProvider {
namespace Media {

CarClass(CMediaScannerService), public MediaScannerService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace MediaProvider
} // namespace Providers
} // namespace Elastos

#endif // _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMEDIASCANNERSERVICE_H__