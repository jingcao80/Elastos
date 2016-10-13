
#ifndef __ORG_CONSCRYPT_CKEYMANAGERFACTORYIMPL_H__
#define __ORG_CONSCRYPT_CKEYMANAGERFACTORYIMPL_H__

#include "_Org_Conscrypt_CKeyManagerFactoryImpl.h"
#include "KeyManagerFactoryImpl.h"

namespace Org {
namespace Conscrypt {

CarClass(CKeyManagerFactoryImpl)
    , public KeyManagerFactoryImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_CKEYMANAGERFACTORYIMPL_H__
