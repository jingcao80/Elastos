#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CSYSTEMGESTURESPOINTEREVENTLISTENER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CSYSTEMGESTURESPOINTEREVENTLISTENER_H__

#include "_Elastos_Droid_Internal_Policy_Impl_CSystemGesturesPointerEventListener.h"
#include "elastos/droid/internal/policy/impl/SystemGesturesPointerEventListener.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

CarClass(CSystemGesturesPointerEventListener)
    , public SystemGesturesPointerEventListener
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CSYSTEMGESTURESPOINTEREVENTLISTENER_H__
