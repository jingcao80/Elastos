
#ifndef __ORG_CONSCRYPT_COPENSSLSOCKETFACTORYIMPL_H__
#define __ORG_CONSCRYPT_COPENSSLSOCKETFACTORYIMPL_H__

#include "_Org_Conscrypt_COpenSSLSocketFactoryImpl.h"
#include "OpenSSLSocketFactoryImpl.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLSocketFactoryImpl)
    , public OpenSSLSocketFactoryImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLSOCKETFACTORYIMPL_H__
