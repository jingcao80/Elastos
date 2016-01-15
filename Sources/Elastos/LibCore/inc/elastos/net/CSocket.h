
#ifndef __ELASTOS_NET_CSOCKET_H__
#define __ELASTOS_NET_CSOCKET_H__

#include "_Elastos_Net_CSocket.h"
#include "Socket.h"

namespace Elastos {
namespace Net {

CarClass(CSocket) , public Socket
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CSOCKET_H__
