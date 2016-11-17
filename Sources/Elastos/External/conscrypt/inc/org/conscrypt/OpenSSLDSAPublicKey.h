
#ifndef __ORG_CONSCRYPT_OPENSSLDSAPUBLICKEY_H__
#define __ORG_CONSCRYPT_OPENSSLDSAPUBLICKEY_H__

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
using Elastos::Security::Interfaces::IDSAPublicKey;
using Elastos::Security::IPublicKey;
using Elastos::Security::IKey;
using Elastos::Security::Spec::IDSAPublicKeySpec;

namespace Org {
namespace Conscrypt {

class OpenSSLDSAPublicKey
    : public IOpenSSLDSAPublicKey
    , public IDSAPublicKey
    , public IDSAKey
    , public IKey
    , public IPublicKey
    , public IOpenSSLKeyHolder
    , public Object
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLDSAPublicKey();

    CARAPI constructor(
        /* [in] */ IOpenSSLKey* key);

    CARAPI constructor(
        /* [in] */ IDSAPublicKeySpec* dsaKeySpec);

    // @Override
    CARAPI GetOpenSSLKey(
        /* [out] */ IOpenSSLKey** result);

    static CARAPI GetInstance(
        /* [in] */ IDSAPublicKey* dsaPublicKey,
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
    CARAPI GetY(
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
    static const Int64 serialVersionUID = 5238609500353792232L;

    /*transient*/ AutoPtr<OpenSSLKey> mKey;

    /*transient*/ AutoPtr<OpenSSLDSAParams> mParams;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLDSAPUBLICKEY_H__
