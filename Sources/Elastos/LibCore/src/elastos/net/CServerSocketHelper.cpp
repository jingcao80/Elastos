
#include "CServerSocketHelper.h"
#include "ServerSocket.h"

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CServerSocketHelper, Singleton, IServerSocketHelper)

CAR_SINGLETON_IMPL(CServerSocketHelper)

ECode CServerSocketHelper::SetSocketFactory(
    /* [in] */ ISocketImplFactory* aFactory)
{
    return ServerSocket::SetSocketFactory(aFactory);
}

} // namespace Net
} // namespace Elastos
