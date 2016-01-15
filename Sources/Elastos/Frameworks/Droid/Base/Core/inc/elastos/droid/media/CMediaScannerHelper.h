
#ifndef __ELASTOS_DROID_MEDIA_CMEDIASCANNERHELPER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIASCANNERHELPER_H__

#include "_Elastos_Droid_Media_CMediaScannerHelper.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaScannerHelper)
{
public:

    CARAPI IsNoMediaPath(
        /* [in] */ const String& path,
        /* [out] */ Boolean* result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIASCANNERHELPER_H__
