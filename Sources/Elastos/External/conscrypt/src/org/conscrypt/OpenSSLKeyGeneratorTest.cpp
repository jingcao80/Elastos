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

#include "OpenSSLKeyGeneratorTest.h"
#include "org/conscrypt/COpenSSLSecretKey.h"
#include <elastos/utility/logging/Logger.h>

using Elastosx::Crypto::EIID_IKeyGeneratorSpi;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLKeyGeneratorTest::
//=========================================

CAR_INTERFACE_IMPL(OpenSSLKeyGeneratorTest, Object, IKeyGeneratorSpi);

ECode OpenSSLKeyGeneratorTest::EngineGenerateKey(
    /* [out] */ ISecretKey** result)
{
    Logger::E("leliang", "TODO OpenSSLKeyGeneratorTest::EngineGenerateKey");
    VALIDATE_NOT_NULL(result);
    String algorithm("HMACMD5");
    AutoPtr<ArrayOf<Byte> > encoded = ArrayOf<Byte>::Alloc(10);
    for (Int32 i = 0; i < 10; ++i) {
        encoded->Set(i, 'A' + i);
    }
    COpenSSLSecretKey::New(algorithm, encoded, result);
    Logger::E("leliang", "TODO OpenSSLKeyGeneratorTest::EngineGenerateKey, result:%p", *result);
    return NOERROR;
}

ECode OpenSSLKeyGeneratorTest::EngineInit(
    /* [in] */ IAlgorithmParameterSpec* params,
    /* [in] */ ISecureRandom* random)
{
    Logger::E("leliang", "TODO OpenSSLKeyGeneratorTest::EngineInit 1");
    return NOERROR;
}

ECode OpenSSLKeyGeneratorTest::EngineInit(
    /* [in] */ Int32 keysize,
    /* [in] */ ISecureRandom* random)
{
    Logger::E("leliang", "TODO OpenSSLKeyGeneratorTest::EngineInit 2");
    return NOERROR;
}

ECode OpenSSLKeyGeneratorTest::EngineInit(
    /* [in] */ ISecureRandom* random)
{
    Logger::E("leliang", "TODO OpenSSLKeyGeneratorTest::EngineInit 3");
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
