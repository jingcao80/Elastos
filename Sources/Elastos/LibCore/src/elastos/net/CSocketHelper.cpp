
#include "CSocketHelper.h"
#include "Socket.h"

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CSocketHelper, Singleton, ISocketHelper)

CAR_SINGLETON_IMPL(CSocketHelper)

ECode CSocketHelper::SetSocketImplFactory(
    /* [in] */ ISocketImplFactory* fac)
{
    return Socket::SetSocketImplFactory(fac);
}

} // namespace Net
} // namespace Elastos
