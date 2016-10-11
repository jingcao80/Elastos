
#ifndef __ORG_CONSCRYPT_CSSLNULLSESSION_H__
#define __ORG_CONSCRYPT_CSSLNULLSESSION_H__

#include "_Org_Conscrypt_CSSLNullSession.h"
#include "SSLNullSession.h"

namespace Org {
namespace Conscrypt {

CarClass(CSSLNullSession)
    , public SSLNullSession
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_CSSLNULLSESSION_H__
