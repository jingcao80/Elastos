
#ifndef __ELASTOS_DROID_WEBKIT_SSLERRORHANDLER_H__
#define __ELASTOS_DROID_WEBKIT_SSLERRORHANDLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Os::Handler;

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * SslErrorHandler: class responsible for handling SSL errors.
 * This class is passed as a parameter to BrowserCallback.displaySslErrorDialog
 * and is meant to receive the user's response.
 */
class SslErrorHandler : public Handler
{
public:
    /**
     * @hide Only for use by WebViewProvider implementations.
     */
    SslErrorHandler();

    /**
     * Proceed with the SSL certificate.
     */
    virtual CARAPI Proceed();

    /**
     * Cancel this request and all pending requests for the WebView that had
     * the error.
     */
    virtual CARAPI Cancel();

    CARAPI ToString(
        /* [out] */ String* info);
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_SSLERRORHANDLER_H__
