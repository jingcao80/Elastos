
#ifndef __ORG_CONSCRYPT_COPENSSLSESSIONIMPL_H__
#define __ORG_CONSCRYPT_COPENSSLSESSIONIMPL_H__

#include "_Org_Conscrypt_COpenSSLSessionImpl.h"
#include "OpenSSLSessionImpl.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLSessionImpl)
    , public OpenSSLSessionImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLSESSIONIMPL_H__
