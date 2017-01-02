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

#include "elastos/devsamples/securitydemo/CipherTest.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Security.h"
#include "elastosx/crypto/Cipher.h"
#include <elastos/utility/Arrays.h>
#include "elastos/utility/logging/Logger.h"

using Elastos::Math::CBigInteger;
using Elastos::Math::IBigInteger;
using Elastos::Security::CKeyFactoryHelper;
using Elastos::Security::CSignatureHelper;
using Elastos::Security::CSecureRandomHelper;
using Elastos::Security::IKeyFactory;
using Elastos::Security::IKeyFactoryHelper;
using Elastos::Security::IPrivateKey;
using Elastos::Security::IPublicKey;
using Elastos::Security::ISignature;
using Elastos::Security::ISignatureHelper;
using Elastos::Security::ISecureRandomHelper;
using Elastos::Security::Spec::CRSAPrivateKeySpec;
using Elastos::Security::Spec::CRSAPublicKeySpec;
using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastos::Security::Spec::IKeySpec;
using Elastos::Security::Spec::IRSAPrivateKeySpec;
using Elastos::Security::Spec::IRSAPublicKeySpec;
using Elastos::Security::Spec::IPKCS8EncodedKeySpec;
using Elastos::Security::Spec::CPKCS8EncodedKeySpec;
using Elastos::Security::Spec::IX509EncodedKeySpec;
using Elastos::Security::Spec::CX509EncodedKeySpec;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;
using Elastosx::Crypto::Cipher;
using Elastosx::Crypto::CKeyGeneratorHelper;
using Elastosx::Crypto::CSecretKeyFactoryHelper;
using Elastosx::Crypto::ICipher;
using Elastosx::Crypto::IKeyGenerator;
using Elastosx::Crypto::IKeyGeneratorHelper;
using Elastosx::Crypto::ISecretKey;
using Elastosx::Crypto::ISecretKeyFactory;
using Elastosx::Crypto::ISecretKeyFactoryHelper;
using Elastosx::Crypto::Spec::CIvParameterSpec;
using Elastosx::Crypto::Spec::CSecretKeySpec;
using Elastosx::Crypto::Spec::CDESedeKeySpec;
using Elastosx::Crypto::Spec::IDESedeKeySpec;
using Elastosx::Crypto::Spec::ISecretKeySpec;
using Elastosx::Crypto::Spec::IIvParameterSpec;

namespace Elastos {
namespace DevSamples {
namespace SecurityDemo {

static const String TAG("CipherTest");
const String CipherTest::IV16("2222222211111111");
const String CipherTest::IV8("55555555");
ECode CipherTest::Test()
{
    String testData("kortideelastostd");
    String testKey16("kortideelastos16");
    String testKey24("kortideelastos2016111817");

    AES_CBC_NoPaddingTest(testData.GetBytes(), testKey16.GetBytes());
    AES_CBC_PKCS5PaddingTest(testData.GetBytes(), testKey16.GetBytes());
    AES_ECB_NoPaddingTest(testData.GetBytes(), testKey16.GetBytes());
    AES_ECB_PKCS5PaddingTest(testData.GetBytes(), testKey16.GetBytes());
    AES_CFB_NoPaddingTest(testData.GetBytes(), testKey16.GetBytes());
    AES_CTR_NoPaddingTest(testData.GetBytes(), testKey16.GetBytes());
    AES_OFB_NoPaddingTest(testData.GetBytes(), testKey16.GetBytes());

    DESEDE_ECB_NoPaddingTest(testData.GetBytes(), testKey16.GetBytes());
    DESEDE_ECB_PKCS5PaddingTest(testData.GetBytes(), testKey16.GetBytes());

    DESEDE_CBC_NoPaddingTest(testData.GetBytes(), testKey16.GetBytes());
    DESEDE_CBC_PKCS5PaddingTest(testData.GetBytes(), testKey16.GetBytes());

    DESEDE_CFB_NoPaddingTest(testData.GetBytes(), testKey24.GetBytes());
    DESEDE_OFB_NoPaddingTest(testData.GetBytes(), testKey24.GetBytes());

    ARC4Test(testData.GetBytes(), testKey24.GetBytes());
    return NOERROR;
}

void CipherTest::AES_CBC_NoPaddingTest(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ ArrayOf<Byte>* key)
{
    ECode ec = NOERROR;
    AutoPtr<ICipher> cipher;
    ec = Cipher::GetInstance(String("AES/CBC/NoPadding"), (ICipher**)&cipher);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ISecretKeySpec> secretKey;
    ec = CSecretKeySpec::New(key, String("AES"), (ISecretKeySpec**)&secretKey);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<IIvParameterSpec> ips;
    ec = CIvParameterSpec::New(IV16.GetBytes(), (IIvParameterSpec**)&ips);
    ASSERT_SUCCEEDED(ec);

    ec = cipher->Init(ICipher::ENCRYPT_MODE, IKey::Probe(secretKey), IAlgorithmParameterSpec::Probe(ips));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > encryptData;
    ec = cipher->DoFinal(data, (ArrayOf<Byte>**)&encryptData);
    ASSERT_SUCCEEDED(ec);

    ips = NULL;
    ec = CIvParameterSpec::New(IV16.GetBytes(), (IIvParameterSpec**)&ips);
    ASSERT_SUCCEEDED(ec);
    ec = cipher->Init(ICipher::DECRYPT_MODE, IKey::Probe(secretKey), IAlgorithmParameterSpec::Probe(ips));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > tdata;
    ec = cipher->DoFinal(encryptData, (ArrayOf<Byte>**)&tdata);
    ASSERT_SUCCEEDED(ec);
    String result(*tdata);

    Logger::D(TAG, "AES_CBC_NoPaddingTest result:[%s]", result.string());
 }

void CipherTest::AES_CBC_PKCS5PaddingTest(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ ArrayOf<Byte>* key)
{
    ECode ec = NOERROR;
    AutoPtr<ICipher> cipher;
    ec = Cipher::GetInstance(String("AES/CBC/PKCS5Padding"), (ICipher**)&cipher);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ISecretKeySpec> secretKey;
    ec = CSecretKeySpec::New(key, String("AES"), (ISecretKeySpec**)&secretKey);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<IIvParameterSpec> ips;
    ec = CIvParameterSpec::New(IV16.GetBytes(), (IIvParameterSpec**)&ips);
    ASSERT_SUCCEEDED(ec);
    cipher->Init(ICipher::ENCRYPT_MODE, IKey::Probe(secretKey), IAlgorithmParameterSpec::Probe(ips));

    AutoPtr<ArrayOf<Byte> > encryptData;
    ec = cipher->DoFinal(data, (ArrayOf<Byte>**)&encryptData);
    ASSERT_SUCCEEDED(ec);

    ips = NULL;
    ec = CIvParameterSpec::New(IV16.GetBytes(), (IIvParameterSpec**)&ips);
    ASSERT_SUCCEEDED(ec);
    ec = cipher->Init(ICipher::DECRYPT_MODE, IKey::Probe(secretKey), IAlgorithmParameterSpec::Probe(ips));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > tdata;
    ec = cipher->DoFinal(encryptData, (ArrayOf<Byte>**)&tdata);
    ASSERT_SUCCEEDED(ec);
    String result(*tdata);

    Logger::D(TAG, "AES_CBC_PKCS5PaddingTest result:[%s]", result.string());
}

void CipherTest::AES_ECB_NoPaddingTest(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ ArrayOf<Byte>* key)
{
    ECode ec = NOERROR;
    AutoPtr<ICipher> cipher;
    ec = Cipher::GetInstance(String("AES/ECB/NoPadding"), (ICipher**)&cipher);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ISecretKeySpec> secretKey;
    ec = CSecretKeySpec::New(key, String("AES"), (ISecretKeySpec**)&secretKey);
    ASSERT_SUCCEEDED(ec);

    ec = cipher->Init(ICipher::ENCRYPT_MODE, IKey::Probe(secretKey));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > encryptData;
    ec = cipher->DoFinal(data, (ArrayOf<Byte>**)&encryptData);
    ASSERT_SUCCEEDED(ec);

    ec = cipher->Init(ICipher::DECRYPT_MODE, IKey::Probe(secretKey));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > tdata;
    ec = cipher->DoFinal(encryptData, (ArrayOf<Byte>**)&tdata);
    ASSERT_SUCCEEDED(ec);
    String result(*tdata);

    Logger::D(TAG, "AES_ECB_NoPaddingTest result:[%s]", result.string());
}

void CipherTest::AES_ECB_PKCS5PaddingTest(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ ArrayOf<Byte>* key)
{
    ECode ec = NOERROR;
    AutoPtr<ICipher> cipher;
    ec = Cipher::GetInstance(String("AES/ECB/PKCS5Padding"), (ICipher**)&cipher);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ISecretKeySpec> secretKey;
    ec = CSecretKeySpec::New(key, String("AES"), (ISecretKeySpec**)&secretKey);
    ASSERT_SUCCEEDED(ec);
    ec = cipher->Init(ICipher::ENCRYPT_MODE, IKey::Probe(secretKey));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > encryptData;
    ec = cipher->DoFinal(data, (ArrayOf<Byte>**)&encryptData);
    ASSERT_SUCCEEDED(ec);

    ec = cipher->Init(ICipher::DECRYPT_MODE, IKey::Probe(secretKey));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > tdata;
    ec = cipher->DoFinal(encryptData, (ArrayOf<Byte>**)&tdata);
    ASSERT_SUCCEEDED(ec);
    String result(*tdata);

    Logger::D(TAG, "AES_ECB_PKCS5PaddingTest result:[%s]", result.string());
}

void CipherTest::AES_CFB_NoPaddingTest(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ ArrayOf<Byte>* key)
{
    ECode ec = NOERROR;
    AutoPtr<ICipher> cipher;
    ec = Cipher::GetInstance(String("AES/CFB/NoPadding"), (ICipher**)&cipher);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ISecretKeySpec> secretKey;
    ec = CSecretKeySpec::New(key, String("AES"), (ISecretKeySpec**)&secretKey);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<IIvParameterSpec> ips;
    ec = CIvParameterSpec::New(IV16.GetBytes(), (IIvParameterSpec**)&ips);
    ASSERT_SUCCEEDED(ec);
    ec = cipher->Init(ICipher::ENCRYPT_MODE, IKey::Probe(secretKey), IAlgorithmParameterSpec::Probe(ips));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > encryptData;
    ec = cipher->DoFinal(data, (ArrayOf<Byte>**)&encryptData);
    ASSERT_SUCCEEDED(ec);

    ips = NULL;
    ec = CIvParameterSpec::New(IV16.GetBytes(), (IIvParameterSpec**)&ips);
    ASSERT_SUCCEEDED(ec);
    cipher->Init(ICipher::DECRYPT_MODE, IKey::Probe(secretKey), IAlgorithmParameterSpec::Probe(ips));

    AutoPtr<ArrayOf<Byte> > tdata;
    ec = cipher->DoFinal(encryptData, (ArrayOf<Byte>**)&tdata);
    ASSERT_SUCCEEDED(ec);
    String result(*tdata);

    Logger::D(TAG, "AES_CFB_NoPaddingTest result:[%s]", result.string());
}

void CipherTest::AES_CTR_NoPaddingTest(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ ArrayOf<Byte>* key)
{
    ECode ec = NOERROR;
    AutoPtr<ICipher> cipher;
    ec = Cipher::GetInstance(String("AES/CTR/NoPadding"), (ICipher**)&cipher);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ISecretKeySpec> secretKey;
    ec = CSecretKeySpec::New(key, String("AES"), (ISecretKeySpec**)&secretKey);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<IIvParameterSpec> ips;
    ec = CIvParameterSpec::New(IV16.GetBytes(), (IIvParameterSpec**)&ips);
    ASSERT_SUCCEEDED(ec);
    ec = cipher->Init(ICipher::ENCRYPT_MODE, IKey::Probe(secretKey), IAlgorithmParameterSpec::Probe(ips));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > encryptData;
    ec = cipher->DoFinal(data, (ArrayOf<Byte>**)&encryptData);
    ASSERT_SUCCEEDED(ec);

    ips = NULL;
    ec = CIvParameterSpec::New(IV16.GetBytes(), (IIvParameterSpec**)&ips);
    ASSERT_SUCCEEDED(ec);
    ec = cipher->Init(ICipher::DECRYPT_MODE, IKey::Probe(secretKey), IAlgorithmParameterSpec::Probe(ips));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > tdata;
    ec = cipher->DoFinal(encryptData, (ArrayOf<Byte>**)&tdata);
    ASSERT_SUCCEEDED(ec);
    String result(*tdata);

    Logger::D(TAG, "AES_CTR_NoPaddingTest result:[%s]", result.string());
}

void CipherTest::AES_OFB_NoPaddingTest(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ ArrayOf<Byte>* key)
{
    ECode ec = NOERROR;
    AutoPtr<ICipher> cipher;
    ec = Cipher::GetInstance(String("AES/OFB/NoPadding"), (ICipher**)&cipher);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ISecretKeySpec> secretKey;
    ec = CSecretKeySpec::New(key, String("AES"), (ISecretKeySpec**)&secretKey);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<IIvParameterSpec> ips;
    ec = CIvParameterSpec::New(IV16.GetBytes(), (IIvParameterSpec**)&ips);
    ASSERT_SUCCEEDED(ec);
    ec = cipher->Init(ICipher::ENCRYPT_MODE, IKey::Probe(secretKey), IAlgorithmParameterSpec::Probe(ips));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > encryptData;
    ec = cipher->DoFinal(data, (ArrayOf<Byte>**)&encryptData);
    ASSERT_SUCCEEDED(ec);

    ips = NULL;
    ec = CIvParameterSpec::New(IV16.GetBytes(), (IIvParameterSpec**)&ips);
    ASSERT_SUCCEEDED(ec);
    ec = cipher->Init(ICipher::DECRYPT_MODE, IKey::Probe(secretKey), IAlgorithmParameterSpec::Probe(ips));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > tdata;
    ec = cipher->DoFinal(encryptData, (ArrayOf<Byte>**)&tdata);
    ASSERT_SUCCEEDED(ec);
    String result(*tdata);

    Logger::D(TAG, "AES_OFB_NoPaddingTest result:[%s]", result.string());
}

void CipherTest::DESEDE_ECB_NoPaddingTest(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ ArrayOf<Byte>* key)
{
    ECode ec = NOERROR;
    AutoPtr<ICipher> cipher;
    ec = Cipher::GetInstance(String("DESEDE/ECB/NoPadding"), (ICipher**)&cipher);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ISecretKeySpec> secretKey;
    ec = CSecretKeySpec::New(key, String("DESEDE"), (ISecretKeySpec**)&secretKey);
    ASSERT_SUCCEEDED(ec);
    ec = cipher->Init(ICipher::ENCRYPT_MODE, IKey::Probe(secretKey));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > encryptData;
    ec = cipher->DoFinal(data, (ArrayOf<Byte>**)&encryptData);
    ASSERT_SUCCEEDED(ec);

    ec = cipher->Init(ICipher::DECRYPT_MODE, IKey::Probe(secretKey));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > tdata;
    ec = cipher->DoFinal(encryptData, (ArrayOf<Byte>**)&tdata);
    ASSERT_SUCCEEDED(ec);
    String result(*tdata);

    Logger::D(TAG, "DESEDE_ECB_NoPaddingTest result:[%s]", result.string());
}

void CipherTest::DESEDE_ECB_PKCS5PaddingTest(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ ArrayOf<Byte>* key)
{
    ECode ec = NOERROR;
    AutoPtr<ICipher> cipher;
    ec = Cipher::GetInstance(String("DESEDE/ECB/PKCS5Padding"), (ICipher**)&cipher);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ISecretKeySpec> secretKey;
    ec = CSecretKeySpec::New(key, String("DESEDE"), (ISecretKeySpec**)&secretKey);
    ASSERT_SUCCEEDED(ec);
    ec = cipher->Init(ICipher::ENCRYPT_MODE, IKey::Probe(secretKey));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > encryptData;
    ec = cipher->DoFinal(data, (ArrayOf<Byte>**)&encryptData);
    ASSERT_SUCCEEDED(ec);

    ec = cipher->Init(ICipher::DECRYPT_MODE, IKey::Probe(secretKey));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > tdata;
    ec = cipher->DoFinal(encryptData, (ArrayOf<Byte>**)&tdata);
    ASSERT_SUCCEEDED(ec);
    String result(*tdata);

    Logger::D(TAG, "DESEDE_ECB_PKCS5PaddingTest result:[%s]", result.string());
}

void CipherTest::DESEDE_CBC_NoPaddingTest(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ ArrayOf<Byte>* key)
{
    ECode ec = NOERROR;
    AutoPtr<ICipher> cipher;
    ec = Cipher::GetInstance(String("DESEDE/CBC/NoPadding"), (ICipher**)&cipher);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ISecretKeySpec> secretKey;
    ec = CSecretKeySpec::New(key, String("DESEDE"), (ISecretKeySpec**)&secretKey);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<IIvParameterSpec> ips;
    ec = CIvParameterSpec::New(IV8.GetBytes(), (IIvParameterSpec**)&ips);
    ASSERT_SUCCEEDED(ec);
    ec = cipher->Init(ICipher::ENCRYPT_MODE, IKey::Probe(secretKey), IAlgorithmParameterSpec::Probe(ips));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > encryptData;
    ec = cipher->DoFinal(data, (ArrayOf<Byte>**)&encryptData);
    ASSERT_SUCCEEDED(ec);

    ips = NULL;
    ec = CIvParameterSpec::New(IV8.GetBytes(), (IIvParameterSpec**)&ips);
    ASSERT_SUCCEEDED(ec);
    ec = cipher->Init(ICipher::DECRYPT_MODE, IKey::Probe(secretKey), IAlgorithmParameterSpec::Probe(ips));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > tdata;
    ec = cipher->DoFinal(encryptData, (ArrayOf<Byte>**)&tdata);
    ASSERT_SUCCEEDED(ec);
    String result(*tdata);

    Logger::D(TAG, "DESEDE_CBC_NoPaddingTest result:[%s]", result.string());
}

void CipherTest::DESEDE_CBC_PKCS5PaddingTest(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ ArrayOf<Byte>* key)
{
    ECode ec = NOERROR;
    AutoPtr<ICipher> cipher;
    ec = Cipher::GetInstance(String("DESEDE/CBC/PKCS5Padding"), (ICipher**)&cipher);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ISecretKeySpec> secretKey;
    ec = CSecretKeySpec::New(key, String("DESEDE"), (ISecretKeySpec**)&secretKey);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<IIvParameterSpec> ips;
    ec = CIvParameterSpec::New(IV8.GetBytes(), (IIvParameterSpec**)&ips);
    ASSERT_SUCCEEDED(ec);
    ec = cipher->Init(ICipher::ENCRYPT_MODE, IKey::Probe(secretKey), IAlgorithmParameterSpec::Probe(ips));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > encryptData;
    ec = cipher->DoFinal(data, (ArrayOf<Byte>**)&encryptData);
    ASSERT_SUCCEEDED(ec);


    ips = NULL;
    ec = CIvParameterSpec::New(IV8.GetBytes(), (IIvParameterSpec**)&ips);
    ASSERT_SUCCEEDED(ec);
    ec = cipher->Init(ICipher::DECRYPT_MODE, IKey::Probe(secretKey), IAlgorithmParameterSpec::Probe(ips));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > tdata;
    ec = cipher->DoFinal(encryptData, (ArrayOf<Byte>**)&tdata);
    ASSERT_SUCCEEDED(ec);
    String result(*tdata);

    Logger::D(TAG, "DESEDE_CBC_PKCS5PaddingTest result:[%s]", result.string());
}

void CipherTest::DESEDE_CFB_NoPaddingTest(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ ArrayOf<Byte>* key)
{
    ECode ec = NOERROR;
    AutoPtr<ICipher> cipher;
    ec = Cipher::GetInstance(String("DESEDE/CFB/NoPadding"), (ICipher**)&cipher);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ISecretKeySpec> secretKey;
    ec = CSecretKeySpec::New(key, String("DESEDE"), (ISecretKeySpec**)&secretKey);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<IIvParameterSpec> ips;
    ec = CIvParameterSpec::New(IV8.GetBytes(), (IIvParameterSpec**)&ips);
    ASSERT_SUCCEEDED(ec);
    ec = cipher->Init(ICipher::ENCRYPT_MODE, IKey::Probe(secretKey), IAlgorithmParameterSpec::Probe(ips));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > encryptData;
    ec = cipher->DoFinal(data, (ArrayOf<Byte>**)&encryptData);
    ASSERT_SUCCEEDED(ec);

    ips = NULL;
    ec = CIvParameterSpec::New(IV8.GetBytes(), (IIvParameterSpec**)&ips);
    ASSERT_SUCCEEDED(ec);
    ec = cipher->Init(ICipher::DECRYPT_MODE, IKey::Probe(secretKey), IAlgorithmParameterSpec::Probe(ips));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > tdata;
    ec = cipher->DoFinal(encryptData, (ArrayOf<Byte>**)&tdata);
    ASSERT_SUCCEEDED(ec);
    String result(*tdata);

    Logger::D(TAG, "DESEDE_CFB_NoPaddingTest result:[%s]", result.string());
}

void CipherTest::DESEDE_OFB_NoPaddingTest(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ ArrayOf<Byte>* key)
{
    ECode ec = NOERROR;
    AutoPtr<ICipher> cipher;
    ec = Cipher::GetInstance(String("DESEDE/OFB/NoPadding"), (ICipher**)&cipher);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ISecretKeySpec> secretKey;
    ec = CSecretKeySpec::New(key, String("DESEDE"), (ISecretKeySpec**)&secretKey);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<IIvParameterSpec> ips;
    ec = CIvParameterSpec::New(IV8.GetBytes(), (IIvParameterSpec**)&ips);
    ASSERT_SUCCEEDED(ec);
    ec = cipher->Init(ICipher::ENCRYPT_MODE, IKey::Probe(secretKey), IAlgorithmParameterSpec::Probe(ips));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > encryptData;
    ec = cipher->DoFinal(data, (ArrayOf<Byte>**)&encryptData);
    ASSERT_SUCCEEDED(ec);

    ips = NULL;
    ec = CIvParameterSpec::New(IV8.GetBytes(), (IIvParameterSpec**)&ips);
    ASSERT_SUCCEEDED(ec);
    ec = cipher->Init(ICipher::DECRYPT_MODE, IKey::Probe(secretKey), IAlgorithmParameterSpec::Probe(ips));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > tdata;
    ec = cipher->DoFinal(encryptData, (ArrayOf<Byte>**)&tdata);
    ASSERT_SUCCEEDED(ec);
    String result(*tdata);

    Logger::D(TAG, "DESEDE_OFB_NoPaddingTest result:[%s]", result.string());
}

void CipherTest::ARC4Test(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ ArrayOf<Byte>* key)
{
    ECode ec = NOERROR;
    AutoPtr<ICipher> cipher;
    ec = Cipher::GetInstance(String("ARC4"), (ICipher**)&cipher);
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ISecretKeySpec> secretKey;
    ec = CSecretKeySpec::New(key, String("ARC4"), (ISecretKeySpec**)&secretKey);
    ASSERT_SUCCEEDED(ec);
    ec = cipher->Init(ICipher::ENCRYPT_MODE, IKey::Probe(secretKey));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > encryptData;
    ec = cipher->DoFinal(data, (ArrayOf<Byte>**)&encryptData);
    ASSERT_SUCCEEDED(ec);

    ec = cipher->Init(ICipher::DECRYPT_MODE, IKey::Probe(secretKey));
    ASSERT_SUCCEEDED(ec);

    AutoPtr<ArrayOf<Byte> > tdata;
    ec = cipher->DoFinal(encryptData, (ArrayOf<Byte>**)&tdata);
    ASSERT_SUCCEEDED(ec);
    String result(*tdata);

    Logger::D(TAG, "ARC4Test result:[%s]", result.string());
}

} // namespace SecurityDemo
} // namespace DevSamples
} // namespace Elastos
