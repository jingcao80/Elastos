
#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwBrowserContext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

const String AwBrowserContext::HTTP_AUTH_DATABASE_FILE("http_auth.db");

AwBrowserContext::AwBrowserContext(
    /* [in] */ ISharedPreferences* sharedPreferences)
    : mSharedPreferences(sharedPreferences)
{
}

AutoPtr<AwGeolocationPermissions> AwBrowserContext::GetGeolocationPermissions()
{
    if (mGeolocationPermissions == NULL) {
        mGeolocationPermissions = new AwGeolocationPermissions(mSharedPreferences);
    }
    return mGeolocationPermissions;
}

AutoPtr<AwCookieManager> AwBrowserContext::GetCookieManager()
{
    if (mCookieManager == NULL) {
        mCookieManager = new AwCookieManager();
    }
    return mCookieManager;
}

AutoPtr<AwFormDatabase> AwBrowserContext::GetFormDatabase() {
    if (mFormDatabase == NULL) {
        mFormDatabase = new AwFormDatabase();
    }
    return mFormDatabase;
}

AutoPtr<HttpAuthDatabase> AwBrowserContext::GetHttpAuthDatabase(
    /* [in] */ IContext* context)
{
    if (mHttpAuthDatabase == NULL) {
        mHttpAuthDatabase = new HttpAuthDatabase(context, HTTP_AUTH_DATABASE_FILE);
    }
    return mHttpAuthDatabase;
}

AutoPtr<DefaultElastosKeyStore> AwBrowserContext::GetKeyStore()
{
    if (mLocalKeyStore == NULL) {
        mLocalKeyStore = new DefaultElastosKeyStore();
    }
    return mLocalKeyStore;
}

/**
 * @see android.webkit.WebView#pauseTimers()
 */
void AwBrowserContext::PauseTimers()
{
    ContentViewStatics::SetWebKitSharedTimersSuspended(TRUE);
}

/**
 * @see android.webkit.WebView#resumeTimers()
 */
void AwBrowserContext::ResumeTimers()
{
    ContentViewStatics::SetWebKitSharedTimersSuspended(FALSE);
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
