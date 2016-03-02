#include "ValidatorHandler.h"
#include "utility/logging/Slogger.h"

namespace Elastosx {
namespace Xml {
namespace Validation {

CAR_INTERFACE_IMPL_2(ValidatorHandler, Object, IValidatorHandler, IContentHandler)

ECode ValidatorHandler::GetFeature(
    /* [in] */ const String& name,
    /* [out] */ Boolean* feature)
{
    if (name == NULL) {
        SLOGGERD("Validator", "name == null")
        return E_NULL_POINTER_EXCEPTION;
    }
    return E_SAX_NOT_RECOGNIZED_EXCEPTION;
}

ECode ValidatorHandler::SetFeature(
    /* [in] */ const String& name,
    /* [in] */ Boolean value)
{
    if (name == NULL) {
        SLOGGERD("Validator", "name == null")
        return E_NULL_POINTER_EXCEPTION;
    }
    return E_SAX_NOT_RECOGNIZED_EXCEPTION;
}

ECode ValidatorHandler::SetProperty(
    /* [in] */ const String& name,
    /* [in] */ IInterface* object)
{
    if (name == NULL) {
        SLOGGERD("Validator", "name == null")
        return E_NULL_POINTER_EXCEPTION;
    }
    return E_SAX_NOT_RECOGNIZED_EXCEPTION;
}

ECode ValidatorHandler::GetProperty(
    /* [in] */ const String& name,
    /* [out] */ IInterface** property)
{
    if (name == NULL) {
        SLOGGERD("Validator", "name == null")
        return E_NULL_POINTER_EXCEPTION;
    }
    return E_SAX_NOT_RECOGNIZED_EXCEPTION;
}

} // namespace Validation
} // namespace Xml
} // namespace Elastosx
