#ifndef _ELASTOS_DROID_PROVIDERS_MEDIA_CMTPSERVICE_H__
#define _ELASTOS_DROID_PROVIDERS_MEDIA_CMTPSERVICE_H__

#include "_Elastos_Droid_Providers_Media_CMtpService.h"
#include "elastos/droid/providers/media/MtpService.h"

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

CarClass(CMtpService), public MtpService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_PROVIDERS_MEDIA_CMTPSERVICE_H__