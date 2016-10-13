
#ifndef __ORG_CONSCRYPT_COPENSSLDSAKEYFACTORY_H__
#define __ORG_CONSCRYPT_COPENSSLDSAKEYFACTORY_H__

#include "_Org_Conscrypt_COpenSSLDSAKeyFactory.h"
#include "OpenSSLDSAKeyFactory.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLDSAKeyFactory)
    , public OpenSSLDSAKeyFactory
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLDSAKEYFACTORY_H__
