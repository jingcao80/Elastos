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

#ifndef __ORG_CONSCRYPT_OPENSSLRANDOM_H__
#define __ORG_CONSCRYPT_OPENSSLRANDOM_H__

#include "Elastos.CoreLibrary.Security.h"
#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::IO::ISerializable;
using Elastos::Security::ISecureRandomSpi;

namespace Org {
namespace Conscrypt {

class OpenSSLRandom
    : public Object
    , public ISecureRandomSpi
    , public ISerializable
    , public IOpenSSLRandom
{
public:
    CAR_INTERFACE_DECL()

    CARAPI EngineSetSeed(
        /* [in] */ ArrayOf<Byte>* seed);

    CARAPI EngineNextBytes(
        /* [in] */ ArrayOf<Byte>* bytes);

    CARAPI EngineGenerateSeed(
        /* [in] */ Int32 numBytes,
        /* [out] */ ArrayOf<Byte>** result);

    /**
     * Obtains a seed from the Linux RNG and mixes it into the OpenSSL PRNG (default RAND engine).
     *
     * <p>NOTE: This modifies the OpenSSL PRNG shared by all instances of OpenSSLRandom and other
     * crypto primitives offered by or built on top of OpenSSL.
     */
    static CARAPI SeedOpenSSLPRNGFromLinuxRNG();

private:
    /**
     * Self-seeds this instance from the Linux RNG. Does nothing if this instance has already been
     * seeded.
     */
    void SelfSeedIfNotSeeded();

private:
    static Int64 mSerialVersionUID;

    Boolean mSeeded;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLRANDOM_H__