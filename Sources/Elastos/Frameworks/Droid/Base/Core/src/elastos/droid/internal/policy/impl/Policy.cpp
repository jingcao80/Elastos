
#include "elastos/droid/internal/policy/impl/Policy.h"
#include "elastos/droid/internal/policy/impl/CPhoneWindow.h"
#include "elastos/droid/internal/policy/impl/CPhoneLayoutInflater.h"
#include "elastos/droid/internal/policy/impl/CPhoneWindowManager.h"
#include "elastos/droid/internal/policy/impl/CPhoneFallbackEventHandler.h"

using Elastos::Droid::Internal::Policy::EIID_IIPolicy;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

//=====================================================================
//                                Policy
//=====================================================================
CAR_INTERFACE_IMPL(Policy, Object, IIPolicy);
const String Policy::TAG("PhonePolicy");

Policy::Policy()
{
}

ECode Policy::MakeNewWindow(
    /* [in] */ IContext* context,
    /* [out] */ IWindow** window)
{
    VALIDATE_NOT_NULL(window);
    return CPhoneWindow::New(context, window);
}

ECode Policy::MakeNewLayoutInflater(
    /* [in] */ IContext* context,
    /* [out] */ ILayoutInflater** inflater)
{
    VALIDATE_NOT_NULL(inflater);
    return CPhoneLayoutInflater::New(context, inflater);
}

ECode Policy::MakeNewWindowManager(
    /* [out] */ IWindowManagerPolicy** wm)
{
    VALIDATE_NOT_NULL(wm);
    return CPhoneWindowManager::New(wm);
}

ECode Policy::MakeNewFallbackEventHandler(
    /* [in] */ IContext* context,
    /* [out] */ IFallbackEventHandler** handler)
{
    VALIDATE_NOT_NULL(handler);
    AutoPtr<IPhoneFallbackEventHandler> phoneHandler;
    FAIL_RETURN(CPhoneFallbackEventHandler::New(context, (IPhoneFallbackEventHandler**)&phoneHandler));
    *handler = (IFallbackEventHandler*)phoneHandler.Get();
    REFCOUNT_ADD(*handler);
    return NOERROR;
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos
