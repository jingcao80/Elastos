
#ifndef __ORG_CONSCRYPT_OPENSSLMAC_H__
#define __ORG_CONSCRYPT_OPENSSLMAC_H__

#include "Org.Conscrypt.h"

using Elastos::Security::Provider;

namespace Org {
namespace Conscrypt {

class OpenSSLMac
    : public MacSpi
{
public:
    class HmacMD5
        : public OpenSSLMac
    {
    public:
        HmacMD5();

    private:
        static Int64 EVP_MD;
        static Int32 SIZE;
    };

    class HmacSHA1
        : public OpenSSLMac
    {
    public:
        HmacSHA1();

    private:
        static Int64 EVP_MD;
        static Int32 SIZE;
    };

    class HmacSHA224
        : public OpenSSLMac
    {
    public:
        HmacSHA224();

    private:
        static Int64 EVP_MD;
        static Int32 SIZE;
    };

    class HmacSHA256
        : public OpenSSLMac
    {
    public:
        HmacSHA256();

    private:
        static Int64 EVP_MD;
        static Int32 SIZE;
    };

    class HmacSHA384
        : public OpenSSLMac
    {
    public:
        HmacSHA384();

    private:
        static Int64 EVP_MD;
        static Int32 SIZE;
    };

    class HmacSHA512
        : public OpenSSLMac
    {
    public:
        HmacSHA512();

    private:
        static Int64 EVP_MD;
        static Int32 SIZE;
    };

protected:
    Int32 EngineGetMacLength();

    void EngineInit(
        /* [in] */ IKey* key,
        /* [in] */ IAlgorithmParameterSpec* params);

    void EngineUpdate(
        /* [in] */ Byte input);

    void EngineUpdate(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len);

    AutoPtr<ArrayOf<Byte> > EngineDoFinal();

    void EngineReset();

private:
    OpenSSLMac(
        /* [in] */ Int64 evp_md,
        /* [in] */ Int32 size,
        /* [in] */ Int32 evp_pkey_type);

    void ResetContext();

private:
    AutoPtr<IOpenSSLDigestContext> ctx;

    /**
     * Holds the EVP_MD for the hashing algorithm, e.g.
     * EVP_get_digestbyname("sha1");
     */
    Int64 mEvp_md;

    /**
     * The key type of the secret key.
     */
    Int32 mEvp_pkey_type;

    /**
     * The secret key used in this keyed MAC.
     */
    AutoPtr<IOpenSSLKey> mMacKey;

    /**
     * Holds the output size of the message digest.
     */
    Int32 mSize;

    /**
     * Holds a dummy buffer for writing single bytes to the digest.
     */
    AutoPtr<ArrayOf<Byte> > mSingleByte;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLMAC_H__