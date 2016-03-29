
#ifndef __ELASTOS_NET_CINETUNIXADDRESS_H__
#define __ELASTOS_NET_CINETUNIXADDRESS_H__

#include "_Elastos_Net_CInetUnixAddress.h"
#include "InetUnixAddress.h"

namespace Elastos {
namespace Net {

CarClass(CInetUnixAddress)
    , public InetUnixAddress
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CINETUNIXADDRESS_H__
