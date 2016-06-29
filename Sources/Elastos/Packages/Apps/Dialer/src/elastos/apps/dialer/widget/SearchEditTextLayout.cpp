
#include "elastos/apps/dialer/widget/SearchEditTextLayout.h"
#include "elastos/apps/dialer/util/DialerUtils.h"
#include <elastos/core/CoreUtils.h>
#include "R.h"

using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Core::CoreUtils;
using Elastos::Apps::Dialer::Util::DialerUtils;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Widget {


//================================================================
// SearchEditTextLayout::SearchViewFocusChangeListener
//================================================================
CAR_INTERFACE_IMPL(SearchEditTextLayout::SearchViewFocusChangeListener, Object, IViewOnFocusChangeListener);

SearchEditTextLayout::SearchViewFocusChangeListener::SearchViewFocusChangeListener(
    /* [in] */ SearchEditTextLayout* host)
    : mHost(host)
{}

ECode SearchEditTextLayout::SearchViewFocusChangeListener::OnFocusChange(
    /* [in] */ IView* v,
    /* [in] */ Boolean hasFocus)
{
    if (hasFocus) {
        DialerUtils::ShowInputMethod(v);
    }
    else {
        DialerUtils::HideInputMethod(v);
    }
    return NOERROR;
}

//================================================================
// SearchEditTextLayout::SearchCloseButtonClickListener
//================================================================
CAR_INTERFACE_IMPL(SearchEditTextLayout::SearchCloseButtonClickListener, Object, IViewOnClickListener);

SearchEditTextLayout::SearchCloseButtonClickListener::SearchCloseButtonClickListener(
    /* [in] */ SearchEditTextLayout* host)
    : mHost(host)
{}

ECode SearchEditTextLayout::SearchCloseButtonClickListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->mSearchView->SetText(NULL);
    return NOERROR;
}

//================================================================
// SearchEditTextLayout::SearchBackButtonClickListener
//================================================================
CAR_INTERFACE_IMPL(SearchEditTextLayout::SearchBackButtonClickListener, Object, IViewOnClickListener);

SearchEditTextLayout::SearchBackButtonClickListener::SearchBackButtonClickListener(
    /* [in] */ SearchEditTextLayout* host)
    : mHost(host)
{}

ECode SearchEditTextLayout::SearchBackButtonClickListener::OnClick(
    /* [in] */ IView* v)
{
    if (mHost->mOnBackButtonClickedListener != NULL) {
        mHost->mOnBackButtonClickedListener->OnBackButtonClicked();
    }
    return NOERROR;
}


//================================================================
// SearchEditTextLayout::AnimatorUpdateListener
//================================================================
CAR_INTERFACE_IMPL(SearchEditTextLayout::AnimatorUpdateListener, Object, IAnimatorUpdateListener);

SearchEditTextLayout::AnimatorUpdateListener::AnimatorUpdateListener(
    /* [in] */ SearchEditTextLayout* host)
    : mHost(host)
{}

ECode SearchEditTextLayout::AnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> value;
    animation->GetAnimatedValue((IInterface**)&value);
    Float fraction = CoreUtils::Unbox(value);
    mHost->SetMargins(fraction);
    return NOERROR;
}

//================================================================
// SearchEditTextLayout
//================================================================
CAR_INTERFACE_IMPL(SearchEditTextLayout, FrameLayout, ISearchEditTextLayout);

SearchEditTextLayout::SearchEditTextLayout()
    : mIsExpanded(FALSE)
    , mIsFadedOut(FALSE)
{}

ECode SearchEditTextLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return FrameLayout::constructor(context, attrs);
}

ECode SearchEditTextLayout::SetPreImeKeyListener(
    /* [in] */ IViewOnKeyListener* listener)
{
    mPreImeKeyListener = listener;
    return NOERROR;
}

ECode SearchEditTextLayout::SetOnBackButtonClickedListener(
    /* [in] */ ISearchEditTextLayoutOnBackButtonClickedListener* listener)
{
    mOnBackButtonClickedListener = listener;
    return NOERROR;
}

ECode SearchEditTextLayout::OnFinishInflate()
{
    AutoPtr<IViewGroupLayoutParams> lp;
    GetLayoutParams((IViewGroupLayoutParams**)&lp);
    IViewGroupMarginLayoutParams* params = IViewGroupMarginLayoutParams::Probe(lp);
    params->GetTopMargin(&mTopMargin);
    params->GetBottomMargin(&mBottomMargin);
    params->GetLeftMargin(&mLeftMargin);
    params->GetRightMargin(&mRightMargin);

    GetElevation(&mCollapsedElevation);

    FindViewById(R::id::search_box_collapsed, (IView**)&mCollapsed);
    FindViewById(R::id::search_box_expanded, (IView**)&mExpanded);
    AutoPtr<IView> view;
    mExpanded->FindViewById(R::id::search_view, (IView**)&view);
    mSearchView = IEditText::Probe(view);

    FindViewById(R::id::search_magnifying_glass, (IView**)&mSearchIcon);
    FindViewById(R::id::search_box_start_search, (IView**)&mCollapsedSearchBox);
    FindViewById(R::id::voice_search_button, (IView**)&mVoiceSearchButtonView);
    FindViewById(R::id::dialtacts_options_menu_button, (IView**)&mOverflowButtonView);
    FindViewById(R::id::search_back_button, (IView**)&mBackButtonView);
    FindViewById(R::id::search_box_expanded, (IView**)&mExpandedSearchBox);
    FindViewById(R::id::search_close_button, (IView**)&mClearButtonView);

    mSearchView->SetOnFocusChangeListener(new SearchViewFocusChangeListener(this));

    view = NULL;
    FindViewById(R::id::search_close_button, (IView**)&view);
    view->SetOnClickListener(new SearchCloseButtonClickListener(this));

    view = NULL;
    FindViewById(R::id::search_back_button, (IView**)&view);
    view->SetOnClickListener(new SearchBackButtonClickListener(this));

    return FrameLayout::OnFinishInflate();
}

ECode SearchEditTextLayout::DispatchKeyEventPreIme(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    if (mPreImeKeyListener != NULL) {
        Int32 keyCode;
        event->GetKeyCode(&keyCode);
        if (mPreImeKeyListener->OnKey(this, keyCode, event)) {
            *res = TRUE;
            return NOERROR;
        }
    }
    return FrameLayout::DispatchKeyEventPreIme(event, res);
}

ECode SearchEditTextLayout::FadeOut()
{
    assert(0 && "TODO");
    // return FadeOut(NULL);
    return NOERROR;
}

// TODO:
// ECode SearchEditTextLayout::FadeOut(
//     /* [in] */ AnimUtils.AnimationCallback callback)
// {
//     AnimUtils.fadeOut(this, ANIMATION_DURATION, callback);
//     mIsFadedOut = TRUE;
//     return NOERROR;
// }

ECode SearchEditTextLayout::FadeIn()
{
    assert(0 && "TODO");
    // AnimUtils.fadeIn(this, ANIMATION_DURATION);
    mIsFadedOut = FALSE;
    return NOERROR;
}

ECode SearchEditTextLayout::SetVisible(
    /* [in] */ Boolean visible)
{
    if (visible) {
        SetAlpha(1);
        SetVisibility(IView::VISIBLE);
        mIsFadedOut = FALSE;
    }
    else {
        SetAlpha(0);
        SetVisibility(IView::GONE);
        mIsFadedOut = TRUE;
    }
    return NOERROR;
}

ECode SearchEditTextLayout::Expand(
    /* [in] */ Boolean animate,
    /* [in] */ Boolean requestFocus)
{
    UpdateVisibility(TRUE /* isExpand */);

    if (animate) {
        assert(0 && "TODO");
        // AnimUtils.crossFadeViews(mExpanded, mCollapsed, ANIMATION_DURATION);
        AutoPtr<IValueAnimatorHelper> helper;
        CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
        helper->OfFloat(EXPAND_MARGIN_FRACTION_START, 0f, &mAnimator);
        SetMargins(EXPAND_MARGIN_FRACTION_START);
        PrepareAnimator(TRUE);
    }
    else {
        mExpanded->SetVisibility(IView::VISIBLE);
        mExpanded->SetAlpha(1);
        SetMargins(0f);
        mCollapsed->SetVisibility(IView::GONE);
    }

    // Set 9-patch background. This owns the padding, so we need to restore the original values.
    Int32 paddingTop;
    GetPaddingTop(&paddingTop);
    Int32 paddingStart;
    GetPaddingStart(&paddingStart);
    Int32 paddingBottom;
    GetPaddingBottom(&paddingBottom);
    Int32 paddingEnd;
    GetPaddingEnd(&paddingEnd);
    SetBackgroundResource(R::drawable::search_shadow);
    SetElevation(0);
    SetPaddingRelative(paddingStart, paddingTop, paddingEnd, paddingBottom);

    SetElevation(0);
    if (requestFocus) {
        mSearchView->RequestFocus();
    }
    mIsExpanded = TRUE;
    return NOERROR;
}

ECode SearchEditTextLayout::Collapse(
    /* [in] */ Boolean animate)
{
    UpdateVisibility(FALSE /* isExpand */);

    if (animate) {
        assert(0 && "TODO");
        // AnimUtils.crossFadeViews(mCollapsed, mExpanded, ANIMATION_DURATION);
        AutoPtr<IValueAnimatorHelper> helper;
        CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
        helper->OfFloat(0, 1, &mAnimator);
        PrepareAnimator(FALSE);
    }
    else {
        mCollapsed->SetVisibility(IView::VISIBLE);
        mCollapsed->SetAlpha(1);
        SetMargins(1);
        mExpanded->SetVisibility(IView::GONE);
    }

    mIsExpanded = FALSE;
    SetElevation(mCollapsedElevation);
    SetBackgroundResource(R::drawable::rounded_corner);
    return NOERROR;
}

void SearchEditTextLayout::UpdateVisibility(
    /* [in] */ Boolean isExpand)
{
    Int32 collapsedViewVisibility = isExpand ? IView::GONE : IView::VISIBLE;
    Int32 expandedViewVisibility = isExpand ? IView::VISIBLE : IView::GONE;

    mSearchIcon->SetVisibility(collapsedViewVisibility);
    mCollapsedSearchBox->SetVisibility(collapsedViewVisibility);
    mVoiceSearchButtonView->SetVisibility(collapsedViewVisibility);
    mOverflowButtonView->SetVisibility(collapsedViewVisibility);
    mBackButtonView->SetVisibility(expandedViewVisibility);
    // TODO: Prevents keyboard from jumping up in landscape mode after exiting the
    // SearchFragment when the query string is empty. More elegant fix?
    //mExpandedSearchBox.setVisibility(expandedViewVisibility);
    mClearButtonView->SetVisibility(expandedViewVisibility);
}

void SearchEditTextLayout::PrepareAnimator(
    /* [in] */ Boolean expand)
{
    if (mAnimator != NULL) {
        mAnimator->Cancel();
    }

    mAnimator->SddUpdateListener(new AnimatorUpdateListener(this));

    mAnimator->SetDuration(ANIMATION_DURATION);
    mAnimator->Start();
}


ECode SearchEditTextLayout::IsExpanded(
    /* [out] */ Boolean* isExpanded)
{
    VALIDATE_NOT_NULL(isExpanded);
    *isExpanded = mIsExpanded;
    return NOERROR;
}

ECode SearchEditTextLayout::IsFadedOut(
    /* [out] */ Boolean* isFadedOut)
{
    VALIDATE_NOT_NULL(isFadedOut);
    *isFadedOut = mIsFadedOut;
    return NOERROR;
}

void SearchEditTextLayout::SetMargins(
    /* [in] */ Float fraction)
{
    AutoPtr<IViewGroupLayoutParams> lp;
    GetLayoutParams((IViewGroupLayoutParams**)&lp);
    IViewGroupMarginLayoutParams* params = IViewGroupMarginLayoutParams::Probe(lp);

    params->SetTopMargin((Int32) (mTopMargin * fraction));
    params->SetBottomMargin((Int32) (mBottomMargin * fraction));
    params->SetLeftMargin((Int32) (mLeftMargin * fraction));
    params->SetRightMargin((Int32) (mRightMargin * fraction));
    RequestLayout();
}

} // Widget
} // Dialer
} // Apps
} // Elastos
