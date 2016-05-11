#ifndef __ELASTOS_MATH_CBIGDECIMALHELPER_H__
#define __ELASTOS_MATH_CBIGDECIMALHELPER_H__

#include "_Elastos_Math_CBigDecimalHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Math {

CarClass(CBigDecimalHelper)
    , public Singleton
    , public IBigDecimalHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Returns a new {@code BigDecimal} instance whose value is equal to {@code
     * unscaledVal} 10^(-{@code scale}). The scale of the result is {@code
     * scale}, and its unscaled value is {@code unscaledVal}.
     *
     * @param unscaledVal
     *            unscaled value to be used to construct the new {@code
     *            BigDecimal}.
     * @param scale
     *            scale to be used to construct the new {@code BigDecimal}.
     * @return {@code BigDecimal} instance with the value {@code unscaledVal}*
     *         10^(-{@code unscaledVal}).
     */
    CARAPI ValueOf(
        /* [in] */ Int64 unscaledVal,
        /* [in] */ Int32 scale,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} instance whose value is equal to {@code
     * unscaledVal}. The scale of the result is {@code 0}, and its unscaled
     * value is {@code unscaledVal}.
     *
     * @param unscaledVal
     *            value to be converted to a {@code BigDecimal}.
     * @return {@code BigDecimal} instance with the value {@code unscaledVal}.
     */
    CARAPI ValueOf(
        /* [in] */ Int64 unscaledVal,
        /* [out] */ IBigDecimal** result);

    /**
     * Returns a new {@code BigDecimal} instance whose value is equal to {@code
     * val}. The new decimal is constructed as if the {@code BigDecimal(String)}
     * constructor is called with an argument which is equal to {@code
     * Double.toString(val)}. For example, {@code valueOf("0.1")} is converted to
     * (unscaled=1, scale=1), although the double {@code 0.1} cannot be
     * represented exactly as a double value. In contrast to that, a new {@code
     * BigDecimal(0.1)} instance has the value {@code
     * 0.1000000000000000055511151231257827021181583404541015625} with an
     * unscaled value {@code 1000000000000000055511151231257827021181583404541015625}
     * and the scale {@code 55}.
     *
     * @param valprivate:
     *            double value to be converted to a {@code BigDecimal}.
     * @return {@code BigDecimal} instance with the value {@code val}.
     * @throws NumberFormatException
     *             if {@code val} is infinite or {@code val} is not a number
     */
    CARAPI ValueOf(
        /* [in] */ Double val,
        /* [out] */ IBigDecimal** result);

    CARAPI GetZERO(
        /* [out] */ IBigDecimal** zero);

    CARAPI GetONE(
        /* [out] */ IBigDecimal** one);

    CARAPI GetTEN(
        /* [out] */ IBigDecimal** ten);
};

} // namespace Math
} // namespace Elastos

#endif // __ELASTOS_MATH_CBIGDECIMALHELPER_H__
