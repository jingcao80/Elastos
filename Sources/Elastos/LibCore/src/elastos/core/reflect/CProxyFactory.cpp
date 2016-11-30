
#include "elastos/core/reflect/CProxyFactory.h"

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
    assert(0 && "TODO");
    return NOERROR;
}

} // namespace Reflect
} // namespace Core
} // namespace Elastos
