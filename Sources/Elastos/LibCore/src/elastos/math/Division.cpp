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

#include "Division.h"

namespace Elastos {
namespace Math {

Int32 Division::DivideArrayByInt(
    /* [in] */ ArrayOf<Int32>& quotient,
    /* [in] */ ArrayOf<Int32>& dividend,
    /* [in] */ Int32 dividendLength,
    /* [in] */ Int32 divisor)
{
    Int64 rem = 0;
    Int64 bLong = divisor & 0xffffffffL;
    Int32 uDivisor = (UInt32)divisor;

    for (Int32 i = dividendLength - 1; i >= 0; i--) {
        Int64 temp = (rem << 32) | (dividend[i] & 0xffffffffL);
        UInt64 uTemp = (UInt64)temp;
        Int64 quot;
        if (temp >= 0) {
            quot = (temp / bLong);
            rem = (temp % bLong);
        }
        else {
            /*
             * make the dividend positive shifting it right by 1 bit then
             * get the quotient an remainder and correct them properly
             */
            Int64 aPos = uTemp >> 1;    //temp >>> 1;
            Int64 bPos = uDivisor >> 1; //divisor >>> 1;
            quot = aPos / bPos;
            rem = aPos % bPos;
            // double the remainder and add 1 if a is odd
            rem = (rem << 1) + (temp & 1);
            if ((divisor & 1) != 0) {
                // the divisor is odd
                if (quot <= rem) {
                    rem -= quot;
                }
                else {
                    if (quot - rem <= bLong) {
                        rem += bLong - quot;
                        quot -= 1;
                    }
                    else {
                        rem += (bLong << 1) - quot;
                        quot -= 2;
                    }
                }
            }
        }
        quotient[i] = (Int32) (quot & 0xffffffffL);
    }
    return (Int32) rem;
}

} // namespace Math
} // namespace Elastos
