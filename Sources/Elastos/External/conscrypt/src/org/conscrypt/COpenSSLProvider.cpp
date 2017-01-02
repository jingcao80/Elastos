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

#include "COpenSSLProvider.h"
#include "Platform.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Conscrypt {

const String COpenSSLProvider::PROVIDER_NAME("ElastosOpenSSL");

CAR_OBJECT_IMPL(COpenSSLProvider)

ECode COpenSSLProvider::constructor()
{
    return constructor(PROVIDER_NAME);
}

ECode COpenSSLProvider::constructor(
    /* [in] */ const String& providerName)
{
    //Logger::E("leliang", "TODO remove this COpenSSLProvider::constructor begin");
    FAIL_RETURN(Provider::constructor(providerName, 1.0, String("Elastos's OpenSSL-backed security provider")))

    // Make sure the platform is initialized.
    Platform::Setup();

    const String prefix("Org.Conscrypt.");

    /* === SSL Contexts === */
    const String classOpenSSLContextImpl = prefix + String("COpenSSLContextImpl");
    Put("SSLContext.SSL", classOpenSSLContextImpl);
    Put("SSLContext.SSLv3", classOpenSSLContextImpl);
    Put("SSLContext.TLS", classOpenSSLContextImpl);
    Put("SSLContext.TLSv1", classOpenSSLContextImpl);
    Put("SSLContext.TLSv1.1", classOpenSSLContextImpl);
    Put("SSLContext.TLSv1.2", classOpenSSLContextImpl);
    Put("SSLContext.Default", prefix + "CDefaultSSLContextImpl");

    /* === Message Digests === */
    Put("MessageDigest.SHA-1", prefix + "COpenSSLMessageDigestJDKSHA1");
    Put("Alg.Alias.MessageDigest.SHA1", "SHA-1");
    Put("Alg.Alias.MessageDigest.SHA", "SHA-1");
    Put("Alg.Alias.MessageDigest.1.3.14.3.2.26", "SHA-1");

    Put("MessageDigest.SHA-224", prefix + "COpenSSLMessageDigestJDKSHA224");
    Put("Alg.Alias.MessageDigest.SHA224", "SHA-224");
    Put("Alg.Alias.MessageDigest.2.16.840.1.101.3.4.2.4", "SHA-224");

    Put("MessageDigest.SHA-256", prefix + "COpenSSLMessageDigestJDKSHA256");
    Put("Alg.Alias.MessageDigest.SHA256", "SHA-256");
    Put("Alg.Alias.MessageDigest.2.16.840.1.101.3.4.2.1", "SHA-256");

    Put("MessageDigest.SHA-384", prefix + "COpenSSLMessageDigestJDKSHA384");
    Put("Alg.Alias.MessageDigest.SHA384", "SHA-384");
    Put("Alg.Alias.MessageDigest.2.16.840.1.101.3.4.2.2", "SHA-384");

    Put("MessageDigest.SHA-512", prefix + "COpenSSLMessageDigestJDKSHA512");
    Put("Alg.Alias.MessageDigest.SHA512", "SHA-512");
    Put("Alg.Alias.MessageDigest.2.16.840.1.101.3.4.2.3", "SHA-512");

    // iso(1) member-body(2) US(840) rsadsi(113549) digestAlgorithm(2) md5(5)
    Put("MessageDigest.MD5", prefix + "COpenSSLMessageDigestJDKMD5");
    Put("Alg.Alias.MessageDigest.1.2.840.113549.2.5", "MD5");

    /* == KeyPairGenerators == */
    Put("KeyPairGenerator.RSA", prefix + "COpenSSLRSAKeyPairGenerator");
    Put("Alg.Alias.KeyPairGenerator.1.2.840.113549.1.1.1", "RSA");

    Put("KeyPairGenerator.DH", prefix + "COpenSSLDHKeyPairGenerator");
    Put("Alg.Alias.KeyPairGenerator.1.2.840.113549.1.3.1", "DH");

    Put("KeyPairGenerator.DSA", prefix + "COpenSSLDSAKeyPairGenerator");

    Put("KeyPairGenerator.EC", prefix + "COpenSSLECKeyPairGenerator");

    /* == KeyFactory == */
    Put("KeyFactory.RSA", prefix + "COpenSSLRSAKeyFactory");
    Put("Alg.Alias.KeyFactory.1.2.840.113549.1.1.1", "RSA");

    Put("KeyFactory.DH", prefix + "COpenSSLDHKeyFactory");
    Put("Alg.Alias.KeyFactory.1.2.840.113549.1.3.1", "DH");

    Put("KeyFactory.DSA", prefix + "COpenSSLDSAKeyFactory");

    Put("KeyFactory.EC", prefix + "COpenSSLECKeyFactory");

    /* == KeyAgreement == */
    Put("KeyAgreement.ECDH", prefix + "COpenSSLECDHKeyAgreement");

    /* == Signatures == */
    Put("Signature.MD5WithRSA", prefix + "COpenSSLSignatureMD5RSA");
    Put("Alg.Alias.Signature.MD5WithRSAEncryption", "MD5WithRSA");
    Put("Alg.Alias.Signature.MD5/RSA", "MD5WithRSA");
    Put("Alg.Alias.Signature.1.2.840.113549.1.1.4", "MD5WithRSA");
    Put("Alg.Alias.Signature.1.2.840.113549.2.5with1.2.840.113549.1.1.1", "MD5WithRSA");

    Put("Signature.SHA1WithRSA", prefix + "COpenSSLSignatureSHA1RSA");
    Put("Alg.Alias.Signature.SHA1WithRSAEncryption", "SHA1WithRSA");
    Put("Alg.Alias.Signature.SHA1/RSA", "SHA1WithRSA");
    Put("Alg.Alias.Signature.SHA-1/RSA", "SHA1WithRSA");
    Put("Alg.Alias.Signature.1.2.840.113549.1.1.5", "SHA1WithRSA");
    Put("Alg.Alias.Signature.1.3.14.3.2.26with1.2.840.113549.1.1.1", "SHA1WithRSA");
    Put("Alg.Alias.Signature.1.3.14.3.2.26with1.2.840.113549.1.1.5", "SHA1WithRSA");
    Put("Alg.Alias.Signature.1.3.14.3.2.29", "SHA1WithRSA");

    Put("Signature.SHA224WithRSA", prefix + "COpenSSLSignatureSHA224RSA");
    Put("Alg.Alias.Signature.SHA224WithRSAEncryption", "SHA224WithRSA");
    Put("Alg.Alias.Signature.1.2.840.113549.1.1.11", "SHA224WithRSA");
    Put("Alg.Alias.Signature.2.16.840.1.101.3.4.2.4with1.2.840.113549.1.1.1",
            "SHA224WithRSA");
    Put("Alg.Alias.Signature.2.16.840.1.101.3.4.2.4with1.2.840.113549.1.1.11",
            "SHA224WithRSA");

    Put("Signature.SHA256WithRSA", prefix + "COpenSSLSignatureSHA256RSA");
    Put("Alg.Alias.Signature.SHA256WithRSAEncryption", "SHA256WithRSA");
    Put("Alg.Alias.Signature.1.2.840.113549.1.1.11", "SHA256WithRSA");
    Put("Alg.Alias.Signature.2.16.840.1.101.3.4.2.1with1.2.840.113549.1.1.1",
            "SHA256WithRSA");
    Put("Alg.Alias.Signature.2.16.840.1.101.3.4.2.1with1.2.840.113549.1.1.11",
            "SHA256WithRSA");

    Put("Signature.SHA384WithRSA", prefix + "COpenSSLSignatureSHA384RSA");
    Put("Alg.Alias.Signature.SHA384WithRSAEncryption", "SHA384WithRSA");
    Put("Alg.Alias.Signature.1.2.840.113549.1.1.12", "SHA384WithRSA");
    Put("Alg.Alias.Signature.2.16.840.1.101.3.4.2.2with1.2.840.113549.1.1.1",
            "SHA384WithRSA");

    Put("Signature.SHA512WithRSA", prefix + "COpenSSLSignatureSHA512RSA");
    Put("Alg.Alias.Signature.SHA512WithRSAEncryption", "SHA512WithRSA");
    Put("Alg.Alias.Signature.1.2.840.113549.1.1.13", "SHA512WithRSA");
    Put("Alg.Alias.Signature.2.16.840.1.101.3.4.2.3with1.2.840.113549.1.1.1",
            "SHA512WithRSA");

    Put("Signature.SHA1withDSA", prefix + "COpenSSLSignatureSHA1DSA");
    Put("Alg.Alias.Signature.SHA/DSA", "SHA1withDSA");
    Put("Alg.Alias.Signature.DSA", "SHA1withDSA");
    Put("Alg.Alias.Signature.1.3.14.3.2.26with1.2.840.10040.4.1", "SHA1withDSA");
    Put("Alg.Alias.Signature.1.3.14.3.2.26with1.2.840.10040.4.3", "SHA1withDSA");
    Put("Alg.Alias.Signature.DSAWithSHA1", "SHA1withDSA");
    Put("Alg.Alias.Signature.1.2.840.10040.4.3", "SHA1withDSA");

    Put("Signature.NONEwithRSA", prefix + "COpenSSLSignatureRawRSA");

    Put("Signature.ECDSA", prefix + "COpenSSLSignatureSHA1ECDSA");
    Put("Alg.Alias.Signature.SHA1withECDSA", "ECDSA");
    Put("Alg.Alias.Signature.ECDSAwithSHA1", "ECDSA");
    // iso(1) member-body(2) us(840) ansi-x962(10045) signatures(4) ecdsa-with-SHA1(1)
    Put("Alg.Alias.Signature.1.2.840.10045.4.1", "ECDSA");
    Put("Alg.Alias.Signature.1.3.14.3.2.26with1.2.840.10045.2.1", "ECDSA");

    // iso(1) member-body(2) us(840) ansi-x962(10045) signatures(4) ecdsa-with-SHA2(3)
    Put("Signature.SHA224withECDSA", prefix + "COpenSSLSignatureSHA224ECDSA");
    // ecdsa-with-SHA224(1)
    Put("Alg.Alias.Signature.1.2.840.10045.4.3.1", "SHA224withECDSA");
    Put("Alg.Alias.Signature.2.16.840.1.101.3.4.2.4with1.2.840.10045.2.1", "SHA224withECDSA");

    // iso(1) member-body(2) us(840) ansi-x962(10045) signatures(4) ecdsa-with-SHA2(3)
    Put("Signature.SHA256withECDSA", prefix + "COpenSSLSignatureSHA256ECDSA");
    // ecdsa-with-SHA256(2)
    Put("Alg.Alias.Signature.1.2.840.10045.4.3.2", "SHA256withECDSA");
    Put("Alg.Alias.Signature.2.16.840.1.101.3.4.2.1with1.2.840.10045.2.1", "SHA256withECDSA");

    Put("Signature.SHA384withECDSA", prefix + "COpenSSLSignatureSHA384ECDSA");
    // ecdsa-with-SHA384(3)
    Put("Alg.Alias.Signature.1.2.840.10045.4.3.3", "SHA384withECDSA");
    Put("Alg.Alias.Signature.2.16.840.1.101.3.4.2.2with1.2.840.10045.2.1", "SHA384withECDSA");

    Put("Signature.SHA512withECDSA", prefix + "COpenSSLSignatureSHA512ECDSA");
    // ecdsa-with-SHA512(4)
    Put("Alg.Alias.Signature.1.2.840.10045.4.3.4", "SHA512withECDSA");
    Put("Alg.Alias.Signature.2.16.840.1.101.3.4.2.3with1.2.840.10045.2.1", "SHA512withECDSA");

    /* === SecureRandom === */
    /*
     * We have to specify SHA1PRNG because various documentation mentions
     * that algorithm by name instead of just recommending calling
     * "new SecureRandom()"
     */
    Put("SecureRandom.SHA1PRNG", prefix + "COpenSSLRandom");
    Put("SecureRandom.SHA1PRNG ImplementedIn", "Software");

    /* === Cipher === */
    Put("Cipher.RSA/ECB/NoPadding", prefix + "COpenSSLCipherRSARaw");
    Put("Alg.Alias.Cipher.RSA/None/NoPadding", "RSA/ECB/NoPadding");
    Put("Cipher.RSA/ECB/PKCS1Padding", prefix + "COpenSSLCipherRSAPKCS1");
    Put("Alg.Alias.Cipher.RSA/None/PKCS1Padding", "RSA/ECB/PKCS1Padding");

    /*
     * OpenSSL only supports a subset of modes, so we'll name them
     * explicitly here.
     *
     * Moreover, OpenSSL only supports PKCS#7 padding. PKCS#5 padding
     * is also supported because it's a special case of PKCS#7 for 64-bit
     * blocks. PKCS#5 technically supports only 64-bit blocks and won't
     * produce the same result as PKCS#7 for blocks that are not 64 bits
     * long. However, everybody assumes PKCS#7 when they say PKCS#5. For
     * example, lots of code uses PKCS#5 with AES whose blocks are longer
     * than 64 bits. We solve this confusion by making PKCS7Padding an
     * alias for PKCS5Padding.
     */
    Put("Cipher.AES/ECB/NoPadding", prefix + "COpenSSLCipherAESECBNoPadding");
    Put("Cipher.AES/ECB/PKCS5Padding", prefix + "COpenSSLCipherAESECBPKCS5Padding");
    Put("Alg.Alias.Cipher.AES/ECB/PKCS7Padding", "AES/ECB/PKCS5Padding");
    Put("Cipher.AES/CBC/NoPadding", prefix + "COpenSSLCipherAESCBCNoPadding");
    Put("Cipher.AES/CBC/PKCS5Padding", prefix + "COpenSSLCipherAESCBCPKCS5Padding");
    Put("Alg.Alias.Cipher.AES/CBC/PKCS7Padding", "AES/CBC/PKCS5Padding");
    Put("Cipher.AES/CFB/NoPadding", prefix + "COpenSSLCipherAESCFB");
    Put("Cipher.AES/CTR/NoPadding", prefix + "COpenSSLCipherAESCTR");
    Put("Cipher.AES/OFB/NoPadding", prefix + "COpenSSLCipherAESOFB");

    Put("Cipher.DESEDE/ECB/NoPadding", prefix + "COpenSSLCipherDESEDEECBNoPadding");
    Put("Cipher.DESEDE/ECB/PKCS5Padding", prefix + "COpenSSLCipherDESEDEECBPKCS5Padding");
    Put("Alg.Alias.Cipher.DESEDE/ECB/PKCS7Padding", "DESEDE/ECB/PKCS5Padding");
    Put("Cipher.DESEDE/CBC/NoPadding", prefix + "COpenSSLCipherDESEDECBCNoPadding");
    Put("Cipher.DESEDE/CBC/PKCS5Padding", prefix + "COpenSSLCipherDESEDECBCPKCS5Padding");
    Put("Alg.Alias.Cipher.DESEDE/CBC/PKCS7Padding", "DESEDE/CBC/PKCS5Padding");
    Put("Cipher.DESEDE/CFB/NoPadding", prefix + "COpenSSLCipherDESEDECFB");
    Put("Cipher.DESEDE/OFB/NoPadding", prefix + "COpenSSLCipherDESEDEOFB");

    Put("Cipher.ARC4", prefix + "COpenSSLCipherARC4");

    /* === Mac === */

    Put("Mac.HmacMD5", prefix + "COpenSSLMacHmacMD5");

    // PKCS#2 - iso(1) member-body(2) US(840) rsadsi(113549) digestAlgorithm(2)
    // http://www.oid-info.com/get/1.2.840.113549.2

    // HMAC-SHA-1 PRF (7)
    Put("Mac.HmacSHA1", prefix + "COpenSSLMacHmacSHA1");
    Put("Alg.Alias.Mac.1.2.840.113549.2.7", "HmacSHA1");
    Put("Alg.Alias.Mac.HMAC-SHA1", "HmacSHA1");
    Put("Alg.Alias.Mac.HMAC/SHA1", "HmacSHA1");

    // id-hmacWithSHA224 (8)
    Put("Mac.HmacSHA224", prefix + "COpenSSLMacHmacSHA224");
    Put("Alg.Alias.Mac.1.2.840.113549.2.9", "HmacSHA224");
    Put("Alg.Alias.Mac.HMAC-SHA224", "HmacSHA224");
    Put("Alg.Alias.Mac.HMAC/SHA224", "HmacSHA224");

    // id-hmacWithSHA256 (9)
    Put("Mac.HmacSHA256", prefix + "COpenSSLMacHmacSHA256");
    Put("Alg.Alias.Mac.1.2.840.113549.2.9", "HmacSHA256");
    Put("Alg.Alias.Mac.HMAC-SHA256", "HmacSHA256");
    Put("Alg.Alias.Mac.HMAC/SHA256", "HmacSHA256");

    // id-hmacWithSHA384 (10)
    Put("Mac.HmacSHA384", prefix + "COpenSSLMacHmacSHA384");
    Put("Alg.Alias.Mac.1.2.840.113549.2.10", "HmacSHA384");
    Put("Alg.Alias.Mac.HMAC-SHA384", "HmacSHA384");
    Put("Alg.Alias.Mac.HMAC/SHA384", "HmacSHA384");

    // id-hmacWithSHA384 (11)
    Put("Mac.HmacSHA512", prefix + "COpenSSLMacHmacSHA512");
    Put("Alg.Alias.Mac.1.2.840.113549.2.11", "HmacSHA512");
    Put("Alg.Alias.Mac.HMAC-SHA512", "HmacSHA512");
    Put("Alg.Alias.Mac.HMAC/SHA512", "HmacSHA512");

    /* === Certificate === */

    Put("CertificateFactory.X509", prefix + "COpenSSLX509CertificateFactory");
    Put("Alg.Alias.CertificateFactory.X.509", "X509");
    Put("KeyGenerator.HMACMD5", prefix + "COpenSSLKeyGeneratorTest");

    //Logger::E("leliang", "TODO remove this COpenSSLProvider::constructor end");
    return NOERROR;
}

ECode COpenSSLProvider::Put(
    /* [in] */ const char* key,
    /* [in] */ const char* value)
{
    return Put(String(key), String(value));
}

ECode COpenSSLProvider::Put(
    /* [in] */ const char* key,
    /* [in] */ const String& value)
{
    return Put(String(key), value);
}

ECode COpenSSLProvider::Put(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    AutoPtr<ICharSequence> keyObj, valueObj;
    CString::New(key, (ICharSequence**)&keyObj);
    CString::New(value, (ICharSequence**)&valueObj);
    AutoPtr<IInterface> oldValueObj;
    return Provider::Put(keyObj, valueObj, (IInterface**)&oldValueObj);
}

} // namespace Conscrypt
} // namespace Org
