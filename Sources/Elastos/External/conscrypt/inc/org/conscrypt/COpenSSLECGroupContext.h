
#ifndef __ORG_CONSCRYPT_COpenSSLECGroupContext_H__
#define __ORG_CONSCRYPT_COpenSSLECGroupContext_H__

#include "_Org_Conscrypt_COpenSSLECGroupContext.h"
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Security.h>
#include <elastos/core/Object.h>

using Elastos::Math::IBigInteger;
using Elastos::Security::Spec::IECParameterSpec;

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLECGroupContext)
    , public IOpenSSLECGroupContext
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    COpenSSLECGroupContext();

    ~COpenSSLECGroupContext();

    CARAPI constructor(
        /* [in] */ Int64 groupCtx);

    static CARAPI GetCurveByName(
        /* [in] */ String curveName,
        /* [out] */ IOpenSSLECGroupContext** result);

    static CARAPI GetInstance(
        /* [in] */ Int32 type,
        /* [in] */ IBigInteger* p,
        /* [in] */ IBigInteger* a,
        /* [in] */ IBigInteger* b,
        /* [in] */ IBigInteger* x,
        /* [in] */ IBigInteger* y,
        /* [in] */ IBigInteger* n,
        /* [in] */ IBigInteger* h,
        /* [out] */ IOpenSSLECGroupContext** result);

    static CARAPI GetInstance(
        /* [in] */ IECParameterSpec* params,
        /* [out] */ IOpenSSLECGroupContext** result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI GetContext(
        /* [out] */ Int64* result);

    CARAPI GetECParameterSpec(
        /* [out] */ IECParameterSpec** result);

private:
    Int64 mGroupCtx;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COpenSSLECGroupContext_H__
