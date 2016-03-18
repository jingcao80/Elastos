#ifndef _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMEDIAUPGRADERECEIVER_H__
#define _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMEDIAUPGRADERECEIVER_H__

#include "_Elastos_Providers_MediaProvider_Media_CMediaUpgradeReceiver.h"
#include "elastos/providers/mediaprovider/media/MediaUpgradeReceiver.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Providers {
namespace MediaProvider {
namespace Media {

CarClass(CMediaUpgradeReceiver), public MediaUpgradeReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace MediaProvider
} // namespace Providers
} // namespace Elastos

#endif // _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMEDIAUPGRADERECEIVER_H__