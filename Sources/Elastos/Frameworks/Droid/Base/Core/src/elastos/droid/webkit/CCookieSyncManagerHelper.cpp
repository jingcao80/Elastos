
#include "elastos/droid/webkit/CCookieSyncManagerHelper.h"
#include "elastos/droid/webkit/CCookieSyncManager.h"
#include "elastos/droid/webkit/CookieSyncManager.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(CCookieSyncManagerHelper, Object, ICookieSyncManagerHelper);

CAR_SINGLETON_IMPL(CCookieSyncManagerHelper);

ECode CCookieSyncManagerHelper::GetInstance(
    /* [out] */ ICookieSyncManager** instance)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(instance);
    FAIL_RETURN(CookieSyncManager::CheckInstanceIsAllowed());
    if (CookieSyncManager::sRef == NULL) {
        FAIL_RETURN(CCookieSyncManager::New((ICookieSyncManager**)&CookieSyncManager::sRef));
    }
    *instance = CookieSyncManager::sRef;
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

ECode CCookieSyncManagerHelper::CreateInstance(
    /* [in] */ IContext* context,
    /* [out] */ ICookieSyncManager** instance)
{
    AutoLock lock(this);
    if (context == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    VALIDATE_NOT_NULL(instance);
    FAIL_RETURN(CookieSyncManager::SetGetInstanceIsAllowed());
    return GetInstance(instance);
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos