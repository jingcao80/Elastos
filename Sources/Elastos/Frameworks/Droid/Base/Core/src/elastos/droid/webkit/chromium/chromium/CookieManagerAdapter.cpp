
namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

// import android.net.ParseException;
// import android.net.WebAddress;
// import android.util.Log;
// import android.webkit.CookieManager;
// import android.webkit.ValueCallback;
// import android.webkit.WebView;

// import org.chromium.android_webview.AwCookieManager;

const String CookieManagerAdapter::LOGTAG("CookieManager");

CookieManagerAdapter::CookieManagerAdapter(
    /* [in] */ AwCookieManager* chromeCookieManager)
    : mChromeCookieManager(chromeCookieManager)
{
}

//@Override
//synchronized
void CookieManagerAdapter::SetAcceptCookie(
    /* [in] */ Boolean accept)
{
    mChromeCookieManager->SetAcceptCookie(accept);
}

//@Override
//synchronized
Boolean CookieManagerAdapter::AcceptCookie()
{
    return mChromeCookieManager->AcceptCookie();
}

//@Override
//synchronized
void CookieManagerAdapter::SetAcceptThirdPartyCookies(
    /* [in] */ WebView* webView,
    /* [in] */ Boolean accept)
{
    webView->GetSettings()->SetAcceptThirdPartyCookies(accept);
}

//@Override
//synchronized
Boolean CookieManagerAdapter::AcceptThirdPartyCookies(
    /* [in] */ WebView* webView)
{
    return webView->GetSettings()->GetAcceptThirdPartyCookies();
}

//@Override
void CookieManagerAdapter::SetCookie(
    /* [in] */ const String& url,
    /* [in] */ const String& value)
{
    // try {
        mChromeCookieManager->SetCookie(FixupUrl(url), value);
    // } catch (ParseException e) {
    //     Log.e(LOGTAG, "Not setting cookie due to error parsing URL: " + url, e);
    // }
}

//@Override
void CookieManagerAdapter::SetCookie(
    /* [in] */ const String& url,
    /* [in] */ const String& value,
    /* [in] */ IValueCallback* callback)
{
    // try {
        mChromeCookieManager->SetCookie(FixupUrl(url), value, callback);
    // } catch (ParseException e) {
    //     Log.e(LOGTAG, "Not setting cookie due to error parsing URL: " + url, e);
    // }
}

//@Override
String CookieManagerAdapter::GetCookie(
    /* [in] */ const String& url)
{
    // try {
        return mChromeCookieManager->GetCookie(FixupUrl(url));
    // } catch (ParseException e) {
    //     Log.e(LOGTAG, "Unable to get cookies due to error parsing URL: " + url, e);
    //     return null;
    // }
}

//@Override
String CookieManagerAdapter::GetCookie(
    /* [in] */ const String& url,
    /* [in] */ Boolean privateBrowsing)
{
    return GetCookie(url);
}

//@Override
//synchronized
String CookieManagerAdapter::GetCookie(
    /* [in] */ WebAddress* uri)
{
    return mChromeCookieManager->GetCookie(uri->ToString());
}

//@Override
void CookieManagerAdapter::RemoveSessionCookie()
{
    mChromeCookieManager->RemoveSessionCookies();
}

//@Override
void CookieManagerAdapter::RemoveSessionCookies(
    /* [in] */ IValueCallback* callback)
{
    mChromeCookieManager->RemoveSessionCookies(callback);
}

//@Override
void CookieManagerAdapter::RemoveAllCookie()
{
    mChromeCookieManager->RemoveAllCookies();
}

//@Override
void CookieManagerAdapter::RemoveAllCookies(
    /* [in] */ IValueCallback* callback)
{
    mChromeCookieManager->RemoveAllCookies(callback);
}

//@Override
//synchronized
Boolean CookieManagerAdapter::HasCookies()
{
    return mChromeCookieManager->HasCookies();
}

//@Override
//synchronized
Boolean CookieManagerAdapter::HasCookies(
    /* [in] */ Boolean privateBrowsing)
{
    return mChromeCookieManager->HasCookies();
}

//@Override
void CookieManagerAdapter::RemoveExpiredCookie()
{
    mChromeCookieManager->RemoveExpiredCookie();
}

//@Override
void CookieManagerAdapter::FlushCookieStore()
{
    mChromeCookieManager->FlushCookieStore();
}

//@Override
Boolean CookieManagerAdapter::AllowFileSchemeCookiesImpl()
{
    return mChromeCookieManager->AllowFileSchemeCookies();
}

//@Override
void CookieManagerAdapter::SetAcceptFileSchemeCookiesImpl(
    /* [in] */ Boolean accept)
{
    mChromeCookieManager->SetAcceptFileSchemeCookies(accept);
}

String CookieManagerAdapter::FixupUrl(
    /* [in] */ const String& url)
{
    // WebAddress is a private API in the android framework and a "quirk"
    // of the Classic WebView implementation that allowed embedders to
    // be relaxed about what URLs they passed into the CookieManager, so we
    // do the same normalisation before entering the chromium stack.
    WebAddress webAddress(url);
    return webAddress.ToString();
}

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
