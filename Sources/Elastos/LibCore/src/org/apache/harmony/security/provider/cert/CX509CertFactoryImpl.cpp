
#include "CX509CertFactoryImpl.h"
#include <cmdef.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Cert {

CAR_INTERFACE_IMPL_LIGHT(CX509CertFactoryImpl::RestoringInputStream, IInputStream)

ECode CX509CertFactoryImpl::RestoringInputStream::Available(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    mInStream->Available(number);
    *number += mBar - mPos;
    return NOERROR;
}

ECode CX509CertFactoryImpl::RestoringInputStream::Mark(
    /* [in] */ Int32 readLimit)
{
    if (mPos < 0) {
        mPos = 0;
        mBar = 0;
        mEnd = BUFF_SIZE - 1;
    } else {
        mEnd = (mPos + BUFF_SIZE - 1) % BUFF_SIZE;
    }
    return NOERROR;
}

ECode CX509CertFactoryImpl::RestoringInputStream::IsMarkSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported)
    *supported = TRUE;
    return NOERROR;
}

ECode CX509CertFactoryImpl::RestoringInputStream::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    // if buffer is currently used
    if (mPos >= 0) {
        // current position in the buffer
        Int32 cur = mPos % BUFF_SIZE;
        // check whether the buffer contains the data to be read
        if (cur < mBar) {
            // return the data from the buffer
            mPos++;
            *value = (*mBuff)[cur];
            return NOERROR;
        }
        // check whether buffer has free space
        if (cur != mEnd) {
            // it has, so read the data from the wrapped stream
            // and place it in the buffer
            mInStream->Read(&(*mBuff)[cur]);
            mBuff = cur+1;
            mPos++;
            *value = (*mBuff)[cur];
            return NOERROR;
        } else {
            // buffer if full and can not operate
            // any more, so invalidate the mark position
            // and turn off the using of buffer
            mPos = -1;
        }
    }
    // buffer is not used, so return the data from the wrapped stream
    return mInStream->Read(value);
}

ECode CX509CertFactoryImpl::RestoringInputStream::ReadBytes(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [out] */ Int32* number)
{
    return InputStream::ReadBytes(buffer, number);
}

ECode CX509CertFactoryImpl::RestoringInputStream::ReadBytesEx(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    Int32 read_b;
    Int32 i;
    for (i = 0; i<length; i++) {
        if ((Read(&read_b), read_b) == -1) {
            *number = (i == 0) ? -1 : i;
            return NOERROR;
        }
        (*buffer)[offset + i] = (Byte) read_b;
    }
    *number = i;
    return NOERROR;
}

ECode CX509CertFactoryImpl::RestoringInputStream::Reset()
{
    if (mPos >= 0) {
        mPos = (mEnd + 1) % BUFF_SIZE;
    } else {
        /*throw new IOException("Could not reset the stream: " +
                "position became invalid or stream has not been marked");*/
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode CX509CertFactoryImpl::RestoringInputStream::Skip(
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* number)
{
    return InputStream::Skip(byteCount, number);
}

ECode CX509CertFactoryImpl::RestoringInputStream::GetLock(
    /* [out] */ IInterface** lockObj)
{
    return InputStream::GetLock(lockObj);
}

ECode CX509CertFactoryImpl::RestoringInputStream::Close()
{
    return mInStream->Close();
}

CX509CertFactoryImpl::RestoringInputStream::RestoringInputStream(
    /* [in] */ IInputStream* inStream)
    : mPos(-1)
    , mBar(0)
    , mEnd(0)
{
    mBuff = ArrayOf<Int32>::Alloc(BUFF_SIZE*2);
    mInStream = inStream;
}

AutoPtr<ICache> CX509CertFactoryImpl::CERT_CACHE;// = new Cache(CERT_CACHE_SEED_LENGTH);
AutoPtr<ArrayOf<Byte> > CX509CertFactoryImpl::PEM_BEGIN;
AutoPtr<ArrayOf<Byte> > CX509CertFactoryImpl::PEM_END;
AutoPtr<ArrayOf<Byte> > CX509CertFactoryImpl::FREE_BOUND_SUFFIX;
AutoPtr<ArrayOf<Byte> > CX509CertFactoryImpl::CERT_BOUND_SUFFIX;
Object CX509CertFactoryImpl::mLock;
AutoPtr<ICache> CX509CertFactoryImpl::CRL_CACHE = InitStatic();


AutoPtr<ICache> CX509CertFactoryImpl::InitStatic()
{
    AutoPtr<ICache> ret;
    CCache::New(CERT_CACHE_SEED_LENGTH, (ICache**)&CERT_CACHE);
    CCache::New(CRL_CACHE_SEED_LENGTH, (ICache**)&ret);
    String str("-----BEGIN");
    PEM_BEGIN = str->GetBytes();
    str = "-----END";
    PEM_END = str->GetBytes();
    str = " CERTIFICATE-----";
    CERT_BOUND_SUFFIX = PEM_END = str->GetBytes();
    return ret;
}

CAR_OBJECT_IMPL(CX509CertFactoryImpl)

ECode CX509CertFactoryImpl::EngineGenerateCertificate(
    /* [in] */ IInputStream* inStream,
    /* [out] */ ICertificate** cert)
{
    VALIDATE_NOT_NULL(cert)
    if (inStream == NULL) {
        //throw new CertificateException("inStream == null");
        return E_CERTIFICATE_EXCEPTION;
    }
    Boolean flag;
    if (inStream->MarkSupported(&flag), !flag) {
        // create the mark supporting wrapper
        inStream = new RestoringInputStream(inStream);
    }
    // mark is needed to recognize the format of the provided encoding
    // (ASN.1 or PEM)
    inStream->Mark(1);
    // check whether the provided certificate is in PEM encoded form
    Char32 c;
    inStream->Read(&c);
    if (c == '-') {
        // decode PEM, retrieve CRL
        AutoPtr<ArrayOf<Byte> > bytes;
        DecodePEM(inStream, CERT_BOUND_SUFFIX, (ArrayOf<Byte>**)&bytes);
        return GetCertificate(bytes, cert);
    } else {
        inStream->Reset();
        // retrieve CRL
        return GetCertificate(inStream, cert);
    }
}

ECode CX509CertFactoryImpl::EngineGenerateCertificates(
    /* [in] */ IInputStream* inStream,
    /* [out] */ ICollection** col)
{
    VALIDATE_NOT_NULL(col)
    if (inStream == NULL) {
        //throw new CertificateException("inStream == null");
        return E_CERTIFICATE_EXCEPTION;
    }
    AutoPtr<IArrayList> result;
    CArrayList::New((IArrayList**)&result);
    Boolean flag;
    if (inStream->MarkSupported(&flag), !flag) {
        // create the mark supporting wrapper
        inStream = new RestoringInputStream(inStream);
    }
    // if it is PEM encoded form this array will contain the encoding
    // so ((it is PEM) <-> (encoding != null))
    AutoPtr<ArrayOf<Byte> > encoding;
    // The following by SEQUENCE ASN.1 tag, used for
    // recognizing the data format
    // (is it PKCS7 ContentInfo structure, X.509 Certificate, or
    // unsupported encoding)
    Int32 second_asn1_tag = -1;
    inStream->Mark(1);
    Int32 ch;
    while ((inStream->Read(&ch), ch) != -1) {
        // check if it is PEM encoded form
        if (ch == '-') { // beginning of PEM encoding ('-' char)
            // decode PEM chunk and store its content (ASN.1 encoding)
            DecodePEM(inStream, FREE_BOUND_SUFFIX, (ArrayOf<Byte>**)&encoding);
        } else if (ch == 0x30) { // beginning of ASN.1 sequence (0x30)
            encoding = NULL;
            inStream->Reset();
            // prepare for data format determination
            inStream->Mark(CERT_CACHE_SEED_LENGTH);
        } else { // unsupported data
            Int32 size;
            result->GetSize(&size);
            if (size == 0) {
                //throw new CertificateException("Unsupported encoding");
                return E_CERTIFICATE_EXCEPTION;
            } else {
                // it can be trailing user data,
                // so keep it in the stream
                inStream->Reset();
                *cert = result;
                REFCOUNT_ADD(*cert)
                result NOERROR;
            }
        }
        // Check the data format
        AutoPtr<IBerInputStream> bis;
        if (encoding == NULL) {
            CBerInputStream::New(inStream, (IBerInputStream**)&bis);
        }
        else {
            CBerInputStream::New(encoding, (IBerInputStream**)&bis);
        }
        // read the next ASN.1 tag
        bis->Next(&second_asn1_tag); // inStream position changed
        if (encoding == NULL) {
            // keep whole structure in the stream
            inStream->Reset();
        }
        // check if it is a TBSCertificate structure
        if (second_asn1_tag != IASN1Constants::TAG_C_SEQUENCE) {
            Int32 size;
            result->GetSize(&size);
            if (size == 0) {
                // there were not read X.509 Certificates, so
                // break the cycle and check
                // whether it is PKCS7 structure
                break;
            } else {
                // it can be trailing user data,
                // so return what we already read
                *cert = result;
                REFCOUNT_ADD(*cert)
                return NOERROR;
            }
        } else {
            if (encoding == NULL) {
                AutoPtr<ICertificate> ct;
                GetCertificate(inStream, (ICertificate**)&ct);
                result->Add(ct.Get());
            } else {
                AutoPtr<ICertificate> ct;
                GetCertificate(encoding, (ICertificate**)&ct);
                result->Add(ct.Get());
            }
        }
        // mark for the next iteration
        inStream->Mark(1);
    }
    Int32 sz;
    result->GetSize(&sz);
    if (sz != 0) {
        // some Certificates have been read
        *cert = result;
        REFCOUNT_ADD(*cert)
        return NOERROR;
    } else if (ch == -1) {
        //throw new CertificateException("There is no data in the stream");
        return E_CERTIFICATE_EXCEPTION;
    }
    // else: check if it is PKCS7
    if (second_asn1_tag == IASN1Constants::TAG_OID) {
        // it is PKCS7 ContentInfo structure, so decode it
        AutoPtr<IInterface> decoded;
        if (encoding ! = NULL) {
            CContentInfo::ASN1->Decode(encoding, (IInterface**)&decoded);
        }
        else {
            CContentInfo::ASN1->Decode(inStream, (IInterface**)&decoded);
        }
        AutoPtr<IContentInfo> info = IContentInfo::Probe(decoded);
        AutoPtr<ISignedData> data;
        info->GetSignedData((ISignedData**)&data);
        if (data == NULL) {
            //throw new CertificateException("Invalid PKCS7 data provided");
            return E_CERTIFICATE_EXCEPTION;
        }

        AutoPtr<IList> certs;
        data->GetCertificates((IList**)&certs);
        if (certs != NULL) {
            AutoPtr<IIterator> it;
            certs->GetIterator((IIterator**)&it);
            Boolean next;
            it->HasNext(&next);
            while (next) {
                AutoPtr<IInterface> elem;
                it->GetNext((IInterface**)&elem);
                AutoPtr<IX509CertImpl> certImpl;
                CX509CertImpl::New(elem, (IX509CertImpl**)&certImpl);
                result->Add(certImpl.Get());
                it->HasNext(&next);
            }
        }
        *col = result;
        REFCOUNT_ADD(*col)
        return NOERROR;
    }
    return NOERROR;
}

ECode CX509CertFactoryImpl::EngineGenerateCRL(
    /* [in] */ IInputStream* inStream,
    /* [out] */ ICRL** crl)
{
    VALIDATE_NOT_NULL(crl)
    if (inStream == NULL) {
        //throw new CRLException("inStream == null");
        return E_CRL_EXCEPTION;
    }

    if (!inStream->MarkSupported()) {
        // Create the mark supporting wrapper
        // Mark is needed to recognize the format
        // of provided encoding form (ASN.1 or PEM)
        inStream = new RestoringInputStream(inStream);
    }
    inStream->Mark(1);
    // check whether the provided crl is in PEM encoded form
    Char32 ch;
    if ((inStream->Read(&ch), ch) == '-') {
        // decode PEM, retrieve CRL
        AutoPtr<ArrayOf<Byte> > tmp;
        DecodePEM(inStream, FREE_BOUND_SUFFIX, (ArrayOf<Byte>**)&tmp);
        return GetCRL(tmp, crl);
    } else {
        inStream->Reset();
        // retrieve CRL
        return GetCRL(inStream, crl);
    }
}

ECode CX509CertFactoryImpl::EngineGenerateCRLs(
    /* [in] */ IInputStream* inStream,
    /* [out] */ ICollection** crls)
{
    VALIDATE_NOT_NULL(crls)
    if (inStream == NULL) {
        //throw new CRLException("inStream == null");
        return E_CRL_EXCEPTION;
    }
    AutoPtr<IArrayList> result;
    CArrayList::New((IArrayList**)&result);
    Boolean flag;
    if (inStream->MarkSupported(&flag), !flag) {
        inStream = new RestoringInputStream(inStream);
    }
    // if it is PEM encoded form this array will contain the encoding
    // so ((it is PEM) <-> (encoding != null))
    AutoPtr<ArrayOf<Byte> > encoding;
    // The following by SEQUENCE ASN.1 tag, used for
    // recognizing the data format
    // (is it PKCS7 ContentInfo structure, X.509 CRL, or
    // unsupported encoding)
    Int32 second_asn1_tag = -1;
    inStream->Mark(1);
    Int32 ch;
    while ((inStream->Read(&ch), ch) != -1) {
        // check if it is PEM encoded form
        if (ch == '-') { // beginning of PEM encoding ('-' char)
            // decode PEM chunk and store its content (ASN.1 encoding)
            DecodePEM(inStream, FREE_BOUND_SUFFIX, (ArrayOf<Byte>**)&encoding);
        } else if (ch == 0x30) { // beginning of ASN.1 sequence (0x30)
            encoding = NULL;
            inStream->Reset();
            // prepare for data format determination
            inStream->Mark(CRL_CACHE_SEED_LENGTH);
        } else { // unsupported data
            Int32 size;
            result->GetSize(&size);
            if (size == 0) {
                //throw new CRLException("Unsupported encoding");
                return E_CRL_EXCEPTION;
            } else {
                // it can be trailing user data,
                // so keep it in the stream
                inStream->Reset();
                //return result;
                *crls = result;
                REFCOUNT_ADD(*crls)
                return NOERROR;
            }
        }
        // Check the data format
        AutoPtr<IBerInputStream> bis;
        if (encoding == NULL) {
            CBerInputStream::New(inStream, (IBerInputStream**)&bis);
        }
        else {
            CBerInputStream::New(encoding, (IBerInputStream**)&bis);
        }
        // read the next ASN.1 tag
        bis->Next(&second_asn1_tag);
        if (encoding == NULL) {
            // keep whole structure in the stream
            inStream->Reset();
        }
        // check if it is a TBSCertList structure
        if (second_asn1_tag != IASN1Constants::TAG_C_SEQUENCE) {
            Int32 sz;
            result->GetSize(&sz);
            if (sz == 0) {
                // there were not read X.509 CRLs, so
                // break the cycle and check
                // whether it is PKCS7 structure
                break;
            } else {
                // it can be trailing user data,
                // so return what we already read
                *crls = result;
                REFCOUNT_ADD(*crls)
                return NOERROR;
            }
        } else {
            if (encoding == NULL) {
                AutoPtr<ICRL> crl;
                GetCRL(inStream, (ICRL**)&crl);
                result->Add(crl.Get());
            } else {
                AutoPtr<ICRL> crl;
                GetCRL(encoding, (ICRL**)&crl);
                result->Add(crl.Get());
            }
        }
        inStream->Mark(1);
    }
    Int32 rs;
    result->GetSize(&rs);
    if (rs != 0) {
        // the stream was read out
        *crls = result;
        REFCOUNT_ADD(*crls)
        return NOERROR;
    } else if (ch == -1) {
        //throw new CRLException("There is no data in the stream");
        return E_CRL_EXCEPTION;
    }
    // else: check if it is PKCS7
    if (second_asn1_tag == IASN1Constants::TAG_OID) {
        // it is PKCS7 ContentInfo structure, so decode it
        AutoPtr<IInterface> decoded;
        if (encoding ! = NULL) {
            CContentInfo::ASN1->Decode(encoding, (IInterface**)&decoded);
        }
        else {
            CContentInfo::ASN1->Decode(inStream, (IInterface**)&decoded);
        }
        AutoPtr<IContentInfo> info = IContentInfo::Probe(decoded);
        AutoPtr<ISignedData> data;
        info->GetSignedData((ISignedData**)&data);
        if (data == NULL) {
            //throw new CRLException("Invalid PKCS7 data provided");
            return E_CRL_EXCEPTION;
        }

        AutoPtr<IList> certs;
        data->GetCRLs((IList**)&certs);
        if (certs != NULL) {
            AutoPtr<IIterator> it;
            certs->GetIterator((IIterator**)&it);
            Boolean next;
            it->HasNext(&next);
            while (next) {
                AutoPtr<IInterface> elem;
                it->GetNext((IInterface**)&elem);
                AutoPtr<IX509CRLImpl> crlImpl;
                CX509CRLImpl::New(elem, (IX509CRLImpl**)&crlImpl);
                result->Add(crlImpl.Get());
                it->HasNext(&next);
            }
        }
        *crls = result;
        REFCOUNT_ADD(*crls)
        return NOERROR;
    }
    // else: Unknown data format
    //throw new CRLException("Unsupported encoding");
    return E_CRL_EXCEPTION;
}

ECode CX509CertFactoryImpl::EngineGenerateCertPath(
    /* [in] */ IInputStream* inStream,
    /* [out] */ ICertPath** certPath)
{
    if (inStream == NULL) {
        //throw new CertificateException("inStream == null");
        return E_CERTIFICATE_EXCEPTION;
    }
    return EngineGenerateCertPath(inStream, "PkiPath", certPath);
}

ECode CX509CertFactoryImpl::EngineGenerateCertPath(
    /* [in] */ IInputStream* inStream,
    /* [in] */ const String& encoding,
    /* [out] */ ICertPath** certPath)
{
    VALIDATE_NOT_NULL(certPath)
    if (inStream == NULL) {
        //throw new CertificateException("inStream == null");
        return E_CERTIFICATE_EXCEPTION;
    }
    Boolean flag;
    if (inStream->MarkSupported(&flag), !flag) {
        inStream = new RestoringInputStream(inStream);
    }
    inStream->Mark(1);
    Int32 ch;

    // check if it is PEM encoded form
    if ((inStream->Read(&ch), ch) == '-') {
        // decode PEM chunk into ASN.1 form and decode CertPath object
        AutoPtr<IX509CertPathImplHelper> hlp;
        CX509CertPathImplHelper::AcquireSingleton((IX509CertPathImplHelper**)&hlp);
        AutoPtr<ArrayOf<Byte> > arg0;
        DecodePEM(inStream, FREE_BOUND_SUFFIX, (ArrayOf<Byte>**)&arg0);
        return hlp->GetInstanceEx3(arg0, encoding, certPath);
    } else if (ch == 0x30) { // ASN.1 Sequence
        inStream->Reset();
        // decode ASN.1 form
        AutoPtr<IX509CertPathImplHelper> hlp;
        CX509CertPathImplHelper::AcquireSingleton((IX509CertPathImplHelper**)&hlp);
        return hlp->GetInstanceEx(inStream, encoding, certPath);
    } else {
        //throw new CertificateException("Unsupported encoding");
        return E_CERTIFICATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CX509CertFactoryImpl::EngineGenerateCertPath(
    /* [in] */ IList* certificates,
    /* [out] */ ICertPath** certPath)
{
    return CX509CertPathImpl::New(certificates, certPath);
}

ECode CX509CertFactoryImpl::EngineGetCertPathEncodings(
    /* [out] */ IIterator** encodings)
{
    return CX509CertPathImpl::mEncodings->GetIterator(encodings);
}

ECode CX509CertFactoryImpl::constructor()
{
    return NOERROR;
}

ECode CX509CertFactoryImpl::DecodePEM(
    /* [in] */ IInputStream* inStream,
    /* [in] */ ArrayOf<Byte>* boundary_suffix,
    /* [out, callee] */ ArrayOf<Byte>** decoded)
{
    VALIDATE_NOT_NULL(decoded)
    Int32 ch; // the char to be read
    // check and skip opening boundary delimiter
    // (first '-' is supposed as already read)
    for (Int32 i = 1; i < PEM_BEGIN->GetLength(); ++i) {
        if ((*PEM_BEGIN)[i] != (inStream->Read(&ch), ch)) {
            /*
            throw new IOException(
                "Incorrect PEM encoding: '-----BEGIN"
                + ((boundary_suffix == null)
                    ? "" : new String(boundary_suffix))
                + "' is expected as opening delimiter boundary.");
            */
            return E_IO_EXCEPTION;
        }
    }
    if (boundary_suffix == NULL) {
        // read (skip) the trailing characters of
        // the beginning PEM boundary delimiter
        while ((inStream->Read(&ch), ch) != '\n') {
            if (ch == -1) {
                //throw new IOException("Incorrect PEM encoding: EOF before content");
                return E_IO_EXCEPTION;
            }
        }
    } else {
        for (Int32 i=0; i < boundary_suffix->GetLength(); i++) {
            if ((*boundary_suffix)[i] != (inStream->Read(&ch), ch)) {
                /*throw new IOException("Incorrect PEM encoding: '-----BEGIN" +
                        new String(boundary_suffix) + "' is expected as opening delimiter boundary.");*/
                return E_IO_EXCEPTION;
            }
        }
        // read new line characters
        if ((inStream->Read(&ch), ch) == '\r') {
            // CR has been read, now read LF character
            inStream->Read(&ch);
        }
        if (ch != '\n') {
            /*throw new IOException("Incorrect PEM encoding: newline expected after " +
                    "opening delimiter boundary");*/
            return E_IO_EXCEPTION;
        }
    }
    Int32 size = 1024; // the size of the buffer containing Base64 data
    AutoPtr<ArrayOf<Byte> > buff = ArrayOf<Byte>::Alloc(size);
    Int32 index = 0;
    // read bytes while ending boundary delimiter is not reached
    while ((inStream->Read(&ch), ch) != '-') {
        if (ch == -1) {
            //throw new IOException("Incorrect Base64 encoding: EOF without closing delimiter");
            return E_IO_EXCEPTION;
        }
        (*buff)[index++] = (Byte) ch;
        if (index == size) {
            // enlarge the buffer
            AutoPtr<ArrayOf<Byte> > newbuff = ArrayOf<Byte>::Alloc(size+1024);
            newbuff->Copy(0, buff, 0, size);
            buff = newbuff;
            size += 1024;
        }
    }
    if ((*buff)[index-1] != '\n') {
        /*throw new IOException("Incorrect Base64 encoding: newline expected before " +
                "closing boundary delimiter");*/
        return E_IO_EXCEPTION;
    }
    // check and skip closing boundary delimiter prefix
    // (first '-' was read)
    for (Int32 i = 1; i < PEM_END->GetLength(); ++i) {
        if ((*PEM_END)[i] != (inStream->Read(&ch), ch)) {
            //throw badEnd(boundary_suffix);
            return E_IO_EXCEPTION;
        }
    }
    if (boundary_suffix == NULL) {
        // read (skip) the trailing characters of
        // the closing PEM boundary delimiter
        while (((inStream->Read(&ch), ch) != -1) && (ch != '\n') && (ch != '\r')) {
        }
    } else {
        for (Int32 i=0; i < boundary_suffix->GetLength(); i++) {
            if ((*boundary_suffix)[i] != (inStream->Read(&ch), ch)) {
                //throw badEnd(boundary_suffix);
                return E_IO_EXCEPTION;
            }
        }
    }
    // skip trailing line breaks
    inStream->Mark(1);
    while (((inStream->Read(&ch), ch) != -1) && (ch == '\n' || ch == '\r')) {
        inStream->Mark(1);
    }
    inStream->Reset();

    AutoPtr<IBase64> bs;
    CBase64::AcquireSingleton((IBase64**)&bs);
    bs->DecodeEx(buff, index, (ArrayOf<Byte>**)&buff);
    if (buff == NULL) {
        //throw new IOException("Incorrect Base64 encoding");
        return E_IO_EXCEPTION;
    }
    *decoded = buff;
    REFCOUNT_ADD(*decoded)
    return NOERROR;
}

ECode CX509CertFactoryImpl::ReadBytes(
    /* [in] */ IInputStream* source,
    /* [in] */ Int32 length,
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        Int32 bytik;
        source->Read(&bytik);
        if (bytik == -1) {
            return NOERROR;
        }
        (*result)[i] = (Byte) bytik;
    }
    *bytes = result;
    REFCOUNT_ADD(*bytes)
    return NOERROR;
}

ECode CX509CertFactoryImpl::GetCertificate(
    /* [in] */ ArrayOf<Byte>* encoding,
    /* [out] */ ICertificate** cert)
{
    if (encoding->GetLength() < CERT_CACHE_SEED_LENGTH) {
        //throw new CertificateException("encoding.length < CERT_CACHE_SEED_LENGTH");
        return E_CERTIFICATE_EXCEPTION;
    }
    //Todo later
    //{    AutoLock syncLock(CERT_CACHE);
        Int64 hash;
        Boolean isContains;
        CERT_CACHE->GetHash(encoding, &hash);
        AutoPtr<ICertificate> res;
        if (CERT_CACHE->Contains(hash, &isContains), isContains) {
            CERT_CACHE->Get(hash, encoding, (ICertificate**)&res);
            if (res != NULL) {
                //return res;
                *cert = res;
                REFCOUNT_ADD(*cert)
                return NOERROR;
            }
        }
        res = NULL;
        CX509CertImpl::New(encoding, (ICertificate**)&res);
        CERT_CACHE->Put(hash, encoding, res.Get());
        *cert = res;
        REFCOUNT_ADD(*cert)
        return NOERROR;
    }
}

ECode CX509CertFactoryImpl::GetCertificate(
    /* [in] */ IInputStream* inStream,
    /* [out] */ ICertificate** cert)
{
        //{    AutoLock syncLock(CERT_CACHE);
        VALIDATE_NOT_NULL(cert)
        inStream->Mark(CERT_CACHE_SEED_LENGTH);
        // read the prefix of the encoding
        AutoPtr<ArrayOf<Byte> > buff;
        ReadBytes(inStream, CERT_CACHE_SEED_LENGTH, (ArrayOf<Byte>**)&buff);
        inStream->Reset();
        if (buff == NULL) {
            //throw new CertificateException("InputStream doesn't contain enough data");
            return E_CERTIFICATE_EXCEPTION;
        }
        Int64 hash;
        CERT_CACHE->GetHash(buff, &hash);
        Boolean isContains;
        if (CERT_CACHE->Contains(hash, &isContains), isContains) {
            Int32 len;
            CBerInputStream::GetLength(buff, &len);
            AutoPtr<ArrayOf<Byte> > encoding = ArrayOf<Byte>::Alloc(len);
            if (encoding->GetLength() < CERT_CACHE_SEED_LENGTH) {
                //throw new CertificateException("Bad Certificate encoding");
                return E_CERTIFICATE_EXCEPTION;
            }
            AutoPtr<IStreams> streams;
            CStreams::AcquireSingleton((IStreams**)&streams);
            streams->ReadFully(inStream, encoding);
            AutoPtr<ICertificate> res;
            CERT_CACHE->Get(hash, encoding, (ICertificate**)&res);
            if (res != NULL) {
                *cert = res;
                REFCOUNT_ADD(*cert)
                return NOERROR;
            }
            CX509CertImpl::New(encoding, (ICertificate**)&res);
            CERT_CACHE->Put(hash, encoding, res);
            *cert = res;
            REFCOUNT_ADD(*cert)
            return NOERROR;
        } else {
            inStream->Reset();
            AutoPtr<ICertificate> res;
            CX509CertImpl::New(inStream, (ICertificate**)&res);
            AutoPtr<ArrayOf<Byte> > encoding;
            res->GetEncoded((ArrayOf<Byte>**)&encoding);
            CERT_CACHE->Put(hash, encoding, res);
            *cert = res;
            REFCOUNT_ADD(*cert)
            return NOERROR;
        }
    }
}

ECode CX509CertFactoryImpl::GetCRL(
    /* [in] */ ArrayOf<Byte>* encoding,
    /* [out] */ ICRL** crl)
{
    VALIDATE_NOT_NULL(crl)
    if (encoding->GetLength() < CRL_CACHE_SEED_LENGTH) {
        //throw new CRLException("encoding.length < CRL_CACHE_SEED_LENGTH");
        return E_CRL_EXCEPTION;
    }
    //{    AutoLock syncLock(CRL_CACHE);
        Int64 hash;
        CRL_CACHE->GetHash(encoding, &hash);
        Boolean isContains;
        if (CRL_CACHE->Contains(hash, &isContains), isContains) {
            AutoPtr<IX509CRL> res;
            CRL_CACHE->Get(hash, encoding, (IX509CRL**)&res);
            //X509CRL res = (X509CRL) CRL_CACHE.get(hash, encoding);
            if (res != NULL) {
                *crl = res;
                REFCOUNT_ADD(*crl)
                return NOERROR;
            }
        }
        AutoPtr<IX509CRL> res;
        CX509CRLImpl::New(encoding, (IX509CRL**)&res);
        CRL_CACHE->Put(hash, encoding, res.Get());
        *crl = res;
        REFCOUNT_ADD(*crl)
        return NOERROR;
    }
}

ECode CX509CertFactoryImpl::GetCRL(
    /* [in] */ IInputStream* inStream,
    /* [out] */ ICRL** crl)
{
    AutoLock lock(mLock);
    inStream->Mark(CRL_CACHE_SEED_LENGTH);
    AutoPtr<ArrayOf<Byte> > buff;
    ReadBytes(inStream, CRL_CACHE_SEED_LENGTH, (ArrayOf<Byte>**)&buff);
    // read the prefix of the encoding
    inStream->Reset();
    if (buff == NULL) {
        //throw new CRLException("InputStream doesn't contain enough data");
        return E_CRL_EXCEPTION;
    }
    Int64 hash;
    CRL_CACHE->GetHash(buff, &hash);
    Boolean isContains;
    if (CRL_CACHE->Contains(hash, &isContains), isContains) {
        Int32 len;
        CBerInputStream::GetLength(buff, &len);
        AutoPtr<ArrayOf<Byte> > encoding = ArrayOf<Byte>::Alloc(len);
        if (encoding->GetLength() < CRL_CACHE_SEED_LENGTH) {
            //throw new CRLException("Bad CRL encoding");
            return E_CRL_EXCEPTION;
        }
        AutoPtr<IStreams> streams;
        CStreams::AcquireSingleton((IStreams**)&streams);
        streams->ReadFully(inStream, encoding);
        AutoPtr<IInterface> tmp;
        CRL_CACHE->Get(hash, encoding, (IInterface**)&tmp);
        AutoPtr<ICRL> res = ICRL::Probe(tmp);
        if (res != NULL) {
            *crl = res.Get();
            REFCOUNT_ADD(*crl)
            return NOERROR;
        }
        res = NULL;
        AutoPtr<IX509CRL> impl;
        CX509CRLImpl::New(encoding, (IX509CRL**)&impl);
        res = impl.Get();
        CRL_CACHE->Put(hash, encoding, res.Get());
        *crl = res.Get();
        REFCOUNT_ADD(*crl)
        return NOERROR;
    } else {
        AutoPtr<IX509CRL> res;
        CX509CRLImpl::New(inStream, (IX509CRL**)&res);
        AutoPtr<ArrayOf<Byte> > bytes;
        res->GetEncoded((ArrayOf<Byte>**)&bytes);
        CRL_CACHE->Put(hash, bytes, res.Get());
        *crl = res.Get();
        REFCOUNT_ADD(*crl)
        return NOERROR;
    }
}

} // namespace Cert
} // namespace Provider
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

