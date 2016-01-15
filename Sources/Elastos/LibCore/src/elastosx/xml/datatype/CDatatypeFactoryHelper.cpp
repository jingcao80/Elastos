#include "CDatatypeFactoryHelper.h"
#include "DatatypeFactory.h"

namespace Elastosx {
namespace Xml {
namespace Datatype {

CAR_SINGLETON_IMPL(CDatatypeFactoryHelper)

CAR_INTERFACE_IMPL(CDatatypeFactoryHelper, Singleton, IDatatypeFactoryHelper)

ECode CDatatypeFactoryHelper::NewInstance(
    /* [out] */ IDatatypeFactory** instance)
{
    return DatatypeFactory::NewInstance(instance);
}

ECode CDatatypeFactoryHelper::NewInstance(
    /* [in] */ const String& factoryClassName,
    /* [in] */ IClassLoader* classLoader,
    /* [out] */ IDatatypeFactory** instance)
{
    return DatatypeFactory::NewInstance(factoryClassName, classLoader, instance);
}

} // namespace Datatype
} // namespace Xml
} // namespace Elastosx