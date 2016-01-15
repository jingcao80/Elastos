#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/policy/CPolicyManager.h"
#include "elastos/droid/internal/policy/impl/CPolicy.h"

using Elastos::Droid::Internal::Policy::Impl::CPolicy;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {

CAR_INTERFACE_IMPL(CPolicyManager, Singleton, IPolicyManager)

CAR_SINGLETON_IMPL(CPolicyManager)


CPolicyManager::CPolicyManager()
{
    ASSERT_SUCCEEDED(CPolicy::New((IIPolicy**)&mPolicy));
}

ECode CPolicyManager::MakeNewWindow(
    /* [in] */ IContext* ctx,
    /* [out] */ IWindow** window)
{
    return mPolicy->MakeNewWindow(ctx, window);
}

ECode CPolicyManager::MakeNewLayoutInflater(
    /* [in] */ IContext* ctx,
    /* [out] */ ILayoutInflater** inflater)
{
    return mPolicy->MakeNewLayoutInflater(ctx, inflater);
}

ECode CPolicyManager::MakeNewWindowManager(
    /* [out] */ IWindowManagerPolicy** wm)
{
    return mPolicy->MakeNewWindowManager(wm);
}

ECode CPolicyManager::MakeNewFallbackEventHandler(
    /* [in] */ IContext* context,
    /* [out] */ IFallbackEventHandler** handler)
{
    return mPolicy->MakeNewFallbackEventHandler(context, handler);
}

} // namespace Policy
} // namespace Internal
} // namepsace Droid
} // namespace Elastos
