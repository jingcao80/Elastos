
#ifndef __ORG_CONSCRYPT_CABSTRACTSESSIONCONTEXT_H__
#define __ORG_CONSCRYPT_CABSTRACTSESSIONCONTEXT_H__

#include "_Org_Conscrypt_CAbstractSessionContext.h"
#include "AbstractSessionContext.h"

namespace Org {
namespace Conscrypt {

CarClass(CAbstractSessionContext)
    , public AbstractSessionContext
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_CABSTRACTSESSIONCONTEXT_H__
