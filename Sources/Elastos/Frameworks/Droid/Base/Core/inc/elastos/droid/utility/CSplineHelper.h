
#ifndef __ELASTOS_DROID_UTILITY_CSPLINE_HELPER_H__
#define __ELASTOS_DROID_UTILITY_CSPLINE_HELPER_H__

#include "_Elastos_Droid_Utility_CSplineHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CSplineHelper)
    , public Singleton
    , public ISplineHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Creates an appropriate spline based on the properties of the control points.
     *
     * If the control points are monotonic then the resulting spline will preserve that and
     * otherwise optimize for error bounds.
     */
    CARAPI CreateSpline(
        /* [in] */ ArrayOf<Float>* x,
        /* [in] */ ArrayOf<Float>* y,
        /* [out] */ ISpline** spline);

    /**
     * Creates a monotone cubic spline from a given set of control points.
     *
     * The spline is guaranteed to pass through each control point exactly.
     * Moreover, assuming the control points are monotonic (Y is non-decreasing or
     * non-increasing) then the interpolated values will also be monotonic.
     *
     * This function uses the Fritsch-Carlson method for computing the spline parameters.
     * http://en.wikipedia.org/wiki/Monotone_cubic_interpolation
     *
     * @param x The X component of the control points, strictly increasing.
     * @param y The Y component of the control points, monotonic.
     * @return
     *
     * @throws IllegalArgumentException if the X or Y arrays are null, have
     * different lengths or have fewer than 2 values.
     * @throws IllegalArgumentException if the control points are not monotonic.
     */
    CARAPI CreateMonotoneCubicSpline(
        /* [in] */ ArrayOf<Float>* x,
        /* [in] */ ArrayOf<Float>* y,
        /* [out] */ ISpline** spline);

    /**
     * Creates a linear spline from a given set of control points.
     *
     * Like a monotone cubic spline, the interpolated curve will be monotonic if the control points
     * are monotonic.
     *
     * @param x The X component of the control points, strictly increasing.
     * @param y The Y component of the control points.
     * @return
     *
     * @throws IllegalArgumentException if the X or Y arrays are null, have
     * different lengths or have fewer than 2 values.
     * @throws IllegalArgumentException if the X components of the control points are not strictly
     * increasing.
     */
    CARAPI CreateLinearSpline(
        /* [in] */ ArrayOf<Float>* x,
        /* [in] */ ArrayOf<Float>* y,
        /* [out] */ ISpline** spline);
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_CSPLINE_HELPER_H__
