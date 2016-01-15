#include "CDocumentBuilderFactoryHelper.h"
#include "DocumentBuilderFactory.h"

namespace Elastosx {
namespace Xml {
namespace Parsers {

CAR_SINGLETON_IMPL(CDocumentBuilderFactoryHelper)

CAR_INTERFACE_IMPL(CDocumentBuilderFactoryHelper, Singleton, IDocumentBuilderFactoryHelper)

ECode CDocumentBuilderFactoryHelper::NewInstance(
    /* [out] */ IDocumentBuilderFactory** instance)
{
    return DocumentBuilderFactory::NewInstance(instance);
}

ECode CDocumentBuilderFactoryHelper::NewInstance(
    /* [in] */ const String& factoryClassName,
    /* [in] */ IClassLoader* classLoader,
    /* [out] */ IDocumentBuilderFactory** instance)
{
    return DocumentBuilderFactory::NewInstance(factoryClassName, classLoader, instance);
}

} // namespace Parsers
} // namespace Xml
} // namespace Elastosx