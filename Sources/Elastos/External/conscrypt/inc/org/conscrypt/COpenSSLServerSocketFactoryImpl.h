
#ifndef __ORG_CONSCRYPT_COPENSSLRSAPRIVATEKEY_H__
#define __ORG_CONSCRYPT_COPENSSLRSAPRIVATEKEY_H__

#include "_Org_Conscrypt_COpenSSLServerSocketFactoryImpl.h"
#include "OpenSSLServerSocketFactoryImpl.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLServerSocketFactoryImpl)
    , public OpenSSLServerSocketFactoryImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLRSAPRIVATEKEY_H__
