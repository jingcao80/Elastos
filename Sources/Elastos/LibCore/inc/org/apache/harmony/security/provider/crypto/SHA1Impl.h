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

#ifndef __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_SHA1IMPL_H__
#define __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_SHA1IMPL_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "coredef.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

/**
 * This class contains methods providing SHA-1 functionality to use in classes. <BR>
 * The methods support the algorithm described in "SECURE HASH STANDARD", FIPS PUB 180-2, <BR>
 * "http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf"      <BR>
 * <BR>
 * The class contains two package level access methods, -
 * "void updateHash(Int32[], byte[], Int32, Int32)" and "void computeHash(Int32[])", -
 * performing the following operations. <BR>
 * <BR>
 * The "updateHash(..)" method appends new bytes to existing ones
 * within limit of a frame of 64 bytes (16 words).
 * Once a length of accumulated bytes reaches the limit
 * the "computeHash(Int32[])" method is invoked on the frame to compute updated hash,
 * and the number of bytes in the frame is set to 0.
 * Thus, after appending all bytes, the frame contain only those bytes
 * that were not used in computing final hash value yet. <BR>
 * <BR>
 * The "computeHash(..)" method generates a 160 bit hash value using
 * a 512 bit message stored in first 16 words of Int32[] array argument and
 * current hash value stored in five words, beginning HASH_OFFSET, of the array argument.
 * Computation is done according to SHA-1 algorithm. <BR>
 * <BR>
 * The resulting hash value replaces the previous hash value in the array;
 * original bits of the message are not preserved.
 */
class SHA1Impl
{
public:
    /**
     * The method generates a 160 bit hash value using
     * a 512 bit message stored in first 16 words of Int32[] array argument and
     * current hash value stored in five words, beginning OFFSET+1, of the array argument.
     * Computation is done according to SHA-1 algorithm.
     *
     * The resulting hash value replaces the previous hash value in the array;
     * original bits of the message are not preserved.
     *
     * No checks on argument supplied, that is,
     * a calling method is responsible for such checks.
     * In case of incorrect array passed to the method
     * either NPE or IndexOutOfBoundException gets thrown by JVM.
     *
     * @params
     *        arrW - integer array; arrW.length >= (BYTES_OFFSET+6); <BR>
     *               only first (BYTES_OFFSET+6) words are used
     */
    static CARAPI_(void) ComputeHash(
        /* [in] */ ArrayOf<Int32>* arrW);

    /**
     * The method appends new bytes to existing ones
     * within limit of a frame of 64 bytes (16 words).
     *
     * Once a length of accumulated bytes reaches the limit
     * the "computeHash(Int32[])" method is invoked on the array to compute updated hash,
     * and the number of bytes in the frame is set to 0.
     * Thus, after appending all bytes, the array contain only those bytes
     * that were not used in computing final hash value yet.
     *
     * No checks on arguments passed to the method, that is,
     * a calling method is responsible for such checks.
     *
     * @params
     *        intArray  - Int32 array containing bytes to which to append;
     *                    intArray.length >= (BYTES_OFFSET+6)
     * @params
     *        byteInput - array of bytes to use for the update
     * @params
     *        from      - the offset to start in the "byteInput" array
     * @params
     *        to        - a number of the last byte in the input array to use,
     *                that is, for first byte "to"==0, for last byte "to"==input.length-1
     */
    static CARAPI_(void) UpdateHash(
        /* [in] */ ArrayOf<Int32>* intArray,
        /* [in] */ ArrayOf<Byte>* byteInput,
        /* [in] */ Int32 fromByte,
        /* [in] */ Int32 toByte);
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_SHA1IMPL_H__
