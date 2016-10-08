
#ifndef __ORG_CONSCRYPT_COPENSSLECPUBLICKEY_H__
#define __ORG_CONSCRYPT_COPENSSLECPUBLICKEY_H__

#include "_Org_Conscrypt_COpenSSLECPublicKey.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Security.h>
#include <Elastos.CoreLibrary.Extensions.h>
#include <elastos/core/Object.h>
#include "org/conscrypt/OpenSSLKey.h"

using Elastos::IO::IObjectInputStream;
using Elastos::IO::IObjectOutputStream;
using Elastos::Security::Interfaces::IECPublicKey;
using Elastos::Security::Spec::IECParameterSpec;
using Elastos::Security::Spec::IECPoint;
using Elastos::Security::Spec::IECPublicKeySpec;

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLECPublicKey)
    , public IOpenSSLECPublicKey
    , public IECPublicKey
    , public IOpenSSLKeyHolder
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IOpenSSLECGroupContext* group,
        /* [in] */ IOpenSSLKey* key);

    CARAPI constructor(
        /* [in] */ IOpenSSLKey* key);

    CARAPI constructor(
        /* [in] */ IECPublicKeySpec* ecKeySpec);

    static CARAPI GetInstance(
        /* [in] */ IECPublicKey* ecPublicKey,
        /* [out] */ IOpenSSLKey** result);

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
    CARAPI GetParams(
        /* [out] */ IECParameterSpec** result);

    // @Override
    CARAPI GetW(
        /* [out] */ IECPoint** result);

    // @Override
    CARAPI GetOpenSSLKey(
        /* [out] */ IOpenSSLKey** result);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    CARAPI_(AutoPtr<IECPoint>) GetPublicKey();

    CARAPI_(void) ReadObject(
        /* [in] */ IObjectInputStream* stream);

    CARAPI WriteObject(
        /* [in] */ IObjectOutputStream* stream);

protected:
    /*transient*/ AutoPtr<OpenSSLKey> mKey;

    /*transient*/ AutoPtr<IOpenSSLECGroupContext> mGroup;

private:
    static const Int64 serialVersionUID = 3215842926808298020L;

    static const String ALGORITHM; // = "EC";
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLECPUBLICKEY_H__
