
#include "elastos/droid/hardware/camera2/utils/ParamsUtils.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include "elastos/droid/utility/CSize.h"
#include "elastos/droid/utility/CRational.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CRectF.h"
#include <elastos/core/StringBuffer.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Utility::CSize;
using Elastos::Droid::Utility::CRational;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Core::StringBuffer;
using Elastos::Core::Math;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

const Int32 ParamsUtils::RATIONAL_DENOMINATOR = 1000000; // 1million

ECode ParamsUtils::CreateRect(
    /* [in] */ ISize* size,
    /* [out] */ IRect** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(Preconditions::CheckNotNull(size, String("size must not be null")))
    Int32 width;
    size->GetHeight(&width);
    Int32 heihtt;
    size->GetHeight(&heihtt);
    return CRect::New(/*left*/0, /*top*/0, width, heihtt, result);
}

ECode ParamsUtils::CreateRect(
    /* [in] */ IRectF* rect,
    /* [out] */ IRect** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(Preconditions::CheckNotNull(rect, String("rect must not be null")))
    AutoPtr<IRect> r;
    CRect::New((IRect**)&r);
    rect->RoundOut(r);

    *result = r;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ParamsUtils::MapRect(
    /* [in] */ IMatrix* transform,
    /* [in] */ IRect* rect,
    /* [out] */ IRect** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(Preconditions::CheckNotNull(transform, String("transform must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(rect, String("rect must not be null")))
    AutoPtr<IRectF> rectF;
    CRectF::New(rect, (IRectF**)&rectF);
    Boolean res;
    transform->MapRect(rectF, &res);
    return CreateRect(rectF, result);
}

ECode ParamsUtils::CreateSize(
    /* [in] */ IRect* rect,
    /* [out] */ ISize** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(Preconditions::CheckNotNull(rect, String("rect must not be null")))
    Int32 width;
    rect->GetWidth(&width);
    Int32 height;
    rect->GetHeight(&height);
    return CSize::New(width, height, result);
}

ECode ParamsUtils::CreateRational(
    /* [in] */ Float value,
    /* [out] */ IRational** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    assert(0 && "TODO");
    // if (Float.isNaN(value)) {
    //     return Rational.NaN;
    // }
    // else if (value == Float.POSITIVE_INFINITY) {
    //     return Rational.POSITIVE_INFINITY;
    // }
    // else if (value == Float.NEGATIVE_INFINITY) {
    //     return Rational.NEGATIVE_INFINITY;
    // }
    // else if (value == 0.0f) {
    //     return Rational.ZERO;
    // }

    // normal finite value: approximate it

    /*
     * Start out trying to approximate with denominator = 1million,
     * but if the numerator doesn't fit into an Int then keep making the denominator
     * smaller until it does.
     */
    Int32 den = RATIONAL_DENOMINATOR;
    Float numF;
    do {
        numF = value * den;

        if ((numF > Elastos::Core::Math::INT32_MIN_VALUE && numF < Elastos::Core::Math::INT32_MIN_VALUE) || (den == 1)) {
            break;
        }

        den /= 10;
    } while (TRUE);

    /*
     *  By float -> int narrowing conversion in JLS 5.1.3, this will automatically become
     *  MIN_VALUE or MAX_VALUE if numF is too small/large to be represented by an integer
     */
    Int32 num = (Int32) numF;

    return CRational::New(num, den, result);
}

ECode ParamsUtils::ConvertRectF(
    /* [in] */ IRect* source,
    /* [in] */ IRectF* destination)
{
    FAIL_RETURN(Preconditions::CheckNotNull(source, String("source must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(destination, String("destination must not be null")))

    Int32 left;
    source->GetLeft(&left);
    destination->SetLeft(left);

    Int32 right;
    source->GetRight(&right);
    destination->SetRight(right);

    Int32 bottom;
    source->GetBottom(&bottom);
    destination->SetBottom(bottom);

    Int32 top;
    source->GetTop(&top);
    destination->SetTop(top);

    return NOERROR;
}

ECode ParamsUtils::GetOrDefault(
    /* [in] */ ICaptureRequest* r,
    /* [in] */ ICaptureRequestKey* key,
    /* [in] */ IInterface* defaultValue,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(Preconditions::CheckNotNull(r, String("r must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(key, String("key must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(defaultValue, String("defaultValue must not be null")))
    AutoPtr<IInterface> value;
    assert(0 && "weit get");
    //r->Get(key, (IInterface**)&value);
    if (value == NULL) {
        *result = defaultValue;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else {
        *result = value;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    return NOERROR;
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
