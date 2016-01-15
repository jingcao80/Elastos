#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONELAYOUTINFLATER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONELAYOUTINFLATER_H__

#include "_Elastos_Droid_Internal_Policy_Impl_CPhoneLayoutInflater.h"
#include "elastos/droid/internal/policy/impl/PhoneLayoutInflater.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

CarClass(CPhoneLayoutInflater)
    , public PhoneLayoutInflater
{
public:
    CAR_OBJECT_DECL()

    CARAPI CloneInContext(
        /* [in] */ IContext* newContext,
        /* [out] */ ILayoutInflater** newInflater);

};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONELAYOUTINFLATER_H__
