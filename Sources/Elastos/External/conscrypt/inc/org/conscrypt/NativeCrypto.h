
#ifndef __NATIVECRYPTO_H__
#define __NATIVECRYPTO_H__

#include "Org.Conscrypt.h"

using Elastos::Core::IArrayOf;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IOutputStream;
using Elastos::Security::Interfaces::IDSAPrivateKey;
using Elastos::Security::Interfaces::IECPrivateKey;
using Elastos::Security::Interfaces::IRSAPrivateKey;
using Elastos::Utility::ICalendar;
using Elastos::Utility::IMap;
using Elastosx::Security::Auth::X500::IX500Principal;

namespace Org {
namespace Conscrypt {

class NativeCrypto
{
public:
    static CARAPI ENGINE_load_dynamic();

    static CARAPI ENGINE_by_id(
        /* [in] */ const String& id,
        /* [out] */ Int64* result);

    static CARAPI ENGINE_add(
        /* [in] */ Int64 engineRef,
        /* [out] */ Int32* result);

    static CARAPI ENGINE_init(
        /* [in] */ Int64 engineRef,
        /* [out] */ Int32* result);

    static CARAPI ENGINE_finish(
        /* [in] */ Int64 engineRef,
        /* [out] */ Int32* result);

    static CARAPI ENGINE_free(
        /* [in] */ Int64 engineRef,
        /* [out] */ Int32* result);

    static CARAPI ENGINE_load_private_key(
        /* [in] */ Int64 engineRef,
        /* [in] */ const String& key_id,
        /* [out] */ Int64* result);

    static CARAPI ENGINE_get_id(
        /* [in] */ Int64 engineRef,
        /* [out] */ String* result);

    static CARAPI ENGINE_ctrl_cmd_string(
        /* [in] */ Int64 engineRef,
        /* [in] */ const String& cmd,
        /* [in] */ const String& arg,
        /* [in] */ Int32 cmd_optional,
        /* [out] */ Int32* result);

    static CARAPI EVP_PKEY_new_DSA(
        /* [in] */ ArrayOf<Byte>* p,
        /* [in] */ ArrayOf<Byte>* q,
        /* [in] */ ArrayOf<Byte>* g,
        /* [in] */ ArrayOf<Byte>* pub_key,
        /* [in] */ ArrayOf<Byte>* priv_key,
        /* [out] */ Int64* result);

    static CARAPI EVP_PKEY_new_RSA(
        /* [in] */ ArrayOf<Byte>* n,
        /* [in] */ ArrayOf<Byte>* e,
        /* [in] */ ArrayOf<Byte>* d,
        /* [in] */ ArrayOf<Byte>* p,
        /* [in] */ ArrayOf<Byte>* q,
        /* [in] */ ArrayOf<Byte>* dmp1,
        /* [in] */ ArrayOf<Byte>* dmq1,
        /* [in] */ ArrayOf<Byte>* iqmp,
        /* [out] */ Int64* result);

    static CARAPI EVP_PKEY_new_mac_key(
        /* [in] */ Int32 type,
        /* [in] */ ArrayOf<Byte>* key,
        /* [out] */ Int64* result);

    static CARAPI EVP_PKEY_size(
        /* [in] */ Int64 pkeyRef,
        /* [out] */ Int32* result);

    static CARAPI EVP_PKEY_type(
        /* [in] */ Int64 pkeyRef,
        /* [out] */ Int32* result);

    static CARAPI EVP_PKEY_print_public(
        /* [in] */ Int64 pkeyRef,
        /* [out] */ String* result);

    static CARAPI EVP_PKEY_print_private(
        /* [in] */ Int64 pkeyRef,
        /* [out] */ String* result);

    static CARAPI EVP_PKEY_free(
        /* [in] */ Int64 pkeyRef);

    static CARAPI EVP_PKEY_cmp(
        /* [in] */ Int64 pkey1Ref,
        /* [in] */ Int64 pkey2Ref,
        /* [out] */ Int32* result);

    static CARAPI I2d_PKCS8_PRIV_KEY_INFO(
        /* [in] */ Int64 pkeyRef,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI D2i_PKCS8_PRIV_KEY_INFO(
        /* [in] */ ArrayOf<Byte>* keyBytes,
        /* [out] */ Int64* result);

    static CARAPI I2d_PUBKEY(
        /* [in] */ Int64 pkeyRef,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI D2i_PUBKEY(
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ Int64* result);

    static CARAPI GetRSAPrivateKeyWrapper(
        /* [in] */ IRSAPrivateKey* key,
        /* [in] */ ArrayOf<Byte>* modulus,
        /* [out] */ Int64* result);

    static CARAPI GetDSAPrivateKeyWrapper(
        /* [in] */ IDSAPrivateKey* key,
        /* [out] */ Int64* result);

    static CARAPI GetECPrivateKeyWrapper(
        /* [in] */ IECPrivateKey* key,
        /* [in] */ Int64 ecGroupRef,
        /* [out] */ Int64* result);

    static CARAPI RSA_generate_key_ex(
        /* [in] */ Int32 modulusBits,
        /* [in] */ ArrayOf<Byte>* publicExponent,
        /* [out] */ Int64* result);

    static CARAPI RSA_size(
        /* [in] */ Int64 pkeyRef,
        /* [out] */ Int32* result);

    static CARAPI RSA_private_encrypt(
        /* [in] */ Int32 flen,
        /* [in] */ ArrayOf<Byte>* from,
        /* [in] */ ArrayOf<Byte>* to,
        /* [in] */ Int64 pkeyRef,
        /* [in] */ Int32 padding,
        /* [out] */ Int32* result);

    static CARAPI RSA_public_decrypt(
        /* [in] */ Int32 flen,
        /* [in] */ ArrayOf<Byte>* from,
        /* [in] */ ArrayOf<Byte>* to,
        /* [in] */ Int64 pkeyRef,
        /* [in] */ Int32 padding,
        /* [out] */ Int32* result);

    static CARAPI RSA_public_encrypt(
        /* [in] */ Int32 flen,
        /* [in] */ ArrayOf<Byte>* from,
        /* [in] */ ArrayOf<Byte>* to,
        /* [in] */ Int64 pkeyRef,
        /* [in] */ Int32 padding,
        /* [out] */ Int32* result);

    static CARAPI RSA_private_decrypt(
        /* [in] */ Int32 flen,
        /* [in] */ ArrayOf<Byte>* from,
        /* [in] */ ArrayOf<Byte>* to,
        /* [in] */ Int64 pkeyRef,
        /* [in] */ Int32 padding,
        /* [out] */ Int32* result);

    static CARAPI Get_RSA_public_params(
        /* [in] */ Int64 pkeyRef,
        /* [out, callee] */ ArrayOf<Byte>** n,
        /* [out, callee] */ ArrayOf<Byte>** e);

    static CARAPI Get_RSA_private_params(
        /* [in] */ Int64 pkeyRef,
        /* [out, callee] */ ArrayOf<Byte>** n,
        /* [out, callee] */ ArrayOf<Byte>** e,
        /* [out, callee] */ ArrayOf<Byte>** d,
        /* [out, callee] */ ArrayOf<Byte>** p,
        /* [out, callee] */ ArrayOf<Byte>** q,
        /* [out, callee] */ ArrayOf<Byte>** dmp1,
        /* [out, callee] */ ArrayOf<Byte>** dmq1,
        /* [out, callee] */ ArrayOf<Byte>** iqmp);

    static CARAPI DSA_generate_key(
        /* [in] */ Int32 primeBits,
        /* [in] */ ArrayOf<Byte>* seed,
        /* [in] */ ArrayOf<Byte>* g,
        /* [in] */ ArrayOf<Byte>* p,
        /* [in] */ ArrayOf<Byte>* q,
        /* [out] */ Int64* result);

    static CARAPI Get_DSA_params(
        /* [in] */ Int64 pkeyRef,
        /* [out, callee] */ ArrayOf<Byte>** g,
        /* [out, callee] */ ArrayOf<Byte>** p,
        /* [out, callee] */ ArrayOf<Byte>** q,
        /* [out, callee] */ ArrayOf<Byte>** ypub,
        /* [out, callee] */ ArrayOf<Byte>** xpriv);

    static CARAPI Set_DSA_flag_nonce_from_hash(
        /* [in] */ Int64 pkeyRef);

    static CARAPI I2d_RSAPublicKey(
        /* [in] */ Int64 pkeyRef,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI I2d_RSAPrivateKey(
        /* [in] */ Int64 pkeyRef,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI I2d_DSAPublicKey(
        /* [in] */ Int64 pkeyRef,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI I2d_DSAPrivateKey(
        /* [in] */ Int64 pkeyRef,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI EVP_PKEY_new_DH(
        /* [in] */ ArrayOf<Byte>* p,
        /* [in] */ ArrayOf<Byte>* g,
        /* [in] */ ArrayOf<Byte>* pub_key,
        /* [in] */ ArrayOf<Byte>* priv_key,
        /* [out] */ Int64* result);

    static CARAPI DH_generate_parameters_ex(
        /* [in] */ Int32 primeBits,
        /* [in] */ Int64 generator,
        /* [out] */ Int64* result);

    static CARAPI DH_generate_key(
        /* [in] */ Int64 pkeyRef);

    static CARAPI Get_DH_params(
        /* [in] */ Int64 pkeyRef,
        /* [out, callee] */ ArrayOf<Byte>** p,
        /* [out, callee] */ ArrayOf<Byte>** g,
        /* [out, callee] */ ArrayOf<Byte>** ypub,
        /* [out, callee] */ ArrayOf<Byte>** xpriv);

    static CARAPI EVP_PKEY_new_EC_KEY(
        /* [in] */ Int64 groupRef,
        /* [in] */ Int64 pubkeyRef,
        /* [in] */ ArrayOf<Byte>* keyBytes,
        /* [out] */ Int64* result);

    static CARAPI EC_GROUP_new_by_curve_name(
        /* [in] */ const String& curveName,
        /* [out] */ Int64* result);

    static CARAPI EC_GROUP_new_curve(
        /* [in] */ Int32 type,
        /* [in] */ ArrayOf<Byte>* p,
        /* [in] */ ArrayOf<Byte>* a,
        /* [in] */ ArrayOf<Byte>* b,
        /* [out] */ Int64* result);

    static CARAPI EC_GROUP_dup(
        /* [in] */ Int64 groupRef,
        /* [out] */ Int64* result);

    static CARAPI EC_GROUP_set_asn1_flag(
        /* [in] */ Int64 groupRef,
        /* [in] */ Int32 flag);

    static CARAPI EC_GROUP_set_point_conversion_form(
        /* [in] */ Int64 groupRef,
        /* [in] */ Int32 form);

    static CARAPI EC_GROUP_get_curve_name(
        /* [in] */ Int64 groupRef,
        /* [out] */ String* result);

    static CARAPI EC_GROUP_get_curve(
        /* [in] */ Int64 groupRef,
        /* [out, callee] */ ArrayOf<Byte>** p,
        /* [out, callee] */ ArrayOf<Byte>** a,
        /* [out, callee] */ ArrayOf<Byte>** b);

    static CARAPI EC_GROUP_clear_free(
        /* [in] */ Int64 groupRef);

    static CARAPI EC_GROUP_cmp(
        /* [in] */ Int64 group1Ref,
        /* [in] */ Int64 group2Ref,
        /* [out] */ Boolean* result);

    static CARAPI EC_GROUP_set_generator(
        /* [in] */ Int64 groupRef,
        /* [in] */ Int64 pointRef,
        /* [in] */ ArrayOf<Byte>* nBytes,
        /* [in] */ ArrayOf<Byte>* hBytes);

    static CARAPI EC_GROUP_get_generator(
        /* [in] */ Int64 groupRef,
        /* [out] */ Int64* result);

    static CARAPI Get_EC_GROUP_type(
        /* [in] */ Int64 groupRef,
        /* [out] */ Int32* result);

    static CARAPI EC_GROUP_get_order(
        /* [in] */ Int64 groupRef,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI EC_GROUP_get_degree(
        /* [in] */ Int64 groupRef,
        /* [out] */ Int32* result);

    static CARAPI EC_GROUP_get_cofactor(
        /* [in] */ Int64 groupRef,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI EC_POINT_new(
        /* [in] */ Int64 groupRef,
        /* [out] */ Int64* result);

    static CARAPI EC_POINT_clear_free(
        /* [in] */ Int64 pointRef);

    static CARAPI EC_POINT_cmp(
        /* [in] */ Int64 groupRef,
        /* [in] */ Int64 point1Ref,
        /* [in] */ Int64 point2Ref,
        /* [out] */ Boolean* result);

    static CARAPI EC_POINT_get_affine_coordinates(
        /* [in] */ Int64 groupRef,
        /* [in] */ Int64 pointRef,
        /* [out, callee] */ ArrayOf<Byte>** xBytes,
        /* [out, callee] */ ArrayOf<Byte>** yBytes);

    static CARAPI EC_POINT_set_affine_coordinates(
        /* [in] */ Int64 groupRef,
        /* [in] */ Int64 pointRef,
        /* [in] */ ArrayOf<Byte>* xBytes,
        /* [in] */ ArrayOf<Byte>* yBytes);

    static CARAPI EC_KEY_generate_key(
        /* [in] */ Int64 groupRef,
        /* [out] */ Int64* result);

    static CARAPI EC_KEY_get0_group(
        /* [in] */ Int64 pkeyRef,
        /* [out] */ Int64* result);

    static CARAPI EC_KEY_get_private_key(
        /* [in] */ Int64 pkeyRef,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI EC_KEY_get_public_key(
        /* [in] */ Int64 pkeyRef,
        /* [out] */ Int64* result);

    static CARAPI EC_KEY_set_nonce_from_hash(
        /* [in] */ Int64 pkeyRef,
        /* [in] */ Boolean enabled);

    static CARAPI ECDH_compute_key(
        /* [in] */ ArrayOf<Byte>* outArray,
        /* [in] */ Int32 outOffset,
        /* [in] */ Int64 publicKeyRef,
        /* [in] */ Int64 privateKeyRef,
        /* [out] */ Int32* result);

    // --- Message digest functions --------------

    static CARAPI EVP_get_digestbyname(
        /* [in] */ const String& algorithm,
        /* [out] */ Int64* result);

    static CARAPI EVP_MD_size(
        /* [in] */ Int64 evpMdRef,
        /* [out] */ Int32* result);

    static CARAPI EVP_MD_block_size(
        /* [in] */ Int64 evpMdRef,
        /* [out] */ Int32* result);

    // --- Message digest context functions --------------

    static CARAPI EVP_MD_CTX_create(
        /* [out] */ Int64* result);

    static CARAPI EVP_MD_CTX_init(
        /* [in] */ IOpenSSLDigestContext* ctxRef);

    static CARAPI EVP_MD_CTX_destroy(
        /* [in] */ Int64 ctx);

    static CARAPI EVP_MD_CTX_copy(
        /* [in] */ IOpenSSLDigestContext* dstCtxRef,
        /* [in] */ IOpenSSLDigestContext* srcCtxRef,
        /* [out] */ Int32* result);

    // --- Digest handling functions -------------------------------------------

    static CARAPI EVP_DigestInit(
        /* [in] */ IOpenSSLDigestContext* evpMdCtxRef,
        /* [in] */ Int64 evpMdRef,
        /* [out] */ Int32* result);

    static CARAPI EVP_DigestUpdate(
        /* [in] */ IOpenSSLDigestContext* evpMdCtxRef,
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    static CARAPI EVP_DigestFinal(
        /* [in] */ IOpenSSLDigestContext* ctxRef,
        /* [in] */ ArrayOf<Byte>* hash,
        /* [in] */ Int32 offset,
        /* [out] */ Int32* result);

    // --- MAC handling functions ----------------------------------------------

    static CARAPI EVP_DigestSignInit(
        /* [in] */ IOpenSSLDigestContext* evpMdCtxRef,
        /* [in] */ Int64 evpMdRef,
        /* [in] */ Int64 pkeyRef);

    static CARAPI EVP_DigestSignUpdate(
        /* [in] */ IOpenSSLDigestContext* evpMdCtxRef,
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    static CARAPI EVP_DigestSignFinal(
        /* [in] */ IOpenSSLDigestContext* evpMdCtxRef,
        /* [out, callee] */ ArrayOf<Byte>** result);

    // --- Signature handling functions ----------------------------------------

    static CARAPI EVP_SignInit(
        /* [in] */ IOpenSSLDigestContext* evpMdCtxRef,
        /* [in] */ Int64 evpMdRef,
        /* [out] */ Int32* result);

    static CARAPI EVP_SignUpdate(
        /* [in] */ IOpenSSLDigestContext* evpMdCtxRef,
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    static CARAPI EVP_SignFinal(
        /* [in] */ IOpenSSLDigestContext* ctxRef,
        /* [in] */ ArrayOf<Byte>* signature,
        /* [in] */ Int32 offset,
        /* [in] */ Int64 pkeyRef,
        /* [out] */ Int32* result);

    static CARAPI EVP_VerifyInit(
        /* [in] */ IOpenSSLDigestContext* evpMdCtxRef,
        /* [in] */ Int64 evpMdRef,
        /* [out] */ Int32* result);

    static CARAPI EVP_VerifyUpdate(
        /* [in] */ IOpenSSLDigestContext* ctxRef,
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    static CARAPI EVP_VerifyFinal(
        /* [in] */ IOpenSSLDigestContext* ctxRef,
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [in] */ Int64 pkeyRef,
        /* [out] */ Int32* result);

    // --- Block ciphers -------------------------------------------------------

    static CARAPI EVP_get_cipherbyname(
        /* [in] */ const String& algorithm,
        /* [out] */ Int64* result);

    static CARAPI EVP_CipherInit_ex(
        /* [in] */ Int64 ctxRef,
        /* [in] */ Int64 evpCipherRef,
        /* [in] */ ArrayOf<Byte>* keyArray,
        /* [in] */ ArrayOf<Byte>* ivArray,
        /* [in] */ Boolean encrypting);

    static CARAPI EVP_CipherUpdate(
        /* [in] */ Int64 ctxRef,
        /* [in] */ ArrayOf<Byte>* outArray,
        /* [in] */ Int32 outOffset,
        /* [in] */ ArrayOf<Byte>* inArray,
        /* [in] */ Int32 inOffset,
        /* [in] */ Int32 inLength,
        /* [out] */ Int32* result);

    static CARAPI EVP_CipherFinal_ex(
        /* [in] */ Int64 ctxRef,
        /* [in] */ ArrayOf<Byte>* outArray,
        /* [in] */ Int32 outOffset,
        /* [out] */ Int32* result);

    static CARAPI EVP_CIPHER_iv_length(
        /* [in] */ Int64 evpCipherRef,
        /* [out] */ Int32* result);

    static CARAPI EVP_CIPHER_CTX_new(
        /* [out] */ Int64* result);

    static CARAPI EVP_CIPHER_CTX_block_size(
        /* [in] */ Int64 ctxRef,
        /* [out] */ Int32* result);

    static CARAPI Get_EVP_CIPHER_CTX_buf_len(
        /* [in] */ Int64 ctxRef,
        /* [out] */ Int32* result);

    static CARAPI EVP_CIPHER_CTX_set_padding(
        /* [in] */ Int64 ctxRef,
        /* [in] */ Boolean enablePaddingBool);

    static CARAPI EVP_CIPHER_CTX_set_key_length(
        /* [in] */ Int64 ctxRef,
        /* [in] */ Int32 keySizeBits);

    static CARAPI EVP_CIPHER_CTX_cleanup(
        /* [in] */ Int64 ctxRef);

    // --- RAND seeding --------------------------------------------------------

    static CARAPI RAND_seed(
        /* [in] */ ArrayOf<Byte>* seed);

    static CARAPI RAND_load_file(
        /* [in] */ const String& filename,
        /* [in] */ Int64 max_bytes,
        /* [out] */ Int32* result);

    static CARAPI RAND_bytes(
        /* [in] */ ArrayOf<Byte>* output);

    // --- ASN.1 objects -------------------------------------------------------

    static CARAPI OBJ_txt2nid(
        /* [in] */ const String& oid,
        /* [out] */ Int32* result);

    static CARAPI OBJ_txt2nid_longName(
        /* [in] */ const String& oid,
        /* [out] */ String* result);

    static CARAPI OBJ_txt2nid_oid(
        /* [in] */ const String& oid,
        /* [out] */ String* result);

    // --- X509_NAME -----------------------------------------------------------

    static CARAPI X509_NAME_hash(
        /* [in] */ IX500Principal* principal,
        /* [out] */ Int32* result);

    static CARAPI X509_NAME_hash_old(
        /* [in] */ IX500Principal* principal,
        /* [out] */ Int32* result);

    static CARAPI X509_NAME_print_ex(
        /* [in] */ Int64 x509NameRef,
        /* [in] */ Int64 flags,
        /* [out] */ String* result);

    // --- X509 ----------------------------------------------------------------

    static CARAPI D2i_X509_bio(
        /* [in] */ Int64 bioRef,
        /* [out] */ Int64* result);

    static CARAPI D2i_X509(
        /* [in] */ ArrayOf<Byte>* certBytes,
        /* [out] */ Int64* result);

    static CARAPI PEM_read_bio_X509(
        /* [in] */ Int64 bioRef,
        /* [out] */ Int64* result);

    static CARAPI I2d_X509(
        /* [in] */ Int64 x509Ref,
        /* [out, callee] */ ArrayOf<Byte>** result);

    /** Takes an X509 context not an X509_PUBKEY context. */
    static CARAPI I2d_X509_PUBKEY(
        /* [in] */ Int64 x509Ref,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI ASN1_seq_pack_X509(
        /* [in] */ ArrayOf<Int64>* x509CertRefs,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI ASN1_seq_unpack_X509_bio(
        /* [in] */ Int64 bioRef,
        /* [out, callee] */ ArrayOf<Int64>** result);

    static CARAPI X509_free(
        /* [in] */ Int64 x509Ref);

    static CARAPI X509_cmp(
        /* [in] */ Int64 x509Ref1,
        /* [in] */ Int64 x509Ref2,
        /* [out] */ Int32* result);

    static CARAPI Get_X509_hashCode(
        /* [in] */ Int64 x509Ref,
        /* [out] */ Int32* result);

    static CARAPI X509_print_ex(
        /* [in] */ Int64 bioRef,
        /* [in] */ Int64 x509Ref,
        /* [in] */ Int64 nmflag,
        /* [in] */ Int64 certflag);

    static CARAPI X509_get_issuer_name(
        /* [in] */ Int64 x509Ref,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI X509_get_subject_name(
        /* [in] */ Int64 x509Ref,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI Get_X509_sig_alg_oid(
        /* [in] */ Int64 x509Ref,
        /* [out] */ String* result);

    static CARAPI Get_X509_sig_alg_parameter(
        /* [in] */ Int64 x509Ref,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI Get_X509_issuerUID(
        /* [in] */ Int64 x509Ref,
        /* [out, callee] */ ArrayOf<Boolean>** result);

    static CARAPI Get_X509_subjectUID(
        /* [in] */ Int64 x509Ref,
        /* [out, callee] */ ArrayOf<Boolean>** result);

    static CARAPI X509_get_pubkey(
        /* [in] */ Int64 x509Ref,
        /* [out] */ Int64* result);

    static CARAPI Get_X509_pubkey_oid(
        /* [in] */ Int64 x509Ref,
        /* [out] */ String* result);

    static CARAPI X509_get_ext_oid(
        /* [in] */ Int64 x509Ref,
        /* [in] */ const String& oid,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI Get_X509_ext_oids(
        /* [in] */ Int64 x509Ref,
        /* [in] */ Int32 critical,
        /* [out, callee] */ ArrayOf<String>** result);

    static CARAPI Get_X509_GENERAL_NAME_stack(
        /* [in] */ Int64 x509Ref,
        /* [in] */ Int32 type,
        /* [out, callee] */ ArrayOf<IArrayOf*>** result);

    static CARAPI Get_X509_ex_kusage(
        /* [in] */ Int64 x509Ref,
        /* [out, callee] */ ArrayOf<Boolean>** result);

    static CARAPI Get_X509_ex_xkusage(
        /* [in] */ Int64 x509Ref,
        /* [out, callee] */ ArrayOf<String>** result);

    static CARAPI Get_X509_ex_pathlen(
        /* [in] */ Int64 x509Ref,
        /* [out] */ Int32* result);

    static CARAPI X509_get_notBefore(
        /* [in] */ Int64 x509Ref,
        /* [out] */ Int64* result);

    static CARAPI X509_get_notAfter(
        /* [in] */ Int64 x509Ref,
        /* [out] */ Int64* result);

    static CARAPI X509_get_version(
        /* [in] */ Int64 x509Ref,
        /* [out] */ Int64* result);

    static CARAPI X509_get_serialNumber(
        /* [in] */ Int64 x509Ref,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI X509_verify(
        /* [in] */ Int64 x509Ref,
        /* [in] */ Int64 pkeyRef);

    static CARAPI Get_X509_cert_info_enc(
        /* [in] */ Int64 x509Ref,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI Get_X509_signature(
        /* [in] */ Int64 x509Ref,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI Get_X509_ex_flags(
        /* [in] */ Int64 x509Ref,
        /* [out] */ Int32* result);

    static CARAPI X509_check_issued(
        /* [in] */ Int64 x509Ref1,
        /* [in] */ Int64 x509Ref2,
        /* [out] */ Int32* result);

    // --- PKCS7 ---------------------------------------------------------------

    /** Returns an array of X509 or X509_CRL pointers. */
    static CARAPI D2i_PKCS7_bio(
        /* [in] */ Int64 bioRef,
        /* [in] */ Int32 which,
        /* [out, callee] */ ArrayOf<Int64>** result);

    /** Returns an array of X509 or X509_CRL pointers. */
    static CARAPI I2d_PKCS7(
        /* [in] */ ArrayOf<Int64>* certsArray,
        /* [out, callee] */ ArrayOf<Byte>** result);

    /** Returns an array of X509 or X509_CRL pointers. */
    static CARAPI PEM_read_bio_PKCS7(
        /* [in] */ Int64 bioRef,
        /* [in] */ Int32 which,
        /* [out, callee] */ ArrayOf<Int64>** result);

    // --- X509_CRL ------------------------------------------------------------

    static CARAPI D2i_X509_CRL_bio(
        /* [in] */ Int64 bioRef,
        /* [out] */ Int64* result);

    static CARAPI PEM_read_bio_X509_CRL(
        /* [in] */ Int64 bioRef,
        /* [out] */ Int64* result);

    static CARAPI I2d_X509_CRL(
        /* [in] */ Int64 x509CrlRef,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI X509_CRL_free(
        /* [in] */ Int64 x509CrlRef);

    static CARAPI X509_CRL_print(
        /* [in] */ Int64 bioRef,
        /* [in] */ Int64 x509CrlRef);

    static CARAPI Get_X509_CRL_sig_alg_oid(
        /* [in] */ Int64 x509CrlRef,
        /* [out] */ String* result);

    static CARAPI Get_X509_CRL_sig_alg_parameter(
        /* [in] */ Int64 x509CrlRef,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI X509_CRL_get_issuer_name(
        /* [in] */ Int64 x509CrlRef,
        /* [out, callee] */ ArrayOf<Byte>** result);

    /** Returns X509_REVOKED reference that is not duplicated! */
    static CARAPI X509_CRL_get0_by_cert(
        /* [in] */ Int64 x509crlRef,
        /* [in] */ Int64 x509Ref,
        /* [out] */ Int64* result);

    /** Returns X509_REVOKED reference that is not duplicated! */
    static CARAPI X509_CRL_get0_by_serial(
        /* [in] */ Int64 x509crlRef,
        /* [in] */ ArrayOf<Byte>* serialArray,
        /* [out] */ Int64* result);

    /** Returns an array of X509_REVOKED that are owned by the caller. */
    static CARAPI X509_CRL_get_REVOKED(
        /* [in] */ Int64 x509crlRef,
        /* [out, callee] */ ArrayOf<Int64>** result);

    static CARAPI Get_X509_CRL_ext_oids(
        /* [in] */ Int64 x509crlRef,
        /* [in] */ Int32 critical,
        /* [out, callee] */ ArrayOf<String>** result);

    static CARAPI X509_CRL_get_ext_oid(
        /* [in] */ Int64 x509crlRef,
        /* [in] */ const String& oid,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI X509_CRL_get_version(
        /* [in] */ Int64 x509crlRef,
        /* [out] */ Int64* result);

    static CARAPI X509_CRL_get_ext(
        /* [in] */ Int64 x509crlRef,
        /* [in] */ const String& oid,
        /* [out] */ Int64* result);

    static CARAPI Get_X509_CRL_signature(
        /* [in] */ Int64 x509crlRef,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI X509_CRL_verify(
        /* [in] */ Int64 x509crlRef,
        /* [in] */ Int64 pkeyRef);

    static CARAPI Get_X509_CRL_crl_enc(
        /* [in] */ Int64 x509crlRef,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI X509_CRL_get_lastUpdate(
        /* [in] */ Int64 x509crlRef,
        /* [out] */ Int64* result);

    static CARAPI X509_CRL_get_nextUpdate(
        /* [in] */ Int64 x509crlRef,
        /* [out] */ Int64* result);

    // --- X509_REVOKED --------------------------------------------------------

    static CARAPI X509_REVOKED_dup(
        /* [in] */ Int64 x509RevokedRef,
        /* [out] */ Int64* result);

    static CARAPI I2d_X509_REVOKED(
        /* [in] */ Int64 x509RevokedRef,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI Get_X509_REVOKED_ext_oids(
        /* [in] */ Int64 x509RevokedRef,
        /* [in] */ Int32 critical,
        /* [out, callee] */ ArrayOf<String>** result);

    static CARAPI X509_REVOKED_get_ext_oid(
        /* [in] */ Int64 x509RevokedRef,
        /* [in] */ const String& oid,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI X509_REVOKED_get_serialNumber(
        /* [in] */ Int64 x509RevokedRef,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI X509_REVOKED_get_ext(
        /* [in] */ Int64 x509RevokedRef,
        /* [in] */ const String& oid,
        /* [out] */ Int64* result);

    /** Returns ASN1_TIME reference. */
    static CARAPI Get_X509_REVOKED_revocationDate(
        /* [in] */ Int64 x509RevokedRef,
        /* [out] */ Int64* result);

    static CARAPI X509_REVOKED_print(
        /* [in] */ Int64 bioRef,
        /* [in] */ Int64 x509RevokedRef);

    // --- X509_EXTENSION ------------------------------------------------------

    static CARAPI X509_supported_extension(
        /* [in] */ Int64 x509ExtensionRef,
        /* [out] */ Int32* result);

    // --- ASN1_TIME -----------------------------------------------------------

    static CARAPI ASN1_TIME_to_Calendar(
        /* [in] */ Int64 asn1TimeRef,
        /* [in] */ ICalendar* cal);

    // --- BIO stream creation -------------------------------------------------

    static CARAPI Create_BIO_InputStream(
        /* [in] */ IOpenSSLBIOInputStream* is,
        /* [out] */ Int64* result);

    static CARAPI Create_BIO_OutputStream(
        /* [in] */ IOutputStream* os,
        /* [out] */ Int64* result);

    static CARAPI BIO_read(
        /* [in] */ Int64 bioRef,
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [out] */ Int32* result);

    static CARAPI BIO_write(
        /* [in] */ Int64 bioRef,
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    static CARAPI BIO_free_all(
        /* [in] */ Int64 bioRef);

    // --- SSL handling --------------------------------------------------------

    static CARAPI Get_OPENSSL_TO_STANDARD_CIPHER_SUITES(
        /* [out] */ IMap** result);

    static CARAPI Get_STANDARD_TO_OPENSSL_CIPHER_SUITES(
        /* [out] */ IMap** result);

    static CARAPI SSL_CTX_new(
        /* [out] */ Int64* result);

    static CARAPI GetSupportedCipherSuites(
        /* [out, callee] */ ArrayOf<String>** result);

    static CARAPI SSL_CTX_free(
        /* [in] */ Int64 ssl_ctx_address);

    static CARAPI SSL_CTX_set_session_id_context(
        /* [in] */ Int64 ssl_ctx_address,
        /* [in] */ ArrayOf<Byte>* sid_ctx);

    static CARAPI SSL_new(
        /* [in] */ Int64 ssl_ctx_address,
        /* [out] */ Int64* result);

    static CARAPI SSL_enable_tls_channel_id(
        /* [in] */ Int64 ssl_address);

    static CARAPI SSL_get_tls_channel_id(
        /* [in] */ Int64 ssl_address,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI SSL_set1_tls_channel_id(
        /* [in] */ Int64 ssl_address,
        /* [in] */ Int64 pkeyRef);

    static CARAPI SSL_use_certificate(
        /* [in] */ Int64 ssl_address,
        /* [in] */ ArrayOf<Int64>* certificates);

    static CARAPI SSL_use_PrivateKey(
        /* [in] */ Int64 ssl_address,
        /* [in] */ Int64 pkeyRef);

    static CARAPI SSL_check_private_key(
        /* [in] */ Int64 ssl_address);

    static CARAPI SSL_set_client_CA_list(
        /* [in] */ Int64 ssl_address,
        /* [in] */ ArrayOf<Handle32>* asn1DerEncodedX500Principals);

    static CARAPI SSL_get_mode(
        /* [in] */ Int64 ssl_address,
        /* [out] */ Int64* result);

    static CARAPI SSL_set_mode(
        /* [in] */ Int64 ssl_address,
        /* [in] */ Int64 mode,
        /* [out] */ Int64* result);

    static CARAPI SSL_clear_mode(
        /* [in] */ Int64 ssl_address,
        /* [in] */ Int64 mode,
        /* [out] */ Int64* result);

    static CARAPI SSL_get_options(
        /* [in] */ Int64 ssl_address,
        /* [out] */ Int64* result);

    static CARAPI SSL_set_options(
        /* [in] */ Int64 ssl_address,
        /* [in] */ Int64 options,
        /* [out] */ Int64* result);

    static CARAPI SSL_clear_options(
        /* [in] */ Int64 ssl_address,
        /* [in] */ Int64 options,
        /* [out] */ Int64* result);

    static CARAPI SSL_use_psk_identity_hint(
        /* [in] */ Int64 ssl_address,
        /* [in] */ const String& identityHint);

    static CARAPI Set_SSL_psk_client_callback_enabled(
        /* [in] */ Int64 ssl_address,
        /* [in] */ Boolean enabled);

    static CARAPI Set_SSL_psk_server_callback_enabled(
        /* [in] */ Int64 ssl_address,
        /* [in] */ Boolean enabled);

    static CARAPI Get_DEFAULT_PROTOCOLS(
        /* [out, callee] */ ArrayOf<String>** result);

    static CARAPI GetSupportedProtocols(
        /* [out, callee] */ ArrayOf<String>** result);

    static CARAPI SetEnabledProtocols(
        /* [in] */ Int64 ssl,
        /* [in] */ ArrayOf<String>* protocols);

    static CARAPI CheckEnabledProtocols(
        /* [in] */ ArrayOf<String>* protocols,
        /* [out, callee] */ ArrayOf<String>** result);

    static CARAPI SSL_set_cipher_lists(
        /* [in] */ Int64 ssl_address,
        /* [in] */ ArrayOf<String>* ciphers);

    /**
     * Gets the list of cipher suites enabled for the provided {@code SSL} instance.
     *
     * @return array of {@code SSL_CIPHER} references.
     */
    static CARAPI SSL_get_ciphers(
        /* [in] */ Int64 ssl_address,
        /* [out, callee] */ ArrayOf<Int64>** result);

    static CARAPI Get_SSL_CIPHER_algorithm_mkey(
        /* [in] */ Int64 ssl_cipher_address,
        /* [out] */ Int32* result);

    static CARAPI Get_SSL_CIPHER_algorithm_auth(
        /* [in] */ Int64 ssl_cipher_address,
        /* [out] */ Int32* result);

    static CARAPI SetEnabledCipherSuites(
        /* [in] */ Int64 ssl,
        /* [in] */ ArrayOf<String>* cipherSuites);

    static CARAPI CheckEnabledCipherSuites(
        /* [in] */ ArrayOf<String>* cipherSuites,
        /* [out, callee] */ ArrayOf<String>** result);

    static CARAPI SSL_set_accept_state(
        /* [in] */ Int64 sslRef);

    static CARAPI SSL_set_connect_state(
        /* [in] */ Int64 sslRef);

    static CARAPI SSL_set_verify(
        /* [in] */ Int64 sslRef,
        /* [in] */ Int32 mode);

    static CARAPI SSL_set_session(
        /* [in] */ Int64 sslRef,
        /* [in] */ Int64 sslSessionNativePointer);

    static CARAPI SSL_set_session_creation_enabled(
        /* [in] */ Int64 sslRef,
        /* [in] */ Boolean creationEnabled);

    static CARAPI SSL_set_tlsext_host_name(
        /* [in] */ Int64 sslRef,
        /* [in] */ const String& hostname);

    static CARAPI SSL_get_servername(
        /* [in] */ Int64 sslRef,
        /* [out] */ String* result);

    /**
     * Enables NPN for all SSL connections in the context.
     *
     * <p>For clients this causes the NPN extension to be included in the
     * ClientHello message.
     *
     * <p>For servers this causes the NPN extension to be included in the
     * ServerHello message. The NPN extension will not be included in the
     * ServerHello response if the client didn't include it in the ClientHello
     * request.
     *
     * <p>In either case the caller should pass a non-null byte array of NPN
     * protocols to {@link #SSL_do_handshake}.
     */
    static CARAPI SSL_CTX_enable_npn(
        /* [in] */ Int64 ssl_ctx_address);

    /**
     * Disables NPN for all SSL connections in the context.
     */
    static CARAPI SSL_CTX_disable_npn(
        /* [in] */ Int64 ssl_ctx_address);

    /**
     * For clients, sets the list of supported ALPN protocols in wire-format
     * (length-prefixed 8-bit strings).
     */
    static CARAPI SSL_set_alpn_protos(
        /* [in] */ Int64 ssl_address,
        /* [in] */ ArrayOf<Byte>* protos,
        /* [out] */ Int32* result);

    /**
     * Returns the selected ALPN protocol. If the server did not select a
     * protocol, {@code null} will be returned.
     */
    static CARAPI SSL_get0_alpn_selected(
        /* [in] */ Int64 ssl_address,
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * Returns the sslSessionNativePointer of the negotiated session. If this is
     * a server negotiation, supplying the {@code alpnProtocols} will enable
     * ALPN negotiation.
     */
    static CARAPI SSL_do_handshake(
        /* [in] */ Int64 ssl_address,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ISSLHandshakeCallbacks* shc,
        /* [in] */ Int32 timeoutMillis,
        /* [in] */ Boolean client_mode,
        /* [in] */ ArrayOf<Byte>* npnProtocols,
        /* [in] */ ArrayOf<Byte>* alpnProtocols,
        /* [out] */ Int64* result);

    /**
     * Returns the sslSessionNativePointer of the negotiated session. If this is
     * a server negotiation, supplying the {@code alpnProtocols} will enable
     * ALPN negotiation.
     */
    static CARAPI SSL_do_handshake_bio(
        /* [in] */ Int64 ssl_address,
        /* [in] */ Int64 sourceBioRef,
        /* [in] */ Int64 sinkBioRef,
        /* [in] */ ISSLHandshakeCallbacks* shc,
        /* [in] */ Boolean client_mode,
        /* [in] */ ArrayOf<Byte>* npnProtocols,
        /* [in] */ ArrayOf<Byte>* alpnProtocols,
        /* [out] */ Int64* result);

    static CARAPI SSL_get_npn_negotiated_protocol(
        /* [in] */ Int64 ssl_address,
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * Currently only intended for forcing renegotiation for testing.
     * Not used within OpenSSLSocketImpl.
     */
    static CARAPI SSL_renegotiate(
        /* [in] */ Int64 ssl_address);

    /**
     * Returns the local X509 certificate references. Must X509_free when done.
     */
    static CARAPI SSL_get_certificate(
        /* [in] */ Int64 ssl_address,
        /* [out, callee] */ ArrayOf<Int64>** result);

    /**
     * Returns the peer X509 certificate references. Must X509_free when done.
     */
    static CARAPI SSL_get_peer_cert_chain(
        /* [in] */ Int64 ssl_address,
        /* [out, callee] */ ArrayOf<Int64>** result);

    /**
     * Reads with the native SSL_read function from the encrypted data stream
     * @return -1 if error or the end of the stream is reached.
     */
    static CARAPI SSL_read(
        /* [in] */ Int64 ssl_address,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ISSLHandshakeCallbacks* shc,
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len,
        /* [in] */ Int32 readTimeoutMillis,
        /* [out] */ Int32* result);

    static CARAPI SSL_read_BIO(
        /* [in] */ Int64 ssl_address,
        /* [in] */ ArrayOf<Byte>* dest,
        /* [in] */ Int32 destOffset,
        /* [in] */ Int32 destLength,
        /* [in] */ Int64 sourceBioRef,
        /* [in] */ Int64 sinkBioRef,
        /* [in] */ ISSLHandshakeCallbacks* shc,
        /* [out] */ Int32* result);

    /**
     * Writes with the native SSL_write function to the encrypted data stream.
     */
    static CARAPI SSL_write(
        /* [in] */ Int64 ssl_address,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ISSLHandshakeCallbacks* shc,
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len,
        /* [in] */ Int32 writeTimeoutMillis);

    static CARAPI SSL_write_BIO(
        /* [in] */ Int64 ssl_address,
        /* [in] */ ArrayOf<Byte>* source,
        /* [in] */ Int32 length,
        /* [in] */ Int64 sinkBioRef,
        /* [in] */ ISSLHandshakeCallbacks* shc,
        /* [out] */ Int32* result);

    static CARAPI SSL_interrupt(
        /* [in] */ Int64 ssl_address);

    static CARAPI SSL_shutdown(
        /* [in] */ Int64 ssl_address,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ISSLHandshakeCallbacks* shc);

    static CARAPI SSL_shutdown_BIO(
        /* [in] */ Int64 ssl_address,
        /* [in] */ Int64 sourceBioRef,
        /* [in] */ Int64 sinkBioRef,
        /* [in] */ ISSLHandshakeCallbacks* shc);

    static CARAPI SSL_get_shutdown(
        /* [in] */ Int64 ssl_address,
        /* [out] */ Int32* result);

    static CARAPI SSL_free(
        /* [in] */ Int64 ssl_address);

    static CARAPI SSL_SESSION_session_id(
        /* [in] */ Int64 ssl_session_address,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI SSL_SESSION_get_time(
        /* [in] */ Int64 ssl_session_address,
        /* [out] */ Int64* result);

    static CARAPI SSL_SESSION_get_version(
        /* [in] */ Int64 ssl_session_address,
        /* [out] */ String* result);

    static CARAPI SSL_SESSION_cipher(
        /* [in] */ Int64 ssl_session_address,
        /* [out] */ String* result);

    static CARAPI SSL_SESSION_free(
        /* [in] */ Int64 ssl_session_address);

    static CARAPI I2d_SSL_SESSION(
        /* [in] */ Int64 ssl_session_address,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI D2i_SSL_SESSION(
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ Int64* result);

    static CARAPI ERR_peek_last_error(
        /* [out] */ Int64* error);

private:
    static CARAPI X509_NAME_hash(
        /* [in] */ IX500Principal* principal,
        /* [in] */ const String& algorithm,
        /* [out] */ Int32* result);

    static CARAPI_(Boolean) Clinit();

public:
    static AutoPtr<IMap> OPENSSL_TO_STANDARD_CIPHER_SUITES;
    static AutoPtr<IMap> STANDARD_TO_OPENSSL_CIPHER_SUITES;
    static AutoPtr< ArrayOf<String> > DEFAULT_PROTOCOLS;

private:
    static const Boolean sInitialized;
};

} // namespace Conscrypt
} // namespace Org

#endif //__NATIVECRYPTO_H__
