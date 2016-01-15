#include "CSchemaFactoryHelper.h"
#include "SchemaFactory.h"

namespace Elastosx {
namespace Xml {
namespace Validation {

CAR_SINGLETON_IMPL(CSchemaFactoryHelper)

CAR_INTERFACE_IMPL(CSchemaFactoryHelper, Singleton, ISchemaFactoryHelper)

ECode CSchemaFactoryHelper::NewInstance(
    /* [in] */ const String& schemaLanguage,
    /* [out] */ ISchemaFactory** factory)
{
    return SchemaFactory::NewInstance(schemaLanguage, factory);
}

ECode CSchemaFactoryHelper::NewInstance(
    /* [in] */ const String& schemaLanguage,
    /* [in] */ const String& factoryClassName,
    /* [in] */ IClassLoader* classLoader,
    /* [out] */ ISchemaFactory** factory)
{
    return SchemaFactory::NewInstance(schemaLanguage, factoryClassName, classLoader, factory);
}

} // namespace Validation
} // namespace Xml
} // namespace Elastosx