#ifndef __ELASTOS_DROID_KEYSTORE_SECURITY_CELASTOSKEYPAIRGENERATOR_H__
#define __ELASTOS_DROID_KEYSTORE_SECURITY_CELASTOSKEYPAIRGENERATOR_H__

#include "_Elastos_Droid_KeyStore_Security_CElastosKeyPairGenerator.h"
#include "elastos/droid/keystore/security/ElastosKeyPairGenerator.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CarClass(CElastosKeyPairGenerator)
    , public ElastosKeyPairGenerator
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_KEYSTORE_SECURITY_CELASTOSKEYPAIRGENERATOR_H__
