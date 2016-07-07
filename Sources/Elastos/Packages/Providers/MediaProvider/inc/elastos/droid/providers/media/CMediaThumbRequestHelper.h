#ifndef _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIATHUMBREQUESTHELPER_H__
#define _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIATHUMBREQUESTHELPER_H__

#include "_Elastos_Droid_Providers_Media_CMediaThumbRequestHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::IComparator;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

CarClass(CMediaThumbRequestHelper)
    , public Singleton
    , public IMediaThumbRequestHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetComparator(
        /* [out] */ IComparator** result);
};

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_PROVIDERS_MEDIA_CMEDIATHUMBREQUESTHELPER_H__