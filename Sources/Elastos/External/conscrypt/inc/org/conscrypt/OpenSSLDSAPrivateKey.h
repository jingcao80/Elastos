
#ifndef __ORG_CONSCRYPT_OPENSSLDSAPRIVATEKEY_H__
#define __ORG_CONSCRYPT_OPENSSLDSAPRIVATEKEY_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Security.h>
#include <elastos/core/Object.h>
#include "_Org.Conscrypt.h"
#include "org/conscrypt/OpenSSLDSAParams.h"
#include "org/conscrypt/OpenSSLKey.h"

using Elastos::IO::IObjectInputStream;
using Elastos::IO::IObjectOutputStream;
using Elastos::Math::IBigInteger;
using Elastos::Security::Interfaces::IDSAKey;
using Elastos::Security::Interfaces::IDSAParams;
using Elastos::Security::Interfaces::IDSAPrivateKey;
using Elastos::Security::IPrivateKey;
using Elastos::Security::Spec::IDSAPrivateKeySpec;

namespace Org {
namespace Conscrypt {

class OpenSSLDSAPrivateKey
    : public IOpenSSLDSAPrivateKey
    , public IDSAPrivateKey
    , public IDSAKey
    , public IPrivateKey
    , public IOpenSSLKeyHolder
    , public Object
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLDSAPrivateKey();

    CARAPI constructor(
        /* [in] */ IOpenSSLKey* key);

    CARAPI constructor(
        /* [in] */ IDSAPrivateKeySpec* dsaKeySpec);

    // @Override
    CARAPI GetOpenSSLKey(
        /* [out] */ IOpenSSLKey** result);

    static CARAPI GetInstance(
        /* [in] */ IDSAPrivateKey* dsaPrivateKey,
        /* [out] */ IOpenSSLKey** result);

    static CARAPI WrapPlatformKey(
        /* [in] */ IDSAPrivateKey* dsaPrivateKey,
        /* [out] */ IOpenSSLKey** result);

    // @Override
    CARAPI GetParams(
        /* [out] */ IDSAParams** result);

    // @Override
    CARAPI GetAlgorithm(
        /* [out] */ String* result);

    // @Override
    CARAPI GetFormat(
        /* [out] */ String* result);

    // @Override
    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** result);

    // @Override
    CARAPI GetX(
        /* [out] */ IBigInteger** result);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    CARAPI_(void) EnsureReadParams();

    CARAPI_(void) ReadObject(
        /* [in] */ IObjectInputStream* stream);

    CARAPI WriteObject(
        /* [in] */ IObjectOutputStream* stream);

private:
    static const Int64 serialVersionUID = 6524734576187424628L;

    /*transient*/ AutoPtr<OpenSSLKey> mKey;

    /*transient*/ AutoPtr<OpenSSLDSAParams> mParams;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLDSAPRIVATEKEY_H__
