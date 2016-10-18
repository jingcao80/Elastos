
#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.CoreLibrary.Net.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "OpenSSLSessionImpl.h"
#include "NativeCrypto.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastosx::Net::Ssl::EIID_ISSLSession;
using Elastosx::Net::Ssl::ISSLSessionBindingListener;
using Elastosx::Net::Ssl::ISSLSessionBindingEvent;
using Elastosx::Net::Ssl::CSSLSessionBindingEvent;
using Elastos::Utility::IHashMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ISet;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLSessionImpl::
//=========================================
CAR_INTERFACE_IMPL_2(OpenSSLSessionImpl, Object, ISSLSession, IOpenSSLSessionImpl)

OpenSSLSessionImpl::OpenSSLSessionImpl()
    : mLastAccessedTime(0)
    , mSslSessionNativePointer(0)
    , mCreationTime(0)
    , mIsValid(TRUE)
    , mPeerHost(String(NULL))
    , mPeerPort(-1)
    , mCipherSuite(String(NULL))
    , mProtocol(String(NULL))
{
    CHashMap::New((IHashMap**)&mValues);
}

ECode OpenSSLSessionImpl::constructor(
    /* [in] */ Int64 sslSessionNativePointer,
    /* [in] */ ArrayOf<IX509Certificate*>* localCertificates,
    /* [in] */ ArrayOf<IX509Certificate*>* peerCertificates,
    /* [in] */ String peerHost,
    /* [in] */ Int32 peerPort,
    /* [in] */ IAbstractSessionContext* sessionContext)
{
    mSslSessionNativePointer = sslSessionNativePointer;
    mLocalCertificates = localCertificates;
    mPeerCertificates = peerCertificates;
    mPeerHost = peerHost;
    mPeerPort = peerPort;
    mSessionContext = sessionContext;
    return NOERROR;
}

ECode OpenSSLSessionImpl::constructor(
    /* [in] */ ArrayOf<Byte>* derData,
    /* [in] */ String peerHost,
    /* [in] */ Int32 peerPort,
    /* [in] */ ArrayOf<IX509Certificate*>* peerCertificates,
    /* [in] */ IAbstractSessionContext* sessionContext)
{
    Int64 session = 0;
    NativeCrypto::D2i_SSL_SESSION(derData, &session);
    constructor(session,
        NULL,
        peerCertificates,
        peerHost,
        peerPort,
        sessionContext);
    // TODO move this check into native code so we can throw an error with more information
    if (mSslSessionNativePointer == 0) {
        // throw new IOException("Invalid session data");
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode OpenSSLSessionImpl::GetId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    if (mId == NULL) {
        ResetId();
    }
    *result = mId;
    return NOERROR;
}

ECode OpenSSLSessionImpl::ResetId()
{
    NativeCrypto::SSL_SESSION_session_id(mSslSessionNativePointer, (ArrayOf<Byte>**)&mId);
    return NOERROR;
}

ECode OpenSSLSessionImpl::GetEncoded(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::I2d_SSL_SESSION(mSslSessionNativePointer, result);
}

ECode OpenSSLSessionImpl::GetCreationTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    if (mCreationTime == 0) {
        NativeCrypto::SSL_SESSION_get_time(mSslSessionNativePointer, &mCreationTime);
    }
    *result = mCreationTime;
    return NOERROR;
}

ECode OpenSSLSessionImpl::GetLastAccessedTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    if (mLastAccessedTime == 0) {
        return GetCreationTime(result);
    }
    *result = mLastAccessedTime;
    return NOERROR;
}

ECode OpenSSLSessionImpl::GetApplicationBufferSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ISSLRecordProtocol::MAX_DATA_LENGTH;
    return NOERROR;
}

ECode OpenSSLSessionImpl::GetPacketBufferSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ISSLRecordProtocol::MAX_SSL_PACKET_SIZE;
    return NOERROR;
}

ECode OpenSSLSessionImpl::GetLocalPrincipal(
    /* [out] */ IPrincipal** result)
{
    VALIDATE_NOT_NULL(result)
    if (mLocalCertificates != NULL && mLocalCertificates->GetLength() > 0) {
        AutoPtr<IX500Principal> res;
        (*mLocalCertificates)[0]->GetSubjectX500Principal((IX500Principal**)&res);
        *result = IPrincipal::Probe(res);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else {
        *result = NULL;
        return NOERROR;
    }
}

ECode OpenSSLSessionImpl::GetLocalCertificates(
    /* [out] */ ArrayOf<ICertificate*>** result)
{
    VALIDATE_NOT_NULL(result)
    assert(0 && "TODO");
    // *result = mLocalCertificates;
    return NOERROR;
}

ECode OpenSSLSessionImpl::GetPeerCertificateChain(
    /* [out] */ ArrayOf<Elastosx::Security::Cert::IX509Certificate*>** result)
{
    VALIDATE_NOT_NULL(result)
    CheckPeerCertificatesPresent();
    AutoPtr<ArrayOf<Elastosx::Security::Cert::IX509Certificate*> > res = mPeerCertificateChain;
    if (res == NULL) {
        // single-check idiom
        res = CreatePeerCertificateChain();
        mPeerCertificateChain = res;
    }
    *result = res;
    return NOERROR;
}

AutoPtr<ArrayOf<Elastosx::Security::Cert::IX509Certificate*> > OpenSSLSessionImpl::CreatePeerCertificateChain()
{
    // try {
    AutoPtr<ArrayOf<Elastosx::Security::Cert::IX509Certificate*> > chain
            = ArrayOf<Elastosx::Security::Cert::IX509Certificate*>::Alloc(mPeerCertificates->GetLength());

    for (Int32 i = 0; i < mPeerCertificates->GetLength(); i++) {
        AutoPtr<ArrayOf<Byte> > encoded;
        ICertificate::Probe((*mPeerCertificates)[i])->GetEncoded((ArrayOf<Byte>**)&encoded);
        assert(0 && "TODO");
        // (*chain)[i] = Elastosx::Security::Cert::X509Certificate::GetInstance(encoded);
    }
    return chain;
    // } catch (CertificateEncodingException e) {
    //     SSLPeerUnverifiedException exception = new SSLPeerUnverifiedException(e.getMessage());
    //     exception.initCause(exception);
    //     throw exception;
    // } catch (CertificateException e) {
    //     SSLPeerUnverifiedException exception = new SSLPeerUnverifiedException(e.getMessage());
    //     exception.initCause(exception);
    //     throw exception;
    // }
}

ECode OpenSSLSessionImpl::GetPeerCertificates(
    /* [out] */ ArrayOf<ICertificate*>** result)
{
    VALIDATE_NOT_NULL(result)
    CheckPeerCertificatesPresent();
    assert(0 && "TODO");
    // *result = mPeerCertificates;
    return NOERROR;
}

void OpenSSLSessionImpl::CheckPeerCertificatesPresent()
{
    if (mPeerCertificates == NULL || mPeerCertificates->GetLength() == 0) {
        // throw new SSLPeerUnverifiedException("No peer certificates");
        return;
    }
}

ECode OpenSSLSessionImpl::GetPeerPrincipal(
    /* [out] */ IPrincipal** result)
{
    VALIDATE_NOT_NULL(result)
    CheckPeerCertificatesPresent();
    AutoPtr<IX500Principal> res;
    (*mPeerCertificates)[0]->GetSubjectX500Principal((IX500Principal**)&res);
    *result = IPrincipal::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLSessionImpl::GetPeerHost(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPeerHost;
    return NOERROR;
}

ECode OpenSSLSessionImpl::GetPeerPort(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPeerPort;
    return NOERROR;
}

ECode OpenSSLSessionImpl::GetCipherSuite(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if (mCipherSuite.IsNull()) {
        String name;
        NativeCrypto::SSL_SESSION_cipher(mSslSessionNativePointer, &name);
        AutoPtr<IInterface> p;
        NativeCrypto::OPENSSL_TO_STANDARD_CIPHER_SUITES->Get(CoreUtils::Convert(name), (IInterface**)&p);
        ICharSequence::Probe(p)->ToString(&mCipherSuite);
        if (mCipherSuite.IsNull()) {
            mCipherSuite = name;
        }
    }
    *result = mCipherSuite;
    return NOERROR;
}

ECode OpenSSLSessionImpl::GetProtocol(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if (mProtocol == NULL) {
        NativeCrypto::SSL_SESSION_get_version(mSslSessionNativePointer, &mProtocol);
    }
    *result = mProtocol;
    return NOERROR;
}

ECode OpenSSLSessionImpl::GetSessionContext(
    /* [out] */ ISSLSessionContext** result)
{
    VALIDATE_NOT_NULL(result)
    *result = ISSLSessionContext::Probe(mSessionContext);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLSessionImpl::IsValid(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISSLSessionContext> context = ISSLSessionContext::Probe(mSessionContext);
    Int32 timeout = 0;
    Int64 cTime = 0;
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 mils = 0;
    if (mIsValid
            && context != NULL
            && (context->GetSessionTimeout(&timeout), timeout) != 0
            && (GetCreationTime(&cTime), cTime) + ((context->GetSessionTimeout(&timeout), timeout) * 1000)
                < (sys->GetCurrentTimeMillis(&mils), mils)) {
        mIsValid = FALSE;
    }
    *result = mIsValid;
    return NOERROR;
}

ECode OpenSSLSessionImpl::Invalidate()
{
    mIsValid = FALSE;
    mSessionContext = NULL;
    return NOERROR;
}

ECode OpenSSLSessionImpl::GetValue(
    /* [in] */ const String& name,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    if (name.IsNull()) {
        // throw new IllegalArgumentException("name == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return mValues->Get(CoreUtils::Convert(name), result);
}

ECode OpenSSLSessionImpl::GetValueNames(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 size = 0;
    mValues->GetSize(&size);
    AutoPtr<ISet> st;
    mValues->GetKeySet((ISet**)&st);
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(size);
    assert(0 && "TODO");
    // return st->ToArray(arr, result);
    return NOERROR;
}

ECode OpenSSLSessionImpl::PutValue(
    /* [in] */ const String& name,
    /* [in] */ IInterface* value)
{
    if (name.IsNull() || value == NULL) {
        // throw new IllegalArgumentException("name == null || value == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInterface> old;
    mValues->Put(CoreUtils::Convert(name), value, (IInterface**)&old);
    if (ISSLSessionBindingListener::Probe(value) != NULL) {
        AutoPtr<ISSLSessionBindingEvent> event;
        CSSLSessionBindingEvent::New(this, name, (ISSLSessionBindingEvent**)&event);
        ISSLSessionBindingListener::Probe(value)->ValueBound(event);
    }
    if (ISSLSessionBindingListener::Probe(old) != NULL) {
        AutoPtr<ISSLSessionBindingEvent> event;
        CSSLSessionBindingEvent::New(this, name, (ISSLSessionBindingEvent**)&event);
        ISSLSessionBindingListener::Probe(old)->ValueUnbound(event);
    }
    return NOERROR;
}

ECode OpenSSLSessionImpl::RemoveValue(
    /* [in] */ const String& name)
{
    if (name.IsNull()) {
        // throw new IllegalArgumentException("name == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInterface> old;
    mValues->Remove(CoreUtils::Convert(name), (IInterface**)&old);
    if (ISSLSessionBindingListener::Probe(old) != NULL) {
        AutoPtr<ISSLSessionBindingListener> listener = ISSLSessionBindingListener::Probe(old);
        AutoPtr<ISSLSessionBindingEvent> event;
        CSSLSessionBindingEvent::New(this, name, (ISSLSessionBindingEvent**)&event);
        listener->ValueUnbound(event);
    }
    return NOERROR;
}

ECode OpenSSLSessionImpl::Finalize()
{
    NativeCrypto::SSL_SESSION_free(mSslSessionNativePointer);
    // super::Finalize();
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
