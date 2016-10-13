
#ifndef __ORG_CONSCRYPT_COPENSSLDHKEYPAIRGENERATOR_H__
#define __ORG_CONSCRYPT_COPENSSLDHKEYPAIRGENERATOR_H__

#include "_Org_Conscrypt_COpenSSLDHKeyPairGenerator.h"
#include "OpenSSLDHKeyPairGenerator.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLDHKeyPairGenerator)
    , public OpenSSLDHKeyPairGenerator
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLDHKEYPAIRGENERATOR_H__
