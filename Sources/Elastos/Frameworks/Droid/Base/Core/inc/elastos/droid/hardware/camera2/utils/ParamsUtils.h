
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_PARAMSUTILS_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_PARAMSUTILS_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::ISize;
using Elastos::Droid::Utility::IRational;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Hardware::Camera2::ICaptureRequest;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

class ParamsUtils
{
public:
    /**
     * Create a {@link Rect} from a {@code Size} by creating a new rectangle with
     * left, top = {@code (0, 0)} and right, bottom = {@code (width, height)}
     *
     * @param size a non-{@code null} size
     *
     * @return a {@code non-null} rectangle
     *
     * @throws NullPointerException if {@code size} was {@code null}
     */
    static CARAPI CreateRect(
        /* [in] */ ISize* size,
        /* [out] */ IRect** result);

    /**
     * Create a {@link Rect} from a {@code RectF} by creating a new rectangle with
     * each corner (left, top, right, bottom) rounded towards the nearest integer bounding box.
     *
     * <p>In particular (left, top) is floored, and (right, bottom) is ceiled.</p>
     *
     * @param size a non-{@code null} rect
     *
     * @return a {@code non-null} rectangle
     *
     * @throws NullPointerException if {@code rect} was {@code null}
     */
    static CARAPI CreateRect(
        /* [in] */ IRectF* rect,
        /* [out] */ IRect** result);

    /**
     * Map the rectangle in {@code rect} with the transform in {@code transform} into
     * a new rectangle, with each corner (left, top, right, bottom) rounded towards the nearest
     * integer bounding box.
     *
     * <p>None of the arguments are mutated.</p>
     *
     * @param transform a non-{@code null} transformation matrix
     * @param rect a non-{@code null} rectangle
     * @return a new rectangle that was transformed by {@code transform}
     *
     * @throws NullPointerException if any of the args were {@code null}
     */
    static CARAPI MapRect(
        /* [in] */ IMatrix* transform,
        /* [in] */ IRect* rect,
        /* [out] */ IRect** result);

    /**
     * Create a {@link Size} from a {@code Rect} by creating a new size whose width
     * and height are the same as the rectangle's width and heights.
     *
     * @param rect a non-{@code null} rectangle
     *
     * @return a {@code non-null} size
     *
     * @throws NullPointerException if {@code rect} was {@code null}
     */
    static CARAPI CreateSize(
        /* [in] */ IRect* rect,
        /* [out] */ ISize** result);

    /**
     * Create a {@link Rational} value by approximating the float value as a rational.
     *
     * <p>Floating points too large to be represented as an integer will be converted to
     * to {@link Integer#MAX_VALUE}; floating points too small to be represented as an integer
     * will be converted to {@link Integer#MIN_VALUE}.</p>
     *
     * @param value a floating point value
     * @return the rational representation of the float
     */
    static CARAPI CreateRational(
        /* [in] */ Float value,
        /* [out] */ IRational** result);

    /**
     * Convert an integral rectangle ({@code source}) to a floating point rectangle
     * ({@code destination}) in-place.
     *
     * @param source the originating integer rectangle will be read from here
     * @param destination the resulting floating point rectangle will be written out to here
     *
     * @throws NullPointerException if {@code rect} was {@code null}
     */
    static CARAPI ConvertRectF(
        /* [in] */ IRect* source,
        /* [in] */ IRectF* destination);

    /**
     * Return the value set by the key, or the {@code defaultValue} if no value was set.
     *
     * @throws NullPointerException if any of the args were {@code null}
     */
    static CARAPI GetOrDefault(
        /* [in] */ ICaptureRequest* r,
        /* [in] */ ICaptureRequestKey* key,
        /* [in] */ IInterface* defaultValue,
        /* [out] */ IInterface** result);

private:
    ParamsUtils() {}

private:
    /** Arbitrary denominator used to estimate floats as rationals */
    static const Int32 RATIONAL_DENOMINATOR;
};

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_PARAMSUTILS_H__
