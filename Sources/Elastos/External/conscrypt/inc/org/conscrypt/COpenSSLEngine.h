
#ifndef __ORG_CONSCRYPT_COPENSSLENGINE_H__
#define __ORG_CONSCRYPT_COPENSSLENGINE_H__

#include "_Org_Conscrypt_COpenSSLEngine.h"
#include "OpenSSLEngine.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLEngine)
    , public OpenSSLEngine
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLENGINE_H__
