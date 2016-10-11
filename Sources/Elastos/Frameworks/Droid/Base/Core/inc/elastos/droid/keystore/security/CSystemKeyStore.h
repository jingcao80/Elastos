#ifndef __ELASTOS_DROID_KEYSTORE_SECURITY_CSYSTEMKEYSTORE_H__
#define __ELASTOS_DROID_KEYSTORE_SECURITY_CSYSTEMKEYSTORE_H__

#include "_Elastos_Droid_KeyStore_Security_CSystemKeyStore.h"
#include "elastos/droid/keystore/security/SystemKeyStore.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CarClass(CSystemKeyStore)
    , public SystemKeyStore
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_KEYSTORE_SECURITY_CSYSTEMKEYSTORE_H__
