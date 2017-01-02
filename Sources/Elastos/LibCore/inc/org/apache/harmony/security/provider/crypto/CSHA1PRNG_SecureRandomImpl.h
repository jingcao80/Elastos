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

#ifndef __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CSHA1PRNG_SECURERANDOMIMPL_H__
#define __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CSHA1PRNG_SECURERANDOMIMPL_H__

#include "_Org_Apache_Harmony_Security_Provider_Crypto_CSHA1PRNG_SecureRandomImpl.h"
#include "Elastos.CoreLibrary.IO.h"
#include "SecureRandomSpi.h"

using Elastos::IO::IInputStream;
using Elastos::IO::IObjectInputStream;
using Elastos::IO::IObjectOutputStream;
using Elastos::Security::SecureRandomSpi;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

/**
 * This class extends the SecureRandomSpi class implementing all its abstract methods.
 *
 * <p>To generate pseudo-random bits, the implementation uses technique described in
 * the "Random Number Generator (RNG) algorithms" section, Appendix A,
 * JavaTM Cryptography Architecture, API Specification & Reference.
 */
CarClass(CSHA1PRNG_SecureRandomImpl)
    , public SecureRandomSpi
    , public ISHA1PRNG_SecureRandomImpl
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CSHA1PRNG_SecureRandomImpl();

    // The "seed" array is used to compute both "current seed hash" and "next bytes".
    //
    // As the "SHA1" algorithm computes a hash of entire seed by splitting it into
    // a number of the 512-bit length frames (512 bits = 64 bytes = 16 words),
    // "current seed hash" is a hash (5 words, 20 bytes) for all previous full frames;
    // remaining bytes are stored in the 0-15 word frame of the "seed" array.
    //
    // As for calculating "next bytes",
    // both remaining bytes and "current seed hash" are used,
    // to preserve the latter for following "setSeed(..)" commands,
    // the following technique is used:
    // - upon getting "nextBytes(Byte[])" invoked, single or first in row,
    //   which requires computing new hash, that is,
    //   there is no more bytes remaining from previous "next bytes" computation,
    //   remaining bytes are copied into the 21-36 word frame of the "copies" array;
    // - upon getting "setSeed(Byte[])" invoked, single or first in row,
    //   remaining bytes are copied back.

    /**
     *  Creates object and sets implementation variables to their initial values
     */
    CARAPI constructor();

protected:
    /**
     * Changes current seed by supplementing a seed argument to the current seed,
     * if this already set;
     * the argument is used as first seed otherwise. <BR>
     *
     * The method overrides "engineSetSeed(Byte[])" in class SecureRandomSpi.
     *
     * @param
     *       seed - Byte array
     * @throws
     *       NullPointerException - if NULL is passed to the "seed" argument
     */
    CARAPI EngineSetSeed(
        /* [in] */ ArrayOf<Byte>* seed);

    /**
     * Returns a required number of random bytes. <BR>
     *
     * The method overrides "engineGenerateSeed (Int32)" in class SecureRandomSpi. <BR>
     *
     * @param
     *       numBytes - number of bytes to return; should be >= 0.
     * @return
     *       Byte array containing bits in order from left to right
     * @throws
     *       InvalidParameterException - if numBytes < 0
     */
    CARAPI EngineGenerateSeed(
        /* [in] */ Int32 numBytes,
        /* [out] */ ArrayOf<Byte>** result);

    /**
     * Writes random bytes into an array supplied.
     * Bits in a Byte are from left to right. <BR>
     *
     * To generate random bytes, the "expansion of source bits" method is used,
     * that is,
     * the current seed with a 64-bit counter appended is used to compute new bits.
     * The counter is incremented by 1 for each 20-Byte output. <BR>
     *
     * The method overrides engineNextBytes in class SecureRandomSpi.
     *
     * @param
     *       bytes - Byte array to be filled in with bytes
     * @throws
     *       NullPointerException - if NULL is passed to the "bytes" argument
     */
    CARAPI EngineNextBytes(
        /* [in] */ ArrayOf<Byte>* bytes);

private:
    /*
     * The method invokes the SHA1Impl's "updateHash(..)" method
     * to update current seed frame and
     * to compute new intermediate hash value if the frame is full.
     *
     * After that it computes a length of whole seed.
     */
    CARAPI_(void) UpdateSeed(
        /* [in] */ ArrayOf<Byte>* bytes);

    CARAPI WriteObject(
        /* [in] */ IObjectOutputStream* oos) /*throws IOException*/;

    CARAPI ReadObject(
        /* [in] */ IObjectInputStream* ois) /*throws IOException, ClassNotFoundException*/;

    static CARAPI GetRandomBytes(
        /* [in] */ Int32 byteCount,
        /* [out] */ ArrayOf<Byte>** retValue);

private:
    // private static final Int64 serialVersionUID = 283736797212159675L;
    static AutoPtr<IInputStream> sDevURandom;

    // constants to use in expressions operating on bytes in Int32 and Int64 variables:
    // END_FLAGS - final bytes in words to append to message;
    //             see "ch.5.1 Padding the Message, FIPS 180-2"
    // RIGHT1    - shifts to right for left half of Int64
    // RIGHT2    - shifts to right for right half of Int64
    // LEFT      - shifts to left for bytes
    // MASK      - mask to select counter's bytes after shift to right

    static Int32 END_FLAGS[];

    static const Int32 RIGHT1[];

    static const Int32 RIGHT2[];

    static const Int32 LEFT[];

    static const Int32 MASK[];

    // HASHBYTES_TO_USE defines # of bytes returned by "computeHash(Byte[])"
    // to use to form Byte array returning by the "nextBytes(Byte[])" method
    // Note, that this implementation uses more bytes than it is defined
    // in the above specification.
    static const Int32 HASHBYTES_TO_USE;

    // value of 16 defined in the "SECURE HASH STANDARD", FIPS PUB 180-2
    static const Int32 FRAME_LENGTH;

    // miscellaneous constants defined in this implementation:
    // COUNTER_BASE - initial value to set to "counter" before computing "nextBytes(..)";
    //                note, that the exact value is not defined in STANDARD
    // HASHCOPY_OFFSET   - offset for copy of current hash in "copies" array
    // EXTRAFRAME_OFFSET - offset for extra frame in "copies" array;
    //                     as the extra frame follows the current hash frame,
    //                     EXTRAFRAME_OFFSET is equal to length of current hash frame
    // FRAME_OFFSET      - offset for frame in "copies" array
    // MAX_BYTES - maximum # of seed bytes processing which doesn't require extra frame
    //             see (1) comments on usage of "seed" array below and
    //             (2) comments in "engineNextBytes(Byte[])" method
    //
    // UNDEFINED  - three states of engine; initially its state is "UNDEFINED"
    // SET_SEED     call to "engineSetSeed"  sets up "SET_SEED" state,
    // NEXT_BYTES   call to "engineNextByte" sets up "NEXT_BYTES" state

    static const Int32 COUNTER_BASE;

    static const Int32 HASHCOPY_OFFSET;

    static const Int32 EXTRAFRAME_OFFSET;

    static const Int32 FRAME_OFFSET;

    static const Int32 MAX_BYTES;

    static const Int32 UNDEFINED;

    static const Int32 SET_SEED;

    static const Int32 NEXT_BYTES;

    static AutoPtr<CSHA1PRNG_SecureRandomImpl> sMyRandom;

    // Structure of "seed" array:
    // -  0-79 - words for computing hash
    // - 80    - unused
    // - 81    - # of seed bytes in current seed frame
    // - 82-86 - 5 words, current seed hash
    /*transient*/ AutoPtr<ArrayOf<Int32> > mSeed;

    // total length of seed bytes, including all processed
    /*transient*/ Int64 mSeedLength;

    // Structure of "copies" array
    // -  0-4  - 5 words, copy of current seed hash
    // -  5-20 - extra 16 words frame;
    //           is used if final padding exceeds 512-bit length
    // - 21-36 - 16 word frame to store a copy of remaining bytes
    /*transient*/ AutoPtr<ArrayOf<Int32> > mCopies;

    // ready "next" bytes; needed because words are returned
    /*transient*/ AutoPtr<ArrayOf<Byte> > mNextBytes;

    // index of used bytes in "nextBytes" array
    /*transient*/ Int32 mNextBIndex;

    // variable required according to "SECURE HASH STANDARD"
    /*transient*/ Int64 mCounter;

    // contains Int32 value corresponding to engine's current state
    /*transient*/ Int32 mState;
    Object THIS;
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CSHA1PRNG_SECURERANDOMIMPL_H__
