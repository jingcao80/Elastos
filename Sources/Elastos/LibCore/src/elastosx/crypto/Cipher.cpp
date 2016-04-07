
#include "Elastos.CoreLibrary.Utility.h"
#include "Cipher.h"
#include "CCipher.h"
#include "AutoLock.h"
#include "StringUtils.h"
//TODO: Need CSecurity
//#include "CSecurity.h"

using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Security::IPublicKey;
using Elastos::Security::ISecurity;
//TODO: Need CSecurity
// using Elastos::Security::CSecurity;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::Cert::IX509Extension;

namespace Elastosx {
namespace Crypto {

CAR_INTERFACE_IMPL(Cipher, Object, ICipher)

const String Cipher::SERVICE = String("Cipher");

//TODO: Need IEngine
//AutoPtr<IEngine> Cipher::mENGINE;

const String Cipher::ATTRIBUTE_PADDINGS = String("SupportedPaddings");

const String Cipher::ATTRIBUTE_MODES = String("SupportedModes");

AutoPtr<ISecureRandom> Cipher::mSecureRandom;

Cipher::Cipher()
    : mMode(0)
    , mTransformation(String(NULL))
{
//TODO: Need IEngine
    // CEngine::New(SERVICE, (IEngine**)&mEngine);
}

ECode Cipher::constructor(
    /* [in] */ ICipherSpi* cipherSpi,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& transformation)
{
    if (cipherSpi == NULL) {
        // throw new NullPointerException("cipherSpi == null");
        return E_NULL_POINTER_EXCEPTION;
    }
//TODO: Need INullCipherSpi
    // AutoPtr<INullCipherSpi> spi = INullCipherSpi::Probe(cipherSpi);
    // if (spi == NULL && provider == NULL) {
    //     // throw new NullPointerException("provider == null");
    //     return E_NULL_POINTER_EXCEPTION;
    // }
    mSpecifiedProvider = provider;
    mSpecifiedSpi = cipherSpi;
    mTransformation = transformation;
    mTransformParts = NULL;
    return NOERROR;
}

ECode Cipher::constructor(
    /* [in] */ const String& transformation,
    /* [in] */ ArrayOf<String> * transformParts,
    /* [in] */ IProvider* provider)
{
    mTransformation = transformation;
    mTransformParts = transformParts;
    mSpecifiedProvider = provider;
    mSpecifiedSpi = NULL;
    return NOERROR;
}

ECode Cipher::GetProvider(
    /* [out] */ IProvider ** provider)
{
    VALIDATE_NOT_NULL(provider)
    AutoPtr<ICipherSpi> spi;
    GetSpi((ICipherSpi**)&spi);
    *provider = mProvider;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode Cipher::GetAlgorithm(
    /* [out] */ String * value)
{
    VALIDATE_NOT_NULL(value);
    *value = mTransformation;
    return NOERROR;
}

ECode Cipher::GetBlockSize(
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<ICipherSpi> spi;
    GetSpi((ICipherSpi**)&spi);
    return spi->EngineGetBlockSize(value);
}

ECode Cipher::GetOutputSize(
    /* [in] */ Int32 inputLen,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    if (mMode == 0) {
        // throw new IllegalStateException("Cipher has not yet been initialized");
        return E_ILLEGAL_STATE_EXCEPTION;
    } else {
        AutoPtr<ICipherSpi> spi;
        GetSpi((ICipherSpi**)&spi);
        return spi->EngineGetOutputSize(inputLen, value);
    }
}

ECode Cipher::GetIV(
    /* [out, callee] */ ArrayOf<Byte> ** value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<ICipherSpi> spi;
    GetSpi((ICipherSpi**)&spi);
    return spi->EngineGetIV(value);
}

ECode Cipher::GetParameters(
    /* [out] */ IAlgorithmParameters ** parameters)
{
    VALIDATE_NOT_NULL(parameters);
    AutoPtr<ICipherSpi> spi;
    GetSpi((ICipherSpi**)&spi);
    return spi->EngineGetParameters(parameters);
}

ECode Cipher::GetExemptionMechanism(
    /* [out] */ IExemptionMechanism ** parameters)
{
    VALIDATE_NOT_NULL(parameters);
    *parameters = NULL;
    return NOERROR;
}

ECode Cipher::Init(
    /* [in] */ Int32 opmode,
    /* [in] */ IKey * key)
{
    if (mSecureRandom == NULL) {
        // In theory it might be thread-unsafe but in the given case it's OK
        // since it does not matter which SecureRandom instance is passed
        // to the init()
//TODO: Need CSecureRandom
        // CSecureRandom::New((ISecureRandom**)&mSecureRandom);
    }
    return Init(opmode, key, mSecureRandom);
}

ECode Cipher::Init(
    /* [in] */ Int32 opmode,
    /* [in] */ IKey * key,
    /* [in] */ ISecureRandom * random)
{
    CheckMode(opmode);
    //        FIXME InvalidKeyException
    //        if keysize exceeds the maximum allowable keysize
    //        (jurisdiction policy files)
    AutoPtr<ICipherSpi> spi;
    GetSpi(key, (ICipherSpi**)&spi);
    spi->EngineInit(opmode, key, random);
    mMode = opmode;
    return NOERROR;
}

ECode Cipher::Init(
    /* [in] */ Int32 opmode,
    /* [in] */ IKey * key,
    /* [in] */ IAlgorithmParameterSpec * params)
{
    if (mSecureRandom == NULL) {
//TODO: Need CSecureRandom
        // CSecureRandom::New((ISecureRandom**)&mSecureRandom);
    }
    return Init(opmode, key, params, mSecureRandom);
}

ECode Cipher::Init(
    /* [in] */ Int32 opmode,
    /* [in] */ IKey * key,
    /* [in] */ IAlgorithmParameterSpec * params,
    /* [in] */ ISecureRandom * random)
{
    CheckMode(opmode);
    //        FIXME InvalidKeyException
    //        if keysize exceeds the maximum allowable keysize
    //        (jurisdiction policy files)
    //        FIXME InvalidAlgorithmParameterException
    //        cryptographic strength exceed the legal limits
    //        (jurisdiction policy files)
    AutoPtr<ICipherSpi> spi;
    GetSpi(key, (ICipherSpi**)&spi);
    spi->EngineInit(opmode, key, params, random);
    mMode = opmode;
    return NOERROR;
}

ECode Cipher::Init(
    /* [in] */ Int32 opmode,
    /* [in] */ IKey * key,
    /* [in] */ IAlgorithmParameters * params)
{
    if (mSecureRandom == NULL) {
//TODO: Need CSecureRandom
        // CSecureRandom::New((ISecureRandom**)&mSecureRandom);
    }
    return Init(opmode, key, params, mSecureRandom);
}

ECode Cipher::Init(
    /* [in] */ Int32 opmode,
    /* [in] */ IKey * key,
    /* [in] */ IAlgorithmParameters * params,
    /* [in] */ ISecureRandom * random)
{
    CheckMode(opmode);
    //        FIXME InvalidKeyException
    //        if keysize exceeds the maximum allowable keysize
    //        (jurisdiction policy files)
    //        FIXME InvalidAlgorithmParameterException
    //        cryptographic strength exceed the legal limits
    //        (jurisdiction policy files)
    AutoPtr<ICipherSpi> spi;
    GetSpi(key, (ICipherSpi**)&spi);
    spi->EngineInit(opmode, key, params, random);
    mMode = opmode;
    return NOERROR;
}

ECode Cipher::Init(
    /* [in] */ Int32 opmode,
    /* [in] */ ICertificate * certificate)
{
    if (mSecureRandom == NULL) {
//TODO: Need CSecureRandom
        // CSecureRandom::New((ISecureRandom**)&mSecureRandom);
    }
    return Init(opmode, certificate, mSecureRandom);
}

ECode Cipher::Init(
    /* [in] */ Int32 opmode,
    /* [in] */ ICertificate * certificate,
    /* [in] */ ISecureRandom * random)
{
    CheckMode(opmode);
    AutoPtr<IX509Certificate> cert = IX509Certificate::Probe(certificate);
    if (cert != NULL) {
        AutoPtr<ISet> ce;
        IX509Extension::Probe(cert)->GetCriticalExtensionOIDs((ISet**)&ce);
        Boolean critical = FALSE;
        if (ce != NULL) {
            Boolean b;
            ce->IsEmpty(&b);
            if(!b) {
                AutoPtr<IIterator> it;
                ce->GetIterator((IIterator**)&it);
                Boolean hasNext;
                while (it->HasNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> cs;
                    it->GetNext((IInterface**)&cs);
                    String oid;
                    ICharSequence::Probe(cs)->ToString(&oid);
                    if (oid.Equals("2.5.29.15")) { // KeyUsage OID = 2.5.29.15
                        critical = TRUE;
                        break;
                    }
                }

                if (critical) {
                    AutoPtr<ArrayOf<Boolean> > keyUsage;
                    cert->GetKeyUsage((ArrayOf<Boolean>**)&keyUsage);
                    // As specified in RFC 3280:
                    //   Internet X.509 Public Key Infrastructure
                    //   Certificate and Certificate Revocation List (CRL) Profile.
                    // Section 4.2.1.3  Key Usage
                    // http://www.ietf.org/rfc/rfc3280.txt
                    //
                    // KeyUsage ::= BIT STRING {digitalSignature (0),
                    //                          nonRepudiation   (1),
                    //                          keyEncipherment  (2),
                    //                          dataEncipherment (3),
                    //                          keyAgreement     (4),
                    //                          keyCertSign      (5),
                    //                          cRLSign          (6),
                    //                          encipherOnly     (7),
                    //                          decipherOnly     (8) }
                    if (keyUsage != NULL) {
                        if (opmode == ENCRYPT_MODE && !(*keyUsage)[3]) {
                            // throw new InvalidKeyException("The public key in the certificate "
                            //                               + "cannot be used for ENCRYPT_MODE");
                            return E_INVALID_KEY_EXCEPTION;
                        } else if (opmode == WRAP_MODE && !(*keyUsage)[2]) {
                            // throw new InvalidKeyException("The public key in the certificate "
                            //                               + "cannot be used for WRAP_MODE");
                            return E_INVALID_KEY_EXCEPTION;
                        }
                    }
                }
            }
        }
    }
    //        FIXME InvalidKeyException
    //        if keysize exceeds the maximum allowable keysize
    //        (jurisdiction policy files)
    AutoPtr<IPublicKey> pkey;
    certificate->GetPublicKey((IPublicKey**)&pkey);
    AutoPtr<IKey> key = IKey::Probe(pkey);
    AutoPtr<ICipherSpi> spi;
    GetSpi(key, (ICipherSpi**)&spi);
    spi->EngineInit(opmode, key, random);
    mMode = opmode;
    return NOERROR;
}

ECode Cipher::Update(
    /* [in] */ ArrayOf<Byte> * input,
    /* [out, callee] */ ArrayOf<Byte> ** output)
{
    VALIDATE_NOT_NULL(output);
    *output = NULL;

    if (mMode != ENCRYPT_MODE && mMode != DECRYPT_MODE) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (input == NULL) {
        // throw new IllegalArgumentException("input == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (input->GetLength() == 0) {
        *output = NULL;
        return NOERROR;
    }
    AutoPtr<ICipherSpi> spi;
    GetSpi((ICipherSpi**)&spi);
    return spi->EngineUpdate(input, 0, input->GetLength(), output);
}

ECode Cipher::Update(
    /* [in] */ ArrayOf<Byte> * input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [out, callee] */ ArrayOf<Byte> ** output)
{
    VALIDATE_NOT_NULL(output);
    *output = NULL;

    if (mMode != ENCRYPT_MODE && mMode != DECRYPT_MODE) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (input == NULL) {
        // throw new IllegalArgumentException("input == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    CheckInputOffsetAndCount(input->GetLength(), inputOffset, inputLen);
    if (input->GetLength() == 0) {
        *output = NULL;
        return NOERROR;
    }
    AutoPtr<ICipherSpi> spi;
    GetSpi((ICipherSpi**)&spi);
    return spi->EngineUpdate(input, inputOffset, inputLen, output);
}

ECode Cipher::Update(
    /* [in] */ ArrayOf<Byte> * input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [in] */ ArrayOf<Byte> * output,
    /* [out] */ Int32 * value)
{
    return Update(input, inputOffset, inputLen, output, 0, value);
}

ECode Cipher::Update(
    /* [in] */ ArrayOf<Byte> * input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [in] */ ArrayOf<Byte> * output,
    /* [in] */ Int32 outputOffset,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    if (mMode != ENCRYPT_MODE && mMode != DECRYPT_MODE) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (input == NULL) {
        // throw new IllegalArgumentException("input == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (output == NULL) {
        // throw new IllegalArgumentException("output == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (outputOffset < 0) {
        // throw new IllegalArgumentException("outputOffset < 0. outputOffset=" + outputOffset);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    CheckInputOffsetAndCount(input->GetLength(), inputOffset, inputLen);
    if (input->GetLength() == 0) {
        *value = 0;
        return NOERROR;
    }
    AutoPtr<ICipherSpi> spi;
    GetSpi((ICipherSpi**)&spi);
    return spi->EngineUpdate(input, inputOffset, inputLen, output,
            outputOffset, value);
}

ECode Cipher::Update(
    /* [in] */ IByteBuffer * input,
    /* [in] */ IByteBuffer * output,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    if (mMode != ENCRYPT_MODE && mMode != DECRYPT_MODE) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (input == output) {
        // throw new IllegalArgumentException("input == output");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICipherSpi> spi;
    GetSpi((ICipherSpi**)&spi);
    return spi->EngineUpdate(input, output, value);
}

ECode Cipher::UpdateAAD(
    /* [in] */ ArrayOf<Byte> * input)
{
    if (input == NULL) {
        // throw new IllegalArgumentException("input == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mMode != ENCRYPT_MODE && mMode != DECRYPT_MODE) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (input->GetLength() == 0) {
        return NOERROR;
    }
    AutoPtr<ICipherSpi> spi;
    GetSpi((ICipherSpi**)&spi);
//TODO: Need ICipherSpi::EngineUpdateAAD
   // return spi->EngineUpdateAAD(input, 0, input->GetLength());
    return NOERROR;
}

ECode Cipher::UpdateAAD(
    /* [in] */ ArrayOf<Byte> * input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen)
{
    if (mMode != ENCRYPT_MODE && mMode != DECRYPT_MODE) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (input == NULL) {
        // throw new IllegalArgumentException("input == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    CheckInputOffsetAndCount(input->GetLength(), inputOffset, inputLen);
    if (input->GetLength() == 0) {
        return NOERROR;
    }
    AutoPtr<ICipherSpi> spi;
    GetSpi((ICipherSpi**)&spi);
//TODO: Need ICipherSpi::EngineUpdateAAD
    // return spi->EngineUpdateAAD(input, inputOffset, inputLen);
    return NOERROR;
}

ECode Cipher::UpdateAAD(
    /* [in] */ IByteBuffer * input)
{
    if (mMode != ENCRYPT_MODE && mMode != DECRYPT_MODE) {
        // throw new IllegalStateException("Cipher is not initialized");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (input == NULL) {
        // throw new IllegalArgumentException("input == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICipherSpi> spi;
    GetSpi((ICipherSpi**)&spi);
//TODO: Need ICipherSpi::EngineUpdateAAD
    // return spi->EngineUpdateAAD(input);
    return NOERROR;
}

ECode Cipher::DoFinal(
    /* [out, callee] */ ArrayOf<Byte> ** buf)
{
    VALIDATE_NOT_NULL(buf)
    *buf = NULL;

    if (mMode != ENCRYPT_MODE && mMode != DECRYPT_MODE) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<ICipherSpi> spi;
    GetSpi((ICipherSpi**)&spi);
    return spi->EngineDoFinal(NULL, 0, 0, buf);
}

ECode Cipher::DoFinal(
    /* [in] */ ArrayOf<Byte> * output,
    /* [in] */ Int32 outputOffset,
    /* [out] */ Int32 * number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;

    if (mMode != ENCRYPT_MODE && mMode != DECRYPT_MODE) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (outputOffset < 0) {
        // throw new IllegalArgumentException("outputOffset < 0. outputOffset=" + outputOffset);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICipherSpi> spi;
    GetSpi((ICipherSpi**)&spi);
    return spi->EngineDoFinal(NULL, 0, 0, output, outputOffset, number);
}

ECode Cipher::DoFinal(
    /* [in] */ ArrayOf<Byte> * input,
    /* [out, callee] */ ArrayOf<Byte> ** output)
{
    VALIDATE_NOT_NULL(output);
    *output = NULL;

    if (mMode != ENCRYPT_MODE && mMode != DECRYPT_MODE) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<ICipherSpi> spi;
    GetSpi((ICipherSpi**)&spi);
    return spi->EngineDoFinal(input, 0, input->GetLength(), output);
}

ECode Cipher::DoFinal(
    /* [in] */ ArrayOf<Byte> * input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [out, callee] */ ArrayOf<Byte> ** output)
{
    VALIDATE_NOT_NULL(output);
    *output = NULL;

    if (mMode != ENCRYPT_MODE && mMode != DECRYPT_MODE) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    CheckInputOffsetAndCount(input->GetLength(), inputOffset, inputLen);
    AutoPtr<ICipherSpi> spi;
    GetSpi((ICipherSpi**)&spi);
    return spi->EngineDoFinal(input, inputOffset, inputLen, output);
}

ECode Cipher::DoFinal(
    /* [in] */ ArrayOf<Byte> * input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [in] */ ArrayOf<Byte> * output,
    /* [out] */ Int32 * number)
{
    return DoFinal(input, inputOffset, inputLen, output, 0, number);
}

ECode Cipher::DoFinal(
    /* [in] */ ArrayOf<Byte> * input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [in] */ ArrayOf<Byte> * output,
    /* [in] */ Int32 outputOffset,
    /* [out] */ Int32 * number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;

    if (mMode != ENCRYPT_MODE && mMode != DECRYPT_MODE) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    CheckInputOffsetAndCount(input->GetLength(), inputOffset, inputLen);
    AutoPtr<ICipherSpi> spi;
    GetSpi((ICipherSpi**)&spi);
    return spi->EngineDoFinal(input, inputOffset, inputLen, output,
            outputOffset, number);
}

ECode Cipher::DoFinal(
    /* [in] */ IByteBuffer * input,
    /* [in] */ IByteBuffer * output,
    /* [out] */ Int32 * number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;

    if (mMode != ENCRYPT_MODE && mMode != DECRYPT_MODE) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (input == output) {
        // throw new IllegalArgumentException("input == output");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICipherSpi> spi;
    GetSpi((ICipherSpi**)&spi);
    return spi->EngineDoFinal(input, output, number);
}

ECode Cipher::Wrap(
    /* [in] */ IKey * key,
    /* [out, callee] */ ArrayOf<Byte> ** array)
{
    VALIDATE_NOT_NULL(array);
    *array = NULL;

    if (mMode != WRAP_MODE) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<ICipherSpi> spi;
    GetSpi((ICipherSpi**)&spi);
    return spi->EngineWrap(key, array);
}

ECode Cipher::Unwrap(
    /* [in] */ ArrayOf<Byte> * wrappedKey,
    /* [in] */ const String& wrappedKeyAlgorithm,
    /* [in] */ Int32 wrappedKeyType,
    /* [out] */ IKey ** key)
{
    VALIDATE_NOT_NULL(key);
    *key = NULL;

    if (mMode != UNWRAP_MODE) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<ICipherSpi> spi;
    GetSpi((ICipherSpi**)&spi);
    return spi->EngineUnwrap(wrappedKey, wrappedKeyAlgorithm,
            wrappedKeyType, key);
}

ECode Cipher::GetInstance(
    /* [in] */ const String& transformation,
    /* [out] */ ICipher ** cipher)
{
	return GetCipher(transformation, NULL, cipher);
}

ECode Cipher::GetInstance(
    /* [in] */ const String& transformation,
    /* [in] */ const String& provider,
    /* [out] */ ICipher ** cipher)
{
    VALIDATE_NOT_NULL(cipher);
    *cipher = NULL;

    if (provider == NULL) {
        // throw new IllegalArgumentException("provider == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IProvider> p;
    AutoPtr<ISecurity> security;
//TODO: Need CSecurity
    // CSecurity::AcquireSingleton((ISecurity**)&security);
    security->GetProvider(provider, (IProvider**)&p);
    if (p == NULL) {
        // throw new NoSuchProviderException("Provider not available: " + provider);
        return E_NO_SUCH_PROVIDER_EXCEPTION;
    }
    return GetInstance(transformation, p, cipher);
}

ECode Cipher::GetInstance(
    /* [in] */ const String& transformation,
    /* [in] */ IProvider * provider,
    /* [out] */ ICipher ** cipher)
{
	VALIDATE_NOT_NULL(cipher);
    *cipher = NULL;
    if (provider == NULL) {
        // throw new IllegalArgumentException("provider == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return GetCipher(transformation, provider, cipher);
}

ECode Cipher::GetMaxAllowedKeyLength(
    /* [in] */ const String& transformation,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    if (transformation == NULL) {
        // throw new NullPointerException("transformation == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ArrayOf<String> > transformParts;
    CheckTransformation(transformation, (ArrayOf<String>**)&transformParts);
    //FIXME jurisdiction policy files
    *value = 0x7FFFFFFF/*IInteger::MAX_VALUE*/;
    return NOERROR;
}

ECode Cipher::GetMaxAllowedParameterSpec(
    /* [in] */ const String& transformation,
    /* [out] */ IAlgorithmParameterSpec ** value)
{
    VALIDATE_NOT_NULL(value);
    *value = NULL;

    if (transformation == NULL) {
        // throw new NullPointerException("transformation == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ArrayOf<String> > transformParts;
    CheckTransformation(transformation, (ArrayOf<String>**)&transformParts);
    //FIXME jurisdiction policy files
    *value = NULL;
    return NOERROR;
}

ECode Cipher::GetCipher(
    /* [in] */ const String& transformation,
    /* [in] */ IProvider* provider,
    /* [out] */ ICipher** out)
{
    VALIDATE_NOT_NULL(out);
    *out = NULL;

    if (transformation == NULL || transformation.IsEmpty()) {
        // throw invalidTransformation(transformation);
        return E_INVALID_TRANSFORMATION;
    }

    AutoPtr<ArrayOf<String> > transformParts;
    CheckTransformation(transformation, (ArrayOf<String>**)&transformParts);
//TODO: Need IEngine
    // if (TryCombinations(NULL, provider, transformParts) == NULL) {
    //     if (provider == NULL) {
    //         // throw new NoSuchAlgorithmException("No provider found for " + transformation);
    //         return E_NO_SUCH_ALGORITHM_EXCEPTION;
    //     } else {
    //         // throw new NoSuchAlgorithmException("Provider " + provider.getName()
    //         //         + " does not provide " + transformation);
    //         return E_NO_SUCH_ALGORITHM_EXCEPTION;
    //     }
    // }
    CCipher::New(transformation, transformParts, provider, out);
    return NOERROR;
}

ECode Cipher::CheckTransformation(
    /* [in] */ const String& _transformation,
    /* [out] */ ArrayOf<String>** out)
{
    String transformation = _transformation;
    VALIDATE_NOT_NULL(out);
    *out = NULL;
    // ignore an extra prefix / characters such as in
    // "/DES/CBC/PKCS5Padding" http://b/3387688
    if (transformation.StartWith("/")) {
        transformation = transformation.Substring(1);
    }
    // 'transformation' should be of the form "algorithm/mode/padding".
    AutoPtr<ArrayOf<String> > pieces;
    StringUtils::Split(transformation, "/", (ArrayOf<String>**)&pieces);
    if (pieces->GetLength() > 3) {
        // throw invalidTransformation(transformation);
        return E_INVALID_TRANSFORMATION;
    }
    // Empty or missing pieces are represented by NULL.
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(3);
    for (int i = 0; i < pieces->GetLength(); ++i) {
        String piece = (*pieces)[i].Trim();
        if (!piece.IsEmpty()) {
            (*result)[i] = piece;
        }
    }
    // You MUST specify an algorithm.
    if ((*result)[0] == NULL) {
        // throw invalidTransformation(transformation);
        return E_INVALID_TRANSFORMATION;
    }
    if (!((*result)[1] == NULL && (*result)[2] == NULL) && ((*result)[1] == NULL || (*result)[2] == NULL)) {
        // throw invalidTransformation(transformation);
        return E_INVALID_TRANSFORMATION;
    }
    *out = result;
    REFCOUNT_ADD(*out);
    return NOERROR;
}

ECode Cipher::GetSpi(
    /* [in] */ IKey* key,
    /* [out] */ ICipherSpi** spi)
{
    VALIDATE_NOT_NULL(spi);
    *spi = NULL;
    if (mSpecifiedSpi != NULL) {
        *spi = mSpecifiedSpi;
        REFCOUNT_ADD(*spi);
        return NOERROR;
    }

    synchronized(mInitLock) {
        if (mSpiImpl != NULL && key == NULL) {
            *spi = mSpiImpl;
            REFCOUNT_ADD(*spi);
            return NOERROR;
        }

//TODO: Need IEngine
        // AutoPtr<Engine.SpiAndProvider> sap = TryCombinations(key, specifiedProvider, mTransformParts);
        // if (sap == NULL) {
        //     // throw new ProviderException("No provider for " + transformation);
        //     return E_PROVIDER_EXCEPTION;
        // }

        // mSpiImpl = ICipherSpi::Probe(sap.spi);
        // mProvider = sap.provider;

        *spi = mSpiImpl;
        REFCOUNT_ADD(*spi);
        return NOERROR;
    }
    return NOERROR;
}

ECode Cipher::GetSpi(
    /* [out] */ ICipherSpi** spi)
{
    return GetSpi(spi);
}

//TODO: Need IEngine
// AutoPtr<Engine.SpiAndProvider> Cipher::TryCombinations(
//     /* [in] */ IKey* key,
//     /* [in] */ IProvider* provider,
//     /* [in] */ ArrayOf<String> * transformParts)
// {
//     Engine.SpiAndProvider sap = null;

//     if (transformParts[1] != null && transformParts[2] != null) {
//         sap = tryTransform(key, provider, transformParts[0] + "/" + transformParts[1] + "/"
//                 + transformParts[2], transformParts, NeedToSet.NONE);
//         if (sap != null) {
//             return sap;
//         }
//     }

//     if (transformParts[1] != null) {
//         sap = tryTransform(key, provider, transformParts[0] + "/" + transformParts[1],
//                 transformParts, NeedToSet.PADDING);
//         if (sap != null) {
//             return sap;
//         }
//     }

//     if (transformParts[2] != null) {
//         sap = tryTransform(key, provider, transformParts[0] + "//" + transformParts[2],
//                 transformParts, NeedToSet.MODE);
//         if (sap != null) {
//             return sap;
//         }
//     }

//     return tryTransform(key, provider, transformParts[0], transformParts, NeedToSet.BOTH);
// }

// AutoPtr<Engine.SpiAndProvider> Cipher::TryTransform(
//     /* [in] */ IKey* key,
//     /* [in] */ IProvider* provider,
//     /* [in] */ const String& transform,
//     /* [in] */ ArrayOf<String> * transformParts,
//     /* [in] */ NeedToSet type)
// {
//     if (provider != null) {
//         Provider.Service service = provider.getService(SERVICE, transform);
//         if (service == null) {
//             return null;
//         }
//         return tryTransformWithProvider(key, transformParts, type, service);
//     }
//     ArrayList<Provider.Service> services = ENGINE.getServices(transform);
//     if (services == null) {
//         return null;
//     }
//     for (Provider.Service service : services) {
//         Engine.SpiAndProvider sap = tryTransformWithProvider(key, transformParts, type, service);
//         if (sap != null) {
//             return sap;
//         }
//     }
//     return null;
// }

// AutoPtr<Engine.SpiAndProvider> Cipher::TryTransformWithProvider(
//     /* [in] */ IKey* key,
//     /* [in] */ ArrayOf<String> * transformParts,
//     /* [in] */ NeedToSet type,
//     /* [in] */ IProvider.Service * service)
// {
//     try {
//         if (key != null && !service.supportsParameter(key)) {
//             return null;
//         }

//         /*
//          * Check to see if the Cipher even supports the attributes before
//          * trying to instantiate it.
//          */
//         if (!matchAttribute(service, ATTRIBUTE_MODES, transformParts[1])
//                 || !matchAttribute(service, ATTRIBUTE_PADDINGS, transformParts[2])) {
//             return null;
//         }

//         Engine.SpiAndProvider sap = ENGINE.getInstance(service, null);
//         if (sap.spi == null || sap.provider == null) {
//             return null;
//         }
//         if (!(sap.spi instanceof CipherSpi)) {
//             return null;
//         }
//         CipherSpi spi = (CipherSpi) sap.spi;
//         if (((type == NeedToSet.MODE) || (type == NeedToSet.BOTH))
//                 && (transformParts[1] != null)) {
//             spi.engineSetMode(transformParts[1]);
//         }
//         if (((type == NeedToSet.PADDING) || (type == NeedToSet.BOTH))
//                 && (transformParts[2] != null)) {
//             spi.engineSetPadding(transformParts[2]);
//         }
//         return sap;
//     } catch (NoSuchAlgorithmException ignored) {
//     } catch (NoSuchPaddingException ignored) {
//     }
//     return null;
// }

Boolean Cipher::MatchAttribute(
    /* [in] */ IProviderService* service,
    /* [in] */ const String& attr,
    /* [in] */ const String& value)
{
    if (value.IsNull()) {
        return TRUE;
    }
    String pattern;
    service->GetAttribute(attr, &pattern);
    if (pattern.IsNull()) {
        return TRUE;
    }
    Boolean b;
    StringUtils::Matches(value.ToUpperCase(), pattern.ToUpperCase(), &b);
    return b;
}

ECode Cipher::CheckMode(
    /* [in] */ Int32 mode)
{
    if (mode != ENCRYPT_MODE && mode != DECRYPT_MODE && mode != UNWRAP_MODE
            && mode != WRAP_MODE) {
        // throw new InvalidParameterException("Invalid mode: " + mode);
        return E_INVALID_PARAMETER_EXCEPTION;
    }
    return NOERROR;
}

ECode Cipher::CheckInputOffsetAndCount(
    /* [in] */ Int32 inputArrayLength,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen)
{
    if ((inputOffset | inputLen) < 0
            || inputOffset > inputArrayLength
            || inputArrayLength - inputOffset < inputLen) {
        // throw new IllegalArgumentException("input.length=" + inputArrayLength
        //                                    + "; inputOffset=" + inputOffset
        //                                    + "; inputLen=" + inputLen);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

} // Crypto
} // Elastosx