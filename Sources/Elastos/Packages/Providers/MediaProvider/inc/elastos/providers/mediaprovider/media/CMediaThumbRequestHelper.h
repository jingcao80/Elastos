#ifndef _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMEDIATHUMBREQUESTHELPER_H__
#define _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMEDIATHUMBREQUESTHELPER_H__

#include "_Elastos_Providers_MediaProvider_Media_CMediaThumbRequestHelper.h"
#include "elastos/providers/mediaprovider/media/MediaThumbRequest.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Providers {
namespace MediaProvider {
namespace Media {

CarClass(CMediaThumbRequestHelper)
    , public IMediaThumbRequestHelper
    , public Object
{
public:
    CMediaThumbRequestHelper(
        /* [in] */ IMediaThumbRequest* r1,
        /* [in] */ IMediaThumbRequest* r2);

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetComparator(
        /* [out] */ IComparator** result);

private:
    AutoPtr<IMediaThumbRequest> mR1;
    AutoPtr<IMediaThumbRequest> mR2;
};

} // namespace Media
} // namespace MediaProvider
} // namespace Providers
} // namespace Elastos

#endif // _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMEDIATHUMBREQUESTHELPER_H__