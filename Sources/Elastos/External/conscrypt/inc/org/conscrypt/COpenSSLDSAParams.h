
#ifndef __ORG_CONSCRYPT_COPENSSLDSAPARAMS_H__
#define __ORG_CONSCRYPT_COPENSSLDSAPARAMS_H__

#include "_Org_Conscrypt_COpenSSLDSAParams.h"
#include "OpenSSLDSAParams.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLDSAParams)
    , public OpenSSLDSAParams
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLDSAPARAMS_H__
