#ifndef __ELASTOS_DROID_INTERNAL_POLICY_CPOLICYMANAGER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_CPOLICYMANAGER_H__

#include "_Elastos_Droid_Internal_Policy_CPolicyManager.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IWindowManagerPolicy;
using Elastos::Droid::View::IFallbackEventHandler;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {

CarClass(CPolicyManager)
    , public Singleton
    , public IPolicyManager
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CPolicyManager();

    CARAPI MakeNewWindow(
        /* [in] */ IContext* ctx,
        /* [out] */ IWindow** window);

    CARAPI MakeNewLayoutInflater(
        /* [in] */ IContext* ctx,
        /* [out] */ ILayoutInflater** inflater);

    CARAPI MakeNewWindowManager(
        /* [out] */ IWindowManagerPolicy** wm);

    CARAPI MakeNewFallbackEventHandler(
        /* [in] */ IContext* context,
        /* [out] */ IFallbackEventHandler** handler);

private:
    AutoPtr<IIPolicy> mPolicy;
};

} // namespace Policy
} // namespace Internal
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_CPOLICYMANAGER_H__
