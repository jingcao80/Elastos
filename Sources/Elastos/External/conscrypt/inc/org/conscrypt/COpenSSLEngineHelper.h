
#ifndef __ORG_CONSCRYPT_COPENSSLENGINEHELPER_H__
#define __ORG_CONSCRYPT_COPENSSLENGINEHELPER_H__

#include "_Org_Conscrypt_COpenSSLEngineHelper.h"
#include <elastos/core/Singleton.h>

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLEngineHelper)
    , public Singleton
    , public IOpenSSLEngineHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetInstance(
        /* [in] */ const String& engine,
        /* [out] */ IOpenSSLEngine** result);
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLENGINEHELPER_H__
