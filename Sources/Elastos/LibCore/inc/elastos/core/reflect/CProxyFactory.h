
#ifndef __ELASTOS_CORE_REFLECT_CPROXYFACTORY_H__
#define __ELASTOS_CORE_REFLECT_CPROXYFACTORY_H__

#include "_Elastos_Core_Reflect_CProxyFactory.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Core {
namespace Reflect {

CarClass(CProxyFactory)
    , public Singleton
    , public IProxyFactory
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI NewProxyInstance(
        /* [in] */ IClassLoader* loader,
        /* [in] */ ArrayOf<IInterfaceInfo*>* interfaces,
        /* [in] */ IInvocationHandler* invocationHandler,
        /* [out] */ IInterface** prxObject);
};

} // namespace Reflect
} // namespace Core
} // namespace Elastos

#endif // __ELASTOS_CORE_REFLECT_CPROXYFACTORY_H__
