
#ifndef __ORG_CONSCRYPT_COPENSSLENGINEIMPL_H__
#define __ORG_CONSCRYPT_COPENSSLENGINEIMPL_H__

#include "_Org_Conscrypt_COpenSSLEngineImpl.h"
#include "OpenSSLEngineImpl.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLEngineImpl)
    , public OpenSSLEngineImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLENGINEIMPL_H__
