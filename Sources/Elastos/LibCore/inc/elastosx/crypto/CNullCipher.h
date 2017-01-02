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

#ifndef __ELASTOSX_CRYPTO_CNULLCIPHER_H__
#define __ELASTOSX_CRYPTO_CNULLCIPHER_H__

#include "_Elastosx_Crypto_CNullCipher.h"
#include "Cipher.h"

namespace Elastosx {
namespace Crypto {

CarClass(CNullCipher)
    , public Cipher
    , public INullCipher
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CNullCipher();

    /**
     * Creates a new {@code NullCipher} instance.
     */
    CARAPI constructor();

};

}
}

#endif // __ELASTOSX_CRYPTO_CNULLCIPHER_H__
