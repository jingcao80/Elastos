
#ifndef __ORG_CONSCRYPT_COPENSSLECGROUPCONTEXTHELPER_H__
#define __ORG_CONSCRYPT_COPENSSLECGROUPCONTEXTHELPER_H__

#include "_Org_Conscrypt_COpenSSLECGroupContextHelper.h"
#include <Elastos.CoreLibrary.Security.h>
#include <elastos/core/Singleton.h>

using Elastos::Math::IBigInteger;
using Elastos::Security::Spec::IECParameterSpec;

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLECGroupContextHelper)
    , public Singleton
    , public IOpenSSLECGroupContextHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCurveByName(
        /* [in] */ const String& curveName,
        /* [out] */ IOpenSSLECGroupContext** result);

    CARAPI GetInstance(
        /* [in] */ Int32 type,
        /* [in] */ IBigInteger* p,
        /* [in] */ IBigInteger* a,
        /* [in] */ IBigInteger* b,
        /* [in] */ IBigInteger* x,
        /* [in] */ IBigInteger* y,
        /* [in] */ IBigInteger* n,
        /* [in] */ IBigInteger* h,
        /* [out] */ IOpenSSLECGroupContext** result);

    CARAPI GetInstance(
        /* [in] */ IECParameterSpec* params,
        /* [out] */ IOpenSSLECGroupContext** result);
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLECGROUPCONTEXTHELPER_H__
