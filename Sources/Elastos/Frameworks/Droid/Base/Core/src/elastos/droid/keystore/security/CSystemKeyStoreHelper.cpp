#include "elastos/droid/keystore/security/CSystemKeyStoreHelper.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CAR_INTERFACE_IMPL(CSystemKeyStoreHelper, Singleton, ISystemKeyStoreHelper)
CAR_SINGLETON_IMPL(CSystemKeyStoreHelper)

ECode CSystemKeyStoreHelper::GetInstance(
    /* [out] */ ISystemKeyStore** store)
{
    VALIDATE_NOT_NULL(store);
    AutoPtr<ISystemKeyStore> ss = SystemKeyStore::GetInstance();
    *store = ss;
    REFCOUNT_ADD(*store);
    return NOERROR;
}

ECode CSystemKeyStoreHelper::ToHexString(
    /* [in] */ ArrayOf<Byte>* keyData,
    /* [out] */ String* hex)
{
    VALIDATE_NOT_NULL(hex);
    *hex = SystemKeyStore::ToHexString(keyData);
    return NOERROR;
}

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos
