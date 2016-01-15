#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CENABLEACCESSIBILITYCONTROLLERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CENABLEACCESSIBILITYCONTROLLERHELPER_H__

#include "_Elastos_Droid_Internal_Policy_Impl_CEnableAccessibilityControllerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

CarClass(CEnableAccessibilityControllerHelper)
    , public Singleton
    , public IEnableAccessibilityControllerHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI CanEnableAccessibilityViaGesture(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);


};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CENABLEACCESSIBILITYCONTROLLERHELPER_H__
