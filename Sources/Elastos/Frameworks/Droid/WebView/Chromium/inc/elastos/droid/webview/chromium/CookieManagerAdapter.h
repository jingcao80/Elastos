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

#ifndef _ELASTOS_DROID_WEBKIT_CHROMIUM_COOKIEMANAGERADAPTER_H_
#define _ELASTOS_DROID_WEBKIT_CHROMIUM_COOKIEMANAGERADAPTER_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/CookieManager.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwCookieManager.h"

using Elastos::Droid::Net::IWebAddress;
using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::AwCookieManager;
using Elastos::Droid::Webkit::CookieManager;
using Elastos::Droid::Webkit::IValueCallback;
using Elastos::Droid::Webkit::IWebView;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

class CookieManagerAdapter
    : public CookieManager
{
public:
    CookieManagerAdapter(
        /* [in] */ AwCookieManager* chromeCookieManager);

    // @Override
    // synchronized
    CARAPI SetAcceptCookie(
        /* [in] */ Boolean accept);

    // @Override
    // synchronized
    CARAPI AcceptCookie(
        /* [out] */ Boolean* result);

    // @Override
    // synchronized
    CARAPI SetAcceptThirdPartyCookies(
        /* [in] */ IWebView* webView,
        /* [in] */ Boolean accept);

    // @Override
    // synchronized
    CARAPI AcceptThirdPartyCookies(
        /* [in] */ IWebView* webView,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetCookie(
        /* [in] */ const String& url,
        /* [in] */ const String& value);

    // @Override
    CARAPI SetCookie(
        /* [in] */ const String& url,
        /* [in] */ const String& value,
        /* [in] */ IValueCallback* callback);

    // @Override
    CARAPI GetCookie(
        /* [in] */ const String& url,
        /* [out] */ String* result);

    // @Override
    CARAPI GetCookie(
        /* [in] */ const String& url,
        /* [in] */ Boolean privateBrowsing,
        /* [out] */ String* result);

    // @Override
    // synchronized
    CARAPI GetCookie(
        /* [in] */ IWebAddress* uri,
        /* [out] */ String* result);

    // @Override
    CARAPI RemoveSessionCookie();

    // @Override
    CARAPI RemoveSessionCookies(
        /* [in] */ IValueCallback* callback);

    // @Override
    CARAPI RemoveAllCookie();

    // @Override
    CARAPI RemoveAllCookies(
        /* [in] */ IValueCallback* callback);

    // @Override
    // synchronized
    CARAPI HasCookies(
        /* [out] */ Boolean* result);

    // @Override
    // synchronized
    CARAPI HasCookies(
        /* [in] */ Boolean privateBrowsing,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI RemoveExpiredCookie();

protected:
    // @Override
    CARAPI_(void) FlushCookieStore();

    // @Override
    CARAPI_(Boolean) AllowFileSchemeCookiesImpl();

    // @Override
    CARAPI_(void) SetAcceptFileSchemeCookiesImpl(
        /* [in] */ Boolean accept);

private:
    static CARAPI_(String) FixupUrl(
        /* [in] */ const String& url);

public:
    AutoPtr<AwCookieManager> mChromeCookieManager;

private:
    static const String LOGTAG;
};

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_CHROMIUM_COOKIEMANAGERADAPTER_H_

