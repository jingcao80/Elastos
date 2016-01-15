
#ifndef __ELASTOS_DROID_CHROMIUM_CHROMIUM_WEBVIEWDATABASEADAPTER_H__
#define __ELASTOS_DROID_CHROMIUM_CHROMIUM_WEBVIEWDATABASEADAPTER_H__

// import android.content.Context;
// import android.webkit.WebViewDatabase;

// import org.chromium.android_webview.AwFormDatabase;
// import org.chromium.android_webview.HttpAuthDatabase;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

/**
 * Chromium implementation of WebViewDatabase -- forwards calls to the
 * chromium internal implementation.
 */
class WebViewDatabaseAdapter
    : public IWebViewDatabase
{
public:
    WebViewDatabaseAdapter(AwFormDatabase formDatabase, HttpAuthDatabase httpAuthDatabase) {
        mFormDatabase = formDatabase;
        mHttpAuthDatabase = httpAuthDatabase;
    }

    //@Override
    CARAPI HasUsernamePassword(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI ClearUsernamePassword();

    //@Override
    CARAPI HasHttpAuthUsernamePassword(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI ClearHttpAuthUsernamePassword();

    //@Override
    CARAPI HasFormData(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI ClearFormData();

private:
    AutoPtr<AwFormDatabase> mFormDatabase;
    AutoPtr<HttpAuthDatabase> mHttpAuthDatabase;
};

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_CHROMIUM_CHROMIUM_WEBVIEWDATABASEADAPTER_H__
