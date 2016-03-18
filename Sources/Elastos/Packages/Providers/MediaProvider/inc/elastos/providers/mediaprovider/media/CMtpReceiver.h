#ifndef _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMTPRECEIVER_H__
#define _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMTPRECEIVER_H__

#include "_Elastos_Providers_MediaProvider_Media_CMtpReceiver.h"
#include "elastos/providers/mediaprovider/media/MtpReceiver.h"

namespace Elastos {
namespace Providers {
namespace MediaProvider {
namespace Media {

CarClass(CMtpReceiver), public MtpReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace MediaProvider
} // namespace Providers
} // namespace Elastos

#endif // _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMTPRECEIVER_H__