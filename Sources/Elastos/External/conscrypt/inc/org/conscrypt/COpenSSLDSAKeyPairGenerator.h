
#ifndef __ORG_CONSCRYPT_COPENSSLDSAKEYPAIRGENERATOR_H__
#define __ORG_CONSCRYPT_COPENSSLDSAKEYPAIRGENERATOR_H__

#include "_Org_Conscrypt_COpenSSLDSAKeyPairGenerator.h"
#include "OpenSSLDSAKeyPairGenerator.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLDSAKeyPairGenerator)
    , public OpenSSLDSAKeyPairGenerator
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLDSAKEYPAIRGENERATOR_H__
