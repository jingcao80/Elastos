#include "org/conscrypt/AbstractSessionContext.h"
#include "org/conscrypt/ByteArray.h"
#include "org/conscrypt/NativeCrypto.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/Arrays.h>

using Elastos::Core::CArrayOf;
using Elastos::Core::CByte;
using Elastos::Core::EIID_IByte;
using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::IDataInput;
using Elastos::IO::IDataInputStream;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Utility::Arrays;
using Elastos::Utility::EIID_IEnumeration;
using Elastos::Utility::ICollection;
using Elastos::Utility::IList;
using Elastosx::Net::Ssl::EIID_ISSLSessionContext;

namespace Org {
namespace Conscrypt {

//=======================================================
// AbstractSessionContext::_Enumeration
//=======================================================

CAR_INTERFACE_IMPL(AbstractSessionContext::_Enumeration, Object, IEnumeration)

AbstractSessionContext::_Enumeration::_Enumeration(
    /* [in] */ IIterator* i,
    /* [in] */ AbstractSessionContext* host)
    : mHost(host)
    , mI(i)
{
}

ECode AbstractSessionContext::_Enumeration::HasMoreElements(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mNext != NULL) {
        *result = TRUE;
        return NOERROR;
    }
    Boolean b;
    while (mI->HasNext(&b), b) {
        AutoPtr<IInterface> obj;
        mI->GetNext((IInterface**)&obj);
        AutoPtr<ISSLSession> session = ISSLSession::Probe(obj);

        if (session->IsValid(&b), b) {
            mNext = session;
            *result = TRUE;
            return NOERROR;
        }
    }
    mNext = NULL;
    *result = FALSE;
    return NOERROR;
}

ECode AbstractSessionContext::_Enumeration::GetNextElement(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    Boolean b;
    if (HasMoreElements(&b), b) {
        AutoPtr<ArrayOf<Byte> > id;
        mNext->GetId((ArrayOf<Byte>**)&id);
        mNext = NULL;
        AutoPtr<IArrayOf> iArray;
        CArrayOf::New(EIID_IByte, id->GetLength(), (IArrayOf**)&iArray);
        for (Int32 i = 0; i < id->GetLength(); ++i) {
            AutoPtr<IByte> iB;
            CByte::New((*id)[i], (IByte**)&iB);
            iArray->Set(i, iB);
        }
        *result = iArray;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    // throw new NoSuchElementException();
    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

//=======================================================
// AbstractSessionContext::CustomLinkedHashMap
//=======================================================

AbstractSessionContext::CustomLinkedHashMap::CustomLinkedHashMap(
    /* [in] */ AbstractSessionContext* host)
    : mHost(host)
{
    LinkedHashMap::constructor();
}

Boolean AbstractSessionContext::CustomLinkedHashMap::RemoveEldestEntry(
    /* [in] */ IMapEntry* eldest)
{
    Int32 size;
    GetSize(&size);
    Boolean remove = mHost->mMaximumSize > 0 && size > mHost->mMaximumSize;
    if (remove) {
        AutoPtr<IInterface> obj;
        eldest->GetKey((IInterface**)&obj);
        Remove(obj);
        obj = NULL;
        eldest->GetValue((IInterface**)&obj);
        mHost->SessionRemoved(ISSLSession::Probe(obj));
    }
    return FALSE;
}

//=======================================================
// AbstractSessionContext
//=======================================================

CAR_INTERFACE_IMPL_2(AbstractSessionContext, Object, ISSLSessionContext, IAbstractSessionContext)

AbstractSessionContext::AbstractSessionContext()
    : mMaximumSize(0)
    , mTimeout(0)
{
    NativeCrypto::SSL_CTX_new(&mSslCtxNativePointer);

    mSessions = new CustomLinkedHashMap(this);
}

AbstractSessionContext::~AbstractSessionContext()
{
    // try {
    NativeCrypto::SSL_CTX_free(mSslCtxNativePointer);
    // } finally {
    // super.finalize();
    // }
}

ECode AbstractSessionContext::constructor(
    /* [in] */ Int32 maximumSize,
    /* [in] */ Int32 timeout)
{
    mMaximumSize = maximumSize;
    mTimeout = timeout;
    return NOERROR;
}

ECode AbstractSessionContext::GetIds(
    /* [out] */ IEnumeration** result)
{
    AutoPtr<IIterator> i = SessionIterator();
    AutoPtr<_Enumeration> ret = new _Enumeration(i, this);
    *result = IEnumeration::Probe(ret);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode AbstractSessionContext::GetSessionCacheSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMaximumSize;
    return NOERROR;
}

ECode AbstractSessionContext::GetSessionTimeout(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTimeout;
    return NOERROR;
}

ECode AbstractSessionContext::SetSessionTimeout(
    /* [in] */ Int32 seconds)
{
    if (seconds < 0) {
        // throw new IllegalArgumentException("seconds < 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mTimeout = seconds;

    AutoLock lock(mSessions);
    AutoPtr<ICollection> values;
    mSessions->GetValues((ICollection**)&values);
    AutoPtr<IIterator> i;
    values->GetIterator((IIterator**)&i);

    Boolean b;
    while (i->HasNext(&b), b) {
        AutoPtr<IInterface> obj;
        i->GetNext((IInterface**)&obj);
        AutoPtr<ISSLSession> session = ISSLSession::Probe(obj);

        // SSLSession's know their context and consult the
        // timeout as part of their validity condition.
        if (session->IsValid(&b), !b) {
            i->Remove();
            SessionRemoved(session);
        }
    }

    return NOERROR;
}

ECode AbstractSessionContext::SetSessionCacheSize(
    /* [in] */ Int32 size)
{
    if (size < 0) {
        // throw new IllegalArgumentException("size < 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 oldMaximum = mMaximumSize;
    mMaximumSize = size;

    // Trim cache to size if necessary.
    if (size < oldMaximum) {
        TrimToSize();
    }
    return NOERROR;
}

ECode AbstractSessionContext::ToBytes(
    /* [in] */ ISSLSession* session,
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    // TODO: Support SSLSessionImpl, too.
// TODO: Need IOpenSSLSessionImpl
    // AutoPtr<IOpenSSLSessionImpl> sslSession = IOpenSSLSessionImpl::Probe(session);
    // if (sslSession == NULL) {
    //     *result = NULL;
    //     return NOERROR;
    // }

    // try {
    AutoPtr<IByteArrayOutputStream> baos;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&baos);
    AutoPtr<IDataOutputStream> daos;
    CDataOutputStream::New(IOutputStream::Probe(baos), (IDataOutputStream**)&daos);

    IOutputStream::Probe(daos)->Write(IAbstractSessionContext::OPEN_SSL); // session type ID

    // Session data.
    AutoPtr<ArrayOf<Byte> > data;
// TODO: Need IOpenSSLSessionImpl
    // sslSession->GetEncoded((ArrayOf<Byte>**)&data);
    IOutputStream::Probe(daos)->Write(data->GetLength());
    IOutputStream::Probe(daos)->Write(data);

    // Certificates.
    AutoPtr<ArrayOf<ICertificate*> > certs;
    session->GetPeerCertificates((ArrayOf<ICertificate*>**)&certs);
    IOutputStream::Probe(daos)->Write(certs->GetLength());

    for (Int32 i = 0; i < certs->GetLength(); i++) {
        AutoPtr<ICertificate> cert = (*certs)[i];
        cert->GetEncoded((ArrayOf<Byte>**)&data);
        IOutputStream::Probe(daos)->Write(data->GetLength());
        IOutputStream::Probe(daos)->Write(data);
    }
    // TODO: local certificates?

    return baos->ToByteArray(result);
    // } catch (IOException e) {
    //     log(e);
    //     return NULL;
    // } catch (CertificateEncodingException e) {
    //     log(e);
    //     return NULL;
    // }
}

ECode AbstractSessionContext::ToSession(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [out] */ ISSLSession** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IByteArrayInputStream> bais;
    CByteArrayInputStream::New(data, (IByteArrayInputStream**)&bais);
    AutoPtr<IDataInputStream> dais;
    CDataInputStream::New(IInputStream::Probe(bais), (IDataInputStream**)&dais);

    // try {
    Int32 type;
    IInputStream::Probe(dais)->Read(&type);
    if (type != IAbstractSessionContext::OPEN_SSL) {
        // log(new AssertionError("Unexpected type ID: " + type));
         *result = NULL;
         return NOERROR;
    }

    Int32 length;
    IInputStream::Probe(dais)->Read(&length);
    AutoPtr<ArrayOf<Byte> > sessionData = ArrayOf<Byte>::Alloc(length);
    IDataInput::Probe(dais)->ReadFully(sessionData);

    Int32 count;
    IInputStream::Probe(dais)->Read(&count);
    AutoPtr<ArrayOf<IX509Certificate*> > certs = ArrayOf<IX509Certificate*>::Alloc(count);
    for (Int32 i = 0; i < count; i++) {
        IInputStream::Probe(dais)->Read(&length);
        AutoPtr<ArrayOf<Byte> > certData = ArrayOf<Byte>::Alloc(length);
        IDataInput::Probe(dais)->ReadFully(certData);
// TODO: Need OpenSSLX509Certificate
        // certs->Set(i, OpenSSLX509Certificate::FromX509Der(certData));
    }

// TODO: Need COpenSSLSessionImpl
    // COpenSSLSessionImpl::New(sessionData, host, port, certs, this, result);
    // } catch (IOException e) {
    //     log(e);
    //     return NULL;
    // }
    return NOERROR;
}

ECode AbstractSessionContext::GetSession(
    /* [in] */ ArrayOf<Byte>* sessionId,
    /* [out] */ ISSLSession** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (sessionId == NULL) {
        // throw new NullPointerException("sessionId == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ByteArray> key = new ByteArray(sessionId);
    AutoPtr<ISSLSession> session;
    {
        AutoLock lock(mSessions);
        AutoPtr<IInterface> obj;
        mSessions->Get((IInterface*)(IObject*)key.Get(), (IInterface**)&obj);
        session = ISSLSession::Probe(obj);
    }

    Boolean b;
    if (session != NULL && (session->IsValid(&b), b)) {
        *result = session;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    return NOERROR;
}

ECode AbstractSessionContext::PutSession(
    /* [in] */ ISSLSession* session)
{
    AutoPtr<ArrayOf<Byte> > id;
    session->GetId((ArrayOf<Byte>**)&id);
    if (id->GetLength() == 0) {
        return NOERROR;
    }
    AutoPtr<ByteArray> key = new ByteArray(id);
    AutoLock lock(mSessions);
    mSessions->Put((IInterface*)(IObject*)key.Get(), session);
    return NOERROR;
}

void AbstractSessionContext::TrimToSize()
{
    AutoLock lock(mSessions);
    Int32 size;
    mSessions->GetSize(&size);
    if (size > mMaximumSize) {
        Int32 removals = size - mMaximumSize;
        AutoPtr<ICollection> values;
        mSessions->GetValues((ICollection**)&values);
        AutoPtr<IIterator> i;
        values->GetIterator((IIterator**)&i);

        do {
            AutoPtr<IInterface> obj;
            i->GetNext((IInterface**)&obj);
            AutoPtr<ISSLSession> session = ISSLSession::Probe(obj);
            i->Remove();
            SessionRemoved(session);
        } while (--removals > 0);
    }
}

void AbstractSessionContext::SessionRemoved(
    /* [in] */ ISSLSession* session)
{
    return;
}

AutoPtr<IIterator> AbstractSessionContext::SessionIterator()
{
    AutoLock lock(mSessions);
    AutoPtr<ICollection> values;
    mSessions->GetValues((ICollection**)&values);
    Int32 size;
    mSessions->GetSize(&size);
    AutoPtr<ArrayOf<IInterface*> > inArray = ArrayOf<IInterface*>::Alloc(size);
    AutoPtr<ArrayOf<IInterface*> > array;
    values->ToArray(inArray, (ArrayOf<IInterface*>**)&array);
    AutoPtr<IList> list;
    Arrays::AsList(array, (IList**)&list);
    AutoPtr<IIterator> it;
    list->GetIterator((IIterator**)&it);
    return it;
}

} // namespace Conscrypt
} // namespace Org
