
#ifndef __ORG_CONSCRYPT_COPENSSLBIOSOURCE_H__
#define __ORG_CONSCRYPT_COPENSSLBIOSOURCE_H__

#include "_Org_Conscrypt_COpenSSLBIOSource.h"
#include "OpenSSLBIOSource.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLBIOSource)
    , public OpenSSLBIOSource
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLBIOSOURCE_H__
