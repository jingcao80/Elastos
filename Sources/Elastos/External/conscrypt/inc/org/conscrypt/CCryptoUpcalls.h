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

#ifndef __ORG_CONSCRYPT_CCRYPTOUPCALLS_H__
#define __ORG_CONSCRYPT_CCRYPTOUPCALLS_H__

#include "_Org_Conscrypt_CCryptoUpcalls.h"
#include <Elastos.CoreLibrary.Security.h>
#include <elastos/core/Singleton.h>

using Elastos::Security::IProvider;
using Elastos::Security::IPrivateKey;

namespace Org {
namespace Conscrypt {

CarClass(CCryptoUpcalls)
    , public Singleton
    , public ICryptoUpcalls
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Finds the first provider which provides {@code algorithm} but is not from
     * the same ClassLoader as ours.
     */
    CARAPI GetExternalProvider(
        /* [in] */ const String& algorithm,
        /* [out] */ IProvider** result);

    CARAPI RawSignDigestWithPrivateKey(
        /* [in] */ IPrivateKey* key,
        /* [in] */ ArrayOf<Byte>* message,
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI RawCipherWithPrivateKey(
        /* [in] */ IPrivateKey* key,
        /* [in] */ Boolean encrypt,
        /* [in] */ ArrayOf<Byte>* input,
        /* [out, callee] */ ArrayOf<Byte>** result);
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_CCRYPTOUPCALLS_H__
