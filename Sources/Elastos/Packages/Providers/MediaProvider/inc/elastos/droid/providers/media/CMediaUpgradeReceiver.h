#ifndef _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIAUPGRADERECEIVER_H__
#define _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIAUPGRADERECEIVER_H__

#include "_Elastos_Droid_Providers_Media_CMediaUpgradeReceiver.h"
#include "elastos/droid/providers/media/MediaUpgradeReceiver.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

CarClass(CMediaUpgradeReceiver)
    , public MediaUpgradeReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIAUPGRADERECEIVER_H__