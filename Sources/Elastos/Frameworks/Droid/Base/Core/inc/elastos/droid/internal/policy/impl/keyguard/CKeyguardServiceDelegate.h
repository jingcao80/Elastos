#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_KEYGUARD_CKEYGUARDSERVICEDELEGATE_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_KEYGUARD_CKEYGUARDSERVICEDELEGATE_H__

#include "_Elastos_Droid_Internal_Policy_Impl_Keyguard_CKeyguardServiceDelegate.h"
#include "elastos/droid/internal/policy/impl/keyguard/KeyguardServiceDelegate.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {
namespace Keyguard {

/**
* A local class that keeps a cache of keyguard state that can be restored in the event
* keyguard crashes. It currently also allows runtime-selectable
* local or remote instances of keyguard.
*/
CarClass(CKeyguardServiceDelegate)
    , public KeyguardServiceDelegate
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Keyguard
} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_KEYGUARD_CKEYGUARDSERVICEDELEGATE_H__
