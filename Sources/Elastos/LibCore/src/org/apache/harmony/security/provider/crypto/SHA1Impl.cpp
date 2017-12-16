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

#include "SHA1Impl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

#ifdef __clang__
__attribute__((no_sanitize("integer")))
#endif
void SHA1Impl::ComputeHash(
    /* [in] */ ArrayOf<Int32>* arrW)
{
    Int32  a = (*arrW)[ISHA1Constants::HASH_OFFSET   ];
    Int32  b = (*arrW)[ISHA1Constants::HASH_OFFSET +1];
    Int32  c = (*arrW)[ISHA1Constants::HASH_OFFSET +2];
    Int32  d = (*arrW)[ISHA1Constants::HASH_OFFSET +3];
    Int32  e = (*arrW)[ISHA1Constants::HASH_OFFSET +4];

    Int32 temp;

    // In this implementation the "d. For t = 0 to 79 do" loop
    // is split into four loops. The following constants:
    //     K = 5A827999   0 <= t <= 19
    //     K = 6ED9EBA1  20 <= t <= 39
    //     K = 8F1BBCDC  40 <= t <= 59
    //     K = CA62C1D6  60 <= t <= 79
    // are hex literals in the loops.

    for ( Int32 t = 16; t < 80 ; t++ ) {

        temp  = (*arrW)[t-3] ^ (*arrW)[t-8] ^ (*arrW)[t-14] ^ (*arrW)[t-16];
        (*arrW)[t] = ( temp<<1 ) | ( ((UInt32)temp) >> 31 );
    }

    for ( Int32 t = 0 ; t < 20 ; t++ ) {

        temp = ( ( a<<5 ) | ( ((UInt32)a) >> 27 )   ) +
               ( ( b & c) | ((~b) & d)   ) +
               ( e + (*arrW)[t] + 0x5A827999 ) ;
        e = d;
        d = c;
        c = ( b<<30 ) | ( ((UInt32)b) >> 2 ) ;
        b = a;
        a = temp;
    }
    for ( Int32 t = 20 ; t < 40 ; t++ ) {

        temp = ((( a<<5 ) | ( ((UInt32)a) >> 27 ))) + (b ^ c ^ d) + (e + (*arrW)[t] + 0x6ED9EBA1) ;
        e = d;
        d = c;
        c = ( b<<30 ) | ( ((UInt32)b) >> 2 ) ;
        b = a;
        a = temp;
    }
    for ( Int32 t = 40 ; t < 60 ; t++ ) {

        temp = (( a<<5 ) | ( ((UInt32)a) >> 27 )) + ((b & c) | (b & d) | (c & d)) +
                                                         (e + (*arrW)[t] + 0x8F1BBCDC) ;
        e = d;
        d = c;
        c = ( b<<30 ) | ( ((UInt32)b) >> 2 ) ;
        b = a;
        a = temp;
    }
    for ( Int32 t = 60 ; t < 80 ; t++ ) {

        temp = ((( a<<5 ) | ( ((UInt32)a) >> 27 ))) + (b ^ c ^ d) + (e + (*arrW)[t] + 0xCA62C1D6) ;
        e = d;
        d = c;
        c = ( b<<30 ) | ( ((UInt32)b) >> 2 ) ;
        b = a;
        a = temp;
    }

    (*arrW)[ISHA1Constants::HASH_OFFSET   ] += a;
    (*arrW)[ISHA1Constants::HASH_OFFSET +1] += b;
    (*arrW)[ISHA1Constants::HASH_OFFSET +2] += c;
    (*arrW)[ISHA1Constants::HASH_OFFSET +3] += d;
    (*arrW)[ISHA1Constants::HASH_OFFSET +4] += e;
}

void SHA1Impl::UpdateHash(
    /* [in] */ ArrayOf<Int32>* intArray,
    /* [in] */ ArrayOf<Byte>* byteInput,
    /* [in] */ Int32 fromByte,
    /* [in] */ Int32 toByte)
{
    // As intArray contains a packed bytes
    // the buffer's index is in the intArray[ISHA1Constants::BYTES_OFFSET] element

    Int32 index = (*intArray)[ISHA1Constants::BYTES_OFFSET];
    Int32 i = fromByte;
    Int32 maxWord;
    Int32 nBytes;

    Int32 wordIndex = index >>2;
    Int32 byteIndex = index & 0x03;

    (*intArray)[ISHA1Constants::BYTES_OFFSET] = ( index + toByte - fromByte + 1 ) & 077 ;

    // In general case there are 3 stages :
    // - appending bytes to non-full word,
    // - writing 4 bytes into empty words,
    // - writing less than 4 bytes in last word

    if ( byteIndex != 0 ) {       // appending bytes in non-full word (as if)

        for ( ; ( i <= toByte ) && ( byteIndex < 4 ) ; i++ ) {
            (*intArray)[wordIndex] |= ( (*byteInput)[i] & 0xFF ) << ((3 - byteIndex)<<3) ;
            byteIndex++;
        }
        if ( byteIndex == 4 ) {
            wordIndex++;
            if ( wordIndex == 16 ) {          // intArray is full, computing hash

                ComputeHash(intArray);
                wordIndex = 0;
            }
        }
        if ( i > toByte ) {                 // all input bytes appended
            return ;
        }
    }

    // writing full words

    maxWord = (toByte - i + 1) >> 2;           // # of remaining full words, may be "0"
    for ( Int32 k = 0; k < maxWord ; k++ ) {

        (*intArray)[wordIndex] = ( ((Int32) (*byteInput)[i   ] & 0xFF) <<24 ) |
                              ( ((Int32) (*byteInput)[i +1] & 0xFF) <<16 ) |
                              ( ((Int32) (*byteInput)[i +2] & 0xFF) <<8  ) |
                              ( ((Int32) (*byteInput)[i +3] & 0xFF)      )  ;
        i += 4;
        wordIndex++;

        if ( wordIndex < 16 ) {     // buffer is not full yet
            continue;
        }
        ComputeHash(intArray);      // buffer is full, computing hash
        wordIndex = 0;
    }

    // writing last incomplete word
    // after writing free byte positions are set to "0"s

    nBytes = toByte - i +1;
    if ( nBytes != 0 ) {

        Int32 w =  ((Int32) (*byteInput)[i] & 0xFF) <<24 ;

        if ( nBytes != 1 ) {
            w |= ((Int32) (*byteInput)[i +1] & 0xFF) <<16 ;
            if ( nBytes != 2) {
                w |= ((Int32) (*byteInput)[i +2] & 0xFF) <<8 ;
            }
        }
        (*intArray)[wordIndex] = w;
    }

    return ;
}

}
}
}
}
}
}
