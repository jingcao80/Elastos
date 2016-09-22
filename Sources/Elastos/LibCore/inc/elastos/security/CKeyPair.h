
#ifndef __ELASTOS_SECURITY_CKEYPAIR_H__
#define __ELASTOS_SECURITY_CKEYPAIR_H__

#include "_Elastos_Security_CKeyPair.h"
#include "Object.h"

using Elastos::IO::ISerializable;

namespace Elastos {
namespace Security {

CarClass(CKeyPair)
    , public Object
    , public IKeyPair
    , public ISerializable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Constructs a new instance of {@code KeyPair} with a public key and the
     * corresponding private key.
     *
     * @param publicKey
     *            the public key.
     * @param privateKey
     *            the private key.
     */
    CARAPI constructor(
        /* [in] */ IPublicKey* publicKey,
        /* [in] */ IPrivateKey* privateKey);

    /**
     * Returns the private key.
     *
     * @return the private key.
     */
    CARAPI GetPrivate(
        /* [out] */ IPrivateKey** result);

    /**
     * Returns the public key.
     *
     * @return the public key.
     */
    CARAPI GetPublic(
        /* [out] */ IPublicKey** result);

private:
    // private static final long serialVersionUID = -7565189502268009837L;
    AutoPtr<IPrivateKey> mPrivateKey;
    AutoPtr<IPublicKey> mPublicKey;
};

}
}

#endif // __ELASTOS_SECURITY_CKEYPAIR_H__
