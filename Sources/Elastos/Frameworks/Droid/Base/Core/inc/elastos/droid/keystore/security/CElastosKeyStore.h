#ifndef __ELASTOS_DROID_KEYSTORE_SECURITY_CELASTOSKEYSTORE_H__
#define __ELASTOS_DROID_KEYSTORE_SECURITY_CELASTOSKEYSTORE_H__

#include "_Elastos_Droid_KeyStore_Security_CElastosKeyStore.h"
#include "elastos/droid/keystore/security/ElastosKeyStore.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CarClass(CElastosKeyStore)
    , public ElastosKeyStore
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_KEYSTORE_SECURITY_CELASTOSKEYSTORE_H__
