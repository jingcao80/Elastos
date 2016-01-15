#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_LOGDECELERATEINTERPOLATOR_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_LOGDECELERATEINTERPOLATOR_H__

#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::View::Animation::IInterpolator;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

class LogDecelerateInterpolator
    : public Object
    , public ILogDecelerateInterpolator
    , public IInterpolator
    , public ITimeInterpolator
{
public:
    CAR_INTERFACE_DECL()

    LogDecelerateInterpolator();

    CARAPI constructor(
        /* [in] */ Int32 base,
        /* [in] */ Int32 drift);

    // @Override
    CARAPI GetInterpolation(
        /* [in] */ Float t,
        /* [out] */ Float* output);

private:
    CARAPI HasNativeInterpolator(
        /* [out] */ Boolean* res);

    static CARAPI_(Float) ComputeLog(
        /* [in] */ Float t,
        /* [in] */ Int32 base,
        /* [in] */ Int32 drift);

private:
    Int32 mBase;
    Int32 mDrift;
    Float mLogScale;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_LOGDECELERATEINTERPOLATOR_H__

