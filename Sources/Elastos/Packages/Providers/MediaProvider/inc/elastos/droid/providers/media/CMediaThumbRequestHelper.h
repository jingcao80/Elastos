#ifndef _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIATHUMBREQUESTHELPER_H__
#define _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIATHUMBREQUESTHELPER_H__

#include "_Elastos_Droid_Providers_Media_CMediaThumbRequestHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::IComparator;

namespace Elastos {
namespace Droid {
namespace Providers {
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
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIATHUMBREQUESTHELPER_H__