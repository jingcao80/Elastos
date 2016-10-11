#ifndef _ELASTOS_DROID_KEYSTORE_SECURITY_ELASTOSKEYSTOREPROVIDER_H
#define _ELASTOS_DROID_KEYSTORE_SECURITY_ELASTOSKEYSTOREPROVIDER_H

#include "Elastos.Droid.KeyStore.h"
#include "elastos/security/Provider.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

/**
 * A provider focused on providing JCA interfaces for the Android KeyStore.
 *
 * @hide
 */
class ElastosKeyStoreProvider
    : public Elastos::Security::Provider
    , public IElastosKeyStoreProvider
{
public:
    CAR_INTERFACE_DECL();

    ElastosKeyStoreProvider();

    CARAPI constructor();

public:
    static const String PROVIDER_NAME;// = "AndroidKeyStore";
};

}// namespace Elastos
}// namespace Droid
}// namespace KeyStore
}// namespace Security

#endif // _ELASTOS_DROID_KEYSTORE_SECURITY_ELASTOSKEYSTOREPROVIDER_H
