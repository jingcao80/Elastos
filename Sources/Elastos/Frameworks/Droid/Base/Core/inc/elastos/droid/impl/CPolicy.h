
#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPOLICY_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPOLICY_H__

#include "_Elastos_Droid_Internal_Policy_Impl_CPolicy.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IWindowManagerPolicy;
using Elastos::Droid::View::IFallbackEventHandler;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

CarClass(CPolicy)
{
public:
    CARAPI MakeNewWindow(
        /* [in] */ IContext* context,
        /* [out] */ IWindow** window);

    CARAPI MakeNewLayoutInflater(
        /* [in] */ IContext* context,
        /* [out] */ ILayoutInflater** inflater);

    CARAPI MakeNewWindowManager(
        /* [out] */ IWindowManagerPolicy** wm);

    CARAPI MakeNewFallbackEventHandler(
        /* [in] */ IContext* context,
        /* [out] */ IFallbackEventHandler** handler);
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPOLICY_H__
