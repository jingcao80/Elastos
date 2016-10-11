
#ifndef __ORG_CONSCRYPT_OPENSSLRSAPRIVATEKEY_H__
#define __ORG_CONSCRYPT_OPENSSLRSAPRIVATEKEY_H__

#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::Security::Interfaces::IRSAPrivateKey;
using Elastos::Security::Spec::IRSAPrivateKeySpec;
using Elastos::Math::IBigInteger;
using Elastos::IO::IObjectInputStream;
using Elastos::IO::IObjectOutputStream;

namespace Org {
namespace Conscrypt {

class OpenSSLRSAPrivateKey
    : public Object
    , public IRSAPrivateKey
    , public IOpenSSLKeyHolder
    , public IOpenSSLRSAPrivateKey
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IOpenSSLKey* key);

    CARAPI constructor(
        /* [in] */ IOpenSSLKey* key,
        /* [in] */ ArrayOf<Handle32>* params);

    CARAPI constructor(
        /* [in] */ IRSAPrivateKeySpec* rsaKeySpec);

    CARAPI GetOpenSSLKey(
        /* [out] */ IOpenSSLKey** result);

    static AutoPtr<IOpenSSLRSAPrivateKey> GetInstance(
        /* [in] */ IOpenSSLKey* key);

    static AutoPtr<IOpenSSLKey> WrapPlatformKey(
        /* [in] */ IRSAPrivateKey* rsaPrivateKey);

    static AutoPtr<IOpenSSLKey> GetInstance(
        /* [in] */ IRSAPrivateKey* rsaPrivateKey);

    CARAPI EnsureReadParams();

    CARAPI ReadParams(
        /* [in] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >* params);

    CARAPI GetPrivateExponent(
        /* [out] */ IBigInteger** result);

    CARAPI GetModulus(
        /* [out] */ IBigInteger** result);

    CARAPI GetEncoded(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetFormat(
        /* [out] */ String* result);

    CARAPI GetAlgorithm(
        /* [out] */ String* result);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    static AutoPtr<IOpenSSLKey> Init(
        /* [in] */ IRSAPrivateKeySpec* rsaKeySpec);

    void ReadObject(
        /* [in] */ IObjectInputStream* stream);

    void WriteObject(
        /* [in] */ IObjectOutputStream* stream);

public:
    AutoPtr<IOpenSSLKey> mKey;

    Boolean mFetchedParams;

    AutoPtr<IBigInteger> mModulus;

    AutoPtr<IBigInteger> mPrivateExponent;

private:
    static Int64 mSerialVersionUID;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLRSAPRIVATEKEY_H__