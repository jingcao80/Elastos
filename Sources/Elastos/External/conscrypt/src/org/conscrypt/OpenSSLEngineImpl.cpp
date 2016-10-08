#include "org/conscrypt/OpenSSLEngineImpl.h"
#include "org/conscrypt/AbstractSessionContext.h"
#include "org/conscrypt/NativeCrypto.h"
#include <elastos/core/AutoLock.h>

namespace Org {
namespace Conscrypt {

CAR_INTERFACE_IMPL_3(OpenSSLEngineImpl, SSLEngine, ISSLHandshakeCallbacks,
        ISSLParametersImplAliasChooser, ISSLParametersImplPSKCallbacks)

OpenSSLEngineImpl::OpenSSLEngineImpl()
    : mEngineState(EngineState_NEW)
    , mSslNativePointer(0)
{
    sNullSource; // = OpenSSLBIOSource.wrap(ByteBuffer.allocate(0));
    mLocalToRemoteSink; // = OpenSSLBIOSink.create();
}

OpenSSLEngineImpl::~OpenSSLEngineImpl()
{
    // try {
    Free();
    // } finally {
    //     super.finalize();
    // }
}

ECode OpenSSLEngineImpl::constructor(
    /* [in] */ ISSLParametersImpl* sslParameters)
{
    mSslParameters = sslParameters;
    return NOERROR;
}

ECode OpenSSLEngineImpl::constructor(
    /* [in] */ String host,
    /* [in] */ Int32 port,
    /* [in] */ ISSLParametersImpl* sslParameters)
{
    SSLEngine::constructor(host, port);
    mSslParameters = sslParameters;
    return NOERROR;
}

ECode OpenSSLEngineImpl::BeginHandshake()
{
    {
        AutoLock lock(mStateLock);
        if (mEngineState == EngineState_CLOSED || mEngineState == EngineState_CLOSED_OUTBOUND
                || mEngineState == EngineState_CLOSED_INBOUND) {
            // throw new IllegalStateException("Engine has already been closed");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        if (mEngineState == EngineState_HANDSHAKE_STARTED) {
            // throw new IllegalStateException("Handshake has already been started");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        if (mEngineState != EngineState_MODE_SET) {
            // throw new IllegalStateException("Client/server mode must be set before handshake");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        Boolean b;
        if (GetUseClientMode(&b), b) {
            mEngineState = EngineState_HANDSHAKE_WANTED;
        }
        else {
            mEngineState = EngineState_HANDSHAKE_STARTED;
        }
    }

    Boolean releaseResources = TRUE;
    // try {
    AutoPtr<IAbstractSessionContext> sessionContext;
    mSslParameters->GetSessionContext((IAbstractSessionContext**)&sessionContext);
    Int64 sslCtxNativePointer = (AbstractSessionContext*)sessionContext->mSslCtxNativePointer;
    NativeCrypto::SSL_new(sslCtxNativePointer, &mSslNativePointer);
    String host;
    GetPeerHost(&host);
    mSslParameters->SetSSLParameters(sslCtxNativePointer, mSslNativePointer, this, this, host);
    mSslParameters->SetCertificateValidation(mSslNativePointer);
    mSslParameters->SetTlsChannelId(mSslNativePointer, channelIdPrivateKey);

    Boolean b;
    if (GetUseClientMode(&b), b) {
        NativeCrypto::SSL_set_connect_state(mSslNativePointer);
    }
    else {
        NativeCrypto::SSL_set_accept_state(mSslNativePointer);
    }
    OpenSSLBIOSink::Create((IOpenSSLBIOSink**)&mHandshakeSink);
    releaseResources = FALSE;
    // } catch (IOException e) {
    //     // Write CCS errors to EventLog
    //     String message = e.getMessage();
    //     // Must match error reason string of SSL_R_UNEXPECTED_CCS (in ssl/ssl_err.c)
    //     if (message.contains("unexpected CCS")) {
    //         String logMessage = String.format("ssl_unexpected_ccs: host=%s", getPeerHost());
    //         Platform.logEvent(logMessage);
    //     }
    //     throw new SSLException(e);
    // } finally {
    if (releaseResources) {
            AutoLock lock(mStateLock);
            mEngineState = EngineState_CLOSED;
        }
        ShutdownAndFreeSslNative();
    }
    // }
    return NOERROR;
}

ECode OpenSSLEngineImpl::CloseInbound()
{
    {
        AutoLock lock(mStateLock);
        if (mEngineState == EngineState_CLOSED) {
            return NOERROR;
        }
        if (mEngineState == EngineState_CLOSED_OUTBOUND) {
            mEngineState = EngineState_CLOSED;
        }
        else {
            mEngineState = EngineState_CLOSED_INBOUND;
        }
    }
    // TODO anything else to notify OpenSSL layer?
    return NOERROR;
}

ECode OpenSSLEngineImpl::CloseOutbound()
{
    {
        AutoLock lock(mStateLock);
        if (mEngineState == EngineState_CLOSED || mEngineState == EngineState_CLOSED_OUTBOUND) {
            return NOERROR;
        }
        if (mEngineState != EngineState_MODE_SET && mEngineState != EngineState_NEW) {
            ShutdownAndFreeSslNative();
        }
        if (mEngineState == EngineState_CLOSED_INBOUND) {
            mEngineState = EngineState_CLOSED;
        }
        else {
            mEngineState = EngineState_CLOSED_OUTBOUND;
        }
    }
    Shutdown();
    return NOERROR;
}

ECode OpenSSLEngineImpl::GetDelegatedTask(
    /* [out] */ IRunnable** result)
{
    VALIDATE_NOT_NULL(result)
    /* This implementation doesn't use any delegated tasks. */
    *result = NULL;
    return NOERROR;
}

ECode OpenSSLEngineImpl::GetEnabledCipherSuites(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    return mSslParameters->GetEnabledCipherSuites(result);
}

ECode OpenSSLEngineImpl::GetEnabledProtocols(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    return mSslParameters->GetEnabledProtocols(result);
}

ECode OpenSSLEngineImpl::GetEnableSessionCreation(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mSslParameters->GetEnableSessionCreation(result);
}

ECode OpenSSLEngineImpl::GetHandshakeStatus(
    /* [out] */ SSLEngineResult_HandshakeStatus* result)
{
    VALIDATE_NOT_NULL(result)
    {
        AutoLock lock(mStateLock);
        Boolean b;
        Int32 val;
        switch (mEngineState) {
            case HANDSHAKE_WANTED:
                if (GetUseClientMode(&b), b) {
                    *result = SSLEngineResult_HandshakeStatus_NEED_WRAP;
                    return NOERROR;
                }
                else {
                    *result = SSLEngineResult_HandshakeStatus_NEED_UNWRAP;
                    return NOERROR;
                }
            case HANDSHAKE_STARTED:
                if ((mHandshakeSink->Available(&val), val) > 0) {
                    *result = SSLEngineResult_HandshakeStatus_NEED_WRAP;
                    return NOERROR;
                }
                else {
                    *result = SSLEngineResult_HandshakeStatus_NEED_UNWRAP;
                    return NOERROR;
                }
            case HANDSHAKE_COMPLETED:
                if ((mHandshakeSink->Available(&val), val) == 0) {
                    mHandshakeSink = NULL;
                    mEngineState = EngineState_READY;
                    *result = SSLEngineResult_HandshakeStatus_FINISHED;
                    return NOERROR;
                }
                else {
                    *result = SSLEngineResult_HandshakeStatus_NEED_WRAP;
                    return NOERROR;
                }
            case NEW:
            case MODE_SET:
            case CLOSED:
            case CLOSED_INBOUND:
            case CLOSED_OUTBOUND:
            case READY:
            case READY_HANDSHAKE_CUT_THROUGH:
                *result = SSLEngineResult_HandshakeStatus_NOT_HANDSHAKING;
                return NOERROR;
            default:
                break;
        }
        // throw new IllegalStateException("Unexpected engine state: " + engineState);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode OpenSSLEngineImpl::GetNeedClientAuth(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mSslParameters->GetNeedClientAuth(result);
}

ECode OpenSSLEngineImpl::GetSession(
    /* [out] */ ISSLSession** result)
{
    VALIDATE_NOT_NULL(result)
    if (mSslSession == NULL) {
        return SSLNullSession::GetNULLSession(result);
    }
    *result = mSslSession;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLEngineImpl::GetSupportedCipherSuites(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::GetSupportedCipherSuites(result);
}

ECode OpenSSLEngineImpl::GetSupportedProtocols(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::GetSupportedProtocols(result);
}

ECode OpenSSLEngineImpl::GetUseClientMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mSslParameters->GetUseClientMode(result);
}

ECode OpenSSLEngineImpl::GetWantClientAuth(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mSslParameters->GetWantClientAuth(result);
}

ECode OpenSSLEngineImpl::IsInboundDone(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mSslNativePointer == 0) {
        AutoLock lock(mStateLock);
        *result = mEngineState == EngineState_CLOSED
                || mEngineState == EngineState_CLOSED_INBOUND;
    }
    Int32 val;
    NativeCrypto::SSL_get_shutdown(mSslNativePointer, &val);
    *result = (val & NativeCrypto::SSL_RECEIVED_SHUTDOWN) != 0;
    return NOERROR;
}

ECode OpenSSLEngineImpl::IsOutboundDone(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mSslNativePointer == 0) {
        AutoLock lock(mStateLock);
        *result = mEngineState == EngineState_CLOSED
                || mEngineState == EngineState_CLOSED_OUTBOUND;
    }
    Int32 val;
    NativeCrypto::SSL_get_shutdown(mSslNativePointer, &val);
    *result = (val & NativeCrypto::SSL_SENT_SHUTDOWN) != 0;
    return NOERROR;
}

ECode OpenSSLEngineImpl::SetEnabledCipherSuites(
    /* [in] */ ArrayOf<String>* suites)
{
    return mSslParameters->SetEnabledCipherSuites(suites);
}

ECode OpenSSLEngineImpl::SetEnabledProtocols(
    /* [in] */ ArrayOf<String>* protocols)
{
    return mSslParameters->SetEnabledProtocols(protocols);
}

ECode OpenSSLEngineImpl::SetEnableSessionCreation(
    /* [in] */ Boolean flag)
{
    return mSslParameters->SetEnableSessionCreation(flag);
}

ECode OpenSSLEngineImpl::SetNeedClientAuth(
    /* [in] */ Boolean need)
{
    return mSslParameters->SetNeedClientAuth(need);
}

ECode OpenSSLEngineImpl::SetUseClientMode(
    /* [in] */ Boolean mode)
{
    {
        AutoLock lock(mStateLock);
        if (mEngineState != EngineState_MODE_SET && mEngineState != EngineState_NEW) {
            // throw new IllegalArgumentException(
            //         "Can not change mode after handshake: mEngineState == " + engineState);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        mEngineState = EngineState_MODE_SET;
    }
    mSslParameters->SetUseClientMode(mode);
    return NOERROR;
}

ECode OpenSSLEngineImpl::SetWantClientAuth(
    /* [in] */ Boolean want)
{
    return mSslParameters->SetWantClientAuth(want);
}

ECode OpenSSLEngineImpl::Unwrap(
    /* [in] */ IByteBuffer* src,
    /* [in] */ ArrayOf<IByteBuffer*>* dsts,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ ISSLEngineResult** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (src == NULL) {
        // throw new IllegalArgumentException("src == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (dsts == NULL) {
        // throw new IllegalArgumentException("dsts == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    checkIndex(dsts.length, offset, length);
    Int32 dstRemaining = 0;
    for (Int32 i = 0; i < dsts->GetLength(); i++) {
        AutoPtr<IByteBuffer> dst = (*dsts)[i];
        Boolean b;
        if (dst == NULL) {
            // throw new IllegalArgumentException("one of the dst == NULL");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        else if (dst->IsReadOnly(&b), b) {
            // throw new ReadOnlyBufferException();
            return E_READ_ONLY_BUFFER_EXCEPTION;
        }
        if (i >= offset && i < offset + length) {
            Int32 val;
            dst->Remaining(&val);
            dstRemaining += val;
        }
    }

    {
        AutoLock lock(mStateLock);
        // If the inbound direction is closed. we can't send anymore.
        if (mEngineState == EngineState_CLOSED || mEngineState == EngineState_CLOSED_INBOUND) {
            SSLEngineResult_HandshakeStatus handshakeStatus;
            GetHandshakeStatus(&handshakeStatus);
            AutoPtr<SSLEngineResult> ssler = new SSLEngineResult();
            ssler->constructor(Elastosx::Net::Ssl::CLOSED, handshakeStatus, 0, 0);
            *result = ssler;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        if (mEngineState == EngineState_NEW || mEngineState == EngineState_MODE_SET) {
            BeginHandshake();
        }
    }

    // If we haven't completed the handshake yet, just let the caller know.
    SSLEngineResult_HandshakeStatus handshakeStatus;
    GetHandshakeStatus(&handshakeStatus);
    if (handshakeStatus == SSLEngineResult_HandshakeStatus_NEED_UNWRAP) {
        AutoPtr<IOpenSSLBIOSource> source;
        OpenSSLBIOSource::Wrap(src, (IOpenSSLBIOSource**)&source);
        Int64 sslSessionCtx = 0;
        // try {
        Int64 sourceBioRef;
        source->GetContext(&sourceBioRef);
        Int64 sinkBioRef;
        mHandshakeSink->GetContext(&sinkBioRef);
        Boolean client_mode;
        GetUseClientMode(&client_mode);
        NativeCrypto::SSL_do_handshake_bio(mSslNativePointer,
                sourceBioRef, sinkBioRef, ISSLHandshakeCallbacks::Probe(this), client_mode,
                mSslParameters->npnProtocols, mSslParameters->alpnProtocols, &sslSessionCtx);
        if (sslSessionCtx != 0) {
            if (sslSession != NULL && mEngineState == EngineState_HANDSHAKE_STARTED) {
                mEngineState = EngineState_READY_HANDSHAKE_CUT_THROUGH;
            }
            String host;
            GetPeerHost(&host);
            Int32 port;
            GetPeerPort(&port);
            sslSession = mSslParameters->SetupSession(sslSessionCtx, mSslNativePointer, sslSession,
                    host, port, TRUE);
        }
        Int32 bytesWritten;
        mHandshakeSink->Position(&bytesWritten);
        SSLEngineResult_HandshakeStatus status;
        GetHandshakeStatus(&status);
        AutoPtr<SSLEngineResult> ssler = new SSLEngineResult();
        ssler->constructor(Elastosx::Net::Ssl::OK, status, 0, bytesWritten);
        *result = ssler;
        REFCOUNT_ADD(*result)
        return NOERROR;
        // } catch (Exception e) {
        //     throw (SSLHandshakeException) new SSLHandshakeException("Handshake failed")
        //             .initCause(e);
        // } finally {
        if (sslSession == NULL && sslSessionCtx != 0) {
            NativeCrypto::SSL_SESSION_free(sslSessionCtx);
        }
        source->releaseResources();
        // }
    }
    else if (handshakeStatus != SSLEngineResult_HandshakeStatus_NOT_HANDSHAKING) {
        AutoPtr<SSLEngineResult> ssler = new SSLEngineResult();
        ssler->constructor(Elastosx::Net::Ssl::OK, handshakeStatus, 0, 0);
        *result = ssler;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    if (dstRemaining == 0) {
        SSLEngineResult_HandshakeStatus status;
        GetHandshakeStatus(&status);
        AutoPtr<SSLEngineResult> ssler = new SSLEngineResult();
        ssler->constructor(Elastosx::Net::Ssl::BUFFER_OVERFLOW, status, 0, 0);
        *result = ssler;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    AutoPtr<IByteBuffer> srcDuplicate;
    src->Duplicate((IByteBuffer**)&srcDuplicate);
    AutoPtr<IOpenSSLBIOSource> source;
    OpenSSLBIOSource::Wrap(srcDuplicate, (IOpenSSLBIOSource**)&source);

    // try {
    Int32 positionBeforeRead;
    srcDuplicate->GetPosition(&positionBeforeRead);
    Int32 produced = 0;
    Boolean shouldStop = FALSE;

    while (!shouldStop) {
        AutoPtr<IByteBuffer> dst = GetNextAvailableByteBuffer(dsts, offset, length);
        if (dst == NULL) {
            shouldStop = TRUE;
            continue;
        }
        AutoPtr<IByteBuffer> arrayDst = dst;
        Boolean b;
        if (dst->IsDirect(&b), b) {
            arrayDst = ByteBuffer.allocate(dst.remaining());
        }

        Int32 dstOffset = arrayDst.arrayOffset() + arrayDst.position();

        Int32 internalProduced;
        NativeCrypto::SSL_read_BIO(mSslNativePointer,
                arrayDst.array(), dstOffset, dst.remaining(), source.getContext(),
                localToRemoteSink.getContext(), this, &internalProduced);
        if (internalProduced <= 0) {
            shouldStop = TRUE;
            continue;
        }
        arrayDst.position(arrayDst.position() + internalProduced);
        produced += internalProduced;
        if (dst != arrayDst) {
            arrayDst.flip();
            dst.put(arrayDst);
        }
    }

    Int32 position;
    srcDuplicate->GetPosition(&position);
    Int32 consumed = position - positionBeforeRead;
    src->SetPosition(position);
    SSLEngineResult_HandshakeStatus status;
    GetHandshakeStatus(&status);
    AutoPtr<SSLEngineResult> ssler = new SSLEngineResult();
    ssler->constructor(Elastosx::Net::Ssl::OK, status, consumed, produced);
    *result = ssler;
    REFCOUNT_ADD(*result)
    // } catch (IOException e) {
    //     throw new SSLException(e);
    // } finally {
    source->releaseResources();
    // }
    return NOERROR;
}

ECode OpenSSLEngineImpl::Wrap(
    /* [in] */ ArrayOf<IByteBuffer*>* srcs,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ IByteBuffer* dst,
    /* [out] */ ISSLEngineResult** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Boolean b;
    if (srcs == NULL) {
        // throw new IllegalArgumentException("srcs == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (dst == NULL) {
        // throw new IllegalArgumentException("dst == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (dst->IsReadOnly(&b), b) {
        // throw new ReadOnlyBufferException();
        return E_READ_ONLY_BUFFER_EXCEPTION;
    }

    for (Int32 i = 0; i < srcs->GetLength(); ++i) {
        AutoPtr<IByteBuffer> src = (*srcs)[i];
        if (src == NULL) {
            // throw new IllegalArgumentException("one of the src == NULL");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    CheckIndex(srcs->GetLength(), offset, length);

    Int32 remaining;
    dst->GetRemaining(&remaining);
    if (remaining < INativeCrypto::SSL3_RT_MAX_PACKET_SIZE) {
        AutoPtr<SSLEngineResult> ssler = new SSLEngineResult();
        SSLEngineResult_HandshakeStatus status;
        GetHandshakeStatus(&status);
        ssler->constructor(Elastosx::Net::Ssl::BUFFER_OVERFLOW, status, 0, 0);
        *result = ssler;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    {
        AutoLock lock(mStateLock);
        // If the outbound direction is closed. we can't send anymore.
        if (mEngineState == EngineState_CLOSED || mEngineState == EngineState_CLOSED_OUTBOUND) {
            AutoPtr<SSLEngineResult> ssler = new SSLEngineResult();
            SSLEngineResult_HandshakeStatus status;
            GetHandshakeStatus(&status);
            ssler->constructor(Elastosx::Net::Ssl::CLOSED, status, 0, 0);
            *result = ssler;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        if (mEngineState == EngineState_NEW || mEngineState == EngineState_MODE_SET) {
            BeginHandshake();
        }
    }

    // If we haven't completed the handshake yet, just let the caller know.
    SSLEngineResult_HandshakeStatus handshakeStatus;
    GetHandshakeStatus(&handshakeStatus);
    if (handshakeStatus == SSLEngineResult_HandshakeStatus_NEED_WRAP) {
        Int32 available;
        mHandshakeSink->Available(&available);
        if (available == 0) {
            Int64 sslSessionCtx = 0L;
            // try {
            Int64 sourceBioRef;
            NULLSource->GetContext(&sourceBioRef);
            Int64 sinkBioRef;
            mHandshakeSink->GetContext(&sinkBioRef);
            Boolean client_mode;
            GetUseClientMode(&client_mode);
            NativeCrypto::SSL_do_handshake_bio(mSslNativePointer,
                    sourceBioRef, sinkBioRef, ISSLHandshakeCallbacks::Probe(this),
                    client_mode, mSslParameters->npnProtocols,
                    mSslParameters->alpnProtocols, &sslSessionCtx);
            if (sslSessionCtx != 0) {
                if (mSslSession != NULL && mEngineState == EngineState_HANDSHAKE_STARTED) {
                    mEngineState = EngineState_READY_HANDSHAKE_CUT_THROUGH;
                }
                String host;
                GetPeerHost(&host);
                Int32 port;
                GetPeerPort(&port);
                mSslSession = mSslParameters->SetupSession(sslSessionCtx, mSslNativePointer, mSslSession,
                        host, port, TRUE);
            }
            // } catch (Exception e) {
            //     throw (SSLHandshakeException) new SSLHandshakeException("Handshake failed")
            //             .initCause(e);
            // } finally {
            if (mSslSession == NULL && sslSessionCtx != 0) {
                NativeCrypto::SSL_SESSION_free(sslSessionCtx);
            }
            // }
        }
        Int32 bytesWritten = WriteSinkToByteBuffer(mHandshakeSink, dst);
        AutoPtr<SSLEngineResult> ssler = new SSLEngineResult();
        SSLEngineResult_HandshakeStatus status;
        GetHandshakeStatus(&status);
        ssler->constructor(Elastosx::Net::Ssl::OK, status, 0, bytesWritten);
        *result = ssler;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if (handshakeStatus != SSLEngineResult_HandshakeStatus_NOT_HANDSHAKING) {
        AutoPtr<SSLEngineResult> ssler = new SSLEngineResult();
        SSLEngineResult_HandshakeStatus status;
        GetHandshakeStatus(&status);
        ssler->constructor(Elastosx::Net::Ssl::OK, handshakeStatus, 0, 0);
        *result = ssler;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    // try {
    Int32 totalRead = 0;
    AutoPtr<ArrayOf<Byte> > buffer;

    for (Int32 i = 0; i < srcs->GetLength(); ++i) {
        AutoPtr<IByteBuffer> src = (*srcs)[i];
        Int32 toRead;
        src->GetRemaining(&toRead);
        if (buffer == NULL || toRead > buffer->GetLength()) {
            buffer = ArrayOf<Byte>::Alloc(toRead);
        }
        /*
         * We can't just use .mark() here because the caller might be
         * using it.
         */
        AutoPtr<IByteBuffer> bb;
        src->Duplicate((IByteBuffer**)&bb);
        bb->Get(buffer, 0, toRead);
        Int64 ctx;
        localToRemoteSink->GetContext(&ctx);
        Int32 numRead;
        NativeCrypto::SSL_write_BIO(mSslNativePointer, buffer, toRead,
                ctx, this, &numRead);
        if (numRead > 0) {
            Int32 position;
            src->GetPosition(&position);
            src->SetPosition(position + numRead);
            totalRead += numRead;
        }
    }

    AutoPtr<SSLEngineResult> ssler = new SSLEngineResult();
    SSLEngineResult_HandshakeStatus status;
    GetHandshakeStatus(&status);
    ssler->constructor(Elastosx::Net::Ssl::OK, status,
            totalRead, WriteSinkToByteBuffer(localToRemoteSink, dst));
    *result = ssler;
    REFCOUNT_ADD(*result)
    // } catch (IOException e) {
    //     throw new SSLException(e);
    // }
    return NOERROR;
}

ECode OpenSSLEngineImpl::ClientPSKKeyRequested(
    /* [in] */ String identityHint,
    /* [in] */ ArrayOf<Byte>* identity,
    /* [in] */ ArrayOf<Byte>* key,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mSslParameters->ClientPSKKeyRequested(identityHint, identity, key, this);
}

ECode OpenSSLEngineImpl::ServerPSKKeyRequested(
    /* [in] */ const String& identityHint,
    /* [in] */ const String& identity,
    /* [in] */ ArrayOf<Byte>* key,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mSslParameters->ServerPSKKeyRequested(identityHint, identity, key, this);
}

ECode OpenSSLEngineImpl::OnSSLStateChange(
    /* [in] */ Int64 sslSessionNativePtr,
    /* [in] */ Int32 type,
    /* [in] */ Int32 val)
{
    {
        AutoLock lock(mStateLock);
        switch (type) {
            case NativeCrypto::SSL_CB_HANDSHAKE_DONE:
                if (mEngineState != EngineState_HANDSHAKE_STARTED &&
                    mEngineState != EngineState_READY_HANDSHAKE_CUT_THROUGH) {
                    // throw new IllegalStateException("Completed handshake while in mode "
                    //         + engineState);
                    return E_ILLEGAL_STATE_EXCEPTION;
                }
                mEngineState = EngineState_HANDSHAKE_COMPLETED;
                break;
            case NativeCrypto::SSL_CB_HANDSHAKE_START:
                // For clients, this will allow the NEED_UNWRAP status to be
                // returned.
                mEngineState = EngineState_HANDSHAKE_STARTED;
                break;
        }
    }
    return NOERROR;
}

ECode OpenSSLEngineImpl::VerifyCertificateChain(
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
    AutoPtr<ArrayOf<IOpenSSLX509Certificate*> > peerCertChain =
            ArrayOf<IOpenSSLX509Certificate*>::Alloc(certRefs->GetLength());

    for (Int32 i = 0; i < certRefs->GetLength(); i++) {
        COpenSSLX509Certificate::New((*certRefs)[i], (IOpenSSLX509Certificate**)&(*peerCertChain)[i]);
    }

    // Used for verifyCertificateChain callback
    String host;
    GetPeerHost(&host);
    Int32 port;
    GetPeerPort(&port);
    mHandshakeSession = new OpenSSLSessionImpl(sslSessionNativePtr, NULL, peerCertChain,
            host, port, NULL);

    Boolean client;
    mSslParameters->GetUseClientMode(&client);
    if (client) {
        Platform::CheckServerTrusted(x509tm, peerCertChain, authMethod, host);
    }
    else {
        AutoPtr<IPublicKey> pk;
        ICertificate::Probe((*peerCertChain)[0])->GetPublicKey((IPublicKey**)&pk);
        String authType;
        pk->GetAlgorithm(&authType);
        x509tm->CheckClientTrusted(peerCertChain, authType);
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

ECode OpenSSLEngineImpl::ClientCertificateRequested(
    /* [in] */ ArrayOf<Byte>* keyTypes,
    /* [in] */ ArrayOf<Handle32>* asn1DerEncodedX500Principals)
{
    return mSslParameters->ChooseClientCertificate(keyTypeBytes, asn1DerEncodedPrincipals,
            mSslNativePointer, this);
}

ECode OpenSSLEngineImpl::ChooseServerAlias(
    /* [in] */ IX509KeyManager* keyManager,
    /* [in] */ const String& keyType,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if (IX509ExtendedKeyManager::Probe(keyManager) != NULL) {
        AutoPtr<IX509ExtendedKeyManager> ekm = IX509ExtendedKeyManager::Probe(keyManager);
        return ekm->ChooseEngineServerAlias(keyType, NULL, this, result);
    }
    else {
        return keyManager->ChooseServerAlias(keyType, NULL, NULL, result);
    }
    return NOERROR;
}

ECode OpenSSLEngineImpl::ChooseClientAlias(
    /* [in] */ IX509KeyManager* keyManager,
    /* [in] */ ArrayOf<IX500Principal*>* issuer,
    /* [in] */ ArrayOf<String>* keyTypes,
    /* [out] */ String* results)
{
    VALIDATE_NOT_NULL(result)
    return keyManager->ChooseServerKeyIdentityHint(this, reuslt);
}

ECode OpenSSLEngineImpl::ChooseServerPSKIdentityHint(
    /* [in] */ IPSKKeyManager* keyManager,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return keyManager->ChooseServerKeyIdentityHint(this, reuslt);
}

ECode OpenSSLEngineImpl::ChooseClientPSKIdentity(
    /* [in] */ IPSKKeyManager* keyManager,
    /* [in] */ const String& identityHint,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return keyManager->ChooseClientKeyIdentity(identityHint, this, result);
}

ECode OpenSSLEngineImpl::GetPSKKey(
    /* [in] */ IPSKKeyManager* keyManager,
    /* [in] */ const String& identityHint,
    /* [in] */ const String& identity,
    /* [out] */ ISecretKey** result)
{
    VALIDATE_NOT_NULL(result)
    return keyManager->GetKey(identityHint, identity, this, result);
}

ECode OpenSSLEngineImpl::CheckIndex(
    /* [in] */ Int32 length,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    if (offset < 0) {
        // throw new IndexOutOfBoundsException("offset < 0");
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    else if (count < 0) {
        // throw new IndexOutOfBoundsException("count < 0");
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    else if (offset > length) {
        // throw new IndexOutOfBoundsException("offset > length");
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    else if (offset > length - count) {
        // throw new IndexOutOfBoundsException("offset + count > length");
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

/** Returns the next non-empty ByteBuffer. */
AutoPtr<IByteBuffer> OpenSSLEngineImpl::GetNextAvailableByteBuffer(
    /* [in] */ ArrayOf<IByteBuffer*>* buffers,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    for (Int32 i = offset; i < length; ++i) {
        Int32 remaining;
        (*buffers)[i]->GetRemaining(&remaining);
        if (remaining > 0) {
            return (*buffers)[i];
        }
    }
    return NULL;
}

/** Writes data available in a BIO sink to a ByteBuffer. */
Int32 OpenSSLEngineImpl::WriteSinkToByteBuffer(
    /* [in] */ IOpenSSLBIOSink* sink,
    /* [in] */ IByteBuffer* dst)
{
    Int32 available;
    sink->Available(&available);
    Int32 remaining;
    dst->GetRemaining(&remaining);
    Int32 toWrite = Math::Min(available, remaining);
    Int32 position;
    sink->GetPosition(&position);
    AutoPtr<ArrayOf<Byte> > array;
    sink.toByteArray((ArrayOf<Byte>**)&array);
    dst->Put(array, position, toWrite);
    Int32 val;
    sink->Skip(toWrite, &val);
    return val;
}

void OpenSSLEngineImpl::Shutdown()
{
    // try {
    Int64 nullCtx;
    sNullSource->GetContext(&nullCtx);
    Int64 ctx;
    mLocalToRemoteSink->GetContext(&ctx);
    NativeCrypto::SSL_shutdown_BIO(mSslNativePointer, nullCtx,
            ctx, this);
    // } catch (IOException ignored) {
    //     /*
    //      * TODO: The RI ignores close failures in SSLSocket, but need to
    //      * investigate whether it does for SSLEngine.
    //      */
    // }
}

void OpenSSLEngineImpl::ShutdownAndFreeSslNative()
{
    // try {
    Shutdown();
    // } finally {
    Free();
    // }
}

void OpenSSLEngineImpl::Free()
{
    if (mSslNativePointer == 0) {
        return;
    }
    NativeCrypto::SSL_free(mSslNativePointer);
    mSslNativePointer = 0;
}

} // namespace Conscrypt
} // namespace Org
