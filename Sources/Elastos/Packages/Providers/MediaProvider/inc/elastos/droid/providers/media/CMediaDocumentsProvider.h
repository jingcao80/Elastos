#ifndef _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIADOCUMETSPROVIDER_H__
#define _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIADOCUMETSPROVIDER_H__

#include "_Elastos_Droid_Providers_Media_CMediaDocumentsProvider.h"
#include "elastos/droid/providers/media/MediaDocumentsProvider.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

CarClass(CMediaDocumentsProvider)
    , public MediaDocumentsProvider
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIADOCUMETSPROVIDER_H__