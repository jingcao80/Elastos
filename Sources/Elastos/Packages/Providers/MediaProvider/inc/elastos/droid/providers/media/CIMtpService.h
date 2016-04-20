#ifndef _ELASTOS_DROID_PROVIDERS_MEDIA_CIMTPSERVICE_H__
#define _ELASTOS_DROID_PROVIDERS_MEDIA_CIMTPSERVICE_H__

#include "_Elastos_Droid_Providers_Media_CIMtpService.h"
#include "elastos/droid/providers/media/MtpService.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

CarClass(CIMtpService), public MtpService::MyIMtpService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_PROVIDERS_MEDIA_CIMTPSERVICE_H__
