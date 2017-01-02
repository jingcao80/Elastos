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

#ifndef __ORG_CONSCRYPT_OPENSSLDSAKEYFACTORY_H__
#define __ORG_CONSCRYPT_OPENSSLDSAKEYFACTORY_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Security.h>
#include <elastos/core/Object.h>
#include "_Org.Conscrypt.h"

using Elastos::Security::IKey;
using Elastos::Security::IKeyFactorySpi;
using Elastos::Security::IPrivateKey;
using Elastos::Security::IPublicKey;
using Elastos::Security::Spec::IKeySpec;

namespace Org {
namespace Conscrypt {

class OpenSSLDSAKeyFactory
    : public IOpenSSLDSAKeyFactory
    , public IKeyFactorySpi
    , public Object
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI EngineGeneratePublic(
       /* [in] */ IKeySpec* keySpec,
       /* [out] */ IPublicKey** result);

    CARAPI EngineGeneratePrivate(
        /* [in] */ IKeySpec* keySpec,
        /* [out] */ IPrivateKey** result);

    CARAPI EngineGetKeySpec(
        /* [in] */ IKey* key,
        /* [in] */ const ClassID& keySpec,
        /* [out] */ IKeySpec** result);

    CARAPI EngineTranslateKey(
        /* [in] */ IKey* key,
        /* [out] */ IKey** result);
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLDSAKEYFACTORY_H__
