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

#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Security.h"
#include "SSLParametersImpl.h"
#include "NativeCrypto.h"
#include "OpenSSLX509Certificate.h"
#include "OpenSSLKey.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/Arrays.h>
#include "libcore/utility/EmptyArray.h"

using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::EIID_ICloneable;
using Elastos::Security::IPrivateKey;
using Elastos::Security::IKeyStore;
using Elastos::Security::IKey;
using Elastos::Utility::Arrays;
using Elastos::Utility::CHashSet;
using Elastos::Utility::ISet;
using Elastosx::Crypto::ISecretKey;
using Elastosx::Net::Ssl::ISSLSession;
using Elastosx::Net::Ssl::ITrustManagerFactory;
using Elastosx::Net::Ssl::CTrustManagerFactory;
using Elastosx::Net::Ssl::CKeyManagerFactory;
using Elastosx::Net::Ssl::IKeyManagerFactory;

using Libcore::Utility::EmptyArray;

namespace Org {
namespace Conscrypt {

//=========================================
// SSLParametersImpl::
//=========================================

AutoPtr<IX509KeyManager> SSLParametersImpl::sDefaultX509KeyManager;

AutoPtr<IX509TrustManager> SSLParametersImpl::sDefaultX509TrustManager;

AutoPtr<ISecureRandom> SSLParametersImpl::sDefaultSecureRandom;

AutoPtr<ISSLParametersImpl> SSLParametersImpl::sDefaultParameters;

String SSLParametersImpl::KEY_TYPE_RSA("RSA");

String SSLParametersImpl::KEY_TYPE_DSA("DSA");

String SSLParametersImpl::KEY_TYPE_DH_RSA("DH_RSA");

String SSLParametersImpl::KEY_TYPE_DH_DSA("DH_DSA");

String SSLParametersImpl::KEY_TYPE_EC("EC");

String SSLParametersImpl::KEY_TYPE_EC_EC("EC_EC");

String SSLParametersImpl::KEY_TYPE_EC_RSA("EC_RSA");

CAR_INTERFACE_IMPL_2(SSLParametersImpl, Object, ISSLParametersImpl, ICloneable)

SSLParametersImpl::SSLParametersImpl()
    : mClient_mode(TRUE)
    , mNeed_client_auth(FALSE)
    , mWant_client_auth(FALSE)
    , mEnable_session_creation(TRUE)
{}

ECode SSLParametersImpl::constructor(
    /* [in] */ ArrayOf<IKeyManager*>* kms,
    /* [in] */ ArrayOf<ITrustManager*>* tms,
    /* [in] */ ISecureRandom* sr,
    /* [in] */ IClientSessionContext* clientSessionContext,
    /* [in] */ IServerSessionContext* serverSessionContext)
{
    mServerSessionContext = serverSessionContext;
    mClientSessionContext = clientSessionContext;

    // initialize key managers
    if (kms == NULL) {
        mX509KeyManager = GetDefaultX509KeyManager();
        // There's no default PSK key manager
        mPskKeyManager = NULL;
    }
    else {
        mX509KeyManager = FindFirstX509KeyManager(kms);
        mPskKeyManager = FindFirstPSKKeyManager(kms);
    }

    // initialize x509TrustManager
    if (tms == NULL) {
        GetDefaultX509TrustManager((IX509TrustManager**)&mX509TrustManager);
    }
    else {
        mX509TrustManager = FindFirstX509TrustManager(tms);
    }

    // initialize secure random
    // We simply use the SecureRandom passed in by the caller. If it's
    // NULL, we don't replace it by a new instance. The native code below
    // then directly accesses /dev/urandom. Not the most elegant solution,
    // but faster than going through the SecureRandom object.
    mSecureRandom = sr;

    // initialize the list of cipher suites and protocols enabled by default
    mEnabledProtocols = GetDefaultProtocols();
    Boolean x509CipherSuitesNeeded = (mX509KeyManager != NULL) || (mX509TrustManager != NULL);
    Boolean pskCipherSuitesNeeded = mPskKeyManager != NULL;
    mEnabledCipherSuites = GetDefaultCipherSuites(x509CipherSuitesNeeded, pskCipherSuitesNeeded);
    return NOERROR;
}

ECode SSLParametersImpl::GetDefault(
    /* [out] */ ISSLParametersImpl** res)
{
    VALIDATE_NOT_NULL(res)
    AutoPtr<ISSLParametersImpl> result = sDefaultParameters;
    if (result == NULL) {
        // single-check idiom
        assert(0 && "TODO");
        // sDefaultParameters = result = new SSLParametersImpl(NULL,
        //                                                    NULL,
        //                                                    NULL,
        //                                                    new ClientSessionContext(),
        //                                                    new ServerSessionContext());
    }
    AutoPtr<IInterface> p;
    ICloneable::Probe(result)->Clone((IInterface**)&p);
    *res = ISSLParametersImpl::Probe(p);
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode SSLParametersImpl::GetSessionContext(
    /* [out] */ IAbstractSessionContext** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mClient_mode ?
            IAbstractSessionContext::Probe(mClientSessionContext) :
            IAbstractSessionContext::Probe(mServerSessionContext);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SSLParametersImpl::GetServerSessionContext(
    /* [out] */ IServerSessionContext** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mServerSessionContext;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SSLParametersImpl::GetClientSessionContext(
    /* [out] */ IClientSessionContext** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mClientSessionContext;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SSLParametersImpl::GetX509KeyManager(
    /* [out] */ IX509KeyManager** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mX509KeyManager;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SSLParametersImpl::GetPSKKeyManager(
    /* [out] */ IPSKKeyManager** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPskKeyManager;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SSLParametersImpl::GetX509TrustManager(
    /* [out] */ IX509TrustManager** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mX509TrustManager;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SSLParametersImpl::GetSecureRandom(
    /* [out] */ ISecureRandom** result)
{
    VALIDATE_NOT_NULL(result)
    if (mSecureRandom != NULL) {
        *result = mSecureRandom;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    AutoPtr<ISecureRandom> res = sDefaultSecureRandom;
    if (res == NULL) {
        // single-check idiom
        assert(0 && "TODO");
        // CSecureRandom::New((ISecureRandom**)&res);
        sDefaultSecureRandom = res;
    }
    mSecureRandom = res;
    *result = mSecureRandom;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SSLParametersImpl::GetSecureRandomMember(
    /* [out] */ ISecureRandom** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSecureRandom;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SSLParametersImpl::GetEnabledCipherSuites(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<String> > arr = mEnabledCipherSuites->Clone();
    *result = arr;
    return NOERROR;
}

ECode SSLParametersImpl::SetEnabledCipherSuites(
    /* [in] */ ArrayOf<String>* cipherSuites)
{
    AutoPtr<ArrayOf<String> > arr;
    NativeCrypto::CheckEnabledCipherSuites(cipherSuites, (ArrayOf<String>**)&arr);
    mEnabledCipherSuites = arr->Clone();
    return NOERROR;
}

ECode SSLParametersImpl::GetEnabledProtocols(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<String> > arr = mEnabledProtocols->Clone();
    *result = arr;
    return NOERROR;
}

ECode SSLParametersImpl::SetEnabledProtocols(
    /* [in] */ ArrayOf<String>* protocols)
{
    AutoPtr<ArrayOf<String> > arr;
    NativeCrypto::CheckEnabledProtocols(protocols, (ArrayOf<String>**)&arr);
    mEnabledProtocols = arr->Clone();
    return NOERROR;
}

ECode SSLParametersImpl::SetUseClientMode(
    /* [in] */ Boolean mode)
{
    mClient_mode = mode;
    return NOERROR;
}

ECode SSLParametersImpl::GetUseClientMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mClient_mode;
    return NOERROR;
}

ECode SSLParametersImpl::SetNeedClientAuth(
    /* [in] */ Boolean need)
{
    mNeed_client_auth = need;
    // reset the want_client_auth setting
    mWant_client_auth = FALSE;
    return NOERROR;
}

ECode SSLParametersImpl::GetNeedClientAuth(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mNeed_client_auth;
    return NOERROR;
}

ECode SSLParametersImpl::SetWantClientAuth(
    /* [in] */ Boolean want)
{
    mWant_client_auth = want;
    // reset the need_client_auth setting
    mNeed_client_auth = FALSE;
    return NOERROR;
}

ECode SSLParametersImpl::GetWantClientAuth(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWant_client_auth;
    return NOERROR;
}

ECode SSLParametersImpl::SetEnableSessionCreation(
    /* [in] */ Boolean flag)
{
    mEnable_session_creation = flag;
    return NOERROR;
}

ECode SSLParametersImpl::GetEnableSessionCreation(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEnable_session_creation;
    return NOERROR;
}

ECode SSLParametersImpl::SetUseSni(
    /* [in] */ Boolean flag)
{
    mUseSni = flag;
    return NOERROR;
}

ECode SSLParametersImpl::GetUseSni(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUseSni;
    return NOERROR;
}

ECode SSLParametersImpl::EncodeIssuerX509Principals(
    /* [in] */ ArrayOf<IX509Certificate*>* certificates,
    /* [out] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >** result)
{
    VALIDATE_NOT_NULL(result)
    assert(0 && "TODO");
    AutoPtr<ArrayOf<AutoPtr<ArrayOf<Byte> > > > principalBytes;// = new byte[certificates->GetLength()][];
    for (Int32 i = 0; i < certificates->GetLength(); i++) {
        AutoPtr<IX500Principal> principal;
        (*certificates)[i]->GetIssuerX500Principal((IX500Principal**)&principal);
        principal->GetEncoded((ArrayOf<Byte>**)&((*principalBytes)[i]));
    }
    *result = principalBytes;
    return NOERROR;
}

AutoPtr<ArrayOf<IOpenSSLX509Certificate*> > SSLParametersImpl::CreateCertChain(
    /* [in] */ ArrayOf<Int64>* certificateRefs)
{
    if (certificateRefs == NULL) {
        return NULL;
    }
    AutoPtr<ArrayOf<IOpenSSLX509Certificate*> > certificates =
            ArrayOf<IOpenSSLX509Certificate*>::Alloc(certificateRefs->GetLength());
    for (Int32 i = 0; i < certificateRefs->GetLength(); i++) {
        assert(0 && "TODO");
        // COpenSSLX509Certificate::New((*certificateRefs)[i], (IOpenSSLX509Certificate**)&((*certificates)[i]));
    }
    return certificates;
}

ECode SSLParametersImpl::GetSessionToReuse(
    /* [in] */ Int64 sslNativePointer,
    /* [in] */ const String& hostname,
    /* [in] */ Int32 port,
    /* [out] */ IOpenSSLSessionImpl** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLSessionImpl> sessionToReuse;
    if (mClient_mode) {
        // look for client session to reuse
        GetCachedClientSession(mClientSessionContext, hostname, port, (IOpenSSLSessionImpl**)&sessionToReuse);
        if (sessionToReuse != NULL) {
            assert(0 && "TODO");
            // NativeCrypto::SSL_set_session(sslNativePointer,
            //         sessionToReuse->mSslSessionNativePointer);
        }
    }
    else {
        sessionToReuse = NULL;
    }
    *result = sessionToReuse;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SSLParametersImpl::SetTlsChannelId(
    /* [in] */ Int64 sslNativePointer,
    /* [in] */ IOpenSSLKey* channelIdPrivateKey)
{
    // TLS Channel ID
    if (mChannelIdEnabled) {
        if (mClient_mode) {
            // Client-side TLS Channel ID
            if (channelIdPrivateKey == NULL) {
                // throw new SSLHandshakeException("Invalid TLS channel ID key specified");
                // return E_SSL_HAND_SHAKE_EXCEPTION;
                return NOERROR;
            }
            Int64 context = 0;
            channelIdPrivateKey->GetPkeyContext(&context);
            NativeCrypto::SSL_set1_tls_channel_id(sslNativePointer,
                    context);
        }
        else {
            // Server-side TLS Channel ID
            NativeCrypto::SSL_enable_tls_channel_id(sslNativePointer);
        }
    }
    return NOERROR;
}

ECode SSLParametersImpl::SetCertificate(
    /* [in] */ Int64 sslNativePointer,
    /* [in] */ const String& alias)
{
    if (alias.IsNull()) {
        return NOERROR;
    }
    AutoPtr<IX509KeyManager> keyManager;
    GetX509KeyManager((IX509KeyManager**)&keyManager);
    if (keyManager == NULL) {
        return NOERROR;
    }
    AutoPtr<IPrivateKey> privateKey;
    keyManager->GetPrivateKey(alias, (IPrivateKey**)&privateKey);
    if (privateKey == NULL) {
        return NOERROR;
    }
    AutoPtr<ArrayOf<IX509Certificate*> > certificates;
    keyManager->GetCertificateChain(alias, (ArrayOf<IX509Certificate*>**)&certificates);
    if (certificates == NULL) {
        return NOERROR;
    }

    /*
     * Make sure we keep a reference to the OpenSSLX509Certificate by using
     * this array. Otherwise, if they're not OpenSSLX509Certificate
     * instances originally, they may be garbage collected before we
     * complete our JNI calls.
     */
    AutoPtr<ArrayOf<IOpenSSLX509Certificate*> > openSslCerts
        = ArrayOf<IOpenSSLX509Certificate*>::Alloc(certificates->GetLength());
    AutoPtr<ArrayOf<Int64> > x509refs = ArrayOf<Int64>::Alloc(certificates->GetLength());
    for (Int32 i = 0; i < certificates->GetLength(); i++) {
        AutoPtr<IOpenSSLX509Certificate> openSslCert;
        OpenSSLX509Certificate::FromCertificate(
                ICertificate::Probe((*certificates)[i]),
                (IOpenSSLX509Certificate**)&openSslCert);
        (*openSslCerts)[i] = openSslCert;
        openSslCert->GetContext(&((*x509refs)[i]));
    }

    // Note that OpenSSL says to use SSL_use_certificate before
    // SSL_use_PrivateKey.
    NativeCrypto::SSL_use_certificate(sslNativePointer, x509refs);

    AutoPtr<IOpenSSLKey> key = OpenSSLKey::FromPrivateKey(privateKey);
    Int64 context = 0;
    key->GetPkeyContext(&context);
    NativeCrypto::SSL_use_PrivateKey(sslNativePointer, context);

    // checks the last installed private key and certificate,
    // so need to do this once per loop iteration
    NativeCrypto::SSL_check_private_key(sslNativePointer);
    return NOERROR;
}

ECode SSLParametersImpl::SetSSLParameters(
    /* [in] */ Int64 sslCtxNativePointer,
    /* [in] */ Int64 sslNativePointer,
    /* [in] */ ISSLParametersImplAliasChooser* chooser,
    /* [in] */ ISSLParametersImplPSKCallbacks* pskCallbacks,
    /* [in] */ const String& hostname)
{
    if (mNpnProtocols != NULL) {
        NativeCrypto::SSL_CTX_enable_npn(sslCtxNativePointer);
    }

    if (mClient_mode && mAlpnProtocols != NULL) {
        Int32 res = 0;
        NativeCrypto::SSL_set_alpn_protos(sslNativePointer, mAlpnProtocols, &res);
    }

    NativeCrypto::SetEnabledProtocols(sslNativePointer, mEnabledProtocols);
    NativeCrypto::SetEnabledCipherSuites(sslNativePointer, mEnabledCipherSuites);

    // setup server certificates and private keys.
    // clients will receive a call back to request certificates.
    if (!mClient_mode) {
        AutoPtr<ISet> keyTypes;
        CHashSet::New((ISet**)&keyTypes);
        AutoPtr<ArrayOf<Int64> > ciphers;
        NativeCrypto::SSL_get_ciphers(sslNativePointer, (ArrayOf<Int64>**)&ciphers);
        for (Int32 i = 0; i < ciphers->GetLength(); i++) {
            Int64 sslCipherNativePointer = (*ciphers)[i];
            String keyType = GetServerX509KeyType(sslCipherNativePointer);
            if (!keyType.IsNull()) {
                keyTypes->Add(CoreUtils::Convert(keyType));
            }
        }
        AutoPtr<IX509KeyManager> keyManager;
        GetX509KeyManager((IX509KeyManager**)&keyManager);
        if (keyManager != NULL) {
            assert(0 && "TODO");
            // for (String keyType : keyTypes) {
            //     SetCertificate(sslNativePointer,
            //             chooser->ChooseServerAlias(mX509KeyManager, keyType));
            // }
        }
    }

    // Enable Pre-Shared Key (PSK) key exchange if requested
    AutoPtr<IPSKKeyManager> pskKeyManager;
    GetPSKKeyManager((IPSKKeyManager**)&pskKeyManager);
    if (pskKeyManager != NULL) {
        Boolean pskEnabled = FALSE;
        for (Int32 i = 0; i < mEnabledCipherSuites->GetLength(); i++) {
            String enabledCipherSuite = (*mEnabledCipherSuites)[i];
            if ((!enabledCipherSuite.IsNull()) && (enabledCipherSuite.Contains("PSK"))) {
                pskEnabled = TRUE;
                break;
            }
        }
        if (pskEnabled) {
            if (mClient_mode) {
                NativeCrypto::Set_SSL_psk_client_callback_enabled(sslNativePointer, TRUE);
            }
            else {
                NativeCrypto::Set_SSL_psk_server_callback_enabled(sslNativePointer, TRUE);
                String identityHint;
                pskCallbacks->ChooseServerPSKIdentityHint(pskKeyManager, &identityHint);
                NativeCrypto::SSL_use_psk_identity_hint(sslNativePointer, identityHint);
            }
        }
    }

    if (mUseSessionTickets) {
        Int64 res = 0;
        NativeCrypto::SSL_clear_options(sslNativePointer, INativeCrypto::SSL_OP_NO_TICKET, &res);
    }
    if (mUseSni) {
        NativeCrypto::SSL_set_tlsext_host_name(sslNativePointer, hostname);
    }

    // BEAST attack mitigation (1/n-1 record splitting for CBC cipher suites
    // with TLSv1 and SSLv3).
    Int64 mode = 0;
    NativeCrypto::SSL_set_mode(sslNativePointer, INativeCrypto::SSL_MODE_CBC_RECORD_SPLITTING, &mode);

    Boolean enableSessionCreation = FALSE;
    GetEnableSessionCreation(&enableSessionCreation);
    if (!enableSessionCreation) {
        NativeCrypto::SSL_set_session_creation_enabled(sslNativePointer, enableSessionCreation);
    }
    return NOERROR;
}

ECode SSLParametersImpl::SetCertificateValidation(
    /* [in] */ Int64 sslNativePointer)
{
    // setup peer certificate verification
    if (!mClient_mode) {
        // needing client auth takes priority...
        Boolean certRequested = FALSE;
        Boolean bNeedAuth = FALSE, bWantAuth = FALSE;
        if ((GetNeedClientAuth(&bNeedAuth), bNeedAuth)) {
            NativeCrypto::SSL_set_verify(sslNativePointer,
                                        INativeCrypto::SSL_VERIFY_PEER
                                        | INativeCrypto::SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
            certRequested = TRUE;
        // ... over just wanting it...
        }
        else if ((GetWantClientAuth(&bWantAuth), bWantAuth)) {
            NativeCrypto::SSL_set_verify(sslNativePointer, INativeCrypto::SSL_VERIFY_PEER);
            certRequested = TRUE;
        // ... and we must disable verification if we don't want client auth.
        }
        else {
            NativeCrypto::SSL_set_verify(sslNativePointer, INativeCrypto::SSL_VERIFY_NONE);
            certRequested = FALSE;
        }

        if (certRequested) {
            AutoPtr<IX509TrustManager> trustManager;
            GetX509TrustManager((IX509TrustManager**)&trustManager);
            AutoPtr<ArrayOf<IX509Certificate*> > issuers;
            trustManager->GetAcceptedIssuers((ArrayOf<IX509Certificate*>**)&issuers);
            if (issuers != NULL && issuers->GetLength() != 0) {
                AutoPtr<ArrayOf<AutoPtr<ArrayOf<Byte> > > > issuersBytes;
                EncodeIssuerX509Principals(issuers, (ArrayOf<AutoPtr<ArrayOf<Byte> > >**)&issuersBytes);
                assert(0 && "TODO");
                // NativeCrypto::SSL_set_client_CA_list(sslNativePointer, issuersBytes);
            }
        }
    }
    return NOERROR;
}

ECode SSLParametersImpl::SetupSession(
    /* [in] */ Int64 sslSessionNativePointer,
    /* [in] */ Int64 sslNativePointer,
    /* [in] */ IOpenSSLSessionImpl* sessionToReuse,
    /* [in] */ const String& hostname,
    /* [in] */ Int32 port,
    /* [in] */ Boolean handshakeCompleted,
    /* [out] */ IOpenSSLSessionImpl** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLSessionImpl> sslSession;
    AutoPtr<ArrayOf<Byte> > sessionId;
    NativeCrypto::SSL_SESSION_session_id(sslSessionNativePointer, (ArrayOf<Byte>**)&sessionId);
    AutoPtr<ArrayOf<Byte> > id;
    if (sessionToReuse != NULL &&
        Arrays::Equals((ISSLSession::Probe(sessionToReuse)->GetId((ArrayOf<Byte>**)&id), id),
            sessionId)) {
        sslSession = sessionToReuse;
        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        assert(0 && "TODO");
        // sys->GetCurrentTimeMillis(&(sslSession->mLastAccessedTime));
        NativeCrypto::SSL_SESSION_free(sslSessionNativePointer);
    }
    else {
        Boolean b = FALSE;
        if (!(GetEnableSessionCreation(&b), b)) {
            // Should have been prevented by
            // NativeCrypto.SSL_set_session_creation_enabled
            // throw new IllegalStateException("SSL Session may not be created");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        AutoPtr<ArrayOf<Int64> > certificate;
        NativeCrypto::SSL_get_certificate(sslNativePointer, (ArrayOf<Int64>**)&certificate);
        assert(0 && "TODO");
        AutoPtr<ArrayOf<IX509Certificate*> > localCertificates;// = CreateCertChain(certificate);
        AutoPtr<ArrayOf<Int64> > cert_chain;
        NativeCrypto::SSL_get_peer_cert_chain(sslNativePointer, (ArrayOf<Int64>**)&cert_chain);
        assert(0 && "TODO");
        AutoPtr<ArrayOf<IX509Certificate*> > peerCertificates;// = CreateCertChain(cert_chain);
        AutoPtr<IAbstractSessionContext> sessionContext;
        GetSessionContext((IAbstractSessionContext**)&sessionContext);
        assert(0 && "TODO");
        // COpenSSLSessionImpl::New(sslSessionNativePointer, localCertificates,
        //         peerCertificates, hostname, port, sessionContext, (IOpenSSLSessionImpl**)&sslSession);
        // if not, putSession later in handshakeCompleted() callback
        if (handshakeCompleted) {
            AutoPtr<IAbstractSessionContext> ctx;
            GetSessionContext((IAbstractSessionContext**)&ctx);
            ctx->PutSession(ISSLSession::Probe(sslSession));
        }
    }
    *result = sslSession;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SSLParametersImpl::ChooseClientCertificate(
    /* [in] */ ArrayOf<Byte>* keyTypeBytes,
    /* [in] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >* asn1DerEncodedPrincipals,
    /* [in] */ Int64 sslNativePointer,
    /* [in] */ ISSLParametersImplAliasChooser* chooser)
{
    AutoPtr<ArrayOf<String> > keyTypes = ArrayOf<String>::Alloc(keyTypeBytes->GetLength());
    for (Int32 i = 0; i < keyTypeBytes->GetLength(); i++) {
        GetClientKeyType((*keyTypeBytes)[i], &((*keyTypes)[i]));
    }

    AutoPtr<ArrayOf<IX500Principal*> > issuers;
    if (asn1DerEncodedPrincipals == NULL) {
        issuers = NULL;
    }
    else {
        issuers = ArrayOf<IX500Principal*>::Alloc(asn1DerEncodedPrincipals->GetLength());
        for (Int32 i = 0; i < asn1DerEncodedPrincipals->GetLength(); i++) {
            assert(0 && "TODO");
            // (*issuers)[i] = ArrayOf<IX500Principal*>::Alloc(((*asn1DerEncodedPrincipals)[i]));
        }
    }
    AutoPtr<IX509KeyManager> keyManager;
    GetX509KeyManager((IX509KeyManager**)&keyManager);
    String alias(NULL);
    if (keyManager != NULL) {
        chooser->ChooseClientAlias(keyManager, issuers, keyTypes, &alias);
    }
    SetCertificate(sslNativePointer, alias);
    return NOERROR;
}

ECode SSLParametersImpl::ClientPSKKeyRequested(
    /* [in] */ const String& identityHint,
    /* [in] */ ArrayOf<Byte>* identityBytesOut,
    /* [in] */ ArrayOf<Byte>* key,
    /* [in] */ ISSLParametersImplPSKCallbacks* pskCallbacks,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPSKKeyManager> pskKeyManager;
    GetPSKKeyManager((IPSKKeyManager**)&pskKeyManager);
    if (pskKeyManager == NULL) {
        *result = 0;
        return NOERROR;
    }

    String identity;
    pskCallbacks->ChooseClientPSKIdentity(pskKeyManager, identityHint, (String*)&identity);
    // Store identity in NULL-terminated modified UTF-8 representation into ientityBytesOut
    AutoPtr<ArrayOf<Byte> > identityBytes;
    if (identity.IsNull()) {
        identity = String("");
        identityBytes = EmptyArray::BYTE;
    }
    else if (identity.IsEmpty()) {
        identityBytes = EmptyArray::BYTE;
    }
    else {
        identityBytes = identity.GetBytes(); // "UTF-8");
    }
    if (identityBytes->GetLength() + 1 > identityBytesOut->GetLength()) {
        // Insufficient space in the output buffer
        return 0;
    }
    if (identityBytes->GetLength() > 0) {
        identityBytesOut->Copy(0, identityBytes, 0, identityBytes->GetLength());
    }
    (*identityBytesOut)[identityBytes->GetLength()] = 0;

    AutoPtr<ISecretKey> secretKey;
    pskCallbacks->GetPSKKey(pskKeyManager, identityHint, identity, (ISecretKey**)&secretKey);
    AutoPtr<ArrayOf<Byte> > secretKeyBytes;
    IKey::Probe(secretKey)->GetEncoded((ArrayOf<Byte>**)&secretKeyBytes);
    if (secretKeyBytes == NULL) {
        *result = 0;
        return NOERROR;
    }
    else if (secretKeyBytes->GetLength() > key->GetLength()) {
        // Insufficient space in the output buffer
        *result = 0;
        return NOERROR;
    }
    key->Copy(0, secretKeyBytes, 0, secretKeyBytes->GetLength());
    *result = secretKeyBytes->GetLength();
    return NOERROR;
}

ECode SSLParametersImpl::ServerPSKKeyRequested(
    /* [in] */ const String& identityHint,
    /* [in] */ const String& identity,
    /* [in] */ ArrayOf<Byte>* key,
    /* [in] */ ISSLParametersImplPSKCallbacks* pskCallbacks,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPSKKeyManager> pskKeyManager;
    GetPSKKeyManager((IPSKKeyManager**)&pskKeyManager);
    if (pskKeyManager == NULL) {
        *result = 0;
        return NOERROR;
    }
    AutoPtr<ISecretKey> secretKey;
    pskCallbacks->GetPSKKey(pskKeyManager, identityHint, identity, (ISecretKey**)&secretKey);
    AutoPtr<ArrayOf<Byte> > secretKeyBytes;
    IKey::Probe(secretKey)->GetEncoded((ArrayOf<Byte>**)&secretKeyBytes);
    if (secretKeyBytes == NULL) {
        *result = 0;
        return NOERROR;
    }
    else if (secretKeyBytes->GetLength() > key->GetLength()) {
        *result = 0;
        return NOERROR;
    }
    key->Copy(0, secretKeyBytes, 0, secretKeyBytes->GetLength());
    *result = secretKeyBytes->GetLength();
    return NOERROR;
}

ECode SSLParametersImpl::GetCachedClientSession(
    /* [in] */ IClientSessionContext* sessionContext,
    /* [in] */ const String& hostName,
    /* [in] */ Int32 port,
    /* [out] */ IOpenSSLSessionImpl** result)
{
    VALIDATE_NOT_NULL(result)
    if (hostName.IsNull()) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<ISSLSession> ses;
    sessionContext->GetSession(hostName, port, (ISSLSession**)&ses);
    AutoPtr<IOpenSSLSessionImpl> session = IOpenSSLSessionImpl::Probe(ses);
    if (session == NULL) {
        *result = NULL;
        return NOERROR;
    }

    String protocol;
    ISSLSession::Probe(session)->GetProtocol(&protocol);
    Boolean protocolFound = FALSE;
    for (Int32 i = 0; i < mEnabledProtocols->GetLength(); i++) {
        String enabledProtocol = (*mEnabledProtocols)[i];
        if (protocol.Equals(enabledProtocol)) {
            protocolFound = TRUE;
            break;
        }
    }
    if (!protocolFound) {
        *result = NULL;
        return NOERROR;
    }

    String cipherSuite;
    ISSLSession::Probe(session)->GetCipherSuite(&cipherSuite);
    Boolean cipherSuiteFound = FALSE;
    for (Int32 i = 0; i < mEnabledCipherSuites->GetLength(); i++) {
        String enabledCipherSuite = (*mEnabledCipherSuites)[i];
        if (cipherSuite.Equals(enabledCipherSuite)) {
            cipherSuiteFound = TRUE;
            break;
        }
    }
    if (!cipherSuiteFound) {
        *result = NULL;
        return NOERROR;
    }

    *result = session;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SSLParametersImpl::Clone(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    assert(0 && "TODO");
    // return super.clone();
    return NOERROR;
}

AutoPtr<IX509KeyManager> SSLParametersImpl::GetDefaultX509KeyManager()
{
    AutoPtr<IX509KeyManager> result = sDefaultX509KeyManager;
    if (result == NULL) {
        // single-check idiom
        result = CreateDefaultX509KeyManager();
        sDefaultX509KeyManager = result;
    }
    return result;
}

AutoPtr<IX509KeyManager> SSLParametersImpl::CreateDefaultX509KeyManager()
{
    // try {
        String algorithm;
        assert(0 && "TODO");
        // CKeyManagerFactory::GetDefaultAlgorithm(&algorithm);
        AutoPtr<IKeyManagerFactory> kmf;
        // CKeyManagerFactory::GetInstance(algorithm, (IKeyManagerFactory**)&kmf);
        kmf->Init(NULL, NULL);
        AutoPtr<ArrayOf<IKeyManager*> > kms;
        kmf->GetKeyManagers((ArrayOf<IKeyManager*>**)&kms);
        AutoPtr<IX509KeyManager> result = FindFirstX509KeyManager(kms);
        if (result == NULL) {
            // throw new KeyManagementException("No X509KeyManager among default KeyManagers: "
            //         + Arrays.toString(kms));
            // return E_KEY_MANAGEMENT_EXCEPTION;
            return NULL;
        }
        return result;
    // } catch (NoSuchAlgorithmException e) {
    //     throw new KeyManagementException(e);
    // } catch (KeyStoreException e) {
    //     throw new KeyManagementException(e);
    // } catch (UnrecoverableKeyException e) {
    //     throw new KeyManagementException(e);
    // }
}

AutoPtr<IX509KeyManager> SSLParametersImpl::FindFirstX509KeyManager(
    /* [in] */ ArrayOf<IKeyManager*>* kms)
{
    for (Int32 i = 0; i < kms->GetLength(); i++) {
        AutoPtr<IKeyManager> km = (*kms)[i];
        if (IX509KeyManager::Probe(km) != NULL) {
            return IX509KeyManager::Probe(km);
        }
    }
    return NULL;
}

AutoPtr<IPSKKeyManager> SSLParametersImpl::FindFirstPSKKeyManager(
    /* [in] */ ArrayOf<IKeyManager*>* kms)
{
    for (Int32 i = 0; i < kms->GetLength(); i++) {
        AutoPtr<IKeyManager> km = (*kms)[i];
        if (IPSKKeyManager::Probe(km) != NULL) {
            return IPSKKeyManager::Probe(km);
        }
    }
    return NULL;
}

ECode SSLParametersImpl::GetDefaultX509TrustManager(
    /* [out] */ IX509TrustManager** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IX509TrustManager> res = sDefaultX509TrustManager;
    if (res == NULL) {
        // single-check idiom
        sDefaultX509TrustManager = res = CreateDefaultX509TrustManager();
    }
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<IX509TrustManager> SSLParametersImpl::CreateDefaultX509TrustManager()
{
    // try {
        String algorithm;
        assert(0 && "TODO");
        // CTrustManagerFactory::GetDefaultAlgorithm(&algorithm);
        AutoPtr<ITrustManagerFactory> tmf;
        // CTrustManagerFactory::GetInstance(algorithm, (ITrustManagerFactory**)&tmf);
        AutoPtr<IKeyStore> p;
        tmf->Init(p); // tmf->Init((KeyStore) NULL);
        AutoPtr<ArrayOf<ITrustManager*> > tms;
        tmf->GetTrustManagers((ArrayOf<ITrustManager*>**)&tms);
        AutoPtr<IX509TrustManager> trustManager = FindFirstX509TrustManager(tms);
        if (trustManager == NULL) {
            // throw new KeyManagementException(
            //         "No X509TrustManager in among default TrustManagers: "
            //                 + Arrays.toString(tms));
            return NULL;
        }
        return trustManager;
    // } catch (NoSuchAlgorithmException e) {
    //     throw new KeyManagementException(e);
    // } catch (KeyStoreException e) {
    //     throw new KeyManagementException(e);
    // }
}

AutoPtr<IX509TrustManager> SSLParametersImpl::FindFirstX509TrustManager(
    /* [in] */ ArrayOf<ITrustManager*>* tms)
{
    for (Int32 i = 0; i < tms->GetLength(); i++) {
        AutoPtr<ITrustManager> tm = (*tms)[i];
        if (IX509TrustManager::Probe(tm) != NULL) {
            return IX509TrustManager::Probe(tm);
        }
    }
    return NULL;
}

ECode SSLParametersImpl::GetEndpointIdentificationAlgorithm(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEndpointIdentificationAlgorithm;
    return NOERROR;
}

ECode SSLParametersImpl::SetEndpointIdentificationAlgorithm(
    /* [in] */ const String& endpointIdentificationAlgorithm)
{
    mEndpointIdentificationAlgorithm = endpointIdentificationAlgorithm;
    return NOERROR;
}

String SSLParametersImpl::GetServerX509KeyType(
    /* [in] */ Int64 sslCipherNative)
{
    Int32 algorithm_mkey = 0;
    NativeCrypto::Get_SSL_CIPHER_algorithm_mkey(sslCipherNative, &algorithm_mkey);
    Int32 algorithm_auth = 0;
    NativeCrypto::Get_SSL_CIPHER_algorithm_auth(sslCipherNative, &algorithm_auth);
    switch (algorithm_mkey) {
        case INativeCrypto::SSL_kRSA:
            return KEY_TYPE_RSA;
        case INativeCrypto::SSL_kEDH:
            switch (algorithm_auth) {
                case INativeCrypto::SSL_aDSS:
                    return KEY_TYPE_DSA;
                case INativeCrypto::SSL_aRSA:
                    return KEY_TYPE_RSA;
                case INativeCrypto::SSL_aNULL:
                    return String(NULL);
            }
            break;
        case INativeCrypto::SSL_kECDHr:
            return KEY_TYPE_EC_RSA;
        case INativeCrypto::SSL_kECDHe:
            return KEY_TYPE_EC_EC;
        case INativeCrypto::SSL_kEECDH:
            switch (algorithm_auth) {
                case INativeCrypto::SSL_aECDSA:
                    return KEY_TYPE_EC_EC;
                case INativeCrypto::SSL_aRSA:
                    return KEY_TYPE_RSA;
                case INativeCrypto::SSL_aPSK:
                    return String(NULL);
                case INativeCrypto::SSL_aNULL:
                    return String(NULL);
            }
            break;
        case INativeCrypto::SSL_kPSK:
            return String(NULL);
    }

    // throw new SSLException("Unsupported key exchange. "
    //         + "mkey: 0x" + Long.toHexString(algorithm_mkey & 0xffffffffL)
    //         + ", auth: 0x" + Long.toHexString(algorithm_auth & 0xffffffffL));
    return String(NULL);
}

ECode SSLParametersImpl::GetClientKeyType(
    /* [in] */ Byte keyType,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // See also http://www.ietf.org/assignments/tls-parameters/tls-parameters.xml
    switch (keyType) {
        case INativeCrypto::TLS_CT_RSA_SIGN: {
            *result = KEY_TYPE_RSA; // RFC rsa_sign
            return NOERROR;
        }
        case INativeCrypto::TLS_CT_DSS_SIGN: {
            *result = KEY_TYPE_DSA; // RFC dss_sign
            return NOERROR;
        }
        case INativeCrypto::TLS_CT_RSA_FIXED_DH: {
            *result = KEY_TYPE_DH_RSA; // RFC rsa_fixed_dh
            return NOERROR;
        }
        case INativeCrypto::TLS_CT_DSS_FIXED_DH: {
            *result = KEY_TYPE_DH_DSA; // RFC dss_fixed_dh
            return NOERROR;
        }
        case INativeCrypto::TLS_CT_ECDSA_SIGN: {
            *result = KEY_TYPE_EC; // RFC ecdsa_sign
            return NOERROR;
        }
        case INativeCrypto::TLS_CT_RSA_FIXED_ECDH: {
            *result = KEY_TYPE_EC_RSA; // RFC rsa_fixed_ecdh
            return NOERROR;
        }
        case INativeCrypto::TLS_CT_ECDSA_FIXED_ECDH: {
            *result = KEY_TYPE_EC_EC; // RFC ecdsa_fixed_ecdh
            return NOERROR;
        }
        default: {
            *result = String(NULL);
            return NOERROR;
        }
    }
}

AutoPtr<ArrayOf<String> > SSLParametersImpl::GetDefaultCipherSuites(
    /* [in] */ Boolean x509CipherSuitesNeeded,
    /* [in] */ Boolean pskCipherSuitesNeeded)
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
    (*arr)[0] = INativeCrypto::TLS_EMPTY_RENEGOTIATION_INFO_SCSV;
    if (x509CipherSuitesNeeded) {
        // X.509 based cipher suites need to be listed.
        if (pskCipherSuitesNeeded) {
            // Both X.509 and PSK based cipher suites need to be listed. Because TLS-PSK is not
            // normally used, we assume that when PSK cipher suites are requested here they
            // should be preferred over other cipher suites. Thus, we give PSK cipher suites
            // higher priority than X.509 cipher suites.
            // NOTE: There are cipher suites that use both X.509 and PSK (e.g., those based on
            // RSA_PSK key exchange). However, these cipher suites are not currently supported.
            assert(0 && "TODO");
            // return Concat(
            //         INativeCrypto::DEFAULT_PSK_CIPHER_SUITES,
            //         INativeCrypto::DEFAULT_X509_CIPHER_SUITES,
            //         arr);
            return NULL;
        }
        else {
            // Only X.509 cipher suites need to be listed.
            assert(0 && "TODO");
            // return Concat(
            //         INativeCrypto::DEFAULT_X509_CIPHER_SUITES,
            //         arr);
            return NULL;
        }
    }
    else if (pskCipherSuitesNeeded) {
        // Only PSK cipher suites need to be listed.
        assert(0 && "TODO");
        // return Concat(
        //         INativeCrypto::DEFAULT_PSK_CIPHER_SUITES,
        //         arr);
        return NULL;
    }
    else {
        // Neither X.509 nor PSK cipher suites need to be listed.
        return arr;
    }
}

AutoPtr<ArrayOf<String> > SSLParametersImpl::GetDefaultProtocols()
{
    assert(0 && "TODO");
    // return NativeCrypto::DEFAULT_PROTOCOLS.Clone();
    return NULL;
}

AutoPtr<ArrayOf<String> > SSLParametersImpl::Concat(
    /* [in] */ ArrayOf<AutoPtr<ArrayOf<String> > >* arrays)
{
    Int32 resultLength = 0;
    for (Int32 i = 0; i < arrays->GetLength(); i++) {
        AutoPtr<ArrayOf<String> > array = (*arrays)[i];
        resultLength += array->GetLength();
    }
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(resultLength);
    Int32 resultOffset = 0;
    for (Int32 i = 0; i < arrays->GetLength(); i++) {
        AutoPtr<ArrayOf<String> > array = (*arrays)[i];
        result->Copy(resultOffset, array, 0, array->GetLength());
        resultOffset += array->GetLength();
    }
    return result;
}

} // namespace Conscrypt
} // namespace Org
