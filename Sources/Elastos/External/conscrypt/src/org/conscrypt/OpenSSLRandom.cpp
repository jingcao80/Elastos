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

#include "OpenSSLRandom.h"
#include "NativeCrypto.h"

using Elastos::IO::EIID_ISerializable;
using Elastos::Security::EIID_ISecureRandomSpi;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLRandom::
//=========================================
Int64 OpenSSLRandom::mSerialVersionUID = 8506210602917522860L;

CAR_INTERFACE_IMPL_3(OpenSSLRandom, Object, ISecureRandomSpi, ISerializable, IOpenSSLRandom)

ECode OpenSSLRandom::EngineSetSeed(
    /* [in] */ ArrayOf<Byte>* seed)
{
    // NOTE: The contract of the SecureRandomSpi does not appear to prohibit self-seeding here
    // (in addition to using the provided seed).
    SelfSeedIfNotSeeded();
    NativeCrypto::RAND_seed(seed);
    return NOERROR;
}

ECode OpenSSLRandom::EngineNextBytes(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    SelfSeedIfNotSeeded();
    NativeCrypto::RAND_bytes(bytes);
    return NOERROR;
}

ECode OpenSSLRandom::EngineGenerateSeed(
    /* [in] */ Int32 numBytes,
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    SelfSeedIfNotSeeded();
    AutoPtr<ArrayOf<Byte> > output = ArrayOf<Byte>::Alloc(numBytes);
    NativeCrypto::RAND_bytes(output);
    *result = output;
    return NOERROR;
}

void OpenSSLRandom::SelfSeedIfNotSeeded()
{
    // NOTE: No need to worry about concurrent access to this field because the worst case is
    // that the code below is executed multiple times (by different threads), which may only
    // increase the entropy of the OpenSSL PRNG.
    if (mSeeded) {
        return;
    }

    SeedOpenSSLPRNGFromLinuxRNG();
    mSeeded = TRUE;
}

ECode OpenSSLRandom::SeedOpenSSLPRNGFromLinuxRNG()
{
    Int32 seedLengthInBytes = INativeCrypto::RAND_SEED_LENGTH_IN_BYTES;
    Int32 bytesRead = 0;
    NativeCrypto::RAND_load_file(String("/dev/urandom"), seedLengthInBytes, &bytesRead);
    if (bytesRead != seedLengthInBytes) {
        // throw new SecurityException("Failed to read sufficient bytes from /dev/urandom."
        //         + " Expected: " + seedLengthInBytes + ", actual: " + bytesRead);
        return NOERROR;
    }
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
