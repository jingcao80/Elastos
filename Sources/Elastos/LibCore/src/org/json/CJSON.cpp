
#include "CJSON.h"
#include "JSON.h"

namespace Org {
namespace Json {

CAR_SINGLETON_IMPL(CJSON);

CAR_INTERFACE_IMPL(CJSON, Singleton, IJSON);

ECode CJSON::CheckDouble(
    /* [in] */ Double d,
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result);
    return JSON::CheckDouble(d, result);
}

ECode CJSON::ToBoolean(
    /* [in] */ IInterface* value,
    /* [out] */ IBoolean** result)
{
    VALIDATE_NOT_NULL(result);
    return JSON::ToBoolean(value, result);
}

ECode CJSON::ToDouble(
    /* [in] */ IInterface* value,
    /* [out] */ IDouble** result)
{
    VALIDATE_NOT_NULL(result);
    return JSON::ToDouble(value, result);
}

ECode CJSON::ToInteger32(
    /* [in] */ IInterface* value,
    /* [out] */ IInteger32** result)
{
    VALIDATE_NOT_NULL(result);
    return JSON::ToInteger32(value, result);
}

ECode CJSON::ToInteger64(
    /* [in] */ IInterface* value,
    /* [out] */ IInteger64** result)
{
    VALIDATE_NOT_NULL(result);
    return JSON::ToInteger64(value, result);
}

ECode CJSON::ToString(
    /* [in] */ IInterface* value,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    return JSON::ToString(value, result);
}

ECode CJSON::TypeMismatch(
    /* [in] */ IInterface* indexOrName,
    /* [in] */ IInterface* actual,
    /* [in] */ const String& requiredType)
{
    return JSON::TypeMismatch(indexOrName, actual, requiredType);
}

ECode CJSON::TypeMismatch(
    /* [in] */ IInterface* actual,
    /* [in] */ const String& requiredType)
{
    return JSON::TypeMismatch(actual, requiredType);
}

} //namespace Json
} //namespace Org
