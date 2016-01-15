
#ifndef __ELASTOSX_SECURITY_CERT_CERTIFICATEX_H__
#define __ELASTOSX_SECURITY_CERT_CERTIFICATEX_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Object.h"

using Elastos::Security::IPublicKey;

namespace Elastosx {
namespace Security {
namespace Cert {

class Certificate
    : public Object
    , public ICertificate
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Compares the argument to this Certificate. If both have the same bytes
     * they are assumed to be equal.
     *
     * @param obj
     *            the {@code Certificate} to compare with this object
     * @return <code>true</code> if {@code obj} is the same as this
     *         {@code Certificate}, <code>false</code> otherwise
     * @see #hashCode
     */
    virtual CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* isEqual);

    /**
     * Returns an integer hash code for the receiver. Any two objects which
     * return <code>true</code> when passed to <code>equals</code> must answer
     * the same value for this method.
     *
     * @return the receiver's hash
     * @see #equals
     */
    virtual CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    /**
     * Returns the encoded representation for this certificate.
     *
     * @return the encoded representation for this certificate.
     * @throws CertificateEncodingException
     *             if encoding fails.
     */
    virtual CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** encode) = 0;

    /**
     * Verifies that this certificate was signed with the given public key.
     *
     * @param key
     *            public key for which verification should be performed.
     * @throws CertificateException
     *             if encoding errors are detected
     * @throws NoSuchAlgorithmException
     *             if an unsupported algorithm is detected
     * @throws InvalidKeyException
     *             if an invalid key is detected
     * @throws NoSuchProviderException
     *             if there is no default provider
     * @throws SignatureException
     *             if signature errors are detected
     */
    virtual CARAPI Verify(
        /* [in] */ IPublicKey* key) = 0;

    /**
     * Verifies that this certificate was signed with the given public key. Uses
     * the signature algorithm given by the provider.
     *
     * @param key
     *            public key for which verification should be performed.
     * @param sigProvider
     *            the name of the signature provider.
     * @throws CertificateException
     *                if encoding errors are detected
     * @throws NoSuchAlgorithmException
     *                if an unsupported algorithm is detected
     * @throws InvalidKeyException
     *                if an invalid key is detected
     * @throws NoSuchProviderException
     *                if the specified provider does not exists.
     * @throws SignatureException
     *                if signature errors are detected
     */
    virtual CARAPI Verify(
        /* [in] */ IPublicKey* key,
        /* [in] */ const String& sigProvider) = 0;

    /**
     * Returns a string containing a concise, human-readable description of the
     * receiver.
     *
     * @return a printable representation for the receiver.
     */
    virtual CARAPI ToString(
        /* [out] */ String* str) = 0;

    /**
     * Returns the public key corresponding to this certificate.
     *
     * @return the public key corresponding to this certificate.
     */
    virtual CARAPI GetPublicKey(
        /* [out] */ IPublicKey** pubkey) = 0;

};

} // end Cert
} // end Security
} // end Elastosx

#endif // __ELASTOSX_SECURITY_CERT_CERTIFICATEX_H__