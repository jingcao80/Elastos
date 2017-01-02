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

#include "CNullCipher.h"

using Elastos::Security::IKey;
using Elastos::Security::ISecureRandom;

namespace Elastosx {
namespace Crypto {

CAR_OBJECT_IMPL(CNullCipher)

CAR_INTERFACE_IMPL(CNullCipher, Cipher, INullCipher)

CNullCipher::CNullCipher()
{
}

ECode CNullCipher::constructor()
{
//TODO: Need INullCipherSpi
    // AutoPtr<INullCipherSpi> spi;
    // FAIL_RETURN(CNullCipherSpi::New((INullCipherSpi**)&spi));
    // Cipher::constructor(spi, NULL, NULL);
    // try {
    AutoPtr<IKey> key;
    AutoPtr<ISecureRandom> sr;
    return this->Init(ICipher::ENCRYPT_MODE, key, sr);
    // } catch (InvalidKeyException e) {
    // }
}

} // Crypto
} // Elastosx