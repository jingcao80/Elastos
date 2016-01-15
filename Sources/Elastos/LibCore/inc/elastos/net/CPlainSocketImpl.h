
#ifndef __ELASTOS_NET_CPLAINSOCKETIMPL_H__
#define __ELASTOS_NET_CPLAINSOCKETIMPL_H__

#include "_Elastos_Net_CPlainSocketImpl.h"
#include "PlainSocketImpl.h"

namespace Elastos {
namespace Net {

CarClass(CPlainSocketImpl) , public PlainSocketImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CPLAINSOCKETIMPL_H__
