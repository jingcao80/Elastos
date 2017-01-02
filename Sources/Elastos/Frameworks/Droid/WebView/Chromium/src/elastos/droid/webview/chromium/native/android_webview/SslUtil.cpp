//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <Elastos.CoreLibrary.Security.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/SslUtil.h"
#include "elastos/droid/webkit/webview/chromium/native/net/NetError.h"
#include "elastos/droid/webkit/webview/chromium/native/net/X509Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Net::Http::CSslError;
using Elastos::Droid::Net::Http::CSslCertificate;
using Elastos::Droid::Webkit::Webview::Chromium::Net::NetError;
using Elastos::Droid::Webkit::Webview::Chromium::Net::X509Util;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
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
                CSslError::New(ISslError::SSL_IDMISMATCH, cert, url, (ISslError**)&sslError);
                return sslError;
            }
        case NetError::ERR_CERT_DATE_INVALID: {
                AutoPtr<ISslError> sslError;
                CSslError::New(ISslError::SSL_DATE_INVALID, cert, url, (ISslError**)&sslError);
                return sslError;
            }
        case NetError::ERR_CERT_AUTHORITY_INVALID: {
                AutoPtr<ISslError> sslError;
                CSslError::New(ISslError::SSL_UNTRUSTED, cert, url, (ISslError**)&sslError);
                return sslError;
            }
        default:
            break;
    }

    // Map all other codes to SSL_INVALID.
    AutoPtr<ISslError> sslError;
    CSslError::New(ISslError::SSL_INVALID, cert, url, (ISslError**)&sslError);
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
    CSslCertificate::New(x509Certificate, (ISslCertificate**)&sslCertificate);
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
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
