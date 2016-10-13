
#ifndef __ORG_CONSCRYPT_CKEYMANAGERIMPL_H__
#define __ORG_CONSCRYPT_CKEYMANAGERIMPL_H__

#include "_Org_Conscrypt_CKeyManagerImpl.h"
#include "KeyManagerImpl.h"

namespace Org {
namespace Conscrypt {

CarClass(CKeyManagerImpl)
    , public KeyManagerImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_CKEYMANAGERIMPL_H__
