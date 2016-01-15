
#ifndef __ELASTOS_DROID_NET_CETHERNETMANAGER_H__
#define __ELASTOS_DROID_NET_CETHERNETMANAGER_H__

#include "_Elastos_Droid_Net_CEthernetManager.h"
#include "elastos/droid/net/EthernetManager.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A class representing the IP configuration of the Ethernet network.
 *
 * @hide
 */
CarClass(CEthernetManager)
    , public EthernetManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CETHERNETMANAGER_H__
