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

#ifndef __ORG_CONSCRYPT_SSLPARAMETERSIMPL_H__
#define __ORG_CONSCRYPT_SSLPARAMETERSIMPL_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Security.h"
#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICloneable;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::ISecureRandom;
using Elastosx::Net::Ssl::IKeyManager;
using Elastosx::Net::Ssl::IX509KeyManager;
using Elastosx::Net::Ssl::IX509TrustManager;
using Elastosx::Net::Ssl::ITrustManager;

namespace Org {
namespace Conscrypt {

/**
 * The instances of this class encapsulate all the info
 * about enabled cipher suites and protocols,
 * as well as the information about client/server mode of
 * ssl socket, whether it require/want client authentication or not,
 * and controls whether new SSL sessions may be established by this
 * socket or not.
 */
class SSLParametersImpl
    : public Object
    , public ISSLParametersImpl
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Initializes the parameters. Naturally this constructor is used
     * in SSLContextImpl.engineInit method which directly passes its
     * parameters. In other words this constructor holds all
     * the functionality provided by SSLContext.init method.
     * See {@link javax.net.ssl.SSLContext#init(KeyManager[],TrustManager[],
     * SecureRandom)} for more information
     */
    SSLParametersImpl();

    CARAPI constructor(
        /* [in] */ ArrayOf<IKeyManager*>* kms,
        /* [in] */ ArrayOf<ITrustManager*>* tms,
        /* [in] */ ISecureRandom* sr,
        /* [in] */ IClientSessionContext* clientSessionContext,
        /* [in] */ IServerSessionContext* serverSessionContext);

    static CARAPI GetDefault(
        /* [out] */ ISSLParametersImpl** result);

    /**
     * Returns the appropriate session context.
     */
    CARAPI GetSessionContext(
        /* [out] */ IAbstractSessionContext** result);

    /**
     * @return server session context
     */
    CARAPI GetServerSessionContext(
        /* [out] */ IServerSessionContext** result);

    /**
     * @return client session context
     */
    CARAPI GetClientSessionContext(
        /* [out] */ IClientSessionContext** result);

    /**
     * @return X.509 key manager or {@code null} for none.
     */
    CARAPI GetX509KeyManager(
        /* [out] */ IX509KeyManager** result);

    /**
     * @return Pre-Shared Key (PSK) key manager or {@code null} for none.
     */
    CARAPI GetPSKKeyManager(
        /* [out] */ IPSKKeyManager** result);

    /**
     * @return X.509 trust manager or {@code null} for none.
     */
    CARAPI GetX509TrustManager(
        /* [out] */ IX509TrustManager** result);

    /**
     * @return secure random
     */
    CARAPI GetSecureRandom(
        /* [out] */ ISecureRandom** result);

    /**
     * @return the secure random member reference, even it is null
     */
    CARAPI GetSecureRandomMember(
        /* [out] */ ISecureRandom** result);

    /**
     * @return the names of enabled cipher suites
     */
    CARAPI GetEnabledCipherSuites(
        /* [out] */ ArrayOf<String>** result);

    /**
     * Sets the enabled cipher suites after filtering through OpenSSL.
     */
    CARAPI SetEnabledCipherSuites(
        /* [in] */ ArrayOf<String>* cipherSuites);

    /**
     * @return the set of enabled protocols
     */
    CARAPI GetEnabledProtocols(
        /* [out] */ ArrayOf<String>** result);

    /**
     * Sets the set of available protocols for use in SSL connection.
     * @param protocols String[]
     */
    CARAPI SetEnabledProtocols(
        /* [in] */ ArrayOf<String>* protocols);

    /**
     * Tunes the peer holding this parameters to work in client mode.
     * @param   mode if the peer is configured to work in client mode
     */
    CARAPI SetUseClientMode(
        /* [in] */ Boolean mode);

    /**
     * Returns the value indicating if the parameters configured to work
     * in client mode.
     */
    CARAPI GetUseClientMode(
        /* [out] */ Boolean* result);

    /**
     * Tunes the peer holding this parameters to require client authentication
     */
    CARAPI SetNeedClientAuth(
        /* [in] */ Boolean need);

    /**
     * Returns the value indicating if the peer with this parameters tuned
     * to require client authentication
     */
    CARAPI GetNeedClientAuth(
        /* [out] */ Boolean* result);

    /**
     * Tunes the peer holding this parameters to request client authentication
     */
    CARAPI SetWantClientAuth(
        /* [in] */ Boolean want);

    /**
     * Returns the value indicating if the peer with this parameters
     * tuned to request client authentication
     */
    CARAPI GetWantClientAuth(
        /* [out] */ Boolean* result);

    /**
     * Allows/disallows the peer holding this parameters to
     * create new SSL session
     */
    CARAPI SetEnableSessionCreation(
        /* [in] */ Boolean flag);

    /**
     * Returns the value indicating if the peer with this parameters
     * allowed to cteate new SSL session
     */
    CARAPI GetEnableSessionCreation(
        /* [out] */ Boolean* result);

    /**
     * Whether connections using this SSL connection should use the TLS
     * extension Server Name Indication (SNI).
     */
    CARAPI SetUseSni(
        /* [in] */ Boolean flag);

    /**
     * Returns whether connections using this SSL connection should use the TLS
     * extension Server Name Indication (SNI).
     */
    CARAPI GetUseSni(
        /* [out] */ Boolean* result);

    static CARAPI EncodeIssuerX509Principals(
        /* [in] */ ArrayOf<IX509Certificate*>* certificates,
        /* [out] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >** result);

    CARAPI GetSessionToReuse(
        /* [in] */ Int64 sslNativePointer,
        /* [in] */ const String& hostname,
        /* [in] */ Int32 port,
        /* [out] */ IOpenSSLSessionImpl** result);

    CARAPI SetTlsChannelId(
        /* [in] */ Int64 sslNativePointer,
        /* [in] */ IOpenSSLKey* channelIdPrivateKey);

    CARAPI SetCertificate(
        /* [in] */ Int64 sslNativePointer,
        /* [in] */ const String& alias);

    CARAPI SetSSLParameters(
        /* [in] */ Int64 sslCtxNativePointer,
        /* [in] */ Int64 sslNativePointer,
        /* [in] */ ISSLParametersImplAliasChooser* chooser,
        /* [in] */ ISSLParametersImplPSKCallbacks* pskCallbacks,
        /* [in] */ const String& hostname);

    CARAPI SetCertificateValidation(
        /* [in] */ Int64 sslNativePointer);

    CARAPI SetupSession(
        /* [in] */ Int64 sslSessionNativePointer,
        /* [in] */ Int64 sslNativePointer,
        /* [in] */ IOpenSSLSessionImpl* sessionToReuse,
        /* [in] */ const String& hostname,
        /* [in] */ Int32 port,
        /* [in] */ Boolean handshakeCompleted,
        /* [out] */ IOpenSSLSessionImpl** result);

    CARAPI ChooseClientCertificate(
        /* [in] */ ArrayOf<Byte>* keyTypeBytes,
        /* [in] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >* asn1DerEncodedPrincipals,
        /* [in] */ Int64 sslNativePointer,
        /* [in] */ ISSLParametersImplAliasChooser* chooser);

    /**
     * @see NativeCrypto.SSLHandshakeCallbacks#clientPSKKeyRequested(String, byte[], byte[])
     */
    CARAPI ClientPSKKeyRequested(
        /* [in] */ const String& identityHint,
        /* [in] */ ArrayOf<Byte>* identityBytesOut,
        /* [in] */ ArrayOf<Byte>* key,
        /* [in] */ ISSLParametersImplPSKCallbacks* pskCallbacks,
        /* [out] */ Int32* result);

    /**
     * @see NativeCrypto.SSLHandshakeCallbacks#serverPSKKeyRequested(String, String, byte[])
     */
    CARAPI ServerPSKKeyRequested(
        /* [in] */ const String& identityHint,
        /* [in] */ const String& identity,
        /* [in] */ ArrayOf<Byte>* key,
        /* [in] */ ISSLParametersImplPSKCallbacks* pskCallbacks,
        /* [out] */ Int32* result);

    /**
     * Gets the suitable session reference from the session cache container.
     */
    CARAPI GetCachedClientSession(
        /* [in] */ IClientSessionContext* sessionContext,
        /* [in] */ const String& hostName,
        /* [in] */ Int32 port,
        /* [out] */ IOpenSSLSessionImpl** result);

    /**
     * Returns the clone of this object.
     * @return the clone.
     */
    CARAPI Clone(
        /* [out] */ IInterface** result);

    /**
     * Gets the default X.509 trust manager.
     *
     * TODO: Move this to a published API under dalvik.system.
     */
    static CARAPI GetDefaultX509TrustManager(
        /* [out] */ IX509TrustManager** result);

    CARAPI GetEndpointIdentificationAlgorithm(
        /* [out] */ String* result);

    CARAPI SetEndpointIdentificationAlgorithm(
        /* [in] */ const String& endpointIdentificationAlgorithm);

    /**
     * Similar to getServerKeyType, but returns value given TLS
     * ClientCertificateType byte values from a CertificateRequest
     * message for use with X509KeyManager.chooseClientAlias or
     * X509ExtendedKeyManager.chooseEngineClientAlias.
     */
    static CARAPI GetClientKeyType(
        /* [in] */ Byte keyType,
        /* [out] */ String* result);

private:
    /**
     * Return a possibly null array of X509Certificates given the possibly null
     * array of DER encoded bytes.
     */
    static AutoPtr<ArrayOf<IOpenSSLX509Certificate*> > CreateCertChain(
        /* [in] */ ArrayOf<Int64>* certificateRefs);

    static AutoPtr<IX509KeyManager> GetDefaultX509KeyManager();

    static AutoPtr<IX509KeyManager> CreateDefaultX509KeyManager();

    /**
     * Finds the first {@link X509KeyManager} element in the provided array.
     *
     * @return the first {@code X509KeyManager} or {@code null} if not found.
     */
    static AutoPtr<IX509KeyManager> FindFirstX509KeyManager(
        /* [in] */ ArrayOf<IKeyManager*>* kms);

    /**
     * Finds the first {@link PSKKeyManager} element in the provided array.
     *
     * @return the first {@code PSKKeyManager} or {@code null} if not found.
     */
    static AutoPtr<IPSKKeyManager> FindFirstPSKKeyManager(
        /* [in] */ ArrayOf<IKeyManager*>* kms);

    static AutoPtr<IX509TrustManager> CreateDefaultX509TrustManager();

    /**
     * Finds the first {@link X509TrustManager} element in the provided array.
     *
     * @return the first {@code X509TrustManager} or {@code null} if not found.
     */
    static AutoPtr<IX509TrustManager> FindFirstX509TrustManager(
        /* [in] */ ArrayOf<ITrustManager*>* tms);

    /**
     * Returns key type constant suitable for calling X509KeyManager.chooseServerAlias or
     * X509ExtendedKeyManager.chooseEngineServerAlias. Returns {@code null} for key exchanges that
     * do not use X.509 for server authentication.
     */
    static String GetServerX509KeyType(
        /* [in] */ Int64 sslCipherNative);

    static AutoPtr<ArrayOf<String> > GetDefaultCipherSuites(
        /* [in] */ Boolean x509CipherSuitesNeeded,
        /* [in] */ Boolean pskCipherSuitesNeeded);

    static AutoPtr<ArrayOf<String> > GetDefaultProtocols();

    static AutoPtr<ArrayOf<String> > Concat(
        /* [in] */ ArrayOf<AutoPtr<ArrayOf<String> > >* arrays);

public:
    AutoPtr<ArrayOf<Byte> > mNpnProtocols;
    AutoPtr<ArrayOf<Byte> > mAlpnProtocols;
    Boolean mUseSessionTickets;
    Boolean mUseSni;

    /**
     * Whether the TLS Channel ID extension is enabled. This field is
     * server-side only.
     */
    Boolean mChannelIdEnabled;

private:
    // default source of X.509 certificate based authentication keys
    static /* volatile */ AutoPtr<IX509KeyManager> sDefaultX509KeyManager;
    // default source of X.509 certificate based authentication trust decisions
    static /* volatile */ AutoPtr<IX509TrustManager> sDefaultX509TrustManager;
    // default source of random numbers
    static /* volatile */ AutoPtr<ISecureRandom> sDefaultSecureRandom;
    // default SSL parameters
    static /* volatile */ AutoPtr<ISSLParametersImpl> sDefaultParameters;

    // client session context contains the set of reusable
    // client-side SSL sessions
    AutoPtr<IClientSessionContext> mClientSessionContext;
    // server session context contains the set of reusable
    // server-side SSL sessions
    AutoPtr<IServerSessionContext> mServerSessionContext;
    // source of X.509 certificate based authentication keys or null if not provided
    AutoPtr<IX509KeyManager> mX509KeyManager;
    // source of Pre-Shared Key (PSK) authentication keys or null if not provided.
    AutoPtr<IPSKKeyManager> mPskKeyManager;
    // source of X.509 certificate based authentication trust decisions or null if not provided
    AutoPtr<IX509TrustManager> mX509TrustManager;
    // source of random numbers
    AutoPtr<ISecureRandom> mSecureRandom;

    // protocols enabled for SSL connection
    AutoPtr<ArrayOf<String> > mEnabledProtocols;
    // cipher suites enabled for SSL connection
    AutoPtr<ArrayOf<String> > mEnabledCipherSuites;

    // if the peer with this parameters tuned to work in client mode
    Boolean mClient_mode;
    // if the peer with this parameters tuned to require client authentication
    Boolean mNeed_client_auth;
    // if the peer with this parameters tuned to request client authentication
    Boolean mWant_client_auth;
    // if the peer with this parameters allowed to cteate new SSL session
    Boolean mEnable_session_creation;
    String mEndpointIdentificationAlgorithm;

    /** Key type: RSA. */
    static String KEY_TYPE_RSA;

    /** Key type: DSA. */
    static String KEY_TYPE_DSA;

    /** Key type: Diffie-Hellman with RSA signature. */
    static String KEY_TYPE_DH_RSA;

    /** Key type: Diffie-Hellman with DSA signature. */
    static String KEY_TYPE_DH_DSA;

    /** Key type: Elliptic Curve. */
    static String KEY_TYPE_EC;

    /** Key type: Eliiptic Curve with ECDSA signature. */
    static String KEY_TYPE_EC_EC;

    /** Key type: Eliiptic Curve with RSA signature. */
    static String KEY_TYPE_EC_RSA;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_SSLPARAMETERSIMPL_H__