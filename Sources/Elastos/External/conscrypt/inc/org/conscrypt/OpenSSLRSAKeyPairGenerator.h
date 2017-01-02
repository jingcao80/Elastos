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

#ifndef __ORG_CONSCRYPT_OPENSSLRSAKEYPAIRGENERATOR_H__
#define __ORG_CONSCRYPT_OPENSSLRSAKEYPAIRGENERATOR_H__

#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::Security::IKeyPair;
using Elastos::Security::ISecureRandom;
using Elastos::Security::IKeyPairGeneratorSpi;
using Elastos::Security::Spec::IAlgorithmParameterSpec;

namespace Org {
namespace Conscrypt {

class OpenSSLRSAKeyPairGenerator
    : public Object
    , public IKeyPairGeneratorSpi
    , public IOpenSSLRSAKeyPairGenerator
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLRSAKeyPairGenerator();

    CARAPI GenerateKeyPair(
        /* [out] */ IKeyPair** result);

    CARAPI Initialize(
        /* [in] */ Int32 keysize,
        /* [in] */ ISecureRandom* random);

    CARAPI Initialize(
        /* [in] */ IAlgorithmParameterSpec* params,
        /* [in] */ ISecureRandom* random);

private:
    /**
     * Default modulus size is 0x10001 (65537)
     */
    AutoPtr<ArrayOf<Byte> > mPublicExponent;

    /**
     * Default RSA key size 2048 bits.
     */
    Int32 mModulusBits;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLRSAKEYPAIRGENERATOR_H__