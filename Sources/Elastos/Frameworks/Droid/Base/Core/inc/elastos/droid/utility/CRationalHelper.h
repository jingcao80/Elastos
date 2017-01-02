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

#ifndef __ELASTOS_DROID_UTILITY_CRATIONAL_HELPER_H__
#define __ELASTOS_DROID_UTILITY_CRATIONAL_HELPER_H__

#include "_Elastos_Droid_Utility_CRationalHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CRationalHelper)
    , public Singleton
    , public IRationalHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Calculates the greatest common divisor using Euclid's algorithm.
     *
     * <p><em>Visible for testing only.</em></p>
     *
     * @param numerator the numerator in a fraction
     * @param denominator the denominator in a fraction
     *
     * @return An int value representing the gcd. Always positive.
     * @hide
     */
    CARAPI Gcd(
        /* [in] */ Int32 numerator,
        /* [in] */ Int32 denominator,
        /* [out] */ Int32* value);

    /**
     * Parses the specified string as a rational value.
     * <p>The ASCII characters {@code \}{@code u003a} (':') and
     * {@code \}{@code u002f} ('/') are recognized as separators between
     * the numerator and denumerator.</p>
     * <p>
     * For any {@code Rational r}: {@code Rational.parseRational(r.toString()).equals(r)}.
     * However, the method also handles rational numbers expressed in the
     * following forms:</p>
     * <p>
     * "<i>num</i>{@code /}<i>den</i>" or
     * "<i>num</i>{@code :}<i>den</i>" {@code => new Rational(num, den);},
     * where <i>num</i> and <i>den</i> are string integers potentially
     * containing a sign, such as "-10", "+7" or "5".</p>
     *
     * <pre>{@code
     * Rational.parseRational("3:+6").equals(new Rational(1, 2)) == true
     * Rational.parseRational("-3/-6").equals(new Rational(1, 2)) == true
     * Rational.parseRational("4.56") => throws NumberFormatException
     * }</pre>
     *
     * @param string the string representation of a rational value.
     * @return the rational value represented by {@code string}.
     *
     * @throws NumberFormatException if {@code string} cannot be parsed
     * as a rational value.
     * @throws NullPointerException if {@code string} was {@code null}
     */
    CARAPI ParseRational(
        /* [in] */ const String& string,
        /* [out] */ IRational** outration);
};

}
}
}

#endif // __ELASTOS_DROID_UTILITY_CRATIONAL_HELPER_H__
