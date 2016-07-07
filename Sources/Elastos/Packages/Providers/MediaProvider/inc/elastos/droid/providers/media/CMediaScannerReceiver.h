#ifndef _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIASCANNERRECEIVER_H__
#define _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIASCANNERRECEIVER_H__

#include "_Elastos_Droid_Providers_Media_CMediaScannerReceiver.h"
#include "elastos/droid/providers/media/MediaScannerReceiver.h"

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

CarClass(CMediaScannerReceiver)
    , public MediaScannerReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIASCANNERRECEIVER_H__