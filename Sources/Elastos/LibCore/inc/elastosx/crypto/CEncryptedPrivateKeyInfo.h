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

#ifndef __ELASTOSX_CRYPTO_CENCRYPTEDPRIVATEKEYINFO_H__
#define __ELASTOSX_CRYPTO_CENCRYPTEDPRIVATEKEYINFO_H__

#include "Elastos.CoreLibrary.Security.h"
#include "_Elastosx_Crypto_CEncryptedPrivateKeyInfo.h"
#include <elastos/core/Object.h>

using Elastos::Security::IKey;
using Elastos::Security::IProvider;
using Elastos::Security::IAlgorithmParameters;
using Elastos::Security::Spec::IKeySpec;
using Elastos::Security::Spec::IPKCS8EncodedKeySpec;
//TODO: Need Org::Apache::Harmony::Security
// using Org::Apache::Harmony::Security::Asn1::IASN1SetOf;
// using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;

namespace Elastosx {
namespace Crypto {

CarClass(CEncryptedPrivateKeyInfo)
    , public Object
    , public IEncryptedPrivateKeyInfo
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CEncryptedPrivateKeyInfo();

    /**
     * Creates an {@code EncryptedPrivateKeyInfo} instance from its encoded
     * representation by parsing it.
     *
     * @param encoded
     *            the encoded representation of this object
     * @throws IOException
     *             if parsing the encoded representation fails.
     * @throws NullPointerException
     *             if {@code encoded} is {@code null}.
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> * encoded);

    /**
     * Creates an {@code EncryptedPrivateKeyInfo} instance from an algorithm
     * name and its encrypted data.
     *
     * @param encryptionAlgorithmName
     *            the name of an algorithm.
     * @param encryptedData
     *            the encrypted data.
     * @throws NoSuchAlgorithmException
     *             if the {@code encrAlgName} is not a supported algorithm.
     * @throws NullPointerException
     *             if {@code encrAlgName} or {@code encryptedData} is {@code
     *             null}.
     * @throws IllegalArgumentException
     *             if {@code encryptedData} is empty.
     */
    CARAPI constructor(
        /* [in] */ const String& encryptionAlgorithmName,
        /* [in] */ ArrayOf<Byte> * encryptedData);

    /**
     * Creates an {@code EncryptedPrivateKeyInfo} instance from the
     * encryption algorithm parameters an its encrypted data.
     *
     * @param algParams
     *            the encryption algorithm parameters.
     * @param encryptedData
     *            the encrypted data.
     * @throws NoSuchAlgorithmException
     *             if the algorithm name of the specified {@code algParams}
     *             parameter is not supported.
     * @throws NullPointerException
     *             if {@code algParams} or {@code encryptedData} is
     *             {@code null}.
     */
    CARAPI constructor(
        /* [in] */ IAlgorithmParameters * algParams,
        /* [in] */ ArrayOf<Byte> * encryptedData);

    /**
     * Returns the name of the encryption algorithm.
     *
     * @return the name of the encryption algorithm.
     */
    CARAPI GetAlgName(
        /* [out] */ String * name);

    /**
     * Returns the parameters used by the encryption algorithm.
     *
     * @return the parameters used by the encryption algorithm.
     */
    CARAPI GetAlgParameters(
        /* [out] */ IAlgorithmParameters ** param);

    /**
     * Returns the encrypted data of this key.
     *
     * @return the encrypted data of this key, each time this method is called a
     *         new array is returned.
     */
    CARAPI GetEncryptedData(
        /* [out, callee] */ ArrayOf<Byte> ** data);

    /**
     * Returns the {@code PKCS8EncodedKeySpec} object extracted from the
     * encrypted data.
     * <p>
     * The cipher must be initialize in either {@code Cipher.DECRYPT_MODE} or
     * {@code Cipher.UNWRAP_MODE} with the same parameters and key used for
     * encrypting this.
     *
     * @param cipher
     *            the cipher initialized for decrypting the encrypted data.
     * @return the extracted {@code PKCS8EncodedKeySpec}.
     * @throws InvalidKeySpecException
     *             if the specified cipher is not suited to decrypt the
     *             encrypted data.
     * @throws NullPointerException
     *             if {@code cipher} is {@code null}.
     */
    CARAPI GetKeySpec(
        /* [in] */ ICipher * cipher,
        /* [out] */ IPKCS8EncodedKeySpec ** spec);

    /**
     * Returns the {@code PKCS8EncodedKeySpec} object extracted from the
     * encrypted data.
     *
     * @param decryptKey
     *            the key to decrypt the encrypted data with.
     * @return the extracted {@code PKCS8EncodedKeySpec}.
     * @throws NoSuchAlgorithmException
     *             if no usable cipher can be found to decrypt the encrypted
     *             data.
     * @throws InvalidKeyException
     *             if {@code decryptKey} is not usable to decrypt the encrypted
     *             data.
     * @throws NullPointerException
     *             if {@code decryptKey} is {@code null}.
     */
    CARAPI GetKeySpec(
        /* [in] */ IKey * decryptKey,
        /* [out] */ IPKCS8EncodedKeySpec ** spec);

    /**
     * Returns the {@code PKCS8EncodedKeySpec} object extracted from the
     * encrypted data.
     *
     * @param decryptKey
     *            the key to decrypt the encrypted data with.
     * @param providerName
     *            the name of a provider whose cipher implementation should be
     *            used.
     * @return the extracted {@code PKCS8EncodedKeySpec}.
     * @throws NoSuchProviderException
     *             if no provider with {@code providerName} can be found.
     * @throws NoSuchAlgorithmException
     *             if no usable cipher can be found to decrypt the encrypted
     *             data.
     * @throws InvalidKeyException
     *             if {@code decryptKey} is not usable to decrypt the encrypted
     *             data.
     * @throws NullPointerException
     *             if {@code decryptKey} or {@code providerName} is {@code null}
     *             .
     */
    CARAPI GetKeySpec(
        /* [in] */ IKey * decryptKey,
        /* [in] */ const String& providerName,
        /* [out] */ IPKCS8EncodedKeySpec ** spec);

    /**
     * Returns the {@code PKCS8EncodedKeySpec} object extracted from the
     * encrypted data.
     *
     * @param decryptKey
     *            the key to decrypt the encrypted data with.
     * @param provider
     *            the provider whose cipher implementation should be used.
     * @return the extracted {@code PKCS8EncodedKeySpec}.
     * @throws NoSuchAlgorithmException
     *             if no usable cipher can be found to decrypt the encrypted
     *             data.
     * @throws InvalidKeyException
     *             if {@code decryptKey} is not usable to decrypt the encrypted
     *             data.
     * @throws NullPointerException
     *             if {@code decryptKey} or {@code provider} is {@code null}.
     */
    CARAPI GetKeySpec(
        /* [in] */ IKey * decryptKey,
        /* [in] */ IProvider * provider,
        /* [out] */ IPKCS8EncodedKeySpec ** spec);

    /**
     * Returns the ASN.1 encoded representation of this object.
     *
     * @return the ASN.1 encoded representation of this object.
     * @throws IOException
     *             if encoding this object fails.
     */
    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** data);

private:
    static CARAPI_(Boolean) IsNullValue(
        /* [in] */ ArrayOf<Byte> * toCheck);

    CARAPI InvalidKey();

    // Performs all needed alg name mappings.
    // Returns 'true' if mapping available 'false' otherwise
    CARAPI_(Boolean) MapAlgName();

private:
    // Encryption algorithm name
    String mAlgName;
    // Encryption algorithm parameters
    AutoPtr<IAlgorithmParameters> mAlgParameters;
    // Encrypted private key data
    AutoPtr<ArrayOf<Byte> > mEncryptedData;
    // Encryption algorithm OID
    String mOid;
    // This EncryptedPrivateKeyInfo ASN.1 DER encoding
    AutoPtr<ArrayOf<Byte> > mEncoded;

    //
    // EncryptedPrivateKeyInfo DER encoder/decoder.
    // EncryptedPrivateKeyInfo ASN.1 definition
    // (as defined in PKCS #8: Private-Key Information Syntax Standard
    //  http://www.ietf.org/rfc/rfc2313.txt)
    //
    // EncryptedPrivateKeyInfo ::=  SEQUENCE {
    //      encryptionAlgorithm   AlgorithmIdentifier,
    //      encryptedData   OCTET STRING }
    //

    static AutoPtr<ArrayOf<Byte> > sNullParam; // = new byte[] { 5, 0 };

    // PrivateKeyInfo DER decoder.
    // PrivateKeyInfo ASN.1 definition
    // (as defined in PKCS #8: Private-Key Information Syntax Standard
    //  http://www.ietf.org/rfc/rfc2313.txt)
    //
    //
    //    PrivateKeyInfo ::= SEQUENCE {
    //        version Version,
    //        privateKeyAlgorithm PrivateKeyAlgorithmIdentifier,
    //        privateKey PrivateKey,
    //        attributes [0] IMPLICIT Attributes OPTIONAL }
    //
    //      Version ::= INTEGER
    //
    //      PrivateKeyAlgorithmIdentifier ::= AlgorithmIdentifier
    //
    //      PrivateKey ::= OCTET STRING
    //
    //      Attributes ::= SET OF Attribute

//TODO: Need Org::Apache::Harmony::Security
    // static AutoPtr<IASN1SetOf> sASN1Attributes; // = new ASN1SetOf(ASN1Any.getInstance());
//TODO: Need Org::Apache::Harmony::Security
    // static AutoPtr<IASN1Sequence> sAsn1; // = new ASN1Sequence(new ASN1Type[] {
    //         AlgorithmIdentifier.ASN1, ASN1OctetString.getInstance() }) {

    //             @Override
    //             protected void getValues(Object object, Object[] values) {

    //                 EncryptedPrivateKeyInfo epki = (EncryptedPrivateKeyInfo) object;

    //                 try {
    //                     byte[] algParmsEncoded = (epki.algParameters == null) ? nullParam
    //                             : epki.algParameters.getEncoded();
    //                     values[0] = new AlgorithmIdentifier(epki.oid, algParmsEncoded);
    //                     values[1] = epki.encryptedData;
    //                 } catch (IOException e) {
    //                     throw new RuntimeException(e.getMessage());
    //                 }
    //             }
    // };

//TODO: Need Org::Apache::Harmony::Security
    // static AutoPtr<IASN1Sequence> sASN1PrivateKeyInfo; // = new ASN1Sequence(
    //         new ASN1Type[] { ASN1Integer.getInstance(), AlgorithmIdentifier.ASN1,
    //                 ASN1OctetString.getInstance(),
    //                 new ASN1Implicit(0, ASN1Attributes) }) {
    //     {
    //         setOptional(3); //attributes are optional
    //     }
    // };
};

}
}

#endif // __ELASTOSX_CRYPTO_CENCRYPTEDPRIVATEKEYINFO_H__
