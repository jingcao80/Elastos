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

#ifndef __ELASTOS_DROID_NET_HTTP_CERTIFICATECHAINVALIDATOR_H__
#define __ELASTOS_DROID_NET_HTTP_CERTIFICATECHAINVALIDATOR_H__

#include <Elastos.CoreLibrary.Extensions.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::IArrayOf;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::Cert::ICertificate;

using Elastosx::Net::Ssl::IHostnameVerifier;
using Elastosx::Net::Ssl::ISSLSocket;
using Elastosx::Net::Ssl::IX509TrustManager;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * Class responsible for all server certificate validation functionality
 *
 * {@hide}
 */
class CertificateChainValidator
    : public Object
    , public ICertificateChainValidator
{
private:
    class NoPreloadHolder
        : public Object
    {
    private:
        static CARAPI_(AutoPtr<ICertificateChainValidator>) InitInstance();

        static CARAPI_(AutoPtr<IHostnameVerifier>) InitVerifier();

    private:
        /**
         * The singleton instance of the certificate chain validator.
         */
        static const AutoPtr<ICertificateChainValidator> sInstance;

        /**
         * The singleton instance of the hostname verifier.
         */
        static const AutoPtr<IHostnameVerifier> sVerifier;

        friend class CertificateChainValidator;
    };

public:
    CAR_INTERFACE_DECL()

    CertificateChainValidator();

    /**
     * @return The singleton instance of the certificates chain validator
     */
    static CARAPI GetInstance(
        /* [out] */ ICertificateChainValidator** result);

    /**
     * Performs the handshake and server certificates validation
     * Notice a new chain will be rebuilt by tracing the issuer and subject
     * before calling checkServerTrusted().
     * And if the last traced certificate is self issued and it is expired, it
     * will be dropped.
     * @param sslSocket The secure connection socket
     * @param domain The website domain
     * @return An SSL error object if there is an error and null otherwise
     */
    CARAPI DoHandshakeAndValidateServerCertificates(
        /* [in] */ IHttpsConnection* connection,
        /* [in] */ ISSLSocket* sslSocket,
        /* [in] */ const String& domain,
        /* [out] */ ISslError** result);

    /**
     * Similar to doHandshakeAndValidateServerCertificates but exposed to JNI for use
     * by Chromium HTTPS stack to validate the cert chain.
     * @param certChain The bytes for certificates in ASN.1 DER encoded certificates format.
     * @param domain The full website hostname and domain
     * @param authType The authentication type for the cert chain
     * @return An SSL error object if there is an error and null otherwise
     */
    static CARAPI VerifyServerCertificates(
        /* [in] */ ArrayOf<IArrayOf*>* certChain,
        /* [in] */ const String& domain,
        /* [in] */ const String& authType,
        /* [out] */ ISslError** result);

    /**
     * Handles updates to credential storage.
     */
    static CARAPI HandleTrustStorageUpdate();

private:
    /**
     * Creates a new certificate chain validator. This is a private constructor.
     * If you need a Certificate chain validator, call getInstance().
     */
    CARAPI constructor();

    /**
     * Common code of doHandshakeAndValidateServerCertificates and verifyServerCertificates.
     * Calls DomainNamevalidator to verify the domain, and TrustManager to verify the certs.
     * @param chain the cert chain in X509 cert format.
     * @param domain The full website hostname and domain
     * @param authType The authentication type for the cert chain
     * @return An SSL error object if there is an error and null otherwise
     */
    static CARAPI VerifyServerDomainAndCertificates(
        /* [in] */ ArrayOf<IX509Certificate*>* chain,
        /* [in] */ const String& domain,
        /* [in] */ const String& authType,
        /* [out] */ ISslError** result);

    /**
     * Returns the platform default {@link X509TrustManager}.
     */
    CARAPI GetTrustManager(
        /* [out] */ IX509TrustManager** result);

    CARAPI CloseSocketThrowException(
        /* [in] */ ISSLSocket* socket,
        /* [in] */ const String& errorMessage,
        /* [in] */ const String& defaultErrorMessage);

    CARAPI CloseSocketThrowException(
        /* [in] */ ISSLSocket* socket,
        /* [in] */ const String& errorMessage);

private:
    static const String TAG;

    AutoPtr<IX509TrustManager> mTrustManager;
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_CERTIFICATECHAINVALIDATOR_H__
