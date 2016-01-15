
#ifndef __ELASTOS_DROID_NET_CPACPROXYSELECTOR_H__
#define __ELASTOS_DROID_NET_CPACPROXYSELECTOR_H__

#include "_Elastos_Droid_Net_CPacProxySelector.h"
#include "elastos/droid/net/PacProxySelector.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * @hide
 */
CarClass(CPacProxySelector)
    , public PacProxySelector
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CPACPROXYSELECTOR_H__
