
#ifndef __ORG_CONSCRYPT_COPENSSLBIOSINKHELPER_H__
#define __ORG_CONSCRYPT_COPENSSLBIOSINKHELPER_H__

#include "_Org_Conscrypt_COpenSSLBIOSinkHelper.h"
#include <elastos/core/Singleton.h>

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLBIOSinkHelper)
    , public Singleton
    , public IOpenSSLBIOSinkHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Create(
        /* [out] */ IOpenSSLBIOSink** result);
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLBIOSINKHELPER_H__
