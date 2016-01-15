
#include "elastos/droid/webkit/HttpAuthHandler.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(HttpAuthHandler, Object, IHttpAuthHandler);

/**
 * @hide Only for use by WebViewProvider implementations.
 */
HttpAuthHandler::HttpAuthHandler()
{
}

/**
 * Gets whether the credentials stored for the current host (i.e. the host
 * for which {@link WebViewClient#onReceivedHttpAuthRequest} was called)
 * are suitable for use. Credentials are not suitable if they have
 * previously been rejected by the server for the current request.
 *
 * @return whether the credentials are suitable for use
 * @see Webview#getHttpAuthUsernamePassword
 */
ECode HttpAuthHandler::UseHttpAuthUsernamePassword(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

/**
 * Instructs the WebView to cancel the authentication request.
 */
ECode HttpAuthHandler::Cancel()
{
    return NOERROR;
}

/**
 * Instructs the WebView to proceed with the authentication with the given
 * credentials. Credentials for use with this method can be retrieved from
 * the WebView's store using {@link WebView#getHttpAuthUsernamePassword}.
 */
ECode HttpAuthHandler::Proceed(
    /* [in] */ const String& username,
    /* [in] */ const String& password)
{
    return NOERROR;
}

/**
 * Gets whether the prompt dialog should be suppressed.
 *
 * @return whether the prompt dialog should be suppressed
 * @hide
 */
ECode HttpAuthHandler::SuppressDialog(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode HttpAuthHandler::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "HttpAuthHandler";
    return NOERROR;
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos
