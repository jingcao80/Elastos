#ifndef _ELASTOS_DROID_PROVIDERS_MEDIA_CIMEDIASCANNERSERVICE_H__
#define _ELASTOS_DROID_PROVIDERS_MEDIA_CIMEDIASCANNERSERVICE_H__

#include "_Elastos_Droid_Providers_Media_CIMediaScannerService.h"
#include "elastos/droid/providers/media/MediaScannerService.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

CarClass(CIMediaScannerService), public MediaScannerService::MyMediaScannerService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_PROVIDERS_MEDIA_CIMEDIASCANNERSERVICE_H__
