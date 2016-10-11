#ifndef __ELASTOS_DROID_KEYSTORE_SECURITY_CSYSTEMKEYSTOREHELPER_H__
#define __ELASTOS_DROID_KEYSTORE_SECURITY_CSYSTEMKEYSTOREHELPER_H__

#include "_Elastos_Droid_KeyStore_Security_CSystemKeyStoreHelper.h"
#include "elastos/droid/keystore/security/SystemKeyStore.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CarClass(CSystemKeyStoreHelper)
    , public Singleton
    , public ISystemKeyStoreHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [out] */ ISystemKeyStore** store);

    CARAPI ToHexString(
        /* [in] */ ArrayOf<Byte>* keyData,
        /* [out] */ String* hex);

};

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_KEYSTORE_SECURITY_CSYSTEMKEYSTOREHELPER_H__
