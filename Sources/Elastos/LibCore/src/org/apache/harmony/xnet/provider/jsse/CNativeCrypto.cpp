
#include "CNativeCrypto.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(CNativeCrypto)

CAR_INTERFACE_IMPL(CNativeCrypto, Object, INativeCrypto)

ECode CNativeCrypto::ENGINE_load_dynamic()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::ENGINE_by_id(
    /* [in] */ const String& id,
    /* [out] */ Int32 * pEngine)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::ENGINE_add(
    /* [in] */ Int32 e,
    /* [out] */ Int32 * pEngine)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::ENGINE_init(
    /* [in] */ Int32 e,
    /* [out] */ Int32 * pEngine)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::ENGINE_finish(
    /* [in] */ Int32 e,
    /* [out] */ Int32 * pEngine)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::ENGINE_free(
    /* [in] */ Int32 e,
    /* [out] */ Int32 * pEngine)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::ENGINE_load_private_key(
    /* [in] */ Int32 e,
    /* [in] */ const String& key_id,
    /* [out] */ Int32 * pEngine)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_PKEY_new_DSA(
    /* [in] */ ArrayOf<Byte> * pP,
    /* [in] */ ArrayOf<Byte> * pQ,
    /* [in] */ ArrayOf<Byte> * pG,
    /* [in] */ ArrayOf<Byte> * pPub_key,
    /* [in] */ ArrayOf<Byte> * pPriv_key,
    /* [out] */ Int32 * pEngine)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_PKEY_new_RSA(
    /* [in] */ ArrayOf<Byte> * pN,
    /* [in] */ ArrayOf<Byte> * pE,
    /* [in] */ ArrayOf<Byte> * pD,
    /* [in] */ ArrayOf<Byte> * pP,
    /* [in] */ ArrayOf<Byte> * pQ,
    /* [in] */ ArrayOf<Byte> * pDmp1,
    /* [in] */ ArrayOf<Byte> * pDmq1,
    /* [in] */ ArrayOf<Byte> * pIqmp,
    /* [out] */ Int32 * pEngine)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_PKEY_size(
    /* [in] */ Int32 pkey,
    /* [out] */ Int32 * pSize)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_PKEY_type(
    /* [in] */ Int32 pkey,
    /* [out] */ Int32 * pType)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_PKEY_free(
    /* [in] */ Int32 pkey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_PKEY_cmp(
    /* [in] */ Int32 pkey1,
    /* [in] */ Int32 pkey2,
    /* [out] */ Int32 * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::I2d_PKCS8_PRIV_KEY_INFO(
    /* [in] */ Int32 pkey,
    /* [out, callee] */ ArrayOf<Byte> ** ppInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::D2i_PKCS8_PRIV_KEY_INFO(
    /* [in] */ ArrayOf<Byte> * pData,
    /* [out] */ Int32 * pInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::I2d_PUBKEY(
    /* [in] */ Int32 pkey,
    /* [out, callee] */ ArrayOf<Byte> ** ppPubKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::D2I_PUBKEY(
    /* [in] */ ArrayOf<Byte> * pData,
    /* [out] */ Int32 * pPubKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::RSA_generate_key_ex(
    /* [in] */ Int32 modulusBits,
    /* [in] */ ArrayOf<Byte> * pPublicExponent,
    /* [out] */ Int32 * pKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::RSA_size(
    /* [in] */ Int32 pkey,
    /* [out] */ Int32 * pSize)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::RSA_private_encrypt(
    /* [in] */ Int32 flen,
    /* [in] */ ArrayOf<Byte> * pFrom,
    /* [in] */ ArrayOf<Byte> * pTo,
    /* [in] */ Int32 pkey,
    /* [in] */ Int32 padding,
    /* [out] */ Int32 * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::RSA_public_decrypt(
    /* [in] */ Int32 flen,
    /* [in] */ ArrayOf<Byte> * pFrom,
    /* [in] */ ArrayOf<Byte> * pTo,
    /* [in] */ Int32 pkey,
    /* [in] */ Int32 padding,
    /* [out] */ Int32 * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::RSA_public_encrypt(
    /* [in] */ Int32 flen,
    /* [in] */ ArrayOf<Byte> * pFrom,
    /* [in] */ ArrayOf<Byte> * pTo,
    /* [in] */ Int32 pkey,
    /* [in] */ Int32 padding,
    /* [out] */ Int32 * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::RSA_private_decrypt(
    /* [in] */ Int32 flen,
    /* [in] */ ArrayOf<Byte> * pFrom,
    /* [in] */ ArrayOf<Byte> * pTo,
    /* [in] */ Int32 pkey,
    /* [in] */ Int32 padding,
    /* [out] */ Int32 * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::Get_RSA_public_params(
    /* [in] */ Int32 rsa,
    /* [out, callee] */ ArrayOf<Byte> ** ppParams)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::Get_RSA_private_params(
    /* [in] */ Int32 rsa,
    /* [out, callee] */ ArrayOf<Byte> ** ppParams)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::DSA_generate_key(
    /* [in] */ Int32 primeBits,
    /* [in] */ ArrayOf<Byte> * pSeed,
    /* [in] */ ArrayOf<Byte> * pG,
    /* [in] */ ArrayOf<Byte> * pP,
    /* [in] */ ArrayOf<Byte> * pQ,
    /* [out] */ Int32 * pKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::Get_DSA_params(
    /* [in] */ Int32 dsa,
    /* [out, callee] */ ArrayOf<Byte> ** ppParams)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::I2d_RSAPublicKey(
    /* [in] */ Int32 rsa,
    /* [out, callee] */ ArrayOf<Byte> ** ppParams)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::I2d_RSAPrivateKey(
    /* [in] */ Int32 rsa,
    /* [out, callee] */ ArrayOf<Byte> ** ppKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::I2d_DSAPublicKey(
    /* [in] */ Int32 dsa,
    /* [out, callee] */ ArrayOf<Byte> ** ppKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::I2d_DSAPrivateKey(
    /* [in] */ Int32 dsa,
    /* [out, callee] */ ArrayOf<Byte> ** ppKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_get_digestbyname(
    /* [in] */ const String& name,
    /* [out] */ Int32 * pDigest)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_MD_size(
    /* [in] */ Int32 evp_md,
    /* [out] */ Int32 * pSize)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_MD_block_size(
    /* [in] */ Int32 evp_md,
    /* [out] */ Int32 * pSize)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_MD_CTX_destroy(
    /* [in] */ Int32 ctx)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_MD_CTX_copy(
    /* [in] */ Int32 ctx,
    /* [out] */ Int32 * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_DigestInit(
    /* [in] */ Int32 evp_md,
    /* [out] */ Int32 * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_DigestUpdate(
    /* [in] */ Int32 ctx,
    /* [in] */ ArrayOf<Byte> * pBuffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_DigestFinal(
    /* [in] */ Int32 ctx,
    /* [in] */ ArrayOf<Byte> * pHash,
    /* [in] */ Int32 offset,
    /* [out] */ Int32 * pDigestFinal)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_SignInit(
    /* [in] */ const String& algorithm,
    /* [out] */ Int32 * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_SignUpdate(
    /* [in] */ Int32 ctx,
    /* [in] */ ArrayOf<Byte> * pBuffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_SignFinal(
    /* [in] */ Int32 ctx,
    /* [in] */ ArrayOf<Byte> * pSignature,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 key,
    /* [out] */ Int32 * pDigestFinal)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_VerifyInit(
    /* [in] */ const String& algorithm,
    /* [out] */ Int32 * pInit)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_VerifyUpdate(
    /* [in] */ Int32 ctx,
    /* [in] */ ArrayOf<Byte> * pBuffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_VerifyFinal(
    /* [in] */ Int32 ctx,
    /* [in] */ ArrayOf<Byte> * pSignature,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ Int32 key,
    /* [out] */ Int32 * pDigestFinal)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_get_cipherbyname(
    /* [in] */ const String& string,
    /* [out] */ Int32 * pCipher)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_CipherInit_ex(
    /* [in] */ Int32 cipherNid,
    /* [in] */ ArrayOf<Byte> * pKey,
    /* [in] */ ArrayOf<Byte> * pIv,
    /* [in] */ Boolean encrypting,
    /* [out] */ Int32 * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_CipherUpdate(
    /* [in] */ Int32 ctx,
    /* [in] */ ArrayOf<Byte> * pO,
    /* [in] */ Int32 outOffset,
    /* [in] */ ArrayOf<Byte> * pI,
    /* [in] */ Int32 inOffset,
    /* [out] */ Int32 * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_CipherFinal_ex(
    /* [in] */ Int32 ctx,
    /* [in] */ ArrayOf<Byte> * pO,
    /* [in] */ Int32 outOffset,
    /* [out] */ Int32 * pDigestFinal)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EVP_CIPHER_CTX_cleanup(
    /* [in] */ Int32 ctx)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::RAND_seed(
    /* [in] */ ArrayOf<Byte> * pSeed)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::RAND_load_file(
    /* [in] */ const String& filename,
    /* [in] */ Int64 max_bytes,
    /* [out] */ Int32 * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::RAND_bytes(
    /* [in] */ ArrayOf<Byte> * pOutput)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::X509_NAME_hash(
    /* [in] */ Elastosx::Security::Auth::X500::IX500Principal * pPrincipal,
    /* [out] */ Int32 * pHash)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::X509_NAME_hash_old(
    /* [in] */ Elastosx::Security::Auth::X500::IX500Principal * pPrincipal,
    /* [out] */ Int32 * pHash)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::Get_OPENSSL_TO_STANDARD_CIPHER_SUITES(
    /* [out] */ Elastos::Utility::IMap ** ppSuites)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::Get_STANDARD_TO_OPENSSL_CIPHER_SUITES(
    /* [out] */ Elastos::Utility::IMap ** ppSuites)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_CTX_new(
    /* [out] */ Int32 * pCtx)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::GetDefaultCipherSuites(
    /* [out, callee] */ ArrayOf<String> ** ppSuites)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::GetSupportedCipherSuites(
    /* [out, callee] */ ArrayOf<String> ** ppSuites)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_CTX_free(
    /* [in] */ Int32 ssl_ctx)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_CTX_set_session_id_context(
    /* [in] */ Int32 ssl_ctx,
    /* [in] */ ArrayOf<Byte> * pSid_ctx)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_new(
    /* [in] */ Int32 ssl_ctx,
    /* [out] */ Int32 * pSsl_new)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EncodeCertificates(
    /* [in] */ ArrayOf<Elastos::Security::Cert::ICertificate *> * pInCertificates,
    /* [out, callee] */ ArrayOf<Byte> ** ppOutCertificates)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_use_certificate(
    /* [in] */ Int32 ssl,
    /* [in] */ ArrayOf<Byte> * pAsn1DerEncodedCertificateChain)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_use_OpenSSL_PrivateKey(
    /* [in] */ Int32 ssl,
    /* [in] */ Int32 pkey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_use_PrivateKey(
    /* [in] */ Int32 ssl,
    /* [in] */ ArrayOf<Byte> * pPkcs8EncodedPrivateKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_check_private_key(
    /* [in] */ Int32 ssl)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::EncodeIssuerX509Principals(
    /* [in] */ ArrayOf<Elastos::Security::Cert::IX509Certificate *> * pCertificates,
    /* [out, callee] */ ArrayOf<Byte> ** ppPrincipals)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_set_client_CA_list(
    /* [in] */ Int32 ssl,
    /* [in] */ ArrayOf<Byte> * pAsn1DerEncodedX500Principals)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_get_mode(
    /* [in] */ Int32 ssl,
    /* [out] */ Int64 * pMode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_set_mode(
    /* [in] */ Int32 ssl,
    /* [in] */ Int64 inMode,
    /* [out] */ Int64 * pOutMode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_clear_mode(
    /* [in] */ Int32 ssl,
    /* [in] */ Int64 inMode,
    /* [out] */ Int64 * pOutMode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_get_options(
    /* [in] */ Int32 ssl,
    /* [out] */ Int64 * pOptions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_set_options(
    /* [in] */ Int32 ssl,
    /* [in] */ Int64 options,
    /* [out] */ Int64 * pRetOptions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_clear_options(
    /* [in] */ Int32 ssl,
    /* [in] */ Int64 options,
    /* [out] */ Int64 * pRetOptions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::GetDefaultProtocols(
    /* [out, callee] */ ArrayOf<String> ** ppProtocols)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::GetSupportedProtocols(
    /* [out, callee] */ ArrayOf<String> ** ppProtocols)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SetEnabledProtocols(
    /* [in] */ Int32 ssl,
    /* [in] */ ArrayOf<String> * pProtocols)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::CheckEnabledProtocols(
    /* [in] */ ArrayOf<String> * pProtocols,
    /* [out, callee] */ ArrayOf<String> ** ppRetProtocols)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_set_cipher_lists(
    /* [in] */ Int32 ssl,
    /* [in] */ ArrayOf<String> * pCiphers)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SetEnabledCipherSuites(
    /* [in] */ Int32 ssl,
    /* [in] */ ArrayOf<String> * pCipherSuites)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::CheckEnabledCipherSuites(
    /* [in] */ ArrayOf<String> * pCipherSuites,
    /* [out, callee] */ ArrayOf<String> ** ppSuites)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_set_verify(
    /* [in] */ Int32 sslNativePointer,
    /* [in] */ Int32 mode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_set_session(
    /* [in] */ Int32 sslNativePointer,
    /* [in] */ Int32 sslSessionNativePointer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_set_session_creation_enabled(
    /* [in] */ Int32 sslNativePointer,
    /* [in] */ Boolean creationEnabled)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_set_tlsext_host_name(
    /* [in] */ Int32 sslNativePointer,
    /* [in] */ const String& hostname)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_get_servername(
    /* [in] */ Int32 sslNativePointer,
    /* [out] */ String * pServerName)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_CTX_enable_npn(
    /* [in] */ Int32 sslCtxNativePointer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_CTX_disable_npn(
    /* [in] */ Int32 sslCtxNativePointer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_do_handshake(
    /* [in] */ Int32 sslNativePointer,
    /* [in] */ Elastos::IO::IFileDescriptor * pFd,
    /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ISSLHandshakeCallbacks * pShc,
    /* [in] */ Int32 timeoutMillis,
    /* [in] */ Boolean client_mode,
    /* [in] */ ArrayOf<Byte> * pNpnProtocols,
    /* [out] */ Int32 * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_get_npn_negotiated_protocol(
    /* [in] */ Int32 sslNativePointer,
    /* [out, callee] */ ArrayOf<Byte> ** ppProtocol)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_renegotiate(
    /* [in] */ Int32 sslNativePointer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_get_certificate(
    /* [in] */ Int32 sslNativePointer,
    /* [out, callee] */ ArrayOf<Byte> ** ppCertificate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_get_peer_cert_chain(
    /* [in] */ Int32 sslNativePointer,
    /* [out, callee] */ ArrayOf<Byte> ** ppCert_chain)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_read(
    /* [in] */ Int32 sslNativePointer,
    /* [in] */ Elastos::IO::IFileDescriptor * pFd,
    /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ISSLHandshakeCallbacks * pShc,
    /* [in] */ ArrayOf<Byte> * pB,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len,
    /* [in] */ Int32 readTimeoutMillis,
    /* [out] */ Int32 * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_write(
    /* [in] */ Int32 sslNativePointer,
    /* [in] */ Elastos::IO::IFileDescriptor * pFd,
    /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ISSLHandshakeCallbacks * pShc,
    /* [in] */ ArrayOf<Byte> * pB,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len,
    /* [in] */ Int32 writeTimeoutMillis)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_interrupt(
    /* [in] */ Int32 sslNativePointer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_shutdown(
    /* [in] */ Int32 sslNativePointer,
    /* [in] */ Elastos::IO::IFileDescriptor * pFd,
    /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ISSLHandshakeCallbacks * pShc)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_free(
    /* [in] */ Int32 sslNativePointer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_SESSION_session_id(
    /* [in] */ Int32 sslSessionNativePointer,
    /* [out, callee] */ ArrayOf<Byte> ** ppSession_id)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_SESSION_get_time(
    /* [in] */ Int32 sslSessionNativePointer,
    /* [out] */ Int64 * pTime)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_SESSION_get_version(
    /* [in] */ Int32 sslSessionNativePointer,
    /* [out] */ String * pVer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_SESSION_cipher(
    /* [in] */ Int32 sslSessionNativePointer,
    /* [out] */ String * pCipher)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::SSL_SESSION_free(
    /* [in] */ Int32 sslSessionNativePointer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::I2d_SSL_SESSION(
    /* [in] */ Int32 sslSessionNativePointer,
    /* [out, callee] */ ArrayOf<Byte> ** ppSession)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNativeCrypto::D2i_SSL_SESSION(
    /* [in] */ ArrayOf<Byte> * pData,
    /* [out] */ Int32 * pSession)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}
}

