#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONEWINDOW_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONEWINDOW_H__

#include "_Elastos_Droid_Internal_Policy_Impl_CPhoneWindow.h"
#include "elastos/droid/internal/policy/impl/PhoneWindow.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

CarClass(CPhoneWindow)
    , public PhoneWindow
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONEWINDOW_H__
