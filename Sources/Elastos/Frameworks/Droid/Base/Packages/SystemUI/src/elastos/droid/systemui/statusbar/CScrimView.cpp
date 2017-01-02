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

#include "elastos/droid/systemui/statusbar/CScrimView.h"

using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::PorterDuffMode;
using Elastos::Droid::Graphics::PorterDuffMode_SRC;
using Elastos::Droid::Graphics::PorterDuffMode_SRC_OVER;
using Elastos::Core::IFloat;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CAR_INTERFACE_IMPL(CScrimView::AlphaUpdateListener, Object, IAnimatorUpdateListener)

CScrimView::AlphaUpdateListener::AlphaUpdateListener(
    /* [in] */ CScrimView* host)
    : mHost(host)
{}

ECode CScrimView::AlphaUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    IFloat::Probe(obj)->GetValue(&mHost->mViewAlpha);
    mHost->Invalidate();
    return NOERROR;
}

CScrimView::ClearAnimatorListener::ClearAnimatorListener(
    /* [in] */ CScrimView* host)
    : mHost(host)
{}

ECode CScrimView::ClearAnimatorListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mAlphaAnimator = NULL;
    return NOERROR;
}

CAR_OBJECT_IMPL(CScrimView)

CAR_INTERFACE_IMPL(CScrimView, View, IScrimView)

CScrimView::CScrimView()
    : mScrimColor(0)
    , mIsEmpty(TRUE)
    , mDrawAsSrc(FALSE)
    , mViewAlpha(1.0)
{
}

ECode CScrimView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CScrimView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CScrimView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode CScrimView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    mAlphaUpdateListener = new AlphaUpdateListener(this);
    mClearAnimatorListener = new ClearAnimatorListener(this);
    return View::constructor(context, attrs, defStyleAttr, defStyleRes);
}

void CScrimView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    if (mDrawAsSrc || (!mIsEmpty && mViewAlpha > 0.f)) {
        PorterDuffMode mode = mDrawAsSrc ? PorterDuffMode_SRC : PorterDuffMode_SRC_OVER;

        AutoPtr<IColor> c;
        CColor::AcquireSingleton((IColor**)&c);
        Int32 alpha = 0, red = 0, green = 0, blue = 0;
        c->Alpha(mScrimColor ,&alpha);
        c->Red(mScrimColor, &red);
        c->Green(mScrimColor, &green);
        c->Blue(mScrimColor, &blue);
        Int32 color = 0;
        c->Argb((Int32) (alpha * mViewAlpha), red, green, blue, &color);
        canvas->DrawColor(color, mode);
    }
}

ECode CScrimView::SetDrawAsSrc(
    /* [in] */ Boolean asSrc)
{
    mDrawAsSrc = asSrc;
    Invalidate();
    return NOERROR;
}

ECode CScrimView::SetScrimColor(
    /* [in] */ Int32 color)
{
    if (color != mScrimColor) {
        AutoPtr<IColor> c;
        CColor::AcquireSingleton((IColor**)&c);
        Int32 alpha = 0;
        c->Alpha(color, &alpha);
        mIsEmpty = alpha == 0;
        mScrimColor = color;
        Invalidate();
    }
    return NOERROR;
}

ECode CScrimView::GetScrimColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);
    *color = mScrimColor;
    return NOERROR;
}

ECode CScrimView::HasOverlappingRendering(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    *has = FALSE;
    return NOERROR;
}

ECode CScrimView::SetViewAlpha(
    /* [in] */ Float alpha)
{
    if (mAlphaAnimator != NULL) {
        IAnimator::Probe(mAlphaAnimator)->Cancel();
    }
    mViewAlpha = alpha;
    Invalidate();
    return NOERROR;
}

ECode CScrimView::AnimateViewAlpha(
    /* [in] */ Float alpha,
    /* [in] */ Int64 durationOut,
    /* [in] */ IInterpolator* interpolator)
{
    if (mAlphaAnimator != NULL) {
        IAnimator::Probe(mAlphaAnimator)->Cancel();
        mAlphaAnimator = NULL;
    }

    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);

    AutoPtr<ArrayOf<Float> > floats = ArrayOf<Float>::Alloc(2);
    (*floats)[0] = mViewAlpha;
    (*floats)[1] = alpha;
    helper->OfFloat(floats, (IValueAnimator**)&mAlphaAnimator);
    IAnimator* animator = IAnimator::Probe(mAlphaAnimator);
    mAlphaAnimator->AddUpdateListener(mAlphaUpdateListener);
    animator->AddListener(mClearAnimatorListener);
    animator->SetInterpolator(ITimeInterpolator::Probe(interpolator));
    mAlphaAnimator->SetDuration(durationOut);
    animator->Start();
    return NOERROR;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
