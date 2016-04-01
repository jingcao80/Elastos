
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Math.h>

using Elastos::Core::StringBuilder;

using namespace Elastos::Core;

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CRectF);
CAR_INTERFACE_IMPL_2(CRectF, Object, IRectF, IParcelable);
CRectF::CRectF()
    : mLeft(0.0f)
    , mTop(0.0f)
    , mRight(0.0f)
    , mBottom(0.0f)
{}

ECode CRectF::constructor()
{
    return NOERROR;
}

ECode CRectF::constructor(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom)
{
    mLeft = left;
    mTop = top;
    mRight = right;
    mBottom = bottom;
    return NOERROR;
}

ECode CRectF::constructor(
    /* [in] */ IRectF* rf)
{
    if (rf == NULL) {
        mLeft = 0.0f;
        mTop = 0.0f;
        mRight = 0.0f;
        mBottom = 0.0f;
    }
    else {
        CRectF* rfObj = (CRectF*)rf;
        mLeft = rfObj->mLeft;
        mTop = rfObj->mTop;
        mRight = rfObj->mRight;
        mBottom = rfObj->mBottom;
    }
    return NOERROR;
}

ECode CRectF::constructor(
    /* [in] */ IRect* r)
{
    if (r == NULL) {
        mLeft = 0.0f;
        mTop = 0.0f;
        mRight = 0.0f;
        mBottom = 0.0f;
    }
    else {
        CRect* rObj = (CRect*)r;
        mLeft = rObj->mLeft;
        mTop = rObj->mTop;
        mRight = rObj->mRight;
        mBottom = rObj->mBottom;
    }
    return NOERROR;
}

ECode CRectF::Equals(
    /* [in] */ IRectF* r,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (r != NULL) {
        Float l, t, rt, b;
        r->Get(&l, &t, &rt, &b);
        *result = mLeft == l && mTop == t && mRight == rt && mBottom == b;
    }
    return NOERROR;
}

ECode CRectF::Equals(
    /* [in] */ IInterface* r,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return Equals(IRectF::Probe(r), result);
}

ECode CRectF::GetHashCode(
    /* [out] */ Int32* hash)
{
    using Elastos::Core::Math;
    VALIDATE_NOT_NULL(hash);
    Int32 result = (mLeft != +0.0f ? Math::FloatToInt32Bits(mLeft) : 0);
    result = 31 * result + (mTop != +0.0f ? Math::FloatToInt32Bits(mTop) : 0);
    result = 31 * result + (mRight != +0.0f ? Math::FloatToInt32Bits(mRight) : 0);
    result = 31 * result + (mBottom != +0.0f ? Math::FloatToInt32Bits(mBottom) : 0);
    *hash = result;
    return NOERROR;
}

ECode CRectF::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb(32);
    sb += "RectF(";
    sb += mLeft; sb += ", ";
    sb += mTop; sb += ", ";
    sb += mRight; sb += ", ";
    sb += mBottom; sb += ", ";
    sb.ToString(str);
    return NOERROR;
}

ECode CRectF::ToShortString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb(32);
    sb += ('[');
    sb += (mLeft); sb += (',');
    sb += (mTop); sb += ("]["); sb += (mRight);
    sb += (','); sb += (mBottom);
    sb += (']');
    sb.ToString(str);
    return NOERROR;
}

ECode CRectF::IsEmpty(
    /* [out] */ Boolean* isEmpty)
{
    VALIDATE_NOT_NULL(isEmpty);
    *isEmpty = mLeft >= mRight || mTop >= mBottom;
    return NOERROR;
}

ECode CRectF::GetWidth(
    /* [out] */ Float* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mRight - mLeft;
    return NOERROR;
}

ECode CRectF::GetHeight(
    /* [out] */ Float* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mBottom - mTop;
    return NOERROR;
}

ECode CRectF::GetCenterX(
    /* [out] */ Float* x)
{
    VALIDATE_NOT_NULL(x);
    *x = (mLeft + mRight) * 0.5f;
    return NOERROR;
}

ECode CRectF::GetCenterY(
    /* [out] */ Float* y)
{
    VALIDATE_NOT_NULL(y);
    *y = (mTop + mBottom) * 0.5f;
    return NOERROR;
}

ECode CRectF::GetLeft(
    /* [out] */ Float* left)
{
    VALIDATE_NOT_NULL(left);
    *left = mLeft;
    return NOERROR;
}

ECode CRectF::SetLeft(
    /* [in] */ Float left)
{
    mLeft = left;
    return NOERROR;
}

ECode CRectF::GetTop(
    /* [out] */ Float* top)
{
    VALIDATE_NOT_NULL(top);
    *top = mTop;
    return NOERROR;
}

ECode CRectF::SetTop(
    /* [in] */ Float top)
{
    mTop = top;
    return NOERROR;
}

ECode CRectF::GetRight(
    /* [out] */ Float* right)
{
    VALIDATE_NOT_NULL(right);
    *right = mRight;
    return NOERROR;
}

ECode CRectF::SetRight(
    /* [in] */ Float right)
{
    mRight = right;
    return NOERROR;
}

ECode CRectF::GetBottom(
    /* [out] */ Float* bottom)
{
    VALIDATE_NOT_NULL(bottom);
    *bottom = mBottom;
    return NOERROR;
}

ECode CRectF::SetBottom(
    /* [in] */ Float bottom)
{
    mBottom = bottom;
    return NOERROR;
}

ECode CRectF::SetEmpty()
{
    mLeft = mRight = mTop = mBottom = 0;
    return NOERROR;
}

ECode CRectF::Set(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom)
{
    mLeft = left;
    mTop = top;
    mRight = right;
    mBottom = bottom;
    return NOERROR;
}

ECode CRectF::Get(
    /* [out] */ Float* left,
    /* [out] */ Float* top,
    /* [out] */ Float* right,
    /* [out] */ Float* bottom)
{
    if (left)
        *left = mLeft;
    if (top)
        *top = mTop;
    if (right)
        *right = mRight;
    if (bottom)
        *bottom = mBottom;
    return NOERROR;
}

ECode CRectF::Set(
    /* [in] */ IRectF* src)
{
    CRectF* srcObj = (CRectF*)src;
    mLeft = srcObj->mLeft;
    mTop = srcObj->mTop;
    mRight = srcObj->mRight;
    mBottom = srcObj->mBottom;
    return NOERROR;
}

ECode CRectF::Set(
    /* [in] */ IRect* src)
{
    CRect* srcObj = (CRect*)src;
    mLeft = srcObj->mLeft;
    mTop = srcObj->mTop;
    mRight = srcObj->mRight;
    mBottom = srcObj->mBottom;
    return NOERROR;
}

ECode CRectF::Offset(
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    mLeft    += dx;
    mTop     += dy;
    mRight   += dx;
    mBottom  += dy;
    return NOERROR;
}

ECode CRectF::OffsetTo(
    /* [in] */ Float newLeft,
    /* [in] */ Float newTop)
{
    mRight += newLeft - mLeft;
    mBottom += newTop - mTop;
    mLeft = newLeft;
    mTop = newTop;
    return NOERROR;
}

ECode CRectF::Inset(
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    mLeft    += dx;
    mTop     += dy;
    mRight   -= dx;
    mBottom  -= dy;
    return NOERROR;
}

ECode CRectF::Contains(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [out] */ Boolean* isContained)
{
    VALIDATE_NOT_NULL(isContained);
    *isContained =  mLeft < mRight && mTop < mBottom  // check for empty first
        && x >= mLeft && x < mRight && y >= mTop && y < mBottom;
    return NOERROR;
}

ECode CRectF::Contains(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [out] */ Boolean* isContained)
{
    VALIDATE_NOT_NULL(isContained);
    // check for empty first
    *isContained = mLeft < mRight && mTop < mBottom
            // now check for containment
            && mLeft <= left && mTop <= top
            && mRight >= right && mBottom >= bottom;
    return NOERROR;
}

ECode CRectF::Contains(
    /* [in] */ IRectF* r,
    /* [out] */ Boolean* isContained)
{
    VALIDATE_NOT_NULL(isContained);

    CRectF* cr = (CRectF*)r;
            // check for empty first
    *isContained = mLeft < mRight && mTop < mBottom
            // now check for containment
            && mLeft <= mLeft && this->mTop <= cr->mTop
            && this->mRight >= cr->mRight && this->mBottom >= cr->mBottom;
    return NOERROR;
}

ECode CRectF::Intersect(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mLeft < right && left < mRight
            && mTop < bottom && top < mBottom) {
        if (mLeft < left) {
            mLeft = left;
        }
        if (mTop < top) {
            mTop = top;
        }
        if (mRight > right) {
            mRight = right;
        }
        if (mBottom > bottom) {
            mBottom = bottom;
        }
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CRectF::Intersect(
    /* [in] */ IRectF* r,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    CRectF* cr = (CRectF*)r;
    return Intersect(cr->mLeft, cr->mTop, cr->mRight, cr->mBottom, result);
}

ECode CRectF::SetIntersect(
    /* [in] */ IRectF* a,
    /* [in] */ IRectF* b,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    CRectF* ca = (CRectF*)a;
    CRectF* cb = (CRectF*)b;
    if (ca->mLeft < cb->mRight && cb->mLeft < ca->mRight
            && ca->mTop < cb->mBottom && cb->mTop < ca->mBottom) {
        mLeft = Elastos::Core::Math::Max(ca->mLeft, cb->mLeft);
        mTop = Elastos::Core::Math::Max(ca->mTop, cb->mTop);
        mRight = Elastos::Core::Math::Min(ca->mRight, cb->mRight);
        mBottom = Elastos::Core::Math::Min(ca->mBottom, cb->mBottom);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CRectF::Intersects(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mLeft < right && left < mRight
        && mTop < bottom && top < mBottom;
    return NOERROR;
}

ECode CRectF::Round(
    /* [in] */ IRect* dst)
{
    dst->Set(Elastos::Core::Math::Round(mLeft), Elastos::Core::Math::Round(mTop),
             Elastos::Core::Math::Round(mRight), Elastos::Core::Math::Round(mBottom));
    return NOERROR;
}

ECode CRectF::RoundOut(
    /* [in] */ IRect* dst)
{
    dst->Set((Int32) Elastos::Core::Math::Floor(mLeft), (Int32) Elastos::Core::Math::Floor(mTop),
             (Int32) Elastos::Core::Math::Ceil(mRight), (Int32) Elastos::Core::Math::Ceil(mBottom));
    return NOERROR;
}

ECode CRectF::Union(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom)
{
    if ((left < right) && (top < bottom)) {
        if ((mLeft < mRight) && (mTop < mBottom)) {
            if (mLeft > left) mLeft = left;
            if (mTop > top) mTop = top;
            if (mRight < right) mRight = right;
            if (mBottom < bottom) mBottom = bottom;
        }
        else {
            mLeft = left;
            mTop = top;
            mRight = right;
            mBottom = bottom;
        }
    }
    return NOERROR;
}

ECode CRectF::Union(
    /* [in] */ IRectF* r)
{
    CRectF* cr = (CRectF*)r;
    return Union(cr->mLeft, cr->mTop, cr->mRight, cr->mBottom);
}

ECode CRectF::Union(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    if (x < mLeft) {
        mLeft = x;
    }
    else if (x > mRight) {
        mRight = x;
    }
    if (y < mTop) {
        mTop = y;
    }
    else if (y > mBottom) {
        mBottom = y;
    }
    return NOERROR;
}

ECode CRectF::Sort()
{
    if (mLeft > mRight) {
        Float temp = mLeft;
        mLeft = mRight;
        mRight = temp;
    }
    if (mTop > mBottom) {
        Float temp = mTop;
        mTop = mBottom;
        mBottom = temp;
    }
    return NOERROR;
}

ECode CRectF::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadFloat(&mLeft);
    source->ReadFloat(&mTop);
    source->ReadFloat(&mRight);
    source->ReadFloat(&mBottom);
    return NOERROR;
}

ECode CRectF::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteFloat(mLeft);
    dest->WriteFloat(mTop);
    dest->WriteFloat(mRight);
    dest->WriteFloat(mBottom);
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
