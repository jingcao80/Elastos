//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/webkit/CookieManager.h"
#include "elastos/droid/webkit/WebViewFactory.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;
using Elastos::Core::EIID_ICloneable;

namespace Elastos {
namespace Droid {
namespace Webkit {

Object CookieManager::sLock;

CAR_INTERFACE_IMPL_2(CookieManager, Object, ICookieManager, ICloneable);

/**
 * @hide Only for use by WebViewProvider implementations
 */
CookieManager::CookieManager()
{
}

/**
 * Gets the singleton CookieManager instance. If this method is used
 * before the application instantiates a {@link WebView} instance,
 * {@link CookieSyncManager#createInstance CookieSyncManager.createInstance(Context)}
 * must be called first.
 *
 * @return the singleton CookieManager instance
 */
//synchronized
AutoPtr<ICookieManager> CookieManager::GetInstance()
{
    AutoLock lock(sLock);
    AutoPtr<ICookieManager> cookieManager;
    WebViewFactory::GetProvider()->GetCookieManager((ICookieManager**)&cookieManager);
    return cookieManager;
}

/**
 * Sets whether the application's {@link WebView} instances should send and
 * accept cookies.
 *
 * @param accept whether {@link WebView} instances should send and accept
 *               cookies
 */
//synchronized
ECode CookieManager::SetAcceptCookie(
    /* [in] */ Boolean accept)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether the application's {@link WebView} instances send and accept
 * cookies.
 *
 * @return true if {@link WebView} instances send and accept cookies
 */
//synchronized
ECode CookieManager::AcceptCookie(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

ECode CookieManager::SetAcceptThirdPartyCookies(
    /* [in] */ IWebView* webview,
    /* [in] */ Boolean accept)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

ECode CookieManager::AcceptThirdPartyCookies(
    /* [in] */ IWebView* webview,
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

 /**
 * Sets a cookie for the given URL. Any existing cookie with the same host,
 * path and name will be replaced with the new cookie. The cookie being set
 * must not have expired and must not be a session cookie, otherwise it
 * will be ignored.
 *
 * @param url the URL for which the cookie is set
 * @param value the cookie as a string, using the format of the 'Set-Cookie'
 *              HTTP response header
 */
ECode CookieManager::SetCookie(
    /* [in] */ const String& url,
    /* [in] */ const String& value)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

ECode CookieManager::SetCookie(
    /* [in] */ const String& url,
    /* [in] */ const String& value,
    /* [in] */ IValueCallback* callBack)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets the cookies for the given URL.
 *
 * @param url the URL for which the cookies are requested
 * @return value the cookies as a string, using the format of the 'Cookie'
 *               HTTP request header
 */
ECode CookieManager::GetCookie(
    /* [in] */ const String& url,
    /* [out] */ String* cookie)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * See {@link #getCookie(String)}.
 *
 * @param url the URL for which the cookies are requested
 * @param privateBrowsing whether to use the private browsing cookie jar
 * @return value the cookies as a string, using the format of the 'Cookie'
 *               HTTP request header
 * @hide Used by Browser, no intention to publish.
 */
ECode CookieManager::GetCookie(
    /* [in] */ const String& url,
    /* [in] */ Boolean privateBrowsing,
    /* [out] */ String* cookie)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets cookie(s) for a given uri so that it can be set to "cookie:" in http
 * request header.
 *
 * @param uri the WebAddress for which the cookies are requested
 * @return value the cookies as a string, using the format of the 'Cookie'
 *               HTTP request header
 * @hide Used by RequestHandle, no intention to publish.
 */
//synchronized
ECode CookieManager::GetCookie(
    /* [in] */ IWebAddress* uri,
    /* [out] */ String* cookie)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Removes all session cookies, which are cookies without an expiration
 * date.
 */
ECode CookieManager::RemoveSessionCookie()
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

ECode CookieManager::RemoveSessionCookies(
    /* [in] */ IValueCallback* callBack)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Removes all cookies.
 */
ECode CookieManager::RemoveAllCookie()
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

ECode CookieManager::RemoveAllCookies(
    /* [in] */ IValueCallback* callBack)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether there are stored cookies.
 *
 * @return true if there are stored cookies
 */
//synchronized
ECode CookieManager::HasCookies(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * See {@link #hasCookies()}.
 *
 * @param privateBrowsing whether to use the private browsing cookie jar
 * @hide Used by Browser, no intention to publish.
 */
//synchronized
ECode CookieManager::HasCookies(
    /* [in] */ Boolean privateBrowsing,
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Removes all expired cookies.
 */
ECode CookieManager::RemoveExpiredCookie()
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

ECode CookieManager::Flush()
{
    FlushCookieStore();
    return NOERROR;
}

/**
 * Gets whether the application's {@link WebView} instances send and accept
 * cookies for file scheme URLs.
 *
 * @return true if {@link WebView} instances send and accept cookies for
 *         file scheme URLs
 */
// Static for backward compatibility.
Boolean CookieManager::AllowFileSchemeCookies()
{
    return ((CookieManager*)GetInstance().Get())->AllowFileSchemeCookiesImpl();
}

/**
 * Sets whether the application's {@link WebView} instances should send and
 * accept cookies for file scheme URLs.
 * Use of cookies with file scheme URLs is potentially insecure. Do not use
 * this feature unless you can be sure that no unintentional sharing of
 * cookie data can take place.
 * <p>
 * Note that calls to this method will have no effect if made after a
 * {@link WebView} or CookieManager instance has been created.
 */
// Static for backward compatibility.
void CookieManager::SetAcceptFileSchemeCookies(
    /* [in] */ Boolean accept)
{
    ((CookieManager*)GetInstance().Get())->SetAcceptFileSchemeCookiesImpl(accept);
}

/**
 * Implements {@link #setAcceptFileSchemeCookies(boolean)}.
 *
 * @hide Only for use by WebViewProvider implementations
 */
void CookieManager::SetAcceptFileSchemeCookiesImpl(
    /* [in] */ Boolean accept)
{
    assert(0);
}

//@Override
ECode CookieManager::Clone(
    /*[out]*/ IInterface** object)
{
    return E_CLONE_NOT_SUPPORTED_EXCEPTION;
}

/**
 * Flushes all cookies managed by the Chrome HTTP stack to flash.
 *
 * @hide Package level api, called from CookieSyncManager
 */
void CookieManager::FlushCookieStore()
{
}

/**
 * Implements {@link #allowFileSchemeCookies()}.
 *
 * @hide Only for use by WebViewProvider implementations
 */
Boolean CookieManager::AllowFileSchemeCookiesImpl()
{
    assert(0);
    return FALSE;
}

ECode CookieManager::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "CookieManager";
    return NOERROR;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos