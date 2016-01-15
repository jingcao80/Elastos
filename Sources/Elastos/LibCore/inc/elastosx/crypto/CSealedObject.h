
#ifndef __ELASTOSX_CRYPTO_CSEALEDOBJECT_H__
#define __ELASTOSX_CRYPTO_CSEALEDOBJECT_H__

#include "_Elastosx_Crypto_CSealedObject.h"
#include <elastos/core/Object.h>

using Elastos::Security::IKey;
using Elastos::IO::ISerializable;
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IObjectInputStream;
using Elastos::IO::IObjectInputStreamGetField;

namespace Elastosx {
namespace Crypto {

CarClass(CSealedObject)
    , public Object
    , public ISealedObject
    , public ISerializable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CSealedObject();

    /**
     * Creates a new {@code SealedObject} instance wrapping the specified object
     * and sealing it using the specified cipher.
     * <p>
     * The cipher must be fully initialized.
     *
     * @param object
     *            the object to seal, can be {@code null}.
     * @param c
     *            the cipher to encrypt the object.
     * @throws IOException
     *             if the serialization fails.
     * @throws IllegalBlockSizeException
     *             if the specified cipher is a block cipher and the length of
     *             the serialized data is not a multiple of the ciphers block
     *             size.
     * @throws NullPointerException
     *             if the cipher is {@code null}.
     */
    CARAPI constructor(
        /* [in] */ ISerializable* object,
        /* [in] */ ICipher* c);

    /**
     * Creates a new {@code SealedObject} instance by copying the data from
     * the specified object.
     *
     * @param so
     *            the object to copy.
     */
    CARAPI constructor(
        /* [in] */ ISealedObject* object);

    /**
     * Returns the algorithm this object was sealed with.
     *
     * @return the algorithm this object was sealed with.
     */
    CARAPI GetAlgorithm(
        /* [out] */ String * name);

    /**
     * Returns the wrapped object, decrypting it using the specified key.
     *
     * @param key
     *            the key to decrypt the data with.
     * @return the encapsulated object.
     * @throws IOException
     *             if deserialization fails.
     * @throws ClassNotFoundException
     *             if deserialization fails.
     * @throws NoSuchAlgorithmException
     *             if the algorithm to decrypt the data is not available.
     * @throws InvalidKeyException
     *             if the specified key cannot be used to decrypt the data.
     */
    CARAPI GetObject(
        /* [in] */ IKey * key,
        /* [out] */ IInterface ** result);

    /**
     * Returns the wrapped object, decrypting it using the specified
     * cipher.
     *
     * @param c
     *            the cipher to decrypt the data.
     * @return the encapsulated object.
     * @throws IOException
     *             if deserialization fails.
     * @throws ClassNotFoundException
     *             if deserialization fails.
     * @throws IllegalBlockSizeException
     *             if the specified cipher is a block cipher and the length of
     *             the serialized data is not a multiple of the ciphers block
     *             size.
     * @throws BadPaddingException
     *             if the padding of the data does not match the padding scheme.
     */
    CARAPI GetObject(
        /* [in] */ ICipher * c,
        /* [out] */ IInterface ** result);

    /**
     * Returns the wrapped object, decrypting it using the specified key. The
     * specified provider is used to retrieve the cipher algorithm.
     *
     * @param key
     *            the key to decrypt the data.
     * @param provider
     *            the name of the provider that provides the cipher algorithm.
     * @return the encapsulated object.
     * @throws IOException
     *             if deserialization fails.
     * @throws ClassNotFoundException
     *             if deserialization fails.
     * @throws NoSuchAlgorithmException
     *             if the algorithm used to decrypt the data is not available.
     * @throws NoSuchProviderException
     *             if the specified provider is not available.
     * @throws InvalidKeyException
     *             if the specified key cannot be used to decrypt the data.
     */
    CARAPI GetObject(
        /* [in] */ IKey * key,
        /* [in] */ const String& provider,
        /* [out] */ IInterface ** result);

private:
    static CARAPI_(AutoPtr<ArrayOf<Byte> >) GetSafeCopy(
        /* [in] */ IObjectInputStreamGetField * fields,
        /* [in] */ const String& fieldName);

    CARAPI_(void) ReadObject(
        /* [in] */ IObjectInputStream * s);

    static CARAPI ReadSerialized(
        /* [in] */ ArrayOf<Byte> * serialized,
        /* [out] */ IInterface ** result);

protected:
    /**
     * The cipher's {@link AlgorithmParameters} in encoded format.
     * Equivalent to {@code cipher.getParameters().getEncoded()},
     * or null if the cipher did not use any parameters.
     */
    AutoPtr<ArrayOf<Byte> > mEncodedParams;

private:
    static Int64 mSerialVersionUID; // = 4482838265551344752L;

    AutoPtr<ArrayOf<Byte> > mEncryptedContent;
    String mSealAlg;
    String mParamsAlg;

};

}
}

#endif // __ELASTOSX_CRYPTO_CSEALEDOBJECT_H__
