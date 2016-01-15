#ifndef __ELASTOS_NET_PLAINSERVERSOCKETIMPL_H__
#define __ELASTOS_NET_PLAINSERVERSOCKETIMPL_H__

#include "PlainSocketImpl.h"

namespace Elastos {
namespace Net {

class PlainServerSocketImpl
    : public PlainSocketImpl
    , public IPlainServerSocketImpl
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IFileDescriptor *fd);

    CARAPI Create(
        /* [in] */ Boolean isStreaming);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_PLAINSERVERSOCKETIMPL_H__
