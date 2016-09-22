
#include "CKeyFactoryHelper.h"
#include "CKeyFactory.h"

namespace Elastos {
namespace Security {

CAR_INTERFACE_IMPL(CKeyFactoryHelper, Singleton, IKeyFactoryHelper)
CAR_SINGLETON_IMPL(CKeyFactoryHelper)

ECode CKeyFactoryHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IKeyFactory** instance)
{
    return CKeyFactory::GetInstance(algorithm, instance);
}

ECode CKeyFactoryHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IKeyFactory** instance)
{
    return CKeyFactory::GetInstance(algorithm, provider, instance);
}

ECode CKeyFactoryHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider* provider,
    /* [out] */ IKeyFactory** instance)
{
    return CKeyFactory::GetInstance(algorithm, provider, instance);
}

}
}
