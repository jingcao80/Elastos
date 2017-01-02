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

#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "OpenSSLX509CertPath.h"
#include "NativeCrypto.h"
#include "OpenSSLX509Certificate.h"
#include <elastos/utility/Arrays.h>

using Elastos::IO::IPushbackInputStream;
using Elastos::IO::CPushbackInputStream;
using Elastos::Utility::Arrays;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::CArrayList;
using Elastos::Security::Cert::IX509Certificate;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLX509CertPath::
//=========================================
static AutoPtr<ArrayOf<Byte> > Init_PKCS7_MARKER()
{
    AutoPtr<ArrayOf<Byte> > arr = ArrayOf<Byte>::Alloc(16);
    (*arr)[0] = '-';
    (*arr)[1] = '-';
    (*arr)[2] = '-';
    (*arr)[3] = '-';
    (*arr)[4] = '-';
    (*arr)[5] = 'B';
    (*arr)[6] = 'E';
    (*arr)[7] = 'G';
    (*arr)[8] = 'I';
    (*arr)[9] = 'N';
    (*arr)[10] = ' ';
    (*arr)[11] = 'P';
    (*arr)[12] = 'K';
    (*arr)[13] = 'C';
    (*arr)[14] = 'S';
    (*arr)[15] = '7';
    return arr;
}
AutoPtr<ArrayOf<Byte> > OpenSSLX509CertPath::PKCS7_MARKER = Init_PKCS7_MARKER();

Int32 OpenSSLX509CertPath::PUSHBACK_SIZE = 64;

// private enum Encoding {
//     PKI_PATH("PkiPath"),
//     PKCS7("PKCS7");

//     private String apiName;

//     Encoding(String apiName) {
//         this.apiName = apiName;
//     }

//     static Encoding findByApiName(String apiName) throws CertificateEncodingException {
//         for (Encoding element : values()) {
//             if (element.apiName.equals(apiName)) {
//                 return element;
//             }
//         }

//         return NULL;
//     }
// }

static AutoPtr<IList> Init_ALL_ENCODINGS()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(2);
    (*arr)[0] = String("PkiPath");    // OpenSSLX509CertPath::PKI_PATH;
    (*arr)[1] = String("PKCS7");    // OpenSSLX509CertPath::PKCS7;
    AutoPtr<IList> l;
    Arrays::AsList(arr, (IList**)&l);
    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);
    AutoPtr<IList> res;
    cls->UnmodifiableList(l, (IList**)&res);
    return res;
}
AutoPtr<IList> OpenSSLX509CertPath::ALL_ENCODINGS = Init_ALL_ENCODINGS();

OpenSSLX509CertPath::Encoding OpenSSLX509CertPath::DEFAULT_ENCODING = PKI_PATH;

CAR_INTERFACE_IMPL(OpenSSLX509CertPath, Object, IOpenSSLX509CertPath)

ECode OpenSSLX509CertPath::GetEncodingsIterator(
    /* [out] */ IIterator** result)
{
    VALIDATE_NOT_NULL(result)
    return ALL_ENCODINGS->GetIterator(result);
}

ECode OpenSSLX509CertPath::constructor(
    /* [in] */ IList* certificates)
//    : CertPath("X.509")
{
    mCertificates = certificates;
    return NOERROR;
}

ECode OpenSSLX509CertPath::GetCertificates(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);
    return cls->UnmodifiableList(mCertificates, result);
}

AutoPtr<ArrayOf<Byte> > OpenSSLX509CertPath::GetEncoded(
    /* [in] */ Encoding encoding)
{
    Int32 size = 0;
    mCertificates->GetSize(&size);
    AutoPtr<ArrayOf<IOpenSSLX509Certificate*> > certs =
                ArrayOf<IOpenSSLX509Certificate*>::Alloc(size);
    AutoPtr<ArrayOf<Int64> > certRefs = ArrayOf<Int64>::Alloc(certs->GetLength());

    for (Int32 i = 0, j = certs->GetLength() - 1; j >= 0; i++, j--) {
        AutoPtr<IInterface> p;
        mCertificates->Get(i, (IInterface**)&p);
        AutoPtr<IX509Certificate> cert = IX509Certificate::Probe(p);

        if (IOpenSSLX509Certificate::Probe(cert) != NULL) {
            (*certs)[j] = IOpenSSLX509Certificate::Probe(cert);
        }
        else {
            AutoPtr<ArrayOf<Byte> > encoded;
            ICertificate::Probe(cert)->GetEncoded((ArrayOf<Byte>**)&encoded);
            OpenSSLX509Certificate::FromX509Der(encoded, (IOpenSSLX509Certificate**)&((*certs)[j]));
        }

        (*certs)[j]->GetContext(&((*certRefs)[j]));
    }

    switch (encoding) {
        case PKI_PATH: {
            AutoPtr<ArrayOf<Byte> > res;
            NativeCrypto::ASN1_seq_pack_X509(certRefs, (ArrayOf<Byte>**)&res);
            return res;
        }
        case PKCS7: {
            AutoPtr<ArrayOf<Byte> > res;
            NativeCrypto::I2d_PKCS7(certRefs, (ArrayOf<Byte>**)&res);
            return res;
        }
        default: {
            // throw new CertificateEncodingException("Unknown encoding");
            return NULL;
        }
    }
}

ECode OpenSSLX509CertPath::GetEncoded(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<Byte> > res = GetEncoded(DEFAULT_ENCODING);
    *result = res;
    return NOERROR;
}

ECode OpenSSLX509CertPath::GetEncoded(
    /* [in] */ String encoding,
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    assert(0 && "TODO");
    // Encoding enc = Encoding::FindByApiName(encoding);
    // if (enc == NULL) {
    //     // throw new CertificateEncodingException("Invalid encoding: " + encoding);
    //     *result = NULL;
    //     return NOERROR;
    // }

    AutoPtr<ArrayOf<Byte> > res;
    assert(0 && "TODO");
    // GetEncoded(enc, (ArrayOf<Byte>**)&res);
    *result = res;
    return NOERROR;
}

ECode OpenSSLX509CertPath::GetEncodings(
    /* [out] */ IIterator** result)
{
    VALIDATE_NOT_NULL(result)
    return GetEncodingsIterator(result);
}

AutoPtr<ICertPath> OpenSSLX509CertPath::FromPkiPathEncoding(
    /* [in] */ IInputStream* inStream)
{
    AutoPtr<IOpenSSLBIOInputStream> bis;
    assert(0 && "TODO");
    // COpenSSLBIOInputStream::New(inStream, (IOpenSSLBIOInputStream**)&bis);

    Boolean markable = FALSE;
    inStream->IsMarkSupported(&markable);
    if (markable) {
        inStream->Mark(PUSHBACK_SIZE);
    }

    AutoPtr<ArrayOf<Int64> > certRefs;
    // try {
    Int64 context = 0;
    bis->GetBioContext(&context);
    NativeCrypto::ASN1_seq_unpack_X509_bio(context, (ArrayOf<Int64>**)&certRefs);
    // } catch (Exception e) {
    //     if (markable) {
    //         try {
    //             inStream.reset();
    //         } catch (IOException ignored) {
    //         }
    //     }
    //     throw new CertificateException(e);
    // } finally {
        bis->Release();
    // }

    if (certRefs == NULL) {
        AutoPtr<ICollections> cls;
        CCollections::AcquireSingleton((ICollections**)&cls);
        AutoPtr<IList> emplist;
        cls->GetEmptyList((IList**)&emplist);
        AutoPtr<IOpenSSLX509CertPath> p;
        assert(0 && "TODO");
        // COpenSSLX509CertPath::New(IX509Certificate::Probe(emplist), (IOpenSSLX509CertPath**)&p);
        return ICertPath::Probe(p);
    }

    AutoPtr<IList> certs;
    CArrayList::New(certRefs->GetLength(), (IList**)&certs);
    for (Int32 i = certRefs->GetLength() - 1; i >= 0; i--) {
        if ((*certRefs)[i] == 0) {
            continue;
        }
        AutoPtr<IOpenSSLX509Certificate> p;
        assert(0 && "TODO");
        // COpenSSLX509Certificate::New((*certRefs)[i], (IOpenSSLX509Certificate**)&p);
        certs->Add(p);
    }

    AutoPtr<IOpenSSLX509CertPath> res;
    assert(0 && "TODO");
    // COpenSSLX509CertPath::New(certs, (IOpenSSLX509CertPath**)&res);
    return ICertPath::Probe(res);
}

AutoPtr<ICertPath> OpenSSLX509CertPath::FromPkcs7Encoding(
    /* [in] */ IInputStream* inStream)
{
    // try {
    Int32 avail = 0;
    if (inStream == NULL || (inStream->Available(&avail), avail) == 0) {
        AutoPtr<ICollections> cls;
        CCollections::AcquireSingleton((ICollections**)&cls);
        AutoPtr<IList> emplist;
        cls->GetEmptyList((IList**)&emplist);
        AutoPtr<IOpenSSLX509CertPath> p;
        assert(0 && "TODO");
        // COpenSSLX509CertPath::New(IX509Certificate::Probe(emplist), (IOpenSSLX509CertPath**)&p);
        return ICertPath::Probe(p);
    }
    // } catch (IOException e) {
    //     throw new CertificateException("Problem reading input stream", e);
    // }

    Boolean markable = FALSE;
    inStream->IsMarkSupported(&markable);
    if (markable) {
        inStream->Mark(PUSHBACK_SIZE);
    }

    /* Attempt to see if this is a PKCS#7 bag. */
    AutoPtr<IPushbackInputStream> pbis;
    CPushbackInputStream::New(inStream, PUSHBACK_SIZE, (IPushbackInputStream**)&pbis);
    // try {
        AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(PKCS7_MARKER->GetLength());

        Int32 len = 0;
        IInputStream::Probe(pbis)->Read(buffer, &len);
        if (len < 0) {
            /* No need to reset here. The stream was empty or EOF. */
            // throw new ParsingException("inStream is empty");
            return NULL;
        }
        pbis->Unread(buffer, 0, len);

        if (len == PKCS7_MARKER->GetLength() && Arrays::Equals(PKCS7_MARKER, buffer)) {
            AutoPtr<IOpenSSLX509CertPath> p;
            assert(0 && "TODO");
            // COpenSSLX509CertPath::New(OpenSSLX509Certificate::FromPkcs7PemInputStream(pbis), (IOpenSSLX509CertPath**)&p);
            return ICertPath::Probe(p);
        }

        AutoPtr<IOpenSSLX509CertPath> res;
        assert(0 && "TODO");
        // COpenSSLX509CertPath::New(OpenSSLX509Certificate::FromPkcs7DerInputStream(pbis), (IOpenSSLX509CertPath**)&res);
        return ICertPath::Probe(res);
    // } catch (Exception e) {
    //     if (markable) {
    //         try {
    //             inStream.reset();
    //         } catch (IOException ignored) {
    //         }
    //     }
    //     throw new CertificateException(e);
    // }
}

AutoPtr<ICertPath> OpenSSLX509CertPath::FromEncoding(
    /* [in] */ IInputStream* inStream,
    /* [in] */ Encoding encoding)
{
    switch (encoding) {
        case PKI_PATH:
            return FromPkiPathEncoding(inStream);
        case PKCS7:
            return FromPkcs7Encoding(inStream);
        default: {
            // throw new CertificateEncodingException("Unknown encoding");
            return NULL;
        }
    }
}

ECode OpenSSLX509CertPath::FromEncoding(
    /* [in] */ IInputStream* inStream,
    /* [in] */ String encoding,
    /* [out] */ ICertPath** result)
{
    VALIDATE_NOT_NULL(result)
    if (inStream == NULL) {
        // throw new CertificateException("inStream == null");
        return E_CERTIFICATE_EXCEPTION;
    }

    assert(0 && "TODO");
    // Encoding enc = Encoding::FindByApiName(encoding);
    // if (enc == NULL) {
    //     // throw new CertificateException("Invalid encoding: " + encoding);
    //     return E_CERTIFICATE_EXCEPTION;
    // }

    // return FromEncoding(inStream, enc, result);
    return NOERROR;
}

ECode OpenSSLX509CertPath::FromEncoding(
    /* [in] */ IInputStream* inStream,
    /* [out] */ ICertPath** result)
{
    VALIDATE_NOT_NULL(result)
    assert(0 && "TODO");
    // return FromEncoding(inStream, DEFAULT_ENCODING, result);
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
