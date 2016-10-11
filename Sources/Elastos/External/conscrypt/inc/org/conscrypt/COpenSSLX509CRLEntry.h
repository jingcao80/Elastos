
#ifndef __ORG_CONSCRYPT_COPENSSLX509CRLENTRY_H__
#define __ORG_CONSCRYPT_COPENSSLX509CRLENTRY_H__

#include "_Org_Conscrypt_COpenSSLX509CRLEntry.h"
#include "OpenSSLX509CRLEntry.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLX509CRLEntry)
    , public OpenSSLX509CRLEntry
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLX509CRLENTRY_H__
