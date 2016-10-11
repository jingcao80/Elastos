#ifndef __ELASTOS_DROID_KEYSTORE_SECURITY_CELASTOSKEYSTOREPROVIDER_H__
#define __ELASTOS_DROID_KEYSTORE_SECURITY_CELASTOSKEYSTOREPROVIDER_H__

#include "_Elastos_Droid_KeyStore_Security_CElastosKeyStoreProvider.h"
#include "elastos/droid/keystore/security/ElastosKeyStoreProvider.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CarClass(CElastosKeyStoreProvider)
    , public ElastosKeyStoreProvider
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_KEYSTORE_SECURITY_CELASTOSKEYSTOREPROVIDER_H__
