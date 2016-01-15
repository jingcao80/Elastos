
#ifndef __ELASTOS_NET_CSOCKS4MESSAGE_H__
#define __ELASTOS_NET_CSOCKS4MESSAGE_H__

#include "_Elastos_Net_CSocks4Message.h"
#include "Socks4Message.h"

namespace Elastos {
namespace Net {

CarClass(CSocks4Message) , public Socks4Message
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CSOCKS4MESSAGE_H__
