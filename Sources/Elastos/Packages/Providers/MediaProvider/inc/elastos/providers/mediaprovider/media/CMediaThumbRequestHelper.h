#ifndef _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMEDIATHUMBREQUESTHELPER_H__
#define _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMEDIATHUMBREQUESTHELPER_H__

#include "_Elastos_Providers_MediaProvider_Media_CMediaThumbRequestHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::IComparator;

namespace Elastos {
namespace Providers {
namespace MediaProvider {
namespace Media {

CarClass(CMediaThumbRequestHelper)
    , public Object
    , public IMediaThumbRequestHelper
{
public:
    CMediaThumbRequestHelper();

    virtual ~CMediaThumbRequestHelper();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetComparator(
        /* [out] */ IComparator** result);
};

} // namespace Media
} // namespace MediaProvider
} // namespace Providers
} // namespace Elastos

#endif // _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIA_CMEDIATHUMBREQUESTHELPER_H__