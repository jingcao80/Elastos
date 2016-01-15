
#ifndef __ELASTOS_DROID_CHROMIUM_CHROMIUM_COOKIEMANAGERADAPTER_H__
#define __ELASTOS_DROID_CHROMIUM_CHROMIUM_COOKIEMANAGERADAPTER_H__

// import android.net.ParseException;
// import android.net.WebAddress;
// import android.util.Log;
// import android.webkit.CookieManager;
// import android.webkit.ValueCallback;
// import android.webkit.WebView;

// import org.chromium.android_webview.AwCookieManager;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

class CookieManagerAdapter
    : public CookieManager
{
public:
    CookieManagerAdapter(
        /* [in] */ AwCookieManager* chromeCookieManager);

    //@Override
    //synchronized
    CARAPI_(void) SetAcceptCookie(
        /* [in] */ Boolean accept);

    //@Override
    //synchronized
    CARAPI_(Boolean) AcceptCookie();

    //@Override
    //synchronized
    CARAPI_(void) SetAcceptThirdPartyCookies(
        /* [in] */ WebView* webView,
        /* [in] */ Boolean accept);

    //@Override
    //synchronized
    CARAPI_(Boolean) AcceptThirdPartyCookies(
        /* [in] */ WebView* webView);

    //@Override
    CARAPI_(void) SetCookie(
        /* [in] */ const String& url,
        /* [in] */ const String& value);

    //@Override
    CARAPI_(void) SetCookie(
        /* [in] */ const String& url,
        /* [in] */ const String& value,
        /* [in] */ IValueCallback* callback);

    //@Override
    CARAPI_(String) GetCookie(
        /* [in] */ const String& url);

    //@Override
    CARAPI_(String) GetCookie(
        /* [in] */ const String& url,
        /* [in] */ Boolean privateBrowsing);

    //@Override
    //synchronized
    CARAPI_(String) GetCookie(
        /* [in] */ WebAddress* uri);

    //@Override
    CARAPI_(void) RemoveSessionCookie();

    //@Override
    CARAPI_(void) RemoveSessionCookies(
        /* [in] */ IValueCallback* callback);

    //@Override
    CARAPI_(void) RemoveAllCookie();

    //@Override
    CARAPI_(void) RemoveAllCookies(
        /* [in] */ IValueCallback* callback);

    //@Override
    //synchronized
    CARAPI_(Boolean) HasCookies();

    //@Override
    //synchronized
    CARAPI_(Boolean) HasCookies(
        /* [in] */ Boolean privateBrowsing);

    //@Override
    CARAPI_(void) RemoveExpiredCookie();

public:
    AutoPtr<AwCookieManager> mChromeCookieManager;

protected:
    //@Override
    CARAPI_(void) FlushCookieStore();

    //@Override
    CARAPI_(Boolean) AllowFileSchemeCookiesImpl();

    //@Override
    CARAPI_(void) SetAcceptFileSchemeCookiesImpl(
        /* [in] */ Boolean accept);

private:
    static CARAPI_(String) FixupUrl(
        /* [in] */ const String& url);

private:
    static const String LOGTAG;
};

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_CHROMIUM_CHROMIUM_COOKIEMANAGERADAPTER_H__
