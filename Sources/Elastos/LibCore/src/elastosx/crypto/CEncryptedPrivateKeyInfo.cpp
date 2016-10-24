
#include "Elastos.CoreLibrary.Security.h"
#include "CEncryptedPrivateKeyInfo.h"
//TODO: Need CPKCS8EncodedKeySpec.h
// #include "CPKCS8EncodedKeySpec.h"
//TODO: Need CAlgNameMapperHelper
// #include "CAlgNameMapperHelper.h"

using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
//TODO: Need CPKCS8EncodedKeySpec
// using Elastos::Security::Spec::CPKCS8EncodedKeySpec;
using Elastos::Security::IAlgorithmParametersHelper;
//TODO: Need CAlgorithmParametersHelper
// using Elastos::Security::CAlgorithmParametersHelper;

//TODO: Need Org::Apache::Harmony::Security::X509::IAlgorithmIdentifier
// using Org::Apache::Harmony::Security::X509::IAlgorithmIdentifier;

//TODO: Need Org::Apache::Harmony::Security::Utils::IAlgNameMapperHelper
// using Org::Apache::Harmony::Security::Utils::IAlgNameMapperHelper;
// using Org::Apache::Harmony::Security::Utils::CAlgNameMapperHelper;

namespace Elastosx {
namespace Crypto {

CAR_OBJECT_IMPL(CEncryptedPrivateKeyInfo)
CAR_INTERFACE_IMPL(CEncryptedPrivateKeyInfo, Object, IEncryptedPrivateKeyInfo)

AutoPtr<ArrayOf<Byte> > CEncryptedPrivateKeyInfo::sNullParam;

//TODO: Need Org::Apache::Harmony::Security
// AutoPtr<IASN1SetOf> CEncryptedPrivateKeyInfo::sASN1Attributes;
// AutoPtr<IASN1SetOf> CEncryptedPrivateKeyInfo::sAsn1;
// AutoPtr<IASN1SetOf> CEncryptedPrivateKeyInfo::sASN1PrivateKeyInfo;

CEncryptedPrivateKeyInfo::CEncryptedPrivateKeyInfo()
    : mAlgName(String(NULL))
    , mOid(String(NULL))
{
    sNullParam = ArrayOf<Byte>::Alloc(2);
    sNullParam->Set(0, 5);
    sNullParam->Set(1, 0);

//TODO: Need Org::Apache::Harmony::Security
    // AutoPtr<IASN1Any> asn1any;
    // AutoPtr<IASN1AnyHelper> helper;
    // CASN1AnyHelper::AcquireSingalton((IASN1AnyHelper**)&helper);
    // helper->GetInstance((IASN1Any**)&asn1any);
    // CASN1SetOf::New(asn1any, (IASN1SetOf**)&sASN1Attributes);
}

ECode CEncryptedPrivateKeyInfo::constructor(
    /* [in] */ ArrayOf<Byte> * encoded)
{
    if (encoded == NULL) {
        // throw new NullPointerException("encoded == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    mEncoded = ArrayOf<Byte>::Alloc(encoded->GetLength());
    // System.arraycopy(encoded, 0, mEncoded, 0, encoded.length);
    mEncoded->Copy(0, encoded, 0, encoded->GetLength());

    AutoPtr<ArrayOf<IInterface*> > values;

//TODO: Need Org::Apache::Harmony::Security
    // sAsn1->Decode(encoded, (IInterface**)&values);

//TODO: Need Org::Apache::Harmony::Security::X509::IAlgorithmIdentifier
    // AutoPtr<IAlgorithmIdentifier> aId = IAlgorithmIdentifier::Probe((*values)[0]);
    // aId->GetAlgorithm(&mAlgName);
    // algName == oid now
    Boolean mappingExists = MapAlgName();
    // algName == name from map oid->name if mapping exists, or
    // algName == oid if mapping does not exist

    AutoPtr<IAlgorithmParameters> aParams;
    AutoPtr<ArrayOf<Byte> > params;
//TODO: Need Org::Apache::Harmony::Security::X509::IAlgorithmIdentifier
    // aId->GetParameters((ArrayOf<Byte>**)&params);
    if (params != NULL && !IsNullValue(params)) {
        // try {
            AutoPtr<IAlgorithmParametersHelper> helper;
//TODO: Need CAlgorithmParametersHelper
            // CAlgorithmParametersHelper::AcquireSingalton((IAlgorithmParametersHelper**)&helper);
            helper->GetInstance(mAlgName, (IAlgorithmParameters**)&aParams);
            aParams->Init(params);
            if (!mappingExists) {
                aParams->GetAlgorithm(&mAlgName);
            }
        // } catch (NoSuchAlgorithmException e) {
        // }
    }
    mAlgParameters = aParams;

    AutoPtr<IArrayOf> array = IArrayOf::Probe((*values)[1]);
    if (array != NULL) {
        Int32 len;
        array->GetLength(&len);
        mEncryptedData = ArrayOf<Byte>::Alloc(len);
        for (Int32 i = 0; i < len; i++) {
            AutoPtr<IInterface> ib;
            array->Get(i, (IInterface**)&ib);
            Byte b;
            IByte::Probe(ib)->GetValue(&b);
            mEncryptedData->Set(i, b);
        }
    }
    return NOERROR;
}

ECode CEncryptedPrivateKeyInfo::constructor(
    /* [in] */ const String& encryptionAlgorithmName,
    /* [in] */ ArrayOf<Byte> * encryptedData)
{
    if (encryptionAlgorithmName == NULL) {
        // throw new NullPointerException("encryptionAlgorithmName == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    mAlgName = encryptionAlgorithmName;
    if (!MapAlgName()) {
        // throw new NoSuchAlgorithmException("Unsupported algorithm: " + mAlgName);
        return E_NO_SUCH_ALGORITHM_EXCEPTION;
    }
    if (encryptedData == NULL) {
        // throw new NullPointerException("encryptedData == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (encryptedData->GetLength() == 0) {
        // throw new IllegalArgumentException("encryptedData.length == 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mEncryptedData = ArrayOf<Byte>::Alloc(encryptedData->GetLength());
    // System.arraycopy(encryptedData, 0, mEncryptedData, 0, encryptedData.length);
    mEncryptedData->Copy(0, encryptedData, 0, encryptedData->GetLength());
    mAlgParameters = NULL;
    return NOERROR;
}

ECode CEncryptedPrivateKeyInfo::constructor(
    /* [in] */ IAlgorithmParameters * algParams,
    /* [in] */ ArrayOf<Byte> * encryptedData)
{
    if (algParams == NULL) {
        // throw new NullPointerException("algParams == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    mAlgParameters = algParams;
    if (encryptedData == NULL) {
        // throw new NullPointerException("encryptedData == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (encryptedData->GetLength() == 0) {
        // throw new IllegalArgumentException("encryptedData.length == 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mEncryptedData = ArrayOf<Byte>::Alloc(encryptedData->GetLength());
    // System.arraycopy(encryptedData, 0, mEncryptedData, 0, encryptedData.length);
    mEncryptedData->Copy(0, encryptedData, 0, encryptedData->GetLength());
    mAlgParameters->GetAlgorithm(&mAlgName);
    if (!MapAlgName()) {
        // throw new NoSuchAlgorithmException("Unsupported algorithm: " + mAlgName);
        return E_NO_SUCH_ALGORITHM_EXCEPTION;
    }
    return NOERROR;
}

ECode CEncryptedPrivateKeyInfo::GetAlgName(
    /* [out] */ String * name)
{
    VALIDATE_NOT_NULL(name)
    *name = mAlgName;
    return NOERROR;
}

ECode CEncryptedPrivateKeyInfo::GetAlgParameters(
    /* [out] */ IAlgorithmParameters ** param)
{
    VALIDATE_NOT_NULL(param)
    *param = mAlgParameters;
    REFCOUNT_ADD(*param)
    return NOERROR;
}

ECode CEncryptedPrivateKeyInfo::GetEncryptedData(
    /* [out, callee] */ ArrayOf<Byte> ** data)
{
    VALIDATE_NOT_NULL(data)
    *data = ArrayOf<Byte>::Alloc(mEncryptedData->GetLength());
    // System.arraycopy(encryptedData, 0, ret, 0, encryptedData.length);
    (*data)->Copy(0, mEncryptedData, 0, mEncryptedData->GetLength());
    return NOERROR;
}

ECode CEncryptedPrivateKeyInfo::GetKeySpec(
    /* [in] */ ICipher * cipher,
    /* [out] */ IPKCS8EncodedKeySpec ** spec)
{
    VALIDATE_NOT_NULL(spec)
    *spec = NULL;
    if (cipher == NULL) {
        // throw new NullPointerException("cipher == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    // try {
        AutoPtr<ArrayOf<Byte> > decryptedData;
        cipher->DoFinal(mEncryptedData, (ArrayOf<Byte>**)&decryptedData);
        // try {
//TODO: Need Org::Apache::Harmony::Security
            // sASN1PrivateKeyInfo->Verify(decryptedData);
        // } catch (IOException e1) {
        //     throw new InvalidKeySpecException("Decrypted data does not represent valid PKCS#8 PrivateKeyInfo");
        // }
//TODO: Need CPKCS8EncodedKeySpec
        // CPKCS8EncodedKeySpec::New(decryptedData, spec);
        return NOERROR;
    // } catch (IllegalStateException e) {
    //     throw new InvalidKeySpecException(e.getMessage());
    // } catch (IllegalBlockSizeException e) {
    //     throw new InvalidKeySpecException(e.getMessage());
    // } catch (BadPaddingException e) {
    //     throw new InvalidKeySpecException(e.getMessage());
    // }
}

ECode CEncryptedPrivateKeyInfo::GetKeySpec(
    /* [in] */ IKey * decryptKey,
    /* [out] */ IPKCS8EncodedKeySpec ** spec)
{
    VALIDATE_NOT_NULL(spec)
    *spec = NULL;
    if (decryptKey == NULL) {
        // throw new NullPointerException("decryptKey == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    // try {
        AutoPtr<ICipher> cipher;
        AutoPtr<ICipherHelper> helper;
        helper->GetInstance(mAlgName, (ICipher**)&cipher);
        if (mAlgParameters == NULL) {
            cipher->Init(ICipher::DECRYPT_MODE, decryptKey);
        } else {
            cipher->Init(ICipher::DECRYPT_MODE, decryptKey, mAlgParameters);
        }
        AutoPtr<ArrayOf<Byte> > decryptedData;
        cipher->DoFinal(mEncryptedData, (ArrayOf<Byte>**)&decryptedData);
        // try {
//TODO: Need Org::Apache::Harmony::Security
            // sASN1PrivateKeyInfo->Verify(decryptedData);
        // } catch (IOException e1) {
        //     throw invalidKey();
        // }
//TODO: Need CPKCS8EncodedKeySpec
        // CPKCS8EncodedKeySpec::New(decryptedData, spec);
        return NOERROR;
    // } catch (NoSuchPaddingException e) {
    //     throw new NoSuchAlgorithmException(e.getMessage());
    // } catch (InvalidAlgorithmParameterException e) {
    //     throw new NoSuchAlgorithmException(e.getMessage());
    // } catch (IllegalStateException e) {
    //     throw new InvalidKeyException(e.getMessage());
    // } catch (IllegalBlockSizeException e) {
    //     throw new InvalidKeyException(e.getMessage());
    // } catch (BadPaddingException e) {
    //     throw new InvalidKeyException(e.getMessage());
    // }
}

ECode CEncryptedPrivateKeyInfo::GetKeySpec(
    /* [in] */ IKey * decryptKey,
    /* [in] */ const String& providerName,
    /* [out] */ IPKCS8EncodedKeySpec ** spec)
{
    VALIDATE_NOT_NULL(spec)
    *spec = NULL;
    if (decryptKey == NULL) {
        // throw new NullPointerException("decryptKey == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (providerName == NULL) {
        // throw new NullPointerException("providerName == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    // try {
        AutoPtr<ICipher> cipher;
        AutoPtr<ICipherHelper> helper;
        helper->GetInstance(mAlgName, providerName, (ICipher**)&cipher);
        if (mAlgParameters == NULL) {
            cipher->Init(ICipher::DECRYPT_MODE, decryptKey);
        } else {
            cipher->Init(ICipher::DECRYPT_MODE, decryptKey, mAlgParameters);
        }
        AutoPtr<ArrayOf<Byte> > decryptedData;
        cipher->DoFinal(mEncryptedData, (ArrayOf<Byte>**)&decryptedData);
        // try {
//TODO: Need Org::Apache::Harmony::Security
            // sASN1PrivateKeyInfo->Verify(decryptedData);
        // } catch (IOException e1) {
        //     throw invalidKey();
        // }
//TODO: Need CPKCS8EncodedKeySpec
        // CPKCS8EncodedKeySpec::New(decryptedData, spec);
        return NOERROR;
    // } catch (NoSuchPaddingException e) {
    //     throw new NoSuchAlgorithmException(e.getMessage());
    // } catch (InvalidAlgorithmParameterException e) {
    //     throw new NoSuchAlgorithmException(e.getMessage());
    // } catch (IllegalStateException e) {
    //     throw new InvalidKeyException(e.getMessage());
    // } catch (IllegalBlockSizeException e) {
    //     throw new InvalidKeyException(e.getMessage());
    // } catch (BadPaddingException e) {
    //     throw new InvalidKeyException(e.getMessage());
    // }
}

ECode CEncryptedPrivateKeyInfo::GetKeySpec(
    /* [in] */ IKey * decryptKey,
    /* [in] */ IProvider * provider,
    /* [out] */ IPKCS8EncodedKeySpec ** spec)
{
    VALIDATE_NOT_NULL(spec)
    *spec = NULL;
    if (decryptKey == NULL) {
        // throw new NullPointerException("decryptKey == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (provider == NULL) {
        // throw new NullPointerException("provider == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    // try {
        AutoPtr<ICipher> cipher;
        AutoPtr<ICipherHelper> helper;
        helper->GetInstance(mAlgName, provider, (ICipher**)&cipher);
        if (mAlgParameters == NULL) {
            cipher->Init(ICipher::DECRYPT_MODE, decryptKey);
        } else {
            cipher->Init(ICipher::DECRYPT_MODE, decryptKey, mAlgParameters);
        }
        AutoPtr<ArrayOf<Byte> > decryptedData;
        cipher->DoFinal(mEncryptedData, (ArrayOf<Byte>**)&decryptedData);
        // try {
//TODO: Need Org::Apache::Harmony::Security
            // sASN1PrivateKeyInfo->Verify(decryptedData);
        // } catch (IOException e1) {
        //     throw invalidKey();
        // }
//TODO: Need CPKCS8EncodedKeySpec
        // CPKCS8EncodedKeySpec::New(decryptedData, spec);
        return NOERROR;
    // } catch (NoSuchPaddingException e) {
    //     throw new NoSuchAlgorithmException(e.getMessage());
    // } catch (InvalidAlgorithmParameterException e) {
    //     throw new NoSuchAlgorithmException(e.getMessage());
    // } catch (IllegalStateException e) {
    //     throw new InvalidKeyException(e.getMessage());
    // } catch (IllegalBlockSizeException e) {
    //     throw new InvalidKeyException(e.getMessage());
    // } catch (BadPaddingException e) {
    //     throw new InvalidKeyException(e.getMessage());
    // }
}

ECode CEncryptedPrivateKeyInfo::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** data)
{
    if (mEncoded == NULL) {
        // Generate ASN.1 encoding:

        // sAsn1->Encode(this, (ArrayOf<Byte>**)&mEncoded);
    }
    *data = ArrayOf<Byte>::Alloc(mEncoded->GetLength());
    // System.arraycopy(mEncoded, 0, ret, 0, mEncoded.length);
    (*data)->Copy(0, mEncoded, 0, mEncoded->GetLength());
    return NOERROR;
}

Boolean CEncryptedPrivateKeyInfo::IsNullValue(
    /* [in] */ ArrayOf<Byte> * toCheck)
{
    return (*toCheck)[0] == 5 && (*toCheck)[1] == 0;
}

ECode CEncryptedPrivateKeyInfo::InvalidKey()
{
    return E_INVALID_KEY_EXCEPTION;
}

Boolean CEncryptedPrivateKeyInfo::MapAlgName()
{
//TODO: Need Org::Apache::Harmony::Security::Utils::IAlgNameMapperHelper
    // AutoPtr<IAlgNameMapperHelper> helper;
    // CAlgNameMapperHelper::AcquireSingalton((IAlgNameMapperHelper**)&helper);
    // Boolean b;
    // helper->IsOID(mAlgName, &b);
    // if (b) {
    //     // OID provided to the ctor
    //     // get rid of possible leading "OID."
    //     helper->Normalize(mAlgName, &mOid);
    //     // try to find mapping OID->algName
    //     helper->Map2AlgName(mOid, &mAlgName);
    //     // if there is no mapping OID->algName
    //     // set OID as algName
    //     if (mAlgName == NULL) {
    //         mAlgName = mOid;
    //     }
    // } else {
    //     String stdName;
    //     helper->GetStandardName(mAlgName, &stdName);
    //     // Alg name provided to the ctor
    //     // try to find mapping algName->OID or
    //     // (algName->stdAlgName)->OID
    //     helper->Map2OID(mAlgName, &mOid);
    //     if (mOid == NULL) {
    //         if (stdName == NULL) {
    //             // no above mappings available
    //             return FALSE;
    //         }
    //         helper->Map2OID(stdName, &mOid);
    //         if (mOid == NULL) {
    //             return FALSE;
    //         }
    //         mAlgName = stdName;
    //     } else if (stdName != NULL) {
    //         mAlgName = stdName;
    //     }
    // }
    // return TRUE;

    return FALSE;
}


} // Crypto
} // Elastosx