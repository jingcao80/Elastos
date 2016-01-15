
#include "elastos/droid/hardware/camera2/params/CRggbChannelVector.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include <elastos/core/Math.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Params {

CAR_INTERFACE_IMPL(CRggbChannelVector, Object, IRggbChannelVector)

CAR_OBJECT_IMPL(CRggbChannelVector)

CRggbChannelVector::CRggbChannelVector()
    : mRed(0.0)
    , mGreenEven(0.0)
    , mGreenOdd(0.0)
    , mBlue(0.0)
{
}

CRggbChannelVector::~CRggbChannelVector()
{
}

ECode CRggbChannelVector::constructor()
{
    return NOERROR;
}

ECode CRggbChannelVector::constructor(
    /* [in] */ Float red,
    /* [in] */ Float greenEven,
    /* [in] */ Float greenOdd,
    /* [in] */ Float blue)
{
    FAIL_RETURN(Preconditions::CheckArgumentFinite(red, String("red")))
    FAIL_RETURN(Preconditions::CheckArgumentFinite(greenEven, String("greenEven")))
    FAIL_RETURN(Preconditions::CheckArgumentFinite(greenOdd, String("greenOdd")))
    FAIL_RETURN(Preconditions::CheckArgumentFinite(blue, String("blue")))

    mRed = red;
    mGreenEven = greenEven;
    mGreenOdd = greenOdd;
    mBlue = blue;
    return NOERROR;
}

ECode CRggbChannelVector::GetRed(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mRed;
    return NOERROR;
}

ECode CRggbChannelVector::GetGreenEven(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mGreenEven;
    return NOERROR;
}

ECode CRggbChannelVector::GetGreenOdd(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mGreenOdd;
    return NOERROR;
}

ECode CRggbChannelVector::GetBlue(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mBlue;
    return NOERROR;
}

ECode CRggbChannelVector::GetComponent(
    /* [in] */ Int32 colorChannel,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    if (colorChannel < 0 || colorChannel >= COUNT) {
        //throw new IllegalArgumentException("Color channel out of range");
        Slogger::E("CRggbChannelVector", "Color channel out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    switch (colorChannel) {
        case RED:
            *value = mRed;
            return NOERROR;
        case GREEN_EVEN:
            *value = mGreenEven;
            return NOERROR;
        case GREEN_ODD:
            *value = mGreenOdd;
            return NOERROR;
        case BLUE:
            *value = mBlue;
            return NOERROR;
        default:
            //throw new AssertionError("Unhandled case " + colorChannel);
            Slogger::E("CRggbChannelVector", "Unhandled case %d", colorChannel);
            return E_ASSERTION_ERROR;;
    }
    return NOERROR;
}

ECode CRggbChannelVector::CopyTo(
    /* [in] */ ArrayOf<Float>* destination,
    /* [in] */ Int32 offset)
{
    //FAIL_RETURN(Preconditions::CheckNotNull(destination, String("destination must not be null")))
    if (destination == NULL) {
        Slogger::E("CRggbChannelVector", "destination must not be null");
        return E_NULL_POINTER_EXCEPTION;
    }

    if (destination->GetLength() - offset < COUNT) {
        //throw new ArrayIndexOutOfBoundsException("destination too small to fit elements");
        Slogger::E("CRggbChannelVector", "destination too small to fit elements");
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;;
    }

    (*destination)[offset + RED] = mRed;
    (*destination)[offset + GREEN_EVEN] = mGreenEven;
    (*destination)[offset + GREEN_ODD] = mGreenOdd;
    (*destination)[offset + BLUE] = mBlue;
    return NOERROR;
}

ECode CRggbChannelVector::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* equal)
{
    VALIDATE_NOT_NULL(equal);

    if (obj == NULL) {
        *equal = FALSE;
        return NOERROR;
    }
    else if (THIS_PROBE(IInterface) == TO_IINTERFACE(obj)) {
        *equal = TRUE;
        return NOERROR;
    }
    else if (IRggbChannelVector::Probe(obj) != NULL) {
        const AutoPtr<CRggbChannelVector> other = (CRggbChannelVector*)IRggbChannelVector::Probe(obj);
        *equal = mRed == other->mRed &&
                mGreenEven == other->mGreenEven &&
                mGreenOdd == other->mGreenOdd &&
                mBlue == other->mBlue;
        return NOERROR;
    }
    *equal = FALSE;
    return NOERROR;
}

ECode CRggbChannelVector::GetHashCode(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    assert(0 && "TODO: need floatToIntBits");
    // *value = Float.floatToIntBits(mRed) ^
    //         Float.floatToIntBits(mGreenEven) ^
    //         Float.floatToIntBits(mGreenOdd) ^
    //         Float.floatToIntBits(mBlue);
    return NOERROR;
}

ECode CRggbChannelVector::ToString(
   /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;
    sb += "RggbChannelVector";
    sb += ToShortString();

    return sb.ToString(str);
}

String CRggbChannelVector::ToShortString()
{
    StringBuilder sb;
    sb += "{R:";
    sb += mRed;
    sb += ", G_even:";
    sb += mGreenEven;
    sb += ", G_odd:";
    sb += mGreenOdd;
    sb += ", B:";
    sb += mBlue;

    return sb.ToString();
}

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
