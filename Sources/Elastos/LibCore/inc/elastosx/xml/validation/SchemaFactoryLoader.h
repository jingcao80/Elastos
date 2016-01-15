#ifndef __ELASTOSX_XML_VALIDATION_SCHEMAFACTORYLOADER_H__
#define __ELASTOSX_XML_VALIDATION_SCHEMAFACTORYLOADER_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastosx {
namespace Xml {
namespace Validation {

class SchemaFactoryLoader
    : public Object
    , public ISchemaFactoryLoader
{
public:
    CAR_INTERFACE_DECL()

protected:
    SchemaFactoryLoader() {}
};

} // namespace Validation
} // namespace Xml
} // namespace Elastosx
#endif