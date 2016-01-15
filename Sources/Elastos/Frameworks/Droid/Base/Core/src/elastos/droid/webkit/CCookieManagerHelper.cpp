
#include "elastos/droid/webkit/CCookieManagerHelper.h"
#include "elastos/droid/webkit/CookieManager.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(CCookieManagerHelper, Object, ICookieManagerHelper);

CAR_SINGLETON_IMPL(CCookieManagerHelper);

ECode CCookieManagerHelper::GetInstance(
    /* [out] */ ICookieManager** cookieManager)
{
    VALIDATE_NOT_NULL(cookieManager);
    AutoPtr<ICookieManager> _cookieManager = CookieManager::GetInstance();
    *cookieManager = _cookieManager.Get();
    REFCOUNT_ADD(*cookieManager);
    return NOERROR;
}

ECode CCookieManagerHelper::AllowFileSchemeCookies(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CookieManager::AllowFileSchemeCookies();
    return NOERROR;
}

ECode CCookieManagerHelper::SetAcceptFileSchemeCookies(
    /* [in] */ Boolean accept)
{
    CookieManager::SetAcceptFileSchemeCookies(accept);
    return NOERROR;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos
