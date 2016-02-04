
#include "JSON.h"
#include "CoreUtils.h"
#include "Math.h"
#include "StringUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::CoreUtils;
using Elastos::Core::INumber;
using Elastos::Core::IString;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Json {

ECode JSON::CheckDouble(
    /* [in] */ Double d)
{
    using Elastos::Core::Math;
    if (Math::IsInfinite(d) || Math::IsNaN(d)) {
        Logger::E("JSON", "Forbidden numeric value: %lf", d);
        return E_JSON_EXCEPTION;
        // throw new JSONException("Forbidden numeric value: " + d);
    }
    return NOERROR;
}

ECode JSON::ToBoolean(
    /* [in] */ IInterface* value,
    /* [out] */ IBoolean** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (IBoolean::Probe(value) != NULL) {
        *result = IBoolean::Probe(value);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else if (IString::Probe(value) != NULL) {
        String stringValue;
        ICharSequence::Probe(value)->ToString(&stringValue);
        if (String("true").EqualsIgnoreCase(stringValue)) {
            AutoPtr<IBoolean> bObj = CoreUtils::Convert(TRUE);
            *result = bObj;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
        else if (String("false").EqualsIgnoreCase(stringValue)) {
            AutoPtr<IBoolean> bObj = CoreUtils::Convert(FALSE);
            *result = bObj;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode JSON::ToDouble(
    /* [in] */ IInterface* value,
    /* [out] */ IDouble** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (IDouble::Probe(value) != NULL) {
        *result = IDouble::Probe(value);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else if (INumber::Probe(value) != NULL) {
        Double data;
        INumber::Probe(value)->DoubleValue(&data);
        AutoPtr<IDouble> object = CoreUtils::Convert(data);
        *result = object;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else if (IString::Probe(value) != NULL) {
        // try {
        String stringValue;
        ICharSequence::Probe(value)->ToString(&stringValue);
        Double data;
        ECode ec = StringUtils::Parse(stringValue, &data);
        if (FAILED(ec)) return NOERROR;
        AutoPtr<IDouble> object = CoreUtils::Convert(data);
        *result = object;
        REFCOUNT_ADD(*result);
        return NOERROR;
        // } catch (NumberFormatException ignored) {
        // }
    }
    return NOERROR;
}

ECode JSON::ToInteger32(
    /* [in] */ IInterface* value,
    /* [out] */ IInteger32** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (IInteger32::Probe(value) != NULL) {
        *result = IInteger32::Probe(value);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else if (INumber::Probe(value) != NULL) {
        Int32 data;
        INumber::Probe(value)->Int32Value(&data);
        AutoPtr<IInteger32> object = CoreUtils::Convert(data);
        *result = object;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else if (IString::Probe(value) != NULL) {
        // try {
        String stringValue;
        ICharSequence::Probe(value)->ToString(&stringValue);
        Double data;
        ECode ec = StringUtils::Parse(stringValue, &data);
        if (FAILED(ec)) return NOERROR;
        AutoPtr<IInteger32> object = CoreUtils::Convert((Int32)data);
        *result = object;
        REFCOUNT_ADD(*result);
        return NOERROR;
        // } catch (NumberFormatException ignored) {
        // }
    }
    return NOERROR;
}

ECode JSON::ToInteger64(
    /* [in] */ IInterface* value,
    /* [out] */ IInteger64** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (IInteger64::Probe(value) != NULL) {
        *result = IInteger64::Probe(value);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else if (INumber::Probe(value) != NULL) {
        Int64 data;
        INumber::Probe(value)->Int64Value(&data);
        AutoPtr<IInteger64> object = CoreUtils::Convert(data);
        *result = object;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else if (IString::Probe(value) != NULL) {
        // try {
        String stringValue;
        ICharSequence::Probe(value)->ToString(&stringValue);
        Double data;
        ECode ec = StringUtils::Parse(stringValue, &data);
        if (FAILED(ec)) return NOERROR;
        AutoPtr<IInteger64> object = CoreUtils::Convert((Int64)data);
        *result = object;
        REFCOUNT_ADD(*result);
        return NOERROR;
        // } catch (NumberFormatException ignored) {
        // }
    }
    return NOERROR;
}

ECode JSON::ToString(
    /* [in] */ IInterface* value,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);

    if (IString::Probe(value) != NULL) {
        AutoPtr<ICharSequence> seq = ICharSequence::Probe(value);
        return seq->ToString(result);
    }
    else if (value != NULL) {
        return Object::ToString(value);
    }
    return NOERROR;
}

ECode JSON::TypeMismatch(
    /* [in] */ IInterface* indexOrName,
    /* [in] */ IInterface* actual,
    /* [in] */ const String& requiredType)
{
    if (actual == NULL) {
        Logger::E("JSON", "Value at %p is null", indexOrName);
        return E_JSON_EXCEPTION;
        // throw new JSONException("Value at " + indexOrName + " is null.");
    }
    else {
        Logger::E("JSON", "Value %p at %p of type actual cannot be converted to %s",
                actual, indexOrName, requiredType.string());
        return E_JSON_EXCEPTION;
        // throw new JSONException("Value " + actual + " at " + indexOrName
        //         + " of type " + actual.getClass().getName()
        //         + " cannot be converted to " + requiredType);
    }
    return NOERROR;
}

ECode JSON::TypeMismatch(
    /* [in] */ IInterface* actual,
    /* [in] */ const String& requiredType)
{
    if (actual == NULL) {
        Logger::E("JSON", "Value is null.");
        return E_JSON_EXCEPTION;
        // throw new JSONException("Value is null.");
    }
    else {
        Logger::E("JSON", "Value %p of type actual cannot be converted to %s",
                actual, requiredType.string());
        return E_JSON_EXCEPTION;
        // throw new JSONException("Value " + actual
        //         + " of type " + actual.getClass().getName()
        //         + " cannot be converted to " + requiredType);
    }
    return NOERROR;
}

} //namespace Json
} //namespace Org
