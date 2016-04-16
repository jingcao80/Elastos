
#include "elastos/droid/webkit/webview/chromium/WebViewDatabaseAdapter.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

//=====================================================================
//                        WebViewDatabaseAdapter
//=====================================================================
CAR_INTERFACE_IMPL(WebViewDatabaseAdapter, Object, IWebViewDatabase)

WebViewDatabaseAdapter::WebViewDatabaseAdapter(
    /* [in] */ AwFormDatabase* formDatabase,
    /* [in] */ HttpAuthDatabase* httpAuthDatabase)
    : mFormDatabase(formDatabase)
    , mHttpAuthDatabase(httpAuthDatabase)
{
    // ==================before translated======================
    // mFormDatabase = formDatabase;
    // mHttpAuthDatabase = httpAuthDatabase;
}

ECode WebViewDatabaseAdapter::HasUsernamePassword(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // This is a deprecated API: intentional no-op.
    // return false;

    *result = FALSE;
    return NOERROR;
}

ECode WebViewDatabaseAdapter::ClearUsernamePassword()
{
    // ==================before translated======================
    // // This is a deprecated API: intentional no-op.

    return NOERROR;
}

ECode WebViewDatabaseAdapter::HasHttpAuthUsernamePassword(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mHttpAuthDatabase.hasHttpAuthUsernamePassword();

    *result = mHttpAuthDatabase->HasHttpAuthUsernamePassword();
    return NOERROR;
}

ECode WebViewDatabaseAdapter::ClearHttpAuthUsernamePassword()
{
    // ==================before translated======================
    // mHttpAuthDatabase.clearHttpAuthUsernamePassword();

    mHttpAuthDatabase->ClearHttpAuthUsernamePassword();
    return NOERROR;
}

ECode WebViewDatabaseAdapter::HasFormData(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mFormDatabase.hasFormData();

    *result = mFormDatabase->HasFormData();
    return NOERROR;
}

ECode WebViewDatabaseAdapter::ClearFormData()
{
    // ==================before translated======================
    // mFormDatabase.clearFormData();

    mFormDatabase->ClearFormData();
    return NOERROR;
}

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


