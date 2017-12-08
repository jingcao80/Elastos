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

#ifndef __ELASTOSX_SECURITY_CERT_X509CERTIFICATEX_H__
#define __ELASTOSX_SECURITY_CERT_X509CERTIFICATEX_H__

#include "CertificateX.h"

using Elastos::IO::IInputStream;
using Elastos::Math::IBigInteger;
using Elastos::Utility::IDate;
using Elastos::Security::IPrincipal;

namespace Elastosx {
namespace Security {
namespace Cert {

/**
 * Abstract base class for X.509 certificates.
 * <p>
 * This represents a standard way for accessing the attributes of X.509 v1
 * certificates.
 * <p>
 * Note: This package is provided only for compatibility reasons.
 * It contains a simplified version of the java.security.cert package that was
 * previously used by JSSE (Java SSL package). All applications that do not have
 * to be compatible with older versions of JSSE (that is before Java SDK 1.5)
 * should only use java.security.cert.
 */
class X509Certificate
    : public Certificate
    , public IX509Certificate
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates a new {@code X509Certificate} and initializes it from the
     * specified input stream.
     *
     * @param inStream
     *            input stream containing data to initialize the certificate.
     * @return the certificate initialized from the specified input stream
     * @throws CertificateException
     *             if the certificate cannot be created or initialized.
     */
    static CARAPI GetInstance(
        /* [in] */ IInputStream* inStream,
        /* [out] */ IX509Certificate** cert);

    /**
     * Creates a new {@code X509Certificate} and initializes it from the
     * specified byte array.
     *
     * @param certData
     *            byte array containing data to initialize the certificate.
     * @return the certificate initialized from the specified byte array
     * @throws CertificateException
     *             if the certificate cannot be created or initialized.
     */
    static CARAPI GetInstance(
        /* [in] */ ArrayOf<Byte>* certData,
        /* [out] */ IX509Certificate** cert);

    /**
     * Creates a new {@code X509Certificate}.
     */
    X509Certificate() {};

    /**
     * Checks whether the certificate is currently valid.
     * <p>
     * The validity defined in ASN.1:
     *
     * <pre>
     * validity             Validity
     *
     * Validity ::= SEQUENCE {
     *      notBefore       CertificateValidityDate,
     *      notAfter        CertificateValidityDate }
     *
     * CertificateValidityDate ::= CHOICE {
     *      utcTime         UTCTime,
     *      generalTime     GeneralizedTime }
     * </pre>
     *
     * @throws CertificateExpiredException
     *             if the certificate has expired.
     * @throws CertificateNotYetValidException
     *             if the certificate is not yet valid.
     */
    virtual CARAPI CheckValidity() = 0;

    /**
     * Checks whether the certificate is valid at the specified date.
     *
     * @param date
     *            the date to check the validity against.
     * @throws CertificateExpiredException
     *             if the certificate has expired.
     * @throws CertificateNotYetValidException
     *             if the certificate is not yet valid.
     * @see #checkValidity()
     */
    virtual CARAPI CheckValidity(
        /* [in] */ IDate* date) = 0;

    /**
     * Returns the certificates {@code version} (version number).
     * <p>
     * The version defined is ASN.1:
     *
     * <pre>
     * Version ::=  INTEGER  {  v1(0), v2(1), v3(2)  }
     * </pre>
     *
     * @return the version number.
     */
    virtual CARAPI GetVersion(
        /* [out] */ Int32* version) = 0;

    /**
     * Returns the {@code serialNumber} of the certificate.
     * <p>
     * The ASN.1 definition of {@code serialNumber}:
     *
     * <pre>
     * CertificateSerialNumber  ::=  INTEGER
     * </pre>
     *
     * @return the serial number.
     */
    virtual CARAPI GetSerialNumber(
        /* [out] */ IBigInteger** number) = 0;

    /**
     * Returns the {@code issuer} (issuer distinguished name) as an
     * implementation specific {@code Principal} object.
     * <p>
     * The ASN.1 definition of {@code issuer}:
     *
     * <pre>
     *  issuer      Name
     *
     *  Name ::= CHOICE {
     *      RDNSequence }
     *
     *    RDNSequence ::= SEQUENCE OF RelativeDistinguishedName
     *
     *    RelativeDistinguishedName ::= SET OF AttributeTypeAndValue
     *
     *    AttributeTypeAndValue ::= SEQUENCE {
     *      type     AttributeType,
     *      value    AttributeValue }
     *
     *    AttributeType ::= OBJECT IDENTIFIER
     *
     *    AttributeValue ::= ANY DEFINED BY AttributeType
     * </pre>
     *
     * @return the {@code issuer} as an implementation specific {@code
     *         Principal}.
     */
    virtual CARAPI GetIssuerDN(
        /* [out] */ IPrincipal** dn) = 0;

    /**
     * Returns the {@code subject} (subject distinguished name) as an
     * implementation specific {@code Principal} object.
     * <p>
     * The ASN.1 definition of {@code subject}:
     *
     * <pre>
     * subject      Name
     *
     *  Name ::= CHOICE {
     *      RDNSequence }
     *
     *    RDNSequence ::= SEQUENCE OF RelativeDistinguishedName
     *
     *    RelativeDistinguishedName ::= SET OF AttributeTypeAndValue
     *
     *    AttributeTypeAndValue ::= SEQUENCE {
     *      type     AttributeType,
     *      value    AttributeValue }
     *
     *    AttributeType ::= OBJECT IDENTIFIER
     *
     *    AttributeValue ::= ANY DEFINED BY AttributeType
     * </pre>
     *
     * @return the {@code subject} (subject distinguished name).
     */
    virtual CARAPI GetSubjectDN(
        /* [out] */ IPrincipal** dn) = 0;

    /**
     * Returns the {@code notBefore} date from the validity period of the
     * certificate.
     *
     * @return the start of the validity period.
     */
   virtual CARAPI GetNotBefore(
        /* [out] */ IDate** dt) = 0;

    /**
     * Returns the {@code notAfter} date of the validity period of the
     * certificate.
     *
     * @return the end of the validity period.
     */
    virtual CARAPI GetNotAfter(
        /* [out] */ IDate** dt) = 0;

    /**
     * Returns the name of the algorithm for the certificate signature.
     *
     * @return the signature algorithm name.
     */
    virtual CARAPI GetSigAlgName(
        /* [out] */ String* name) = 0;

    /**
     * Returns the OID of the signature algorithm from the certificate.
     *
     * @return the OID of the signature algorithm.
     */
    virtual CARAPI GetSigAlgOID(
        /* [out] */ String* oid) = 0;

    /**
     * Returns the parameters of the signature algorithm in DER-encoded format.
     *
     * @return the parameters of the signature algorithm, or null if none are
     *         used.
     */

    virtual CARAPI GetSigAlgParams(
        /* [out, callee] */ ArrayOf<Byte>** params) = 0;

private:
    static CARAPI_(AutoPtr<IConstructorInfo>) StaticInitialize();

private:
    static AutoPtr<IConstructorInfo> sConstructor;
};

} // end Cert
} // end Security
} // end Elastosx

#endif // __ELASTOSX_SECURITY_CERT_X509CERTIFICATEX_H__