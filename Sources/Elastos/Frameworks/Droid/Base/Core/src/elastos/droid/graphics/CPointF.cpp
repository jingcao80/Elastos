
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CPointF.h"
#include "elastos/droid/graphics/CPoint.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>

using namespace Elastos::Core;

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CPointF);
CAR_INTERFACE_IMPL_2(CPointF, Object, IPointF, IParcelable);
ECode CPointF::constructor()
{
    mX = 0;
    mY = 0;
    return NOERROR;
}

ECode CPointF::constructor(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    mX = x;
    mY = y;
    return NOERROR;
}

ECode CPointF::constructor(
    /* [in] */ IPoint* src)
{
    mX = ((CPoint*)src)->mX;
    mY = ((CPoint*)src)->mY;
    return NOERROR;
}

ECode CPointF::Set(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    mX = x;
    mY = y;
    return NOERROR;
}

ECode CPointF::Get(
    /* [out] */ Float* x,
    /* [out] */ Float* y)
{
    if (x != NULL)
        *x = mX;

    if (y != NULL)
        *y = mY;
    return NOERROR;
}

ECode CPointF::Set(
    /* [in] */ IPointF* p)
{
    mX = ((CPointF*)p)->mX;
    mY = ((CPointF*)p)->mY;
    return NOERROR;
}

ECode CPointF::GetX(
    /* [out] */ Float* x)
{
    VALIDATE_NOT_NULL(x);
    *x = mX;
    return NOERROR;
}

ECode CPointF::GetY(
    /* [out] */ Float* y)
{
    VALIDATE_NOT_NULL(y);
    *y = mY;
    return NOERROR;
}

ECode CPointF::SetX(
    /* [in] */ Float x)
{
    mX = x;
    return NOERROR;
}

ECode CPointF::SetY(
    /* [in] */ Float y)
{
    mY = y;
    return NOERROR;
}

ECode CPointF::Negate()
{
    mX = -mX;
    mY = -mY;
    return NOERROR;
}

ECode CPointF::Offset(
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    mX += dx;
    mY += dy;
    return NOERROR;
}

ECode CPointF::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadFloat(&mX);
    source->ReadFloat(&mY);
    return NOERROR;
}

ECode CPointF::WriteToParcel(
    /* [out] */ IParcel* dest)
{
    dest->WriteFloat(mX);
    dest->WriteFloat(mY);
    return NOERROR;
}

ECode CPointF::Length(
    /* [out] */ Float* length)
{
    VALIDATE_NOT_NULL(length);

    *length = Length(mX, mY);
    return NOERROR;
}

ECode CPointF::Equals(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (Elastos::Core::Math::Compare(x, mX) != 0) return NOERROR;
    if (Elastos::Core::Math::Compare(y, mY) != 0) return NOERROR;
    *result = TRUE;
    return NOERROR;
}

ECode CPointF::Equals(
    /* [in] */ IPointF* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(o);

    Float x, y;
    o->Get(&x, &y);
    return Equals(x, y, result);
}

ECode CPointF::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return Equals(IPointF::Probe(o), result);
}

ECode CPointF::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    Int32 result = (mX != +0.0f ? Elastos::Core::Math::FloatToInt32Bits(mX) : 0);
    result = 31 * result + (mY != +0.0f ? Elastos::Core::Math::FloatToInt32Bits(mY) : 0);
    *hash = result;
    return NOERROR;
}

ECode CPointF::ToString(
    /* [out] */ String* str)
{
    StringBuilder sb("PointF(");
    sb += mX;
    sb += ", ";
    sb += mY;
    sb += ")";
    sb.ToString(str);
    return NOERROR;
}

Float CPointF::Length(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    return Elastos::Core::Math::Sqrt(x * x + y * y);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
