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

#ifndef __ORG_CONSCRYPT_OPENSSLDHKEYPAIRGENERATOR_H__
#define __ORG_CONSCRYPT_OPENSSLDHKEYPAIRGENERATOR_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Security.h>
#include <elastos/security/KeyPairGeneratorSpi.h>
#include "_Org.Conscrypt.h"

using Elastos::Math::IBigInteger;
using Elastos::Security::IKeyPair;
using Elastos::Security::ISecureRandom;
using Elastos::Security::KeyPairGeneratorSpi;
using Elastos::Security::Spec::IAlgorithmParameterSpec;

namespace Org {
namespace Conscrypt {

class OpenSSLDHKeyPairGenerator
    : public IOpenSSLDHKeyPairGenerator
    , public KeyPairGeneratorSpi
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLDHKeyPairGenerator();

    CARAPI constructor();

    virtual CARAPI GenerateKeyPair(
        /* [out] */  IKeyPair** result);

    virtual CARAPI Initialize(
        /* [in] */ Int32 keysize,
        /* [in] */ ISecureRandom* random);

    virtual CARAPI Initialize(
        /* [in] */ IAlgorithmParameterSpec* params,
        /* [in] */ ISecureRandom* random);

private:
    /** The safe prime to use for the generated DH key pair. */
    AutoPtr<IBigInteger> mPrime;

    /** If {@code prime} is unspecified, this is the size of the generated prime. */
    Int32 mPrimeBits; // = 1024;

    static AutoPtr<IBigInteger> DEFAULT_GENERATOR; // = BigInteger.valueOf(2);

    AutoPtr<IBigInteger> mGenerator; // = DEFAULT_GENERATOR;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLDHKEYPAIRGENERATOR_H__
