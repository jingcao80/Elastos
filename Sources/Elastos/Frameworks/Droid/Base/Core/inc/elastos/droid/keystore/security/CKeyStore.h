#ifndef __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYSTORE_H__
#define __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYSTORE_H__

#include "_Elastos_Droid_KeyStore_Security_CKeyStore.h"
#include "elastos/droid/keystore/security/KeyStore.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CarClass(CKeyStore)
    , public KeyStore
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYSTORE_H__
