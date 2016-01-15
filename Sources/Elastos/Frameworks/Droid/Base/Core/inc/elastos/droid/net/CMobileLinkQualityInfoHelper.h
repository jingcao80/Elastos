
#ifndef __ELASTOS_DROID_NET_CMOBILELINKQUALITYINFOHELPER_H__
#define __ELASTOS_DROID_NET_CMOBILELINKQUALITYINFOHELPER_H__

#include "_Elastos_Droid_Net_CMobileLinkQualityInfoHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 *  Class that represents useful attributes of mobile network links
 *  such as the upload/download throughput or error rate etc.
 *  @hide
 */
CarClass(CMobileLinkQualityInfoHelper)
    , public Singleton
    , public IMobileLinkQualityInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * @hide
     */
    CARAPI CreateFromParcelBody(
        /* [in] */ IParcel* parcel,
        /* [out] */ IMobileLinkQualityInfo** result);
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_CMOBILELINKQUALITYINFOHELPER_H__
