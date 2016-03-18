#ifndef _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMEDIASCANNERRECEIVER_H__
#define _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMEDIASCANNERRECEIVER_H__

#include "_Elastos_Providers_MediaProvider_Media_CMediaScannerReceiver.h"
#include "elastos/providers/mediaprovider/media/MediaScannerReceiver.h"

namespace Elastos {
namespace Providers {
namespace MediaProvider {
namespace Media {

CarClass(CMediaScannerReceiver), public MediaScannerReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace MediaProvider
} // namespace Providers
} // namespace Elastos

#endif // _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMEDIASCANNERRECEIVER_H__