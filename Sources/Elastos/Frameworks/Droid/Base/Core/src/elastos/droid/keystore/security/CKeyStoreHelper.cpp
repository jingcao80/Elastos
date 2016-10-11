#include "elastos/droid/keystore/security/CKeyStoreHelper.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CAR_INTERFACE_IMPL(CKeyStoreHelper, Singleton, IKeyStoreHelper)
CAR_SINGLETON_IMPL(CKeyStoreHelper)

ECode CKeyStoreHelper::GetInstance(
    /* [out] */ IKeyStore** store)
{
    VALIDATE_NOT_NULL(store);
    AutoPtr<KeyStore> ks = KeyStore::GetInstance();
    *store = ks;
    REFCOUNT_ADD(*store);
    return NOERROR;
}

ECode CKeyStoreHelper::GetKeyTypeForAlgorithm(
    /* [in] */ const String& keyType,
    /* [out] */ Int32* algorithm)
{
    VALIDATE_NOT_NULL(algorithm);
    *algorithm = KeyStore::GetKeyTypeForAlgorithm(keyType);
    return NOERROR;
}

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos
