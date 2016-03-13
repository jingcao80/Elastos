
#include "elastos/droid/internal/widget/multiwaveview/TargetDrawable.h"
#include "elastos/droid/internal/widget/multiwaveview/CTargetDrawable.h"
#include "elastos/droid/R.h"

#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::R;
using Elastos::Droid::Graphics::Drawable::IStateListDrawable;
using Elastos::Droid::Internal::Widget::Multiwaveview::CTargetDrawable;

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {
namespace Multiwaveview {

const String TargetDrawable::TAG(String("TargetDrawable"));
const Boolean TargetDrawable::DEBUG = FALSE;

AutoPtr<ArrayOf<Int32> > TargetDrawable::STATE_ACTIVE = ArrayOf<Int32>::Alloc(2);

AutoPtr<ArrayOf<Int32> > TargetDrawable::STATE_INACTIVE = ArrayOf<Int32>::Alloc(2);

AutoPtr<ArrayOf<Int32> > TargetDrawable::STATE_FOCUSED = ArrayOf<Int32>::Alloc(3);

//=====================================================================
//               TargetDrawable::
//=====================================================================
CAR_INTERFACE_IMPL(TargetDrawable, Object, ITargetDrawable)

TargetDrawable::TargetDrawable()
    : mTranslationX(0.0f)
    , mTranslationY(0.0f)
    , mPositionX(0.0f)
    , mPositionY(0.0f)
    , mScaleX(1.0f)
    , mScaleY(1.0f)
    , mAlpha(1.0f)
    , mEnabled(TRUE)
    , mResourceId(0)
{
    (*STATE_ACTIVE)[0] = R::attr::state_enabled;
    (*STATE_ACTIVE)[1] = R::attr::state_active;

    (*STATE_INACTIVE)[0] = R::attr::state_enabled;
    (*STATE_INACTIVE)[1] = -R::attr::state_active;

    (*STATE_FOCUSED)[0] = R::attr::state_enabled;
    (*STATE_FOCUSED)[1] = -R::attr::state_active;
    (*STATE_FOCUSED)[2] = R::attr::state_focused;
}

ECode TargetDrawable::constructor(
    /* [in] */ IResources* res,
    /* [in] */ Int32 resId)
{
    mResourceId = resId;
    SetDrawable(res, resId);
    return NOERROR;
}

ECode TargetDrawable::SetDrawable(
    /* [in] */ IResources* res,
    /* [in] */ Int32 resId)
{
    // Note we explicitly don't set mResourceId to resId since we allow the drawable to be
    // swapped at runtime and want to re-use the existing resource id for identification.
    AutoPtr<IDrawable> drawable;
    if (resId == 0){
        drawable = NULL;
    }
    else {
        res->GetDrawable(resId, (IDrawable**)&drawable);
    }
    // Mutate the drawable so we can animate shared drawable properties.
    if (drawable != NULL) {
        drawable->Mutate();
        mDrawable = drawable;
    }
    else {
        mDrawable = NULL;
    }
    ResizeDrawables();
    SetState(STATE_INACTIVE);
    return NOERROR;
}

TargetDrawable::TargetDrawable(
    /* [in] */ ITargetDrawable* other)
{
    other->GetResourceId(&mResourceId);
    // Mutate the drawable so we can animate shared drawable properties.
    AutoPtr<TargetDrawable> cother = (TargetDrawable*)other;
    if (cother->mDrawable != NULL) {
        cother->mDrawable->Mutate();
        mDrawable = cother->mDrawable;
    }
    else {
        mDrawable = NULL;
    }
    ResizeDrawables();
    SetState(STATE_INACTIVE);
}

ECode TargetDrawable::SetState(
    /* [in] */ ArrayOf<Int32>* state)
{
    if (IStateListDrawable::Probe(mDrawable) != NULL) {
        AutoPtr<IStateListDrawable> d = IStateListDrawable::Probe(mDrawable);
        Boolean res = FALSE;
        IDrawable::Probe(d)->SetState(state, &res);
    }
    return NOERROR;
}

ECode TargetDrawable::HasState(
    /* [in] */ ArrayOf<Int32>* state,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (IStateListDrawable::Probe(mDrawable) != NULL) {
        AutoPtr<IStateListDrawable> d = IStateListDrawable::Probe(mDrawable);
        // TODO: this doesn't seem to work
        Int32 index = 0;
        d->GetStateDrawableIndex(state, &index);
        *result = index != -1;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode TargetDrawable::IsActive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (IStateListDrawable::Probe(mDrawable) != NULL) {
        AutoPtr<IStateListDrawable> d = IStateListDrawable::Probe(mDrawable);
        AutoPtr<ArrayOf<Int32> > states;
        IDrawable::Probe(d)->GetState((ArrayOf<Int32>**)&states);
        for (Int32 i = 0; i < states->GetLength(); i++) {
            if ((*states)[i] == R::attr::state_focused) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode TargetDrawable::IsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mDrawable != NULL && mEnabled;
    return NOERROR;
}

void TargetDrawable::ResizeDrawables()
{
    if (IStateListDrawable::Probe(mDrawable) != NULL) {
        AutoPtr<IStateListDrawable> d = IStateListDrawable::Probe(mDrawable);
        Int32 maxWidth = 0;
        Int32 maxHeight = 0;
        Int32 count = 0;
        d->GetStateCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IDrawable> childDrawable;
            d->GetStateDrawable(i, (IDrawable**)&childDrawable);
            Int32 w = 0, h = 0;
            childDrawable->GetIntrinsicWidth(&w);
            maxWidth = Elastos::Core::Math::Max(maxWidth, w);
            childDrawable->GetIntrinsicHeight(&h);
            maxHeight = Elastos::Core::Math::Max(maxHeight, h);
        }
        if (DEBUG) {
            Logger::V(TAG, "union of childDrawable rects %p to: %dx%d", d.Get(), maxWidth, maxHeight);
        }
        IDrawable::Probe(d)->SetBounds(0, 0, maxWidth, maxHeight);
        d->GetStateCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IDrawable> childDrawable;
            d->GetStateDrawable(i, (IDrawable**)&childDrawable);
            if (DEBUG) {
                Logger::V(TAG, "sizing drawable %p to: %dx%d", childDrawable.Get(), maxWidth, maxHeight);
            }
            childDrawable->SetBounds(0, 0, maxWidth, maxHeight);
        }
    }
    else if (mDrawable != NULL) {
        Int32 w = 0, h = 0;
        mDrawable->GetIntrinsicWidth(&w);
        mDrawable->GetIntrinsicHeight(&h);
        mDrawable->SetBounds(0, 0, w, h);
    }
}

ECode TargetDrawable::SetX(
    /* [in] */ Float x)
{
    mTranslationX = x;
    return NOERROR;
}

ECode TargetDrawable::SetY(
    /* [in] */ Float y)
{
    mTranslationY = y;
    return NOERROR;
}

ECode TargetDrawable::SetScaleX(
    /* [in] */ Float x)
{
    mScaleX = x;
    return NOERROR;
}

ECode TargetDrawable::SetScaleY(
    /* [in] */ Float y)
{
    mScaleY = y;
    return NOERROR;
}

ECode TargetDrawable::SetAlpha(
    /* [in] */ Float alpha)
{
    mAlpha = alpha;
    return NOERROR;
}

ECode TargetDrawable::GetX(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mTranslationX;
    return NOERROR;
}

ECode TargetDrawable::GetY(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mTranslationY;
    return NOERROR;
}

ECode TargetDrawable::GetScaleX(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mScaleX;
    return NOERROR;
}

ECode TargetDrawable::GetScaleY(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mScaleY;
    return NOERROR;
}

ECode TargetDrawable::GetAlpha(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mAlpha;
    return NOERROR;
}

ECode TargetDrawable::SetPositionX(
    /* [in] */ Float x)
{
    mPositionX = x;
    return NOERROR;
}

ECode TargetDrawable::SetPositionY(
    /* [in] */ Float y)
{
    mPositionY = y;
    return NOERROR;
}

ECode TargetDrawable::GetPositionX(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mPositionX;
    return NOERROR;
}

ECode TargetDrawable::GetPositionY(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mPositionY;
    return NOERROR;
}

ECode TargetDrawable::GetWidth(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 w = 0;
    *result = mDrawable != NULL ? (mDrawable->GetIntrinsicWidth(&w), w) : 0;
    return NOERROR;
}

ECode TargetDrawable::GetHeight(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 h = 0;
    *result = mDrawable != NULL ? (mDrawable->GetIntrinsicHeight(&h), h) : 0;
    return NOERROR;
}

ECode TargetDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    if (mDrawable == NULL || !mEnabled) {
        return NOERROR;
    }
    Int32 res = 0;
    canvas->Save(ICanvas::MATRIX_SAVE_FLAG, &res);
    canvas->Scale(mScaleX, mScaleY, mPositionX, mPositionY);
    canvas->Translate(mTranslationX + mPositionX, mTranslationY + mPositionY);
    Int32 w = 0, h = 0;
    GetWidth(&w);
    GetHeight(&h);
    canvas->Translate(-0.5f * w, -0.5f * h);
    mDrawable->SetAlpha((Int32) Elastos::Core::Math::Round(mAlpha * 255.0f));
    mDrawable->Draw(canvas);
    canvas->Restore();
    return NOERROR;
}

ECode TargetDrawable::SetEnabled(
    /* [in] */ Boolean enabled)
{
    mEnabled  = enabled;
    return NOERROR;
}

ECode TargetDrawable::GetResourceId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mResourceId;
    return NOERROR;
}

} // namespace Multiwaveview
} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos
