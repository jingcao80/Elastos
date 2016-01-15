
#ifndef __ELASTOS_NET_CPLAINSERVERSOCKETIMPL_H__
#define __ELASTOS_NET_CPLAINSERVERSOCKETIMPL_H__

#include "_Elastos_Net_CPlainServerSocketImpl.h"
#include "PlainServerSocketImpl.h"

namespace Elastos {
namespace Net {

CarClass(CPlainServerSocketImpl) , public PlainServerSocketImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CPLAINSERVERSOCKETIMPL_H__
