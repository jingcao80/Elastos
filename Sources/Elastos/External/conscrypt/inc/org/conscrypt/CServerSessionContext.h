
#ifndef __ORG_CONSCRYPT_CSERVERSESSIONCONTEXT_H__
#define __ORG_CONSCRYPT_CSERVERSESSIONCONTEXT_H__

#include "_Org_Conscrypt_CServerSessionContext.h"
#include "ServerSessionContext.h"

namespace Org {
namespace Conscrypt {

CarClass(CServerSessionContext)
    , public ServerSessionContext
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_CSERVERSESSIONCONTEXT_H__
