
#ifndef __ORG_CONSCRYPT_COPENSSLCONTEXTIMPL_H__
#define __ORG_CONSCRYPT_COPENSSLCONTEXTIMPL_H__

#include "_Org_Conscrypt_COpenSSLContextImpl.h"
#include "OpenSSLContextImpl.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLContextImpl)
    , public OpenSSLContextImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLCONTEXTIMPL_H__
