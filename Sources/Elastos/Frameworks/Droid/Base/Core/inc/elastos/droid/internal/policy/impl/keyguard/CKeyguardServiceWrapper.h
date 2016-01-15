#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_KEYGUARD_CKEYGUARDSERVICEWRAPPER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_KEYGUARD_CKEYGUARDSERVICEWRAPPER_H__

#include "_Elastos_Droid_Internal_Policy_Impl_Keyguard_CKeyguardServiceWrapper.h"
#include "elastos/droid/internal/policy/impl/keyguard/KeyguardServiceWrapper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {
namespace Keyguard {

CarClass(CKeyguardServiceWrapper)
    , public KeyguardServiceWrapper
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

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_KEYGUARD_CKEYGUARDSERVICEWRAPPER_H__
