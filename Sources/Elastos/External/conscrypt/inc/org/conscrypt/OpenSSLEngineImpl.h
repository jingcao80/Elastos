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

#ifndef __ORG_CONSCRYPT_OPENSSLENGINEIMPL_H__
#define __ORG_CONSCRYPT_OPENSSLENGINEIMPL_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Security.h>
#include <elastos/core/Object.h>
#include "_Org.Conscrypt.h"
#include "elastosx/net/ssl/SSLEngine.h"
#include "org/conscrypt/SSLParametersImpl.h"

using Elastos::Core::IRunnable;
using Elastos::IO::IByteBuffer;
using Elastosx::Crypto::ISecretKey;
using Elastosx::Net::Ssl::ISSLEngineResult;
using Elastosx::Net::Ssl::ISSLSession;
using Elastosx::Net::Ssl::IX509KeyManager;
using Elastosx::Net::Ssl::SSLEngine;
using Elastosx::Net::Ssl::SSLEngineResult_HandshakeStatus;
using Elastosx::Security::Auth::X500::IX500Principal;

namespace Org {
namespace Conscrypt {

/**
 * Implements the {@link SSLEngine} API using OpenSSL's non-blocking interfaces.
 */
class OpenSSLEngineImpl
    : public ISSLHandshakeCallbacks
    , public ISSLParametersImplAliasChooser
    , public ISSLParametersImplPSKCallbacks
    , public SSLEngine
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLEngineImpl();

    ~OpenSSLEngineImpl();

    CARAPI constructor(
        /* [in] */ ISSLParametersImpl* sslParameters);

    CARAPI constructor(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ ISSLParametersImpl* sslParameters);

    // @Override
    CARAPI BeginHandshake();

    // @Override
    CARAPI CloseInbound();

    // @Override
    CARAPI CloseOutbound();

    // @Override
    CARAPI GetDelegatedTask(
        /* [out] */ IRunnable** result);

    // @Override
    CARAPI GetEnabledCipherSuites(
        /* [out, callee] */ ArrayOf<String>** result);

    // @Override
    CARAPI GetEnabledProtocols(
        /* [out, callee] */ ArrayOf<String>** result);

    // @Override
    CARAPI GetEnableSessionCreation(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetHandshakeStatus(
        /* [out] */ SSLEngineResult_HandshakeStatus* result);

    // @Override
    CARAPI GetNeedClientAuth(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetSession(
        /* [out] */ ISSLSession** result);

    // @Override
    CARAPI GetSupportedCipherSuites(
        /* [out, callee] */ ArrayOf<String>** result);

    // @Override
    CARAPI GetSupportedProtocols(
        /* [out, callee] */ ArrayOf<String>** result);

    // @Override
    CARAPI GetUseClientMode(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetWantClientAuth(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsInboundDone(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsOutboundDone(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetEnabledCipherSuites(
        /* [in] */ ArrayOf<String>* suites);

    // @Override
    CARAPI SetEnabledProtocols(
        /* [in] */ ArrayOf<String>* protocols);

    // @Override
    CARAPI SetEnableSessionCreation(
        /* [in] */ Boolean flag);

    // @Override
    CARAPI SetNeedClientAuth(
        /* [in] */ Boolean need);

    // @Override
    CARAPI SetUseClientMode(
        /* [in] */ Boolean mode);

    // @Override
    CARAPI SetWantClientAuth(
        /* [in] */ Boolean want);

    // @Override
    CARAPI Unwrap(
        /* [in] */ IByteBuffer* src,
        /* [in] */ ArrayOf<IByteBuffer*>* dsts,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ ISSLEngineResult** result);

    // @Override
    CARAPI Wrap(
        /* [in] */ ArrayOf<IByteBuffer*>* srcs,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [in] */ IByteBuffer* dst,
        /* [out] */ ISSLEngineResult** result);

    // @Override
    CARAPI ClientPSKKeyRequested(
        /* [in] */ const String& identityHint,
        /* [in] */ ArrayOf<Byte>* identity,
        /* [in] */ ArrayOf<Byte>* key,
        /* [out] */ Int32* result);

    // @Override
    CARAPI ServerPSKKeyRequested(
        /* [in] */ const String& identityHint,
        /* [in] */ const String& identity,
        /* [in] */ ArrayOf<Byte>* key,
        /* [out] */ Int32* result);

    // @Override
    CARAPI OnSSLStateChange(
        /* [in] */ Int64 sslSessionNativePtr,
        /* [in] */ Int32 type,
        /* [in] */ Int32 val);

    // @Override
    CARAPI VerifyCertificateChain(
        /* [in] */ Int64 sslSessionNativePtr,
        /* [in] */ ArrayOf<Int64>* certRefs,
        /* [in] */ const String& authMethod);

    // @Override
    CARAPI ClientCertificateRequested(
        /* [in] */ ArrayOf<Byte>* keyTypes,
        /* [in] */ ArrayOf<Handle32>* asn1DerEncodedX500Principals);

    // @Override
    CARAPI ChooseServerAlias(
        /* [in] */ IX509KeyManager* keyManager,
        /* [in] */ const String& keyType,
        /* [out] */ String* result);

    // @Override
    CARAPI ChooseClientAlias(
        /* [in] */ IX509KeyManager* keyManager,
        /* [in] */ ArrayOf<IX500Principal*>* issuer,
        /* [in] */ ArrayOf<String>* keyTypes,
        /* [out] */ String* results);

    // @Override
    CARAPI ChooseServerPSKIdentityHint(
        /* [in] */ IPSKKeyManager* keyManager,
        /* [out] */ String* result);

    // @Override
    CARAPI ChooseClientPSKIdentity(
        /* [in] */ IPSKKeyManager* keyManager,
        /* [in] */ const String& identityHint,
        /* [out] */ String* result);

    // @Override
    CARAPI GetPSKKey(
        /* [in] */ IPSKKeyManager* keyManager,
        /* [in] */ const String& identityHint,
        /* [in] */ const String& identity,
        /* [out] */ ISecretKey** result);

private:
    static CARAPI CheckIndex(
        /* [in] */ Int32 length,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    /** Returns the next non-empty ByteBuffer. */
    CARAPI_(AutoPtr<IByteBuffer>) GetNextAvailableByteBuffer(
        /* [in] */ ArrayOf<IByteBuffer*>* buffers,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    /** Writes data available in a BIO sink to a ByteBuffer. */
    static CARAPI_(Int32) WriteSinkToByteBuffer(
        /* [in] */ IOpenSSLBIOSink* sink,
        /* [in] */ IByteBuffer* dst);

    CARAPI_(void) Shutdown();

    CARAPI_(void) ShutdownAndFreeSslNative();

    CARAPI_(void) Free();

public:
    /**
     * Private key for the TLS Channel ID extension. This field is client-side
     * only. Set during startHandshake.
     */
    AutoPtr<IOpenSSLKey> mChannelIdPrivateKey;

private:
    AutoPtr<SSLParametersImpl> mSslParameters;

    /**
     * Protects handshakeStarted and handshakeCompleted.
     */
    Object mStateLock; // = new Object();

    enum EngineState {
        /**
         * The {@link OpenSSLSocketImpl} object is constructed, but {@link #beginHandshake()}
         * has not yet been called.
         */
        EngineState_NEW,
        /**
         * {@link #setUseClientMode(boolean)} has been called at least once.
         */
        EngineState_MODE_SET,
        /**
         * {@link #beginHandshake()} has been called at least once.
         */
        EngineState_HANDSHAKE_WANTED,
        /**
         * Handshake task has been started.
         */
        EngineState_HANDSHAKE_STARTED,
        /**
         * Handshake has been completed, but {@link #beginHandshake()} hasn't returned yet.
         */
        EngineState_HANDSHAKE_COMPLETED,
        /**
         * {@link #beginHandshake()} has completed but the task hasn't
         * been called. This is expected behaviour in cut-through mode, where SSL_do_handshake
         * returns before the handshake is complete. We can now start writing data to the socket.
         */
        EngineState_READY_HANDSHAKE_CUT_THROUGH,
        /**
         * {@link #beginHandshake()} has completed and socket is ready to go.
         */
        EngineState_READY,
        EngineState_CLOSED_INBOUND,
        EngineState_CLOSED_OUTBOUND,
        /**
         * Inbound and outbound has been called.
         */
        EngineState_CLOSED,
    };

    // @GuardedBy("stateLock");
    EngineState mEngineState; // = EngineState.NEW;

    /**
     * Protected by synchronizing on stateLock. Starts as 0, set by
     * startHandshake, reset to 0 on close.
     */
    // @GuardedBy("stateLock");
    Int64 mSslNativePointer;

    /** Used during handshake when {@link #wrap(ByteBuffer, ByteBuffer)} is called. */
    // TODO: make this use something similar to BIO_s_null() in native code
    static AutoPtr<IOpenSSLBIOSource> sNullSource; // = OpenSSLBIOSource.wrap(ByteBuffer.allocate(0));

    /** A BIO sink written to only during handshakes. */
    AutoPtr<IOpenSSLBIOSink> mHandshakeSink;

    /** A BIO sink written to during regular operation. */
    AutoPtr<IOpenSSLBIOSink> mLocalToRemoteSink; // = OpenSSLBIOSink.create();

    /** Set during startHandshake. */
    AutoPtr<IOpenSSLSessionImpl> mSslSession;

    /** Used during handshake callbacks. */
    AutoPtr<IOpenSSLSessionImpl> mHandshakeSession;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLENGINEIMPL_H__
