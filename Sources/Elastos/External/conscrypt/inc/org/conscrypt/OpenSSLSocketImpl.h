
#ifndef __ORG_CONSCRYPT_OPENSSLSOCKERIMPL_H__
#define __ORG_CONSCRYPT_OPENSSLSOCKERIMPL_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Net.h"
#include "Elastos.CoreLibrary.Security.h"
#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>
#include <elastos/io/InputStream.h>
#include <elastos/io/OutputStream.h>

using Elastos::Core::ICloseGuard;
using Elastos::IO::InputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::OutputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::IFileDescriptor;
using Elastos::Net::IInetAddress;
using Elastos::Net::ISocket;
using Elastos::Security::IPrivateKey;
using Elastos::Utility::IArrayList;
using Elastosx::Crypto::ISecretKey;
using Elastosx::Security::Auth::X500::IX500Principal;
using Elastosx::Net::Ssl::ISSLSession;
using Elastosx::Net::Ssl::IHandshakeCompletedListener;
using Elastosx::Net::Ssl::IX509KeyManager;

namespace Org {
namespace Conscrypt {

/**
 * Implementation of the class OpenSSLSocketImpl based on OpenSSL.
 * <p>
 * Extensions to SSLSocket include:
 * <ul>
 * <li>handshake timeout
 * <li>session tickets
 * <li>Server Name Indication
 * </ul>
 */
class OpenSSLSocketImpl
    : public Object // public SSLSocket
    , public ISSLHandshakeCallbacks
    , public ISSLParametersImplAliasChooser
    , public ISSLParametersImplPSKCallbacks
    , public IOpenSSLSocketImpl
{
private:
    /**
     * This inner class provides input data stream functionality
     * for the OpenSSL native implementation. It is used to
     * read data received via SSL protocol.
     */
    class SSLInputStream
        : public InputStream
    {
    public:
        SSLInputStream(
            /* [in] */ OpenSSLSocketImpl* host);

        /**
         * Reads one byte. If there is no data in the underlying buffer,
         * this operation can block until the data will be
         * available.
         * @return read value.
         * @throws IOException
         */
        CARAPI Read(
            /* [out] */ Int32* result);

        /**
         * Method acts as described in spec for superclass.
         * @see java.io.InputStream#read(byte[],int,int)
         */
        CARAPI Read(
            /* [in] */ ArrayOf<Byte>* buf,
            /* [in] */ Int32 offset,
            /* [in] */ Int32 byteCount,
            /* [out] */ Int32* result);

        CARAPI AwaitPendingOps();

    private:
        /**
         * OpenSSL only lets one thread read at a time, so this is used to
         * make sure we serialize callers of SSL_read. Thread is already
         * expected to have completed handshaking.
         */
        Object mReadLock;
        OpenSSLSocketImpl* mHost;
    };

    /**
     * This inner class provides output data stream functionality
     * for the OpenSSL native implementation. It is used to
     * write data according to the encryption parameters given in SSL context.
     */
    class SSLOutputStream
        : public OutputStream
    {
    public:
        SSLOutputStream(
            /* [in] */ OpenSSLSocketImpl* host);

        /**
         * Method acts as described in spec for superclass.
         * @see java.io.OutputStream#write(int)
         */
        CARAPI Write(
            /* [in] */ Int32 oneByte);

        /**
         * Method acts as described in spec for superclass.
         * @see java.io.OutputStream#write(byte[],int,int)
         */
        CARAPI Write(
            /* [in] */ ArrayOf<Byte>* buf,
            /* [in] */ Int32 offset,
            /* [in] */ Int32 byteCount);

        CARAPI AwaitPendingOps();

    private:
        /**
         * OpenSSL only lets one thread write at a time, so this is used
         * to make sure we serialize callers of SSL_write. Thread is
         * already expected to have completed handshaking.
         */
        Object mWriteLock;
        OpenSSLSocketImpl* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    OpenSSLSocketImpl();

    CARAPI constructor(
        /* [in] */ ISSLParametersImpl* sslParameters);

    CARAPI constructor(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ ISSLParametersImpl* sslParameters);

    CARAPI constructor(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 port,
        /* [in] */ ISSLParametersImpl* sslParameters);

    CARAPI constructor(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ IInetAddress* clientAddress,
        /* [in] */ Int32 clientPort,
        /* [in] */ ISSLParametersImpl* sslParameters);

    CARAPI constructor(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 port,
        /* [in] */ IInetAddress* clientAddress,
        /* [in] */ Int32 clientPort,
        /* [in] */ ISSLParametersImpl* sslParameters);

    /**
     * Create an SSL socket that wraps another socket. Invoked by
     * OpenSSLSocketImplWrapper constructor.
     */
    CARAPI constructor(
        /* [in] */ ISocket* socket,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ Boolean autoClose,
        /* [in] */ ISSLParametersImpl* sslParameters);

    /**
     * Starts a TLS/SSL handshake on this connection using some native methods
     * from the OpenSSL library. It can negotiate new encryption keys, change
     * cipher suites, or initiate a new session. The certificate chain is
     * verified if the correspondent property in java.Security is set. All
     * listeners are notified at the end of the TLS/SSL handshake.
     */
    CARAPI StartHandshake();

    CARAPI GetPeerHostName(
        /* [out] */ String* result);

    CARAPI GetPeerPort(
        /* [out] */ Int32* result);

    CARAPI ClientCertificateRequested(
        /* [in] */ ArrayOf<Byte>* keyTypeBytes,
        /* [in] */ ArrayOf<Handle32>* asn1DerEncodedPrincipals);

    CARAPI ClientPSKKeyRequested(
        /* [in] */ const String& identityHint,
        /* [in] */ ArrayOf<Byte>* identity,
        /* [in] */ ArrayOf<Byte>* key,
        /* [out] */ Int32* result);

    CARAPI ServerPSKKeyRequested(
        /* [in] */ const String& identityHint,
        /* [in] */ const String& identity,
        /* [in] */ ArrayOf<Byte>* key,
        /* [out] */ Int32* result);

    CARAPI OnSSLStateChange(
        /* [in] */ Int64 sslSessionNativePtr,
        /* [in] */ Int32 type,
        /* [in] */ Int32 val);

    CARAPI VerifyCertificateChain(
        /* [in] */ Int64 sslSessionNativePtr,
        /* [in] */ ArrayOf<Int64>* certRefs,
        /* [in] */ const String& authMethod);

    CARAPI GetInputStream(
        /* [out] */ IInputStream** result);

    CARAPI GetOutputStream(
        /* [out] */ IOutputStream** result);

    CARAPI GetSession(
        /* [out] */ ISSLSession** result);

    CARAPI AddHandshakeCompletedListener(
        /* [in] */ IHandshakeCompletedListener* listener);

    CARAPI RemoveHandshakeCompletedListener(
        /* [in] */ IHandshakeCompletedListener* listener);

    CARAPI GetEnableSessionCreation(
        /* [out] */ Boolean* result);

    CARAPI SetEnableSessionCreation(
        /* [in] */ Boolean flag);

    CARAPI GetSupportedCipherSuites(
        /* [out] */ ArrayOf<String>** result);

    CARAPI GetEnabledCipherSuites(
        /* [out] */ ArrayOf<String>** result);

    CARAPI SetEnabledCipherSuites(
        /* [in] */ ArrayOf<String>* suites);

    CARAPI GetSupportedProtocols(
        /* [out] */ ArrayOf<String>** result);

    CARAPI GetEnabledProtocols(
        /* [out] */ ArrayOf<String>** result);

    CARAPI SetEnabledProtocols(
        /* [in] */ ArrayOf<String>* protocols);

    /**
     * This method enables session ticket support.
     *
     * @param useSessionTickets True to enable session tickets
     */
    CARAPI SetUseSessionTickets(
        /* [in] */ Boolean useSessionTickets);

    /**
     * This method enables Server Name Indication
     *
     * @param hostname the desired SNI hostname, or null to disable
     */
    CARAPI SetHostname(
        /* [in] */ const String& hostname);

    /**
     * Enables/disables TLS Channel ID for this server socket.
     *
     * <p>This method needs to be invoked before the handshake starts.
     *
     * @throws IllegalStateException if this is a client socket or if the handshake has already
     *         started.

     */
    CARAPI SetChannelIdEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Gets the TLS Channel ID for this server socket. Channel ID is only available once the
     * handshake completes.
     *
     * @return channel ID or {@code null} if not available.
     *
     * @throws IllegalStateException if this is a client socket or if the handshake has not yet
     *         completed.
     * @throws SSLException if channel ID is available but could not be obtained.
     */
    CARAPI GetChannelId(
        /* [out] */ ArrayOf<Byte>** result);

    /**
     * Sets the {@link PrivateKey} to be used for TLS Channel ID by this client socket.
     *
     * <p>This method needs to be invoked before the handshake starts.
     *
     * @param privateKey private key (enables TLS Channel ID) or {@code null} for no key (disables
     *        TLS Channel ID). The private key must be an Elliptic Curve (EC) key based on the NIST
     *        P-256 curve (aka SECG secp256r1 or ANSI X9.62 prime256v1).
     *
     * @throws IllegalStateException if this is a server socket or if the handshake has already
     *         started.
     */
    CARAPI SetChannelIdPrivateKey(
        /* [in] */ IPrivateKey* privateKey);

    CARAPI GetUseClientMode(
        /* [out] */ Boolean* result);

    CARAPI SetUseClientMode(
        /* [in] */ Boolean mode);

    CARAPI GetWantClientAuth(
        /* [out] */ Boolean* result);

    CARAPI GetNeedClientAuth(
        /* [out] */ Boolean* result);

    CARAPI SetNeedClientAuth(
        /* [in] */ Boolean need);

    CARAPI SetWantClientAuth(
        /* [in] */ Boolean want);

    CARAPI SendUrgentData(
        /* [in] */ Int32 data);

    CARAPI SetOOBInline(
        /* [in] */ Boolean on);

    CARAPI SetSoTimeout(
        /* [in] */ Int32 readTimeoutMilliseconds);

    CARAPI GetSoTimeout(
        /* [out] */ Int32* result);

    /**
     * Note write timeouts are not part of the javax.net.ssl.SSLSocket API
     */
    CARAPI SetSoWriteTimeout(
        /* [in] */ Int32 writeTimeoutMilliseconds);

    /**
     * Note write timeouts are not part of the javax.net.ssl.SSLSocket API
     */
    CARAPI GetSoWriteTimeout(
        /* [out] */ Int32* result);

    /**
     * Set the handshake timeout on this socket.  This timeout is specified in
     * milliseconds and will be used only during the handshake process.
     */
    CARAPI SetHandshakeTimeout(
        /* [in] */ Int32 handshakeTimeoutMilliseconds);

    CARAPI Close();

    CARAPI Finalize();

    /* @Override */
    CARAPI GetFileDescriptor$(
        /* [out] */ IFileDescriptor** result);

    /**
     * Returns the protocol agreed upon by client and server, or null if no
     * protocol was agreed upon.
     */
    CARAPI GetNpnSelectedProtocol(
        /* [out] */ ArrayOf<Byte>** result);

    /**
     * Returns the protocol agreed upon by client and server, or {@code null} if
     * no protocol was agreed upon.
     */
    CARAPI GetAlpnSelectedProtocol(
        /* [out] */ ArrayOf<Byte>** result);

    /**
     * Sets the list of protocols this peer is interested in. If null no
     * protocols will be used.
     *
     * @param npnProtocols a non-empty array of protocol names. From
     *     SSL_select_next_proto, "vector of 8-bit, length prefixed byte
     *     strings. The length byte itself is not included in the length. A byte
     *     string of length 0 is invalid. No byte string may be truncated.".
     */
    CARAPI SetNpnProtocols(
        /* [in] */ ArrayOf<Byte>* npnProtocols);

    /**
     * Sets the list of protocols this peer is interested in. If the list is
     * {@code null}, no protocols will be used.
     *
     * @param alpnProtocols a non-empty array of protocol names. From
     *            SSL_select_next_proto, "vector of 8-bit, length prefixed byte
     *            strings. The length byte itself is not included in the length.
     *            A byte string of length 0 is invalid. No byte string may be
     *            truncated.".
     */
    CARAPI SetAlpnProtocols(
        /* [in] */ ArrayOf<Byte>* alpnProtocols);

    CARAPI ChooseServerAlias(
        /* [in] */ IX509KeyManager* keyManager,
        /* [in] */ const String& keyType,
        /* [out] */ String* result);

    CARAPI ChooseClientAlias(
        /* [in] */ IX509KeyManager* keyManager,
        /* [in] */ ArrayOf<IX500Principal*>* issuers,
        /* [in] */ ArrayOf<String>* keyTypes,
        /* [out] */ String* result);

    CARAPI ChooseServerPSKIdentityHint(
        /* [in] */ IPSKKeyManager* keyManager,
        /* [out] */ String* result);

    CARAPI ChooseClientPSKIdentity(
        /* [in] */ IPSKKeyManager* keyManager,
        /* [in] */ const String& identityHint,
        /* [out] */ String* result);

    CARAPI GetPSKKey(
        /* [in] */ IPSKKeyManager* keyManager,
        /* [in] */ const String& identityHint,
        /* [in] */ const String& identity,
        /* [out] */ ISecretKey** result);

protected:
    void CheckOpen();

    void NotifyHandshakeCompletedListeners();

    void AssertReadableOrWriteableState();

    void WaitForHandshake();

    void ShutdownAndFreeSslNative();

    void CloseUnderlyingSocket();

    void Free();

private:
    static Boolean DBG_STATE;

    /**
     * Protects handshakeStarted and handshakeCompleted.
     */
    Object mStateLock;

    /**
     * The {@link OpenSSLSocketImpl} object is constructed, but {@link #startHandshake()}
     * has not yet been called.
     */
    static Int32 STATE_NEW;

    /**
     * {@link #startHandshake()} has been called at least once.
     */
    static Int32 STATE_HANDSHAKE_STARTED;

    /**
     * {@link #handshakeCompleted()} has been called, but {@link #startHandshake()} hasn't
     * returned yet.
     */
    static Int32 STATE_HANDSHAKE_COMPLETED;

    /**
     * {@link #startHandshake()} has completed but {@link #handshakeCompleted()} hasn't
     * been called. This is expected behaviour in cut-through mode, where SSL_do_handshake
     * returns before the handshake is complete. We can now start writing data to the socket.
     */
    static Int32 STATE_READY_HANDSHAKE_CUT_THROUGH;

    /**
     * {@link #startHandshake()} has completed and {@link #handshakeCompleted()} has been
     * called.
     */
    static Int32 STATE_READY;

    /**
     * {@link #close()} has been called at least once.
     */
    static Int32 STATE_CLOSED;

    // @GuardedBy("stateLock");
    Int32 mState;

    /**
     * Protected by synchronizing on stateLock. Starts as 0, set by
     * startHandshake, reset to 0 on close.
     */
    // @GuardedBy("stateLock");
    Int64 mSslNativePointer;

    /**
     * Protected by synchronizing on stateLock. Starts as null, set by
     * getInputStream.
     */
    // @GuardedBy("stateLock");
    AutoPtr<SSLInputStream> mIs;

    /**
     * Protected by synchronizing on stateLock. Starts as null, set by
     * getInputStream.
     */
    // @GuardedBy("stateLock");
    AutoPtr<SSLOutputStream> mOs;

    AutoPtr<ISocket> mSocket;
    Boolean mAutoClose;
    String mWrappedHost;
    Int32 mWrappedPort;
    AutoPtr<ISSLParametersImpl> mSslParameters;
    AutoPtr<ICloseGuard> mGuard;

    AutoPtr<IArrayList> mListeners;

    /**
     * Private key for the TLS Channel ID extension. This field is client-side
     * only. Set during startHandshake.
     */
    AutoPtr<IOpenSSLKey> mChannelIdPrivateKey;

    /** Set during startHandshake. */
    AutoPtr<IOpenSSLSessionImpl> mSslSession;

    /** Used during handshake callbacks. */
    AutoPtr<IOpenSSLSessionImpl> mHandshakeSession;

    /**
     * Local cache of timeout to avoid getsockopt on every read and
     * write for non-wrapped sockets. Note that
     * OpenSSLSocketImplWrapper overrides setSoTimeout and
     * getSoTimeout to delegate to the wrapped socket.
     */
    Int32 mReadTimeoutMilliseconds;
    Int32 mWriteTimeoutMilliseconds;

    Int32 mHandshakeTimeoutMilliseconds;  // -1 = same as timeout; 0 = infinite
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLSOCKERIMPL_H__
