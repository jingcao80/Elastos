
#include "elastos/droid/webkit/WebViewDatabase.h"
#include "elastos/droid/webkit/WebViewFactory.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
 */
const String WebViewDatabase::LOGTAG("webviewdatabase");

/**
 * @hide Only for use by WebViewProvider implementations.
 */
WebViewDatabase::WebViewDatabase()
{
}

CAR_INTERFACE_IMPL(WebViewDatabase, Object, IWebViewDatabase);

AutoPtr<IWebViewDatabase> WebViewDatabase::GetInstance(
    /* [in] */ IContext* context)
{
    AutoPtr<IWebViewDatabase> database;
    WebViewFactory::GetProvider()->GetWebViewDatabase(context, (IWebViewDatabase**)&database);
    return database;
}

/**
 * Gets whether there are any saved username/password pairs for web forms.
 * Note that these are unrelated to HTTP authentication credentials.
 *
 * @return true if there are any saved username/password pairs
 * @see WebView#savePassword
 * @see clearUsernamePassword
 */
ECode WebViewDatabase::HasUsernamePassword(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Clears any saved username/password pairs for web forms.
 * Note that these are unrelated to HTTP authentication credentials.
 *
 * @see WebView#savePassword
 * @see hasUsernamePassword
 */
ECode WebViewDatabase::ClearUsernamePassword()
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether there are any saved credentials for HTTP authentication.
 *
 * @return whether there are any saved credentials
 * @see Webview#getHttpAuthUsernamePassword
 * @see Webview#setHttpAuthUsernamePassword
 * @see clearHttpAuthUsernamePassword
 */
ECode WebViewDatabase::HasHttpAuthUsernamePassword(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Clears any saved credentials for HTTP authentication.
 *
 * @see Webview#getHttpAuthUsernamePassword
 * @see Webview#setHttpAuthUsernamePassword
 * @see hasHttpAuthUsernamePassword
 */
ECode WebViewDatabase::ClearHttpAuthUsernamePassword()
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether there is any saved data for web forms.
 *
 * @return whether there is any saved data for web forms
 * @see clearFormData
 */
ECode WebViewDatabase::HasFormData(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Clears any saved data for web forms.
 *
 * @see hasFormData
 */
ECode WebViewDatabase::ClearFormData()
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

ECode WebViewDatabase::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "WebViewDatabase";
    return NOERROR;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos