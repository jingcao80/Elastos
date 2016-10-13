
#ifndef __ORG_CONSCRYPT_COPENSSLDHKEYFACTORY_H__
#define __ORG_CONSCRYPT_COPENSSLDHKEYFACTORY_H__

#include "_Org_Conscrypt_COpenSSLDHKeyFactory.h"
#include "OpenSSLDHKeyFactory.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLDHKeyFactory)
    , public OpenSSLDHKeyFactory
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLDHKEYFACTORY_H__
