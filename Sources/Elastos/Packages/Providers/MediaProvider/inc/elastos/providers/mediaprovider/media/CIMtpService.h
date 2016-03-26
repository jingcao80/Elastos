#ifndef _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CIMTPSERVICE_H__
#define _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CIMTPSERVICE_H__

#include "_Elastos_Providers_MediaProvider_Media_CIMtpService.h"
#include "elastos/providers/mediaprovider/media/MtpService.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Providers {
namespace MediaProvider {
namespace Media {

CarClass(CIMtpService), public MtpService::MyIMtpService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace MediaProvider
} // namespace Providers
} // namespace Elastos

#endif // _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CIMTPSERVICE_H__
