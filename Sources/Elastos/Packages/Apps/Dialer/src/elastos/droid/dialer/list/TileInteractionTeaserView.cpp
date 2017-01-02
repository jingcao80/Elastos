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

#include "elastos/droid/dialer/list/TileInteractionTeaserView.h"
#include <elastos/droid/view/View.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::EIID_IAnimatorListener;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::View;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Core::IInteger32;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

//=================================================================
// TileInteractionTeaserView::DismissClickListener
//=================================================================
CAR_INTERFACE_IMPL(TileInteractionTeaserView::DismissClickListener, Object, IViewOnClickListener);

TileInteractionTeaserView::DismissClickListener::DismissClickListener(
    /* [in] */ TileInteractionTeaserView* host)
    : mHost(host)
{}

ECode TileInteractionTeaserView::DismissClickListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->StartDestroyAnimation();
    return NOERROR;
}

//=================================================================
// TileInteractionTeaserView::ArrowRunnable
//=================================================================
TileInteractionTeaserView::ArrowRunnable::ArrowRunnable(
    /* [in] */ TileInteractionTeaserView* host,
    /* [in] */ ITextView* text,
    /* [in] */ IImageView* arrow)
    : mHost(host)
    , mText(text)
    , mArrow(arrow)
{}

ECode TileInteractionTeaserView::ArrowRunnable::Run()
{
    // The text top is changed when we move the arrow, so we need to
    // do multiple passes
    Int32 textTop;
    IView::Probe(mText)->GetTop(&textTop);
    AutoPtr<IView> arraw = IView::Probe(mArrow);
    if (mHost->mNeedLayout || textTop != mHost->mTextTop) {
        mHost->mNeedLayout = FALSE;
        mHost->mTextTop = textTop;

        Int32 lineHeight;
        mText->GetLineHeight(&lineHeight);
        AutoPtr<IViewGroupLayoutParams> arrowParams;
        arraw->GetLayoutParams((IViewGroupLayoutParams**)&arrowParams);
        IViewGroupMarginLayoutParams::Probe(arrowParams)->SetTopMargin(
                mHost->mTextTop + lineHeight / 2);
        arraw->SetLayoutParams(arrowParams);
    }
    arraw->SetVisibility(IView::VISIBLE);
    return NOERROR;
}

//=================================================================
// TileInteractionTeaserView::HeightAnimatorUpdateListener
//=================================================================
CAR_INTERFACE_IMPL(TileInteractionTeaserView::HeightAnimatorUpdateListener, Object, IAnimatorUpdateListener);

TileInteractionTeaserView::HeightAnimatorUpdateListener::HeightAnimatorUpdateListener(
    /* [in] */ TileInteractionTeaserView* host)
    : mHost(host)
{}

ECode TileInteractionTeaserView::HeightAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> value;
    animation->GetAnimatedValue((IInterface**)&value);
    IInteger32::Probe(value)->GetValue(&mHost->mAnimatedHeight);
    mHost->RequestLayout();
    return NOERROR;
}

//=================================================================
// TileInteractionTeaserView::HeightAnimatorListener
//=================================================================
CAR_INTERFACE_IMPL(TileInteractionTeaserView::HeightAnimatorListener, Object, IAnimatorListener)

TileInteractionTeaserView::HeightAnimatorListener::HeightAnimatorListener(
    /* [in] */ TileInteractionTeaserView* host)
    : mHost(host)
{}

ECode TileInteractionTeaserView::HeightAnimatorListener::OnAnimationStart(
    /* [in] */ IAnimator* animator)
{
    return NOERROR;
}

ECode TileInteractionTeaserView::HeightAnimatorListener::OnAnimationEnd(
    /* [in] */ IAnimator* animator)
{
    mHost->SetVisibility(IView::GONE);
    mHost->SetDismissed();
    assert(0);
    // if (mHost->mAdapter != NULL) {
    //     mHost->mAdapter->NotifyDataSetChanged();
    // }
    return NOERROR;
}

ECode TileInteractionTeaserView::HeightAnimatorListener::OnAnimationCancel(
    /* [in] */ IAnimator* animator)
{
    return NOERROR;
}

ECode TileInteractionTeaserView::HeightAnimatorListener::OnAnimationRepeat(
    /* [in] */ IAnimator* animator)
{
    return NOERROR;
}

//=================================================================
// TileInteractionTeaserView
//=================================================================
Int32 TileInteractionTeaserView::sShrinkAnimationDuration;

const String TileInteractionTeaserView::KEY_TILE_INTERACTION_TEASER_SHOWN("key_tile_interaction_teaser_shown");

CAR_INTERFACE_IMPL(TileInteractionTeaserView, FrameLayout, ITileInteractionTeaserView)

TileInteractionTeaserView::TileInteractionTeaserView()
    : mNeedLayout(FALSE)
    , mTextTop(0)
    , mAnimatedHeight(-1)
{}

ECode TileInteractionTeaserView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode TileInteractionTeaserView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FrameLayout::constructor(context, attrs);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);

    mNeedLayout = TRUE;
    resources->GetInteger(Elastos::Droid::Dialer::R::integer::escape_animation_duration,
            &sShrinkAnimationDuration);
    return NOERROR;
}

ECode TileInteractionTeaserView::OnFinishInflate()
{
    AutoPtr<IView> view;
    FindViewById(Elastos::Droid::Dialer::R::id::dismiss_button, (IView**)&view);
    AutoPtr<IViewOnClickListener> listener = (IViewOnClickListener*)new DismissClickListener(this);
    view->SetOnClickListener(listener);
    return NOERROR;
}

ECode TileInteractionTeaserView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    FrameLayout::OnLayout(changed, left, top, right, bottom);

    AutoPtr<IView> text;
    FindViewById(Elastos::Droid::Dialer::R::id::text, (IView**)&text);
    AutoPtr<IView> arrow;
    FindViewById(Elastos::Droid::Dialer::R::id::arrow, (IView**)&arrow);

    // We post to avoid calling layout within layout
    Boolean result;
    AutoPtr<IRunnable> runnable = (IRunnable*)new ArrowRunnable(this,
            ITextView::Probe(text), IImageView::Probe(arrow));
    arrow->Post(runnable, &result);
    return NOERROR;
}

Boolean TileInteractionTeaserView::GetShouldDisplayInList()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ISharedPreferences> prefs;
    context->GetSharedPreferences(IDialtactsActivity::SHARED_PREFS_NAME,
            IContext::MODE_PRIVATE, (ISharedPreferences**)&prefs);
    Boolean result;
    prefs->GetBoolean(KEY_TILE_INTERACTION_TEASER_SHOWN, TRUE, &result);
    return result;
}

// void TileInteractionTeaserView::SetAdapter(
//     /* [in] */ ShortcutCardsAdapter* adapter)
// {
//     mAdapter = adapter;
//     return NOERROR;
// }

void TileInteractionTeaserView::StartDestroyAnimation()
{
    Int32 start;
    GetHeight(&start);
    Int32 end = 0;
    mAnimatedHeight = start;
    Logger::V(String("Interaction"), "Start from %d", start);

    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<IValueAnimator> heightAnimator;
    AutoPtr<ArrayOf<Int32> > attrs = ArrayOf<Int32>::Alloc(2);
    (*attrs)[0] = start;
    (*attrs)[1] = end;
    helper->OfInt32(attrs, (IValueAnimator**)&heightAnimator);
    heightAnimator->SetDuration(sShrinkAnimationDuration);

    AutoPtr<ITimeInterpolator> interpolator;
    CDecelerateInterpolator::New(2.0f, (ITimeInterpolator**)&interpolator);
    AutoPtr<IAnimator> animator = IAnimator::Probe(heightAnimator);
    animator->SetInterpolator(interpolator);
    AutoPtr<IAnimatorUpdateListener> listener = (IAnimatorUpdateListener*)new HeightAnimatorUpdateListener(this);
    heightAnimator->AddUpdateListener(listener);
    AutoPtr<IAnimatorListener> animatorListener = (IAnimatorListener*)new HeightAnimatorListener(this);
    animator->AddListener(animatorListener);

    animator->Start();
}

void TileInteractionTeaserView::SetDismissed()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ISharedPreferences> prefs;
    context->GetSharedPreferences(IDialtactsActivity::SHARED_PREFS_NAME,
            IContext::MODE_PRIVATE, (ISharedPreferences**)&prefs);
    AutoPtr<ISharedPreferencesEditor> editor;
    prefs->Edit((ISharedPreferencesEditor**)&editor);
    editor->PutBoolean(KEY_TILE_INTERACTION_TEASER_SHOWN, FALSE);
    editor->Apply();
}

ECode TileInteractionTeaserView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    if (mAnimatedHeight == -1) {
        FrameLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    }
    else {
        SetMeasuredDimension(View::MeasureSpec::GetSize(widthMeasureSpec), mAnimatedHeight);
    }
    return NOERROR;
}

} // List
} // Dialer
} // Droid
} // Elastos
