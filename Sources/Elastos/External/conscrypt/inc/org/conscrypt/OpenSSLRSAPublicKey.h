
#ifndef __ORG_CONSCRYPT_OPENSSLRSAPUBLICKEY_H__
#define __ORG_CONSCRYPT_OPENSSLRSAPUBLICKEY_H__

#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::Security::IPublicKey;
using Elastos::Security::IKey;
using Elastos::Security::Interfaces::IRSAPublicKey;
using Elastos::Security::Spec::IRSAPublicKeySpec;
using Elastos::Math::IBigInteger;
using Elastos::IO::IObjectInputStream;
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::ISerializable;

namespace Org {
namespace Conscrypt {

class OpenSSLRSAPublicKey
    : public Object
    , public IRSAPublicKey
    , public IPublicKey
    , public IKey
    , public ISerializable
    , public IOpenSSLKeyHolder
    , public IOpenSSLRSAPublicKey
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IOpenSSLKey* key);

    CARAPI GetOpenSSLKey(
        /* [out] */ IOpenSSLKey** result);

    CARAPI constructor(
        /* [in] */ IRSAPublicKeySpec* spec);

    static CARAPI GetInstance(
        /* [in] */ IRSAPublicKey* rsaPublicKey,
        /* [out] */ IOpenSSLKey** result);

    CARAPI GetAlgorithm(
        /* [out] */ String* result);

    CARAPI GetFormat(
        /* [out] */ String* result);

    CARAPI GetEncoded(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetModulus(
        /* [out] */ IBigInteger** result);

    CARAPI GetPublicExponent(
        /* [out] */ IBigInteger** result);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    void EnsureReadParams();

    void ReadObject(
        /* [in] */ IObjectInputStream* stream);

    void WriteObject(
        /* [in] */ IObjectOutputStream* stream);

private:
    static Int64 mSerialVersionUID;

    AutoPtr<IOpenSSLKey> mKey;

    AutoPtr<IBigInteger> mPublicExponent;

    AutoPtr<IBigInteger> mModulus;

    Boolean mFetchedParams;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLRSAPUBLICKEY_H__