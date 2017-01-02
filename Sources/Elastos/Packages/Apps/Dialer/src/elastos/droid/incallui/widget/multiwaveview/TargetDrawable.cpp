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

#include "elastos/droid/incallui/widget/multiwaveview/TargetDrawable.h"
#include <elastos/droid/R.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::Drawable::IDrawableContainer;
using Elastos::Droid::Graphics::Drawable::IStateListDrawable;
using Elastos::Droid::R;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {
namespace Widget {
namespace MultiwaveView {

static AutoPtr<ArrayOf<Int32> > Init_STATE_ACTIVE()
{
    AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(2);
    (*states)[0] = R::attr::state_enabled;
    (*states)[1] = R::attr::state_active;
    return states;
}

static AutoPtr<ArrayOf<Int32> > Init_STATE_INACTIVE()
{
    AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(2);
    (*states)[0] = R::attr::state_enabled;
    (*states)[1] = -R::attr::state_active;
    return states;
}

static AutoPtr<ArrayOf<Int32> > Init_STATE_FOCUSED()
{
    AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(3);
    (*states)[0] = R::attr::state_enabled;
    (*states)[1] = -R::attr::state_active;
    (*states)[2] = R::attr::state_focused;
    return states;
}

const String TargetDrawable::TAG(String("TargetDrawable"));
const Boolean TargetDrawable::DEBUG = FALSE;
AutoPtr<ArrayOf<Int32> > TargetDrawable::STATE_ACTIVE = Init_STATE_ACTIVE();
AutoPtr<ArrayOf<Int32> > TargetDrawable::STATE_INACTIVE = Init_STATE_INACTIVE();
AutoPtr<ArrayOf<Int32> > TargetDrawable::STATE_FOCUSED = Init_STATE_FOCUSED();

CAR_INTERFACE_IMPL(TargetDrawable, Object, ITargetDrawable);

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
    , mNumDrawables(1)
{}

ECode TargetDrawable::constructor(
    /* [in] */ IResources* res,
    /* [in] */ Int32 resId,
    /* [in] */ Int32 count)
{
    mResourceId = resId;
    mNumDrawables = count;
    SetDrawable(res, resId);
    return NOERROR;
}

ECode TargetDrawable::constructor(
    /* [in] */ ITargetDrawable* _other)
{
    TargetDrawable* other = (TargetDrawable*)_other;
    mResourceId = other->mResourceId;
    // Mutate the drawable so we can animate shared drawable properties.
    if (other->mDrawable != NULL) {
        other->mDrawable->Mutate();
        mDrawable = other->mDrawable;
    }
    ResizeDrawables();
    SetState(STATE_INACTIVE);
    return NOERROR;
}

void TargetDrawable::SetDrawable(
    /* [in] */ IResources* res,
    /* [in] */ Int32 resId)
{
    // Note we explicitly don't set mResourceId to resId since we allow the drawable to be
    // swapped at runtime and want to re-use the existing resource id for identification.
    AutoPtr<IDrawable> drawable;
    if (resId != 0){
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
}

void TargetDrawable::SetState(
    /* [in] */ ArrayOf<Int32>* state)
{
    if (IStateListDrawable::Probe(mDrawable) != NULL) {
        Boolean res = FALSE;
        mDrawable->SetState(state, &res);
    }
}

Boolean TargetDrawable::IsActive()
{
    if (IStateListDrawable::Probe(mDrawable) != NULL) {
        AutoPtr<ArrayOf<Int32> > states;
        mDrawable->GetState((ArrayOf<Int32>**)&states);
        for (Int32 i = 0; i < states->GetLength(); i++) {
            if ((*states)[i] == R::attr::state_focused) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

Boolean TargetDrawable::IsEnabled()
{
    return mDrawable != NULL && mEnabled;
}

void TargetDrawable::ResizeDrawables()
{
    if (IStateListDrawable::Probe(mDrawable) != NULL) {
        Int32 maxWidth = 0;
        Int32 maxHeight = 0;

        for (Int32 i = 0; i < mNumDrawables; i++) {
            Boolean result;
            IDrawableContainer::Probe(mDrawable)->SelectDrawable(i, &result);
            AutoPtr<IDrawable> childDrawable;
            mDrawable->GetCurrent((IDrawable**)&childDrawable);
            Int32 w = 0, h = 0;
            childDrawable->GetIntrinsicWidth(&w);
            maxWidth = Elastos::Core::Math::Max(maxWidth, w);
            childDrawable->GetIntrinsicHeight(&h);
            maxHeight = Elastos::Core::Math::Max(maxHeight, h);
        }

        if (DEBUG) {
            Logger::V(TAG, "union of childDrawable rects %s to: %dx%d", TO_CSTR(mDrawable), maxWidth, maxHeight);
        }
        mDrawable->SetBounds(0, 0, maxWidth, maxHeight);

        for (Int32 i = 0; i < mNumDrawables; i++) {
            Boolean result;
            IDrawableContainer::Probe(mDrawable)->SelectDrawable(i, &result);
            AutoPtr<IDrawable> childDrawable;
            mDrawable->GetCurrent((IDrawable**)&childDrawable);
            if (DEBUG) {
                Logger::V(TAG, "sizing drawable %s to: %dx%d", TO_CSTR(childDrawable), maxWidth, maxHeight);
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

void TargetDrawable::SetX(
    /* [in] */ Float x)
{
    mTranslationX = x;
}

void TargetDrawable::SetY(
    /* [in] */ Float y)
{
    mTranslationY = y;
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

Float TargetDrawable::GetX()
{
    return mTranslationX;
}

Float TargetDrawable::GetY()
{
    return mTranslationY;
}

Float TargetDrawable::GetScaleX()
{
    return mScaleX;
}

Float TargetDrawable::GetScaleY()
{
    return mScaleY;
}

Float TargetDrawable::GetAlpha()
{
    return mAlpha;
}

void TargetDrawable::SetPositionX(
    /* [in] */ Float x)
{
    mPositionX = x;
}

void TargetDrawable::SetPositionY(
    /* [in] */ Float y)
{
    mPositionY = y;
}

Float TargetDrawable::GetPositionX()
{
    return mPositionX;
}

Float TargetDrawable::GetPositionY()
{
    return mPositionY;
}

Int32 TargetDrawable::GetWidth()
{
    Int32 w = 0;
    return mDrawable != NULL ? (mDrawable->GetIntrinsicWidth(&w), w) : 0;
}

Int32 TargetDrawable::GetHeight()
{
    Int32 h = 0;
    return mDrawable != NULL ? (mDrawable->GetIntrinsicHeight(&h), h) : 0;
}

void TargetDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    if (mDrawable == NULL || !mEnabled) {
        return;
    }
    Int32 res = 0;
    canvas->Save(ICanvas::MATRIX_SAVE_FLAG, &res);
    canvas->Scale(mScaleX, mScaleY, mPositionX, mPositionY);
    canvas->Translate(mTranslationX + mPositionX, mTranslationY + mPositionY);
    canvas->Translate(-0.5f * GetWidth(), -0.5f * GetHeight());
    mDrawable->SetAlpha((Int32) Elastos::Core::Math::Round(mAlpha * 255.0f));
    mDrawable->Draw(canvas);
    canvas->Restore();
}

void TargetDrawable::SetEnabled(
    /* [in] */ Boolean enabled)
{
    mEnabled  = enabled;
}

Int32 TargetDrawable::GetResourceId()
{
    return mResourceId;
}

} // namespace MultiwaveView
} // namespace Widget
} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
