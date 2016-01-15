
#include <Elastos.CoreLibrary.Security.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/webkit/native/android_webview/SslUtil.h"
#include "elastos/droid/webkit/native/net/NetError.h"
#include "elastos/droid/webkit/native/net/X509Util.h"
//TODO #include "elastos/droid/net/http/CSslError.h"
//TODO #include "elastos/droid/net/http/CSslCertificate.h"
#include <elastos/utility/logging/Logger.h>

//TODO using Elastos::Droid::Net::Http::CSslError;
//TODO using Elastos::Droid::Net::Http::CSslCertificate;
//using Elastosx::Security::Cert::IX509Certificate;
using Elastos::Droid::Webkit::Net::NetError;
using Elastos::Droid::Webkit::Net::X509Util;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

const String SslUtil::TAG("SslUtil");

/**
 * Creates an SslError object from a chromium net error code.
 */
AutoPtr<ISslError> SslUtil::SslErrorFromNetErrorCode(
    /* [in] */ Int32 error,
    /* [in] */ ISslCertificate* cert,
    /* [in] */ const String& url)
{
    assert (error >= NetError::ERR_CERT_END && error <= NetError::ERR_CERT_COMMON_NAME_INVALID);
    switch(error) {
        case NetError::ERR_CERT_COMMON_NAME_INVALID: {
                AutoPtr<ISslError> sslError;
                //TODO CSslError::New(ISslError::SSL_IDMISMATCH, cert, url, (ISslError**)&sslError);
                return sslError;
            }
        case NetError::ERR_CERT_DATE_INVALID: {
                AutoPtr<ISslError> sslError;
                //TODO CSslError::New(ISslError::SSL_DATE_INVALID, cert, url, (ISslError**)&sslError);
                return sslError;
            }
        case NetError::ERR_CERT_AUTHORITY_INVALID: {
                AutoPtr<ISslError> sslError;
                //TODO CSslError::New(ISslError::SSL_UNTRUSTED, cert, url, (ISslError**)&sslError);
                return sslError;
            }
        default:
            break;
    }

    // Map all other codes to SSL_INVALID.
    AutoPtr<ISslError> sslError;
    //TODO CSslError::New(ISslError::SSL_INVALID, cert, url, (ISslError**)&sslError);
    return sslError;
}

AutoPtr<ISslCertificate> SslUtil::GetCertificateFromDerBytes(
    /* [in] */ ArrayOf<Byte>* derBytes)
{
    if (derBytes == NULL) {
        return NULL;
    }

    // try {
    AutoPtr<IX509Certificate> x509Certificate =
            X509Util::CreateCertificateFromBytes(derBytes);
    AutoPtr<ISslCertificate> sslCertificate;
    //TODO CSslCertificate::New(x509Certificate, (ISslCertificate**)&sslCertificate);
    return sslCertificate;
    // } catch (CertificateException e) {
    //     // A SSL related exception must have occured.  This shouldn't happen.
    //     Log.w(TAG, "Could not read certificate: " + e);
    // } catch (KeyStoreException e) {
    //     // A SSL related exception must have occured.  This shouldn't happen.
    //     Log.w(TAG, "Could not read certificate: " + e);
    // } catch (NoSuchAlgorithmException e) {
    //     // A SSL related exception must have occured.  This shouldn't happen.
    //     Log.w(TAG, "Could not read certificate: " + e);
    // }

    return NULL;
}

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
