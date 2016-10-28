#include "elastos/droid/keystore/security/ElastosKeyStoreProvider.h"
#include "elastos/droid/keystore/security/ElastosKeyStore.h"
#include "elastos/core/CoreUtils.h"

using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CAR_INTERFACE_IMPL(ElastosKeyStoreProvider, Elastos::Security::Provider, IElastosKeyStoreProvider);
const String ElastosKeyStoreProvider::PROVIDER_NAME("ElastosKeyStore");
ElastosKeyStoreProvider::ElastosKeyStoreProvider()
{}

ECode ElastosKeyStoreProvider::constructor()
{
    Elastos::Security::Provider::constructor(PROVIDER_NAME, 1.0, String("Elastos KeyStore security provider"));

    String key1 = String("KeyStore.") + ElastosKeyStore::NAME;
    String value1 = String("Elastos.Droid.KeyStore.Security.CElastosKeyStore");
    // java.security.KeyStore
    AutoPtr<IInterface> tmp;
    Put(CoreUtils::Convert(key1), CoreUtils::Convert(value1), (IInterface**)&tmp);

    tmp = NULL;
    String key2 = String("KeyPairGenerator.RSA");
    String value2 = String("Elastos.Droid.KeyStore.Security.CElastosKeyPairGenerator");
    // java.security.KeyPairGenerator
    Put(CoreUtils::Convert(key2), CoreUtils::Convert(value2), (IInterface**)&tmp);
    return NOERROR;
}

}// namespace Elastos
}// namespace Droid
}// namespace KeyStore
}// namespace Security
