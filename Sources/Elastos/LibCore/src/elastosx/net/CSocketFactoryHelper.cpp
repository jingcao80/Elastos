
#include "CSocketFactoryHelper.h"
#include "SocketFactory.h"

namespace Elastosx {
namespace Net {

CAR_INTERFACE_IMPL(CSocketFactoryHelper, Singleton, ISocketFactoryHelper)

CAR_SINGLETON_IMPL(CSocketFactoryHelper)

ECode CSocketFactoryHelper::GetDefault(
    /* [out] */ ISocketFactory** factory)
{
    return SocketFactory::GetDefault(factory);
}

} // namespace Net
} // namespace Elastosx
