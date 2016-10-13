
#ifndef __ORG_CONSCRYPT_COPENSSLECKEYFACTORY_H__
#define __ORG_CONSCRYPT_COPENSSLECKEYFACTORY_H__

#include "_Org_Conscrypt_COpenSSLECKeyFactory.h"
#include "OpenSSLECKeyFactory.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLECKeyFactory)
    , public OpenSSLECKeyFactory
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLECKEYFACTORY_H__
