
#include "org/apache/http/conn/ssl/CSSLSocketFactory.h"
#include "org/apache/http/conn/ssl/CAllowAllHostnameVerifier.h"
#include "org/apache/http/conn/ssl/CBrowserCompatHostnameVerifier.h"
#include "org/apache/http/conn/ssl/CStrictHostnameVerifier.h"
#include "org/apache/http/params/HttpConnectionParams.h"
#include "elastos/net/CInetSocketAddress.h"
#include "elastos/utility/logging/Logger.h"
#include "elastosx/net/ssl/CTrustManagerFactoryHelper.h"
#include "elastosx/net/ssl/CKeyManagerFactoryHelper.h"
#include "elastosx/net/ssl/CSSLContextHelper.h"
#include "elastosx/net/ssl/HttpsURLConnection.h"

using Elastos::Net::CInetSocketAddress;
using Elastos::Net::ISocketAddress;
using Elastos::Utility::Logging::Logger;
using Elastosx::Net::Ssl::IKeyManagerFactory;
using Elastosx::Net::Ssl::ITrustManagerFactoryHelper;
using Elastosx::Net::Ssl::CTrustManagerFactoryHelper;
using Elastosx::Net::Ssl::IKeyManagerFactory;
using Elastosx::Net::Ssl::ITrustManagerFactory;
using Elastosx::Net::Ssl::IKeyManagerFactoryHelper;
using Elastosx::Net::Ssl::CKeyManagerFactoryHelper;
using Elastosx::Net::Ssl::ISSLContextHelper;
using Elastosx::Net::Ssl::CSSLContextHelper;
using Elastosx::Net::Ssl::HttpsURLConnection;
using Org::Apache::Http::Conn::Scheme::EIID_ILayeredSocketFactory;
using Org::Apache::Http::Conn::Scheme::EIID_ISocketFactory;
using Org::Apache::Http::Params::HttpConnectionParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace SSL {

static AutoPtr<IX509HostnameVerifier> InitAllowAllHostnameVerifier()
{
    AutoPtr<CAllowAllHostnameVerifier> verifier;
    CAllowAllHostnameVerifier::NewByFriend((CAllowAllHostnameVerifier**)&verifier);
    return (IX509HostnameVerifier*)verifier.Get();
}
const AutoPtr<IX509HostnameVerifier> CSSLSocketFactory::ALLOW_ALL_HOSTNAME_VERIFIER
        = InitAllowAllHostnameVerifier();

static AutoPtr<IX509HostnameVerifier> InitBrowserCompatHostnameVerifier()
{
    AutoPtr<CBrowserCompatHostnameVerifier> verifier;
    CBrowserCompatHostnameVerifier::NewByFriend((CBrowserCompatHostnameVerifier**)&verifier);
    return (IX509HostnameVerifier*)verifier.Get();
}
const AutoPtr<IX509HostnameVerifier> CSSLSocketFactory::BROWSER_COMPATIBLE_HOSTNAME_VERIFIER
        = InitBrowserCompatHostnameVerifier();

static AutoPtr<IX509HostnameVerifier> InitStrictHostnameVerifier()
{
    AutoPtr<CStrictHostnameVerifier> verifier;
    CStrictHostnameVerifier::NewByFriend((CStrictHostnameVerifier**)&verifier);
    return (IX509HostnameVerifier*)verifier.Get();
}
const AutoPtr<IX509HostnameVerifier> CSSLSocketFactory::STRICT_HOSTNAME_VERIFIER
        = InitStrictHostnameVerifier();

static AutoPtr<ISSLSocketFactory> InitSSLSocketFactory()
{
    AutoPtr<CSSLSocketFactory> factory;
    CSSLSocketFactory::NewByFriend((CSSLSocketFactory**)&factory);
    return (ISSLSocketFactory*)factory.Get();
}
const AutoPtr<ISSLSocketFactory> CSSLSocketFactory::DEFAULT_FACTORY
        = InitSSLSocketFactory();

CSSLSocketFactory::CSSLSocketFactory()
{
    mHostnameVerifier = BROWSER_COMPATIBLE_HOSTNAME_VERIFIER;
}

CAR_INTERFACE_IMPL_3(CSSLSocketFactory, Object, ISSLSocketFactory, ILayeredSocketFactory, ISocketFactory)

CAR_OBJECT_IMPL(CSSLSocketFactory)

AutoPtr<ISSLSocketFactory> CSSLSocketFactory::GetSocketFactory()
{
    return DEFAULT_FACTORY;
}

ECode CSSLSocketFactory::CreateKeyManagers(
    /* [in] */ IKeyStore* keystore,
    /* [in] */ const String& password,
    /* [out] */ ArrayOf<IKeyManager*>** managers)
{
    VALIDATE_NOT_NULL(managers)
    if (keystore == NULL) {
        Logger::E("CSSLSocketFactory", "Keystore may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IKeyManagerFactoryHelper> helper;
    CKeyManagerFactoryHelper::AcquireSingleton((IKeyManagerFactoryHelper**)&helper);
    String algorithm;
    helper->GetDefaultAlgorithm(&algorithm);
    AutoPtr<IKeyManagerFactory> kmfactory;
    helper->GetInstance(algorithm, (IKeyManagerFactory**)&kmfactory);
    kmfactory->Init(keystore, !password.IsNull() ? password.GetChars(): NULL);
    return kmfactory->GetKeyManagers(managers);
}

ECode CSSLSocketFactory::CreateTrustManagers(
    /* [in] */ IKeyStore* keystore,
    /* [out] */ ArrayOf<ITrustManager*>** managers)
{
    VALIDATE_NOT_NULL(managers)
    if (keystore == NULL) {
        Logger::E("CSSLSocketFactory", "Keystore may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ITrustManagerFactoryHelper> helper;
    CTrustManagerFactoryHelper::AcquireSingleton((ITrustManagerFactoryHelper**)&helper);
    String algorithm;
    helper->GetDefaultAlgorithm(&algorithm);
    AutoPtr<ITrustManagerFactory> tmfactory;
    helper->GetInstance(algorithm, (ITrustManagerFactory**)&tmfactory);
    tmfactory->Init(keystore);
    return tmfactory->GetTrustManagers(managers);
}

ECode CSSLSocketFactory::CreateSocket(
    /* [out] */ ISocket** socket)
{
    VALIDATE_NOT_NULL(socket)
    // the cast makes sure that the factory is working as expected
    return ISocketFactory::Probe(mSocketfactory)->CreateSocket(socket);
}

ECode CSSLSocketFactory::ConnectSocket(
    /* [in] */ ISocket* sock,
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ IInetAddress* localAddress,
    /* [in] */ Int32 localPort,
    /* [in] */ IHttpParams* params,
    /* [out] */ ISocket** socket)
{
    VALIDATE_NOT_NULL(socket)
    *socket = NULL;

    if (host.IsNull()) {
        Logger::E("CSSLSocketFactory", "Target host may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        Logger::E("CSSLSocketFactory", "Parameters may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ISocket> _sock;
    if (sock != NULL) {
        _sock = sock;
    }
    else {
        CreateSocket((ISocket**)&_sock);
    }
    AutoPtr<ISSLSocket> sslsock = ISSLSocket::Probe(_sock);

    if ((localAddress != NULL) || (localPort > 0)) {

        // we need to bind explicitly
        if (localPort < 0)
            localPort = 0; // indicates "any"

        AutoPtr<IInetSocketAddress> isa;
        CInetSocketAddress::New(localAddress, localPort, (IInetSocketAddress**)&isa);
        AutoPtr<ISocketAddress> sa = ISocketAddress::Probe(isa);
        ISocket::Probe(sslsock)->Bind(sa);
    }

    Int32 connTimeout, soTimeout;
    HttpConnectionParams::GetConnectionTimeout(params, &connTimeout);
    HttpConnectionParams::GetSoTimeout(params, &soTimeout);

    AutoPtr<IInetSocketAddress> remoteAddress;
    if (mNameResolver != NULL) {
        AutoPtr<IInetAddress> addr;
        mNameResolver->Resolve(host, (IInetAddress**)&addr);
        CInetSocketAddress::New(addr, port, (IInetSocketAddress**)&remoteAddress);
    }
    else {
        CInetSocketAddress::New(host, port, (IInetSocketAddress**)&remoteAddress);
    }

    AutoPtr<ISocketAddress> sa = ISocketAddress::Probe(remoteAddress);
    ISocket::Probe(sslsock)->Connect(sa, connTimeout);

    ISocket::Probe(sslsock)->SetSoTimeout(soTimeout);
    // try {
    ECode ec = mHostnameVerifier->Verify(host, sslsock);
    if (FAILED(ec)) {
        ISocket::Probe(sslsock)->Close();
        return ec;
    }
        // verifyHostName() didn't blowup - good!
    // } catch (IOException iox) {
    //     // close the socket before re-throwing the exception
    //     try { sslsock.close(); } catch (Exception x) { /*ignore*/ }
    //     throw iox;
    // }

    *socket = ISocket::Probe(sslsock);
    REFCOUNT_ADD(*socket)
    return NOERROR;
}

ECode CSSLSocketFactory::IsSecure(
    /* [in] */ ISocket* sock,
    /* [out] */ Boolean* isSecure)
{
    VALIDATE_NOT_NULL(isSecure)
    *isSecure = FALSE;

    if (sock == NULL) {
        Logger::E("CSSLSocketFactory", "Socket may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // This instanceof check is in line with createSocket() above.
    if (ISSLSocket::Probe(sock) == NULL) {
        Logger::E("CSSLSocketFactory", "Socket not created by this factory.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // This check is performed last since it calls the argument object.
    Boolean isClosed;
    if (sock->IsClosed(&isClosed), isClosed) {
        Logger::E("CSSLSocketFactory", "Socket is closed.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *isSecure = TRUE;
    return NOERROR;
}

ECode CSSLSocketFactory::CreateSocket(
    /* [in] */ ISocket* sock,
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ Boolean autoClose,
    /* [out] */ ISocket** socket)
{
    VALIDATE_NOT_NULL(socket)
    AutoPtr<ISocket> _sock;
    mSocketfactory->CreateSocket(sock, host, port, autoClose, (ISocket**)&_sock);
    AutoPtr<ISSLSocket> sslSocket = ISSLSocket::Probe(_sock);
    mHostnameVerifier->Verify(host, sslSocket);
    // verifyHostName() didn't blowup - good!
    *socket = _sock;
    REFCOUNT_ADD(*socket)
    return NOERROR;
}

ECode CSSLSocketFactory::SetHostnameVerifier(
    /* [in] */ IX509HostnameVerifier* hostnameVerifier)
{
    if (hostnameVerifier == NULL) {
        Logger::E("CSSLSocketFactory", "Hostname verifier may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mHostnameVerifier = hostnameVerifier;
    return NOERROR;
}

ECode CSSLSocketFactory::GetHostnameVerifier(
    /* [out] */ IX509HostnameVerifier** hostnameVerifier)
{
    VALIDATE_NOT_NULL(hostnameVerifier)
    *hostnameVerifier = mHostnameVerifier;
    REFCOUNT_ADD(*hostnameVerifier)
    return NOERROR;
}

ECode CSSLSocketFactory::constructor(
    /* [in] */ const String& _algorithm,
    /* [in] */ IKeyStore* keystore,
    /* [in] */ const String& keystorePassword,
    /* [in] */ IKeyStore* truststore,
    /* [in] */ ISecureRandom* random,
    /* [in] */ IHostNameResolver* nameResolver)
{
    String algorithm = _algorithm;
    if (algorithm.IsNull()) {
        algorithm = TLS;
    }
    AutoPtr< ArrayOf<IKeyManager*> > keymanagers;
    if (keystore != NULL) {
        CreateKeyManagers(keystore, keystorePassword, (ArrayOf<IKeyManager*>**)&keymanagers);
    }
    AutoPtr< ArrayOf<ITrustManager*> > trustmanagers;
    if (truststore != NULL) {
        CreateTrustManagers(truststore, (ArrayOf<ITrustManager*>**)&trustmanagers);
    }
    AutoPtr<ISSLContextHelper> helper;
    CSSLContextHelper::AcquireSingleton((ISSLContextHelper**)&helper);
    mSslcontext = NULL;
    helper->GetInstance(algorithm, (ISSLContext**)&mSslcontext);
    mSslcontext->Init(keymanagers, trustmanagers, random);
    mSocketfactory = NULL;
    mSslcontext->GetSocketFactory((Elastosx::Net::Ssl::ISSLSocketFactory**)&mSocketfactory);
    mNameResolver = nameResolver;

    return NOERROR;
}

ECode CSSLSocketFactory::constructor(
    /* [in] */ IKeyStore* keystore,
    /* [in] */ const String& keystorePassword,
    /* [in] */ IKeyStore* truststore)
{
    return constructor(TLS, keystore, keystorePassword, truststore, NULL, NULL);
}

ECode CSSLSocketFactory::constructor(
    /* [in] */ IKeyStore* keystore,
    /* [in] */ const String& keystorePassword)
{
    return constructor(TLS, keystore, keystorePassword, NULL, NULL, NULL);
}

ECode CSSLSocketFactory::constructor(
    /* [in] */ IKeyStore* truststore)
{
    return constructor(TLS, NULL, String(NULL), truststore, NULL, NULL);
}

ECode CSSLSocketFactory::constructor(
    /* [in] */ Elastosx::Net::Ssl::ISSLSocketFactory* socketfactory)
{
    mSslcontext = NULL;
    mSocketfactory = socketfactory;
    mNameResolver = NULL;
    return NOERROR;
}

ECode CSSLSocketFactory::constructor()
{
    mSslcontext = NULL;
    HttpsURLConnection::GetDefaultSSLSocketFactory((Elastosx::Net::Ssl::ISSLSocketFactory**)&mSocketfactory);
    mNameResolver = NULL;
    return NOERROR;
}

} // namespace SSL
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org