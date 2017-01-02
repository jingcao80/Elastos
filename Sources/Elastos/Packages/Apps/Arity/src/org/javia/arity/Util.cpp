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

#include "org/javia/arity/Util.h"
#include "org/javia/arity/MoreMath.h"
#include "org/javia/arity/CComplex.h"
#include "elastos/core/Math.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/core/StringUtils.h"

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;

namespace Org {
namespace Javia {
namespace Arity {

const Int32 Util::LEN_UNLIMITED;
const Int32 Util::FLOAT_PRECISION;

Double Util::ShortApprox(
    /* [in] */ Double value,
    /* [in] */ Double maxError)
{
    Double v = Elastos::Core::Math::Abs(value);
    Double tail = MoreMath::IntExp10(MoreMath::IntLog10(Elastos::Core::Math::Abs(maxError)));
    Double ret = Elastos::Core::Math::Floor(v / tail + .5) * tail;
    return (value < 0) ? -ret : ret;
}

String Util::SizeTruncate(
    /* [in] */ const String& str,
    /* [in] */ Int32 maxLen)
{
    if (maxLen == LEN_UNLIMITED) {
        return str;
    }
    Int32 ePos = str.LastIndexOf('E');
    String tail = (ePos != -1) ? str.Substring(ePos) : String("");
    Int32 tailLen = tail.GetLength();
    Int32 headLen = str.GetLength() - tailLen;
    Int32 maxHeadLen = maxLen - tailLen;
    Int32 keepLen = Elastos::Core::Math::Min(headLen, maxHeadLen);
    if (keepLen < 1 || (keepLen < 2 && str.GetLength() > 0 && str.GetChar(0) == '-')) {
        return str; // impossible to truncate
    }
    Int32 dotPos = str.IndexOf('.');
    if (dotPos == -1) {
        dotPos = headLen;
    }
    if (dotPos > keepLen) {
        Int32 exponent = (ePos != -1) ? StringUtils::ParseInt32(str.Substring(ePos + 1)) : 0;
        Int32 start = str.GetChar(0) == '-' ? 1 : 0;
        exponent += dotPos - start - 1;
        String newStr = str.Substring(0, start+1) + "." + str.Substring(start + 1, headLen) + "E" + StringUtils::ToString(exponent);
        return SizeTruncate(newStr, maxLen);

    }
    return str.Substring(0, keepLen) + tail;
}

String Util::DoubleToString(
    /* [in] */ Double v,
    /* [in] */ Int32 roundingDigits)
{
    Double absv = Elastos::Core::Math::Abs(v);
    String str = roundingDigits == FLOAT_PRECISION ? StringUtils::ToString((Float)absv) : StringUtils::ToString(absv);
    StringBuilder buf(str);
    Int32 roundingStart = (roundingDigits <= 0 || roundingDigits > 13) ? 17 : (16 - roundingDigits);

    Int32 ePos = str.LastIndexOf('E');
    Int32 exp  =  (ePos != -1) ? StringUtils::ParseInt32(str.Substring(ePos + 1)) : 0;
    if (ePos != -1) {
        buf.SetLength(ePos);
    }
    Int32 len = buf.GetLength();

    //remove dot
    Int32 dotPos;
    for (dotPos = 0; dotPos < len && buf.GetCharAt(dotPos) != '.';) {
        ++dotPos;
    }
    exp += dotPos;
    if (dotPos < len) {
        buf.DeleteCharAt(dotPos);
        --len;
    }

    //round
    for (Int32 p = 0; p < len && buf.GetCharAt(p) == '0'; ++p) {
        ++roundingStart;
    }

    if (roundingStart < len) {
        if (buf.GetCharAt(roundingStart) >= '5') {
            Int32 p;
            for (p = roundingStart - 1; p >= 0 && buf.GetCharAt(p)=='9'; --p) {
                buf.SetCharAt(p, '0');
            }
            if (p >= 0) {
                buf.SetCharAt(p, (Char32)(buf.GetCharAt(p) + 1));
            }
            else {
                buf.InsertChar(0, '1');
                ++roundingStart;
                ++exp;
            }
        }
        buf.SetLength(roundingStart);
    }
    //re-insert dot
    if ((exp < -5) || (exp > 10)) {
        buf.InsertChar(1, '.');
        --exp;
    }
    else {
        for (Int32 i = len; i < exp; ++i) {
            buf.AppendChar('0');
        }
        for (Int32 i = exp; i <= 0; ++i) {
            buf.InsertChar(0, '0');
        }
        buf.InsertChar((exp <= 0)? 1 : exp, '.');
        exp = 0;
    }
    len = buf.GetLength();

    //remove trailing dot and 0s.
    Int32 tail;
    for (tail = len - 1; tail >= 0 && buf.GetCharAt(tail) == '0'; --tail) {
        buf.DeleteCharAt(tail);
    }
    if (tail >= 0 && buf.GetCharAt(tail) == '.') {
        buf.DeleteCharAt(tail);
    }

    if (exp != 0) {
        buf.AppendChar('E');
        buf.Append(exp);
    }
    if (v < 0) {
        buf.InsertChar(0, '-');
    }
    return buf.ToString();
}

String Util::DoubleToString(
    /* [in] */ Double x,
    /* [in] */ Int32 maxLen,
    /* [in] */ Int32 rounding)
{
    return SizeTruncate(DoubleToString(x, rounding), maxLen);
}

String Util::ComplexToString(
    /* [in] */ IComplex* _x,
    /* [in] */ Int32 maxLen,
    /* [in] */ Int32 rounding)
{
    //System.out.println("" + x.re + ' ' + x.im);
    AutoPtr<CComplex> x = (CComplex*)_x;
    if (x->mIm == 0) {
        return DoubleToString(x->mRe, maxLen, rounding);
    }

    Boolean isNaN;
    if (x->IsNaN(&isNaN), isNaN) {
        return String("NaN");
    }

    Double xre = x->mRe, xim = x->mIm;
    Boolean isInfinite;
    if (x->IsInfinite(&isInfinite), isInfinite) {
        if (!Elastos::Core::Math::IsInfinite(xre)) {
            xre = 0;
        }
        else if (!Elastos::Core::Math::IsInfinite(xim)) {
            xim = 0;
        }
    }
    if (xim == 0) {
        return DoubleToString(xre, maxLen, rounding);
    }

    // insert plus between re & im
    Boolean addPlus = xre != 0 && !(xim < 0);
    String sre = xre == 0 ? String("") : DoubleToString(xre, rounding);
    String sim = DoubleToString(xim, rounding);
    String finalMultiply = Elastos::Core::Math::IsInfinite(xim) ? String("*") : String("");

    if (sim.Equals("1")) {
        sim = String("");
    }
    if (sim.Equals("-1")) {
        sim = String("-");
    }

    if (maxLen != LEN_UNLIMITED) {
        --maxLen; // for final "i"
        if (addPlus) {
            --maxLen;
        }
        maxLen -= finalMultiply.GetLength();

        Int32 sreLen = sre.GetLength();
        Int32 simLen = sim.GetLength();
        Int32 reduce = sreLen + simLen - maxLen;
        if (reduce > 0) {
            Int32 diff   = Elastos::Core::Math::Abs(sreLen - simLen);
            Int32 rShort = reduce > diff ? (reduce - diff) / 2 : 0;
            Int32 rLong  = rShort + Elastos::Core::Math::Min(reduce, diff);
            Int32 sreTarget = sreLen;
            Int32 simTarget = simLen;
            if (sreLen > simLen) {
                sreTarget -= rLong;
                simTarget -= rShort;
            }
            else {
                sreTarget -= rShort;
                simTarget -= rLong;
            }
            if (sreTarget + simTarget > maxLen) {
                --simTarget;
            }
            sre = SizeTruncate(sre, sreTarget);
            sim = SizeTruncate(sim, simTarget);
        }
    }
    return sre + (addPlus ? "+" : "") + sim + finalMultiply + "i";
}

} // namespace Arity
} // namespace Javia
} // namespace Org