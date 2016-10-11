#include "elastos/droid/keystore/security/KeyPairGeneratorSpec.h"
#include "elastos/droid/keystore/security/KeyStore.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Text::TextUtils;

//import com.android.org.conscrypt.NativeCrypto;
//TODO using Org::Conscrypt::INativeCrypto;
//
//import android.content.Context;
//import android.text.TextUtils;
//
//import java.math.BigInteger;
//import java.security.NoSuchAlgorithmException;
//import java.security.PrivateKey;
//import java.security.cert.Certificate;
//import java.security.spec.AlgorithmParameterSpec;
//import java.security.spec.DSAParameterSpec;
using Elastos::Security::Spec::IDSAParameterSpec;
using Elastos::Security::Spec::IRSAKeyGenParameterSpec;
using Elastos::Security::Spec::EIID_IAlgorithmParameterSpec;
//import java.security.spec.RSAKeyGenParameterSpec;
//import java.util.Date;
//
//import javax.security.auth.x500.X500Principal;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CAR_INTERFACE_IMPL(KeyPairGeneratorSpec::Builder, Object, IKeyPairGeneratorSpecBuilder);

ECode KeyPairGeneratorSpec::Builder::constructor(
    /* [in] */ IContext* context)
{
    if (context == NULL) {
        //throw new NullPointerException("context == null");
        Logger::E("KeyPairGeneratorSpec", "Builder(), context == null");
        assert(0);
    }
    mContext = context;
    return NOERROR;
}

ECode KeyPairGeneratorSpec::Builder::SetAlias(
    /* [in] */ const String& alias)
{
    if (alias.IsNull()) {
        //throw new NullPointerException("alias == null");
        Logger::E("KeyPairGeneratorSpec", "Builder::SetAlias(), alias == null");
        assert(0);
    }
    mKeystoreAlias = alias;
    return NOERROR;
}

ECode KeyPairGeneratorSpec::Builder::SetKeyType(
    /* [in] */ const String& keyType)
{
    if (keyType.IsNull()) {
        //throw new NullPointerException("keyType == null");
        Logger::E("KeyPairGeneratorSpec", "Builder::SetKeyType(), keyType == null");
        assert(0);
    } else {
        //try {
        KeyStore::GetKeyTypeForAlgorithm(keyType);
        //} catch (IllegalArgumentException e) {
        //    throw new NoSuchAlgorithmException("Unsupported key type: " + keyType);
        //}
    }
    mKeyType = keyType;
    return NOERROR;
}

ECode KeyPairGeneratorSpec::Builder::SetKeySize(
    /* [in] */ Int32 keySize)
{
    if (keySize < 0) {
        //throw new IllegalArgumentException("keySize < 0");
        Logger::E("KeyPairGeneratorSpec", "Builder::SetKeySize(), keySize < 0");
        assert(0);
    }
    mKeySize = keySize;
    return NOERROR;
}

ECode KeyPairGeneratorSpec::Builder::SetAlgorithmParameterSpec(
    /* [in] */ IAlgorithmParameterSpec* spec)
{
    if (spec == NULL) {
        //throw new NullPointerException("spec == null");
        Logger::E("KeyPairGeneratorSpec", "Builder::SetAlgorithmParameterSpec(), spec == NULL");
        assert(0);
    }
    mSpec = spec;
    return NOERROR;
}

ECode KeyPairGeneratorSpec::Builder::SetSubject(
    /* [in] */ IX500Principal* subject)
{
    if (subject == NULL) {
        //throw new NullPointerException("subject == null");
        Logger::E("KeyPairGeneratorSpec", "Builder::SetSubject(), subject == NULL");
        assert(0);
    }
    mSubjectDN = subject;
    return NOERROR;
}

ECode KeyPairGeneratorSpec::Builder::SetSerialNumber(
    /* [in] */ IBigInteger* serialNumber)
{
    if (serialNumber == NULL) {
        //throw new NullPointerException("serialNumber == null");
        Logger::E("KeyPairGeneratorSpec", "Builder::SetSerialNumber(), serialNumber == NULL");
        assert(0);
    }
    mSerialNumber = serialNumber;
    return NOERROR;
}

ECode KeyPairGeneratorSpec::Builder::SetStartDate(
    /* [in] */ IDate* startDate)
{
    if (startDate == NULL) {
        //throw new NullPointerException("startDate == null");
        Logger::E("KeyPairGeneratorSpec", "Builder::SetStartDate(), startDate == NULL");
        assert(0);
    }
    mStartDate = startDate;
    return NOERROR;
}

ECode KeyPairGeneratorSpec::Builder::SetEndDate(
    /* [in] */ IDate* endDate)
{
    if (endDate == NULL) {
        //throw new NullPointerException("endDate == null");
        Logger::E("KeyPairGeneratorSpec", "Builder::SetStartDate(), endDate == NULL");
        assert(0);
    }
    mEndDate = endDate;
    return NOERROR;
}

ECode KeyPairGeneratorSpec::Builder::SetEncryptionRequired()
{
    mFlags |= KeyStore::FLAG_ENCRYPTED;
    return NOERROR;
}

ECode KeyPairGeneratorSpec::Builder::Build(
    /* [out] */ IKeyPairGeneratorSpec** spec)
{
    VALIDATE_NOT_NULL(spec);
    AutoPtr<KeyPairGeneratorSpec> kpgs = new KeyPairGeneratorSpec();
    kpgs->constructor(mContext, mKeystoreAlias, mKeyType, mKeySize, mSpec,
            mSubjectDN, mSerialNumber, mStartDate, mEndDate, mFlags);
    *spec = kpgs;
    REFCOUNT_ADD(*spec);
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(KeyPairGeneratorSpec, Object, IKeyPairGeneratorSpec, IAlgorithmParameterSpec);
/*
 * These must be kept in sync with system/security/keystore/defaults.h
 */

/* DSA */
const Int32 KeyPairGeneratorSpec::DSA_DEFAULT_KEY_SIZE;// = 1024;
const Int32 KeyPairGeneratorSpec::DSA_MIN_KEY_SIZE;// = 512;
const Int32 KeyPairGeneratorSpec::DSA_MAX_KEY_SIZE;// = 8192;

/* EC */
const Int32 KeyPairGeneratorSpec::EC_DEFAULT_KEY_SIZE;// = 256;
const Int32 KeyPairGeneratorSpec::EC_MIN_KEY_SIZE;// = 192;
const Int32 KeyPairGeneratorSpec::EC_MAX_KEY_SIZE;// = 521;

/* RSA */
const Int32 KeyPairGeneratorSpec::RSA_DEFAULT_KEY_SIZE;// = 2048;
const Int32 KeyPairGeneratorSpec::RSA_MIN_KEY_SIZE;// = 512;
const Int32 KeyPairGeneratorSpec::RSA_MAX_KEY_SIZE;// = 8192;

KeyPairGeneratorSpec::KeyPairGeneratorSpec()
{
}

ECode KeyPairGeneratorSpec::constructor(
    /* [in] */ IContext* context,
    /* [in] */ const String& keyStoreAlias,
    /* [in] */ const String& keyType,
    /* [in] */ Int32 keySize,
    /* [in] */ IAlgorithmParameterSpec* spec,
    /* [in] */ IX500Principal* subjectDN,
    /* [in] */ IBigInteger* serialNumber,
    /* [in] */ IDate* startDate,
    /* [in] */ IDate* endDate,
    /* [in] */ Int32 flags)
{
    Boolean bTmp;
    if (context == NULL) {
        //throw new IllegalArgumentException("context == null");
        Logger::E("KeyPairGeneratorSpec", "KeyPairGeneratorSpec(), context == null");
        assert(0);
    } else if (TextUtils::IsEmpty(keyStoreAlias)) {
        //throw new IllegalArgumentException("keyStoreAlias must not be empty");
        Logger::E("KeyPairGeneratorSpec", "KeyPairGeneratorSpec(), keyStoreAlias must not be empty");
        assert(0);
    } else if (subjectDN == NULL) {
        //throw new IllegalArgumentException("subjectDN == null");
        Logger::E("KeyPairGeneratorSpec", "KeyPairGeneratorSpec(), subjectDN == null");
        assert(0);
    } else if (serialNumber == NULL) {
        //throw new IllegalArgumentException("serialNumber == null");
        Logger::E("KeyPairGeneratorSpec", "KeyPairGeneratorSpec(), serialNumber == null");
        assert(0);
    } else if (startDate == NULL) {
        //throw new IllegalArgumentException("startDate == null");
        Logger::E("KeyPairGeneratorSpec", "KeyPairGeneratorSpec(), startDate == null");
        assert(0);
    } else if (endDate == NULL) {
        //throw new IllegalArgumentException("endDate == null");
        Logger::E("KeyPairGeneratorSpec", "KeyPairGeneratorSpec(), endDate == null");
        assert(0);
    } else if (endDate->IsBefore(startDate, &bTmp), bTmp) {
        //throw new IllegalArgumentException("endDate < startDate");
        Logger::E("KeyPairGeneratorSpec", "KeyPairGeneratorSpec(), startDate == null");
        assert(0);
    }

    Int32 keyTypeInt = KeyStore::GetKeyTypeForAlgorithm(keyType);
    if (keySize == -1) {
        GetDefaultKeySizeForType(keyTypeInt, &keySize);
    }
    CheckCorrectParametersSpec(keyTypeInt, keySize, spec);
    CheckValidKeySize(keyTypeInt, keySize);

    mContext = context;
    mKeystoreAlias = keyStoreAlias;
    mKeyType = keyType;
    mKeySize = keySize;
    mSpec = spec;
    mSubjectDN = subjectDN;
    mSerialNumber = serialNumber;
    mStartDate = startDate;
    mEndDate = endDate;
    mFlags = flags;
    return NOERROR;
}

ECode KeyPairGeneratorSpec::GetDefaultKeySizeForType(
    /* [in] */ Int32 keyType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;
    if (keyType == 116/* TODO INativeCrypto::EVP_PKEY_DSA*/) {
        *result = DSA_DEFAULT_KEY_SIZE;
        return NOERROR;
    } else if (keyType == 408/*TODO INativeCrypto::EVP_PKEY_EC*/) {
        *result = EC_DEFAULT_KEY_SIZE;
        return NOERROR;
    } else if (keyType == 6/*TODO INativeCrypto::EVP_PKEY_RSA*/) {
        *result = RSA_DEFAULT_KEY_SIZE;
        return NOERROR;
    }
    //throw new IllegalArgumentException("Invalid key type " + keyType);
    Logger::E("KeyPairGeneratorSpec", "GetDefaultKeySizeForType, Invalid key type: %d", keyType);
    assert(0);
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode KeyPairGeneratorSpec::CheckValidKeySize(
    /* [in] */ Int32 keyType,
    /* [in] */ Int32 keySize)
{
    if (keyType == 116/*TODO INativeCrypto::EVP_PKEY_DSA*/) {
        if (keySize < DSA_MIN_KEY_SIZE || keySize > DSA_MAX_KEY_SIZE) {
            //throw new IllegalArgumentException("DSA keys must be >= " + DSA_MIN_KEY_SIZE + " and <= " + DSA_MAX_KEY_SIZE);
            Logger::E("KeyPairGeneratorSpec", "CheckValidKeySize, DSA keys must be between %d and %d", DSA_MIN_KEY_SIZE, DSA_MAX_KEY_SIZE);
            assert(0);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    } else if (keyType == 408/*TODO INativeCrypto::EVP_PKEY_EC*/) {
        if (keySize < EC_MIN_KEY_SIZE || keySize > EC_MAX_KEY_SIZE) {
            //throw new IllegalArgumentException("EC keys must be >= " + EC_MIN_KEY_SIZE + " and <= " + EC_MAX_KEY_SIZE);
            Logger::E("KeyPairGeneratorSpec", "CheckValidKeySize, EC keys must be between %d and %d", EC_MIN_KEY_SIZE, EC_MAX_KEY_SIZE);
            assert(0);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;

        }
    } else if (keyType == 6/*TODO INativeCrypto::EVP_PKEY_RSA*/) {
        if (keySize < RSA_MIN_KEY_SIZE || keySize > RSA_MAX_KEY_SIZE) {
            //throw new IllegalArgumentException("RSA keys must be >= " + RSA_MIN_KEY_SIZE + " and <= " + RSA_MAX_KEY_SIZE);
            Logger::E("KeyPairGeneratorSpec", "CheckValidKeySize, RSA keys must be between %d and %d", RSA_MIN_KEY_SIZE, RSA_MAX_KEY_SIZE);
            assert(0);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    } else {
        //throw new IllegalArgumentException("Invalid key type " + keyType);
        Logger::E("KeyPairGeneratorSpec", "CheckValidKeySize, Invalid key type %d", keyType);
        assert(0);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode KeyPairGeneratorSpec::CheckCorrectParametersSpec(
    /* [in] */ Int32 keyType,
    /* [in] */ Int32 keySize,
    /* [in] */ IAlgorithmParameterSpec* spec)
{
    if (keyType == 116/*TODO INativeCrypto::EVP_PKEY_DSA*/ && spec != NULL) {
        if (IDSAParameterSpec::Probe(spec) == NULL) {
            //throw new IllegalArgumentException("DSA keys must have DSAParameterSpec specified");
            Logger::E("KeyPairGeneratorSpec", "CheckCorrectParametersSpec, DSA keys must have DSAParameterSpec specified");
            assert(0);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    } else if (keyType == 6/*TODO INativeCrypto::EVP_PKEY_RSA*/ && spec != NULL) {
        if (IRSAKeyGenParameterSpec::Probe(spec) != NULL) {
            AutoPtr<IRSAKeyGenParameterSpec> rsaSpec = IRSAKeyGenParameterSpec::Probe(spec);
            Int32 keysize;
            rsaSpec->GetKeysize(&keysize);
            if (keySize != -1 && keySize != keysize) {
                //throw new IllegalArgumentException("RSA key size must match: " + keySize
                //        + " vs " + rsaSpec.getKeysize());
                Logger::E("KeyPairGeneratorSpec", "CheckCorrectParametersSpec, RSA key size must match: %d vs %d",
                        keySize, keysize);
                assert(0);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        } else {
            //throw new IllegalArgumentException("RSA may only use RSAKeyGenParameterSpec");
            Logger::E("KeyPairGeneratorSpec", "CheckCorrectParametersSpec, RSA may only use RSAKeyGenParameterSpec");
            assert(0);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode KeyPairGeneratorSpec::GetContext(
    /* [out] */ IContext** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mContext;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode KeyPairGeneratorSpec::GetKeystoreAlias(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mKeystoreAlias;
    return NOERROR;
}

ECode KeyPairGeneratorSpec::GetKeyType(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mKeyType;
    return NOERROR;
}

ECode KeyPairGeneratorSpec::GetKeySize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mKeySize;
    return NOERROR;
}

ECode KeyPairGeneratorSpec::GetAlgorithmParameterSpec(
    /* [out] */ IAlgorithmParameterSpec** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSpec;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode KeyPairGeneratorSpec::GetSubjectDN(
    /* [out] */ IX500Principal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSubjectDN;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode KeyPairGeneratorSpec::GetSerialNumber(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSerialNumber;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode KeyPairGeneratorSpec::GetStartDate(
    /* [out] */ IDate** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mStartDate;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode KeyPairGeneratorSpec::GetEndDate(
    /* [out] */ IDate** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mEndDate;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode KeyPairGeneratorSpec::GetFlags(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFlags;
    return NOERROR;
}

ECode KeyPairGeneratorSpec::IsEncryptionRequired(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mFlags & KeyStore::FLAG_ENCRYPTED) != 0;
    return NOERROR;
}


}// namespace Elastos
}// namespace Droid
}// namespace KeyStore
}// namespace Security
