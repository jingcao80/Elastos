
#ifndef __ELASTOS_NET_CINET4ADDRESS_H__
#define __ELASTOS_NET_CINET4ADDRESS_H__

#include "_Elastos_Net_CInet4Address.h"
#include "Inet4Address.h"

namespace Elastos {
namespace Net {

CarClass(CInet4Address)
    , public Inet4Address
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CINET4ADDRESS_H__
