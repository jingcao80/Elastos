
#ifndef __ORG_CONSCRYPT_COPENSSLSOCKETIMPLWRAPPER_H__
#define __ORG_CONSCRYPT_COPENSSLSOCKETIMPLWRAPPER_H__

#include "_Org_Conscrypt_COpenSSLSocketImplWrapper.h"
#include "OpenSSLSocketImplWrapper.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLSocketImplWrapper)
    , public OpenSSLSocketImplWrapper
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLSOCKETIMPLWRAPPER_H__
