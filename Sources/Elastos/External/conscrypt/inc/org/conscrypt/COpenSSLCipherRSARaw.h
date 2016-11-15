#ifndef __ORG_CONSCRYPT_COPENSSLCIPHERRSARAW_H__
#define __ORG_CONSCRYPT_COPENSSLCIPHERRSARAW_H__

#include "_Org_Conscrypt_COpenSSLCipherRSARaw.h"
#include "org/conscrypt/OpenSSLCipherRSA.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLCipherRSARaw)
    , public OpenSSLCipherRSA_Raw
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLCIPHERRSARAW_H__
