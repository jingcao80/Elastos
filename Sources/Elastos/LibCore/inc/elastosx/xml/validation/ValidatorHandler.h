#ifndef __ELASTOSX_XML_VALIDATION_VALIDATORHANDLER_H__
#define __ELASTOSX_XML_VALIDATION_VALIDATORHANDLER_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.External.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastosx::Xml::Transform::ISource;
using Org::Xml::Sax::IContentHandler;
using Org::Xml::Sax::EIID_IContentHandler;

namespace Elastosx {
namespace Xml {
namespace Validation {

class ValidatorHandler
    : public Object
    , public IValidatorHandler
    , public IContentHandler
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetFeature(
        /* [in] */ String name,
        /* [out] */ Boolean* feature);

    CARAPI SetFeature(
        /* [in] */ String name,
        /* [in] */ Boolean value);

    CARAPI SetProperty(
        /* [in] */ String name,
        /* [in] */ IInterface* object);

    CARAPI GetProperty(
        /* [in] */ String name,
        /* [out] */ IInterface** property);

protected:
    ValidatorHandler();
};

} // namespace Validation
} // namespace Xml
} // namespace Elastosx

#endif