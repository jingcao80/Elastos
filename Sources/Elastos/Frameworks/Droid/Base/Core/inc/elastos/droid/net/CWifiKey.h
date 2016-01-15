
#ifndef __ELASTOS_DROID_NET_CWIFIKEY_H__
#define __ELASTOS_DROID_NET_CWIFIKEY_H__

#include "_Elastos_Droid_Net_CWifiKey.h"
#include "elastos/droid/net/WifiKey.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Information identifying a Wi-Fi network.
 * @see NetworkKey
 *
 * @hide
 */
// @SystemApi
CarClass(CWifiKey)
    , public WifiKey
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CWIFIKEY_H__
