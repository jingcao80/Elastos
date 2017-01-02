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

#ifndef __ORG_APACHE_HTTP_CONN_SSL_CSSLSOCKETFACTORY_H_
#define __ORG_APACHE_HTTP_CONN_SSL_CSSLSOCKETFACTORY_H_

#include "Elastos.CoreLibrary.Extensions.h"
#include "_Org_Apache_Http_Conn_SSL_CSSLSocketFactory.h"
#include "elastos/core/Object.h"

using Elastos::Net::IInetAddress;
using Elastos::Net::ISocket;
using Elastos::Security::IKeyStore;
using Elastos::Security::ISecureRandom;
using Elastosx::Net::Ssl::IKeyManager;
using Elastosx::Net::Ssl::ITrustManager;
using Elastosx::Net::Ssl::ISSLContext;
using Org::Apache::Http::Conn::Scheme::IHostNameResolver;
using Org::Apache::Http::Conn::Scheme::ILayeredSocketFactory;
using Org::Apache::Http::Conn::Scheme::ISocketFactory;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace SSL {

/**
 * Layered socket factory for TLS/SSL connections, based on JSSE.
 *.
 * <p>
 * SSLSocketFactory can be used to validate the identity of the HTTPS
 * server against a list of trusted certificates and to authenticate to
 * the HTTPS server using a private key.
 * </p>
 *
 * <p>
 * SSLSocketFactory will enable server authentication when supplied with
 * a {@link KeyStore truststore} file containg one or several trusted
 * certificates. The client secure socket will reject the connection during
 * the SSL session handshake if the target HTTPS server attempts to
 * authenticate itself with a non-trusted certificate.
 * </p>
 *
 * <p>
 * Use JDK keytool utility to import a trusted certificate and generate a truststore file:
 *    <pre>
 *     keytool -import -alias "my server cert" -file server.crt -keystore my.truststore
 *    </pre>
 * </p>
 *
 * <p>
 * SSLSocketFactory will enable client authentication when supplied with
 * a {@link KeyStore keystore} file containg a private key/public certificate
 * pair. The client secure socket will use the private key to authenticate
 * itself to the target HTTPS server during the SSL session handshake if
 * requested to do so by the server.
 * The target HTTPS server will in its turn verify the certificate presented
 * by the client in order to establish client's authenticity
 * </p>
 *
 * <p>
 * Use the following sequence of actions to generate a keystore file
 * </p>
 *   <ul>
 *     <li>
 *      <p>
 *      Use JDK keytool utility to generate a new key
 *      <pre>keytool -genkey -v -alias "my client key" -validity 365 -keystore my.keystore</pre>
 *      For simplicity use the same password for the key as that of the keystore
 *      </p>
 *     </li>
 *     <li>
 *      <p>
 *      Issue a certificate signing request (CSR)
 *      <pre>keytool -certreq -alias "my client key" -file mycertreq.csr -keystore my.keystore</pre>
 *     </p>
 *     </li>
 *     <li>
 *      <p>
 *      Send the certificate request to the trusted Certificate Authority for signature.
 *      One may choose to act as her own CA and sign the certificate request using a PKI
 *      tool, such as OpenSSL.
 *      </p>
 *     </li>
 *     <li>
 *      <p>
 *       Import the trusted CA root certificate
 *       <pre>keytool -import -alias "my trusted ca" -file caroot.crt -keystore my.keystore</pre>
 *      </p>
 *     </li>
 *     <li>
 *      <p>
 *       Import the PKCS#7 file containg the complete certificate chain
 *       <pre>keytool -import -alias "my client key" -file mycert.p7 -keystore my.keystore</pre>
 *      </p>
 *     </li>
 *     <li>
 *      <p>
 *       Verify the content the resultant keystore file
 *       <pre>keytool -list -v -keystore my.keystore</pre>
 *      </p>
 *     </li>
 *   </ul>
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 * @author Julius Davies
 */
CarClass(CSSLSocketFactory)
    , public Object
    , public ISSLSocketFactory
    , public ILayeredSocketFactory
    , public ISocketFactory
{
public:
    CSSLSocketFactory();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Gets an singleton instance of the SSLProtocolSocketFactory.
     * @return a SSLProtocolSocketFactory
     */
    static CARAPI_(AutoPtr<ISSLSocketFactory>) GetSocketFactory();

    CARAPI CreateSocket(
        /* [out] */ ISocket** socket);

    CARAPI ConnectSocket(
        /* [in] */ ISocket* sock,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ IInetAddress* localAddress,
        /* [in] */ Int32 localPort,
        /* [in] */ IHttpParams* params,
        /* [out] */ ISocket** socket);

    CARAPI IsSecure(
        /* [in] */ ISocket* sock,
        /* [out] */ Boolean* isSecure);

    CARAPI CreateSocket(
        /* [in] */ ISocket* sock,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ Boolean autoClose,
        /* [out] */ ISocket** socket);

    CARAPI SetHostnameVerifier(
        /* [in] */ IX509HostnameVerifier* hostnameVerifier);

    CARAPI GetHostnameVerifier(
        /* [out] */ IX509HostnameVerifier** hostnameVerifier);

    CARAPI constructor(
        /* [in] */ const String& algorithm,
        /* [in] */ IKeyStore* keystore,
        /* [in] */ const String& keystorePassword,
        /* [in] */ IKeyStore* truststore,
        /* [in] */ ISecureRandom* random,
        /* [in] */ IHostNameResolver* nameResolver);

    CARAPI constructor(
        /* [in] */ IKeyStore* keystore,
        /* [in] */ const String& keystorePassword,
        /* [in] */ IKeyStore* truststore);

    CARAPI constructor(
        /* [in] */ IKeyStore* keystore,
        /* [in] */ const String& keystorePassword);

    CARAPI constructor(
        /* [in] */ IKeyStore* truststore);

    CARAPI constructor(
        /* [in] */ Elastosx::Net::Ssl::ISSLSocketFactory* socketfactory);

    CARAPI constructor();

private:
    static CARAPI CreateKeyManagers(
        /* [in] */ IKeyStore* keystore,
        /* [in] */ const String& password,
        /* [out] */ ArrayOf<IKeyManager*>** managers);

    static CARAPI CreateTrustManagers(
        /* [in] */ IKeyStore* keystore,
        /* [out] */ ArrayOf<ITrustManager*>** managers);

public:
    static const AutoPtr<IX509HostnameVerifier> ALLOW_ALL_HOSTNAME_VERIFIER;

    static const AutoPtr<IX509HostnameVerifier> BROWSER_COMPATIBLE_HOSTNAME_VERIFIER;

    static const AutoPtr<IX509HostnameVerifier> STRICT_HOSTNAME_VERIFIER;

    /**
     * The factory using the default JVM settings for secure connections.
     */
    static const AutoPtr<ISSLSocketFactory> DEFAULT_FACTORY;

private:
    AutoPtr<ISSLContext> mSslcontext;
    AutoPtr<Elastosx::Net::Ssl::ISSLSocketFactory> mSocketfactory;
    AutoPtr<IHostNameResolver> mNameResolver;
    AutoPtr<IX509HostnameVerifier> mHostnameVerifier;

};

} // namespace SSL
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_SSL_CSSLSOCKETFACTORY_H_
