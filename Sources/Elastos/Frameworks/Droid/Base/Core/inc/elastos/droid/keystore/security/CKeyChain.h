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

#ifndef __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYCHAIN_H__
#define __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYCHAIN_H__

#include "_Elastos_Droid_KeyStore_Security_CKeyChain.h"
#include "elastos/droid/keystore/security/KeyChain.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CarClass(CKeyChain)
    , public Singleton
    , public IKeyChain
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI CreateInstallIntent(
        /* [out]*/  IIntent** intent);

    CARAPI ChoosePrivateKeyAlias(
        /* [in] */ IActivity* activity,
        /* [in] */ IKeyChainAliasCallback* response,
        /* [in] */ ArrayOf<String>* keyTypes,
        /* [in] */ ArrayOf<IPrincipal*>* issuers,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ const String& alias);

    CARAPI GetPrivateKey(
        /* [in] */ IContext* ctx,
        /* [in] */ const String& alias,
        /* [out] */ IPrivateKey** result);

    CARAPI GetCertificateChain(
        /* [in] */ IContext* ctx,
        /* [in] */ const String& alias,
        /* [out, callee] */ ArrayOf<IX509Certificate*>** chain);

    CARAPI IsKeyAlgorithmSupported(
        /* [in] */ const String& algorithm,
        /* [out] */ Boolean* sopported);

    CARAPI IsBoundKeyAlgorithm(
        /* [in] */ const String& algorithm,
        /* [out] */ Boolean* ret);

    CARAPI ToCertificate(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [out] */ IX509Certificate** certificate);

    CARAPI Bind(
        /* [in] */ IContext* ctx,
        /* [out] */ IKeyChainConnection** connection);

    CARAPI BindAsUser(
        /* [in] */ IContext* ctx,
        /* [in] */ IUserHandle* user,
        /* [out] */ IKeyChainConnection** connection);
};

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYCHAIN_H__
