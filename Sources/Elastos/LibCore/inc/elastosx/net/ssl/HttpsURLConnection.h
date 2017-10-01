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

#ifndef __ELASTOSX_NET_SSL_HTTPSURLCONNECTION_H__
#define __ELASTOSX_NET_SSL_HTTPSURLCONNECTION_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "elastos/net/HttpURLConnection.h"
#include "elastos/core/Object.h"

using Elastos::Core::Object;
using Elastos::Net::IURL;
using Elastos::Net::IHttpURLConnection;
using Elastos::Net::IURLConnection;
using Elastos::Net::HttpURLConnection;
using Elastos::Security::IPrincipal;
using Elastos::Security::Cert::ICertificate;
using Elastosx::Net::Ssl::ISSLSocketFactory;
using Elastosx::Net::Ssl::IHostnameVerifier;
using Elastosx::Net::Ssl::IHttpsURLConnection;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * An {@link HttpURLConnection} for HTTPS (<a
 * href="http://tools.ietf.org/html/rfc2818">RFC 2818</a>). A
 * connected {@code HttpsURLConnection} allows access to the
 * negotiated cipher suite, the server certificate chain, and the
 * client certificate chain if any.
 *
 * <h3>Providing an application specific X509TrustManager</h3>
 *
 * If an application wants to trust Certificate Authority (CA)
 * certificates that are not part of the system, it should specify its
 * own {@code X509TrustManager} via a {@code SSLSocketFactory} set on
 * the {@code HttpsURLConnection}. The {@code X509TrustManager} can be
 * created based on a {@code KeyStore} using a {@code
 * TrustManagerFactory} to supply trusted CA certificates. Note that
 * self-signed certificates are effectively their own CA and can be
 * trusted by including them in a {@code KeyStore}.
 *
 * <p>For example, to trust a set of certificates specified by a {@code KeyStore}:
 * <pre>   {@code
 *   KeyStore keyStore = ...;
 *   String algorithm = TrustManagerFactory.getDefaultAlgorithm();
 *   TrustManagerFactory tmf = TrustManagerFactory.getInstance(algorithm);
 *   tmf.init(keyStore);
 *
 *   SSLContext context = SSLContext.getInstance("TLS");
 *   context.init(null, tmf.getTrustManagers(), null);
 *
 *   URL url = new URL("https://www.example.com/");
 *   HttpsURLConnection urlConnection = (HttpsURLConnection) url.openConnection();
 *   urlConnection.setSSLSocketFactory(context.getSocketFactory());
 *   InputStream in = urlConnection.getInputStream();
 * }</pre>
 *
 * <p>It is possible to implement {@code X509TrustManager} directly
 * instead of using one created by a {@code
 * TrustManagerFactory}. While this is straightforward in the insecure
 * case of allowing all certificate chains to pass verification,
 * writing a proper implementation will usually want to take advantage
 * of {@link java.security.cert.CertPathValidator
 * CertPathValidator}. In general, it might be better to write a
 * custom {@code KeyStore} implementation to pass to the {@code
 * TrustManagerFactory} than to try and write a custom {@code
 * X509TrustManager}.
 *
 * <h3>Providing an application specific X509KeyManager</h3>
 *
 * A custom {@code X509KeyManager} can be used to supply a client
 * certificate and its associated private key to authenticate a
 * connection to the server. The {@code X509KeyManager} can be created
 * based on a {@code KeyStore} using a {@code KeyManagerFactory}.
 *
 * <p>For example, to supply client certificates from a {@code KeyStore}:
 * <pre>   {@code
 *   KeyStore keyStore = ...;
 *   String algorithm = KeyManagerFactory.getDefaultAlgorithm();
 *   KeyManagerFactory kmf = KeyManagerFactory.getInstance(algorithm);
 *   kmf.init(keyStore);
 *
 *   SSLContext context = SSLContext.getInstance("TLS");
 *   context.init(kmf.getKeyManagers(), null, null);
 *
 *   URL url = new URL("https://www.example.com/");
 *   HttpsURLConnection urlConnection = (HttpsURLConnection) url.openConnection();
 *   urlConnection.setSSLSocketFactory(context.getSocketFactory());
 *   InputStream in = urlConnection.getInputStream();
 * }</pre>
 *
 * <p>A {@code X509KeyManager} can also be implemented directly. This
 * can allow an application to return a certificate and private key
 * from a non-{@code KeyStore} source or to specify its own logic for
 * selecting a specific credential to use when many may be present in
 * a single {@code KeyStore}.
 *
 * <h3>TLS Intolerance Support</h3>
 *
 * This class attempts to create secure connections using common TLS
 * extensions and SSL deflate compression. Should that fail, the
 * connection will be retried with SSLv3 only.
 */
class HttpsURLConnection
    : public HttpURLConnection
    , public IHttpsURLConnection
{
private:
    /*
     * Holds default instances so class preloading doesn't create an instance of
     * it.
     */
    class NoPreloadHolder
    {
    public:
        static AutoPtr<IHostnameVerifier> sDefaultHostnameVerifier;

        static AutoPtr<ISSLSocketFactory> sDefaultSSLSocketFactory;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Sets the default hostname verifier to be used by new instances.
     *
     * @param v
     *            the new default hostname verifier
     * @throws IllegalArgumentException
     *             if the specified verifier is {@code null}.
     */
    static CARAPI SetDefaultHostnameVerifier(
        /* [in] */ IHostnameVerifier* v);

    /**
     * Returns the default hostname verifier.
     *
     * @return the default hostname verifier.
     */
    static CARAPI GetDefaultHostnameVerifier(
        /* [out] */ IHostnameVerifier** verifier);

    /**
     * Sets the default SSL socket factory to be used by new instances.
     *
     * @param sf
     *            the new default SSL socket factory.
     * @throws IllegalArgumentException
     *             if the specified socket factory is {@code null}.
     */
    static CARAPI SetDefaultSSLSocketFactory(
        /* [in] */ ISSLSocketFactory* sf);

    /**
     * Returns the default SSL socket factory for new instances.
     *
     * @return the default SSL socket factory for new instances.
     */
    static CARAPI GetDefaultSSLSocketFactory(
        /* [out] */ ISSLSocketFactory** factory);

    CARAPI constructor(
        /* [in] */ IURL* url);

    /**
     * Returns the {@code Principal} identifying the peer.
     *
     * @return the {@code Principal} identifying the peer.
     * @throws SSLPeerUnverifiedException
     *             if the identity of the peer has not been verified.
     * @throws IllegalStateException
     *             if no connection has been established yet.
     */
    CARAPI GetPeerPrincipal(
        /* [out] */ IPrincipal** principal);

    /**
     * Returns the {@code Principal} used to identify the local host during the handshake.
     *
     * @return the {@code Principal} used to identify the local host during the handshake, or
     *         {@code null} if none was used.
     * @throws IllegalStateException
     *             if no connection has been established yet.
     */
    CARAPI GetLocalPrincipal(
       /* [out] */ IPrincipal** principal);

    /**
     * Sets the hostname verifier for this instance.
     *
     * @param v
     *            the hostname verifier for this instance.
     * @throws IllegalArgumentException
     *             if the specified verifier is {@code null}.
     */
    CARAPI SetHostnameVerifier(
        /* [in] */ IHostnameVerifier* v);

    /**
     * Returns the hostname verifier used by this instance.
     *
     * @return the hostname verifier used by this instance.
     */
    CARAPI GetHostnameVerifier(
        /* [out] */ IHostnameVerifier** verifier);

    /**
     * Sets the SSL socket factory for this instance.
     *
     * @param sf
     *            the SSL socket factory to be used by this instance.
     * @throws IllegalArgumentException
     *             if the specified socket factory is {@code null}.
     */
    CARAPI SetSSLSocketFactory(
        /* [in] */ ISSLSocketFactory* sf);

    /**
     * Returns the SSL socket factory used by this instance.
     *
     * @return the SSL socket factory used by this instance.
     */
    CARAPI GetSSLSocketFactory(
        /* [out] */ ISSLSocketFactory** factory);

protected:
    /**
     * The host name verifier used by this connection. It is initialized from
     * the default hostname verifier
     * {@link #setDefaultHostnameVerifier(HostnameVerifier)} or
     * {@link #getDefaultHostnameVerifier()}.
     */
    AutoPtr<IHostnameVerifier> mHostnameVerifier;

private:
    AutoPtr<ISSLSocketFactory> mSslSocketFactory;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SSL_HTTPSURLCONNECTION_H__
