#ifndef __ELASTOSX_XML_VALIDATION_VALIDATOR_H__
#define __ELASTOSX_XML_VALIDATION_VALIDATOR_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastosx::Xml::Transform::ISource;

namespace Elastosx {
namespace Xml {
namespace Validation {

class Validator
    : public IValidator
    , public Object
{
public:
    CAR_INTERFACE_DECL()

    CARAPI Validate(
        /* [in] */ ISource* source);

    using IValidator::Validate;

    CARAPI GetFeature(
        /* [in] */ String name,
        /* [out] */ Boolean* feature);

    CARAPI SetFeature(
        /* [in] */ String name,
        /* [in] */ Boolean value);

    CARAPI SetProperty(
        /* [in] */ String name,
        /* [out] */ IInterface** object);

    CARAPI GetProperty(
        /* [in] */ String name,
        /* [out] */ IInterface** property);

protected:
    Validator();
};

} // namespace Validation
} // namespace Xml
} // namespace Elastosx
#endif