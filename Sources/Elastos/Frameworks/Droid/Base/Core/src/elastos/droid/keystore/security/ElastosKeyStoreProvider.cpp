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

#include "elastos/droid/keystore/security/ElastosKeyStoreProvider.h"
#include "elastos/droid/keystore/security/ElastosKeyStore.h"
#include "elastos/core/CoreUtils.h"

using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CAR_INTERFACE_IMPL(ElastosKeyStoreProvider, Elastos::Security::Provider, IElastosKeyStoreProvider);

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
