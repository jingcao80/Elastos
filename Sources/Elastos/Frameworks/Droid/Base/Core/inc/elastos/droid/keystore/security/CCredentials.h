#ifndef __ELASTOS_DROID_KEYSTORE_SECURITY_CCREDENTIALS_H__
#define __ELASTOS_DROID_KEYSTORE_SECURITY_CCREDENTIALS_H__

#include "_Elastos_Droid_KeyStore_Security_CCredentials.h"
#include "elastos/droid/keystore/security/Credentials.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CarClass(CCredentials)
    , public Credentials
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_KEYSTORE_SECURITY_CCREDENTIALS_H__
