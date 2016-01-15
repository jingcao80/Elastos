#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPOLICYCONTROL_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPOLICYCONTROL_H__

#include "_Elastos_Droid_Internal_Policy_Impl_CPolicyControl.h"
#include "elastos/droid/internal/policy/impl/PolicyControl.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

CarClass(CPolicyControl)
    , public Singleton
    , public IPolicyControl
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetSystemUiVisibility(
        /* [in] */ IWindowState* win,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [out] */ Int32* res);

    CARAPI GetWindowFlags(
        /* [in] */ IWindowState* win,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [out] */ Int32* res);

    CARAPI AdjustClearableFlags(
        /* [in] */ IWindowState* win,
        /* [in] */ Int32 clearableFlags,
        /* [out] */ Int32* res);

    CARAPI DisableImmersiveConfirmation(
        /* [in] */ const String& pkg,
        /* [out] */ Boolean* res);

    CARAPI ReloadFromSetting(
        /* [in] */ IContext* context);

    CARAPI Dump(
        /* [in] */ const String& prefix,
        /* [in] */ IPrintWriter* pw);
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPOLICYCONTROL_H__
