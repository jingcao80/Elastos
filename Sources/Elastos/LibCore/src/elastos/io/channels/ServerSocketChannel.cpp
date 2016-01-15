
#include "ServerSocketChannel.h"

namespace Elastos {
namespace IO {
namespace Channels {

ServerSocketChannel::ServerSocketChannel()
{}

ECode ServerSocketChannel::constructor(
    /* [in] */ ISelectorProvider* provider)
{
    return AbstractSelectableChannel::constructor(provider);
}

ECode ServerSocketChannel::Open(
    /* [out] */ IServerSocketChannel** channel)
{
    return E_NOT_IMPLEMENTED;
}

ECode ServerSocketChannel::GetValidOps(
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = ISelectionKey::OP_ACCEPT;
    return NOERROR;
}

} // namespace Channels
} // namespace IO
} // namespace Elastos
