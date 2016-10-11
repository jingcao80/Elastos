
#include "_Org.Conscrypt.h"
#include "OpenSSLMessageDigestJDK.h"
#include "COpenSSLMessageDigestJDK.h"
#include "NativeCrypto.h"

using Elastos::Core::EIID_ICloneable;

namespace Org {
namespace Conscrypt {

//=========================================
// MD5::
//=========================================
static Int64 GetMD5_EVP_MD()
{
    Int64 md = 0;
    NativeCrypto::EVP_get_digestbyname(String("md5"), &md);
    return md;
}
Int64 MD5::EVP_MD = GetMD5_EVP_MD();

static Int32 GetMD5_SIZE()
{
    Int32 size = 0;
    NativeCrypto::EVP_MD_size(MD5::EVP_MD, &size);
    return size;
}
Int32 MD5::SIZE = GetMD5_SIZE();

MD5::MD5()
{
    OpenSSLMessageDigestJDK::constructor(EVP_MD, SIZE);
}

//=========================================
// SHA1::
//=========================================
static Int64 GetSHA1_EVP_MD()
{
    Int64 md = 0;
    NativeCrypto::EVP_get_digestbyname(String("sha1"), &md);
    return md;
}
Int64 SHA1::EVP_MD = GetSHA1_EVP_MD();

static Int32 GetSHA1_SIZE()
{
    Int32 size = 0;
    NativeCrypto::EVP_MD_size(SHA1::EVP_MD, &size);
    return size;
}
Int32 SHA1::SIZE = GetSHA1_SIZE();

SHA1::SHA1()
{
    OpenSSLMessageDigestJDK::constructor(EVP_MD, SIZE);
}

//=========================================
// SHA224::
//=========================================
static Int64 GetSHA224_EVP_MD()
{
    Int64 md = 0;
    NativeCrypto::EVP_get_digestbyname(String("sha224"), &md);
    return md;
}
Int64 SHA224::EVP_MD = GetSHA224_EVP_MD();

static Int32 GetSHA224_SIZE()
{
    Int32 size = 0;
    NativeCrypto::EVP_MD_size(SHA224::EVP_MD, &size);
    return size;
}
Int32 SHA224::SIZE = GetSHA224_SIZE();

SHA224::SHA224()
{
    OpenSSLMessageDigestJDK::constructor(EVP_MD, SIZE);
}

//=========================================
// SHA256::
//=========================================
static Int64 GetSHA256_EVP_MD()
{
    Int64 md = 0;
    NativeCrypto::EVP_get_digestbyname(String("sha256"), &md);
    return md;
}
Int64 SHA256::EVP_MD = GetSHA256_EVP_MD();

static Int32 GetSHA256_SIZE()
{
    Int32 size = 0;
    NativeCrypto::EVP_MD_size(SHA256::EVP_MD, &size);
    return size;
}
Int32 SHA256::SIZE = GetSHA256_SIZE();

SHA256::SHA256()
{
    OpenSSLMessageDigestJDK::constructor(EVP_MD, SIZE);
}

//=========================================
// SHA384::
//=========================================
static Int64 GetSHA384_EVP_MD()
{
    Int64 md = 0;
    NativeCrypto::EVP_get_digestbyname(String("sha384"), &md);
    return md;
}
Int64 SHA384::EVP_MD = GetSHA384_EVP_MD();

static Int32 GetSHA384_SIZE()
{
    Int32 size = 0;
    NativeCrypto::EVP_MD_size(SHA384::EVP_MD, &size);
    return size;
}
Int32 SHA384::SIZE = GetSHA384_SIZE();

SHA384::SHA384()
{
    OpenSSLMessageDigestJDK::constructor(EVP_MD, SIZE);
}

//=========================================
// SHA512::
//=========================================
static Int64 GetSHA512_EVP_MD()
{
    Int64 md = 0;
    NativeCrypto::EVP_get_digestbyname(String("sha512"), &md);
    return md;
}
Int64 SHA512::EVP_MD = GetSHA512_EVP_MD();

static Int32 GetSHA512_SIZE()
{
    Int32 size = 0;
    NativeCrypto::EVP_MD_size(SHA512::EVP_MD, &size);
    return size;
}
Int32 SHA512::SIZE = GetSHA512_SIZE();

SHA512::SHA512()
{
    OpenSSLMessageDigestJDK::constructor(EVP_MD, SIZE);
}

//=========================================
// OpenSSLMessageDigestJDK::
//=========================================
CAR_INTERFACE_IMPL_2(OpenSSLMessageDigestJDK, Object, IOpenSSLMessageDigestJDK, ICloneable)

ECode OpenSSLMessageDigestJDK::constructor(
    /* [in] */ Int64 evp_md,
    /* [in] */ Int32 size)
{
    mEvp_md = evp_md;
    mSize = size;
    mSingleByte = ArrayOf<Byte>::Alloc(1);

    ResetContext();
    return NOERROR;
}

ECode OpenSSLMessageDigestJDK::constructor(
    /* [in] */ Int64 evp_md,
    /* [in] */ Int32 size,
    /* [in] */ IOpenSSLDigestContext* ctx)
{
    mEvp_md = evp_md;
    mSize = size;
    mCtx = ctx;
    mSingleByte = ArrayOf<Byte>::Alloc(1);
    return NOERROR;
}

void OpenSSLMessageDigestJDK::ResetContext()
{
    Int64 ctx = 0;
    NativeCrypto::EVP_MD_CTX_create(&ctx);
    assert(0 && "TODO");
    AutoPtr<IOpenSSLDigestContext> ctxLocal;// = new OpenSSLDigestContext(ctx);
    NativeCrypto::EVP_MD_CTX_init(ctxLocal);
    Int32 res = 0;
    NativeCrypto::EVP_DigestInit(ctxLocal, mEvp_md, &res);
    mCtx = ctxLocal;
}

ECode OpenSSLMessageDigestJDK::EngineReset()
{
    ResetContext();
    return NOERROR;
}

ECode OpenSSLMessageDigestJDK::EngineGetDigestLength(
    /* [out] */ Int32* len)
{
    VALIDATE_NOT_NULL(len)
    *len = mSize;
    return NOERROR;
}

ECode OpenSSLMessageDigestJDK::EngineUpdate(
    /* [in] */ Byte input)
{
    (*mSingleByte)[0] = input;
    EngineUpdate(mSingleByte, 0, 1);
    return NOERROR;
}

ECode OpenSSLMessageDigestJDK::EngineUpdate(
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 len)
{
    NativeCrypto::EVP_DigestUpdate(mCtx, input, offset, len);
    return NOERROR;
}

ECode OpenSSLMessageDigestJDK::EngineDigest(
    /* [out, callee] */ ArrayOf<Byte>** ed)
{
    VALIDATE_NOT_NULL(ed)
    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(mSize);
    Int32 final = 0;
    NativeCrypto::EVP_DigestFinal(mCtx, result, 0, &final);
    ResetContext();
    *ed = result;
    return NOERROR;
}

ECode OpenSSLMessageDigestJDK::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    Int64 ctx = 0;
    NativeCrypto::EVP_MD_CTX_create(&ctx);
    assert(0 && "TODO");
    AutoPtr<IOpenSSLDigestContext> ctxCopy;// = new OpenSSLDigestContext(ctx);
    NativeCrypto::EVP_MD_CTX_init(ctxCopy);
    Int32 res = 0;
    NativeCrypto::EVP_MD_CTX_copy(ctxCopy, mCtx, &res);
    AutoPtr<IOpenSSLMessageDigestJDK> result;
    COpenSSLMessageDigestJDK::New(mEvp_md, mSize, ctxCopy, (IOpenSSLMessageDigestJDK**)&result);
    *object = ICloneable::Probe(result);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
