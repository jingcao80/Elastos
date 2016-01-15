#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CLOGDECELERATEINTERPOLATOR_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CLOGDECELERATEINTERPOLATOR_H__

#include "_Elastos_Droid_Internal_Policy_Impl_CLogDecelerateInterpolator.h"
#include "elastos/droid/internal/policy/impl/LogDecelerateInterpolator.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

CarClass(CLogDecelerateInterpolator)
    , public LogDecelerateInterpolator
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CLOGDECELERATEINTERPOLATOR_H__
