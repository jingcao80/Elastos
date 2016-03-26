#ifndef _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CIMEDIASCANNERSERVICE_H__
#define _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CIMEDIASCANNERSERVICE_H__

#include "_Elastos_Providers_MediaProvider_Media_CIMediaScannerService.h"
#include "elastos/providers/mediaprovider/media/MediaScannerService.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Providers {
namespace MediaProvider {
namespace Media {

CarClass(CIMediaScannerService), public MediaScannerService::MyMediaScannerService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace MediaProvider
} // namespace Providers
} // namespace Elastos

#endif // _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CIMEDIASCANNERSERVICE_H__
