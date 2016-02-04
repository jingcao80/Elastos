
#ifndef __ELASTOS_DROID_MEDIA_CAUDIOSERVICEHELPER_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOSERVICEHELPER_H__

#include "_Elastos_Droid_Media_CAudioServiceHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CAudioServiceHelper)
    , public Singleton
    , public IAudioServiceHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /** @hide */
    CARAPI StreamToString(
        /* [in] */ Int32 stream,
        /* [out] */ String* result);

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
