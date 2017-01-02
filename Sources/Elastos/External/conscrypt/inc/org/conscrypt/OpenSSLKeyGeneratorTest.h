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

#ifndef __ORG_CONSCRYPT_OPENSSLKEYGENERATORTEST_H__
#define __ORG_CONSCRYPT_OPENSSLKEYGENERATORTEST_H__

#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastosx::Crypto::IKeyGeneratorSpi;
using Elastos::Security::ISecureRandom;
using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastosx::Crypto::ISecretKey;

namespace Org {
namespace Conscrypt {

class OpenSSLKeyGeneratorTest
    : public Object
    , public IKeyGeneratorSpi
{
public:
    CAR_INTERFACE_DECL()

    CARAPI EngineGenerateKey(
        /* [out] */ ISecretKey** result);

    CARAPI EngineInit(
        /* [in] */ IAlgorithmParameterSpec* params,
        /* [in] */ ISecureRandom* random);

    CARAPI EngineInit(
        /* [in] */ Int32 keysize,
        /* [in] */ ISecureRandom* random);

    CARAPI EngineInit(
        /* [in] */ ISecureRandom* random);

};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLKEYGENERATORTEST_H__
