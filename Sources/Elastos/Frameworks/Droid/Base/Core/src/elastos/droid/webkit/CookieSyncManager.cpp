
#include "elastos/droid/webkit/CookieManager.h"
#include "elastos/droid/webkit/CookieSyncManager.h"
#include "elastos/droid/webkit/WebViewDatabase.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

AutoPtr<ICookieSyncManager> CookieSyncManager::sRef;
Boolean CookieSyncManager::sGetInstanceAllowed = FALSE;

CAR_INTERFACE_IMPL(CookieSyncManager, Object, ICookieSyncManager);

CookieSyncManager::CookieSyncManager()
    : WebSyncManager(NULL, String(NULL))
{
}

ECode CookieSyncManager::constructor()
{
    return NOERROR;
}

ECode CookieSyncManager::Sync()
{
    return CookieManager::GetInstance()->Flush();
}

ECode CookieSyncManager::ResetSync()
{
    return NOERROR;
}

ECode CookieSyncManager::StartSync()
{
    return NOERROR;
}

ECode CookieSyncManager::StopSync()
{
    return NOERROR;
}

ECode CookieSyncManager::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "CookieSyncManager";
    return NOERROR;
}

void CookieSyncManager::SyncFromRamToFlash()
{
    CookieManager::GetInstance()->Flush();
}

ECode CookieSyncManager::SetGetInstanceIsAllowed()
{
    sGetInstanceAllowed = TRUE;
    return NOERROR;
}

ECode CookieSyncManager::CheckInstanceIsAllowed()
{
    if (!sGetInstanceAllowed) {
        // throw new IllegalStateException(
        //             "CookieSyncManager::createInstance() needs to be called "
        //                     + "before CookieSyncManager::getInstance()");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos