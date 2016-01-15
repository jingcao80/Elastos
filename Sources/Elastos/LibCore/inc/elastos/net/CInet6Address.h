
#ifndef __ELASTOS_NET_CINET6ADDRESS_H__
#define __ELASTOS_NET_CINET6ADDRESS_H__

#include "_Elastos_Net_CInet6Address.h"
#include "Inet6Address.h"

namespace Elastos {
namespace Net {

CarClass(CInet6Address)
    , public Inet6Address
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CINET6ADDRESS_H__
