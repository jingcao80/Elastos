#ifndef __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYSTOREPARAMETER_H__
#define __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYSTOREPARAMETER_H__

#include "_Elastos_Droid_KeyStore_Security_CKeyStoreParameter.h"
#include "elastos/droid/keystore/security/KeyStoreParameter.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CarClass(CKeyStoreParameter)
    , public KeyStoreParameter
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYSTOREPARAMETER_H__
