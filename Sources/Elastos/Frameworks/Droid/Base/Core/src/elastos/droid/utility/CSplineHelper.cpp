
#include "elastos/droid/utility/CSplineHelper.h"
#include "elastos/droid/utility/Spline.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_INTERFACE_IMPL(CSplineHelper, Singleton, ISplineHelper)

CAR_SINGLETON_IMPL(CSplineHelper)

ECode CSplineHelper::CreateSpline(
    /* [in] */ ArrayOf<Float>* x,
    /* [in] */ ArrayOf<Float>* y,
    /* [out] */ ISpline** spline)
{
    return Spline::CreateSpline(x, y, spline);
}

ECode CSplineHelper::CreateMonotoneCubicSpline(
    /* [in] */ ArrayOf<Float>* x,
    /* [in] */ ArrayOf<Float>* y,
    /* [out] */ ISpline** spline)
{
    return Spline::CreateMonotoneCubicSpline(x, y, spline);
}

ECode CSplineHelper::CreateLinearSpline(
    /* [in] */ ArrayOf<Float>* x,
    /* [in] */ ArrayOf<Float>* y,
    /* [out] */ ISpline** spline)
{
    return Spline::CreateLinearSpline(x, y, spline);
}


} // namespace Utility
} // namespace Droid
} // namespace Elastos
