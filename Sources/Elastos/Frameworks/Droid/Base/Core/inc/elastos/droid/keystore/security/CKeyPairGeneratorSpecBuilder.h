#ifndef __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYPAIRGENERATORSPECBUILDER_H__
#define __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYPAIRGENERATORSPECBUILDER_H__

#include "_Elastos_Droid_KeyStore_Security_CKeyPairGeneratorSpecBuilder.h"
#include "elastos/droid/keystore/security/KeyPairGeneratorSpec.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CarClass(CKeyPairGeneratorSpecBuilder)
    , public KeyPairGeneratorSpec::Builder
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYPAIRGENERATORSPECBUILDER_H__
