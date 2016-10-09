#ifndef __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYPAIRGENERATORSPEC_H__
#define __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYPAIRGENERATORSPEC_H__

#include "_Elastos_Droid_KeyStore_Security_CKeyPairGeneratorSpec.h"
#include "elastos/droid/keystore/security/KeyPairGeneratorSpec.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CarClass(CKeyPairGeneratorSpec)
    , public KeyPairGeneratorSpec
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYPAIRGENERATORSPEC_H__
