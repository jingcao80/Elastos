
#include "elastos/droid/webkit/CCookieManager.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

ECode CCookieManager::SetAcceptCookie(
    /* [in] */ Boolean accept)
{
    CookieManager::SetAcceptCookie(accept);
    return NOERROR;
}

ECode CCookieManager::AcceptCookie(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CookieManager::AcceptCookie();
    return NOERROR;
}

ECode CCookieManager::SetCookie(
    /* [in] */ const String& url,
    /* [in] */ const String& value)
{
    CookieManager::SetCookie(url, value);
    return NOERROR;
}

ECode CCookieManager::GetCookie(
    /* [in] */ const String& url,
    /* [out] */ String* cookie)
{
    VALIDATE_NOT_NULL(cookie);
    *cookie = CookieManager::GetCookie(url);
    return NOERROR;
}

ECode CCookieManager::GetCookie(
    /* [in] */ const String& url,
    /* [in] */ Boolean privateBrowsing,
    /* [out] */ String* cookie)
{
    VALIDATE_NOT_NULL(cookie);
    *cookie = CookieManager::GetCookie(url, privateBrowsing);
    return NOERROR;
}

ECode CCookieManager::GetCookie(
    /* [in] */ IWebAddress* uri,
    /* [out] */ String* cookie)
{
    VALIDATE_NOT_NULL(cookie);
    *cookie = CookieManager::GetCookie(uri);
    return NOERROR;
}

ECode CCookieManager::RemoveSessionCookie()
{
    CookieManager::RemoveSessionCookie();
    return NOERROR;
}

ECode CCookieManager::RemoveAllCookie()
{
    CookieManager::RemoveAllCookie();
    return NOERROR;
}

ECode CCookieManager::HasCookies(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = CookieManager::HasCookies();
    return NOERROR;
}

ECode CCookieManager::HasCookies(
    /* [in] */ Boolean privateBrowsing,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CookieManager::HasCookies();
    return NOERROR;
}

ECode CCookieManager::RemoveExpiredCookie()
{
    CookieManager::RemoveExpiredCookie();
    return NOERROR;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos
