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
