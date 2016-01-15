
#ifndef __ELASTOS_DROID_MEDIA_CAUDIOSERVICEHELPER_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOSERVICEHELPER_H__

#include "_Elastos_Droid_Media_CAudioServiceHelper.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CAudioServiceHelper)
{
public:
    CAudioServiceHelper();

    ~CAudioServiceHelper();

    /**
     * @see #setVibrateSetting(int, int);
     */
    CARAPI GetValueForVibrateSetting(
        /* [in] */ Int32 existingValue,
        /* [in] */ Int32 vibrateType,
        /* [in] */ Int32 vibrateSetting,
        /* [out] */ Int32* result);
};

} // namespace Media
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CAUDIOSERVICEHELPER_H__
