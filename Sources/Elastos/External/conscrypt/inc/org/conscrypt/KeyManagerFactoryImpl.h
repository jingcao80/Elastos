//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

    CARAPI constructor();

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
