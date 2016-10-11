
#ifndef __ORG_CONSCRYPT_COPENSSLRSAKEYFACTORY_H__
#define __ORG_CONSCRYPT_COPENSSLRSAKEYFACTORY_H__

#include "_Org_Conscrypt_COpenSSLRSAKeyFactory.h"
#include "OpenSSLRSAKeyFactory.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLRSAKeyFactory)
    , public OpenSSLRSAKeyFactory
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLRSAKEYFACTORY_H__
