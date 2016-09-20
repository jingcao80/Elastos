
#ifndef __ORG_CONSCRYPT_OPENSSLRSAPRIVATECRTKEY_H__
#define __ORG_CONSCRYPT_OPENSSLRSAPRIVATECRTKEY_H__

#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::IO::IObjectInputStream;
using Elastos::IO::IObjectOutputStream;
using Elastos::Math::IBigInteger;
using Elastos::Security::Interfaces::IRSAPrivateCrtKey;
using Elastos::Security::Spec::IRSAPrivateCrtKeySpec;

namespace Org {
namespace Conscrypt {

class OpenSSLRSAPrivateCrtKey
    : public Object // public OpenSSLRSAPrivateKey
    , public IRSAPrivateCrtKey
{
public:
    OpenSSLRSAPrivateCrtKey(
        /* [in] */ IOpenSSLKey* key);

    OpenSSLRSAPrivateCrtKey(
        /* [in] */ IOpenSSLKey* key,
        /* [in] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >* params);

    OpenSSLRSAPrivateCrtKey(
        /* [in] */ IRSAPrivateCrtKeySpec* rsaKeySpec);

    static CARAPI GetInstance(
        /* [in] */ IRSAPrivateCrtKey* rsaPrivateKey,
        /* [out] */ IOpenSSLKey** result);

    void ReadParams(
        /* [in] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >* params);

    CARAPI GetPublicExponent(
        /* [out] */ IBigInteger** result);

    CARAPI GetPrimeP(
        /* [out] */ IBigInteger** result);

    CARAPI GetPrimeQ(
        /* [out] */ IBigInteger** result);

    CARAPI GetPrimeExponentP(
        /* [out] */ IBigInteger** result);

    CARAPI GetPrimeExponentQ(
        /* [out] */ IBigInteger** result);

    CARAPI GetCrtCoefficient(
        /* [out] */ IBigInteger** result);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    static AutoPtr<IOpenSSLKey> Init(
        /* [in] */ IRSAPrivateCrtKeySpec* rsaKeySpec);

    void ReadObject(
        /* [in] */ IObjectInputStream* stream);

    void WriteObject(
        /* [in] */ IObjectOutputStream* stream);

private:
    static Int64 mSerialVersionUID;

    AutoPtr<IBigInteger> mPublicExponent;

    AutoPtr<IBigInteger> mPrimeP;

    AutoPtr<IBigInteger> mPrimeQ;

    AutoPtr<IBigInteger> mPrimeExponentP;

    AutoPtr<IBigInteger> mPrimeExponentQ;

    AutoPtr<IBigInteger> mCrtCoefficient;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLRSAPRIVATECRTKEY_H__