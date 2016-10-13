
#include "Elastos.CoreLibrary.Utility.h"
#include "SSLNullSession.h"
#include <elastos/core/CoreUtils.h>
#include "libcore/utility/EmptyArray.h"

using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_ICloneable;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastosx::Net::Ssl::EIID_ISSLSession;
using Elastosx::Net::Ssl::ISSLSessionBindingListener;
using Elastosx::Net::Ssl::CSSLSessionBindingEvent;
using Elastosx::Net::Ssl::ISSLSessionBindingEvent;
using Elastosx::Security::Cert::IX509Certificate;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ISet;

using Libcore::Utility::EmptyArray;

namespace Org {
namespace Conscrypt {

//=========================================
// SSLNullSession::DefaultHolder::
//=========================================
AutoPtr<ISSLNullSession> SSLNullSession::DefaultHolder::NULL_SESSION;// = new SSLNullSession();

//=========================================
// SSLNullSession::
//=========================================
CAR_INTERFACE_IMPL_3(SSLNullSession, Object, ISSLNullSession, ISSLSession, ICloneable)

AutoPtr<ISSLSession> SSLNullSession::GetNullSession()
{
    return ISSLSession::Probe(DefaultHolder::NULL_SESSION);
}

ECode SSLNullSession::constructor()
{
    CHashMap::New((IHashMap**)&mValues);
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    sys->GetCurrentTimeMillis(&mCreationTime);
    mLastAccessedTime = mCreationTime;
    return NOERROR;
}

ECode SSLNullSession::GetApplicationBufferSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ISSLRecordProtocol::MAX_DATA_LENGTH;
    return NOERROR;
}

ECode SSLNullSession::GetCipherSuite(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("SSL_NULL_WITH_NULL_NULL");
    return NOERROR;
}

ECode SSLNullSession::GetCreationTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCreationTime;
    return NOERROR;
}

ECode SSLNullSession::GetId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = EmptyArray::BYTE;
    return NOERROR;
}

ECode SSLNullSession::GetLastAccessedTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mLastAccessedTime;
    return NOERROR;
}

ECode SSLNullSession::GetLocalCertificates(
    /* [out] */ ArrayOf<ICertificate*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    return NOERROR;
}

ECode SSLNullSession::GetLocalPrincipal(
    /* [out] */ IPrincipal** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    return NOERROR;
}

ECode SSLNullSession::GetPacketBufferSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ISSLRecordProtocol::MAX_SSL_PACKET_SIZE;
    return NOERROR;
}

ECode SSLNullSession::GetPeerCertificateChain(
    /* [out] */ ArrayOf<IX509Certificate*>** result)
{
    VALIDATE_NOT_NULL(result)
    // throw new SSLPeerUnverifiedException("No peer certificate");
    // return E_SSL_PEER_UNVERRIFIED_EXCEPTION;
    return NOERROR;
}

ECode SSLNullSession::GetPeerCertificates(
    /* [out] */ ArrayOf<ICertificate*>** result)
{
    VALIDATE_NOT_NULL(result)
    // throw new SSLPeerUnverifiedException("No peer certificate");
    // return E_SSL_PEER_UNVERRIFIED_EXCEPTION;
    return NOERROR;
}

ECode SSLNullSession::GetPeerHost(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);
    return NOERROR;
}

ECode SSLNullSession::GetPeerPort(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = -1;
    return NOERROR;
}

ECode SSLNullSession::GetPeerPrincipal(
    /* [out] */ IPrincipal** result)
{
    VALIDATE_NOT_NULL(result)
    // throw new SSLPeerUnverifiedException("No peer certificate");
    // return E_SSL_PEER_UNVERRIFIED_EXCEPTION;
    return NOERROR;
}

ECode SSLNullSession::GetProtocol(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("NONE");
    return NOERROR;
}

ECode SSLNullSession::GetSessionContext(
    /* [out] */ ISSLSessionContext** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    return NOERROR;
}

ECode SSLNullSession::GetValue(
    /* [in] */ const String& name,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    if (name.IsNull()) {
        // throw new IllegalArgumentException("name == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return mValues->Get(CoreUtils::Convert(name), result);
}

ECode SSLNullSession::GetValueNames(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 size = 0;
    mValues->GetSize(&size);
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(size);
    AutoPtr<ISet> st;
    mValues->GetKeySet((ISet**)&st);
    assert(0 && "TODO");
    // return st->ToArray(arr, result);
    return NOERROR;
}

ECode SSLNullSession::Invalidate()
{
    return NOERROR;
}

ECode SSLNullSession::IsValid(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode SSLNullSession::PutValue(
    /* [in] */ const String& name,
    /* [in] */ IInterface* value)
{
    if (name.IsNull() || value == NULL) {
        // throw new IllegalArgumentException("name == NULL || value == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInterface> old;
    mValues->Put(CoreUtils::Convert(name), value, (IInterface**)&old);
    if (ISSLSessionBindingListener::Probe(value) != NULL) {
        AutoPtr<ISSLSessionBindingEvent> p;
        CSSLSessionBindingEvent::New(this, name, (ISSLSessionBindingEvent**)&p);
        ISSLSessionBindingListener::Probe(value)->ValueBound(p);
    }
    if (ISSLSessionBindingListener::Probe(old) != NULL) {
        AutoPtr<ISSLSessionBindingEvent> p;
        CSSLSessionBindingEvent::New(this, name, (ISSLSessionBindingEvent**)&p);
        ISSLSessionBindingListener::Probe(old)->ValueUnbound(p);
    }
    return NOERROR;
}

ECode SSLNullSession::RemoveValue(
    /* [in] */ const String& name)
{
    if (name.IsNull()) {
        // throw new IllegalArgumentException("name == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInterface> old;
    mValues->Remove(CoreUtils::Convert(name), (IInterface**)&old);
    if (ISSLSessionBindingListener::Probe(old) != NULL) {
        AutoPtr<ISSLSessionBindingListener> listener = ISSLSessionBindingListener::Probe(old);
        AutoPtr<ISSLSessionBindingEvent> p;
        CSSLSessionBindingEvent::New(this, name, (ISSLSessionBindingEvent**)&p);
        listener->ValueUnbound(p);
    }
    return NOERROR;
}

ECode SSLNullSession::Clone(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
