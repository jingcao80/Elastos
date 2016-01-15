
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/internal/utility/Preconditions.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

const String Preconditions::TAG("Preconditions");

ECode Preconditions::CheckNotNull(
    /* [in] */ IInterface* reference)
{
    if (reference == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    return NOERROR;
}

ECode Preconditions::CheckNotNull(
    /* [in] */ const String& reference)
{
    if (reference.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    return NOERROR;
}

ECode Preconditions::CheckNotNull(
    /* [in] */ IInterface* reference,
    /* [in] */ IObject* errorMessage)
{
    if (reference == NULL) {
        String str;
        errorMessage->ToString(&str);
        Logger::E(TAG, str);
        return E_NULL_POINTER_EXCEPTION;
    }
    return NOERROR;
}

ECode Preconditions::CheckNotNull(
    /* [in] */ IInterface* reference,
    /* [in] */ const String& errorMessage)
{
    if (reference == NULL) {
        Logger::E(TAG, errorMessage);
        return E_NULL_POINTER_EXCEPTION;
    }
    return NOERROR;
}

ECode Preconditions::CheckState(
    /* [in] */ Boolean expression)
{
    if (!expression) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode Preconditions::CheckFlagsArgument(
    /* [in] */ Int32 requestedFlags,
    /* [in] */ Int32 allowedFlags)
{
    if ((requestedFlags & allowedFlags) != requestedFlags) {
        Logger::E(TAG, "Requested flags 0x%x, but only 0x%x are allowed",
            requestedFlags, allowedFlags);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode Preconditions::CheckArgumentNonnegative(
    /* [in] */ Int32 value,
    /* [in] */ const String& errorMessage)
{
    if (value < 0) {
        Logger::E(TAG, errorMessage);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode Preconditions::CheckArgumentNonnegative(
    /* [in] */ Int64 value,
    /* [in] */ const String& errorMessage)
{
    if (value < 0) {
        Logger::E(TAG, errorMessage);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode Preconditions::CheckArgumentPositive(
    /* [in] */ Int32 value,
    /* [in] */ const String& errorMessage)
{
    if (value <= 0) {
        Logger::E(TAG, errorMessage);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode Preconditions::CheckArgumentFinite(
    /* [in] */ Float value,
    /* [in] */ const String& valueName)
{
    if (Elastos::Core::Math::IsNaN(value)) {
        Logger::E(TAG, "%s must not be NaN", valueName.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (Elastos::Core::Math::IsInfinite(value)) {
        Logger::E(TAG, "%s must not be infinite", valueName.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode Preconditions::CheckArgumentInRange(
    /* [in] */ Float value,
    /* [in] */ Float lower,
    /* [in] */ Float upper,
    /* [in] */ const String& valueName)
{
    if (Elastos::Core::Math::IsNaN(value)) {
        Logger::E(TAG, "%s must not be NaN", valueName.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (value < lower) {
        Logger::E(TAG, "%s is out of range of [%f, %f] (too low)", valueName.string(), lower, upper);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (value > upper) {
        Logger::E(TAG, "%s is out of range of [%f, %f] (too high)", valueName.string(), lower, upper);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode Preconditions::CheckArgumentInRange(
    /* [in] */ Int32 value,
    /* [in] */ Int32 lower,
    /* [in] */ Int32 upper,
    /* [in] */ const String& valueName)
{
    if (value < lower) {
        Logger::E(TAG, "%s is out of range of [%d, %d] (too low)", valueName.string(), lower, upper);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (value > upper) {
        Logger::E(TAG, "%s is out of range of [%d, %d] (too low)", valueName.string(), lower, upper);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode Preconditions::CheckArrayElementsNotNull(
    /* [in] */ ArrayOf<IInterface*>* value,
    /* [in] */ const String& valueName)
{
    if (value == NULL) {
        Logger::E(TAG, "%s must not be NULL", valueName.string());
        return E_NULL_POINTER_EXCEPTION;
    }

    for (Int32 i = 0; i < value->GetLength(); ++i) {
        if ((*value)[i] == NULL) {
            Logger::E(TAG, "%s[%d] must not be NULL", valueName.string(), i);
            return E_NULL_POINTER_EXCEPTION;
        }
    }

    return NOERROR;
}

ECode Preconditions::CheckCollectionElementsNotNull(
    /* [in] */ ICollection* value,
    /* [in] */ const String& valueName)
{
    if (value == NULL) {
        Logger::E(TAG, "%s must not be NULL", valueName.string());
        return E_NULL_POINTER_EXCEPTION;
    }

    Int64 ctr = 0;
    AutoPtr<IIterator> iter;
    value->GetIterator((IIterator**)&iter);
    Boolean hasNext;
    while (iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> elem;
        iter->GetNext((IInterface**)&elem);
        if (elem == NULL) {
            Logger::E(TAG, "%s[%lld] must not be NULL", valueName.string(), ctr);
            return E_NULL_POINTER_EXCEPTION;
        }
        ++ctr;
    }

    return NOERROR;
}

ECode Preconditions::CheckCollectionNotEmpty(
    /* [in] */ ICollection* value,
    /* [in] */ const String& valueName)
{
    if (value == NULL) {
        Logger::E(TAG, "%s must not be NULL", valueName.string());
        return E_NULL_POINTER_EXCEPTION;
    }

    Boolean isEmpty;
    value->IsEmpty(&isEmpty);
    if (isEmpty) {
        Logger::E(TAG, "%s is empty", valueName.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode Preconditions::CheckArrayElementsInRange(
    /* [in] */ ArrayOf<Float>* value,
    /* [in] */ Float lower,
    /* [in] */ Float upper,
    /* [in] */ const String& valueName)
{
    if (value == NULL) {
        Logger::E(TAG, "%s must not be NULL", valueName.string());
        return E_NULL_POINTER_EXCEPTION;
    }

    for (Int32 i = 0; i < value->GetLength(); ++i) {
        Float v = (*value)[i];

        if (Elastos::Core::Math::IsNaN(v)) {
            Logger::E(TAG, "%s[%d] must not be NaN", valueName.string(), i);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        else if (v < lower) {
            Logger::E(TAG, "%s[%d] is out of range of [%f, %f] (too low)",
                valueName.string(), i, lower, upper);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        else if (v > upper) {
            Logger::E(TAG, "%s[%d] is out of range of [%f, %f] (too high)",
                valueName.string(), i, lower, upper);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
