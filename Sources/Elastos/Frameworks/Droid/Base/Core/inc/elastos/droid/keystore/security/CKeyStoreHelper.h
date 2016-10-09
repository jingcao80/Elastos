#ifndef __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYSTOREHELPER_H__
#define __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYSTOREHELPER_H__

#include "_Elastos_Droid_KeyStore_Security_CKeyStoreHelper.h"
#include "elastos/droid/keystore/security/KeyStore.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CarClass(CKeyStoreHelper)
    , public Singleton
    , public IKeyStoreHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [out] */ IKeyStore** store);

    CARAPI GetKeyTypeForAlgorithm(
        /* [in] */ const String& keyType,
        /* [out] */ Int32* algorithm);
};

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYSTOREHELPER_H__
