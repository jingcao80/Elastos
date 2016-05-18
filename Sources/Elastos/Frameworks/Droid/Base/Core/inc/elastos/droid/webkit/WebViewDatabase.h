
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEWDATABASE_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEWDATABASE_H__

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * This class allows developers to determine whether any WebView used in the
 * application has stored any of the following types of browsing data and
 * to clear any such stored data for all WebViews in the application.
 * <ul>
 *  <li>Username/password pairs for web forms</li>
 *  <li>HTTP authentication username/password pairs</li>
 *  <li>Data entered into text fields (e.g. for autocomplete suggestions)</li>
 * </ul>
 */
class WebViewDatabase
    : public Object
    , public IWebViewDatabase
{
public:
    static CARAPI_(AutoPtr<IWebViewDatabase>) GetInstance(
        /* [in] */ IContext* context);

    CAR_INTERFACE_DECL()

    /**
     * Gets whether there are any saved username/password pairs for web forms.
     * Note that these are unrelated to HTTP authentication credentials.
     *
     * @return true if there are any saved username/password pairs
     * @see WebView#savePassword
     * @see clearUsernamePassword
     */
    CARAPI HasUsernamePassword(
        /* [out] */ Boolean* result);

    /**
     * Clears any saved username/password pairs for web forms.
     * Note that these are unrelated to HTTP authentication credentials.
     *
     * @see WebView#savePassword
     * @see hasUsernamePassword
     */
    CARAPI ClearUsernamePassword();

    /**
     * Gets whether there are any saved credentials for HTTP authentication.
     *
     * @return whether there are any saved credentials
     * @see Webview#getHttpAuthUsernamePassword
     * @see Webview#setHttpAuthUsernamePassword
     * @see clearHttpAuthUsernamePassword
     */
    CARAPI HasHttpAuthUsernamePassword(
        /* [out] */ Boolean* result);

    /**
     * Clears any saved credentials for HTTP authentication.
     *
     * @see Webview#getHttpAuthUsernamePassword
     * @see Webview#setHttpAuthUsernamePassword
     * @see hasHttpAuthUsernamePassword
     */
    CARAPI ClearHttpAuthUsernamePassword();

    /**
     * Gets whether there is any saved data for web forms.
     *
     * @return whether there is any saved data for web forms
     * @see clearFormData
     */
    CARAPI HasFormData(
        /* [out] */ Boolean* result);

    /**
     * Clears any saved data for web forms.
     *
     * @see hasFormData
     */
    CARAPI ClearFormData();

    CARAPI ToString(
        /* [out] */ String* info);

protected:
    /**
     * @hide Only for use by WebViewProvider implementations.
     */
    WebViewDatabase();

    /**
     * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
     */
    static const String LOGTAG;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_WEBVIEWDATABASE_H__
