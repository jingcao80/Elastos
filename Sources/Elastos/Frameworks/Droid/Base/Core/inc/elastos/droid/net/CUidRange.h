
#ifndef __ELASTOS_DROID_NET_CUIDRANGE_H__
#define __ELASTOS_DROID_NET_CUIDRANGE_H__

#include "_Elastos_Droid_Net_CUidRange.h"
#include "elastos/droid/net/UidRange.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * An inclusive range of UIDs.
 *
 * @hide
 */
CarClass(CUidRange)
    , public UidRange
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CUIDRANGE_H__
