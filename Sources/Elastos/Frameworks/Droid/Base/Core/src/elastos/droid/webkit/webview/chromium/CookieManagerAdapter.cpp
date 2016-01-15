
#include "elastos/droid/net/CWebAddress.h"
#include "elastos/droid/webkit/webview/chromium/CookieManagerAdapter.h"
#include "elastos/core/AutoLock.h"

using Elastos::Droid::Net::CWebAddress;
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

//=====================================================================
//                         CookieManagerAdapter
//=====================================================================
const String CookieManagerAdapter::LOGTAG("CookieManager");

CookieManagerAdapter::CookieManagerAdapter(
    /* [in] */ AwCookieManager* chromeCookieManager)
    : mChromeCookieManager(chromeCookieManager)
{
    // ==================before translated======================
    // mChromeCookieManager = chromeCookieManager;

    assert(mChromeCookieManager);
}

// synchronized
ECode CookieManagerAdapter::SetAcceptCookie(
    /* [in] */ Boolean accept)
{
    // ==================before translated======================
    // mChromeCookieManager.setAcceptCookie(accept);

    AutoLock lock(this);
    mChromeCookieManager->SetAcceptCookie(accept);
    return NOERROR;
}

// synchronized
ECode CookieManagerAdapter::AcceptCookie(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mChromeCookieManager.acceptCookie();

    AutoLock lock(this);
    *result = mChromeCookieManager->AcceptCookie();
    return NOERROR;
}

// synchronized
ECode CookieManagerAdapter::SetAcceptThirdPartyCookies(
    /* [in] */ IWebView* webView,
    /* [in] */ Boolean accept)
{
    VALIDATE_NOT_NULL(webView);
    // ==================before translated======================
    // webView.getSettings().setAcceptThirdPartyCookies(accept);

    AutoLock lock(this);
    AutoPtr<IWebSettings> webSettings;
    webView->GetSettings((IWebSettings**)&webSettings);
    return webSettings->SetAcceptThirdPartyCookies(accept);
}

// synchronized
ECode CookieManagerAdapter::AcceptThirdPartyCookies(
    /* [in] */ IWebView* webView,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(webView);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return webView.getSettings().getAcceptThirdPartyCookies();

    AutoLock lock(this);
    AutoPtr<IWebSettings> webSettings;
    webView->GetSettings((IWebSettings**)&webSettings);
    return webSettings->GetAcceptThirdPartyCookies(result);
}

ECode CookieManagerAdapter::SetCookie(
    /* [in] */ const String& url,
    /* [in] */ const String& value)
{
    // ==================before translated======================
    // try {
    //     mChromeCookieManager.setCookie(fixupUrl(url), value);
    // } catch (ParseException e) {
    //     Log.e(LOGTAG, "Not setting cookie due to error parsing URL: " + url, e);
    // }

    // try {
        mChromeCookieManager->SetCookie(FixupUrl(url), value);
    // } catch (ParseException e) {
    //     Log.e(LOGTAG, "Not setting cookie due to error parsing URL: " + url, e);
    // }
    return NOERROR;
}

ECode CookieManagerAdapter::SetCookie(
    /* [in] */ const String& url,
    /* [in] */ const String& value,
    /* [in] */ IValueCallback* callback)
{
    VALIDATE_NOT_NULL(callback);
    // ==================before translated======================
    // try {
    //     mChromeCookieManager.setCookie(fixupUrl(url), value, callback);
    // } catch (ParseException e) {
    //     Log.e(LOGTAG, "Not setting cookie due to error parsing URL: " + url, e);
    // }

    // try {
        mChromeCookieManager->SetCookie(FixupUrl(url), value, callback);
    // } catch (ParseException e) {
    //     Log.e(LOGTAG, "Not setting cookie due to error parsing URL: " + url, e);
    // }
    return NOERROR;
}

ECode CookieManagerAdapter::GetCookie(
    /* [in] */ const String& url,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // try {
    //     return mChromeCookieManager.getCookie(fixupUrl(url));
    // } catch (ParseException e) {
    //     Log.e(LOGTAG, "Unable to get cookies due to error parsing URL: " + url, e);
    //     return null;
    // }

    // try {
        *result = mChromeCookieManager->GetCookie(FixupUrl(url));
        return NOERROR;
    // } catch (ParseException e) {
    //     Log.e(LOGTAG, "Unable to get cookies due to error parsing URL: " + url, e);
    //     return null;
    // }
    *result = String("");
    return NOERROR;
}

ECode CookieManagerAdapter::GetCookie(
    /* [in] */ const String& url,
    /* [in] */ Boolean privateBrowsing,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getCookie(url);

    return GetCookie(url, result);
}

// synchronized
ECode CookieManagerAdapter::GetCookie(
    /* [in] */ IWebAddress* uri,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mChromeCookieManager.getCookie(uri.toString());

    AutoLock lock(this);
    String address;
    IObject* objTmp = IObject::Probe(uri);
    objTmp->ToString(&address);
    *result = mChromeCookieManager->GetCookie(address);
    return NOERROR;
}

ECode CookieManagerAdapter::RemoveSessionCookie()
{
    // ==================before translated======================
    // mChromeCookieManager.removeSessionCookies();

    mChromeCookieManager->RemoveSessionCookies();
    return NOERROR;
}

ECode CookieManagerAdapter::RemoveSessionCookies(
    /* [in] */ IValueCallback* callback)
{
    VALIDATE_NOT_NULL(callback);
    // ==================before translated======================
    // mChromeCookieManager.removeSessionCookies(callback);

    mChromeCookieManager->RemoveSessionCookies(callback);
    return NOERROR;
}

ECode CookieManagerAdapter::RemoveAllCookie()
{
    // ==================before translated======================
    // mChromeCookieManager.removeAllCookies();

    mChromeCookieManager->RemoveAllCookies();
    return NOERROR;
}

ECode CookieManagerAdapter::RemoveAllCookies(
    /* [in] */ IValueCallback* callback)
{
    VALIDATE_NOT_NULL(callback);
    // ==================before translated======================
    // mChromeCookieManager.removeAllCookies(callback);

    mChromeCookieManager->RemoveAllCookies(callback);
    return NOERROR;
}

// synchronized
ECode CookieManagerAdapter::HasCookies(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mChromeCookieManager.hasCookies();

    AutoLock lock(this);
    *result = mChromeCookieManager->HasCookies();
    return NOERROR;
}

// synchronized
ECode CookieManagerAdapter::HasCookies(
    /* [in] */ Boolean privateBrowsing,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mChromeCookieManager.hasCookies();

    AutoLock lock(this);
    *result = mChromeCookieManager->HasCookies();
    return NOERROR;
}

ECode CookieManagerAdapter::RemoveExpiredCookie()
{
    // ==================before translated======================
    // mChromeCookieManager.removeExpiredCookie();

    mChromeCookieManager->RemoveExpiredCookie();
    return NOERROR;
}

void CookieManagerAdapter::FlushCookieStore()
{
    // ==================before translated======================
    // mChromeCookieManager.flushCookieStore();

    mChromeCookieManager->FlushCookieStore();
}

Boolean CookieManagerAdapter::AllowFileSchemeCookiesImpl()
{
    // ==================before translated======================
    // return mChromeCookieManager.allowFileSchemeCookies();

    return mChromeCookieManager->AllowFileSchemeCookies();
}

void CookieManagerAdapter::SetAcceptFileSchemeCookiesImpl(
    /* [in] */ Boolean accept)
{
    // ==================before translated======================
    // mChromeCookieManager.setAcceptFileSchemeCookies(accept);

    mChromeCookieManager->SetAcceptFileSchemeCookies(accept);
}

String CookieManagerAdapter::FixupUrl(
    /* [in] */ const String& url)
{
    // ==================before translated======================
    // // WebAddress is a private API in the android framework and a "quirk"
    // // of the Classic WebView implementation that allowed embedders to
    // // be relaxed about what URLs they passed into the CookieManager, so we
    // // do the same normalisation before entering the chromium stack.
    // return new WebAddress(url).toString();

    AutoPtr<IWebAddress> webAddress;
    CWebAddress::New(url, (IWebAddress**)&webAddress);
    IObject* objTmp = IObject::Probe(webAddress);
    String result;
    objTmp->ToString(&result);
    return result;
}

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


