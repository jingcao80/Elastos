
#ifndef __ELASTOS_SECURITY_CSIGNEDOBJECT_H__
#define __ELASTOS_SECURITY_CSIGNEDOBJECT_H__

#include "_Elastos_Security_CSignedObject.h"
#include "Object.h"

using Elastos::IO::IObjectInputStream;
using Elastos::IO::ISerializable;

namespace Elastos {
namespace Security {

/**
 * A {@code SignedObject} instance acts as a container for another object. The
 * {@code SignedObject} contains the target in serialized form along with a
 * digital signature of the serialized data.
 */
CarClass(CSignedObject)
    , public Object
    , public ISignedObject
    , public ISerializable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Constructs a new instance of {@code SignedObject} with the target object,
     * the private key and the engine to compute the signature. The given
     * {@code object} is signed with the specified key and engine.
     *
     * @param object
     *            the object to bes signed.
     * @param signingKey
     *            the private key, used to sign the {@code object}.
     * @param signingEngine
     *            the engine that performs the signature generation.
     * @throws IOException
     *             if a serialization error occurs.
     * @throws InvalidKeyException
     *             if the private key is not valid.
     * @throws SignatureException
     *             if signature generation failed.
     */
    CARAPI constructor(
        /* [in] */ ISerializable* object,
        /* [in] */ IPrivateKey* signingKey,
        /* [in] */ ISignature* signingEngine) /*throws IOException, InvalidKeyException, SignatureException*/;

    /**
     * Returns the encapsulated object. Each time this method is invoked, the
     * encapsulated object is deserialized before it is returned.
     *
     * @return the encapsulated object.
     * @throws IOException
     *             if deserialization failed.
     * @throws ClassNotFoundException
     *             if the class of the encapsulated object can not be found.
     */
    CARAPI GetObject(
        /* [out] */ IInterface** result) /*throws IOException, ClassNotFoundException*/;

    /**
     * Returns the signature data of the encapsulated serialized object.
     *
     * @return the signature data of the encapsulated serialized object.
     */
    CARAPI GetSignature(
        /* [out] */ ArrayOf<Byte>** result);

    /**
     * Returns the name of the algorithm of this {@code SignedObject}.
     *
     * @return the name of the algorithm of this {@code SignedObject}.
     */
    CARAPI GetAlgorithm(
        /* [out] */ String* result);

    /**
     * Indicates whether the contained signature for the encapsulated object is
     * valid.
     *
     * @param verificationKey
     *            the public key to verify the signature.
     * @param verificationEngine
     *            the signature engine.
     * @return {@code true} if the contained signature for the encapsulated
     *         object is valid, {@code false} otherwise.
     * @throws InvalidKeyException
     *             if the public key is invalid.
     * @throws SignatureException
     *             if signature verification failed.
     */
    CARAPI Verify(
        /* [in] */ IPublicKey* verificationKey,
        /* [in] */ ISignature* verificationEngine,
        /* [out] */ Boolean* result) /*throws InvalidKeyException, SignatureException*/;

private:
    CARAPI ReadObject(
        /* [in] */ IObjectInputStream* s) /*throws IOException, ClassNotFoundException*/;

private:
    // private static final long serialVersionUID = 720502720485447167L;

    AutoPtr<ArrayOf<Byte> > mContent;

    AutoPtr<ArrayOf<Byte> > mSignature;

    String mThealgorithm;
};

}
}

#endif // __ELASTOS_SECURITY_CSIGNEDOBJECT_H__
