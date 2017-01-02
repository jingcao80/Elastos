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

#ifndef __ORG_CONSCRYPT_COPENSSLECKEYPAIRGENERATOR_H__
#define __ORG_CONSCRYPT_COPENSSLECKEYPAIRGENERATOR_H__

#include "_Org_Conscrypt_COpenSSLECKeyPairGenerator.h"
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Security.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>
#include "elastos/security/KeyPairGenerator.h"

using Elastos::Security::IKeyPair;
using Elastos::Security::ISecureRandom;
using Elastos::Security::KeyPairGenerator;
using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastos::Utility::IMap;

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLECKeyPairGenerator)
    , public IOpenSSLECKeyPairGenerator
    , public KeyPairGenerator
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    // @Override
    CARAPI GenerateKeyPair(
        /* [out] */ IKeyPair** result);

    // @Override
    CARAPI Initialize(
        /* [in] */ Int32 keysize,
        /* [in] */ ISecureRandom* random);

    // @Override
    CARAPI Initialize(
        /* [in] */ IAlgorithmParameterSpec* param,
        /* [in] */ ISecureRandom* random);

private:
    static Boolean Init();

private:
    static const String ALGORITHM; // = "EC";

    static const Int32 DEFAULT_KEY_SIZE = 192;

    static AutoPtr<IMap/*<Integer, String>*/> SIZE_TO_CURVE_NAME;

    AutoPtr<IOpenSSLECGroupContext> mGroup;

    static Boolean sIsInited;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLECKEYPAIRGENERATOR_H__
