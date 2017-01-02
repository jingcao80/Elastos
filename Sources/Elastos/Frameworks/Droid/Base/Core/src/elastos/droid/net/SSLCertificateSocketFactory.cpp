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


#include <Elastos.CoreLibrary.Net.h>
#include "_Org.Conscrypt.h"
#include "elastos/droid/net/SSLCertificateSocketFactory.h"
#include "elastos/droid/net/CSSLCertificateSocketFactory.h"
#include "elastos/droid/net/http/Connection.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include "elastos/droid/net/SSLSessionCache.h"
#include "elastos/droid/os/SystemProperties.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Net::ISSLCertificateSocketFactory;
using Elastos::Droid::Net::ISSLSessionCache;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Utility::ILog;

using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::Net::CURI;
using Elastos::Net::IInetAddress;
using Elastos::Net::ISocket;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::IPrivateKey;
using Elastos::Utility::Logging::Logger;
using Elastosx::Net::ISocketFactory;
using Elastosx::Net::Ssl::CHttpsURLConnectionHelper;
using Elastosx::Net::Ssl::CSSLSocketFactoryHelper;
using Elastosx::Net::Ssl::EIID_ITrustManager;
using Elastosx::Net::Ssl::EIID_IX509TrustManager;
using Elastosx::Net::Ssl::IHostnameVerifier;
using Elastosx::Net::Ssl::IHttpsURLConnection;
using Elastosx::Net::Ssl::IHttpsURLConnectionHelper;
using Elastosx::Net::Ssl::IKeyManager;
using Elastosx::Net::Ssl::ISSLContextSpi;
using Elastosx::Net::Ssl::ISSLSession;
using Elastosx::Net::Ssl::ISSLSessionContext;
using Elastosx::Net::Ssl::ISSLSocket;
using Elastosx::Net::Ssl::ISSLSocketFactory;
using Elastosx::Net::Ssl::ISSLSocketFactoryHelper;
using Elastosx::Net::Ssl::ITrustManager;
using Elastosx::Net::Ssl::IX509TrustManager;

using Org::Conscrypt::COpenSSLContextImpl;
using Org::Conscrypt::IClientSessionContext;
using Org::Conscrypt::IOpenSSLContextImpl;
using Org::Conscrypt::IOpenSSLSocketImpl;
using Org::Conscrypt::ISSLClientSessionCache;

namespace Elastos {
namespace Droid {
namespace Net {

//==========================================================
// SSLCertificateSocketFactory::InnerSub_TrustManager
//==========================================================
CAR_INTERFACE_IMPL_2(SSLCertificateSocketFactory::InnerSub_TrustManager, Object, ITrustManager, IX509TrustManager)

ECode SSLCertificateSocketFactory::InnerSub_TrustManager::GetAcceptedIssuers(
    /* [out, callee] */ ArrayOf<IX509Certificate*>** result)
{
    VALIDATE_NOT_NULL(result)

    *result = NULL;
    return NOERROR;
}

ECode SSLCertificateSocketFactory::InnerSub_TrustManager::CheckClientTrusted(
    /* [in] */ ArrayOf<IX509Certificate*>* certs,
    /* [in] */ const String& authType)
{
    return NOERROR;
}

ECode SSLCertificateSocketFactory::InnerSub_TrustManager::CheckServerTrusted(
    /* [in] */ ArrayOf<IX509Certificate*>* certs,
    /* [in] */ const String& authType)
{
    return NOERROR;
}

//==========================================================
// SSLCertificateSocketFactory
//==========================================================
CAR_INTERFACE_IMPL(SSLCertificateSocketFactory, SSLSocketFactory, ISSLCertificateSocketFactory)

const String SSLCertificateSocketFactory::TAG("SSLCertificateSocketFactory");

SSLCertificateSocketFactory::SSLCertificateSocketFactory()
    : mHandshakeTimeoutMillis(0)
    , mSecure(FALSE)
{}

ECode SSLCertificateSocketFactory::constructor(
    /* [in] */ Int32 handshakeTimeoutMillis)
{
    return constructor(handshakeTimeoutMillis, NULL, TRUE);
}

ECode SSLCertificateSocketFactory::constructor(
    /* [in] */ Int32 handshakeTimeoutMillis,
    /* [in] */ ISSLSessionCache* cache,
    /* [in] */ Boolean secure)
{
    mHandshakeTimeoutMillis = handshakeTimeoutMillis;
    mSessionCache = cache == NULL ? NULL : ((SSLSessionCache*)cache)->mSessionCache;
    mSecure = secure;
    return NOERROR;
}

ECode SSLCertificateSocketFactory::GetDefault(
    /* [in] */ Int32 handshakeTimeoutMillis,
    /* [out] */ ISocketFactory** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ISSLCertificateSocketFactory> sslCertificateSocketFactory = new CSSLCertificateSocketFactory();
    ((SSLCertificateSocketFactory*)sslCertificateSocketFactory.Get())->constructor(handshakeTimeoutMillis, NULL, TRUE);
    *result = ISocketFactory::Probe(sslCertificateSocketFactory);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SSLCertificateSocketFactory::GetDefault(
    /* [in] */ Int32 handshakeTimeoutMillis,
    /* [in] */ ISSLSessionCache* cache,
    /* [out] */ ISSLSocketFactory** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ISSLCertificateSocketFactory> sslCertificateSocketFactory = new CSSLCertificateSocketFactory();
    ((SSLCertificateSocketFactory*)sslCertificateSocketFactory.Get())->constructor(handshakeTimeoutMillis, cache, TRUE);
    *result = ISSLSocketFactory::Probe(sslCertificateSocketFactory);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SSLCertificateSocketFactory::GetInsecure(
    /* [in] */ Int32 handshakeTimeoutMillis,
    /* [in] */ ISSLSessionCache* cache,
    /* [out] */ ISSLSocketFactory** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ISSLCertificateSocketFactory> sslCertificateSocketFactory = new CSSLCertificateSocketFactory();
    ((SSLCertificateSocketFactory*)sslCertificateSocketFactory.Get())->constructor(handshakeTimeoutMillis, cache, FALSE);
    *result = ISSLSocketFactory::Probe(sslCertificateSocketFactory);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SSLCertificateSocketFactory::GetHttpSocketFactory(
    /* [in] */ Int32 handshakeTimeoutMillis,
    /* [in] */ ISSLSessionCache* cache,
    /* [out] */ Org::Apache::Http::Conn::SSL::ISSLSocketFactory** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ISSLCertificateSocketFactory> certSocketFactory;
    CSSLCertificateSocketFactory::New(handshakeTimeoutMillis, (ISSLCertificateSocketFactory**)&certSocketFactory);

    Org::Apache::Http::Conn::SSL::CSSLSocketFactory::New(
            ISSLSocketFactory::Probe(certSocketFactory), result);
    return NOERROR;
}

ECode SSLCertificateSocketFactory::VerifyHostname(
    /* [in] */ ISocket* socket,
    /* [in] */ const String& hostname)
{
    if (ISSLSocket::Probe(socket) == NULL) {
        Logger::E(TAG, "Attempt to verify non-SSL socket");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (!CSSLCertificateSocketFactory::IsSslCheckRelaxed()) {
        // The code at the start of OpenSSLSocketImpl.startHandshake()
        // ensures that the call is idempotent, so we can safely call it.
        AutoPtr<ISSLSocket> ssl = ISSLSocket::Probe(socket);
        ssl->StartHandshake();

        AutoPtr<ISSLSession> session;
        ssl->GetSession((ISSLSession**)&session);
        if (session == NULL) {
            Logger::E(TAG, "Cannot verify SSL socket without session");
            return E_FAIL;
        }
        AutoPtr<IHttpsURLConnectionHelper> helper;
        CHttpsURLConnectionHelper::AcquireSingleton((IHttpsURLConnectionHelper**)&helper);
        AutoPtr<IHostnameVerifier> verifier;
        helper->GetDefaultHostnameVerifier((IHostnameVerifier**)&verifier);
        Boolean result;
        verifier->Verify(hostname, session, &result);
        if (!result) {
            Logger::E(TAG, "Cannot verify hostname: %s", hostname.string());
            return E_FAIL;
        }
    }
    return NOERROR;
}

ECode SSLCertificateSocketFactory::MakeSocketFactory(
    /* [in] */ ArrayOf<IKeyManager*>* keyManagers,
    /* [in] */ ArrayOf<ITrustManager*>* trustManagers,
    /* [out] */ ISSLSocketFactory** result)
{
    VALIDATE_NOT_NULL(result)

    //try {
    AutoPtr<ISocketFactory> rev;
    AutoPtr<IOpenSSLContextImpl> sslContext;
    ECode ec;
    COpenSSLContextImpl::New((IOpenSSLContextImpl**)&sslContext);
    ec = ISSLContextSpi::Probe(sslContext)->EngineInit(keyManagers, trustManagers, NULL);
    if (FAILED(ec)) {
        if (ec == (ECode)E_KEY_MANAGEMENT_EXCEPTION) {
            Logger::W(TAG, "%d", ec);
            AutoPtr<ISSLSocketFactoryHelper> helper;
            CSSLSocketFactoryHelper::AcquireSingleton((ISSLSocketFactoryHelper**)&helper);
            helper->GetDefault((ISocketFactory**)&rev);
            FUNC_RETURN(ISSLSocketFactory::Probe(rev))
        }
        return ec;
    }

    AutoPtr<IClientSessionContext> sessionContext;
    AutoPtr<ISSLSessionContext> ctx;
    ISSLContextSpi::Probe(sslContext)->EngineGetClientSessionContext((ISSLSessionContext**)&ctx);
    sessionContext = IClientSessionContext::Probe(ctx);
    sessionContext->SetPersistentCache(mSessionCache);

    AutoPtr<ISSLSocketFactory> factory;
    ISSLContextSpi::Probe(sslContext)->EngineGetSocketFactory((ISSLSocketFactory**)&factory);
    rev = ISocketFactory::Probe(factory);

    FUNC_RETURN(ISSLSocketFactory::Probe(rev))
    //} catch (KeyManagementException e) {
    //    Log.wtf(TAG, e);
    //    return (SSLSocketFactory) SSLSocketFactory.getDefault();  // Fallback
    //}
}

Boolean SSLCertificateSocketFactory::IsSslCheckRelaxed()
{
    String s1, s2;
    SystemProperties::Get(String("ro.debuggable"), &s1);
    SystemProperties::Get(String("socket.relaxsslcheck"), &s2);
    return s1.Equals("1") && s2.Equals("yes");
}

AutoPtr<ISSLSocketFactory> SSLCertificateSocketFactory::GetDelegate()
{
    {    AutoLock syncLock(this);
        // Relax the SSL check if instructed (for this factory, or systemwide)
        if (!mSecure || IsSslCheckRelaxed()) {
            if (mInsecureFactory == NULL) {
                if (mSecure) {
                    //Log.w(TAG, "*** BYPASSING SSL SECURITY CHECKS (socket.relaxsslcheck=yes) ***");
                } else {
                    //Log.w(TAG, "Bypassing SSL security checks at caller's request");
                }
                MakeSocketFactory(mKeyManagers, SSLCertificateSocketFactory::INSECURE_TRUST_MANAGER, (ISSLSocketFactory**)&mInsecureFactory);
            }
            return mInsecureFactory;
        } else {
            if (mSecureFactory == NULL) {
                MakeSocketFactory(mKeyManagers, mTrustManagers, (ISSLSocketFactory**)&mSecureFactory);
            }
            return mSecureFactory;
        }
    }
    return NULL;
}

ECode SSLCertificateSocketFactory::SetTrustManagers(
    /* [in] */ ArrayOf<ITrustManager*>* trustManager)
{
    mTrustManagers = trustManager;

    // Clear out all cached secure factories since configurations have changed.
    mSecureFactory = NULL;
    // Note - insecure factories only ever use the INSECURE_TRUST_MANAGER so they need not
    // be cleared out here.
    return NOERROR;
}

ECode SSLCertificateSocketFactory::SetNpnProtocols(
    /* [in] */ ArrayOf<IArrayOf*>* npnProtocols)
{
    ToLengthPrefixedList(npnProtocols, (ArrayOf<Byte>**)&mNpnProtocols);
    if(mNpnProtocols == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode SSLCertificateSocketFactory::SetAlpnProtocols(
    /* [in] */ ArrayOf<IArrayOf*>* protocols)
{
    return ToLengthPrefixedList(protocols, (ArrayOf<Byte>**)&mAlpnProtocols);
}

ECode SSLCertificateSocketFactory::ToLengthPrefixedList(
    /* [in] */ ArrayOf<IArrayOf*>* items,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<ArrayOf<Byte> > rev;
    Int32 length = items->GetLength();
    if (length == 0) {
        Logger::E(TAG, "items.length == 0");
        *result = NULL;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 totalLength = 0;
    Int32 i;
    for (i = 0; i < items->GetLength(); ++i) {
        AutoPtr<IArrayOf> s = (*items)[i];
        Int32 length2;
        s->GetLength(&length2);
        if (length2 == 0 || length2 > 255) {
            Logger::E(TAG, "s.length == 0 || s.length > 255: %d", length2);
            *result = NULL;
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        totalLength += 1 + length2;
    }

    rev = ArrayOf<Byte>::Alloc(totalLength);
    Int32 pos = 0;
    for (i = 0; i < items->GetLength(); ++i) {
        AutoPtr<IArrayOf> s = (*items)[i];
        Int32 length3;
        s->GetLength(&length3);
        (*rev)[pos++] = (Byte)length3;
        for (Int32 j = 0; j < Ptr(s)->Func(s->GetLength); ++j) {
            AutoPtr<IByte> byte;
            AutoPtr<IInterface> obj;
            s->Get(j, (IInterface**)&obj);
            byte = IByte::Probe(obj);
            (*rev)[pos++] = Ptr(byte)->Func(byte->GetValue);
        }
    }
    FUNC_RETURN(rev)
}

ECode SSLCertificateSocketFactory::GetNpnSelectedProtocol(
    /* [in] */ ISocket* socket,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IOpenSSLSocketImpl> openSslSocketImpl;
    CastToOpenSSLSocket(socket, (IOpenSSLSocketImpl**)&openSslSocketImpl);

    if(openSslSocketImpl == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return openSslSocketImpl->GetNpnSelectedProtocol(result);
}

ECode SSLCertificateSocketFactory::GetAlpnSelectedProtocol(
    /* [in] */ ISocket* socket,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IOpenSSLSocketImpl> openSslSocketImpl;
    CastToOpenSSLSocket(socket, (IOpenSSLSocketImpl**)&openSslSocketImpl);
    return openSslSocketImpl->GetAlpnSelectedProtocol(result);
}

ECode SSLCertificateSocketFactory::SetKeyManagers(
    /* [in] */ ArrayOf<IKeyManager*>* keyManagers)
{
    mKeyManagers = keyManagers;

    // Clear out any existing cached factories since configurations have changed.
    mSecureFactory = NULL;
    mInsecureFactory = NULL;
    return NOERROR;
}

ECode SSLCertificateSocketFactory::SetChannelIdPrivateKey(
    /* [in] */ IPrivateKey* privateKey)
{
    mChannelIdPrivateKey = privateKey;
    return NOERROR;
}

ECode SSLCertificateSocketFactory::SetUseSessionTickets(
    /* [in] */ ISocket* socket,
    /* [in] */ Boolean useSessionTickets)
{
    AutoPtr<IOpenSSLSocketImpl> openSslSocketImpl;
    CastToOpenSSLSocket(socket, (IOpenSSLSocketImpl**)&openSslSocketImpl);

    if(openSslSocketImpl == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return openSslSocketImpl->SetUseSessionTickets(useSessionTickets);
}

ECode SSLCertificateSocketFactory::SetHostname(
    /* [in] */ ISocket* socket,
    /* [in] */ const String& hostName)
{
    AutoPtr<IOpenSSLSocketImpl> openSslSocketImpl;
    CastToOpenSSLSocket(socket, (IOpenSSLSocketImpl**)&openSslSocketImpl);

    if(openSslSocketImpl == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return openSslSocketImpl->SetHostname(hostName);
}

ECode SSLCertificateSocketFactory::SetSoWriteTimeout(
    /* [in] */ ISocket* socket,
    /* [in] */ Int32 writeTimeoutMilliseconds)
{
    AutoPtr<IOpenSSLSocketImpl> openSslSocketImpl;
    CastToOpenSSLSocket(socket, (IOpenSSLSocketImpl**)&openSslSocketImpl);

    if(openSslSocketImpl == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return openSslSocketImpl->SetSoWriteTimeout(writeTimeoutMilliseconds);
}

ECode SSLCertificateSocketFactory::CastToOpenSSLSocket(
    /* [in] */ ISocket* socket,
    /* [out] */ IOpenSSLSocketImpl** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if(IOpenSSLSocketImpl::Probe(socket) == NULL) {
        String s;
        IObject::Probe(socket)->ToString(&s);
        Logger::E(TAG, "Socket not created by this factory: %s", s.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *result = IOpenSSLSocketImpl::Probe(socket);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SSLCertificateSocketFactory::CreateSocket(
    /* [in] */ ISocket* k,
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ Boolean close,
    /* [out] */ ISocket** result)
{
    VALIDATE_NOT_NULL(result);

    GetDelegate()->CreateSocket(k, host, port, close, result);

    AutoPtr<IOpenSSLSocketImpl> impl = IOpenSSLSocketImpl::Probe(*result);
    impl->SetNpnProtocols(mNpnProtocols);
    impl->SetAlpnProtocols(mAlpnProtocols);
    impl->SetHandshakeTimeout(mHandshakeTimeoutMillis);
    impl->SetChannelIdPrivateKey(mChannelIdPrivateKey);
    if (mSecure) {
        return VerifyHostname(*result, host);
    }
    return NOERROR;
}

ECode SSLCertificateSocketFactory::CreateSocket(
    /* [out] */ ISocket** result)
{
    VALIDATE_NOT_NULL(result);

    ISocketFactory::Probe(GetDelegate())->CreateSocket(result);

    AutoPtr<IOpenSSLSocketImpl> impl = IOpenSSLSocketImpl::Probe(*result);
    impl->SetNpnProtocols(mNpnProtocols);
    impl->SetAlpnProtocols(mAlpnProtocols);
    impl->SetHandshakeTimeout(mHandshakeTimeoutMillis);
    impl->SetChannelIdPrivateKey(mChannelIdPrivateKey);
    return NOERROR;
}

ECode SSLCertificateSocketFactory::CreateSocket(
    /* [in] */ IInetAddress* addr,
    /* [in] */ Int32 port,
    /* [in] */ IInetAddress* localAddr,
    /* [in] */ Int32 localPort,
    /* [out] */ ISocket** result)
{
    VALIDATE_NOT_NULL(result);

    ISocketFactory::Probe(GetDelegate())->CreateSocket(addr, port, localAddr, localPort, result);

    AutoPtr<IOpenSSLSocketImpl> impl = IOpenSSLSocketImpl::Probe(*result);
    impl->SetNpnProtocols(mNpnProtocols);
    impl->SetAlpnProtocols(mAlpnProtocols);
    impl->SetHandshakeTimeout(mHandshakeTimeoutMillis);
    impl->SetChannelIdPrivateKey(mChannelIdPrivateKey);
    return NOERROR;
}

ECode SSLCertificateSocketFactory::CreateSocket(
    /* [in] */ IInetAddress* addr,
    /* [in] */ Int32 port,
    /* [out] */ ISocket** result)
{
    VALIDATE_NOT_NULL(result);

    ISocketFactory::Probe(GetDelegate())->CreateSocket(addr, port, result);

    AutoPtr<IOpenSSLSocketImpl> impl = IOpenSSLSocketImpl::Probe(*result);
    impl->SetNpnProtocols(mNpnProtocols);
    impl->SetAlpnProtocols(mAlpnProtocols);
    impl->SetHandshakeTimeout(mHandshakeTimeoutMillis);
    impl->SetChannelIdPrivateKey(mChannelIdPrivateKey);
    return NOERROR;
}

ECode SSLCertificateSocketFactory::CreateSocket(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ IInetAddress* localAddr,
    /* [in] */ Int32 localPort,
    /* [out] */ ISocket** result)
{
    VALIDATE_NOT_NULL(result);

    ISocketFactory::Probe(GetDelegate())->CreateSocket(host, port, localAddr, localPort, result);

    AutoPtr<IOpenSSLSocketImpl> impl = IOpenSSLSocketImpl::Probe(*result);
    impl->SetNpnProtocols(mNpnProtocols);
    impl->SetAlpnProtocols(mAlpnProtocols);
    impl->SetHandshakeTimeout(mHandshakeTimeoutMillis);
    impl->SetChannelIdPrivateKey(mChannelIdPrivateKey);
    if (mSecure) {
        return VerifyHostname(*result, host);
    }
    return NOERROR;
}

ECode SSLCertificateSocketFactory::CreateSocket(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [out] */ ISocket** result)
{
    VALIDATE_NOT_NULL(result);

    ISocketFactory::Probe(GetDelegate())->CreateSocket(host, port, result);

    AutoPtr<IOpenSSLSocketImpl> impl = IOpenSSLSocketImpl::Probe(*result);
    impl->SetNpnProtocols(mNpnProtocols);
    impl->SetAlpnProtocols(mAlpnProtocols);
    impl->SetHandshakeTimeout(mHandshakeTimeoutMillis);
    impl->SetChannelIdPrivateKey(mChannelIdPrivateKey);
    return NOERROR;
}

ECode SSLCertificateSocketFactory::GetDefaultCipherSuites(
    /* [out, callee] */ ArrayOf<String>** result)
{
    return GetDelegate()->GetDefaultCipherSuites(result);
}

ECode SSLCertificateSocketFactory::GetSupportedCipherSuites(
    /* [out, callee] */ ArrayOf<String>** result)
{
    return GetDelegate()->GetSupportedCipherSuites(result);
}

} //namespace Net
} //namespace Droid
} //namespace Elastos
