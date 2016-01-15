
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CNATIVECRYPTO_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CNATIVECRYPTO_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CNativeCrypto.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CNativeCrypto)
{
public:
    CARAPI ENGINE_load_dynamic();

    CARAPI ENGINE_by_id(
        /* [in] */ const String& id,
        /* [out] */ Int32 * pEngine);

    CARAPI ENGINE_add(
        /* [in] */ Int32 e,
        /* [out] */ Int32 * pEngine);

    CARAPI ENGINE_init(
        /* [in] */ Int32 e,
        /* [out] */ Int32 * pEngine);

    CARAPI ENGINE_finish(
        /* [in] */ Int32 e,
        /* [out] */ Int32 * pEngine);

    CARAPI ENGINE_free(
        /* [in] */ Int32 e,
        /* [out] */ Int32 * pEngine);

    CARAPI ENGINE_load_private_key(
        /* [in] */ Int32 e,
        /* [in] */ const String& key_id,
        /* [out] */ Int32 * pEngine);

    CARAPI EVP_PKEY_new_DSA(
        /* [in] */ ArrayOf<Byte> * pP,
        /* [in] */ ArrayOf<Byte> * pQ,
        /* [in] */ ArrayOf<Byte> * pG,
        /* [in] */ ArrayOf<Byte> * pPub_key,
        /* [in] */ ArrayOf<Byte> * pPriv_key,
        /* [out] */ Int32 * pEngine);

    CARAPI EVP_PKEY_new_RSA(
        /* [in] */ ArrayOf<Byte> * pN,
        /* [in] */ ArrayOf<Byte> * pE,
        /* [in] */ ArrayOf<Byte> * pD,
        /* [in] */ ArrayOf<Byte> * pP,
        /* [in] */ ArrayOf<Byte> * pQ,
        /* [in] */ ArrayOf<Byte> * pDmp1,
        /* [in] */ ArrayOf<Byte> * pDmq1,
        /* [in] */ ArrayOf<Byte> * pIqmp,
        /* [out] */ Int32 * pEngine);

    CARAPI EVP_PKEY_size(
        /* [in] */ Int32 pkey,
        /* [out] */ Int32 * pSize);

    CARAPI EVP_PKEY_type(
        /* [in] */ Int32 pkey,
        /* [out] */ Int32 * pType);

    CARAPI EVP_PKEY_free(
        /* [in] */ Int32 pkey);

    CARAPI EVP_PKEY_cmp(
        /* [in] */ Int32 pkey1,
        /* [in] */ Int32 pkey2,
        /* [out] */ Int32 * pResult);

    CARAPI I2d_PKCS8_PRIV_KEY_INFO(
        /* [in] */ Int32 pkey,
        /* [out, callee] */ ArrayOf<Byte> ** ppInfo);

    CARAPI D2i_PKCS8_PRIV_KEY_INFO(
        /* [in] */ ArrayOf<Byte> * pData,
        /* [out] */ Int32 * pInfo);

    CARAPI I2d_PUBKEY(
        /* [in] */ Int32 pkey,
        /* [out, callee] */ ArrayOf<Byte> ** ppPubKey);

    CARAPI D2I_PUBKEY(
        /* [in] */ ArrayOf<Byte> * pData,
        /* [out] */ Int32 * pPubKey);

    CARAPI RSA_generate_key_ex(
        /* [in] */ Int32 modulusBits,
        /* [in] */ ArrayOf<Byte> * pPublicExponent,
        /* [out] */ Int32 * pKey);

    CARAPI RSA_size(
        /* [in] */ Int32 pkey,
        /* [out] */ Int32 * pSize);

    CARAPI RSA_private_encrypt(
        /* [in] */ Int32 flen,
        /* [in] */ ArrayOf<Byte> * pFrom,
        /* [in] */ ArrayOf<Byte> * pTo,
        /* [in] */ Int32 pkey,
        /* [in] */ Int32 padding,
        /* [out] */ Int32 * pResult);

    CARAPI RSA_public_decrypt(
        /* [in] */ Int32 flen,
        /* [in] */ ArrayOf<Byte> * pFrom,
        /* [in] */ ArrayOf<Byte> * pTo,
        /* [in] */ Int32 pkey,
        /* [in] */ Int32 padding,
        /* [out] */ Int32 * pResult);

    CARAPI RSA_public_encrypt(
        /* [in] */ Int32 flen,
        /* [in] */ ArrayOf<Byte> * pFrom,
        /* [in] */ ArrayOf<Byte> * pTo,
        /* [in] */ Int32 pkey,
        /* [in] */ Int32 padding,
        /* [out] */ Int32 * pResult);

    CARAPI RSA_private_decrypt(
        /* [in] */ Int32 flen,
        /* [in] */ ArrayOf<Byte> * pFrom,
        /* [in] */ ArrayOf<Byte> * pTo,
        /* [in] */ Int32 pkey,
        /* [in] */ Int32 padding,
        /* [out] */ Int32 * pResult);

    CARAPI Get_RSA_public_params(
        /* [in] */ Int32 rsa,
        /* [out, callee] */ ArrayOf<Byte> ** ppParams);

    CARAPI Get_RSA_private_params(
        /* [in] */ Int32 rsa,
        /* [out, callee] */ ArrayOf<Byte> ** ppParams);

    CARAPI DSA_generate_key(
        /* [in] */ Int32 primeBits,
        /* [in] */ ArrayOf<Byte> * pSeed,
        /* [in] */ ArrayOf<Byte> * pG,
        /* [in] */ ArrayOf<Byte> * pP,
        /* [in] */ ArrayOf<Byte> * pQ,
        /* [out] */ Int32 * pKey);

    CARAPI Get_DSA_params(
        /* [in] */ Int32 dsa,
        /* [out, callee] */ ArrayOf<Byte> ** ppParams);

    CARAPI I2d_RSAPublicKey(
        /* [in] */ Int32 rsa,
        /* [out, callee] */ ArrayOf<Byte> ** ppParams);

    CARAPI I2d_RSAPrivateKey(
        /* [in] */ Int32 rsa,
        /* [out, callee] */ ArrayOf<Byte> ** ppKey);

    CARAPI I2d_DSAPublicKey(
        /* [in] */ Int32 dsa,
        /* [out, callee] */ ArrayOf<Byte> ** ppKey);

    CARAPI I2d_DSAPrivateKey(
        /* [in] */ Int32 dsa,
        /* [out, callee] */ ArrayOf<Byte> ** ppKey);

    CARAPI EVP_get_digestbyname(
        /* [in] */ const String& name,
        /* [out] */ Int32 * pDigest);

    CARAPI EVP_MD_size(
        /* [in] */ Int32 evp_md,
        /* [out] */ Int32 * pSize);

    CARAPI EVP_MD_block_size(
        /* [in] */ Int32 evp_md,
        /* [out] */ Int32 * pSize);

    CARAPI EVP_MD_CTX_destroy(
        /* [in] */ Int32 ctx);

    CARAPI EVP_MD_CTX_copy(
        /* [in] */ Int32 ctx,
        /* [out] */ Int32 * pResult);

    CARAPI EVP_DigestInit(
        /* [in] */ Int32 evp_md,
        /* [out] */ Int32 * pResult);

    CARAPI EVP_DigestUpdate(
        /* [in] */ Int32 ctx,
        /* [in] */ ArrayOf<Byte> * pBuffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    CARAPI EVP_DigestFinal(
        /* [in] */ Int32 ctx,
        /* [in] */ ArrayOf<Byte> * pHash,
        /* [in] */ Int32 offset,
        /* [out] */ Int32 * pDigestFinal);

    CARAPI EVP_SignInit(
        /* [in] */ const String& algorithm,
        /* [out] */ Int32 * pResult);

    CARAPI EVP_SignUpdate(
        /* [in] */ Int32 ctx,
        /* [in] */ ArrayOf<Byte> * pBuffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    CARAPI EVP_SignFinal(
        /* [in] */ Int32 ctx,
        /* [in] */ ArrayOf<Byte> * pSignature,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 key,
        /* [out] */ Int32 * pDigestFinal);

    CARAPI EVP_VerifyInit(
        /* [in] */ const String& algorithm,
        /* [out] */ Int32 * pInit);

    CARAPI EVP_VerifyUpdate(
        /* [in] */ Int32 ctx,
        /* [in] */ ArrayOf<Byte> * pBuffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    CARAPI EVP_VerifyFinal(
        /* [in] */ Int32 ctx,
        /* [in] */ ArrayOf<Byte> * pSignature,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [in] */ Int32 key,
        /* [out] */ Int32 * pDigestFinal);

    CARAPI EVP_get_cipherbyname(
        /* [in] */ const String& string,
        /* [out] */ Int32 * pCipher);

    CARAPI EVP_CipherInit_ex(
        /* [in] */ Int32 cipherNid,
        /* [in] */ ArrayOf<Byte> * pKey,
        /* [in] */ ArrayOf<Byte> * pIv,
        /* [in] */ Boolean encrypting,
        /* [out] */ Int32 * pResult);

    CARAPI EVP_CipherUpdate(
        /* [in] */ Int32 ctx,
        /* [in] */ ArrayOf<Byte> * pO,
        /* [in] */ Int32 outOffset,
        /* [in] */ ArrayOf<Byte> * pI,
        /* [in] */ Int32 inOffset,
        /* [out] */ Int32 * pResult);

    CARAPI EVP_CipherFinal_ex(
        /* [in] */ Int32 ctx,
        /* [in] */ ArrayOf<Byte> * pO,
        /* [in] */ Int32 outOffset,
        /* [out] */ Int32 * pDigestFinal);

    CARAPI EVP_CIPHER_CTX_cleanup(
        /* [in] */ Int32 ctx);

    CARAPI RAND_seed(
        /* [in] */ ArrayOf<Byte> * pSeed);

    CARAPI RAND_load_file(
        /* [in] */ const String& filename,
        /* [in] */ Int64 max_bytes,
        /* [out] */ Int32 * pResult);

    CARAPI RAND_bytes(
        /* [in] */ ArrayOf<Byte> * pOutput);

    CARAPI X509_NAME_hash(
        /* [in] */ Elastosx::Security::Auth::X500::IX500Principal * pPrincipal,
        /* [out] */ Int32 * pHash);

    CARAPI X509_NAME_hash_old(
        /* [in] */ Elastosx::Security::Auth::X500::IX500Principal * pPrincipal,
        /* [out] */ Int32 * pHash);

    CARAPI Get_OPENSSL_TO_STANDARD_CIPHER_SUITES(
        /* [out] */ Elastos::Utility::IMap ** ppSuites);

    CARAPI Get_STANDARD_TO_OPENSSL_CIPHER_SUITES(
        /* [out] */ Elastos::Utility::IMap ** ppSuites);

    CARAPI SSL_CTX_new(
        /* [out] */ Int32 * pCtx);

    CARAPI GetDefaultCipherSuites(
        /* [out, callee] */ ArrayOf<String> ** ppSuites);

    CARAPI GetSupportedCipherSuites(
        /* [out, callee] */ ArrayOf<String> ** ppSuites);

    CARAPI SSL_CTX_free(
        /* [in] */ Int32 ssl_ctx);

    CARAPI SSL_CTX_set_session_id_context(
        /* [in] */ Int32 ssl_ctx,
        /* [in] */ ArrayOf<Byte> * pSid_ctx);

    CARAPI SSL_new(
        /* [in] */ Int32 ssl_ctx,
        /* [out] */ Int32 * pSsl_new);

    CARAPI EncodeCertificates(
        /* [in] */ ArrayOf<Elastos::Security::Cert::ICertificate *> * pInCertificates,
        /* [out, callee] */ ArrayOf<Byte> ** ppOutCertificates);

    CARAPI SSL_use_certificate(
        /* [in] */ Int32 ssl,
        /* [in] */ ArrayOf<Byte> * pAsn1DerEncodedCertificateChain);

    CARAPI SSL_use_OpenSSL_PrivateKey(
        /* [in] */ Int32 ssl,
        /* [in] */ Int32 pkey);

    CARAPI SSL_use_PrivateKey(
        /* [in] */ Int32 ssl,
        /* [in] */ ArrayOf<Byte> * pPkcs8EncodedPrivateKey);

    CARAPI SSL_check_private_key(
        /* [in] */ Int32 ssl);

    CARAPI EncodeIssuerX509Principals(
        /* [in] */ ArrayOf<Elastos::Security::Cert::IX509Certificate *> * pCertificates,
        /* [out, callee] */ ArrayOf<Byte> ** ppPrincipals);

    CARAPI SSL_set_client_CA_list(
        /* [in] */ Int32 ssl,
        /* [in] */ ArrayOf<Byte> * pAsn1DerEncodedX500Principals);

    CARAPI SSL_get_mode(
        /* [in] */ Int32 ssl,
        /* [out] */ Int64 * pMode);

    CARAPI SSL_set_mode(
        /* [in] */ Int32 ssl,
        /* [in] */ Int64 inMode,
        /* [out] */ Int64 * pOutMode);

    CARAPI SSL_clear_mode(
        /* [in] */ Int32 ssl,
        /* [in] */ Int64 inMode,
        /* [out] */ Int64 * pOutMode);

    CARAPI SSL_get_options(
        /* [in] */ Int32 ssl,
        /* [out] */ Int64 * pOptions);

    CARAPI SSL_set_options(
        /* [in] */ Int32 ssl,
        /* [in] */ Int64 options,
        /* [out] */ Int64 * pRetOptions);

    CARAPI SSL_clear_options(
        /* [in] */ Int32 ssl,
        /* [in] */ Int64 options,
        /* [out] */ Int64 * pRetOptions);

    CARAPI GetDefaultProtocols(
        /* [out, callee] */ ArrayOf<String> ** ppProtocols);

    CARAPI GetSupportedProtocols(
        /* [out, callee] */ ArrayOf<String> ** ppProtocols);

    CARAPI SetEnabledProtocols(
        /* [in] */ Int32 ssl,
        /* [in] */ ArrayOf<String> * pProtocols);

    CARAPI CheckEnabledProtocols(
        /* [in] */ ArrayOf<String> * pProtocols,
        /* [out, callee] */ ArrayOf<String> ** ppRetProtocols);

    CARAPI SSL_set_cipher_lists(
        /* [in] */ Int32 ssl,
        /* [in] */ ArrayOf<String> * pCiphers);

    CARAPI SetEnabledCipherSuites(
        /* [in] */ Int32 ssl,
        /* [in] */ ArrayOf<String> * pCipherSuites);

    CARAPI CheckEnabledCipherSuites(
        /* [in] */ ArrayOf<String> * pCipherSuites,
        /* [out, callee] */ ArrayOf<String> ** ppSuites);

    CARAPI SSL_set_verify(
        /* [in] */ Int32 sslNativePointer,
        /* [in] */ Int32 mode);

    CARAPI SSL_set_session(
        /* [in] */ Int32 sslNativePointer,
        /* [in] */ Int32 sslSessionNativePointer);

    CARAPI SSL_set_session_creation_enabled(
        /* [in] */ Int32 sslNativePointer,
        /* [in] */ Boolean creationEnabled);

    CARAPI SSL_set_tlsext_host_name(
        /* [in] */ Int32 sslNativePointer,
        /* [in] */ const String& hostname);

    CARAPI SSL_get_servername(
        /* [in] */ Int32 sslNativePointer,
        /* [out] */ String * pServerName);

    CARAPI SSL_CTX_enable_npn(
        /* [in] */ Int32 sslCtxNativePointer);

    CARAPI SSL_CTX_disable_npn(
        /* [in] */ Int32 sslCtxNativePointer);

    CARAPI SSL_do_handshake(
        /* [in] */ Int32 sslNativePointer,
        /* [in] */ Elastos::IO::IFileDescriptor * pFd,
        /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ISSLHandshakeCallbacks * pShc,
        /* [in] */ Int32 timeoutMillis,
        /* [in] */ Boolean client_mode,
        /* [in] */ ArrayOf<Byte> * pNpnProtocols,
        /* [out] */ Int32 * pResult);

    CARAPI SSL_get_npn_negotiated_protocol(
        /* [in] */ Int32 sslNativePointer,
        /* [out, callee] */ ArrayOf<Byte> ** ppProtocol);

    CARAPI SSL_renegotiate(
        /* [in] */ Int32 sslNativePointer);

    CARAPI SSL_get_certificate(
        /* [in] */ Int32 sslNativePointer,
        /* [out, callee] */ ArrayOf<Byte> ** ppCertificate);

    CARAPI SSL_get_peer_cert_chain(
        /* [in] */ Int32 sslNativePointer,
        /* [out, callee] */ ArrayOf<Byte> ** ppCert_chain);

    CARAPI SSL_read(
        /* [in] */ Int32 sslNativePointer,
        /* [in] */ Elastos::IO::IFileDescriptor * pFd,
        /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ISSLHandshakeCallbacks * pShc,
        /* [in] */ ArrayOf<Byte> * pB,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len,
        /* [in] */ Int32 readTimeoutMillis,
        /* [out] */ Int32 * pResult);

    CARAPI SSL_write(
        /* [in] */ Int32 sslNativePointer,
        /* [in] */ Elastos::IO::IFileDescriptor * pFd,
        /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ISSLHandshakeCallbacks * pShc,
        /* [in] */ ArrayOf<Byte> * pB,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len,
        /* [in] */ Int32 writeTimeoutMillis);

    CARAPI SSL_interrupt(
        /* [in] */ Int32 sslNativePointer);

    CARAPI SSL_shutdown(
        /* [in] */ Int32 sslNativePointer,
        /* [in] */ Elastos::IO::IFileDescriptor * pFd,
        /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ISSLHandshakeCallbacks * pShc);

    CARAPI SSL_free(
        /* [in] */ Int32 sslNativePointer);

    CARAPI SSL_SESSION_session_id(
        /* [in] */ Int32 sslSessionNativePointer,
        /* [out, callee] */ ArrayOf<Byte> ** ppSession_id);

    CARAPI SSL_SESSION_get_time(
        /* [in] */ Int32 sslSessionNativePointer,
        /* [out] */ Int64 * pTime);

    CARAPI SSL_SESSION_get_version(
        /* [in] */ Int32 sslSessionNativePointer,
        /* [out] */ String * pVer);

    CARAPI SSL_SESSION_cipher(
        /* [in] */ Int32 sslSessionNativePointer,
        /* [out] */ String * pCipher);

    CARAPI SSL_SESSION_free(
        /* [in] */ Int32 sslSessionNativePointer);

    CARAPI I2d_SSL_SESSION(
        /* [in] */ Int32 sslSessionNativePointer,
        /* [out, callee] */ ArrayOf<Byte> ** ppSession);

    CARAPI D2i_SSL_SESSION(
        /* [in] */ ArrayOf<Byte> * pData,
        /* [out] */ Int32 * pSession);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CNATIVECRYPTO_H__
