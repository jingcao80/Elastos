
#include "elastos/droid/internal/policy/impl/LogDecelerateInterpolator.h"
#include "elastos/core/Math.h"

using Elastos::Droid::Animation::EIID_ITimeInterpolator;
using Elastos::Droid::View::Animation::EIID_IInterpolator;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

//=====================================================================
//                      LogDecelerateInterpolator
//=====================================================================
CAR_INTERFACE_IMPL_3(LogDecelerateInterpolator, Object, ILogDecelerateInterpolator, IInterpolator, ITimeInterpolator)

LogDecelerateInterpolator::LogDecelerateInterpolator()
{
}

ECode LogDecelerateInterpolator::constructor(
    /* [in] */ Int32 base,
    /* [in] */ Int32 drift)
{
    mBase = base;
    mDrift = drift;

    mLogScale = 1 / ComputeLog(1, mBase, mDrift);
    return NOERROR;
}

ECode LogDecelerateInterpolator::GetInterpolation(
    /* [in] */ Float t,
    /* [out] */ Float* output)
{
    VALIDATE_NOT_NULL(output)
    *output = ComputeLog(t, mBase, mDrift) * mLogScale;
    return NOERROR;
}
ECode LogDecelerateInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}


Float LogDecelerateInterpolator::ComputeLog(
    /* [in] */ Float t,
    /* [in] */ Int32 base,
    /* [in] */ Int32 drift)
{
    return (Float) -Elastos::Core::Math::Pow(base, -t) + 1 + (drift * t);
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos


