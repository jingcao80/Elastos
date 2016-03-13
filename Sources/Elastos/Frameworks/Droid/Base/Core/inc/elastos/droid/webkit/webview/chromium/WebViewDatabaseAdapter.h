
#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBVIEWDATABASEADAPTER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBVIEWDATABASEADAPTER_H_

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwFormDatabase.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/HttpAuthDatabase.h"

using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::AwFormDatabase;
using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::HttpAuthDatabase;
using Elastos::Droid::Webkit::IWebViewDatabase;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

/**
  * Chromium implementation of WebViewDatabase -- forwards calls to the
  * chromium internal implementation.
  */
class WebViewDatabaseAdapter
    : public Object
    , public IWebViewDatabase
{
public:
    CAR_INTERFACE_DECL()

    WebViewDatabaseAdapter(
        /* [in] */ AwFormDatabase* formDatabase,
        /* [in] */ HttpAuthDatabase* httpAuthDatabase);

    // @Override
    CARAPI HasUsernamePassword(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ClearUsernamePassword();

    // @Override
    CARAPI HasHttpAuthUsernamePassword(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ClearHttpAuthUsernamePassword();

    // @Override
    CARAPI HasFormData(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ClearFormData();

private:
    AutoPtr<AwFormDatabase> mFormDatabase;
    AutoPtr<HttpAuthDatabase> mHttpAuthDatabase;
};

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBVIEWDATABASEADAPTER_H_

