
#ifndef __ELASTOS_SECURITY_CERT_CX509CERTSELECTOR_H__
#define __ELASTOS_SECURITY_CERT_CX509CERTSELECTOR_H__

#include "_Elastos_Security_Cert_CX509CertSelector.h"

using Elastos::Math::IBigInteger;
using Elastos::Utility::ISet;
using Elastos::Utility::IDate;
using Elastos::Security::IPublicKey;
using Elastos::Utility::ICollection;
using Elastosx::Security::Auth::X500::IX500Principal;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Security {
namespace Cert {

/**
 * A certificate selector ({@code CertSelector} for selecting {@code
 * X509Certificate}s that match the specified criteria.
 */
CarClass(CX509CertSelector)
    , public Object
    , public IX509CertSelector
    , public ICertSelector
    , public ICloneable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Creates a new {@code X509CertSelector}.
     */
    CX509CertSelector();

    /**
     * Sets the certificate that a matching certificate must be equal to.
     *
     * @param certificate
     *            the certificate to match, or null to not check this criteria.
     */
    CARAPI SetCertificate(
        /* [in] */ IX509Certificate *certificate);

    /**
     * Returns the certificate that a matching certificate must be equal to.
     *
     * @return the certificate to match, or null if this criteria is not
     *         checked.
     */
    CARAPI GetCertificate(
        /* [out] */ IX509Certificate **certificate);

    /**
     * Sets the serial number that a certificate must match.
     *
     * @param serialNumber
     *            the serial number to match, or {@code null} to not check the
     *            serial number.
     */
    CARAPI SetSerialNumber(
        /* [in] */ IBigInteger *serialNumber);

    /**
     * Returns the serial number that a certificate must match.
     *
     * @return the serial number to match, or {@code null} if the serial number
     *         is not to be checked.
     */
    CARAPI GetSerialNumber(
        /* [out] */ IBigInteger **serialNumber);

    /**
     * Sets the issuer that a certificate must match.
     *
     * @param issuer
     *            the issuer to match, or {@code null} if the issuer is not to
     *            be checked.
     */
    CARAPI SetIssuer(
        /* [in] */ IX500Principal *issuer);

    /**
     * Returns the issuer that a certificate must match.
     *
     * @return the issuer that a certificate must match, or {@code null} if the
     *         issuer is not to be checked.
     */
    CARAPI GetIssuer(
        /* [out] */ IX500Principal **issuer);

    /**
     * <b>Do not use</b>, use {@link #getIssuer()} or
     * {@link #getIssuerAsBytes()} instead. Sets the issuer that a certificate
     * must match.
     *
     * @param issuerName
     *            the issuer in a RFC 2253 format string, or {@code null} to not
     *            check the issuer.
     * @throws IOException
     *             if parsing the issuer fails.
     */
    CARAPI SetIssuerUsingString(
        /* [in] */ const String& issuerName);

    /**
     * <b>Do not use</b>, use {@link #getIssuer()} or
     * {@link #getIssuerAsBytes()} instead. Returns the issuer that a
     * certificate must match in a RFC 2253 format string.
     *
     * @return the issuer in a RFC 2253 format string, or {@code null} if the
     *         issuer is not to be checked.
     */
    CARAPI GetIssuerAsString(
        /* [out] */ String *issuer);

    /**
     * Sets the issuer that a certificate must match.
     *
     * @param issuerDN
     *            the distinguished issuer name in ASN.1 DER encoded format, or
     *            {@code null} to not check the issuer.
     * @throws IOException
     *             if decoding the issuer fail.
     */
    CARAPI SetIssuerUsingBytes(
        /* [in] */ ArrayOf<Byte> *issuerDN);

    /**
     * Returns the issuer that a certificate must match.
     *
     * @return the distinguished issuer name in ASN.1 DER encoded format, or
     *         {@code null} if the issuer is not to be checked.
     * @throws IOException
     *             if encoding the issuer fails.
     */
    CARAPI GetIssuerAsBytes(
        /* [out, callee] */ ArrayOf<Byte> **issuerDN);

    /**
     * Set the subject that a certificate must match.
     *
     * @param subject
     *            the subject distinguished name or {@code null} to not check
     *            the subject.
     */
    CARAPI SetSubject(
        /* [in] */ IX500Principal *subject);

    /**
     * Returns the subject that a certificate must match.
     *
     * @return the subject distinguished name, or null if the subject is not to
     *         be checked.
     */
    CARAPI GetSubject(
        /* [out] */ IX500Principal **subject);

    /**
     * <b>Do not use</b>, use {@link #setSubject(byte[])} or
     * {@link #setSubject(X500Principal)} instead. Returns the subject that a
     * certificate must match.
     *
     * @param subjectDN
     *            the subject distinguished name in RFC 2253 format or {@code
     *            null} to not check the subject.
     * @throws IOException
     *             if decoding the subject fails.
     */
    CARAPI SetSubjectUsingString(
        /* [in] */ const String& subjectDN);

    /**
     * <b>Do not use</b>, use {@link #getSubject()} or
     * {@link #getSubjectAsBytes()} instead. Returns the subject that a
     * certificate must match.
     *
     * @return the subject distinguished name in RFC 2253 format, or {@code
     *         null} if the subject is not to be checked.
     */
    CARAPI GetSubjectAsString(
        /* [out] */ String *subjectDN);

    /**
     * Sets the subject that a certificate must match.
     *
     * @param subjectDN
     *            the subject distinguished name in ASN.1 DER format, or {@code
     *            null} to not check the subject.
     * @throws IOException
     *             if decoding the subject fails.
     */
    CARAPI SetSubjectUsingBytes(
        /* [in] */ ArrayOf<Byte> *subjectDN);

    /**
     * Returns the subject that a certificate must match.
     *
     * @return the subject distinguished name in ASN.1 DER format, or {@code
     *         null} if the subject is not to be checked.
     * @throws IOException
     *             if encoding the subject fails.
     */
    CARAPI GetSubjectAsBytes(
        /* [out, callee] */ ArrayOf<Byte> **subjectDN);

    /**
     * Sets the criterion for the {@literal SubjectKeyIdentifier} extension.
     * <p>
     * The {@code subjectKeyIdentifier} should be a single DER encoded value.
     *
     * @param subjectKeyIdentifier
     *            the subject key identifier or {@code null} to disable this
     *            check.
     */
    CARAPI SetSubjectKeyIdentifier(
        /* [in] */ ArrayOf<Byte> *subjectKeyIdentifier);

    /**
     * Returns the criterion for the {@literal SubjectKeyIdentifier} extension.
     *
     * @return the subject key identifier or {@code null} if it is not to be
     *         checked.
     */
    CARAPI GetSubjectKeyIdentifier(
        /* [out, callee] */ ArrayOf<Byte> **subjectKeyIdentifier);

    /**
     * Sets the criterion for the {@literal AuthorityKeyIdentifier} extension.
     *
     * @param authorityKeyIdentifier
     *            the authority key identifier, or {@code null} to disable this
     *            check.
     */
    CARAPI SetAuthorityKeyIdentifier(
        /* [in] */ ArrayOf<Byte> *authorityKeyIdentifier);

    /**
     * Returns the criterion for the {@literal AuthorityKeyIdentifier}
     * extension.
     *
     * @return the authority key identifier, or {@code null} if it is not to be
     *         checked.
     */
    CARAPI GetAuthorityKeyIdentifier(
        /* [out, callee] */ ArrayOf<Byte> **authorityKeyIdentifier);

    /**
     * Sets the criterion for the validity date of the certificate.
     * <p>
     * The certificate must be valid at the specified date.
     * @param certificateValid
     *            the validity date or {@code null} to not check the date.
     */
    CARAPI SetCertificateValid(
        /* [in] */ IDate *certificateValid);

    /**
     * Returns the criterion for the validity date of the certificate.
     *
     * @return the validity date or {@code null} if the date is not to be
     *         checked.
     */
    CARAPI GetCertificateValid(
        /* [out] */ IDate **certificateValid);

    /**
     * Sets the criterion for the validity date of the private key.
     * <p>
     * The private key must be valid at the specified date.
     *
     * @param privateKeyValid
     *            the validity date or {@code null} to not check the date.
     */
    CARAPI SetPrivateKeyValid(
        /* [in] */ IDate *privateKeyValid);

    /**
     * Returns the criterion for the validity date of the private key.
     * <p>
     * The private key must be valid at the specified date.
     *
     * @return the validity date or {@code null} if the date is not to be
     *         checked.
     */
    CARAPI GetPrivateKeyValid(
        /* [out] */ IDate **privateKeyValid);
    /**
     * Sets the criterion for the subject public key signature algorithm.
     * <p>
     * The certificate must contain a subject public key with the algorithm
     * specified.
     *
     * @param oid
     *            the OID (object identifier) of the signature algorithm or
     *            {@code null} to not check the OID.
     * @throws IOException
     *             if the specified object identifier is invalid.
     */
    CARAPI SetSubjectPublicKeyAlgID(
        /* [in] */ const String& oid);

    /**
     * Returns the criterion for the subject public key signature algorithm.
     *
     * @return the OID (object identifier) or the signature algorithm or {@code
     *         null} if it's not to be checked.
     */
    CARAPI GetSubjectPublicKeyAlgID(
        /* [out] */ String *oid);

    /**
     * Sets the criterion for the subject public key.
     *
     * @param key
     *            the subject public key or {@code null} to not check the key.
     */
    CARAPI SetSubjectPublicKey(
        /* [in] */ IPublicKey *key);

    /**
     * Sets the criterion for the subject public key.
     *
     * @param key
     *            the subject public key in ASN.1 DER encoded format or {@code null} to
     *            not check the key.
     * @throws IOException
     *             if decoding the the public key fails.
     */
    CARAPI SetSubjectPublicKeyEx(
        /* [in] */ ArrayOf<Byte> *key);

    /**
     * Returns the criterion for the subject public key.
     *
     * @return the subject public key or {@code null} if the key is not to be
     *         checked.
     */
    CARAPI GetSubjectPublicKey(
        /* [out] */ IPublicKey **key);

    /**
     * Sets the criterion for the {@literal KeyUsage} extension.
     *
     * @param keyUsage
     *            the boolean array in the format as returned by
     *            {@link X509Certificate#getKeyUsage()}, or {@code null} to not
     *            check the key usage.
     */
    CARAPI SetKeyUsage(
        /* [in] */ ArrayOf<Boolean> *keyUsage);

    /**
     * Returns the criterion for the {@literal KeyUsage} extension.
     *
     * @return the boolean array in the format as returned by
     *         {@link X509Certificate#getKeyUsage()}, or {@code null} if the key
     *         usage is not to be checked.
     */
    CARAPI GetKeyUsage(
        /* [out, callee] */ ArrayOf<Boolean> **keyUsage);

    /**
     * Sets the criterion for the {@literal ExtendedKeyUsage} extension.
     *
     * @param keyUsage
     *            the set of key usage OIDs, or {@code null} to not check it.
     * @throws IOException
     *             if one of the OIDs is invalid.
     */
    CARAPI SetExtendedKeyUsage(
        /* [in] */ ISet *keyUsage);

    /**
     * Returns the criterion for the {@literal ExtendedKeyUsage} extension.
     *
     * @return the set of key usage OIDs, or {@code null} if it's not to be
     *         checked.
     */
    CARAPI GetExtendedKeyUsage(
        /* [out] */ ISet **keyUsage);

    /**
     * Sets the flag for the matching behavior for subject alternative names.
     * <p>
     * The flag indicates whether a certificate must contain all or at least one
     * of the subject alternative names specified by {@link
     * #setSubjectAlternativeNames} or {@link #addSubjectAlternativeName}.
     *
     * @param matchAllNames
     *            {@code true} if a certificate must contain all of the
     *            specified subject alternative names, otherwise {@code false}.
     */
    CARAPI SetMatchAllSubjectAltNames(
        /* [in] */ Boolean matchAllNames);

    /**
     * Returns the flag for the matching behavior for subject alternative names.
     * <p>
     * The flag indicates whether a certificate must contain all or at least one
     * of the subject alternative names specified by {@link
     * #setSubjectAlternativeNames} or {@link #addSubjectAlternativeName}.
     *
     * @return {@code true} if a certificate must contain all of the specified
     *         subject alternative names, otherwise {@code false}.
     */
    CARAPI GetMatchAllSubjectAltNames(
        /* [out] */ Boolean *matchAllNames);

    /**
     * Sets the criterion for subject alternative names.
     * <p>
     * the certificate must contain all or at least one of the specified subject
     * alternative names. The behavior is specified by
     * {@link #getMatchAllSubjectAltNames}.
     * <p>
     * The specified parameter {@code names} is a collection with an entry for
     * each name to be included in the criterion. The name is specified as a
     * {@code List}, the first entry must be an {@code Integer} specifying the
     * name type (0-8), the second entry must be a {@code String} or a byte
     * array specifying the name (in string or ASN.1 DER encoded form)
     *
     * @param names
     *            the names collection or {@code null} to not perform this check.
     * @throws IOException
     *             if the decoding of a name fails.
     */
    CARAPI SetSubjectAlternativeNames(
        /* [in] */ ICollection *names);

    /**
     * Adds a subject alternative name to the respective criterion.
     *
     * @param tag
     *            the type of the name
     * @param name
     *            the name in string format.
     * @throws IOException
     *             if parsing the name fails.
     */
    CARAPI AddSubjectAlternativeName(
        /* [in] */ Int32 tag,
        /* [in] */ const String& name);

    /**
     * Adds a subject alternative name to the respective criterion.
     *
     * @param tag
     *            the type of the name.
     * @param name
     *            the name in ASN.1 DER encoded form.
     * @throws IOException
     *             if the decoding of the name fails.
     */
    CARAPI AddSubjectAlternativeNameEx(
        /* [in] */ Int32 tag,
        /* [in] */ ArrayOf<Byte> *name);

    /**
     * Returns the criterion for subject alternative names.
     * <p>
     * the certificate must contain all or at least one of the specified subject
     * alternative names. The behavior is specified by
     * {@link #getMatchAllSubjectAltNames}.
     * <p>
     * The subject alternative names is a collection with an entry for each name
     * included in the criterion. The name is specified as a {@code List}, the
     * first entry is an {@code Integer} specifying the name type (0-8), the
     * second entry is byte array specifying the name in ASN.1 DER encoded form)
     *
     * @return the names collection or {@code null} if none specified.
     */
    CARAPI GetSubjectAlternativeNames(
        /* [out] */ ICollection **names);
    /**
     * Sets the criterion for the name constraints.
     * <p>
     * The certificate must constraint subject and subject alternative names
     * that match the specified name constraints.
     * <p>
     * The name constraints in ASN.1:
     *
     * <pre>
     * NameConstraints ::= SEQUENCE {
     *        permittedSubtrees       [0]     GeneralSubtrees OPTIONAL,
     *        excludedSubtrees        [1]     GeneralSubtrees OPTIONAL }
     *
     * GeneralSubtrees ::= SEQUENCE SIZE (1..MAX) OF GeneralSubtree
     *
     * GeneralSubtree ::= SEQUENCE {
     *        base                    GeneralName,
     *        minimum         [0]     BaseDistance DEFAULT 0,
     *        maximum         [1]     BaseDistance OPTIONAL }
     *
     * BaseDistance ::= INTEGER (0..MAX)
     *
     * GeneralName ::= CHOICE {
     *        otherName                       [0]     OtherName,
     *        rfc822Name                      [1]     IA5String,
     *        dNSName                         [2]     IA5String,
     *        x400Address                     [3]     ORAddress,
     *        directoryName                   [4]     Name,
     *        ediPartyName                    [5]     EDIPartyName,
     *        uniformResourceIdentifier       [6]     IA5String,
     *        iPAddress                       [7]     OCTET STRING,
     *        registeredID                    [8]     OBJECT IDENTIFIER}
     *
     * </pre>
     *
     * @param bytes
     *            the name constraints in ASN.1 DER encoded format, or null to
     *            not check any constraints.
     * @throws IOException
     *             if decoding the name constraints fail.
     */
    CARAPI SetNameConstraints(
        /* [in] */ ArrayOf<Byte> *bytes);

    /**
     * Returns the criterion for the name constraints.
     *
     * @return the name constraints or {@code null} if none specified.
     * @see #setNameConstraints
     */
    CARAPI GetNameConstraints(
        /* [out, callee] */ ArrayOf<Byte> **bytes);
    /**
     * Sets the criterion for the basic constraints extension.
     * <p>
     * A value greater than or equal to zero indicates that a certificate must
     * include a basic constraints extension with a path length of a least that
     * value. A value of {@code -2} indicates that only end-entity certificates
     * are accepted. A value of {@code -1} indicates that no check is done.
     *
     * @param pathLen
     *            the value specifying the criterion.
     * @throws IllegalArgumentException
     *             if {@code pathLen} is less than {@code -2}.
     */
    CARAPI SetBasicConstraints(
        /* [in] */ Int32 pathLen);

    /**
     * Returns the criterion for the basic constraints extension.
     * <p>
     * A value greater than or equal to zero indicates that a certificate must
     * include a basic constraints extension with a path length of a least that
     * value. A value of {@code -2} indicates that only end-entity certificates
     * are accepted. A value of {@code -1} indicates that no check is done.
     *
     * @return the value of the criterion.
     */
    CARAPI GetBasicConstraints(
        /* [out] */ Int32 *pathLen);

    /**
     * Sets the criterion for the policy constraint.
     * <p>
     * The certificate must have at least one of the specified certificate
     * policy extensions. For an empty set the certificate must have at least
     * some policies in its policy extension.
     *
     * @param policies
     *            the certificate policy OIDs, an empty set, or {@code null} to
     *            not perform this check.
     * @throws IOException
     *             if parsing the specified OIDs fails.
     */
    CARAPI SetPolicy(
        /* [in] */ ISet *policies);

    /**
     * Returns the criterion for the policy constraint.
     * <p>
     * The certificate must have at least one of the certificate policy
     * extensions. For an empty set the certificate must have at least some
     * policies in its policy extension.
     *
     * @return the certificate policy OIDs, an empty set, or {@code null} if not
     *         to be checked.
     */
    CARAPI GetPolicy(
        /* [out] */ ISet **policies);

    /**
     * Adds a {@literal "pathToName"} to the respective criterion.
     *
     * @param type
     *            the type of the name.
     * @param name
     *            the name in string format.
     * @throws IOException
     *             if parsing fails.
     * @see #setPathToNames
     */
    CARAPI AddPathToName(
        /* [in] */ Int32 type,
        /* [in] */ const String& name);

    /**
     * Sets the criterion for the pathToNames constraint.
     * <p>
     * This allows to specify the complete set of names, a certificate's name
     * constraints must permit.
     * <p>
     * The specified parameter {@code names} is a collection with an entry for
     * each name to be included in the criterion. The name is specified as a
     * {@code List}, the first entry must be an {@code Integer} specifying the
     * name type (0-8), the second entry must be a {@code String} or a byte
     * array specifying the name (in string or ASN.1 DER encoded form)
     *
     * @param names
     *            the names collection or {@code null} to not perform this
     *            check.
     * @throws IOException
     *             if decoding fails.
     */
    CARAPI SetPathToNames(
        /* [in] */ ICollection *names);

    /**
     * Adds a {@literal "pathToName"} to the respective criterion.
     *
     * @param type
     *            the type of the name
     * @param name
     *            the name in ASN.1 DER encoded form.
     * @throws IOException
     *             if decoding fails.
     * @see #setPathToNames
     */
    CARAPI AddPathToNameEx(
        /* [in] */ Int32 type,
        /* [in] */ ArrayOf<Byte> *name);

    /**
     * Returns the criterion for the pathToNames constraint.
     * <p>
     * The constraint is a collection with an entry for each name to be included
     * in the criterion. The name is specified as a {@code List}, the first
     * entry is an {@code Integer} specifying the name type (0-8), the second
     * entry is a byte array specifying the name in ASN.1 DER encoded form.
     *
     * @return the pathToNames constraint or {@code null} if none specified.
     */
    CARAPI GetPathToNames(
        /* [out] */ ICollection **name);

    /**
     * Returns a string representation of this {@code X509CertSelector}
     * instance.
     *
     * @return a string representation of this {@code X509CertSelector}
     *         instance.
     */
    CARAPI ToString(
        /* [out] */ String *str);

    CARAPI constructor();

    /**
     * Returns whether the specified certificate matches all the criteria
     * collected in this instance.
     *
     * @param certificate
     *            the certificate to check.
     * @return {@code true} if the certificate matches all the criteria,
     *         otherwise {@code false}.
     */
    CARAPI Match(
        /* [in] */ ICertificate *certificate,
        /* [out] */ Boolean *result);

    /**
     * Clones this {@code X509CertSelector} instance.
     *
     * @return the cloned instance.
     */
    CARAPI Clone(
        /* [out] */ IInterface **obj);

private:
    CARAPI CheckOID(
        /* [in] */ const String& oid);

    CARAPI GetExtensionValue(
        /* [in] */ IX509Certificate *cert,
        /* [in] */ const String& oid,
        /* [out] */ ArrayOf<Byte> **extval);


private:
    // match criteria
    AutoPtr<IX509Certificate> mCertificateEquals;
    AutoPtr<IBigInteger> mSerialNumber;
    AutoPtr<IX500Principal> mIssuer;
    AutoPtr<IX500Principal> mSubject;
    AutoPtr<ArrayOf<Byte> > mSubjectKeyIdentifier;
    AutoPtr<ArrayOf<Byte> > mAuthorityKeyIdentifier;
    AutoPtr<IDate> mCertificateValid;
    String mSubjectPublicKeyAlgID;
    AutoPtr<IDate> mPrivateKeyValid;
    AutoPtr<ArrayOf<Byte> > mSubjectPublicKey;
    AutoPtr<ArrayOf<Boolean> > mKeyUsage;
    AutoPtr<ISet> mExtendedKeyUsage;
    Boolean mMatchAllNames;// = true;
    Int32 mPathLen;// = -1;
    //Apache...Todo later
    //private List<GeneralName>[] subjectAltNames;
    //private NameConstraints nameConstraints;
    AutoPtr<ISet> mPolicies;
    //Apache...Todo later
    //private ArrayList<GeneralName> pathToNames;

    // needed to avoid needless encoding/decoding work
    AutoPtr<IPublicKey> mSubjectPublicKeyImpl;
    String mIssuerName;
    AutoPtr<ArrayOf<Byte> > mIssuerBytes;
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CX509CERTSELECTOR_H__
