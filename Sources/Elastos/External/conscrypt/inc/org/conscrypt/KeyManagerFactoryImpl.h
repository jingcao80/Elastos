
#ifndef __ORG_CONSCRYPT_KEYMANAGERFACTORYIMPL_H__
#define __ORG_CONSCRYPT_KEYMANAGERFACTORYIMPL_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Security.h>
#include <elastos/core/Object.h>
#include "_Org.Conscrypt.h"

using Elastos::Security::IKeyStore;
using Elastosx::Net::Ssl::IKeyManager;
using Elastosx::Net::Ssl::IKeyManagerFactorySpi;
using Elastosx::Net::Ssl::IManagerFactoryParameters;

namespace Org {
namespace Conscrypt {

/**
 * KeyManagerFactory implementation.
 * @see KeyManagerFactorySpi
 */
class KeyManagerFactoryImpl
    : public Object
    , public IKeyManagerFactoryImpl
    , public IKeyManagerFactorySpi
{
public:
    CAR_INTERFACE_DECL()

    /**
     * @see KeyManagerFactorySpi#engineInit(KeyStore ks, char[] password)
     */
    // @Override
    CARAPI EngineInit(
        /* [in] */ IKeyStore* ks,
        /* [in] */ ArrayOf<Char32>* password);

    /**
     * @see KeyManagerFactorySpi#engineInit(ManagerFactoryParameters spec)
     */
    // @Override
    CARAPI EngineInit(
        /* [in] */ IManagerFactoryParameters* spec);

    /**
     * @see KeyManagerFactorySpi#engineGetKeyManagers()
     */
    // @Override
    CARAPI EngineGetKeyManagers(
        /* [out, callee] */ ArrayOf<IKeyManager*>** managers);

private:
    // source of key material
    AutoPtr<IKeyStore> mKeyStore;

    //password
    AutoPtr<ArrayOf<Char32> > mPwd;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_KEYMANAGERFACTORYIMPL_H__
