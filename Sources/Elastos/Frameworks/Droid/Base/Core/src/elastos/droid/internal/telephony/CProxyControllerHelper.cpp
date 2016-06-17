
#include "elastos/droid/internal/telephony/CProxyControllerHelper.h"
#include "elastos/droid/internal/telephony/ProxyController.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

//==============================================================
//  CProxyControllerHelper::
//==============================================================

CAR_INTERFACE_IMPL(CProxyControllerHelper, Singleton, IProxyControllerHelper)

CAR_SINGLETON_IMPL(CProxyControllerHelper)

ECode CProxyControllerHelper::GetInstance(
    /* [in] */ IContext* context,
    /* [in] */ ArrayOf<IPhone*>* phoneProxy,
    /* [in] */ IUiccController* uiccController,
    /* [in] */ ArrayOf<ICommandsInterface*>* ci,
    /* [out] */ IProxyController** result)
{
    VALIDATE_NOT_NULL(result)
    return ProxyController::GetInstance(context, phoneProxy, uiccController, ci, result);
}

ECode CProxyControllerHelper::GetInstance(
    /* [out] */ IProxyController** result)
{
    VALIDATE_NOT_NULL(result)
    return ProxyController::GetInstance(result);
}


} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
