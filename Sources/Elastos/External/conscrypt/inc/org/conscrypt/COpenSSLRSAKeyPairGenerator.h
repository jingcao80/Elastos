
#ifndef __ORG_CONSCRYPT_COPENSSLRSAKEYPAIRGENERATOR_H__
#define __ORG_CONSCRYPT_COPENSSLRSAKEYPAIRGENERATOR_H__

#include "_Org_Conscrypt_COpenSSLRSAKeyPairGenerator.h"
#include "OpenSSLRSAKeyPairGenerator.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLRSAKeyPairGenerator)
    , public OpenSSLRSAKeyPairGenerator
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLRSAKEYPAIRGENERATOR_H__
