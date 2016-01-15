
#ifndef __ELASTOS_DROID_NET_CUIDRANGEHELPER_H__
#define __ELASTOS_DROID_NET_CUIDRANGEHELPER_H__

#include "_Elastos_Droid_Net_CUidRangeHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * An inclusive range of UIDs.
 *
 * @hide
 */
CarClass(CUidRangeHelper)
    , public Singleton
    , public IUidRangeHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI CreateForUser(
        /* [in] */ Int32 userId,
        /* [out] */ IUidRange** result);
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_CUIDRANGEHELPER_H__
