
#include "elastos/apps/dialer/list/TileInteractionTeaserView.h"
#include "R.h"
#include <elastos/droid/view/View.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::View;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Core::IRunnable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Apps {
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
    mText->GetTop(&textTop);
    if (mHost->mNeedLayout || textTop != mHost->mTextTop) {
        mHost->mNeedLayout = FALSE;
        mHost->mTextTop = textTop;

        Int32 lineHeight;
        mText->GetLineHeight(&lineHeight);
        AutoPtr<IViewGroupLayoutParams> arrowParams;
        IView::Probe(mArrow)->GetLayoutParams((IViewGroupLayoutParams**)&arrowParams);
        IViewGroupMarginLayoutParams::Probe(arrowParams)->SetTopMargin(
                mHost->mTextTop + lineHeight / 2);
        mArrow->SetLayoutParams(arrowParams);
    }
    arrow->SetVisibility(IView::VISIBLE);
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
    animation->GetAnimatedValue(&mHost->mAnimatedHeight);
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
    if (mHost->mAdapter != NULL) {
        mHost->mAdapter->NotifyDataSetChanged();
    }
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
    resources->GetInteger(R::integer::escape_animation_duration,
            &sShrinkAnimationDuration);
    return NOERROR;
}

ECode TileInteractionTeaserView::OnFinishInflate()
{
    AutoPtr<IView> view;
    FindViewById(R::id::dismiss_button, (IView**)&view);
    view->SetOnClickListener((IViewOnClickListener*)new DismissClickListener(this));

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
    FindViewById(R::id::text, (IView**)&text);
    AutoPtr<IView> arrow;
    FindViewById(R::id::arrow, (IView**)&arrow);

    // We post to avoid calling layout within layout
    Boolean result;
    arrow->Post((IRunnable*)new ArrowRunnable(this,
            ITextView::Probe(text), IImageView::Probe(arrow)), &result);
    return NOERROR;
}

ECode TileInteractionTeaserView::GetShouldDisplayInList(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ISharedPreferences> prefs;
    context->GetSharedPreferences(IDialtactsActivity::SHARED_PREFS_NAME,
            IContext::MODE_PRIVATE, (ISharedPreferences**)&prefs);
    return prefs->GetBoolean(KEY_TILE_INTERACTION_TEASER_SHOWN, TRUE, result);
}

ECode TileInteractionTeaserView::SetAdapter(
    /* [in] */ IShortcutCardsAdapter* adapter)
{
    mAdapter = adapter;
    return NOERROR;
}

void TileInteractionTeaserView::StartDestroyAnimation()
{
    Int32 start;
    GetHeight(&start);
    Int32 end = 0;
    mAnimatedHeight = start;
    Logger::V(String("Interaction"), "Start from %d", start);

    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton(&helper);
    AutoPtr<IValueAnimator> heightAnimator;
    helper->OfInt32(start, end, (IValueAnimator**)&heightAnimator);
    heightAnimator->SetDuration(sShrinkAnimationDuration);

    AutoPtr<IDecelerateInterpolator> interpolator;
    CDecelerateInterpolator::New(2.0f, (IDecelerateInterpolator**)&interpolator);
    heightAnimator->SetInterpolator(interpolator);
    heightAnimator->AddUpdateListener(
            (IAnimatorUpdateListener*)new HeightAnimatorUpdateListener(this));
    heightAnimator->AddListener((IAnimatorListener*)new HeightAnimatorListener(this));

    heightAnimator->Start();
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
    editor->PutBoolean(KEY_TILE_INTERACTION_TEASER_SHOWN, FALSE)
    editor->Apply();
}

void TileInteractionTeaserView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    if (mAnimatedHeight == -1) {
        FrameLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    }
    else {
        SetMeasuredDimension(View::MeasureSpec::GetSize(widthMeasureSpec), mAnimatedHeight);
    }
}

} // List
} // Dialer
} // Apps
} // Elastos
