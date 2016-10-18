
#include "elastos/droid/hardware/camera2/params/CTonemapCurve.h"
#include "elastos/droid/hardware/camera2/utils/HashCodeHelpers.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include "elastos/droid/graphics/CPointF.h"
#include <elastos/core/Math.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::CPointF;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Droid::Hardware::Camera2::Utils::HashCodeHelpers;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Params {

const Int32 CTonemapCurve::OFFSET_POINT_IN = 0;
const Int32 CTonemapCurve::OFFSET_POINT_OUT = 1;
const Int32 CTonemapCurve::TONEMAP_MIN_CURVE_POINTS = 2;
const Int32 CTonemapCurve::MIN_CURVE_LENGTH = TONEMAP_MIN_CURVE_POINTS * ITonemapCurve::POINT_SIZE;

CAR_INTERFACE_IMPL(CTonemapCurve, Object, ITonemapCurve)

CAR_OBJECT_IMPL(CTonemapCurve)

CTonemapCurve::CTonemapCurve()
    : mHashCode(0)
{
}

CTonemapCurve::~CTonemapCurve()
{
}

ECode CTonemapCurve::constructor()
{
    mHashCalculated = FALSE;
    return NOERROR;
}

ECode CTonemapCurve::constructor(
    /* [in] */ ArrayOf<Float>* red,
    /* [in] */ ArrayOf<Float>* green,
    /* [in] */ ArrayOf<Float>* blue)
{
    // TODO: maxCurvePoints check?

    FAIL_RETURN(Preconditions::CheckNotNull(red, String("red must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(green, String("green must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(blue, String("blue must not be null")))

    FAIL_RETURN(CheckArgumentArrayLengthDivisibleBy(red, ITonemapCurve::POINT_SIZE, String("red")))
    FAIL_RETURN(CheckArgumentArrayLengthDivisibleBy(green, ITonemapCurve::POINT_SIZE, String("green")))
    FAIL_RETURN(CheckArgumentArrayLengthDivisibleBy(blue, ITonemapCurve::POINT_SIZE, String("blue")))

    FAIL_RETURN(CheckArgumentArrayLengthNoLessThan(red, MIN_CURVE_LENGTH, String("red")))
    FAIL_RETURN(CheckArgumentArrayLengthNoLessThan(green, MIN_CURVE_LENGTH, String("green")))
    FAIL_RETURN(CheckArgumentArrayLengthNoLessThan(blue, MIN_CURVE_LENGTH, String("blue")))

    FAIL_RETURN(Preconditions::CheckArrayElementsInRange(red, ITonemapCurve::LEVEL_BLACK,
            ITonemapCurve::LEVEL_WHITE, String("red")))
    FAIL_RETURN(Preconditions::CheckArrayElementsInRange(green, ITonemapCurve::LEVEL_BLACK,
            ITonemapCurve::LEVEL_WHITE, String("green")))
    FAIL_RETURN(Preconditions::CheckArrayElementsInRange(blue, ITonemapCurve::LEVEL_BLACK,
            ITonemapCurve::LEVEL_WHITE, String("blue")))

    Arrays::CopyOf(red, red->GetLength(), (ArrayOf<Float>**)&mRed);
    Arrays::CopyOf(green, green->GetLength(), (ArrayOf<Float>**)&mGreen);
    Arrays::CopyOf(blue, blue->GetLength(), (ArrayOf<Float>**)&mBlue);

    return NOERROR;
}

ECode CTonemapCurve::CheckArgumentArrayLengthDivisibleBy(
    /* [in] */ ArrayOf<Float>* array,
    /* [in] */ Int32 divisible,
    /* [in] */ const String& arrayName)
{
    if (array->GetLength() % divisible != 0) {
        Logger::E("CTonemapCurve", "%s size must be divisible by %d", arrayName.string(), divisible);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CTonemapCurve::CheckArgumentColorChannel(
    /* [in] */ Int32 colorChannel,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    switch (colorChannel) {
        case ITonemapCurve::CHANNEL_RED:
        case ITonemapCurve::CHANNEL_GREEN:
        case ITonemapCurve::CHANNEL_BLUE:
            break;
        default:
            Logger::E("CTonemapCurve", "colorChannel out of range");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *result = colorChannel;
    return NOERROR;
}

ECode CTonemapCurve::CheckArgumentArrayLengthNoLessThan(
    /* [in] */ ArrayOf<Float>* array,
    /* [in] */ Int32 minLength,
    /* [in] */ const String& arrayName)
{
    if (array->GetLength() < minLength) {
        Logger::E("CTonemapCurve", "%s size must be at least %d", arrayName.string(), minLength);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CTonemapCurve::GetPointCount(
    /* [in] */ Int32 colorChannel,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    Int32 channle;
    FAIL_RETURN(CheckArgumentColorChannel(colorChannel, &channle))
    AutoPtr<ArrayOf<Float> > tmp;
    FAIL_RETURN(GetCurve(colorChannel, (ArrayOf<Float>**)&tmp))
    *value = tmp->GetLength() / ITonemapCurve::POINT_SIZE;
    return NOERROR;
}

ECode CTonemapCurve::GetPoint(
    /* [in] */ Int32 colorChannel,
    /* [in] */ Int32 index,
    /* [out] */ IPointF** outpf)
{
    VALIDATE_NOT_NULL(outpf)
    *outpf = NULL;

    Int32 channle;
    FAIL_RETURN(CheckArgumentColorChannel(colorChannel, &channle))
    if (index < 0) {
        Logger::E("CTonemapCurve", "index out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else {
        Int32 count;
        FAIL_RETURN(GetPointCount(colorChannel, &count))
        if (index >= count) {
            Logger::E("CTonemapCurve", "index out of range");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    AutoPtr<ArrayOf<Float> > curve;
    FAIL_RETURN(GetCurve(colorChannel, (ArrayOf<Float>**)&curve))

    Float pIn = (*curve)[index * POINT_SIZE + OFFSET_POINT_IN];
    Float pOut = (*curve)[index * POINT_SIZE + OFFSET_POINT_OUT];

    return CPointF::New(pIn, pOut, outpf);
}

ECode CTonemapCurve::CopyColorCurve(
    /* [in] */ Int32 colorChannel,
    /* [in] */ ArrayOf<Float>* destination,
    /* [in] */ Int32 offset)
{
    FAIL_RETURN(Preconditions::CheckArgumentNonnegative(offset, String("offset must not be negative")))
    FAIL_RETURN(Preconditions::CheckNotNull(destination, String("destination must not be null")))
    if (destination == NULL) {
        Logger::E("CMeteringRectangle", "destination must not be null");
        return E_NULL_POINTER_EXCEPTION;
    }

    Int32 count;
    FAIL_RETURN(GetPointCount(colorChannel, &count))
    if (destination->GetLength() + offset < count * ITonemapCurve::POINT_SIZE) {
        Logger::E("CTonemapCurve", "destination too small to fit elements");
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    AutoPtr<ArrayOf<Float> > curve ;
    FAIL_RETURN(GetCurve(colorChannel, (ArrayOf<Float>**)&curve))
    return destination->Copy(/*srcPos*/0, curve, offset, curve->GetLength());
}

ECode CTonemapCurve::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* equal)
{
    VALIDATE_NOT_NULL(equal);
    *equal = FALSE;

    ITonemapCurve* tc = ITonemapCurve::Probe(obj);
    if (tc == NULL) {
        *equal = FALSE;
        return NOERROR;
    }

    CTonemapCurve* other = (CTonemapCurve*)tc;
    *equal = (Arrays::Equals(mRed, other->mRed)) && (Arrays::Equals(mGreen, other->mGreen))
            &&  (Arrays::Equals(mBlue, other->mBlue));
    return NOERROR;
}

ECode CTonemapCurve::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    if (mHashCalculated) {
        // Avoid re-calculating hash. Data is immutable so this is both legal and faster.
        *hashCode = mHashCode;
        return NOERROR;
    }

    Int32 h1, h2, h3;
    HashCodeHelpers::GetHashCode(mRed, &h1);
    HashCodeHelpers::GetHashCode(mGreen, &h2);
    HashCodeHelpers::GetHashCode(mBlue, &h3);
    HashCodeHelpers::GetHashCode(h1, h2, h3, &mHashCode);
    mHashCalculated = TRUE;

    *hashCode = mHashCode;
    return NOERROR;
}

ECode CTonemapCurve::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    StringBuilder sb;
    sb += "TonemapCurve{";
    sb += "R:";
    String strRed;
    CurveToString(CHANNEL_RED, &strRed);
    sb += strRed;
    sb += ", G:";
    String strGreen;
    CurveToString(CHANNEL_GREEN, &strGreen);
    sb += strGreen;
    sb += ", B:";
    String strBlue;
    CurveToString(CHANNEL_BLUE, &strBlue);
    sb += strBlue;
    sb += "}";
    return sb.ToString(str);
}

ECode CTonemapCurve::CurveToString(
    /* [in] */ Int32 colorChannel,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = NULL;

    Int32 channle;
    FAIL_RETURN(CheckArgumentColorChannel(colorChannel, &channle))
    StringBuilder sb;
    sb += "[";
    AutoPtr<ArrayOf<Float> > curve;
    FAIL_RETURN(GetCurve(colorChannel, (ArrayOf<Float>**)&curve))
    Int32 pointCount = curve->GetLength() / ITonemapCurve::POINT_SIZE;
    for (Int32 i = 0, j = 0; i < pointCount; i++, j += 2) {
        sb += "(";
        sb += (*curve)[j];
        sb += ", ";
        sb += (*curve)[j+1];
        sb += "), ";
    }
    // trim extra ", " at the end. Guaranteed to work because pointCount >= 2
    sb.SetLength(sb.GetLength() - 2);
    sb += "]";
    return sb.ToString(str);
}

ECode CTonemapCurve::GetCurve(
    /* [in] */ Int32 colorChannel,
    /* [out, callee] */ ArrayOf<Float>** curve)
{
    VALIDATE_NOT_NULL(curve)
    *curve = NULL;

    switch (colorChannel) {
        case ITonemapCurve::CHANNEL_RED:
            *curve = mRed;
            REFCOUNT_ADD(*curve);
            break;
        case ITonemapCurve::CHANNEL_GREEN:
            *curve = mGreen;
            REFCOUNT_ADD(*curve);
            break;
        case ITonemapCurve::CHANNEL_BLUE:
            *curve = mBlue;
            REFCOUNT_ADD(*curve);
            break;
        default:
            Logger::E("CTonemapCurve", "colorChannel out of range");
        return E_ASSERTION_ERROR;
    }
    return NOERROR;
}

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
