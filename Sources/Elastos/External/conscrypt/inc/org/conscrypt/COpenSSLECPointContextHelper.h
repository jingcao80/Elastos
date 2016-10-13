
#ifndef __ORG_CONSCRYPT_COPENSSLECPOINTCONTEXTHELPER_H__
#define __ORG_CONSCRYPT_COPENSSLECPOINTCONTEXTHELPER_H__

#include "_Org_Conscrypt_COpenSSLECPointContextHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Security::Spec::IECPoint;

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLECPointContextHelper)
    , public Singleton
    , public IOpenSSLECPointContextHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetInstance(
        /* [in] */ Int32 curveType,
        /* [in] */ IOpenSSLECGroupContext* group,
        /* [in] */ IECPoint* javaPoint,
        /* [out] */ IOpenSSLECPointContext** result);
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLECPOINTCONTEXTHELPER_H__
