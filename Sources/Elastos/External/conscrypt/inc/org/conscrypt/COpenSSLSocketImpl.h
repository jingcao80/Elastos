
#ifndef __ORG_CONSCRYPT_COPENSSLSOCKERIMPL_H__
#define __ORG_CONSCRYPT_COPENSSLSOCKERIMPL_H__

#include "_Org_Conscrypt_COpenSSLSocketImpl.h"
#include "OpenSSLSocketImpl.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLSocketImpl)
    , public OpenSSLSocketImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLSOCKERIMPL_H__
