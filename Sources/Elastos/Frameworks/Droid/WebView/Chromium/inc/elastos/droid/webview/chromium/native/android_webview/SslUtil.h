#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_SSLUTIL_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_SSLUTIL_H__
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Net::Http::ISslCertificate;
using Elastos::Droid::Net::Http::ISslError;

// import java.security.KeyStoreException;
// import java.security.NoSuchAlgorithmException;
// import java.security.cert.CertificateException;
// import java.security.cert.X509Certificate;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

class SslUtil
{
public:
    /**
     * Creates an SslError object from a chromium net error code.
     */
    static CARAPI_(AutoPtr<ISslError>) SslErrorFromNetErrorCode(
        /* [in] */ Int32 error,
        /* [in] */ ISslCertificate* cert,
        /* [in] */ const String& url);

    static CARAPI_(AutoPtr<ISslCertificate>) GetCertificateFromDerBytes(
        /* [in] */ ArrayOf<Byte>* derBytes);

private:
    static const String TAG;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_SSLUTIL_H__
