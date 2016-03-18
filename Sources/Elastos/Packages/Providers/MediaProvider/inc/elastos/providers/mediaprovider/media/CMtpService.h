#ifndef _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMTPSERVICE_H__
#define _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMTPSERVICE_H__

#include "_Elastos_Providers_MediaProvider_Media_CMtpService.h"
#include "elastos/providers/mediaprovider/media/MtpService.h"

namespace Elastos {
namespace Providers {
namespace MediaProvider {
namespace Media {

CarClass(CMtpService), public MtpService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace MediaProvider
} // namespace Providers
} // namespace Elastos

#endif // _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMTPSERVICE_H__