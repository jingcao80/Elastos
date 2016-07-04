
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CRect.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/regex/Pattern.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Regex::Pattern;
using Elastos::Utility::Regex::IMatchResult;

namespace Elastos {
namespace Droid {
namespace Graphics {

AutoPtr<IPattern> CRect::UnflattenHelper::FLATTENED_PATTERN = CRect::UnflattenHelper::InitStatic();
AutoPtr<IMatcher> CRect::UnflattenHelper::GetMatcher(
    /* [in] */ const String& str)
{
    AutoPtr<IMatcher> matcher;
    FLATTENED_PATTERN->Matcher(str, (IMatcher**)&matcher);
    return matcher;
}

AutoPtr<IPattern> CRect::UnflattenHelper::InitStatic()
{
    AutoPtr<IPattern> pattern;
    Pattern::Compile(
        String("(-?\\d+) (-?\\d+) (-?\\d+) (-?\\d+)"), (IPattern**)&pattern);
    return pattern;
}

CAR_OBJECT_IMPL(CRect)

CAR_INTERFACE_IMPL_2(CRect, Object, IRect, IParcelable)

ECode CRect::constructor()
{
    mLeft = mRight = mTop = mBottom = 0;

    return NOERROR;
}

ECode CRect::constructor(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    mLeft = left;
    mTop = top;
    mRight = right;
    mBottom = bottom;

    return NOERROR;
}

ECode CRect::constructor(
    /* [in] */ IRect* r)
{
    if (r == NULL) {
        mLeft = 0;
        mTop = 0;
        mRight = 0;
        mBottom = 0;
    }
    else {
        CRect* temp = (CRect*)r;
        mLeft = temp->mLeft;
        mTop = temp->mTop;
        mRight = temp->mRight;
        mBottom = temp->mBottom;
    }
    return NOERROR;
}

ECode CRect::Equals(
    /* [in] */ IRect* r,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (r != NULL) {
        Int32 l, t, rt, b;
        r->Get(&l, &t, &rt, &b);
        *result = mLeft == l && mTop == t && mRight == rt && mBottom == b;
    }
    return NOERROR;
}

ECode CRect::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (TO_IINTERFACE(this) == IInterface::Probe(o)) {
        *result = TRUE;
        return NOERROR;
    }

    ClassID id1, id2;
    GetClassID(&id1);
    if (o == NULL || id1 != (IObject::Probe(o)->GetClassID(&id2), id2)) return NOERROR;

    CRect* r = (CRect*)IRect::Probe(o);
    *result = mLeft == r->mLeft && mTop == r->mTop && mRight == r->mRight && mBottom == r->mBottom;
    return NOERROR;
}

ECode CRect::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);
    Int32 result = mLeft;
    result = 31 * result + mTop;
    result = 31 * result + mRight;
    result = 31 * result + mBottom;
    *hash = result;
    return NOERROR;
}

ECode CRect::ToString(
    /* [out] */ String* str)
{
    StringBuilder sb(32);
    sb += "Rect(";
    sb += mLeft; sb += ", ";
    sb += mTop; sb += ", ";
    sb += mRight; sb += ", ";
    sb += mBottom; sb += ")";
    sb.ToString(str);
    return NOERROR;
}

ECode CRect::ToShortString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb(32);
    sb.Append(String("Rect(")); sb.Append(mLeft); sb.Append(String(", "));
    sb.Append(mTop); sb.Append(String(" - ")); sb.Append(mRight);
    sb.Append(String(", ")); sb.Append(mBottom); sb.Append(String(")"));
    return sb.ToString(str);
}

ECode CRect::FlattenToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb(32);
    // WARNING: Do not change the format of this string, it must be
    // preserved because Rects are saved in this flattened format.
    sb += mLeft;
    sb += " ";
    sb += mTop;
    sb += " ";
    sb += mRight;
    sb += " ";
    sb += mBottom;
    return sb.ToString(str);
}

AutoPtr<IRect> CRect::UnflattenFromString(
    /* [in] */ const String& str)
{
    AutoPtr<IMatcher> matcher = UnflattenHelper::GetMatcher(str);
    Boolean matched = FALSE;
    if (matcher->Matches(&matched), !matched) {
        return NULL;
    }
    IMatchResult* matchResult = IMatchResult::Probe(matcher);
    String g1, g2, g3, g4;
    matchResult->Group(1, &g1);
    matchResult->Group(2, &g2);
    matchResult->Group(3, &g3);
    matchResult->Group(4, &g4);
    AutoPtr<IRect> rect;
    CRect::New(StringUtils::ParseInt32(g1),
            StringUtils::ParseInt32(g2),
            StringUtils::ParseInt32(g3),
            StringUtils::ParseInt32(g4), (IRect**)&rect);
    return rect;
}

ECode CRect::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mLeft >= mRight || mTop >= mBottom;
    return NOERROR;
}

ECode CRect::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)

    *width = mRight - mLeft;
    return NOERROR;
}

ECode CRect::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)

    *height = mBottom - mTop;
    return NOERROR;
}

ECode CRect::GetCenterX(
    /* [out] */ Int32* x)
{
    VALIDATE_NOT_NULL(x);

    *x = (mLeft + mRight) >> 1;
    return NOERROR;
}

ECode CRect::GetCenterY(
    /* [out] */ Int32* y)
{
    VALIDATE_NOT_NULL(y);

    *y = (mTop + mBottom) >> 1;
    return NOERROR;
}

ECode CRect::GetExactCenterX(
    /* [out] */ Float* x)
{
    VALIDATE_NOT_NULL(x);

    *x = (mLeft + mRight) * 0.5f;
    return NOERROR;
}

ECode CRect::GetExactCenterY(
    /* [out] */ Float* y)
{
    VALIDATE_NOT_NULL(y);

    *y = (mTop + mBottom) * 0.5f;
    return NOERROR;
}

ECode CRect::SetEmpty()
{
    mLeft = mRight = mTop = mBottom = 0;
    return NOERROR;
}

ECode CRect::Set(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    mLeft = left;
    mTop = top;
    mRight = right;
    mBottom = bottom;
    return NOERROR;
}

ECode CRect::Get(
    /* [out] */ Int32* left,
    /* [out] */ Int32* top,
    /* [out] */ Int32* right,
    /* [out] */ Int32* bottom)
{
    if (left) *left = mLeft;
    if (top) *top = mTop;
    if (right) *right = mRight;
    if (bottom) *bottom = mBottom;
    return NOERROR;
}

ECode CRect::Set(
    /* [in] */ IRect* src)
{
    assert(src != NULL);
    VALIDATE_NOT_NULL(src);

    src->GetLeft(&mLeft);
    src->GetTop(&mTop);
    src->GetRight(&mRight);
    src->GetBottom(&mBottom);

    return NOERROR;
}

ECode CRect::Offset(
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy)
{
    mLeft += dx;
    mTop += dy;
    mRight += dx;
    mBottom += dy;
    return NOERROR;
}

ECode CRect::OffsetTo(
    /* [in] */ Int32 newLeft,
    /* [in] */ Int32 newTop)
{
    mRight += newLeft - mLeft;
    mBottom += newTop - mTop;
    mLeft = newLeft;
    mTop = newTop;
    return NOERROR;
}

ECode CRect::Inset(
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy)
{
    mLeft += dx;
    mTop += dy;
    mRight -= dx;
    mBottom -= dy;
    return NOERROR;
}

ECode CRect::Contains(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mLeft < mRight && mTop < mBottom  // check for empty first
           && x >= mLeft && x < mRight && y >= mTop && y < mBottom;
    return NOERROR;
}

ECode CRect::Contains(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // check for empty first
    *result = mLeft < mRight && mTop < mBottom
           // now check for containment
           && mLeft <= left && mTop <= top
           && mRight >= right && mBottom >= bottom;
    return NOERROR;
}

ECode CRect::Contains(
    /* [in] */ IRect* r,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    CRect* temp = (CRect*)r;
    // check for empty first
    *result = mLeft < mRight && mTop < mBottom
           // now check for containment
           && mLeft <= temp->mLeft && mTop <= temp->mTop
           && mRight >= temp->mRight && mBottom >= temp->mBottom;
    return NOERROR;
}

ECode CRect::Intersect(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
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
    }
    else {
        *result = FALSE;
    }
    return NOERROR;
}

ECode CRect::Intersect(
    /* [in] */ IRect* r,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    CRect* temp = (CRect*)r;
    return Intersect(temp->mLeft, temp->mTop, temp->mRight, temp->mBottom, result);
}

ECode CRect::Intersects(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mLeft < right && left < mRight
        && mTop < bottom && top < mBottom;
    return NOERROR;
}

Boolean CRect::Intersects(
    /* [in] */ IRect* a,
    /* [in] */ IRect* b)
{
    CRect* tempA = (CRect*)a;
    CRect* tempB = (CRect*)b;
    return tempA->mLeft < tempB->mRight && tempB->mLeft < tempA->mRight
        && tempA->mTop < tempB->mBottom && tempB->mTop < tempA->mBottom;
}

ECode CRect::SetIntersect(
    /* [in] */ IRect* a,
    /* [in] */ IRect* b,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    CRect* tempA = (CRect*)a;
    CRect* tempB = (CRect*)b;
    if (tempA->mLeft < tempB->mRight
            && tempB->mLeft < tempA->mRight
            && tempA->mTop < tempB->mBottom && tempB->mTop < tempA->mBottom) {
        mLeft = Elastos::Core::Math::Max(tempA->mLeft, tempB->mLeft);
        mTop = Elastos::Core::Math::Max(tempA->mTop, tempB->mTop);
        mRight = Elastos::Core::Math::Min(tempA->mRight, tempB->mRight);
        mBottom = Elastos::Core::Math::Min(tempA->mBottom, tempB->mBottom);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CRect::Union(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
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

ECode CRect::Union(
    /* [in] */ IRect* r)
{
    CRect* temp = (CRect*)r;
    return Union(temp->mLeft, temp->mTop, temp->mRight, temp->mBottom);
}

ECode CRect::Union(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
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

ECode CRect::Sort()
{
    if (mLeft > mRight) {
        Int32 temp = mLeft;
        mLeft = mRight;
        mRight = temp;
    }
    if (mTop > mBottom) {
        Int32 temp = mTop;
        mTop = mBottom;
        mBottom = temp;
    }
    return NOERROR;
}

ECode CRect::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mLeft);
    dest->WriteInt32(mRight);
    dest->WriteInt32(mTop);
    dest->WriteInt32(mBottom);
    return NOERROR;
}

ECode CRect::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mLeft);
    source->ReadInt32(&mRight);
    source->ReadInt32(&mTop);
    source->ReadInt32(&mBottom);
    return NOERROR;
}

ECode CRect::Scale(
    /* [in] */ Float scale)
{
    if (scale != 1.0f) {
        mLeft = (Int32)(mLeft * scale + 0.5f);
        mTop = (Int32)(mTop * scale + 0.5f);
        mRight = (Int32)(mRight * scale + 0.5f);
        mBottom = (Int32)(mBottom * scale + 0.5f);
    }
    return NOERROR;
}

ECode CRect::GetTop(
    /* [out] */ Int32* top)
{
    VALIDATE_NOT_NULL(top);
    *top = mTop;
    return NOERROR;
}

ECode CRect::SetTop(
    /* [in] */ Int32 top)
{
    mTop = top;
    return NOERROR;
}

ECode CRect::GetBottom(
    /* [out] */ Int32* bottom)
{
    VALIDATE_NOT_NULL(bottom);
    *bottom = mBottom;
    return NOERROR;
}

ECode CRect::SetBottom(
    /* [in] */ Int32 bottom)
{
    mBottom = bottom;
    return NOERROR;
}

ECode CRect::GetLeft(
    /* [out] */ Int32* left)
{
    VALIDATE_NOT_NULL(left);
    *left = mLeft;
    return NOERROR;
}

ECode CRect::SetLeft(
    /* [in] */ Int32 left)
{
    mLeft = left;
    return NOERROR;
}

ECode CRect::GetRight(
    /* [out] */ Int32* right)
{
    VALIDATE_NOT_NULL(right);
    *right = mRight;
    return NOERROR;
}

ECode CRect::SetRight(
    /* [in] */ Int32 right)
{
    mRight = right;
    return NOERROR;
}

void CRect::ScaleRoundIn(
    /* [in] */ Float scale)
{
    if (scale != 1.0f) {
        mLeft = (Int32) Elastos::Core::Math::Ceil(mLeft * scale);
        mTop = (Int32) Elastos::Core::Math::Ceil(mTop * scale);
        mRight = (Int32) Elastos::Core::Math::Floor(mRight * scale);
        mBottom = (Int32) Elastos::Core::Math::Floor(mBottom * scale);
    }
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
