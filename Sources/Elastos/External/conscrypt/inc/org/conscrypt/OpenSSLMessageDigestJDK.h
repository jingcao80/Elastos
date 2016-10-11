
#ifndef __ORG_CONSCRYPT_OPENSSLMESSAGEDIGESTJDK_H__
#define __ORG_CONSCRYPT_OPENSSLMESSAGEDIGESTJDK_H__

#include "Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::ICloneable;

namespace Org {
namespace Conscrypt {

/**
 * Implements the JDK MessageDigest interface using OpenSSL's EVP API.
 */
class OpenSSLMessageDigestJDK
    : public Object // public MessageDigestSpi
    , public IOpenSSLMessageDigestJDK
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI Clone(
        /* [out] */ IInterface** object);

    CARAPI EngineReset();

    CARAPI EngineGetDigestLength(
        /* [out] */ Int32* len);

    CARAPI EngineUpdate(
        /* [in] */ Byte input);

    CARAPI EngineUpdate(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len);

    CARAPI EngineDigest(
        /* [out, callee] */ ArrayOf<Byte>** ed);

    /**
     * Creates a new OpenSSLMessageDigest instance for the given algorithm name.
     */
    CARAPI constructor(
        /* [in] */ Int64 evp_md,
        /* [in] */ Int32 size);

    CARAPI constructor(
        /* [in] */ Int64 evp_md,
        /* [in] */ Int32 size,
        /* [in] */ IOpenSSLDigestContext* ctx);

    void ResetContext();

private:
    AutoPtr<IOpenSSLDigestContext> mCtx;

    /**
     * Holds the EVP_MD for the hashing algorithm, e.g. EVP_get_digestbyname("sha1");
     */
    Int64 mEvp_md;

    /**
     * Holds the output size of the message digest.
     */
    Int32 mSize;

    /**
     * Holds a dummy buffer for writing single bytes to the digest.
     */
    AutoPtr<ArrayOf<Byte> > mSingleByte;
};

class MD5
    : public OpenSSLMessageDigestJDK
{
public:
    MD5();

public:
    static Int64 EVP_MD;
    static Int32 SIZE;
};

class SHA1
    : public OpenSSLMessageDigestJDK
{
public:
    SHA1();

public:
    static Int64 EVP_MD;
    static Int32 SIZE;
};

class SHA224
    : public OpenSSLMessageDigestJDK
{
public:
    SHA224();

public:
    static Int64 EVP_MD;
    static Int32 SIZE;
};

class SHA256
    : public OpenSSLMessageDigestJDK
{
public:
    SHA256();

public:
    static Int64 EVP_MD;
    static Int32 SIZE;
};

class SHA384
    : public OpenSSLMessageDigestJDK
{
public:
    SHA384();

public:
    static Int64 EVP_MD;
    static Int32 SIZE;
};

class SHA512
    : public OpenSSLMessageDigestJDK
{
public:
    SHA512();

public:
    static Int64 EVP_MD;
    static Int32 SIZE;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLMESSAGEDIGESTJDK_H__