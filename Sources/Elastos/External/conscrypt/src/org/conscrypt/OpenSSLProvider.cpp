
#include "OpenSSLProvider.h"
#include "Platform.h"

#include <elastos/core/CoreUtils.h>

using Elastos::Core::CoreUtils;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLProvider::
//=========================================

Int64 OpenSSLProvider::mSerialVersionUID = 2996752495318905136L;

String OpenSSLProvider::PROVIDER_NAME("AndroidOpenSSL");

OpenSSLProvider::OpenSSLProvider()
{
    // Provider(PROVIDER_NAME);
}

OpenSSLProvider::OpenSSLProvider(
    /* [in] */ String providerName)
{
    Provider::constructor(providerName, 1.0, String("Android's OpenSSL-backed security provider"));

    // Make sure the platform is initialized.
    Platform::Setup();

    String prefix("OpenSSLProvider."); // = GetClass().GetPackage().GetName() + String(".");

    /* === SSL Contexts === */
    String classOpenSSLContextImpl = prefix + String("OpenSSLContextImpl");
    AutoPtr<IInterface> res;
    Put(CoreUtils::Convert("SSLContext.SSL"), CoreUtils::Convert(classOpenSSLContextImpl), (IInterface**)&res);
    Put(CoreUtils::Convert("SSLContext.SSLv3"), CoreUtils::Convert(classOpenSSLContextImpl), (IInterface**)&res);
    Put(CoreUtils::Convert("SSLContext.TLS"), CoreUtils::Convert(classOpenSSLContextImpl), (IInterface**)&res);
    Put(CoreUtils::Convert("SSLContext.TLSv1"), CoreUtils::Convert(classOpenSSLContextImpl), (IInterface**)&res);
    Put(CoreUtils::Convert("SSLContext.TLSv1.1"), CoreUtils::Convert(classOpenSSLContextImpl), (IInterface**)&res);
    Put(CoreUtils::Convert("SSLContext.TLSv1.2"), CoreUtils::Convert(classOpenSSLContextImpl), (IInterface**)&res);
    Put(CoreUtils::Convert("SSLContext.Default"), CoreUtils::Convert(prefix + "DefaultSSLContextImpl"), (IInterface**)&res);

    /* === Message Digests === */
    Put(CoreUtils::Convert("MessageDigest.SHA-1"), CoreUtils::Convert(prefix + "OpenSSLMessageDigestJDK$SHA1"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.MessageDigest.SHA1"), CoreUtils::Convert("SHA-1"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.MessageDigest.SHA"), CoreUtils::Convert("SHA-1"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.MessageDigest.1.3.14.3.2.26"), CoreUtils::Convert("SHA-1"), (IInterface**)&res);

    Put(CoreUtils::Convert("MessageDigest.SHA-224"), CoreUtils::Convert(prefix + "OpenSSLMessageDigestJDK$SHA224"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.MessageDigest.SHA224"), CoreUtils::Convert("SHA-224"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.MessageDigest.2.16.840.1.101.3.4.2.4"), CoreUtils::Convert("SHA-224"), (IInterface**)&res);

    Put(CoreUtils::Convert("MessageDigest.SHA-256"), CoreUtils::Convert(prefix + "OpenSSLMessageDigestJDK$SHA256"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.MessageDigest.SHA256"), CoreUtils::Convert("SHA-256"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.MessageDigest.2.16.840.1.101.3.4.2.1"), CoreUtils::Convert("SHA-256"), (IInterface**)&res);

    Put(CoreUtils::Convert("MessageDigest.SHA-384"), CoreUtils::Convert(prefix + "OpenSSLMessageDigestJDK$SHA384"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.MessageDigest.SHA384"), CoreUtils::Convert("SHA-384"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.MessageDigest.2.16.840.1.101.3.4.2.2"), CoreUtils::Convert("SHA-384"), (IInterface**)&res);

    Put(CoreUtils::Convert("MessageDigest.SHA-512"), CoreUtils::Convert(prefix + "OpenSSLMessageDigestJDK$SHA512"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.MessageDigest.SHA512"), CoreUtils::Convert("SHA-512"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.MessageDigest.2.16.840.1.101.3.4.2.3"), CoreUtils::Convert("SHA-512"), (IInterface**)&res);

    // iso(1) member-body(2) US(840) rsadsi(113549) digestAlgorithm(2) md5(5)
    Put(CoreUtils::Convert("MessageDigest.MD5"), CoreUtils::Convert(prefix + "OpenSSLMessageDigestJDK$MD5"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.MessageDigest.1.2.840.113549.2.5"), CoreUtils::Convert("MD5"), (IInterface**)&res);

    /* == KeyPairGenerators == */
    Put(CoreUtils::Convert("KeyPairGenerator.RSA"), CoreUtils::Convert(prefix + "OpenSSLRSAKeyPairGenerator"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.KeyPairGenerator.1.2.840.113549.1.1.1"), CoreUtils::Convert("RSA"), (IInterface**)&res);

    Put(CoreUtils::Convert("KeyPairGenerator.DH"), CoreUtils::Convert(prefix + "OpenSSLDHKeyPairGenerator"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.KeyPairGenerator.1.2.840.113549.1.3.1"), CoreUtils::Convert("DH"), (IInterface**)&res);

    Put(CoreUtils::Convert("KeyPairGenerator.DSA"), CoreUtils::Convert(prefix + "OpenSSLDSAKeyPairGenerator"), (IInterface**)&res);

    Put(CoreUtils::Convert("KeyPairGenerator.EC"), CoreUtils::Convert(prefix + "OpenSSLECKeyPairGenerator"), (IInterface**)&res);

    /* == KeyFactory == */
    Put(CoreUtils::Convert("KeyFactory.RSA"), CoreUtils::Convert(prefix + "OpenSSLRSAKeyFactory"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.KeyFactory.1.2.840.113549.1.1.1"), CoreUtils::Convert("RSA"), (IInterface**)&res);

    Put(CoreUtils::Convert("KeyFactory.DH"), CoreUtils::Convert(prefix + "OpenSSLDHKeyFactory"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.KeyFactory.1.2.840.113549.1.3.1"), CoreUtils::Convert("DH"), (IInterface**)&res);

    Put(CoreUtils::Convert("KeyFactory.DSA"), CoreUtils::Convert(prefix + "OpenSSLDSAKeyFactory"), (IInterface**)&res);

    Put(CoreUtils::Convert("KeyFactory.EC"), CoreUtils::Convert(prefix + "OpenSSLECKeyFactory"), (IInterface**)&res);

    /* == KeyAgreement == */
    Put(CoreUtils::Convert("KeyAgreement.ECDH"), CoreUtils::Convert(prefix + "OpenSSLECDHKeyAgreement"), (IInterface**)&res);

    /* == Signatures == */
    Put(CoreUtils::Convert("Signature.MD5WithRSA"), CoreUtils::Convert(prefix + "OpenSSLSignature$MD5RSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.MD5WithRSAEncryption"), CoreUtils::Convert("MD5WithRSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.MD5/RSA"), CoreUtils::Convert("MD5WithRSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.1.2.840.113549.1.1.4"), CoreUtils::Convert("MD5WithRSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.1.2.840.113549.2.5with1.2.840.113549.1.1.1"), CoreUtils::Convert("MD5WithRSA"), (IInterface**)&res);

    Put(CoreUtils::Convert("Signature.SHA1WithRSA"), CoreUtils::Convert(prefix + "OpenSSLSignature$SHA1RSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.SHA1WithRSAEncryption"), CoreUtils::Convert("SHA1WithRSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.SHA1/RSA"), CoreUtils::Convert("SHA1WithRSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.SHA-1/RSA"), CoreUtils::Convert("SHA1WithRSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.1.2.840.113549.1.1.5"), CoreUtils::Convert("SHA1WithRSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.1.3.14.3.2.26with1.2.840.113549.1.1.1"), CoreUtils::Convert("SHA1WithRSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.1.3.14.3.2.26with1.2.840.113549.1.1.5"), CoreUtils::Convert("SHA1WithRSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.1.3.14.3.2.29"), CoreUtils::Convert("SHA1WithRSA"), (IInterface**)&res);

    Put(CoreUtils::Convert("Signature.SHA224WithRSA"), CoreUtils::Convert(prefix + "OpenSSLSignature$SHA224RSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.SHA224WithRSAEncryption"), CoreUtils::Convert("SHA224WithRSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.1.2.840.113549.1.1.11"), CoreUtils::Convert("SHA224WithRSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.2.16.840.1.101.3.4.2.4with1.2.840.113549.1.1.1"),
            CoreUtils::Convert("SHA224WithRSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.2.16.840.1.101.3.4.2.4with1.2.840.113549.1.1.11"),
            CoreUtils::Convert("SHA224WithRSA"), (IInterface**)&res);

    Put(CoreUtils::Convert("Signature.SHA256WithRSA"), CoreUtils::Convert(prefix + "OpenSSLSignature$SHA256RSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.SHA256WithRSAEncryption"), CoreUtils::Convert("SHA256WithRSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.1.2.840.113549.1.1.11"), CoreUtils::Convert("SHA256WithRSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.2.16.840.1.101.3.4.2.1with1.2.840.113549.1.1.1"),
            CoreUtils::Convert("SHA256WithRSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.2.16.840.1.101.3.4.2.1with1.2.840.113549.1.1.11"),
            CoreUtils::Convert("SHA256WithRSA"), (IInterface**)&res);

    Put(CoreUtils::Convert("Signature.SHA384WithRSA"), CoreUtils::Convert(prefix + "OpenSSLSignature$SHA384RSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.SHA384WithRSAEncryption"), CoreUtils::Convert("SHA384WithRSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.1.2.840.113549.1.1.12"), CoreUtils::Convert("SHA384WithRSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.2.16.840.1.101.3.4.2.2with1.2.840.113549.1.1.1"),
            CoreUtils::Convert("SHA384WithRSA"), (IInterface**)&res);

    Put(CoreUtils::Convert("Signature.SHA512WithRSA"), CoreUtils::Convert(prefix + "OpenSSLSignature$SHA512RSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.SHA512WithRSAEncryption"), CoreUtils::Convert("SHA512WithRSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.1.2.840.113549.1.1.13"), CoreUtils::Convert("SHA512WithRSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.2.16.840.1.101.3.4.2.3with1.2.840.113549.1.1.1"),
            CoreUtils::Convert("SHA512WithRSA"), (IInterface**)&res);

    Put(CoreUtils::Convert("Signature.SHA1withDSA"), CoreUtils::Convert(prefix + "OpenSSLSignature$SHA1DSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.SHA/DSA"), CoreUtils::Convert("SHA1withDSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.DSA"), CoreUtils::Convert("SHA1withDSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.1.3.14.3.2.26with1.2.840.10040.4.1"), CoreUtils::Convert("SHA1withDSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.1.3.14.3.2.26with1.2.840.10040.4.3"), CoreUtils::Convert("SHA1withDSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.DSAWithSHA1"), CoreUtils::Convert("SHA1withDSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.1.2.840.10040.4.3"), CoreUtils::Convert("SHA1withDSA"), (IInterface**)&res);

    Put(CoreUtils::Convert("Signature.NONEwithRSA"), CoreUtils::Convert(prefix + "OpenSSLSignatureRawRSA"), (IInterface**)&res);

    Put(CoreUtils::Convert("Signature.ECDSA"), CoreUtils::Convert(prefix + "OpenSSLSignature$SHA1ECDSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.SHA1withECDSA"), CoreUtils::Convert("ECDSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.ECDSAwithSHA1"), CoreUtils::Convert("ECDSA"), (IInterface**)&res);
    // iso(1) member-body(2) us(840) ansi-x962(10045) signatures(4) ecdsa-with-SHA1(1)
    Put(CoreUtils::Convert("Alg.Alias.Signature.1.2.840.10045.4.1"), CoreUtils::Convert("ECDSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.1.3.14.3.2.26with1.2.840.10045.2.1"), CoreUtils::Convert("ECDSA"), (IInterface**)&res);

    // iso(1) member-body(2) us(840) ansi-x962(10045) signatures(4) ecdsa-with-SHA2(3)
    Put(CoreUtils::Convert("Signature.SHA224withECDSA"), CoreUtils::Convert(prefix + "OpenSSLSignature$SHA224ECDSA"), (IInterface**)&res);
    // ecdsa-with-SHA224(1)
    Put(CoreUtils::Convert("Alg.Alias.Signature.1.2.840.10045.4.3.1"), CoreUtils::Convert("SHA224withECDSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.2.16.840.1.101.3.4.2.4with1.2.840.10045.2.1"), CoreUtils::Convert("SHA224withECDSA"), (IInterface**)&res);

    // iso(1) member-body(2) us(840) ansi-x962(10045) signatures(4) ecdsa-with-SHA2(3)
    Put(CoreUtils::Convert("Signature.SHA256withECDSA"), CoreUtils::Convert(prefix + "OpenSSLSignature$SHA256ECDSA"), (IInterface**)&res);
    // ecdsa-with-SHA256(2)
    Put(CoreUtils::Convert("Alg.Alias.Signature.1.2.840.10045.4.3.2"), CoreUtils::Convert("SHA256withECDSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.2.16.840.1.101.3.4.2.1with1.2.840.10045.2.1"), CoreUtils::Convert("SHA256withECDSA"), (IInterface**)&res);

    Put(CoreUtils::Convert("Signature.SHA384withECDSA"), CoreUtils::Convert(prefix + "OpenSSLSignature$SHA384ECDSA"), (IInterface**)&res);
    // ecdsa-with-SHA384(3)
    Put(CoreUtils::Convert("Alg.Alias.Signature.1.2.840.10045.4.3.3"), CoreUtils::Convert("SHA384withECDSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.2.16.840.1.101.3.4.2.2with1.2.840.10045.2.1"), CoreUtils::Convert("SHA384withECDSA"), (IInterface**)&res);

    Put(CoreUtils::Convert("Signature.SHA512withECDSA"), CoreUtils::Convert(prefix + "OpenSSLSignature$SHA512ECDSA"), (IInterface**)&res);
    // ecdsa-with-SHA512(4)
    Put(CoreUtils::Convert("Alg.Alias.Signature.1.2.840.10045.4.3.4"), CoreUtils::Convert("SHA512withECDSA"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Signature.2.16.840.1.101.3.4.2.3with1.2.840.10045.2.1"), CoreUtils::Convert("SHA512withECDSA"), (IInterface**)&res);

    /* === SecureRandom === */
    /*
     * We have to specify SHA1PRNG because various documentation mentions
     * that algorithm by name instead of just recommending calling
     * "new SecureRandom()"
     */
    Put(CoreUtils::Convert("SecureRandom.SHA1PRNG"), CoreUtils::Convert(prefix + "OpenSSLRandom"), (IInterface**)&res);
    Put(CoreUtils::Convert("SecureRandom.SHA1PRNG ImplementedIn"), CoreUtils::Convert("Software"), (IInterface**)&res);

    /* === Cipher === */
    Put(CoreUtils::Convert("Cipher.RSA/ECB/NoPadding"), CoreUtils::Convert(prefix + "OpenSSLCipherRSA$Raw"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Cipher.RSA/None/NoPadding"), CoreUtils::Convert("RSA/ECB/NoPadding"), (IInterface**)&res);
    Put(CoreUtils::Convert("Cipher.RSA/ECB/PKCS1Padding"), CoreUtils::Convert(prefix + "OpenSSLCipherRSA$PKCS1"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Cipher.RSA/None/PKCS1Padding"), CoreUtils::Convert("RSA/ECB/PKCS1Padding"), (IInterface**)&res);

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
    Put(CoreUtils::Convert("Cipher.AES/ECB/NoPadding"), CoreUtils::Convert(prefix + "OpenSSLCipher$AES$ECB$NoPadding"), (IInterface**)&res);
    Put(CoreUtils::Convert("Cipher.AES/ECB/PKCS5Padding"), CoreUtils::Convert(prefix + "OpenSSLCipher$AES$ECB$PKCS5Padding"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Cipher.AES/ECB/PKCS7Padding"), CoreUtils::Convert("AES/ECB/PKCS5Padding"), (IInterface**)&res);
    Put(CoreUtils::Convert("Cipher.AES/CBC/NoPadding"), CoreUtils::Convert(prefix + "OpenSSLCipher$AES$CBC$NoPadding"), (IInterface**)&res);
    Put(CoreUtils::Convert("Cipher.AES/CBC/PKCS5Padding"), CoreUtils::Convert(prefix + "OpenSSLCipher$AES$CBC$PKCS5Padding"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Cipher.AES/CBC/PKCS7Padding"), CoreUtils::Convert("AES/CBC/PKCS5Padding"), (IInterface**)&res);
    Put(CoreUtils::Convert("Cipher.AES/CFB/NoPadding"), CoreUtils::Convert(prefix + "OpenSSLCipher$AES$CFB"), (IInterface**)&res);
    Put(CoreUtils::Convert("Cipher.AES/CTR/NoPadding"), CoreUtils::Convert(prefix + "OpenSSLCipher$AES$CTR"), (IInterface**)&res);
    Put(CoreUtils::Convert("Cipher.AES/OFB/NoPadding"), CoreUtils::Convert(prefix + "OpenSSLCipher$AES$OFB"), (IInterface**)&res);

    Put(CoreUtils::Convert("Cipher.DESEDE/ECB/NoPadding"), CoreUtils::Convert(prefix + "OpenSSLCipher$DESEDE$ECB$NoPadding"), (IInterface**)&res);
    Put(CoreUtils::Convert("Cipher.DESEDE/ECB/PKCS5Padding"), CoreUtils::Convert(prefix + "OpenSSLCipher$DESEDE$ECB$PKCS5Padding"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Cipher.DESEDE/ECB/PKCS7Padding"), CoreUtils::Convert("DESEDE/ECB/PKCS5Padding"), (IInterface**)&res);
    Put(CoreUtils::Convert("Cipher.DESEDE/CBC/NoPadding"), CoreUtils::Convert(prefix + "OpenSSLCipher$DESEDE$CBC$NoPadding"), (IInterface**)&res);
    Put(CoreUtils::Convert("Cipher.DESEDE/CBC/PKCS5Padding"), CoreUtils::Convert(prefix + "OpenSSLCipher$DESEDE$CBC$PKCS5Padding"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Cipher.DESEDE/CBC/PKCS7Padding"), CoreUtils::Convert("DESEDE/CBC/PKCS5Padding"), (IInterface**)&res);
    Put(CoreUtils::Convert("Cipher.DESEDE/CFB/NoPadding"), CoreUtils::Convert(prefix + "OpenSSLCipher$DESEDE$CFB"), (IInterface**)&res);
    Put(CoreUtils::Convert("Cipher.DESEDE/OFB/NoPadding"), CoreUtils::Convert(prefix + "OpenSSLCipher$DESEDE$OFB"), (IInterface**)&res);

    Put(CoreUtils::Convert("Cipher.ARC4"), CoreUtils::Convert(prefix + "OpenSSLCipher$ARC4"), (IInterface**)&res);

    /* === Mac === */

    Put(CoreUtils::Convert("Mac.HmacMD5"), CoreUtils::Convert(prefix + "OpenSSLMac$HmacMD5"), (IInterface**)&res);

    // PKCS#2 - iso(1) member-body(2) US(840) rsadsi(113549) digestAlgorithm(2)
    // http://www.oid-info.com/get/1.2.840.113549.2

    // HMAC-SHA-1 PRF (7)
    Put(CoreUtils::Convert("Mac.HmacSHA1"), CoreUtils::Convert(prefix + "OpenSSLMac$HmacSHA1"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Mac.1.2.840.113549.2.7"), CoreUtils::Convert("HmacSHA1"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Mac.HMAC-SHA1"), CoreUtils::Convert("HmacSHA1"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Mac.HMAC/SHA1"), CoreUtils::Convert("HmacSHA1"), (IInterface**)&res);

    // id-hmacWithSHA224 (8)
    Put(CoreUtils::Convert("Mac.HmacSHA224"), CoreUtils::Convert(prefix + "OpenSSLMac$HmacSHA224"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Mac.1.2.840.113549.2.9"), CoreUtils::Convert("HmacSHA224"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Mac.HMAC-SHA224"), CoreUtils::Convert("HmacSHA224"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Mac.HMAC/SHA224"), CoreUtils::Convert("HmacSHA224"), (IInterface**)&res);

    // id-hmacWithSHA256 (9)
    Put(CoreUtils::Convert("Mac.HmacSHA256"), CoreUtils::Convert(prefix + "OpenSSLMac$HmacSHA256"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Mac.1.2.840.113549.2.9"), CoreUtils::Convert("HmacSHA256"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Mac.HMAC-SHA256"), CoreUtils::Convert("HmacSHA256"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Mac.HMAC/SHA256"), CoreUtils::Convert("HmacSHA256"), (IInterface**)&res);

    // id-hmacWithSHA384 (10)
    Put(CoreUtils::Convert("Mac.HmacSHA384"), CoreUtils::Convert(prefix + "OpenSSLMac$HmacSHA384"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Mac.1.2.840.113549.2.10"), CoreUtils::Convert("HmacSHA384"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Mac.HMAC-SHA384"), CoreUtils::Convert("HmacSHA384"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Mac.HMAC/SHA384"), CoreUtils::Convert("HmacSHA384"), (IInterface**)&res);

    // id-hmacWithSHA384 (11)
    Put(CoreUtils::Convert("Mac.HmacSHA512"), CoreUtils::Convert(prefix + "OpenSSLMac$HmacSHA512"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Mac.1.2.840.113549.2.11"), CoreUtils::Convert("HmacSHA512"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Mac.HMAC-SHA512"), CoreUtils::Convert("HmacSHA512"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.Mac.HMAC/SHA512"), CoreUtils::Convert("HmacSHA512"), (IInterface**)&res);

    /* === Certificate === */

    Put(CoreUtils::Convert("CertificateFactory.X509"), CoreUtils::Convert(prefix + "OpenSSLX509CertificateFactory"), (IInterface**)&res);
    Put(CoreUtils::Convert("Alg.Alias.CertificateFactory.X.509"), CoreUtils::Convert("X509"), (IInterface**)&res);
}

} // namespace Conscrypt
} // namespace Org
