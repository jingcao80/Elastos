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

#include "OpenSSLX509CertificateFactory.h"
#include "OpenSSLX509CertPath.h"
#include "OpenSSLX509Certificate.h"
#include "OpenSSLX509CRL.h"
#include <elastos/utility/Arrays.h>

using Elastos::IO::IPushbackInputStream;
using Elastos::IO::CPushbackInputStream;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Utility::Arrays;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::CArrayList;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLX509CertificateFactory::ParsingException::
//=========================================

Int64 OpenSSLX509CertificateFactory::ParsingException::mSerialVersionUID = 8390802697728301325L;

OpenSSLX509CertificateFactory::ParsingException::ParsingException(
    /* [in] */ String message)
//    : Exception(message)
{
}

// OpenSSLX509CertificateFactory::ParsingException::ParsingException(
//     /* [in] */ IException* cause)
//     : Exception(cause)
// {
// }

// OpenSSLX509CertificateFactory::ParsingException::ParsingException(
//     /* [in] */ String message,
//     /* [in] */ IException* cause)
//     : Exception(message, cause)
// {
// }

//=========================================
// OpenSSLX509CertificateFactory::Parser::
//=========================================

ECode OpenSSLX509CertificateFactory::Parser::GenerateItem(
    /* [in] */ IInputStream* inStream,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    if (inStream == NULL) {
        // throw new ParsingException("inStream == NULL");
        Logger::E("OpenSSLX509CertificateFactory", "ParsingException inStream == NULL");
        return E_CERTIFICATE_PARSING_EXCEPTION;
    }

    Boolean markable = FALSE;
    inStream->IsMarkSupported(&markable);
    if (markable) {
        inStream->Mark(PKCS7_MARKER->GetLength());
    }

    AutoPtr<IPushbackInputStream> pbis;
    CPushbackInputStream::New(inStream, PUSHBACK_SIZE, (IPushbackInputStream**)&pbis);
    // try {
        AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(PKCS7_MARKER->GetLength());

        Int32 len = 0;
        IInputStream::Probe(pbis)->Read(buffer, &len);
        if (len < 0) {
            /* No need to reset here. The stream was empty or EOF. */
            // throw new ParsingException("inStream is empty");
            Logger::E("OpenSSLX509CertificateFactory", "ParsingException inStream is empty");
            return E_CERTIFICATE_PARSING_EXCEPTION;
        }
        pbis->Unread(buffer, 0, len);

        if ((*buffer)[0] == '-') {
            if (len == PKCS7_MARKER->GetLength() && Arrays::Equals(PKCS7_MARKER, buffer)) {
                AutoPtr<IList> items;
                FromPkcs7PemInputStream(IInputStream::Probe(pbis), (IList**)&items);
                Int32 size = 0;
                items->GetSize(&size);
                if (size == 0) {
                    *result = NULL;
                    return NOERROR;
                }
                AutoPtr<IInterface> p;
                items->Get(0, (IInterface**)&p);
            }
            else {
                return FromX509PemInputStream(IInputStream::Probe(pbis), result);
            }
        }

        /* PKCS#7 bags have a byte 0x06 at position 4 in the stream. */
        if ((*buffer)[4] == 0x06) {
            AutoPtr<IList> certs;
            FromPkcs7DerInputStream(IInputStream::Probe(pbis), (IList**)&certs);
            Int32 size = 0;
            certs->GetSize(&size);
            if (size == 0) {
                *result = NULL;
                return NOERROR;
            }
            return certs->Get(0, result);
        }
        else {
            return FromX509DerInputStream(IInputStream::Probe(pbis), result);
        }
    // } catch (Exception e) {
    //     if (markable) {
    //         try {
    //             inStream.reset();
    //         } catch (IOException ignored) {
    //         }
    //     }
    //     throw new ParsingException(e);
    // }
    return NOERROR;
}

ECode OpenSSLX509CertificateFactory::Parser::GenerateItems(
    /* [in] */ IInputStream* inStream,
    /* [out] */ ICollection** result)
{
    VALIDATE_NOT_NULL(result)
    if (inStream == NULL) {
        // throw new ParsingException("inStream == NULL");
        Logger::E("OpenSSLX509CertificateFactory", "ParsingException inStream == NULL");
        return E_CERTIFICATE_PARSING_EXCEPTION;
    }

    ECode ec = NOERROR;
    // try
    {
        Int32 avail = 0;
        FAIL_GOTO(ec = inStream->Available(&avail), ERROR1)
        if (avail == 0) {
            AutoPtr<ICollections> cls;
            CCollections::AcquireSingleton((ICollections**)&cls);
            AutoPtr<IList> res;
            cls->GetEmptyList((IList**)&res);
            *result = ICollection::Probe(res);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    // catch (IOException e)
ERROR1:
    if ((ECode)E_IO_EXCEPTION == ec) {
        //throw new ParsingException("Problem reading input stream", e);
        Logger::E("OpenSSLX509CertificateFactory", "ParsingException Problem reading input stream");
        return E_CERTIFICATE_PARSING_EXCEPTION;
    }

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
        Logger::E("OpenSSLX509CertificateFactory", "ParsingException inStream is empty");
        return E_CERTIFICATE_PARSING_EXCEPTION;
    }
    pbis->Unread(buffer, 0, len);
    if (len == PKCS7_MARKER->GetLength() && Arrays::Equals(PKCS7_MARKER, buffer)) {
        AutoPtr<IList> res;
        FromPkcs7PemInputStream(IInputStream::Probe(pbis), (IList**)&res);
        *result = ICollection::Probe(res);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    /* PKCS#7 bags have a byte 0x06 at position 4 in the stream. */
    if ((*buffer)[4] == 0x06) {
        AutoPtr<IList> res;
        FromPkcs7DerInputStream(IInputStream::Probe(pbis), (IList**)&res);
        *result = ICollection::Probe(res);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    // } catch (Exception e) {
    //     if (markable) {
    //         try {
    //             inStream.reset();
    //         } catch (IOException ignored) {
    //         }
    //     }
    //     throw new ParsingException(e);
    // }

    /*
     * It wasn't, so just try to keep grabbing certificates until we
     * can't anymore.
     */
    AutoPtr<IList> coll;
    CArrayList::New((IList**)&coll);
    AutoPtr<IInterface> c;
    do {
        /*
         * If this stream supports marking, try to mark here in case
         * there is an error during certificate generation.
         */
        if (markable) {
            inStream->Mark(PUSHBACK_SIZE);
        }

        // try
        ECode ec = NOERROR;
        {
            FAIL_GOTO(ec = GenerateItem(IInputStream::Probe(pbis), (IInterface**)&c), ERROR2)
            coll->Add(c);
        } //catch (ParsingException e) {
    ERROR2:
        if ((ECode)E_CERTIFICATE_PARSING_EXCEPTION == ec) {
            /*
             * If this stream supports marking, attempt to reset it to
             * the mark before the failure.
             */
            if (markable) {
                //try {
                inStream->Reset();
                //} catch (IOException ignored) {
                //}
            }

            c = NULL;
        }
    } while (c != NULL);

    *result = ICollection::Probe(coll);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//=========================================
// OpenSSLX509CertificateFactory::Parser_OpenSSLX509Certificate::
//=========================================

ECode OpenSSLX509CertificateFactory::Parser_OpenSSLX509Certificate::FromX509PemInputStream(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLX509Certificate> res;
    OpenSSLX509Certificate::FromX509PemInputStream(is, (IOpenSSLX509Certificate**)&res);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509CertificateFactory::Parser_OpenSSLX509Certificate::FromX509DerInputStream(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLX509Certificate> res;
    OpenSSLX509Certificate::FromX509DerInputStream(is, (IOpenSSLX509Certificate**)&res);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509CertificateFactory::Parser_OpenSSLX509Certificate::FromPkcs7PemInputStream(
    /* [in] */ IInputStream* is,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    return OpenSSLX509Certificate::FromPkcs7PemInputStream(is, result);
}

ECode OpenSSLX509CertificateFactory::Parser_OpenSSLX509Certificate::FromPkcs7DerInputStream(
    /* [in] */ IInputStream* is,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    return OpenSSLX509Certificate::FromPkcs7DerInputStream(is, result);
}

//=========================================
// OpenSSLX509CertificateFactory::Parser_OpenSSLX509CRL::
//=========================================

ECode OpenSSLX509CertificateFactory::Parser_OpenSSLX509CRL::FromX509PemInputStream(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLX509CRL> res;
    OpenSSLX509CRL::FromX509PemInputStream(is, (IOpenSSLX509CRL**)&res);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509CertificateFactory::Parser_OpenSSLX509CRL::FromX509DerInputStream(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLX509CRL> res;
    OpenSSLX509CRL::FromX509DerInputStream(is, (IOpenSSLX509CRL**)&res);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509CertificateFactory::Parser_OpenSSLX509CRL::FromPkcs7PemInputStream(
    /* [in] */ IInputStream* is,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    return OpenSSLX509CRL::FromPkcs7PemInputStream(is, result);
}

ECode OpenSSLX509CertificateFactory::Parser_OpenSSLX509CRL::FromPkcs7DerInputStream(
    /* [in] */ IInputStream* is,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    return OpenSSLX509CRL::FromPkcs7DerInputStream(is, result);
}

//=========================================
// OpenSSLX509CertificateFactory::
//=========================================

static AutoPtr<ArrayOf<Byte> > InitPKCS7_MARKER()
{
    AutoPtr<ArrayOf<Byte> > res = ArrayOf<Byte>::Alloc(16);
    (*res)[0] = '-';
    (*res)[1] = '-';
    (*res)[2] = '-';
    (*res)[3] = '-';
    (*res)[4] = '-';
    (*res)[5] = 'B';
    (*res)[6] = 'E';
    (*res)[7] = 'G';
    (*res)[8] = 'I';
    (*res)[9] = 'N';
    (*res)[10] = ' ';
    (*res)[11] = 'P';
    (*res)[12] = 'K';
    (*res)[13] = 'C';
    (*res)[14] = 'S';
    (*res)[15] = '7';
    return res;
}
AutoPtr<ArrayOf<Byte> > OpenSSLX509CertificateFactory::PKCS7_MARKER = InitPKCS7_MARKER();

Int32 OpenSSLX509CertificateFactory::PUSHBACK_SIZE = 64;

CAR_INTERFACE_IMPL(OpenSSLX509CertificateFactory, CertificateFactorySpi, IOpenSSLX509CertificateFactory)

OpenSSLX509CertificateFactory::OpenSSLX509CertificateFactory()
{
    mCertificateParser = new Parser_OpenSSLX509Certificate();

    mCrlParser = new Parser_OpenSSLX509CRL();
}

ECode OpenSSLX509CertificateFactory::EngineGenerateCertificate(
    /* [in] */ IInputStream* inStream,
    /* [out] */ ICertificate** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    AutoPtr<IInterface> res;
    mCertificateParser->GenerateItem(inStream, (IInterface**)&res);
    *result = ICertificate::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
    // } catch (ParsingException e) {
    //     throw new CertificateException(e);
    // }
}

ECode OpenSSLX509CertificateFactory::EngineGenerateCertificates(
    /* [in] */ IInputStream* inStream,
    /* [out] */ ICollection** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    return mCertificateParser->GenerateItems(inStream, result);
    // } catch (ParsingException e) {
    //     throw new CertificateException(e);
    // }
}

ECode OpenSSLX509CertificateFactory::EngineGenerateCRL(
    /* [in] */ IInputStream* inStream,
    /* [out] */ ICRL** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    AutoPtr<IInterface> res;
    mCrlParser->GenerateItem(inStream, (IInterface**)&res);
    *result = ICRL::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
    // } catch (ParsingException e) {
    //     throw new CRLException(e);
    // }
}

ECode OpenSSLX509CertificateFactory::EngineGenerateCRLs(
    /* [in] */ IInputStream* inStream,
    /* [out] */ ICollection** result)
{
    VALIDATE_NOT_NULL(result)
    if (inStream == NULL) {
        AutoPtr<ICollections> cls;
        CCollections::AcquireSingleton((ICollections**)&cls);
        AutoPtr<IList> l;
        cls->GetEmptyList((IList**)&l);
        *result = ICollection::Probe(l);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    // try {
    return mCrlParser->GenerateItems(inStream, result);
    // } catch (ParsingException e) {
    //     throw new CRLException(e);
    // }
}

ECode OpenSSLX509CertificateFactory::EngineGetCertPathEncodings(
    /* [out] */ IIterator** result)
{
    VALIDATE_NOT_NULL(result)
    return OpenSSLX509CertPath::GetEncodingsIterator(result);
}

ECode OpenSSLX509CertificateFactory::EngineGenerateCertPath(
    /* [in] */ IInputStream* inStream,
    /* [out] */ ICertPath** result)
{
    VALIDATE_NOT_NULL(result)
    return OpenSSLX509CertPath::FromEncoding(inStream, result);
}

ECode OpenSSLX509CertificateFactory::EngineGenerateCertPath(
    /* [in] */ IInputStream* inStream,
    /* [in] */ const String& encoding,
    /* [out] */ ICertPath** result)
{
    VALIDATE_NOT_NULL(result)
    return OpenSSLX509CertPath::FromEncoding(inStream, encoding, result);
}

ECode OpenSSLX509CertificateFactory::EngineGenerateCertPath(
    /* [in] */ IList* certificates,
    /* [out] */ ICertPath** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 size = 0;
    certificates->GetSize(&size);
    AutoPtr<IList> filtered;
    CArrayList::New(size, (IList**)&filtered);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> p;
        certificates->Get(i, (IInterface**)&p);
        AutoPtr<ICertificate> c = ICertificate::Probe(p);

        if (IX509Certificate::Probe(c) == NULL) {
            // throw new CertificateException("Certificate not X.509 type at index " + i);
            return E_CERTIFICATE_EXCEPTION;
        }

        filtered->Add(IX509Certificate::Probe(c));
    }

    AutoPtr<IOpenSSLX509CertPath> path;
    assert(0 && "TODO");
    // COpenSSLX509CertPath::New(filtered, (IOpenSSLX509CertPath**)&path);
    *result = ICertPath::Probe(path);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
