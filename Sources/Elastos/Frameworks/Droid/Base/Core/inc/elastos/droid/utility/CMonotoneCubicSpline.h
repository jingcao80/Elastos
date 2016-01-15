
#ifndef __ELASTOS_DROID_UTILITY_CMONOTONE_CUBIC_SPLINE_H__
#define __ELASTOS_DROID_UTILITY_CMONOTONE_CUBIC_SPLINE_H__

#include "_Elastos_Droid_Utility_CMonotoneCubicSpline.h"
#include "elastos/droid/utility/Spline.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CMonotoneCubicSpline)
    , public MonotoneCubicSpline
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_CMONOTONE_CUBIC_SPLINE_H__
