
#include "elastos/droid/webkit/SslErrorHandler.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * @hide Only for use by WebViewProvider implementations.
 */
SslErrorHandler::SslErrorHandler()
{
}

/**
 * Proceed with the SSL certificate.
 */
ECode SslErrorHandler::Proceed()
{
    return NOERROR;
}

/**
 * Cancel this request and all pending requests for the WebView that had
 * the error.
 */
ECode SslErrorHandler::Cancel()
{
    return NOERROR;
}

ECode SslErrorHandler::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "SslErrorHandler";
    return NOERROR;
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos