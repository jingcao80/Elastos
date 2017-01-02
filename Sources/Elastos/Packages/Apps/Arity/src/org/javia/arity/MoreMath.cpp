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

#include "org/javia/arity/MoreMath.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <math.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Org {
namespace Javia {
namespace Arity {

const Double MoreMath::LOG2E;

static AutoPtr<ArrayOf<Double> > InitGamma()
{
    AutoPtr<ArrayOf<Double> > doubles = ArrayOf<Double>::Alloc(14);
    (*doubles)[0] = 57.156235665862923517;
    (*doubles)[1] = -59.597960355475491248;
    (*doubles)[2] = 14.136097974741747174;
    (*doubles)[3] = -0.49191381609762019978;
    (*doubles)[4] = .33994649984811888699e-4;
    (*doubles)[5] = .46523628927048575665e-4;
    (*doubles)[6] = -.98374475304879564677e-4;
    (*doubles)[7] = .15808870322491248884e-3;
    (*doubles)[8] = -.21026444172410488319e-3;
    (*doubles)[9] = .21743961811521264320e-3;
    (*doubles)[10] = -.16431810653676389022e-3;
    (*doubles)[11] = .84418223983852743293e-4;
    (*doubles)[12] = -.26190838401581408670e-4;
    (*doubles)[13] = .36899182659531622704e-5;
    return doubles;
}
const AutoPtr<ArrayOf<Double> > MoreMath::GAMMA = InitGamma();

static AutoPtr<ArrayOf<Double> > InitFact()
{
    AutoPtr<ArrayOf<Double> > doubles = ArrayOf<Double>::Alloc(22);
    (*doubles)[0] = 1.0;
    (*doubles)[1] = 40320.0;
    (*doubles)[2] = 2.0922789888E13;
    (*doubles)[3] = 6.204484017332394E23;
    (*doubles)[4] = 2.631308369336935E35;
    (*doubles)[5] = 8.159152832478977E47;
    (*doubles)[6] = 1.2413915592536073E61;
    (*doubles)[7] = 7.109985878048635E74;
    (*doubles)[8] = 1.2688693218588417E89;
    (*doubles)[9] = 6.1234458376886085E103;
    (*doubles)[10] = 7.156945704626381E118;
    (*doubles)[11] = 1.8548264225739844E134;
    (*doubles)[12] = 9.916779348709496E149;
    (*doubles)[13] = 1.0299016745145628E166;
    (*doubles)[14] = 1.974506857221074E182;
    (*doubles)[15] = 6.689502913449127E198;
    (*doubles)[16] = 3.856204823625804E215;
    (*doubles)[17] = 3.659042881952549E232;
    (*doubles)[18] = 5.5502938327393044E249;
    (*doubles)[19] = 1.3113358856834524E267;
    (*doubles)[20] = 4.7147236359920616E284;
    (*doubles)[21] = 2.5260757449731984E302;
    return doubles;
}
const AutoPtr<ArrayOf<Double> > MoreMath::FACT = InitFact();

Double MoreMath::Asinh(
    /* [in] */ Double x)
{
    return (x < 0) ?
            -Asinh(-x) :
            Elastos::Core::Math::Log(x + x + 1 / (Elastos::Core::Math::Sqrt(x * x + 1) + x));
}

Double MoreMath::Acosh(
    /* [in] */ Double x)
{
    return Elastos::Core::Math::Log(x + x - 1 / (Elastos::Core::Math::Sqrt(x * x - 1) + x));
}

Double MoreMath::Atanh(
    /* [in] */ Double x)
{
    return (x < 0) ? -Atanh(-x) : 0.5 * Elastos::Core::Math::Log(1. + (x + x) / (1 - x));
}

Double MoreMath::Trunc(
    /* [in] */ Double x)
{
    return x >= 0 ? Elastos::Core::Math::Floor(x) : Elastos::Core::Math::Ceil(x);
}

Double MoreMath::Gcd(
    /* [in] */ Double x,
    /* [in] */ Double y)
{
    if (Elastos::Core::Math::IsNaN(x) || Elastos::Core::Math::IsNaN(y) ||
            Elastos::Core::Math::IsInfinite(x) || Elastos::Core::Math::IsInfinite(y)) {
        return Elastos::Core::Math::DOUBLE_NAN;
    }
    x = Elastos::Core::Math::Abs(x);
    y = Elastos::Core::Math::Abs(y);
    while (x < y * 1e15) {
        Double save = y;
        y = fmod(x, y);
        x = save;
    }
    return x;
}

Double MoreMath::Lgamma(
    /* [in] */ Double x)
{
    Double tmp = x + 5.2421875; //== 607/128. + .5;
    Double sum = 0.99999999999999709182;
    for (Int32 i = 0; i < GAMMA->GetLength(); ++i) {
        sum += (*GAMMA)[i] / ++x;
    }

    return 0.9189385332046727418 //LN_SQRT2PI, ln(sqrt(2*pi))
            + Elastos::Core::Math::Log(sum)
            + (tmp - 4.7421875) * Elastos::Core::Math::Log(tmp) - tmp;
}

Double MoreMath::Factorial(
    /* [in] */ Double x)
{
    if (x < 0) { // x <= -1 ?
        return Elastos::Core::Math::DOUBLE_NAN;
    }
    if (x <= 170) {
        if (Elastos::Core::Math::Floor(x) == x) {
            Int32 n = (Int32)x;
            Double extra = x;
            switch (n & 7) {
                case 7: extra *= --x;
                case 6: extra *= --x;
                case 5: extra *= --x;
                case 4: extra *= --x;
                case 3: extra *= --x;
                case 2: extra *= --x;
                case 1: return (*FACT)[n >> 3] * extra;
                case 0: return (*FACT)[n >> 3];
            }
        }
    }
    return Elastos::Core::Math::Exp(Lgamma(x));
}

Double MoreMath::Combinations(
    /* [in] */ Double n,
    /* [in] */ Double k)
{
    if (n < 0 || k < 0) {
        return Elastos::Core::Math::DOUBLE_NAN;
    }
    if (n < k) {
        return 0;
    }
    if (Elastos::Core::Math::Floor(n) == n && Elastos::Core::Math::Floor(k) == k) {
        k = Elastos::Core::Math::Min(k, n - k);
        if (n <= 170 && 12 < k && k <= 170) {
            return Factorial(n) / Factorial(k) / Factorial(n-k);
        }
        else {
            Double r = 1, diff = n-k;
            for (Double i = k; i > .5 && r < Elastos::Core::Math::DOUBLE_POSITIVE_INFINITY; --i) {
                r *= (diff+i) / i;
            }
            return r;
        }
    }
    else {
        return Elastos::Core::Math::Exp(Lgamma(n) - Lgamma(k) - Lgamma(n-k));
    }
}

Double MoreMath::Permutations(
    /* [in] */ Double n,
    /* [in] */ Double k)
{
    if (n < 0 || k < 0) {
        return Elastos::Core::Math::DOUBLE_NAN;
    }
    if (n < k) {
        return 0;
    }
    if (Elastos::Core::Math::Floor(n) == n && Elastos::Core::Math::Floor(k) == k) {
        if (n <= 170 && 10 < k && k <= 170) {
            return Factorial(n) / Factorial(n-k);
        }
        else {
            Double r = 1, limit = n - k + .5;
            for (Double i = n; i > limit && r < Elastos::Core::Math::DOUBLE_POSITIVE_INFINITY; --i) {
                r *= i;
            }
            return r;
        }
    }
    else {
        return Elastos::Core::Math::Exp(Lgamma(n) - Lgamma(n-k));
    }
}

Double MoreMath::Log2(
    /* [in] */ Double x)
{
    return Elastos::Core::Math::Log(x) * LOG2E;
}

Boolean MoreMath::IsPiMultiple(
    /* [in] */ Double x)
{
    // x % y == 0
    Double d = x / Elastos::Core::Math::PI;
    return d == Elastos::Core::Math::Floor(d);
}

Double MoreMath::Sin(
    /* [in] */ Double x)
{
    return IsPiMultiple(x) ? 0 : Elastos::Core::Math::Sin(x);
}

Double MoreMath::Cos(
    /* [in] */ Double x)
{
    return IsPiMultiple(x - Elastos::Core::Math::PI / 2) ? 0 : Elastos::Core::Math::Cos(x);
}

Double MoreMath::Tan(
    /* [in] */ Double x)
{
    return IsPiMultiple(x) ? 0 : Elastos::Core::Math::Tan(x);
}

Int32 MoreMath::IntLog10(
    /* [in] */ Double x)
{
    //an alternative implem is using a for loop.
    return (Int32)Elastos::Core::Math::Floor(Elastos::Core::Math::Log10(x));
    //return (int)log10(x);
}

Double MoreMath::IntExp10(
    /* [in] */ Int32 exp)
{
    StringBuilder sb("1E");
    sb += exp;
    return StringUtils::ParseDouble(sb.ToString());
}

} // namespace Arity
} // namespace Javia
} // namespace Org