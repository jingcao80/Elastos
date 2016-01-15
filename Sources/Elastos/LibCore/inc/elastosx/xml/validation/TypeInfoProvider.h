
#ifndef __ELASTOSX_XML_VALIDATION_TYPEINFOPROVIDER_H__
#define __ELASTOSX_XML_VALIDATION_TYPEINFOPROVIDER_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastosx {
namespace Xml {
namespace Validation {

class TypeInfoProvider
    : public Object
    , public ITypeInfoProvider
{
public:
    CAR_INTERFACE_DECL()

protected:
    TypeInfoProvider() {}
};

} // namespace Validation
} // namespace Xml
} // namespace Elastosx
#endif