
#ifndef __ORG_CONSCRYPT_OPENSSLDSAPARAMS_H__
#define __ORG_CONSCRYPT_OPENSSLDSAPARAMS_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Security.h>
#include <elastos/core/Object.h>
#include "_Org.Conscrypt.h"

using Elastos::Math::IBigInteger;
using Elastos::Security::Interfaces::IDSAParams;
using Elastos::Security::Spec::IAlgorithmParameterSpec;

namespace Org {
namespace Conscrypt {

class OpenSSLDSAParams
    : public IOpenSSLDSAParams
    , public IDSAParams
    , public IAlgorithmParameterSpec
    , public Object
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLDSAParams();

    CARAPI constructor(
        /* [in] */ IOpenSSLKey* key);

    CARAPI GetOpenSSLKey(
        /* [out] */ IOpenSSLKey** result);

    // @Override
    CARAPI GetG(
        /* [out] */ IBigInteger** result);

    // @Override
    CARAPI GetP(
        /* [out] */ IBigInteger** result);

    // @Override
    CARAPI GetQ(
        /* [out] */ IBigInteger** result);

    CARAPI HasParams(
        /* [out] */ Boolean* result);

    CARAPI GetY(
        /* [out] */ IBigInteger** result);

    CARAPI GetX(
        /* [out] */ IBigInteger** result);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    /*synchronized*/ CARAPI_(void) EnsureReadParams();

private:
    AutoPtr<IOpenSSLKey> mKey;

    Boolean mFetchedParams;

    AutoPtr<IBigInteger> mG;

    AutoPtr<IBigInteger> mP;

    AutoPtr<IBigInteger> mQ;

    AutoPtr<IBigInteger> mY;

    AutoPtr<IBigInteger> mX;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLDSAPARAMS_H__
