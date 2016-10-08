
#ifndef __ORG_CONSCRYPT_OPENSSLDHPRIVATEKEY_H__
#define __ORG_CONSCRYPT_OPENSSLDHPRIVATEKEY_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Security.h>
#include <elastos/core/Object.h>
#include "_Org.Conscrypt.h"
#include "org/conscrypt/OpenSSLKey.h"

using Elastos::IO::IObjectInputStream;
using Elastos::IO::IObjectOutputStream;
using Elastos::Math::IBigInteger;
using Elastos::Security::IPrivateKey;
using Elastosx::Crypto::Interfaces::IDHKey;
using Elastosx::Crypto::Interfaces::IDHPrivateKey;
using Elastosx::Crypto::Spec::IDHParameterSpec;
using Elastosx::Crypto::Spec::IDHPrivateKeySpec;

namespace Org {
namespace Conscrypt {

class OpenSSLDHPrivateKey
    : public IOpenSSLDHPrivateKey
    , public IDHPrivateKey
    , public IDHKey
    , public IPrivateKey
    , public IOpenSSLKeyHolder
    , public Object
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLDHPrivateKey();

    CARAPI constructor(
        /* [in] */ IOpenSSLKey* key);

    CARAPI constructor(
        /* [in] */ IDHPrivateKeySpec* dhKeySpec);

    // @Override
    CARAPI GetOpenSSLKey(
        /* [out] */ IOpenSSLKey** result);

    static CARAPI GetInstance(
        /* [in] */ IDHPrivateKey* dhPrivateKey,
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
        /* [out] */ IDHParameterSpec** result);

    // @Override
    CARAPI GetX(
        /* [out] */ IBigInteger** result);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

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
    static const Int64 serialVersionUID = -7321023036951606638L;

    /*transient*/ AutoPtr<OpenSSLKey> mKey;

    /** base prime */
    /*transient*/ AutoPtr<ArrayOf<Byte> > mP;

    /** generator */
    /*transient*/ AutoPtr<ArrayOf<Byte> > mG;

    /** private key */
    /*transient*/ AutoPtr<ArrayOf<Byte> > mX;

    /*transient*/ Object mParamsLock; // = new Object();

    /*transient*/ Boolean mReadParams;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLDHPRIVATEKEY_H__
