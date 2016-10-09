#ifndef __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYSTOREPARAMETERBUILDER_H__
#define __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYSTOREPARAMETERBUILDER_H__

#include "_Elastos_Droid_KeyStore_Security_CKeyStoreParameterBuilder.h"
#include "elastos/droid/keystore/security/KeyStoreParameter.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CarClass(CKeyStoreParameterBuilder)
    , public KeyStoreParameter::Builder
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYSTOREPARAMETERBUILDER_H__
