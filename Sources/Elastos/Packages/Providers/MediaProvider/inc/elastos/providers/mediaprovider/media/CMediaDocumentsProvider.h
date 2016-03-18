#ifndef _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMEDIADOCUMETSPROVIDER_H__
#define _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMEDIADOCUMETSPROVIDER_H__

#include "_Elastos_Providers_MediaProvider_Media_CMediaDocumentsProvider.h"
#include "elastos/providers/mediaprovider/media/MediaDocumentsProvider.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Providers {
namespace MediaProvider {
namespace Media {

CarClass(CMediaDocumentsProvider), public MediaDocumentsProvider
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace MediaProvider
} // namespace Providers
} // namespace Elastos

#endif // _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMEDIADOCUMETSPROVIDER_H__