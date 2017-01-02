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

#ifndef __ELASTOS_DROID_WEBKIT_COOKIEMANAGER_H__
#define __ELASTOS_DROID_WEBKIT_COOKIEMANAGER_H__

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IWebAddress;
using Elastos::Core::ICloneable;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * Manages the cookies used by an application's {@link WebView} instances.
 * Cookies are manipulated according to RFC2109.
 */
class ECO_PUBLIC CookieManager
    : public Object
    , public ICookieManager
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Gets the singleton CookieManager instance. If this method is used
     * before the application instantiates a {@link WebView} instance,
     * {@link CookieSyncManager#createInstance CookieSyncManager.createInstance(Context)}
     * must be called first.
     *
     * @return the singleton CookieManager instance
     */
    //synchronized
    static CARAPI_(AutoPtr<ICookieManager>) GetInstance();

    /**
     * Sets whether the application's {@link WebView} instances should send and
     * accept cookies.
     *
     * @param accept whether {@link WebView} instances should send and accept
     *               cookies
     */
    //synchronized
    CARAPI SetAcceptCookie(
        /* [in] */ Boolean accept);

    /**
     * Gets whether the application's {@link WebView} instances send and accept
     * cookies.
     *
     * @return true if {@link WebView} instances send and accept cookies
     */
    //synchronized
    CARAPI AcceptCookie(
        /* [out] */ Boolean* result);

    /**
     * Sets whether the {@link WebView} should allow third party cookies to be set.
     * Allowing third party cookies is a per WebView policy and can be set
     * differently on different WebView instances.
     * <p>
     * Apps that target {@link android.os.Build.VERSION_CODES#KITKAT} or below
     * default to allowing third party cookies. Apps targeting
     * {@link android.os.Build.VERSION_CODES#LOLLIPOP} or later default to disallowing
     * third party cookies.
     *
     * @param webview the {@link WebView} instance to set the cookie policy on
     * @param accept whether the {@link WebView} instance should accept
     *               third party cookies
     */
    CARAPI SetAcceptThirdPartyCookies(
        /* [in] */ IWebView* webview,
        /* [in] */ Boolean accept);

    /**
     * Gets whether the {@link WebView} should allow third party cookies to be set.
     *
     * @param webview the {@link WebView} instance to get the cookie policy for
     * @return true if the {@link WebView} accepts third party cookies
     */
    CARAPI AcceptThirdPartyCookies(
        /* [in] */ IWebView* webview,
        /* [out] */ Boolean* result);

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
    CARAPI SetCookie(
        /* [in] */ const String& url,
        /* [in] */ const String& value);

    CARAPI SetCookie(
        /* [in] */ const String& url,
        /* [in] */ const String& value,
        /* [in] */ IValueCallback* callBack);

    /**
     * Gets the cookies for the given URL.
     *
     * @param url the URL for which the cookies are requested
     * @return value the cookies as a string, using the format of the 'Cookie'
     *               HTTP request header
     */
    CARAPI GetCookie(
        /* [in] */ const String& url,
        /* [out] */ String* cookie);

    /**
     * See {@link #getCookie(String)}.
     *
     * @param url the URL for which the cookies are requested
     * @param privateBrowsing whether to use the private browsing cookie jar
     * @return value the cookies as a string, using the format of the 'Cookie'
     *               HTTP request header
     * @hide Used by Browser, no intention to publish.
     */
    CARAPI GetCookie(
        /* [in] */ const String& url,
        /* [in] */ Boolean privateBrowsing,
        /* [out] */ String* cookie);

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
    CARAPI GetCookie(
        /* [in] */ IWebAddress* uri,
        /* [out] */ String* cookie);

    /**
     * Removes all session cookies, which are cookies without an expiration
     * date.
     */
    CARAPI RemoveSessionCookie();

    /**
     * Removes all session cookies, which are cookies without an expiration
     * date.
     * <p>
     * This method is asynchronous.
     * If a {@link ValueCallback} is provided,
     * {@link ValueCallback#onReceiveValue(T) onReceiveValue()} will be called on the current
     * thread's {@link android.os.Looper} once the operation is complete.
     * The value provided to the callback indicates whether any cookies were removed.
     * You can pass {@code null} as the callback if you don't need to know when the operation
     * completes or whether any cookie were removed, and in this case it is safe to call the
     * method from a thread without a Looper.
     * @param callback a callback which is executed when the session cookies have been removed
     */
    CARAPI RemoveSessionCookies(
        /* [in] */ IValueCallback* callBack);

    /**
     * Removes all cookies.
     */
    CARAPI RemoveAllCookie();

    /**
     * Removes all cookies.
     * <p>
     * This method is asynchronous.
     * If a {@link ValueCallback} is provided,
     * {@link ValueCallback#onReceiveValue(T) onReceiveValue()} will be called on the current
     * thread's {@link android.os.Looper} once the operation is complete.
     * The value provided to the callback indicates whether any cookies were removed.
     * You can pass {@code null} as the callback if you don't need to know when the operation
     * completes or whether any cookies were removed, and in this case it is safe to call the
     * method from a thread without a Looper.
     * @param callback a callback which is executed when the cookies have been removed
     */
    CARAPI RemoveAllCookies(
        /* [in] */ IValueCallback* callBack);

    /**
     * Gets whether there are stored cookies.
     *
     * @return true if there are stored cookies
     */
    //synchronized
    CARAPI HasCookies(
        /* [out] */ Boolean* result);

    /**
     * See {@link #hasCookies()}.
     *
     * @param privateBrowsing whether to use the private browsing cookie jar
     * @hide Used by Browser, no intention to publish.
     */
    //synchronized
    CARAPI HasCookies(
        /* [in] */ Boolean privateBrowsing,
        /* [out] */ Boolean* result);

    /**
     * Removes all expired cookies.
     */
    CARAPI RemoveExpiredCookie();

    /**
     * Ensures all cookies currently accessible through the getCookie API are
     * written to persistent storage.
     * This call will block the caller until it is done and may perform I/O.
     */
    CARAPI Flush();

    /**
     * Gets whether the application's {@link WebView} instances send and accept
     * cookies for file scheme URLs.
     *
     * @return true if {@link WebView} instances send and accept cookies for
     *         file scheme URLs
     */
    // Static for backward compatibility.
    static CARAPI_(Boolean) AllowFileSchemeCookies();

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
    static CARAPI_(void) SetAcceptFileSchemeCookies(
        /* [in] */ Boolean accept);

    CARAPI ToString(
        /* [out] */ String* info);

protected:
    /**
     * @hide Only for use by WebViewProvider implementations
     */
    CookieManager();

    CARAPI Clone(
        /* [out] */ IInterface** object);

    /**
     * Flushes all cookies managed by the Chrome HTTP stack to flash.
     *
     * @hide Package level api, called from CookieSyncManager
     */
    virtual CARAPI_(void) FlushCookieStore();

    /**
     * Implements {@link #setAcceptFileSchemeCookies(boolean)}.
     *
     * @hide Only for use by WebViewProvider implementations
     */
    virtual CARAPI_(void) SetAcceptFileSchemeCookiesImpl(
        /* [in] */ Boolean accept);

    /**
     * Implements {@link #allowFileSchemeCookies()}.
     *
     * @hide Only for use by WebViewProvider implementations
     */
    virtual CARAPI_(Boolean) AllowFileSchemeCookiesImpl();

protected:
    static Object sLock;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_COOKIEMANAGER_H__
