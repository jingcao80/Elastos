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

#ifndef __ELASTOS_SECURITY_CERT_CERTIFICATEFACTORYSPI_H__
#define __ELASTOS_SECURITY_CERT_CERTIFICATEFACTORYSPI_H__

#include "Elastos.CoreLibrary.Security.h"
#include "elastos/core/Object.h"

using Elastos::Core::Object;
using Elastos::IO::IInputStream;
using Elastos::Utility::ICollection;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Security {
namespace Cert {

/**
 * This class defines the <i>Service Provider Interface</i> (<b>SPI</b>) for the
 * {@code CertificateFactory} class. This SPI must be implemented for each
 * certificate type a security provider wishes to support.
 */
class ECO_PUBLIC CertificateFactorySpi
    : public Object
    , public ICertificateFactorySpi
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs a new instance of this class.
     */
    CertificateFactorySpi();

    /**
     * Generates and initializes a {@code Certificate} from the provided input
     * stream.
     *
     * @param inStream
     *            the stream from which the data is read to create the
     *            certificate.
     * @return an initialized certificate.
     * @exception CertificateException
     *                if parsing problems are detected.
     */
    virtual CARAPI EngineGenerateCertificate(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICertificate** cert) = 0;

    /**
     * Generates and initializes a collection of certificates from the provided
     * input stream.
     *
     * @param inStream
     *            the stream from where data is read to create the certificates.
     * @return a collection of certificates.
     * @exception CertificateException
     *                if parsing problems are detected.
     */
    virtual CARAPI EngineGenerateCertificates(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICollection** certs) = 0;

    /**
     * Generates and initializes a <i>Certificate Revocation List</i> (CRL) from
     * the provided input stream.
     *
     * @param inStream
     *            the stream from where data is read to create the CRL.
     * @return an CRL instance.
     * @exception CRLException
     *                if parsing problems are detected.
     */
    virtual CARAPI EngineGenerateCRL(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICRL** crl) = 0;

    /**
     * Generates and initializes a collection of <i>Certificate Revocation
     * List</i> (CRL) from the provided input stream.
     *
     * @param inStream
     *            the stream from which the data is read to create the CRLs.
     * @return a collection of CRLs.
     * @exception CRLException
     *                if parsing problems are detected.
     */
    virtual CARAPI EngineGenerateCRLs(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICollection** crls) = 0;

    /**
     * Generates a {@code CertPath} from the provided {@code InputStream}. The
     * default encoding scheme is applied.
     *
     * @param inStream
     *            an input stream with encoded data.
     * @return a {@code CertPath} initialized from the provided data.
     * @throws CertificateException
     *             if parsing problems are detected.
     */
    virtual CARAPI EngineGenerateCertPath(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICertPath** certPath);

    /**
     * Generates a {@code CertPath} from the provided {@code
     * InputStream} in the specified encoding.
     *
     * @param inStream
     *            an input stream containing certificate path data in specified
     *            encoding.
     * @param encoding
     *            the encoding of the data in the input stream.
     * @return a {@code CertPath} initialized from the provided data
     * @throws CertificateException
     *             if parsing problems are detected.
     * @throws UnsupportedOperationException
     *             if the provider does not implement this method.
     */
    virtual CARAPI EngineGenerateCertPath(
        /* [in] */ IInputStream* inStream,
        /* [in] */ const String& encoding,
        /* [out] */ ICertPath** certPath);

    /**
     * Generates a {@code CertPath} from the provided list of certificates. The
     * encoding is the default encoding.
     *
     * @param certificates
     *            the list containing certificates in a format supported by the
     *            {@code CertificateFactory}.
     * @return a {@code CertPath} initialized from the provided data.
     * @throws CertificateException
     *             if parsing problems are detected.
     * @throws UnsupportedOperationException
     *             if the provider does not implement this method.
     */
    virtual CARAPI EngineGenerateCertPath(
        /* [in] */ IList* certificates,
        /* [out] */ ICertPath** certPath);

    /**
     * Returns an {@code Iterator} over the supported {@code CertPath} encodings
     * (as Strings). The first element is the default encoding.
     *
     * @return an iterator over supported {@code CertPath} encodings (as
     *         Strings).
     */
     virtual CARAPI EngineGetCertPathEncodings(
        /* [out] */ IIterator** it);
};

} // end Cert
} // end Security
} // end Elastos

#endif // __ELASTOS_SECURITY_CERT_CERTIFICATEFACTORYSPI_H__
