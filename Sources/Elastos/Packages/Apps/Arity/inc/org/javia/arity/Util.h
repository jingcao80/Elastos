
#ifndef _ORG_JAVIA_ARITY_UTIL_H__
#define _ORG_JAVIA_ARITY_UTIL_H__

#include "_Org.Javia.Arity.h"
#include <elastos/droid/ext/frameworkext.h>

namespace Org {
namespace Javia {
namespace Arity {

/**
   Contains static helper methods for formatting double values.
 */
class ECO_PUBLIC Util
{
public:
    /** Returns a number which is an approximation of v (within maxError)
       and which has fewer digits in base-10).
       @param value the value to be approximated
       @param maxError the maximum deviation from value
       @return an approximation with a more compact base-10 representation.
    */
    static CARAPI_(Double) ShortApprox(
        /* [in] */ Double value,
        /* [in] */ Double maxError);

    /**
      Returns an approximation with no more than maxLen chars.

      This method is not public, it is called through doubleToString,
      that's why we can make some assumptions about the format of the string,
      such as assuming that the exponent 'E' is upper-case.

      @param str the value to truncate (e.g. "-2.898983455E20")
      @param maxLen the maximum number of characters in the returned string
      @return a truncation no longer then maxLen (e.g. "-2.8E20" for maxLen=7).
     */
    static CARAPI_(String) SizeTruncate(
        /* [in] */ const String& str,
        /* [in] */ Int32 maxLen);

    /**
       Rounds by dropping roundingDigits of double precision
       (similar to 'hidden precision digits' on calculators),
       and formats to String.
       @param v the value to be converted to String
       @param roundingDigits the number of 'hidden precision' digits (e.g. 2).
       @return a String representation of v
     */
    static CARAPI_(String) DoubleToString(
        /* [in] */ Double v,
        /* [in] */ Int32 roundingDigits);

    /**
       Renders a real number to a String (for user display).
       @param maxLen the maximum total length of the resulting string
       @param rounding the number of final digits to round
     */
    static CARAPI_(String) DoubleToString(
        /* [in] */ Double x,
        /* [in] */ Int32 maxLen,
        /* [in] */ Int32 rounding);

    /**
       Renders a complex number to a String (for user display).
       @param maxLen the maximum total length of the resulting string
       @param rounding the number of final digits to round
     */
    static CARAPI_(String) ComplexToString(
        /* [in] */ IComplex* x,
        /* [in] */ Int32 maxLen,
        /* [in] */ Int32 rounding);

public:
    static const Int32 LEN_UNLIMITED = 100;
    static const Int32 FLOAT_PRECISION  = -1;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_UTIL_H__