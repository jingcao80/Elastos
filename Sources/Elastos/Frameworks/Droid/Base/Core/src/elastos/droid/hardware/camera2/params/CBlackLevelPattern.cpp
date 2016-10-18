
#include "elastos/droid/hardware/camera2/params/CBlackLevelPattern.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Params {

CAR_INTERFACE_IMPL(CBlackLevelPattern, Object, IBlackLevelPattern)

CAR_OBJECT_IMPL(CBlackLevelPattern)

CBlackLevelPattern::CBlackLevelPattern()
{
}

CBlackLevelPattern::~CBlackLevelPattern()
{
}

ECode CBlackLevelPattern::constructor()
{
    return NOERROR;
}

ECode CBlackLevelPattern::constructor(
    /* [in] */ ArrayOf<Int32>* offsets)
{
    if (offsets == NULL) {
        Logger::E("CBlackLevelPattern", "Null offsets array passed to constructor");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (offsets->GetLength() < IBlackLevelPattern::COUNT) {
        Logger::E("CBlackLevelPattern", "Invalid offsets array length");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return Arrays::CopyOf(offsets, IBlackLevelPattern::COUNT, (ArrayOf<Int32>**)&mCfaOffsets);
}

ECode CBlackLevelPattern::GetOffsetForIndex(
    /* [in] */ Int32 column,
    /* [in] */ Int32 row,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    if (row < 0 || column < 0) {
        Logger::E("CBlackLevelPattern", "column, row arguments must be positive");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *value = (*mCfaOffsets)[((row & 1) << 1) | (column & 1)];
    return NOERROR;
}

ECode CBlackLevelPattern::CopyTo(
    /* [in] */ ArrayOf<Int32>* destination,
    /* [in] */ Int32 offset)
{
    FAIL_RETURN(Preconditions::CheckNotNull(destination, String("destination must not be null")))
    if (destination == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    if (offset < 0) {
        Logger::E("CBlackLevelPattern", "Null offset passed to copyTo");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (destination->GetLength() - offset < IBlackLevelPattern::COUNT) {
        Logger::E("CBlackLevelPattern", "destination too small to fit elements");
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;;
    }
    for (Int32 i = 0; i < IBlackLevelPattern::COUNT; ++i) {
        (*destination)[offset + i] = (*mCfaOffsets)[i];
    }
    return NOERROR;
}

ECode CBlackLevelPattern::Equals(
    /* [in] */ IInterface *obj,
    /* [out] */ Boolean *equal)
{
    VALIDATE_NOT_NULL(equal);
    *equal = FALSE;

    IBlackLevelPattern* blp = IBlackLevelPattern::Probe(obj);
    if (obj == NULL) {
        return NOERROR;
    }
    CBlackLevelPattern* other = (CBlackLevelPattern*)blp;
    *equal = Arrays::Equals(other->mCfaOffsets, mCfaOffsets);
    return NOERROR;
}

ECode CBlackLevelPattern::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = Arrays::GetHashCode(mCfaOffsets);
    return NOERROR;
}

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
