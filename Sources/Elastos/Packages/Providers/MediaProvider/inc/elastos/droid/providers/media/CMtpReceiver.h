#ifndef _ELASTOS_DROID_PROVIDERS_MEDIA_CMTPRECEIVER_H__
#define _ELASTOS_DROID_PROVIDERS_MEDIA_CMTPRECEIVER_H__

#include "_Elastos_Droid_Providers_Media_CMtpReceiver.h"
#include "elastos/droid/providers/media/MtpReceiver.h"

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

CarClass(CMtpReceiver), public MtpReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_PROVIDERS_MEDIA_CMTPRECEIVER_H__