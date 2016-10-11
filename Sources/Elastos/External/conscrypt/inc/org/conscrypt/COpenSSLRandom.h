
#ifndef __ORG_CONSCRYPT_COPENSSLRANDOM_H__
#define __ORG_CONSCRYPT_COPENSSLRANDOM_H__

#include "_Org_Conscrypt_COpenSSLRandom.h"
#include "OpenSSLRandom.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLRandom)
    , public OpenSSLRandom
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLRANDOM_H__
