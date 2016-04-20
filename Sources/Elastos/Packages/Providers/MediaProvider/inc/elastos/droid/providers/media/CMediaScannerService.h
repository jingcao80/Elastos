#ifndef _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIASCANNERSERVICE_H__
#define _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIASCANNERSERVICE_H__

#include "_Elastos_Droid_Providers_Media_CMediaScannerService.h"
#include "elastos/droid/providers/media/MediaScannerService.h"

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

CarClass(CMediaScannerService), public MediaScannerService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIASCANNERSERVICE_H__