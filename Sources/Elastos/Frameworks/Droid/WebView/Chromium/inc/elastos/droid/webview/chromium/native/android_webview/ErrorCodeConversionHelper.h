#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_ERRORCODECONVERSIONHELPER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_ERRORCODECONVERSIONHELPER_H__
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * This is a helper class to map native error code about loading a page to Android specific ones.
 */
class ErrorCodeConversionHelper
{
public:
    // Success
    static const Int32 ERROR_OK = 0;
    // Generic error
    static const Int32 ERROR_UNKNOWN = -1;
    // Server or proxy hostname lookup failed
    static const Int32 ERROR_HOST_LOOKUP = -2;
    // Unsupported authentication scheme (not basic or digest)
    static const Int32 ERROR_UNSUPPORTED_AUTH_SCHEME = -3;
    // User authentication failed on server
    static const Int32 ERROR_AUTHENTICATION = -4;
    // User authentication failed on proxy
    static const Int32 ERROR_PROXY_AUTHENTICATION = -5;
    // Failed to connect to the server
    static const Int32 ERROR_CONNECT = -6;
    // Failed to read or write to the server
    static const Int32 ERROR_IO = -7;
    // Connection timed out
    static const Int32 ERROR_TIMEOUT = -8;
    // Too many redirects
    static const Int32 ERROR_REDIRECT_LOOP = -9;
    // Unsupported URI scheme
    static const Int32 ERROR_UNSUPPORTED_SCHEME = -10;
    // Failed to perform SSL handshake
    static const Int32 ERROR_FAILED_SSL_HANDSHAKE = -11;
    // Malformed URL
    static const Int32 ERROR_BAD_URL = -12;
    // Generic file error
    static const Int32 ERROR_FILE = -13;
    // File not found
    static const Int32 ERROR_FILE_NOT_FOUND = -14;
    // Too many requests during this load
    static const Int32 ERROR_TOO_MANY_REQUESTS = -15;
public:
    static CARAPI_(Int32) ConvertErrorCode(
        /* [in] */ Int32 netError);
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_ERRORCODECONVERSIONHELPER_H__
