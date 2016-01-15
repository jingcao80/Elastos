
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/COutline.h"
#include "elastos/droid/graphics/CPath.h"
#include "elastos/droid/graphics/CRect.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(COutline);
CAR_INTERFACE_IMPL(COutline, Object, IOutline);
COutline::COutline()
    : mRadius(0)
    , mAlpha(0)
{
}

ECode COutline::constructor()
{
    return NOERROR;
}

ECode COutline::constructor(
    /* [in] */ /*@NonNull*/ IOutline* src)
{
    return Set(src);
}

ECode COutline::SetEmpty()
{
    mPath = NULL;
    mRect = NULL;
    mRadius = 0;
    return NOERROR;
}

ECode COutline::IsEmpty(
    /* [out] */ Boolean * empty)
{
    VALIDATE_NOT_NULL(empty);
    *empty = mRect == NULL && mPath == NULL;
    return NOERROR;
}

ECode COutline::CanClip(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    *can = !(IsEmpty(can), *can) && mRect != NULL;
    return NOERROR;
}

ECode COutline::SetAlpha(
    /* [in] */ Float alpha)
{
    mAlpha = alpha;
    return NOERROR;
}

ECode COutline::GetAlpha(
    /* [out] */ Float* alpha)
{
    VALIDATE_NOT_NULL(alpha);
    *alpha = mAlpha;
    return NOERROR;
}

ECode COutline::Set(
    /* [in] */ /*@NonNull*/ IOutline* _src)
{
    COutline* src = (COutline*)_src;
    if (src->mPath != NULL) {
        if (mPath == NULL) {
            CPath::New((IPath**)&mPath);
        }
        mPath->Set(src->mPath);
        mRect = NULL;
    }
    if (src->mRect != NULL) {
        if (mRect == NULL) {
            CRect::New((IRect**)&mRect);
        }
        mRect->Set(src->mRect);
    }
    mRadius = src->mRadius;
    mAlpha = src->mAlpha;
    return NOERROR;
}

ECode COutline::SetRect(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    return SetRoundRect(left, top, right, bottom, 0.0f);
}

ECode COutline::SetRect(
    /* [in] */ /*@NonNull*/ IRect* rect)
{
    Int32 l = 0, t = 0, r = 0, b = 0;
    rect->Get(&l, &t, &r, &b);
    return SetRect(l, t, r, b);
}

ECode COutline::SetRoundRect(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Float radius)
{
    if (left >= right || top >= bottom) {
        SetEmpty();
        return NOERROR;
    }

    if (mRect == NULL) {
        CRect::New((IRect**)&mRect);
    }

    mRect->Set(left, top, right, bottom);
    mRadius = radius;
    mPath = NULL;
    return NOERROR;
}

ECode COutline::SetRoundRect(
    /* [in] */ /*@NonNull*/ IRect* rect,
    /* [in] */ Float radius)
{
    Int32 l = 0, t = 0, r = 0, b = 0;
    rect->Get(&l, &t, &r, &b);
    return SetRoundRect(l, t, r, b, radius);
}

ECode COutline::SetOval(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    if (left >= right || top >= bottom) {
        SetEmpty();
        return NOERROR;
    }

    if ((bottom - top) == (right - left)) {
        // represent circle as round rect, for efficiency, and to enable clipping
        return SetRoundRect(left, top, right, bottom, (bottom - top) / 2.0f);
    }

    if (mPath == NULL) {
        CPath::New((IPath**)&mPath);
    }
    mPath->Reset();
    mPath->AddOval(left, top, right, bottom, PathDirection_CW);
    mRect = NULL;
    return NOERROR;
}

ECode COutline::SetOval(
    /* [in] */ /*@NonNull*/ IRect* rect)
{
    Int32 l = 0, t = 0, r = 0, b = 0;
    rect->Get(&l, &t, &r, &b);
    return SetOval(l, t, r, b);
}

ECode COutline::SetConvexPath(
    /* [in] */ /*@NonNull*/ IPath* convexPath)
{
    Boolean value = FALSE;
    if (convexPath->IsEmpty(&value), value) {
        SetEmpty();
        return NOERROR;
    }

    if (!(convexPath->IsConvex(&value), value)) {
        // throw new IllegalArgumentException("path must be convex");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mPath == NULL) {
        CPath::New((IPath**)&mPath);
    }

    mPath->Set(convexPath);
    mRect = NULL;
    mRadius = -1.0f;
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
