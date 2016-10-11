#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.Security.h"
#include "elastos/droid/keystore/security/ElastosKeyPairGenerator.h"
#include "elastos/droid/keystore/security/KeyStore.h"
#include "elastos/droid/keystore/security/Credentials.h"
#include "elastos/core/CoreUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Math::IBigInteger;

//import com.android.org.bouncycastle.x509.X509V3CertificateGenerator;
//
//import com.android.org.conscrypt.NativeCrypto;
//TODO using Org::Conscrypt::INativeCrypto;
//TODO using Org::Conscrypt::IOpenSSLEngine;
//import com.android.org.conscrypt.OpenSSLEngine;
//
//import java.security.InvalidAlgorithmParameterException;
//import java.security.InvalidKeyException;
//import java.security.KeyPair;
//import java.security.KeyPairGenerator;
//import java.security.KeyPairGeneratorSpi;
//import java.security.NoSuchAlgorithmException;
using Elastos::Security::IKeyPair;
using Elastos::Security::CKeyPair;
using Elastos::Security::IPrivateKey;
using Elastos::Security::IPublicKey;
using Elastos::Security::IKeyFactory;
using Elastos::Security::IKeyFactoryHelper;
using Elastos::Security::CKeyFactoryHelper;
//import java.security.SecureRandom;
//import java.security.cert.CertificateEncodingException;
//import java.security.cert.X509Certificate;
//import java.security.spec.AlgorithmParameterSpec;
//import java.security.spec.DSAParameterSpec;
//import java.security.spec.InvalidKeySpecException;
//import java.security.spec.RSAKeyGenParameterSpec;
//TODO using Elastos::Security::Auth::X500::IX500Principal;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::Spec::IRSAKeyGenParameterSpec;
using Elastos::Security::Spec::IDSAParameterSpec;
using Elastos::Security::Interfaces::IDSAParams;
using Elastos::Security::Spec::IKeySpec;
using Elastos::Security::Spec::IX509EncodedKeySpec;
using Elastos::Security::Spec::CX509EncodedKeySpec;
using Elastos::Core::CoreUtils;
using Elastos::Utility::IDate;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CAR_INTERFACE_IMPL(ElastosKeyPairGenerator, KeyPairGeneratorSpi, IElastosKeyPairGenerator);

ECode ElastosKeyPairGenerator::constructor()
{
    return NOERROR;
}

ECode ElastosKeyPairGenerator::GenerateKeyPair(
    /* [out] */ IKeyPair** keyPair)
{
    VALIDATE_NOT_NULL(keyPair);

    if (mKeyStore == NULL || mSpec == NULL) {
        //throw new IllegalStateException("Must call initialize with an android.security.KeyPairGeneratorSpec first");
        Logger::E("ElastosKeyPairGenerator", "Must call initialize with an android.security.KeyPairGeneratorSpec first");
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Int32 flags;
    KeyStoreState state;
    if ((((mSpec->GetFlags(&flags), flags) & KeyStore::FLAG_ENCRYPTED) != 0) && ((mKeyStore->State(&state), state) != KeyStoreState_UNLOCKED)) {
        //throw new IllegalStateException( "Android keystore must be in initialized and unlocked state if encryption is required");
        Logger::E("ElastosKeyPairGenerator", "Android keystore must be in initialized and unlocked state if encryption is required");
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    String alias;
    mSpec->GetKeystoreAlias(&alias);

    Boolean bTmp;
    Credentials::DeleteAllTypesForAlias(mKeyStore, alias, &bTmp);

    String skeyType;
    mSpec->GetKeyType(&skeyType);
    Int32 keyType = KeyStore::GetKeyTypeForAlgorithm(skeyType);
    AutoPtr<IAlgorithmParameterSpec> aps;
    mSpec->GetAlgorithmParameterSpec((IAlgorithmParameterSpec**)&aps);
    AutoPtr<ArrayOf<ArrayOf<Byte>*> > args = GetArgsForKeyType(keyType, aps);

    String privateKeyAlias = Credentials::USER_PRIVATE_KEY + alias;
    Int32 keySize;
    mSpec->GetKeySize(&keySize);
    Int32 argsLen = args->GetLength();
    AutoPtr<ArrayOf<IArrayOf*> > args2 = ArrayOf<IArrayOf*>::Alloc(argsLen);
    for (Int32 i = 0; i < argsLen; ++i) {
        args2->Set(i, CoreUtils::ConvertByteArray((*args)[i]));
    }

    if (!(mKeyStore->Generate(privateKeyAlias, KeyStore::UID_SELF, keyType,
            keySize, flags, args2, &bTmp), bTmp)) {
        //throw new IllegalStateException("could not generate key in keystore");
        Logger::E("ElastosKeyPairGenerator", "could not generate key in keystore");
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IPrivateKey> privKey;
    //TODO AutoPtr<IOpenSSLEngineHelper> opensslEngineHelper;
    //TODO COpenSSLEngineHelper::AcquireSingleton((IOpenSSLEngineHelper**)&opensslEngineHelper);
    //TODO AutoPtr<IOpenSSLEngine> engine;
    //TODO opensslEngineHelper->GetInstance(String("keystore"), (IOpenSSLEngine**)&engine);
    //try {
    //TODO engine->GetPrivateKeyById(privateKeyAlias, (IPrivateKey**)&privKey);
    //} catch (InvalidKeyException e) {
    //    throw new RuntimeException("Can't get key", e);
    //}

    AutoPtr<ArrayOf<Byte> > pubKeyBytes;
    mKeyStore->GetPubkey(privateKeyAlias, (ArrayOf<Byte>**)&pubKeyBytes);

    AutoPtr<IPublicKey> pubKey;
    //try {
    AutoPtr<IKeyFactoryHelper> kfHelper;
    CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&kfHelper);
    AutoPtr<IKeyFactory> keyFact;
    kfHelper->GetInstance(skeyType, (IKeyFactory**)&keyFact);
    AutoPtr<IX509EncodedKeySpec> x509eks;
    CX509EncodedKeySpec::New(pubKeyBytes, (IX509EncodedKeySpec**)&x509eks);
    keyFact->GeneratePublic(IKeySpec::Probe(x509eks), (IPublicKey**)&pubKey);
    //} catch (NoSuchAlgorithmException e) {
    //    throw new IllegalStateException("Can't instantiate key generator", e);
    //} catch (InvalidKeySpecException e) {
    //    throw new IllegalStateException("keystore returned invalid key encoding", e);
    //}

    //TODO AutoPtr<IX509V3CertificateGenerator> certGen;
    //TODO CX509V3CertificateGenerator::New((IX509V3CertificateGenerator**)&certGen);
    //TODO certGen->SetPublicKey(pubKey);
    AutoPtr<IBigInteger> serialNumber;
    mSpec->GetSerialNumber((IBigInteger**)&serialNumber);
    //TODO certGen->SetSerialNumber(serialNumber);
    //TODO AutoPtr<IX500Principal> subDN;
    //TODO mSpec->GetSubjectDN((IX500Principal**)&subDN);
    //TODO certGen->SetSubjectDN(subDN);
    //TODO certGen->SetIssuerDN(subDN);
    AutoPtr<IDate> start, end;
    mSpec->GetStartDate((IDate**)&start);
    mSpec->GetEndDate((IDate**)&end);
    //TODO certGen->SetNotBefore(start);
    //TODO certGen->SetNotAfter(end);
    String sigAl;
    GetDefaultSignatureAlgorithmForKeyType(skeyType, &sigAl);
    //TODO certGen->SetSignatureAlgorithm(sigAl);

    AutoPtr<IX509Certificate> cert;
    //try {
    //TODO certGen->Generate(privKey, (IX509Certificate**)&cert);
    //} catch (Exception e) {
    //    Credentials.deleteAllTypesForAlias(mKeyStore, alias);
    //    throw new IllegalStateException("Can't generate certificate", e);
    //}

    AutoPtr<ArrayOf<Byte> > certBytes;
    //try {
    ICertificate::Probe(cert)->GetEncoded((ArrayOf<Byte>**)&certBytes);
    //} catch (CertificateEncodingException e) {
    //    Credentials.deleteAllTypesForAlias(mKeyStore, alias);
    //    throw new IllegalStateException("Can't get encoding of certificate", e);
    //}

    Boolean put;
    if (!(mKeyStore->Put(Credentials::USER_CERTIFICATE + alias, certBytes, KeyStore::UID_SELF, flags, &put), put)) {
        Boolean bTmp;
        Credentials::DeleteAllTypesForAlias(mKeyStore, alias, &bTmp);
        //throw new IllegalStateException("Can't store certificate in AndroidKeyStore");
        Logger::E("ElastosKeyPairGenerator", "Can't store certificate in AndroidKeyStore");
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return CKeyPair::New(pubKey, privKey, keyPair);
}

ECode ElastosKeyPairGenerator::GetDefaultSignatureAlgorithmForKeyType(
    /* [in] */ const String& keyType,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (String("RSA").EqualsIgnoreCase(keyType)) {
        *result = String("sha256WithRSA");
    } else if (String("DSA").EqualsIgnoreCase(keyType)) {
        *result = String("sha1WithDSA");
    } else if (String("EC").EqualsIgnoreCase(keyType)) {
        *result = String("sha256WithECDSA");
    } else {
        //throw new IllegalArgumentException("Unsupported key type " + keyType);
        Logger::E("ElastosKeyPairGenerator", "Unsupported key type %s", keyType.string());
        assert(0);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

AutoPtr< ArrayOf< ArrayOf<Byte>* > > ElastosKeyPairGenerator::GetArgsForKeyType(
    /* [in] */ Int32 keyType,
    /* [in] */ IAlgorithmParameterSpec* spec)
{
    switch (keyType) {
        case 6://TODO INativeCrypto::EVP_PKEY_RSA:
            if (IRSAKeyGenParameterSpec::Probe(spec) != NULL) {
                IRSAKeyGenParameterSpec* rsaSpec = IRSAKeyGenParameterSpec::Probe(spec);
                AutoPtr<IBigInteger> bi;
                rsaSpec->GetPublicExponent((IBigInteger**)&bi);
                AutoPtr<ArrayOf<Byte> > array;
                bi->ToByteArray((ArrayOf<Byte>**)&array);
                AutoPtr<ArrayOf<ArrayOf<Byte>*> > result = ArrayOf<ArrayOf<Byte>* >::Alloc(1);
                result->Set(0, array);
                return result;
            }
            break;
        case 116://TODO INativeCrypto::EVP_PKEY_DSA:
            if (IDSAParameterSpec::Probe(spec) != NULL) {
                IDSAParameterSpec* dsaSpec = IDSAParameterSpec::Probe(spec);
                IDSAParams* dsaParams = IDSAParams::Probe(dsaSpec);

                AutoPtr<IBigInteger> bi1;
                dsaParams->GetG((IBigInteger**)&bi1);
                AutoPtr<ArrayOf<Byte> > array1;
                bi1->ToByteArray((ArrayOf<Byte>**)&array1);

                AutoPtr<IBigInteger> bi2;
                dsaParams->GetP((IBigInteger**)&bi2);
                AutoPtr<ArrayOf<Byte> > array2;
                bi2->ToByteArray((ArrayOf<Byte>**)&array2);

                AutoPtr<IBigInteger> bi3;
                dsaParams->GetQ((IBigInteger**)&bi3);
                AutoPtr<ArrayOf<Byte> > array3;
                bi3->ToByteArray((ArrayOf<Byte>**)&array3);

                AutoPtr<ArrayOf<ArrayOf<Byte>*> > result = ArrayOf<ArrayOf<Byte>* >::Alloc(3);
                result->Set(0, array1);
                result->Set(1, array2);
                result->Set(2, array3);
                return result;
            }
            break;
    }
    return NULL;
}

ECode ElastosKeyPairGenerator::Initialize(
    /* [in] */ Int32 keysize,
    /* [in] */ ISecureRandom* random)
{
    //throw new IllegalArgumentException("cannot specify keysize with AndroidKeyPairGenerator");
    Logger::E("ElastosKeyPairGenerator", "Initialize - cannot specify keysize with AndroidKeyPairGenerator");
    assert(0);
    return NOERROR;
}

ECode ElastosKeyPairGenerator::Initialize(
    /* [in] */ IAlgorithmParameterSpec* params,
    /* [in] */ ISecureRandom* random)
{
    if (params == NULL) {
        //throw new InvalidAlgorithmParameterException(
        //        "must supply params of type android.security.KeyPairGeneratorSpec");
        Logger::E("ElastosKeyPairGenerator", "Initialize - must supply params of type android.security.KeyPairGeneratorSpec");
        assert(0);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    } else if (IKeyPairGeneratorSpec::Probe(params) == NULL) {
        //throw new InvalidAlgorithmParameterException(
        //        "params must be of type android.security.KeyPairGeneratorSpec");
        Logger::E("ElastosKeyPairGenerator", "Initialize - params must be of type android.security.KeyPairGeneratorSpec");
        assert(0);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IKeyPairGeneratorSpec> spec = IKeyPairGeneratorSpec::Probe(params);
    mSpec = spec;

    AutoPtr<IKeyStore>  ks = KeyStore::GetInstance();
    //mKeyStore = KeyStore::GetInstance();
    return NOERROR;
}

}// namespace Elastos
}// namespace Droid
}// namespace KeyStore
}// namespace Security
