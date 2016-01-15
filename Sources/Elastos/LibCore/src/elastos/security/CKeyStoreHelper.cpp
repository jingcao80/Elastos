
#include "CKeyStoreHelper.h"
#include "KeyStore.h"

namespace Elastos {
namespace Security {

CAR_SINGLETON_IMPL(CKeyStoreHelper);
CAR_INTERFACE_IMPL(CKeyStoreHelper, Singleton, IKeyStoreHelper);
ECode CKeyStoreHelper::GetInstance(
    /* [in] */ const String& type,
    /* [out] */ IKeyStore **instance)
{
    return KeyStore::GetInstance(type, instance);
}

ECode CKeyStoreHelper::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ const String& provider,
    /* [out] */ IKeyStore **instance)
{
    return KeyStore::GetInstance(type, provider, instance);
}

ECode CKeyStoreHelper::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ IProvider *provider,
    /* [out] */ IKeyStore **instance)
{
    return KeyStore::GetInstance(type, provider, instance);
}

ECode CKeyStoreHelper::GetDefaultType(
    /* [out] */ String *defaultType)
{
    return KeyStore::GetDefaultType(defaultType);
}

} // namespace Security
} // namespace Elastos