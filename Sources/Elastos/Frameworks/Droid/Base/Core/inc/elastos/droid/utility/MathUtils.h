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

#ifndef __ELASTOS_DROID_UTILITY_MATH_UTILSS_H__
#define __ELASTOS_DROID_UTILITY_MATH_UTILSS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Math.h>

using Elastos::Utility::IRandom;

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * A class that contains utility methods related to numbers.
 *
 * @hide Pending API council approval
 */
class ECO_PUBLIC MathUtils
{
public:
    static Float Abs(Float v) {
        return v > 0 ? v : -v;
    }

    static Int32 Constrain(Int32 amount, Int32 low, Int32 high) {
        return amount < low ? low : (amount > high ? high : amount);
    }

    static Int64 Constrain(Int64 amount, Int64 low, Int64 high) {
        return amount < low ? low : (amount > high ? high : amount);
    }

    static Float Constrain(Float amount, Float low, Float high) {
        return amount < low ? low : (amount > high ? high : amount);
    }

    static Double Constrain(Double amount, Double low, Double high) {
        return amount < low ? low : (amount > high ? high : amount);
    }

    static Float Log(Float a) {
        return (Float) Elastos::Core::Math::Log(a);
    }

    static Float Exp(Float a) {
        return (Float) Elastos::Core::Math::Exp(a);
    }

    static Float Pow(Float a, Float b) {
        return (Float) Elastos::Core::Math::Pow(a, b);
    }

    static Float Max(Float a, Float b) {
        return a > b ? a : b;
    }

    static Float Max(Int32 a, Int32 b) {
        return a > b ? a : b;
    }

    static Float Max(Float a, Float b, Float c) {
        return a > b ? (a > c ? a : c) : (b > c ? b : c);
    }

    static Float Max(Int32 a, Int32 b, Int32 c) {
        return a > b ? (a > c ? a : c) : (b > c ? b : c);
    }

    static Float Min(Float a, Float b) {
        return a < b ? a : b;
    }

    static Float Min(Int32 a, Int32 b) {
        return a < b ? a : b;
    }

    static Float Min(Float a, Float b, Float c) {
        return a < b ? (a < c ? a : c) : (b < c ? b : c);
    }

    static Float Min(Int32 a, Int32 b, Int32 c) {
        return a < b ? (a < c ? a : c) : (b < c ? b : c);
    }

    static Float Dist(Float x1, Float y1, Float x2, Float y2) {
        Float x = (x2 - x1);
        Float y = (y2 - y1);
        return (Float) Elastos::Core::Math::Sqrt(x * x + y * y);
    }

    static Float Dist(Float x1, Float y1, Float z1, Float x2, Float y2, Float z2) {
        Float x = (x2 - x1);
        Float y = (y2 - y1);
        Float z = (z2 - z1);
        return (Float) Elastos::Core::Math::Sqrt(x * x + y * y + z * z);
    }

    static Float Mag(Float a, Float b) {
        return (Float) Elastos::Core::Math::Sqrt(a * a + b * b);
    }

    static Float Mag(Float a, Float b, Float c) {
        return (Float) Elastos::Core::Math::Sqrt(a * a + b * b + c * c);
    }

    static Float Sq(Float v) {
        return v * v;
    }

    static Float Radians(Float degrees) {
        return degrees * DEG_TO_RAD;
    }

    static Float Degrees(Float radians) {
        return radians * RAD_TO_DEG;
    }

    static Float Acos(Float value) {
        return (Float) Elastos::Core::Math::Acos(value);
    }

    static Float Asin(Float value) {
        return (Float) Elastos::Core::Math::Asin(value);
    }

    static Float Atan(Float value) {
        return (Float) Elastos::Core::Math::Atan(value);
    }

    static Float Atan2(Float a, Float b) {
        return (Float) Elastos::Core::Math::Atan2(a, b);
    }

    static Float Tan(Float angle) {
        return (Float) Elastos::Core::Math::Tan(angle);
    }

    static Float Lerp(Float start, Float stop, Float amount) {
        return start + (stop - start) * amount;
    }

    static Float Norm(Float start, Float stop, Float value) {
        return (value - start) / (stop - start);
    }

    static Float Map(Float minStart, Float minStop, Float maxStart, Float maxStop, Float value) {
        return maxStart + (maxStart - maxStop) * ((value - minStart) / (minStop - minStart));
    }

    static Int32 Random(Int32 howbig);

    static Int32 Random(Int32 howsmall, Int32 howbig);

    static Float Random(Float howbig);

    static Float Random(Float howsmall, Float howbig);

    static void RandomSeed(Int64 seed);

private:
    static const AutoPtr<IRandom> sRandom;// = new Random();
    static const Float DEG_TO_RAD;// = 3.1415926f / 180.0f;
    static const Float RAD_TO_DEG;// = 180.0f / 3.1415926f;

private:
    ECO_LOCAL MathUtils();
    ECO_LOCAL MathUtils(const MathUtils&);
};

} // namespace Utility
} // namepsace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_UTILITY_MATH_UTILSS_H__
