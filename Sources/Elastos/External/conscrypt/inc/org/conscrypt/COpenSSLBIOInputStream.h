
#ifndef __ORG_CONSCRYPT_COPENSSLBIOINPUTSTREAM_H__
#define __ORG_CONSCRYPT_COPENSSLBIOINPUTSTREAM_H__

#include "_Org_Conscrypt_COpenSSLBIOInputStream.h"
#include "OpenSSLBIOInputStream.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLBIOInputStream)
    , public OpenSSLBIOInputStream
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLBIOINPUTSTREAM_H__
