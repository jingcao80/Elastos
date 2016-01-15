#ifndef __ELASTOS_DROID_MEDIA_CAUDIOATTRIBUTESHELPER_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOATTRIBUTESHELPER_H__

#include "_Elastos_Droid_Media_CAudioAttributesHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CAudioAttributesHelper)
    , public Singleton
    , public IAudioAttributesHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /** @hide */
    CARAPI UsageToString(
        /* [in] */ Int32 usage,
        /* [out] */ String* result);

    /** @hide */
    CARAPI UsageForLegacyStreamType(
        /* [in] */ Int32 streamType,
        /* [out] */ Int32* result);

    /** @hide */
    CARAPI ToLegacyStreamType(
        /* [in] */ IAudioAttributes* aa,
        /* [out] */ Int32* result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CAUDIOATTRIBUTESHELPER_H__
