
#ifndef __ELASTOS_SECURITY_CERT_CRL_H__
#define __ELASTOS_SECURITY_CERT_CRL_H__

using Elastos::Security::ICertificate;

namespace Elastos {
namespace Security {
namespace Cert {

class CRL
    : public Object
    , public ICRL
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns the type of this CRL.
     *
     * @return the type of this CRL.
     */
    CARAPI GetType(
        /* [out] */ String* type) const;

    /**
     * Returns whether the specified certificate is revoked by this CRL.
     *
     * @param cert
     *            the certificate to check.
     * @return {@code true} if the certificate is revoked by this CRL, otherwise
     *         {@code false}.
     */
    virtual CARAPI IsRevoked(
        /* [in] */ ICertificate* cert,
        /* [out] */  Boolean* isRevoked) = 0;

    /**
     * Returns the string representation of this instance.
     *
     * @return the string representation of this instance.
     */
    virtual CARAPI ToString(
        /* [out] */ String* str) = 0;

protected:
    /**
     * Creates a new certificate revocation list of the specified type.
     *
     * @param type
     *            the type for the CRL.
     */
    CRL(
        /* [in] */ const String& type);

private:
    // The CRL type
    const String mType;
};

} // end Cert
} // end Security
} // end Elastos

#endif // __ELASTOS_SECURITY_CERT_CRL_H__
