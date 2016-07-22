
#include "org/javia/arity/CComplex.h"
#include "org/javia/arity/MoreMath.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Math.h>
#include <math.h>

using Elastos::Core::StringBuilder;
using Org::Javia::Arity::EIID_IComplex;

namespace Org {
namespace Javia {
namespace Arity {

CAR_OBJECT_IMPL(CComplex)

CAR_INTERFACE_IMPL(CComplex, Object, IComplex)

ECode CComplex::constructor()
{
    return NOERROR;
}

ECode CComplex::constructor(
    /* [in] */ Double re,
    /* [in] */ Double im)
{
    return Set(re, im);
}

ECode CComplex::constructor(
    /* [in] */ IComplex* o)
{
    return Set(o);
}

ECode CComplex::Set(
    /* [in] */ Double re,
    /* [in] */ Double im)
{
    mRe = re;
    mIm = im;
    return NOERROR;
}

ECode CComplex::Set(
    /* [in] */ IComplex* o)
{
    AutoPtr<CComplex> obj = (CComplex*)o;
    mRe = obj->mRe;
    mIm = obj->mIm;
    return NOERROR;
}

ECode CComplex::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb;
    if (mIm == 0) {
        sb.Append("");
        sb.Append(mRe);
    }
    else {
        sb.Append("(");
        sb.Append(mRe);
        sb.Append(", ");
        sb.Append(mIm);
        sb.Append(")");
    }
    *str = sb.ToString();
    return NOERROR;
}

ECode CComplex::AsReal(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIm == 0 ? mRe : Elastos::Core::Math::DOUBLE_NAN;
    return NOERROR;
    //return Math.abs(im) < 1E-30 ? re : Double.NaN;
}

ECode CComplex::Conjugate()
{
    return Set(mRe, -mIm);
}

ECode CComplex::Negate()
{
    return Set(-mRe, -mIm);
}

ECode CComplex::IsInfinite(
    /* [out] */ Boolean* isInfinite)
{
    VALIDATE_NOT_NULL(isInfinite)
    Boolean isNan;
    *isInfinite = Elastos::Core::Math::IsInfinite(mRe) ||
            (Elastos::Core::Math::IsInfinite(mIm) && (IsNaN(&isNan), !isNan));
    return NOERROR;
}

ECode CComplex::IsFinite(
    /* [out] */ Boolean* isFinite)
{
    VALIDATE_NOT_NULL(isFinite)
    Boolean isInfinite, isNan;
    *isFinite = (IsInfinite(&isInfinite), !isInfinite) && (IsNaN(&isNan), !isNan);
    return NOERROR;
}

ECode CComplex::IsNaN(
    /* [out] */ Boolean* isNan)
{
    VALIDATE_NOT_NULL(isNan)
    *isNan = Elastos::Core::Math::IsNaN(mRe) || Elastos::Core::Math::IsNaN(mIm);
    return NOERROR;
}

ECode CComplex::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<CComplex> o = (CComplex*)IComplex::Probe(other);
    *result = ((mRe == o->mRe) || (mRe != mRe && o->mRe != o->mRe)) &&
            ((mIm == o->mIm) || (mIm != mIm && o->mIm != o->mIm));
    return NOERROR;
}

ECode CComplex::Arg(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = Elastos::Core::Math::Atan2(mIm, mRe);
    return NOERROR;
}

ECode CComplex::Abs(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    Double a = Elastos::Core::Math::Abs(mRe);
    Double b = Elastos::Core::Math::Abs(mIm);
    if (a == 0 || b == 0) {
        *result = a + b;
        return NOERROR;
    }
    Boolean aGreater = a > b;
    Double q = aGreater ? b / a : a / b;
    *result = (aGreater ? a : b) * Elastos::Core::Math::Sqrt(1 + q*q);
    return NOERROR;
}

ECode CComplex::Abs2(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRe * mRe + mIm * mIm;
    return NOERROR;
}

ECode CComplex::Add(
    /* [in] */ IComplex* o)
{
    Double ulp = Elastos::Core::Math::Ulp(mRe);
    AutoPtr<CComplex> obj = (CComplex*)o;
    mRe += obj->mRe;
    mIm += obj->mIm;
    // hack for "1.1-1-.1"
    if (Elastos::Core::Math::Abs(mRe) < ulp * 1024) {
        mRe = 0;
    }
    return NOERROR;
}

ECode CComplex::Sub(
    /* [in] */ IComplex* o)
{
    Double ulp = Elastos::Core::Math::Ulp(mRe);
    AutoPtr<CComplex> obj = (CComplex*)o;
    mRe += obj->mRe;
    mIm += obj->mIm;
    // hack for "1.1-1-.1"
    if (Elastos::Core::Math::Abs(mRe) < ulp * 1024) {
        mRe = 0;
    }
    return NOERROR;
}

ECode CComplex::Mul(
    /* [in] */ Double o)
{
    mRe *= o;
    mIm *= o;
    return NOERROR;
}

ECode CComplex::Mul(
    /* [in] */ IComplex* o)
{
    Double a = mRe, b = mIm;
    AutoPtr<CComplex> obj = (CComplex*)o;
    Double c = obj->mRe, d = obj->mIm;
    if (b == 0 && d == 0) {
        return Set(a * c, 0);
    }

    Double re = a * c - b * d;
    Double im = a * d + b * c;

    Set(re, im);
    Boolean isNan;
    if (IsNaN(&isNan), !isNan) {
        return NOERROR;
    }

    Set(a, b);
    Boolean isInfinite;
    if (IsInfinite(&isInfinite), isInfinite) {
        NormalizeInfinity();
        a = mRe;
        b = mIm;
    }

    if (o->IsInfinite(&isInfinite), isInfinite) {
        Set(c, d);
        NormalizeInfinity();
        c = mRe;
        d = mIm;
    }

    if (b == 0) {
        if (d == 0) {
            return Set(a * c, 0);
        }
        if (c == 0) {
            return Set(0, a * d);
        }
        return Set(a * c, a * d);
    }

    if (a == 0) {
        if (c == 0) {
            return Set(-b*d, 0);
        }
        if (d == 0) {
            return Set(0, b*c);
        }
        return Set(-b*d, b*c);
    }

    if (d == 0) {
        return Set(a*c, b*c);
    }
    if (c == 0) {
        return Set(-b*d, a*d);
    }
    return Set(re, im);
}

ECode CComplex::Div(
    /* [in] */ IComplex* o)
{
    AutoPtr<CComplex> obj = (CComplex*)o;
    Double c = obj->mRe;
    Double d = obj->mIm;
    if (mIm == 0 && d == 0) {
        return Set(mRe/c, 0);
    }
    Boolean isInfinite, isFinite;
    if ((o->IsInfinite(&isInfinite), isInfinite) && (IsFinite(&isFinite), isFinite)) {
        return Set(0, 0);
    }
    if (d == 0) {
        if (mRe == 0) {
            return Set(0, mIm/c);
        }
        return Set(mRe/c, mIm/c);
    }
    if (c == 0) {
        return Set(mIm/d, -mRe/d);
    }
    if (Elastos::Core::Math::Abs(c) > Elastos::Core::Math::Abs(d)) {
        Double q = d / c;
        Double down = c + d*q;
        return Set((mRe + mIm*q)/down, (mIm - mRe*q)/down);
    }
    else {
        Double q = c / d;
        Double down = c*q + d;
        return Set((mRe*q + mIm)/down, (mIm*q - mRe)/down);
    }
}

ECode CComplex::Sqrt()
{
    if (mIm == 0) {
        if (!(mRe < 0)) {
            Set(Elastos::Core::Math::Sqrt(mRe), 0);
        }
        else {
            Set(0, Elastos::Core::Math::Sqrt(-mRe));
        }
    }
    else {
        Double abs;
        Abs(&abs);
        Double t = Elastos::Core::Math::Sqrt((Elastos::Core::Math::Abs(mRe) + abs) / 2);
        if (mRe >= 0) {
            Set(t, mIm / (t + t));
        }
        else {
            Set(Elastos::Core::Math::Abs(mIm) / (t + t), mIm >= 0 ? t : -t);
        }
    }
    return NOERROR;
}

ECode CComplex::Mod(
    /* [in] */ IComplex* o)
{
    Double a = mRe;
    Double b = mIm;
    AutoPtr<CComplex> obj = (CComplex*)o;
    if (b == 0 && obj->mIm == 0) {
        return Set(fmod(a, obj->mRe), 0);
    }
    Div(o);
    Set(Elastos::Core::Math::Rint(mRe), Elastos::Core::Math::Rint(mIm));
    Mul(o);
    Set(a - mRe, b - mIm);
    return NOERROR;
}

ECode CComplex::Gcd(
    /* [in] */ IComplex* o)
{
    AutoPtr<CComplex> obj = (CComplex*)o;
    if (mIm == 0 && obj->mIm == 0) {
        return Set(MoreMath::Gcd(mRe, obj->mRe), 0);
    }
    AutoPtr<CComplex> y;
    CComplex::NewByFriend(o, (CComplex**)&y);
    Double xabs2;
    Abs2(&xabs2);
    Double yabs2;
    y->Abs2(&yabs2);
    while (xabs2 < yabs2 * 1e30) {
        Double yRe = y->mRe;
        Double yIm = y->mIm;
        Mod((IComplex*)y);
        y->Set((IComplex*)this);
        Set(yRe, yIm);
        xabs2 = yabs2;
        y->Abs2(&yabs2);
    }
    // normalize to positive & larger real
    if (Elastos::Core::Math::Abs(mRe) < Elastos::Core::Math::Abs(mIm)) {
        Set(-mIm, mRe);
    }
    if (mRe < 0) {
        Negate();
    }
    return NOERROR;
}

ECode CComplex::Log()
{
    if (mIm == 0 && !(mRe < 0)) {
        return Set(Elastos::Core::Math::Log(mRe), 0);
    }
    Double newIm = Elastos::Core::Math::Atan2(mIm, mRe);
    Double d;
    Abs(&d);
    return Set(Elastos::Core::Math::Log(d), newIm);
}

ECode CComplex::Exp()
{
    Double expRe = Elastos::Core::Math::Exp(mRe);
    if (mIm == 0) {
        return Set(expRe, 0);
    }
    else {
        return Set(expRe * MoreMath::Cos(mIm), expRe * MoreMath::Sin(mIm));
    }
}

ECode CComplex::Square()
{
    return Set(mRe*mRe - mIm*mIm, 2*mRe*mIm);
}

ECode CComplex::Pow(
    /* [in] */ IComplex* _y)
{
    AutoPtr<CComplex> y = (CComplex*)_y;
    if (y->mIm == 0) {
        if (y->mRe == 0) {
            // anything^0==1, including NaN^0 (!)
            return Set(1, 0);
        }
        if (mIm == 0) {
            Double res = Elastos::Core::Math::Pow(mRe, y->mRe);
            if (res == res) { // !NaN
                return Set(res, 0);
            }
        }
        if (y->mRe == 2) {
            return Square();
        }
        if (y->mRe == .5) {
            return Sqrt();
        }
        Double d;
        Abs2(&d);
        Double p = Elastos::Core::Math::Pow(d, y->mRe/2);
        Double arg;
        Arg(&arg);
        Double a = arg * y->mRe;
        return Set(p * MoreMath::Cos(a), p * MoreMath::Sin(a));
    }
    if (mIm == 0 && mRe > 0) {
        Double a = Elastos::Core::Math::Pow(mRe, y->mRe);
        Set(0, y->mIm*Elastos::Core::Math::Log(mRe));
        Exp();
        Set(a*mRe, a*mIm);
        return NOERROR;
    }
    Log();
    Set(y->mRe*mRe - y->mIm*mIm, y->mRe*mIm + y->mIm*mRe);
    Exp();
    return NOERROR;
}

ECode CComplex::Lgamma()
{
    Double sumRe = 0.99999999999999709182, sumIm = 0;
    Double down = mRe * mRe + mIm * mIm;
    Double xplusk = mRe;
    AutoPtr<ArrayOf<Double> > GAMMA = MoreMath::GAMMA;
    for (Int32 k = 0; k < GAMMA->GetLength(); ++k) {
        ++xplusk;
        down += xplusk + xplusk - 1;
        Double cc = (*GAMMA)[k];
        sumRe += cc * xplusk / down;
        sumIm -= cc * mIm     / down;
    }

    Double a = mRe + .5;
    Double tmpRe = mRe + 5.2421875;
    Double saveIm = mIm;

    mRe = tmpRe;

    Log();
    Double termRe = a * mRe - saveIm * mIm + 0.9189385332046727418 - tmpRe;
    Double termIm = a * mIm + saveIm * mRe - saveIm;

    Set(sumRe, sumIm);
    Log();
    mRe += termRe;
    mIm += termIm;
    return NOERROR;
}

ECode CComplex::Factorial()
{
    if (mIm == 0) {
        Set(MoreMath::Factorial(mRe), 0);
    }
    else {
        Lgamma();
        Exp();
    }
    return NOERROR;
}

ECode CComplex::Sin()
{
    return mIm == 0 ?
            Set(MoreMath::Sin(mRe), 0) :
            Set(MoreMath::Sin(mRe) * Elastos::Core::Math::Cosh(mIm), MoreMath::Cos(mRe) * Elastos::Core::Math::Sinh(mIm));
}

ECode CComplex::Sinh()
{
    if (mIm == 0) {
        Set(Elastos::Core::Math::Sinh(mRe), 0);
    }
    else {
        Swap();
        Sin();
        Swap();
    }
    return NOERROR;
}

ECode CComplex::Cos()
{
    return mIm == 0 ?
            Set(MoreMath::Cos(mRe), 0) :
            Set(MoreMath::Cos(mRe) * Elastos::Core::Math::Cosh(mIm), -MoreMath::Sin(mRe) * Elastos::Core::Math::Sinh(mIm));
}

ECode CComplex::Cosh()
{
    if (mIm == 0) {
        Set(Elastos::Core::Math::Cosh(mRe), 0);
    }
    else {
        Swap();
        Cos();
        Conjugate();
    }
    return NOERROR;
}

ECode CComplex::Tan()
{
    if (mIm == 0) {
        return Set(MoreMath::Tan(mRe), 0);
    }
    Double aa = mRe + mRe;
    Double bb = mIm + mIm;
    Double down = MoreMath::Cos(aa) + Elastos::Core::Math::Cosh(bb);
    return Set(MoreMath::Sin(aa)/down, Elastos::Core::Math::Sinh(bb)/down);
}

ECode CComplex::Tanh()
{
    if (mIm == 0) {
        Set(Elastos::Core::Math::Tanh(mRe), 0);
    }
    else {
        Swap();
        Tan();
        Swap();
    }
    return NOERROR;
}

ECode CComplex::Asin()
{
    if (mIm == 0 && Elastos::Core::Math::Abs(mRe) <= 1) {
        return Set(Elastos::Core::Math::Asin(mRe), 0);
    }
    Double saveA = mRe;
    Double saveB = mIm;
    Sqrt1z();
    Set(mRe - saveB, mIm + saveA);
    Log();
    Set(mIm, -mRe);
    return NOERROR;
}

ECode CComplex::Acos()
{
    if (mIm == 0 && Elastos::Core::Math::Abs(mRe) <= 1) {
        return Set(Elastos::Core::Math::Acos(mRe), 0);
    }
    Double saveA = mRe;
    Double saveB = mIm;
    Sqrt1z();
    Set(saveA - mIm, saveB + mRe);
    Log();
    Set(mIm, -mRe);
    return NOERROR;
}

ECode CComplex::Atan()
{
    if (mIm == 0) {
        return Set(Elastos::Core::Math::Atan(mRe), 0);
    }
    Double a2 = mRe * mRe;
    Double b2 = mIm * mIm;
    Double down = a2 + b2 - mIm - mIm + 1;
    Set(-(a2 + b2 - 1)/down, -(mRe + mRe)/down);
    Log();
    Set(-mIm/2, mRe/2);
    return NOERROR;
}

ECode CComplex::Asinh()
{
    if (mIm == 0) {
        return Set(MoreMath::Asinh(mRe), 0);
    }
    Double a = mRe;
    Double b = mIm;
    Set(mRe*mRe - mIm*mIm + 1, 2*mRe*mIm);
    Sqrt();
    Set(mRe + a, mIm + b);
    Log();
    return NOERROR;
}

ECode CComplex::Acosh()
{
    if (mIm == 0 && mRe >= 1) {
        return Set(MoreMath::Acosh(mRe), 0);
    }
    Double a = mRe;
    Double b = mIm;
    Set(mRe*mRe - mIm*mIm - 1, 2*mRe*mIm);
    Sqrt();
    Set(mRe + a, mIm + b);
    Log();
    return NOERROR;
}

ECode CComplex::Atanh()
{
    if (mIm == 0) {
        return Set(MoreMath::Atanh(mRe), 0);
    }
    Double a2 = mRe * mRe;
    Double b2 = mIm * mIm;
    Double down = a2 + 1 - mRe - mRe;
    Set((1 - a2 - b2)/down, (mIm + mIm)/down);
    Log();
    Set(mRe/2, mIm/2);
    return NOERROR;
}

ECode CComplex::Combinations(
    /* [in] */ IComplex* _o)
{
    AutoPtr<CComplex> o = (CComplex*)_o;
    if (mIm == 0 && o->mIm == 0) {
        return Set(MoreMath::Combinations(mRe, o->mRe), 0);
    }

    Double a = mRe;
    Double b = mIm;

    Lgamma();
    Double c = mRe;
    Double d = mIm;

    Set(_o);
    Lgamma();
    Double e = mRe;
    Double f = mIm;

    Set(a - o->mRe, b - o->mIm);
    Lgamma();
    Set(c - e - mRe, d - f - mIm);
    Exp();
    return NOERROR;
}

ECode CComplex::Permutations(
    /* [in] */ IComplex* _o)
{
    AutoPtr<CComplex> o = (CComplex*)_o;
    if (mIm == 0 && o->mIm == 0) {
        return Set(MoreMath::Permutations(mRe, o->mRe), 0);
    }

    Double a = mRe;
    Double b = mIm;

    Lgamma();
    Double c = mRe;
    Double d = mIm;

    Set(a - o->mRe, b - o->mIm);
    Lgamma();
    Set(c - mRe, d - mIm);
    Exp();
    return NOERROR;
}

void CComplex::Swap()
{
    Set(mIm, mRe);
}

void CComplex::NormalizeInfinity()
{
    // assumes this.isInfinite()
    if (!Elastos::Core::Math::IsInfinite(mIm)) {
        mIm = 0;
    }
    else if (!Elastos::Core::Math::IsInfinite(mRe)) {
        mRe = 0;
    }
}

void CComplex::Sqrt1z()
{
    Set(1 - mRe*mRe + mIm*mIm, -2*mRe*mIm);
    Sqrt();
}

} // namespace Arity
} // namespace Javia
} // namespace Org