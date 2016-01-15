
#ifndef __ELASTOS_SECURITY_CERT_CERTIFICATEFACTORY_H__
#define __ELASTOS_SECURITY_CERT_CERTIFICATEFACTORY_H__

#include "core/Object.h"

using Elastos::Core::Object;
using Elastos::IO::IInputStream;
using Elastos::Security::IProvider;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::ICollection;
using Org::Apache::Harmony::Security::Fortress::IEngine;

namespace Elastos {
namespace Security {
namespace Cert {

class CertificateFactory
    : public Object
    , public ICertificateFactory
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    /**
     * Creates a new {@code CertificateFactory} instance that provides the
     * requested certificate type.
     *
     * @param type
     *            the certificate type.
     * @return the new {@code CertificateFactory} instance.
     * @throws CertificateException
     *             if the specified certificate type is not available at any
     *             installed provider.
     * @throws NullPointerException if {@code type == null}
     */
    static CARAPI GetInstance(
        /* [in] */ const String& type,
        /* [out] */ ICertificateFactory** certFact);

    /**
     * Creates a new {@code CertificateFactory} instance from the specified
     * provider that provides the requested certificate type.
     *
     * @param type
     *            the certificate type.
     * @param provider
     *            the name of the provider providing certificates of the
     *            specified type.
     * @return the new {@code CertificateFactory} instance.
     * @throws CertificateException
     *             if the specified certificate type is not available by the
     *             specified provider.
     * @throws NoSuchProviderException
     *             if no provider with the specified name can be found.
     * @throws IllegalArgumentException if {@code provider == null || provider.isEmpty()}
     * @throws NullPointerException
     *             it {@code type} is {@code null}.
     */
    static CARAPI GetInstance(
        /* [in] */ const String& type,
        /* [in] */ const String& provider,
        /* [out] */ ICertificateFactory** certFact);

    /**
     * Creates a new {@code CertificateFactory} instance from the specified
     * provider that provides the requested certificate type.
     *
     * @param type
     *            the certificate type.
     * @param provider
     *            the name of the provider providing certificates of the
     *            specified type.
     * @return the new {@code CertificateFactory} instance.
     * @throws CertificateException
     *             if the specified certificate type is not available at the
     *             specified provider.
     * @throws IllegalArgumentException
     *             if the specified provider is {@code null}.
     * @throws NullPointerException if {@code type == null}
     * @throws IllegalArgumentException if {@code provider == null}
     */
    static CARAPI GetInstance(
        /* [in] */ const String& type,
        /* [in] */ IProvider* provider,
        /* [out] */ ICertificateFactory** certFact);

    /**
     * Returns the {@code Provider} of the certificate factory represented by
     * the certificate.
     *
     * @return the provider of this certificate factory.
     */
    CARAPI GetProvider(
        /* [out] */ IProvider** provider);

    /**
     * Returns the Certificate type.
     *
     * @return type of certificate being used.
     */
    CARAPI GetType(
        /* [out] */ String* type);

    /**
     * Generates and initializes a {@code Certificate} from the provided input
     * stream.
     *
     * @param inStream
     *            the stream from where data is read to create the {@code
     *            Certificate}.
     * @return an initialized Certificate.
     * @throws CertificateException
     *             if parsing problems are detected.
     */
    CARAPI GenerateCertificate(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICertificate** cert);

    /**
     * Returns an {@code Iterator} over the supported {@code CertPath} encodings
     * (as Strings). The first element is the default encoding scheme to apply.
     *
     * @return an iterator over supported {@link CertPath} encodings (as
     *         Strings).
     */
    CARAPI GetCertPathEncodings(
        /* [out] */ IIterator** it);

    /**
     * Generates a {@code CertPath} (a certificate chain) from the provided
     * {@code InputStream}. The default encoding scheme is applied.
     *
     * @param inStream
     *            {@code InputStream} with encoded data.
     * @return a {@code CertPath} initialized from the provided data.
     * @throws CertificateException
     *             if parsing problems are detected.
     */
    CARAPI GenerateCertPath(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICertPath** certPath);

    /**
     * Generates a {@code CertPath} (a certificate chain) from the provided
     * {@code InputStream} and the specified encoding scheme.
     *
     * @param inStream
     *            {@code InputStream} containing certificate path data in
     *            specified encoding.
     * @param encoding
     *            encoding of the data in the input stream.
     * @return a {@code CertPath} initialized from the provided data.
     * @throws CertificateException
     *             if parsing problems are detected.
     * @throws UnsupportedOperationException
     *             if the provider does not implement this method.
     */
    CARAPI GenerateCertPath(
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
    CARAPI GenerateCertPath(
        /* [in] */ IList* certificates,
        /* [out] */ ICertPath** certPath);

    /**
     * Generates and initializes a collection of (unrelated) certificates from
     * the provided input stream.
     *
     * @param inStream
     *            the stream from which the data is read to create the
     *            collection.
     * @return an initialized collection of certificates.
     * @throws CertificateException
     *             if parsing problems are detected.
     */
    CARAPI GenerateCertificates(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICollection** certs);

    /**
     * Generates and initializes a <i>Certificate Revocation List</i> (CRL) from
     * the provided input stream.
     *
     * @param inStream
     *            the stream from where data is read to create the CRL.
     * @return an initialized CRL.
     * @exception CRLException
     *                if parsing problems are detected.
     */
    CARAPI GenerateCRL(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICRL** crl);

    /**
     * Generates and initializes a collection of <i>Certificate Revocation
     * List</i> (CRL) from the provided input stream.
     *
     * @param inStream
     *            the stream from which the data is read to create the CRLs.
     * @return an initialized collection of CRLs.
     * @exception CRLException
     *                if parsing problems are detected.
     */
    CARAPI GenerateCRLs(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICollection** crls);

protected:
    /**
     * Creates a new {@code CertificateFactory} instance.
     *
     * @param certFacSpi
     *            the implementation delegate.
     * @param provider
     *            the associated provider.
     * @param type
     *            the certificate type.
     */
    CertificateFactory(
        /* [in] */ ICertificateFactorySpi* certFacSpi,
        /* [in] */ IProvider* provider,
        /* [in] */ const String& type);

private:
    static CARAPI_(AutoPtr<IEngine>) InitENGINE();

private:
    // Store CertificateFactory service name
    static const String SERVICE;

    // Used to access common engine functionality
    static const AutoPtr<IEngine> ENGINE;

    // Store used provider
    const AutoPtr<IProvider> mProvider;

    // Store used CertificateFactorySpi implementation
    const AutoPtr<ICertificateFactorySpi> mSpiImpl;

    // Store used type
    const String mType;
};

} // end Cert
} // end Security
} // end Elastos
#endif // __ELASTOS_SECURITY_CERT_CERTIFICATEFACTORY_H__
