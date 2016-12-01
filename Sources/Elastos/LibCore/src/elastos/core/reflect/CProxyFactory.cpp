
#include "elastos/core/reflect/CProxyFactory.h"
#include "elastos/core/reflect/Proxy.h"

namespace Elastos {
namespace Core {
namespace Reflect {

CAR_INTERFACE_IMPL(CProxyFactory, Singleton, IProxyFactory)
CAR_SINGLETON_IMPL(CProxyFactory)

ECode CProxyFactory::NewProxyInstance(
    /* [in] */ IClassLoader* loader,
    /* [in] */ ArrayOf<IInterfaceInfo*>* interfaces,
    /* [in] */ IInvocationHandler* invocationHandler,
    /* [out] */ IInterface** prxObject)
{
    VALIDATE_NOT_NULL(prxObject);
    return CObjectProxy::S_CreateObject(loader, interfaces, invocationHandler, prxObject);
}

} // namespace Reflect
} // namespace Core
} // namespace Elastos
