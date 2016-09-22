
#ifndef __ELASTOS_SECURITY_SECURERANDOMSPI_H__
#define __ELASTOS_SECURITY_SECURERANDOMSPI_H__

#include "Elastos.CoreLibrary.Security.h"
#include "Identity.h"

using Elastos::IO::ISerializable;

namespace Elastos {
namespace Security {

/**
 * {@link Signer} represents an identity (individual or corporation) that owns a
 * private key and the corresponding public key.
 *
 * @deprecated Use the {@link java.security.cert java.security.cert} package
 * and {@link java.security.Principal} instead.
 */
class Signer
    : public Identity
    , public ISigner
{
public:
    CAR_INTERFACE_DECL()

    virtual ~Signer() = 0;

    /**
     * Constructs a new instance of {@code Signer}.
     */
    Signer();

    /**
     * Constructs a new instance of {@code Signer} with the given name.
     *
     * @param name
     *            the name of the signer.
     */
    CARAPI constructor(
        /* [in] */ const String& name);

    /**
     * Constructs a new instance of {@code Signer} with the given name in the
     * given scope.
     *
     * @param name
     *            the name of the signer.
     * @param scope
     *            the scope of the signer.
     * @throws KeyManagementException
     *             if a signer with the specified name already exists in the
     *             provided scope.
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ IIdentityScope* scope) /*throws KeyManagementException*/;

    /**
     * Returns the private key of this {@code Signer}.
     */
    CARAPI GetPrivateKey(
        /* [out] */ IPrivateKey** result);

    /**
     * Associates the specified key pair with this {@code Signer}.
     *
     * @param pair
     *            the key pair to associate with this {@code Signer}.
     * @throws InvalidParameterException
     *             if the key pair is invalid.
     * @throws KeyException
     *             if any other key related problem occurs.
     */
    CARAPI SetKeyPair(
        /* [in] */ IKeyPair* pair) /*throws InvalidParameterException, KeyException*/;

    /**
     * Returns a string containing a concise, human-readable description of this
     * {@code Signer} including its name and its scope if present.
     *
     * @return a printable representation for this {@code Signer}.
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    // private static final long serialVersionUID = -1763464102261361480L;

    AutoPtr<IPrivateKey> mPrivateKey;
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_SECURERANDOMSPI_H__
