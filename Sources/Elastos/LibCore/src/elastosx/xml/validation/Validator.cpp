#include "Validator.h"
#include "utility/logging/Slogger.h"

namespace Elastosx {
namespace Xml {
namespace Validation {

CAR_INTERFACE_IMPL(Validator, Object, IValidator)

ECode Validator::Validate(
    /* [in] */ ISource* source)
{
    return Validate(source, NULL);
}

ECode Validator::GetFeature(
    /* [in] */ String name,
    /* [out] */ Boolean* feature)
{
    if (name == NULL) {
        SLOGGERD("Validator", "name == null")
        return E_NULL_POINTER_EXCEPTION;
    }
    return E_SAX_NOT_RECOGNIZED_EXCEPTION;
}

ECode Validator::SetFeature(
    /* [in] */ String name,
    /* [in] */ Boolean value)
{
    if (name == NULL) {
        SLOGGERD("Validator", "name == null")
        return E_NULL_POINTER_EXCEPTION;
    }
    return E_SAX_NOT_RECOGNIZED_EXCEPTION;
}

ECode Validator::SetProperty(
    /* [in] */ String name,
    /* [out] */ IInterface** object)
{
    if (name == NULL) {
        SLOGGERD("Validator", "name == null")
        return E_NULL_POINTER_EXCEPTION;
    }
    return E_SAX_NOT_RECOGNIZED_EXCEPTION;
}

ECode Validator::GetProperty(
    /* [in] */ String name,
    /* [out] */ IInterface** property)
{
    if (name == NULL) {
        SLOGGERD("Validator", "name == null")
        return E_NULL_POINTER_EXCEPTION;
    }
    return E_SAX_NOT_RECOGNIZED_EXCEPTION;
}

Validator::Validator()
{}

} // namespace Validation
} // namespace Xml
} // namespace Elastosx