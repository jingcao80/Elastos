
#ifndef __ELASTOS_DROID_MEDIA_CMEDIASCANNERHELPER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIASCANNERHELPER_H__

#include "_Elastos_Droid_Media_CMediaScannerHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaScannerHelper)
    , public Singleton
    , public IMediaScannerHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI IsNoMediaPath(
        /* [in] */ const String& path,
        /* [out] */ Boolean* result);

    CARAPI ClearMediaPathCache(
        /* [in] */ Boolean clearMediaPaths,
        /* [in] */ Boolean clearNoMediaPaths);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIASCANNERHELPER_H__
