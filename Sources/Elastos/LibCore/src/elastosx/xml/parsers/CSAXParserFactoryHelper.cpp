#include "CSAXParserFactoryHelper.h"
#include "SAXParserFactory.h"

namespace Elastosx {
namespace Xml {
namespace Parsers {

CAR_SINGLETON_IMPL(CSAXParserFactoryHelper)

CAR_INTERFACE_IMPL(CSAXParserFactoryHelper, Singleton, ISAXParserFactoryHelper)

ECode CSAXParserFactoryHelper::NewInstance(
    /* [out] */ ISAXParserFactory** instance)
{
    return SAXParserFactory::NewInstance(instance);
}

ECode CSAXParserFactoryHelper::NewInstance(
    /* [in] */ const String& factoryClassName,
    /* [in] */ IClassLoader* classLoader,
    /* [out] */ ISAXParserFactory** instance)
{
    return SAXParserFactory::NewInstance(factoryClassName, classLoader, instance);
}

} // namespace Parsers
} // namespace Xml
} // namespace Elastosx