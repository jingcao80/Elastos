
#include "OpenSSLSocketImpl.h"
#include "NativeCrypto.h"
#include "Platform.h"
#include "SSLParametersImpl.h"
#include "OpenSSLKey.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/Arrays.h>

using Elastos::Core::ICloseGuardHelper;
using Elastos::Core::CCloseGuardHelper;
using Elastos::IO::ICloseable;
using Elastos::Security::IKey;
using Elastos::Security::ICertificate;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastosx::Net::Ssl::IHandshakeCompletedEvent;
using Elastosx::Net::Ssl::CHandshakeCompletedEvent;
using Elastosx::Net::Ssl::ISSLSocket;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLSocketImpl::SSLInputStream::
//=========================================

OpenSSLSocketImpl::SSLInputStream::SSLInputStream(
    /* [in] */ OpenSSLSocketImpl* host)
    : mHost(host)
{
}

ECode OpenSSLSocketImpl::SSLInputStream::Read(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(1);
    Int32 res = 0;
    Read(buffer, 0, 1, &res);
    *result = (res != -1) ? (*buffer)[0] & 0xff : -1;
    return NOERROR;
}

ECode OpenSSLSocketImpl::SSLInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    assert(0 && "TODO");
    // BlockGuard::GetThreadPolicy()->OnNetwork();

    mHost->CheckOpen();
    Arrays::CheckOffsetAndCount(buf->GetLength(), offset, byteCount);
    if (byteCount == 0) {
        *result = 0;
        return NOERROR;
    }

    {
        AutoLock lock(mReadLock);
        {
            AutoLock lk(mHost->mStateLock);
            if (mHost->mState == STATE_CLOSED) {
                // throw new SocketException("socket is closed");
                return E_SOCKET_EXCEPTION;
            }

            if (DBG_STATE) {
                mHost->AssertReadableOrWriteableState();
            }
        }

        Int32 timeout = 0;
        mHost->GetSoTimeout(&timeout);
        AutoPtr<IFileDescriptor> fd;
        Platform::GetFileDescriptor(mHost->mSocket, (IFileDescriptor**)&fd);
        return NativeCrypto::SSL_read(mHost->mSslNativePointer, fd,
                mHost, buf, offset, byteCount, timeout, result);
    }
}

ECode OpenSSLSocketImpl::SSLInputStream::AwaitPendingOps()
{
    if (DBG_STATE) {
        {
            AutoLock lock(mHost->mStateLock);
            if (mHost->mState != STATE_CLOSED) {
                // throw new AssertionError("State is: " + mState);
                return E_ASSERTION_ERROR;
            }
        }
    }

    {
        AutoLock lock(mReadLock);
    }
    return NOERROR;
}

//=========================================
// OpenSSLSocketImpl::SSLOutputStream::
//=========================================

OpenSSLSocketImpl::SSLOutputStream::SSLOutputStream(
    /* [in] */ OpenSSLSocketImpl* host)
    : mHost(host)
{
}

ECode OpenSSLSocketImpl::SSLOutputStream::Write(
    /* [in] */ Int32 oneByte)
{
    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(1);
    (*buffer)[0] = (Byte) (oneByte & 0xff);
    return Write(buffer, 0, buffer->GetLength());
}

ECode OpenSSLSocketImpl::SSLOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount)
{
    assert(0 && "TODO");
    // BlockGuard::GetThreadPolicy()->OnNetwork();
    mHost->CheckOpen();
    Arrays::CheckOffsetAndCount(buf->GetLength(), offset, byteCount);
    if (byteCount == 0) {
        return NOERROR;
    }

    {
        AutoLock lock(mWriteLock);
        {
            AutoLock lk(mHost->mStateLock);
            if (mHost->mState == STATE_CLOSED) {
                // throw new SocketException("socket is closed");
                return E_SOCKET_EXCEPTION;
            }

            if (DBG_STATE) {
                mHost->AssertReadableOrWriteableState();
            }
        }

        AutoPtr<IFileDescriptor> fd;
        Platform::GetFileDescriptor(mHost->mSocket, (IFileDescriptor**)&fd);
        NativeCrypto::SSL_write(mHost->mSslNativePointer, fd,
                mHost, buf, offset, byteCount, mHost->mWriteTimeoutMilliseconds);
    }
    return NOERROR;
}

ECode OpenSSLSocketImpl::SSLOutputStream::AwaitPendingOps()
{
    if (DBG_STATE) {
        {
            AutoLock lock(mHost->mStateLock);
            if (mHost->mState != STATE_CLOSED) {
                // throw new AssertionError("State is: " + mState);
                return E_ASSERTION_ERROR;
            }
        }
    }

    {
        AutoLock lock(mWriteLock);
    }
    return NOERROR;
}

//=========================================
// OpenSSLSocketImpl::
//=========================================

Boolean OpenSSLSocketImpl::DBG_STATE = FALSE;

Int32 OpenSSLSocketImpl::STATE_NEW = 0;

Int32 OpenSSLSocketImpl::STATE_HANDSHAKE_STARTED = 1;

Int32 OpenSSLSocketImpl::STATE_HANDSHAKE_COMPLETED = 2;

Int32 OpenSSLSocketImpl::STATE_READY_HANDSHAKE_CUT_THROUGH = 3;

Int32 OpenSSLSocketImpl::STATE_READY = 4;

Int32 OpenSSLSocketImpl::STATE_CLOSED = 5;

CAR_INTERFACE_IMPL_4(OpenSSLSocketImpl, Object, ISSLHandshakeCallbacks, ISSLParametersImplAliasChooser, ISSLParametersImplPSKCallbacks, IOpenSSLSocketImpl)

OpenSSLSocketImpl::OpenSSLSocketImpl()
    : mState(STATE_NEW)
    , mSslNativePointer(0)
    , mAutoClose(FALSE)
    , mWrappedHost(String(NULL))
    , mWrappedPort(0)
    , mReadTimeoutMilliseconds(0)
    , mWriteTimeoutMilliseconds(0)
    , mHandshakeTimeoutMilliseconds(-1)  // -1 = same as timeout; 0 = infinite
{
    AutoPtr<ICloseGuardHelper> hlp;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&hlp);
    hlp->Get((ICloseGuard**)&mGuard);
}

ECode OpenSSLSocketImpl::constructor(
    /* [in] */ ISSLParametersImpl* sslParameters)
{
    mSocket = ISocket::Probe(this);
    mWrappedHost = NULL;
    mWrappedPort = -1;
    mAutoClose = FALSE;
    mSslParameters = sslParameters;
    return NOERROR;
}

ECode OpenSSLSocketImpl::constructor(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ ISSLParametersImpl* sslParameters)
{
    assert(0 && "TODO");
    // SSLSocket::constructor(host, port);
    mSocket = ISocket::Probe(this);
    mWrappedHost = NULL;
    mWrappedPort = -1;
    mAutoClose = FALSE;
    mSslParameters = sslParameters;
    return NOERROR;
}

ECode OpenSSLSocketImpl::constructor(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 port,
    /* [in] */ ISSLParametersImpl* sslParameters)
{
    assert(0 && "TODO");
    // SSLSocket::constructor(address, port);
    mSocket = ISocket::Probe(this);
    mWrappedHost = NULL;
    mWrappedPort = -1;
    mAutoClose = FALSE;
    mSslParameters = sslParameters;
    return NOERROR;
}

ECode OpenSSLSocketImpl::constructor(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ IInetAddress* clientAddress,
    /* [in] */ Int32 clientPort,
    /* [in] */ ISSLParametersImpl* sslParameters)
{
    assert(0 && "TODO");
    // SSLSocket::constructor(host, port, clientAddress, clientPort);
    mSocket = ISocket::Probe(this);
    mWrappedHost = NULL;
    mWrappedPort = -1;
    mAutoClose = FALSE;
    mSslParameters = sslParameters;
    return NOERROR;
}

ECode OpenSSLSocketImpl::constructor(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 port,
    /* [in] */ IInetAddress* clientAddress,
    /* [in] */ Int32 clientPort,
    /* [in] */ ISSLParametersImpl* sslParameters)
{
    assert(0 && "TODO");
    // SSLSocket::constructor(address, port, clientAddress, clientPort);
    mSocket = ISocket::Probe(this);
    mWrappedHost = NULL;
    mWrappedPort = -1;
    mAutoClose = FALSE;
    mSslParameters = sslParameters;
    return NOERROR;
}

ECode OpenSSLSocketImpl::constructor(
    /* [in] */ ISocket* socket,
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ Boolean autoClose,
    /* [in] */ ISSLParametersImpl* sslParameters)
{
    mSocket = socket;
    mWrappedHost = host;
    mWrappedPort = port;
    mAutoClose = autoClose;
    mSslParameters = sslParameters;

    // this.timeout is not set intentionally.
    // OpenSSLSocketImplWrapper.getSoTimeout will delegate timeout
    // to wrapped socket
    return NOERROR;
}

void OpenSSLSocketImpl::CheckOpen()
{
    assert(0 && "TODO");
    // if (IsClosed()) {
    //     // throw new SocketException("Socket is closed");
    //     return;
    // }
}

ECode OpenSSLSocketImpl::StartHandshake()
{
    CheckOpen();
    {
        AutoLock lock(mStateLock);
        if (mState == STATE_NEW) {
            mState = STATE_HANDSHAKE_STARTED;
        }
        else {
            // We've either started the handshake already or have been closed.
            // Do nothing in both cases.
            return NOERROR;
        }
    }

    // note that this modifies the global seed, not something specific to the connection
    Int32 seedLengthInBytes = INativeCrypto::RAND_SEED_LENGTH_IN_BYTES;
    AutoPtr<ISecureRandom> secureRandom;
    mSslParameters->GetSecureRandomMember((ISecureRandom**)&secureRandom);
    if (secureRandom == NULL) {
        Int32 res = 0;
        NativeCrypto::RAND_load_file(String("/dev/urandom"), seedLengthInBytes, &res);
    }
    else {
        AutoPtr<ArrayOf<Byte> > arr;
        secureRandom->GenerateSeed(seedLengthInBytes, (ArrayOf<Byte>**)&arr);
        NativeCrypto::RAND_seed(arr);
    }

    Boolean client = FALSE;
    mSslParameters->GetUseClientMode(&client);

    mSslNativePointer = 0;
    Boolean releaseResources = TRUE;
    // try {
        AutoPtr<IAbstractSessionContext> sessionContext;
        assert(0 && "TODO");
        // mSslParameters->GetSessionContext((IAbstractSessionContext**)&sessionContext);
        Int64 sslCtxNativePointer = 0; // sessionContext->mSslCtxNativePointer;
        NativeCrypto::SSL_new(sslCtxNativePointer, &mSslNativePointer);
        mGuard->Open(String("close"));

        Boolean enableSessionCreation = FALSE;
        GetEnableSessionCreation(&enableSessionCreation);
        if (!enableSessionCreation) {
            NativeCrypto::SSL_set_session_creation_enabled(mSslNativePointer,
                    enableSessionCreation);
        }

        Int32 port = 0;
        GetPeerPort(&port);
        String hostname;
        GetPeerHostName(&hostname);
        AutoPtr<IOpenSSLSessionImpl> sessionToReuse;
        mSslParameters->GetSessionToReuse(
                mSslNativePointer, hostname, port,
                (IOpenSSLSessionImpl**)&sessionToReuse);
        mSslParameters->SetSSLParameters(sslCtxNativePointer, mSslNativePointer, this, this,
                hostname);
        mSslParameters->SetCertificateValidation(mSslNativePointer);
        mSslParameters->SetTlsChannelId(mSslNativePointer, mChannelIdPrivateKey);

        // Temporarily use a different timeout for the handshake process
        Int32 savedReadTimeoutMilliseconds = 0;
        GetSoTimeout(&savedReadTimeoutMilliseconds);
        Int32 savedWriteTimeoutMilliseconds = 0;
        GetSoWriteTimeout(&savedWriteTimeoutMilliseconds);
        if (mHandshakeTimeoutMilliseconds >= 0) {
            SetSoTimeout(mHandshakeTimeoutMilliseconds);
            SetSoWriteTimeout(mHandshakeTimeoutMilliseconds);
        }

        {
            AutoLock lock(mStateLock);
            if (mState == STATE_CLOSED) {
                return NOERROR;
            }
        }

        Int64 sslSessionNativePointer = 0;
        // try {
            AutoPtr<IFileDescriptor> fd;
            Platform::GetFileDescriptor(mSocket, (IFileDescriptor**)&fd);
            AutoPtr<SSLParametersImpl> _mSslParameters = (SSLParametersImpl*)mSslParameters.Get();
            NativeCrypto::SSL_do_handshake(mSslNativePointer,
                    fd, this, savedReadTimeoutMilliseconds, client,
                    _mSslParameters->mNpnProtocols, client ? NULL : _mSslParameters->mAlpnProtocols,
                    &sslSessionNativePointer);
        // } catch (CertificateException e) {
        //     SSLHandshakeException wrapper = new SSLHandshakeException(e.getMessage());
        //     wrapper.initCause(e);
        //     throw wrapper;
        // } catch (SSLException e) {
        //     // Swallow this exception if it's thrown as the result of an interruption.
        //     //
        //     // TODO: SSL_read and SSL_write return -1 when interrupted, but SSL_do_handshake
        //     // will throw the last sslError that it saw before sslSelect, usually SSL_WANT_READ
        //     // (or WANT_WRITE). Catching that exception here doesn't seem much worse than
        //     // changing the native code to return a "special" native pointer value when that
        //     // happens.
        //     synchronized (mStateLock) {
        //         if (mState == STATE_CLOSED) {
        //             return;
        //         }
        //     }

        //     // Write CCS errors to EventLog
        //     String message = e.getMessage();
        //     // Must match error string of SSL_R_UNEXPECTED_CCS
        //     if (message.contains("unexpected CCS")) {
        //         String logMessage = String.format("ssl_unexpected_ccs: host=%s",
        //                 getPeerHostName());
        //         Platform.logEvent(logMessage);
        //     }

        //     throw e;
        // }

        Boolean handshakeCompleted = FALSE;
        {
            AutoLock lock(mStateLock);
            if (mState == STATE_HANDSHAKE_COMPLETED) {
                handshakeCompleted = TRUE;
            }
            else if (mState == STATE_CLOSED) {
                return NOERROR;
            }
        }

        mSslParameters->SetupSession(sslSessionNativePointer, mSslNativePointer,
                sessionToReuse, hostname, port, handshakeCompleted,
                (IOpenSSLSessionImpl**)&mSslSession);

        // Restore the original timeout now that the handshake is complete
        if (mHandshakeTimeoutMilliseconds >= 0) {
            SetSoTimeout(savedReadTimeoutMilliseconds);
            SetSoWriteTimeout(savedWriteTimeoutMilliseconds);
        }

        // if not, notifyHandshakeCompletedListeners later in handshakeCompleted() callback
        if (handshakeCompleted) {
            NotifyHandshakeCompletedListeners();
        }

        {
            AutoLock lock(mStateLock);
            releaseResources = (mState == STATE_CLOSED);

            if (mState == STATE_HANDSHAKE_STARTED) {
                mState = STATE_READY_HANDSHAKE_CUT_THROUGH;
            }
            else if (mState == STATE_HANDSHAKE_COMPLETED) {
                mState = STATE_READY;
            }

            if (!releaseResources) {
                // Unblock threads that are waiting for our state to transition
                // into STATE_READY or STATE_READY_HANDSHAKE_CUT_THROUGH.
                mStateLock.NotifyAll();
            }
        }
    // } catch (SSLProtocolException e) {
    //     throw (SSLHandshakeException) new SSLHandshakeException("Handshake failed")
    //             .initCause(e);
    // } finally {
        // on exceptional exit, treat the socket as closed
        if (releaseResources) {
            {
                AutoLock lock(mStateLock);
                // Mark the socket as closed since we might have reached this as
                // a result on an exception thrown by the handshake process.
                //
                // The state will already be set to closed if we reach this as a result of
                // an early return or an interruption due to a concurrent call to close().
                mState = STATE_CLOSED;
                mStateLock.NotifyAll();
            }

            // try {
                ShutdownAndFreeSslNative();
            // } catch (IOException ignored) {

            // }
        }
    // }
}

ECode OpenSSLSocketImpl::GetPeerHostName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if (mWrappedHost != NULL) {
        *result = mWrappedHost;
        return NOERROR;
    }
    AutoPtr<IInetAddress> inetAddress;
    assert(0 && "TODO");
    // SSLSocket::GetInetAddress((IInetAddress**)&inetAddress);
    if (inetAddress != NULL) {
        return inetAddress->GetHostName(result);
    }
    *result = String(NULL);
    return NOERROR;
}

ECode OpenSSLSocketImpl::GetPeerPort(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    assert(0 && "TODO");
    // *result = mWrappedHost == NULL ? SSLSocket::GetPort() : mWrappedPort;
    return NOERROR;
}

ECode OpenSSLSocketImpl::ClientCertificateRequested(
    /* [in] */ ArrayOf<Byte>* keyTypeBytes,
    /* [in] */ ArrayOf<Handle32>* asn1DerEncodedPrincipals)
{
    assert(0 && "TODO");
    // mSslParameters->ChooseClientCertificate(keyTypeBytes, asn1DerEncodedPrincipals,
    //         mSslNativePointer, this);
    return NOERROR;
}

ECode OpenSSLSocketImpl::ClientPSKKeyRequested(
    /* [in] */ const String& identityHint,
    /* [in] */ ArrayOf<Byte>* identity,
    /* [in] */ ArrayOf<Byte>* key,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mSslParameters->ClientPSKKeyRequested(identityHint, identity, key, this, result);
}

ECode OpenSSLSocketImpl::ServerPSKKeyRequested(
    /* [in] */ const String& identityHint,
    /* [in] */ const String& identity,
    /* [in] */ ArrayOf<Byte>* key,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mSslParameters->ServerPSKKeyRequested(identityHint, identity, key, this, result);
}

ECode OpenSSLSocketImpl::OnSSLStateChange(
    /* [in] */ Int64 sslSessionNativePtr,
    /* [in] */ Int32 type,
    /* [in] */ Int32 val)
{
    if (type != INativeCrypto::SSL_CB_HANDSHAKE_DONE) {
        return NOERROR;
    }

    {
        AutoLock lock(mStateLock);
        if (mState == STATE_HANDSHAKE_STARTED) {
            // If sslSession is NULL, the handshake was completed during
            // the call to NativeCrypto.SSL_do_handshake and not during a
            // later read operation. That means we do not need to fix up
            // the SSLSession and session cache or notify
            // HandshakeCompletedListeners, it will be done in
            // startHandshake.

            mState = STATE_HANDSHAKE_COMPLETED;
            return NOERROR;
        }
        else if (mState == STATE_READY_HANDSHAKE_CUT_THROUGH) {
            // We've returned from startHandshake, which means we've set a sslSession etc.
            // we need to fix them up, which we'll do outside this lock.
        }
        else if (mState == STATE_CLOSED) {
            // Someone called "close" but the handshake hasn't been interrupted yet.
            return NOERROR;
        }
    }

    // reset session id from the native pointer and update the
    // appropriate cache.
    assert(0 && "TODO");
    // mSslSession->ResetId();
    AutoPtr<IAbstractSessionContext> sessionContext;
    Boolean bMode = FALSE;
    mSslParameters->GetUseClientMode(&bMode);
    if (bMode) {
        AutoPtr<IClientSessionContext> res;
        mSslParameters->GetClientSessionContext((IClientSessionContext**)&res);
        sessionContext = IAbstractSessionContext::Probe(res);
    }
    else {
        AutoPtr<IServerSessionContext> res;
        mSslParameters->GetServerSessionContext((IServerSessionContext**)&res);
        sessionContext = IAbstractSessionContext::Probe(res);
    }
    sessionContext->PutSession(ISSLSession::Probe(mSslSession));

    // let listeners know we are finally done
    NotifyHandshakeCompletedListeners();

    {
        AutoLock lock(mStateLock);
        // Now that we've fixed up our state, we can tell waiting threads that
        // we're ready.
        mState = STATE_READY;
        // Notify all threads waiting for the handshake to complete.
        mStateLock.NotifyAll();
    }
    return NOERROR;
}

void OpenSSLSocketImpl::NotifyHandshakeCompletedListeners()
{
    Boolean bEmp = FALSE;
    if (mListeners != NULL && !(mListeners->IsEmpty(&bEmp), bEmp)) {
        // notify the listeners
        AutoPtr<IHandshakeCompletedEvent> event;
        CHandshakeCompletedEvent::New(ISSLSocket::Probe(this), ISSLSession::Probe(mSslSession), (IHandshakeCompletedEvent**)&event);
        Int32 size = 0;
        mListeners->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> p;
            mListeners->Get(i, (IInterface**)&p);
            AutoPtr<IHandshakeCompletedListener> listener = IHandshakeCompletedListener::Probe(p);
            // try {
                listener->HandshakeCompleted(event);
            // } catch (RuntimeException e) {
            //     // The RI runs the handlers in a separate thread,
            //     // which we do not. But we try to preserve their
            //     // behavior of logging a problem and not killing
            //     // the handshaking thread just because a listener
            //     // has a problem.
            //     Thread thread = Thread.currentThread();
            //     thread.getUncaughtExceptionHandler().uncaughtException(thread, e);
            // }
        }
    }
}

ECode OpenSSLSocketImpl::VerifyCertificateChain(
    /* [in] */ Int64 sslSessionNativePtr,
    /* [in] */ ArrayOf<Int64>* certRefs,
    /* [in] */ const String& authMethod)
{
    // try {
        AutoPtr<IX509TrustManager> x509tm;
        mSslParameters->GetX509TrustManager((IX509TrustManager**)&x509tm);
        if (x509tm == NULL) {
            // throw new CertificateException("No X.509 TrustManager");
            return E_CERTIFICATE_EXCEPTION;
        }
        if (certRefs == NULL || certRefs->GetLength() == 0) {
            // throw new SSLException("Peer sent no certificate");
            return E_SSL_EXCEPTION;
        }
        AutoPtr<ArrayOf<IOpenSSLX509Certificate*> > peerCertChain
             = ArrayOf<IOpenSSLX509Certificate*>::Alloc(certRefs->GetLength());
        for (Int32 i = 0; i < certRefs->GetLength(); i++) {
            assert(0 && "TODO");
            // COpenSSLX509Certificate::New((*certRefs)[i], (IOpenSSLX509Certificate**)&((*peerCertChain)[i]));
        }

        // Used for verifyCertificateChain callback
        String hostname;
        GetPeerHostName(&hostname);
        Int32 port = 0;
        GetPeerPort(&port);
        assert(0 && "TODO");
        // COpenSSLSessionImpl::New(sslSessionNativePtr, NULL, peerCertChain,
        //         hostname, port, NULL, (IOpenSSLSessionImpl**)&mHandshakeSession);

        Boolean client = FALSE;
        mSslParameters->GetUseClientMode(&client);
        if (client) {
            assert(0 && "TODO");
            // Platform::CheckServerTrusted(x509tm, peerCertChain, authMethod, hostname);
        }
        else {
            AutoPtr<IPublicKey> pk;
            ICertificate::Probe((*peerCertChain)[0])->GetPublicKey((IPublicKey**)&pk);
            String authType;
            IKey::Probe(pk)->GetAlgorithm(&authType);
            assert(0 && "TODO");
            // x509tm->CheckClientTrusted(peerCertChain, authType);
        }
    // } catch (CertificateException e) {
    //     throw e;
    // } catch (Exception e) {
    //     throw new CertificateException(e);
    // } finally {
        // Clear this before notifying handshake completed listeners
        mHandshakeSession = NULL;
    // }
    return NOERROR;
}

ECode OpenSSLSocketImpl::GetInputStream(
    /* [out] */ IInputStream** result)
{
    VALIDATE_NOT_NULL(result)
    CheckOpen();

    AutoPtr<IInputStream> returnVal;
    {
        AutoLock lock(mStateLock);
        if (mState == STATE_CLOSED) {
            // throw new SocketException("Socket is closed.");
            return E_SOCKET_EXCEPTION;
        }

        if (mIs == NULL) {
            mIs = new SSLInputStream(this);
        }

        returnVal = mIs;
    }

    // Block waiting for a handshake without a lock held. It's possible that the socket
    // is closed at this point. If that happens, we'll still return the input stream but
    // all reads on it will throw.
    WaitForHandshake();
    *result = returnVal;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLSocketImpl::GetOutputStream(
    /* [out] */ IOutputStream** result)
{
    VALIDATE_NOT_NULL(result)
    CheckOpen();

    AutoPtr<IOutputStream> returnVal;
    {
        AutoLock lock(mStateLock);
        if (mState == STATE_CLOSED) {
            // throw new SocketException("Socket is closed.");
            return E_SOCKET_EXCEPTION;
        }

        if (mOs == NULL) {
            mOs = new SSLOutputStream(this);
        }

        returnVal = mOs;
    }

    // Block waiting for a handshake without a lock held. It's possible that the socket
    // is closed at this point. If that happens, we'll still return the output stream but
    // all writes on it will throw.
    WaitForHandshake();
    *result = returnVal;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

void OpenSSLSocketImpl::AssertReadableOrWriteableState()
{
    if (mState == STATE_READY || mState == STATE_READY_HANDSHAKE_CUT_THROUGH) {
        return;
    }

    // throw new AssertionError("Invalid state: " + mState);
}

void OpenSSLSocketImpl::WaitForHandshake()
{
    StartHandshake();

    {
        AutoLock lock(mStateLock);
        while (mState != STATE_READY &&
                mState != STATE_READY_HANDSHAKE_CUT_THROUGH &&
                mState != STATE_CLOSED) {
            // try {
                mStateLock.Wait();
            // } catch (InterruptedException e) {
            //     Thread.currentThread().interrupt();
            //     IOException ioe = new IOException("Interrupted waiting for handshake");
            //     ioe.initCause(e);

            //     throw ioe;
            // }
        }

        if (mState == STATE_CLOSED) {
            // throw new SocketException("Socket is closed");
            return;
        }
    }
}

ECode OpenSSLSocketImpl::GetSession(
    /* [out] */ ISSLSession** result)
{
    VALIDATE_NOT_NULL(result)
    if (mSslSession == NULL) {
        // try {
            WaitForHandshake();
        // } catch (IOException e) {
        //     // return an invalid session with
        //     // invalid cipher suite of "SSL_NULL_WITH_NULL_NULL"
        //     return SSLNullSession.getNullSession();
        // }
    }
    *result = ISSLSession::Probe(mSslSession);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLSocketImpl::AddHandshakeCompletedListener(
    /* [in] */ IHandshakeCompletedListener* listener)
{
    if (listener == NULL) {
        // throw new IllegalArgumentException("Provided listener is NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mListeners == NULL) {
        CArrayList::New((IArrayList**)&mListeners);
    }
    mListeners->Add(listener);
    return NOERROR;
}

ECode OpenSSLSocketImpl::RemoveHandshakeCompletedListener(
    /* [in] */ IHandshakeCompletedListener* listener)
{
    if (listener == NULL) {
        // throw new IllegalArgumentException("Provided listener is NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mListeners == NULL) {
        // throw new IllegalArgumentException(
        //         "Provided listener is not registered");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (!mListeners->Remove(listener)) {
        // throw new IllegalArgumentException(
        //         "Provided listener is not registered");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode OpenSSLSocketImpl::GetEnableSessionCreation(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mSslParameters->GetEnableSessionCreation(result);
}

ECode OpenSSLSocketImpl::SetEnableSessionCreation(
    /* [in] */ Boolean flag)
{
    mSslParameters->SetEnableSessionCreation(flag);
    return NOERROR;
}

ECode OpenSSLSocketImpl::GetSupportedCipherSuites(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::GetSupportedCipherSuites(result);
}

ECode OpenSSLSocketImpl::GetEnabledCipherSuites(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    return mSslParameters->GetEnabledCipherSuites(result);
}

ECode OpenSSLSocketImpl::SetEnabledCipherSuites(
    /* [in] */ ArrayOf<String>* suites)
{
    mSslParameters->SetEnabledCipherSuites(suites);
    return NOERROR;
}

ECode OpenSSLSocketImpl::GetSupportedProtocols(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::GetSupportedProtocols(result);
}

ECode OpenSSLSocketImpl::GetEnabledProtocols(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    return mSslParameters->GetEnabledProtocols(result);
}

ECode OpenSSLSocketImpl::SetEnabledProtocols(
    /* [in] */ ArrayOf<String>* protocols)
{
    mSslParameters->SetEnabledProtocols(protocols);
    return NOERROR;
}

ECode OpenSSLSocketImpl::SetUseSessionTickets(
    /* [in] */ Boolean useSessionTickets)
{
    AutoPtr<SSLParametersImpl> _mSslParameters = (SSLParametersImpl*)mSslParameters.Get();
    _mSslParameters->mUseSessionTickets = useSessionTickets;
    return NOERROR;
}

ECode OpenSSLSocketImpl::SetHostname(
    /* [in] */ const String& hostname)
{
    mSslParameters->SetUseSni(!hostname.IsNull());
    mWrappedHost = hostname;
    return NOERROR;
}

ECode OpenSSLSocketImpl::SetChannelIdEnabled(
    /* [in] */ Boolean enabled)
{
    Boolean bMode = FALSE;
    GetUseClientMode(&bMode);
    if (bMode) {
        // throw new IllegalStateException("Client mode");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    {
        AutoLock lock(mStateLock);
        if (mState != STATE_NEW) {
            // throw new IllegalStateException(
            //         "Could not enable/disable Channel ID after the initial handshake has"
            //                 + " begun.");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    AutoPtr<SSLParametersImpl> _mSslParameters = (SSLParametersImpl*)mSslParameters.Get();
    _mSslParameters->mChannelIdEnabled = enabled;
    return NOERROR;
}

ECode OpenSSLSocketImpl::GetChannelId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    Boolean bMode = FALSE;
    GetUseClientMode(&bMode);
    if (bMode) {
        // throw new IllegalStateException("Client mode");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    {
        AutoLock lock(mStateLock);
        if (mState != STATE_READY) {
            // throw new IllegalStateException(
            //         "Channel ID is only available after handshake completes");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    return NativeCrypto::SSL_get_tls_channel_id(mSslNativePointer, result);
}

ECode OpenSSLSocketImpl::SetChannelIdPrivateKey(
    /* [in] */ IPrivateKey* privateKey)
{
    Boolean bMode = FALSE;
    GetUseClientMode(&bMode);
    if (!bMode) {
        // throw new IllegalStateException("Server mode");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    {
        AutoLock lock(mStateLock);
        if (mState != STATE_NEW) {
            // throw new IllegalStateException(
            //         "Could not change Channel ID private key after the initial handshake has"
            //                 + " begun.");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }

    AutoPtr<SSLParametersImpl> _mSslParameters = (SSLParametersImpl*)mSslParameters.Get();
    if (privateKey == NULL) {
        _mSslParameters->mChannelIdEnabled = FALSE;
        mChannelIdPrivateKey = NULL;
    }
    else {
        _mSslParameters->mChannelIdEnabled = TRUE;
        // try {
            mChannelIdPrivateKey = OpenSSLKey::FromPrivateKey(privateKey);
        // } catch (InvalidKeyException e) {
        //     // Will have error in startHandshake
        // }
    }
    return NOERROR;
}

ECode OpenSSLSocketImpl::GetUseClientMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mSslParameters->GetUseClientMode(result);
}

ECode OpenSSLSocketImpl::SetUseClientMode(
    /* [in] */ Boolean mode)
{
    {
        AutoLock lock(mStateLock);
        if (mState != STATE_NEW) {
            // throw new IllegalArgumentException(
            //         "Could not change the mode after the initial handshake has begun.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    mSslParameters->SetUseClientMode(mode);
    return NOERROR;
}

ECode OpenSSLSocketImpl::GetWantClientAuth(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mSslParameters->GetWantClientAuth(result);
}

ECode OpenSSLSocketImpl::GetNeedClientAuth(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mSslParameters->GetNeedClientAuth(result);
}

ECode OpenSSLSocketImpl::SetNeedClientAuth(
    /* [in] */ Boolean need)
{
    mSslParameters->SetNeedClientAuth(need);
    return NOERROR;
}

ECode OpenSSLSocketImpl::SetWantClientAuth(
    /* [in] */ Boolean want)
{
    mSslParameters->SetWantClientAuth(want);
    return NOERROR;
}

ECode OpenSSLSocketImpl::SendUrgentData(
    /* [in] */ Int32 data)
{
    // throw new SocketException("Method sendUrgentData() is not supported.");
    return E_SOCKET_EXCEPTION;
}

ECode OpenSSLSocketImpl::SetOOBInline(
    /* [in] */ Boolean on)
{
    // throw new SocketException("Methods sendUrgentData, setOOBInline are not supported.");
    return E_SOCKET_EXCEPTION;
}

ECode OpenSSLSocketImpl::SetSoTimeout(
    /* [in] */ Int32 readTimeoutMilliseconds)
{
    assert(0 && "TODO");
    // SSLSocket::SetSoTimeout(readTimeoutMilliseconds);
    mReadTimeoutMilliseconds = readTimeoutMilliseconds;
    return NOERROR;
}

ECode OpenSSLSocketImpl::GetSoTimeout(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mReadTimeoutMilliseconds;
    return NOERROR;
}

ECode OpenSSLSocketImpl::SetSoWriteTimeout(
    /* [in] */ Int32 writeTimeoutMilliseconds)
{
    mWriteTimeoutMilliseconds = writeTimeoutMilliseconds;

    Platform::SetSocketTimeout(ISocket::Probe(this), mWriteTimeoutMilliseconds);
    return NOERROR;
}

ECode OpenSSLSocketImpl::GetSoWriteTimeout(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWriteTimeoutMilliseconds;
    return NOERROR;
}

ECode OpenSSLSocketImpl::SetHandshakeTimeout(
    /* [in] */ Int32 handshakeTimeoutMilliseconds)
{
    mHandshakeTimeoutMilliseconds = handshakeTimeoutMilliseconds;
    return NOERROR;
}

ECode OpenSSLSocketImpl::Close()
{
    // TODO: Close SSL sockets using a background thread so they close gracefully.

    AutoPtr<SSLInputStream> sslInputStream;
    AutoPtr<SSLOutputStream> sslOutputStream;

    {
        AutoLock lock(mStateLock);
        if (mState == STATE_CLOSED) {
            // close() has already been called, so do nothing and return.
            return NOERROR;
        }

        Int32 oldState = mState;
        mState = STATE_CLOSED;

        if (oldState == STATE_NEW) {
            // The handshake hasn't been started yet, so there's no OpenSSL related
            // state to clean up. We still need to close the underlying socket if
            // we're wrapping it and were asked to autoClose.
            CloseUnderlyingSocket();

            mStateLock.NotifyAll();
            return NOERROR;
        }

        if (oldState != STATE_READY && oldState != STATE_READY_HANDSHAKE_CUT_THROUGH) {
            // If we're in these states, we still haven't returned from startHandshake.
            // We call SSL_interrupt so that we can interrupt SSL_do_handshake and then
            // set the state to STATE_CLOSED. startHandshake will handle all cleanup
            // after SSL_do_handshake returns, so we don't have anything to do here.
            NativeCrypto::SSL_interrupt(mSslNativePointer);

            mStateLock.NotifyAll();
            return NOERROR;
        }

        mStateLock.NotifyAll();
        // We've already returned from startHandshake, so we potentially have
        // input and output streams to clean up.
        sslInputStream = mIs;
        sslOutputStream = mOs;
    }

    // Don't bother interrupting unless we have something to interrupt.
    if (sslInputStream != NULL || sslOutputStream != NULL) {
        NativeCrypto::SSL_interrupt(mSslNativePointer);
    }

    // Wait for the input and output streams to finish any reads they have in
    // progress. If there are no reads in progress at this point, future reads will
    // throw because state == STATE_CLOSED
    if (sslInputStream != NULL) {
        sslInputStream->AwaitPendingOps();
    }
    if (sslOutputStream != NULL) {
        sslOutputStream->AwaitPendingOps();
    }

    ShutdownAndFreeSslNative();
    return NOERROR;
}

void OpenSSLSocketImpl::ShutdownAndFreeSslNative()
{
    // try {
        assert(0 && "TODO");
        // BlockGuard::GetThreadPolicy()->OnNetwork();
        AutoPtr<IFileDescriptor> fd;
        Platform::GetFileDescriptor(mSocket, (IFileDescriptor**)&fd);
        NativeCrypto::SSL_shutdown(mSslNativePointer, fd, this);
    // } catch (IOException ignored) {

    //     * Note that although close() can throw
    //     * IOException, the RI does not throw if there
    //     * is problem sending a "close notify" which
    //     * can happen if the underlying socket is closed.

    // } finally {
        Free();
        CloseUnderlyingSocket();
    // }
}

void OpenSSLSocketImpl::CloseUnderlyingSocket()
{
    if (mSocket->Probe(EIID_IInterface) != this->Probe(EIID_IInterface)) {
        Boolean bClosed = FALSE;
        mSocket->IsClosed(&bClosed);
        if (mAutoClose && !bClosed) {
            mSocket->Close();
        }
    }
    else {
        assert(0 && "TODO");
        // if (!SSLSocket::IsClosed()) {
        //     SSLSocket::Close();
        // }
    }
}

void OpenSSLSocketImpl::Free()
{
    if (mSslNativePointer == 0) {
        return;
    }
    NativeCrypto::SSL_free(mSslNativePointer);
    mSslNativePointer = 0;
    ICloseable::Probe(mGuard)->Close();
}

ECode OpenSSLSocketImpl::Finalize()
{
    /*
     * Just worry about our own state. Notably we do not try and
     * close anything. The SocketImpl, either our own
     * PlainSocketImpl, or the Socket we are wrapping, will do
     * that. This might mean we do not properly SSL_shutdown, but
     * if you want to do that, properly close the socket yourself.
     *
     * The reason why we don't try to SSL_shutdown, is that there
     * can be a race between finalizers where the PlainSocketImpl
     * finalizer runs first and closes the socket. However, in the
     * meanwhile, the underlying file descriptor could be reused
     * for another purpose. If we call SSL_shutdown, the
     * underlying socket BIOs still have the old file descriptor
     * and will write the close notify to some unsuspecting
     * reader.
     */
    if (mGuard != NULL) {
        mGuard->WarnIfOpen();
    }
    Free();
    assert(0 && "TODO");
    // SSLSocket::Finalize();
    return NOERROR;
}

ECode OpenSSLSocketImpl::GetFileDescriptor$(
    /* [out] */ IFileDescriptor** result)
{
    VALIDATE_NOT_NULL(result)
    if (mSocket->Probe(EIID_IInterface) == this->Probe(EIID_IInterface)) {
        return Platform::GetFileDescriptorFromSSLSocket(this, result);
    }
    else {
        return Platform::GetFileDescriptor(mSocket, result);
    }
    return NOERROR;
}

ECode OpenSSLSocketImpl::GetNpnSelectedProtocol(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::SSL_get_npn_negotiated_protocol(mSslNativePointer, result);
}

ECode OpenSSLSocketImpl::GetAlpnSelectedProtocol(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::SSL_get0_alpn_selected(mSslNativePointer, result);
}

ECode OpenSSLSocketImpl::SetNpnProtocols(
    /* [in] */ ArrayOf<Byte>* npnProtocols)
{
    if (npnProtocols != NULL && npnProtocols->GetLength() == 0) {
        // throw new IllegalArgumentException("npnProtocols.length == 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<SSLParametersImpl> _mSslParameters = (SSLParametersImpl*)mSslParameters.Get();
    _mSslParameters->mNpnProtocols = npnProtocols;
    return NOERROR;
}

ECode OpenSSLSocketImpl::SetAlpnProtocols(
    /* [in] */ ArrayOf<Byte>* alpnProtocols)
{
    if (alpnProtocols != NULL && alpnProtocols->GetLength() == 0) {
        // throw new IllegalArgumentException("alpnProtocols.length == 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<SSLParametersImpl> _mSslParameters = (SSLParametersImpl*)mSslParameters.Get();
    _mSslParameters->mAlpnProtocols = alpnProtocols;
    return NOERROR;
}

ECode OpenSSLSocketImpl::ChooseServerAlias(
    /* [in] */ IX509KeyManager* keyManager,
    /* [in] */ const String& keyType,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return keyManager->ChooseServerAlias(keyType, NULL, ISocket::Probe(this), result);
}

ECode OpenSSLSocketImpl::ChooseClientAlias(
    /* [in] */ IX509KeyManager* keyManager,
    /* [in] */ ArrayOf<IX500Principal*>* issuers,
    /* [in] */ ArrayOf<String>* keyTypes,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return keyManager->ChooseClientAlias(keyTypes, NULL, ISocket::Probe(this), result);
}

ECode OpenSSLSocketImpl::ChooseServerPSKIdentityHint(
    /* [in] */ IPSKKeyManager* keyManager,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return keyManager->ChooseServerKeyIdentityHint(ISocket::Probe(this), result);
}

ECode OpenSSLSocketImpl::ChooseClientPSKIdentity(
    /* [in] */ IPSKKeyManager* keyManager,
    /* [in] */ const String& identityHint,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return keyManager->ChooseClientKeyIdentity(identityHint, ISocket::Probe(this), result);
}

ECode OpenSSLSocketImpl::GetPSKKey(
    /* [in] */ IPSKKeyManager* keyManager,
    /* [in] */ const String& identityHint,
    /* [in] */ const String& identity,
    /* [out] */ ISecretKey** result)
{
    VALIDATE_NOT_NULL(result)
    return keyManager->GetKey(identityHint, identity, ISocket::Probe(this), result);
}

} // namespace Conscrypt
} // namespace Org
