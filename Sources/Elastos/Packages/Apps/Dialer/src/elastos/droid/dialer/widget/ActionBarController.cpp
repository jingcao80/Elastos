
#include "elastos/droid/dialer/widget/ActionBarController.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::App::IActionBar;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Core::IFloat;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Widget {

//================================================================
// ActionBarController::MyAnimatorUpdateListener
//================================================================
CAR_INTERFACE_IMPL(ActionBarController::MyAnimatorUpdateListener, Object, IAnimatorUpdateListener);

ActionBarController::MyAnimatorUpdateListener::MyAnimatorUpdateListener(
    /* [in] */ ActionBarController* host)
    : mHost(host)
{}

ECode ActionBarController::MyAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> animationValue;
    animation->GetAnimatedValue((IInterface**)&animationValue);
    Float value;
    IFloat::Probe(animationValue)->GetValue(&value);
    Int32 height;
    mHost->mActivityUi->GetActionBarHeight(&height);
    mHost->SetHideOffset((Int32)(height * value));
    return NOERROR;
}


//================================================================
// ActionBarController
//================================================================
const Boolean ActionBarController::DEBUG = TRUE;
const String ActionBarController::TAG("ActionBarController");

const String ActionBarController::KEY_IS_SLID_UP("key_actionbar_is_slid_up");
const String ActionBarController::KEY_IS_FADED_OUT("key_actionbar_is_faded_out");
const String ActionBarController::KEY_IS_EXPANDED("key_actionbar_is_expanded");

ActionBarController::ActionBarController(
    /* [in] */ IActionBarControllerActivityUi* activityUi,
    /* [in] */ CSearchEditTextLayout* searchBox)
    : mIsActionBarSlidUp(FALSE)
{
    mActivityUi = activityUi;
    mSearchBox = searchBox;
}

Boolean ActionBarController::IsActionBarShowing()
{
    Boolean isFadedOut;
    mSearchBox->IsFadedOut(&isFadedOut);
    return !mIsActionBarSlidUp && !isFadedOut;
}

void ActionBarController::OnSearchBoxTapped()
{
    Boolean result;
    mActivityUi->IsInSearchUi(&result);
    if (DEBUG) {
        Logger::D(TAG, "OnSearchBoxTapped: isInSearchUi %d", result);
    }
    if (!result) {
        mSearchBox->Expand(TRUE /* animate */, TRUE /* requestFocus */);
    }
}

void ActionBarController::OnSearchUiExited()
{
    Boolean isExpanded;
    mSearchBox->IsExpanded(&isExpanded);
    Boolean isFadedOut;
    mSearchBox->IsFadedOut(&isFadedOut);
    Boolean shouldShowActionBar;
    mActivityUi->ShouldShowActionBar(&shouldShowActionBar);
    if (DEBUG) {
        Logger::D(TAG, "OnSearchUIExited: isExpanded %d isFadedOut: %d shouldShowActionBar: %d",
                isExpanded, isFadedOut, shouldShowActionBar);
    }
    if (isExpanded) {
        mSearchBox->Collapse(TRUE /* animate */);
    }
    if (isFadedOut) {
        mSearchBox->FadeIn();
    }

    if (shouldShowActionBar) {
        SlideActionBar(FALSE /* slideUp */, FALSE /* animate */);
    }
    else {
        SlideActionBar(TRUE /* slideUp */, FALSE /* animate */);
    }
}

void ActionBarController::OnDialpadDown()
{
    Boolean isInSearchUi;
    mActivityUi->IsInSearchUi(&isInSearchUi);
    Boolean hasSearchQuery;
    mActivityUi->HasSearchQuery(&hasSearchQuery);
    Boolean isExpanded;
    mSearchBox->IsExpanded(&isExpanded);
    Boolean isFadedOut;
    mSearchBox->IsFadedOut(&isFadedOut);

    if (DEBUG) {
        Logger::D(TAG, "OnDialpadDown: isInSearchUi %d hasSearchQuery: %d isFadedOut: %d isExpanded: %d",
                isInSearchUi, hasSearchQuery, isFadedOut, isExpanded);
    }
    if (isInSearchUi) {
        if (hasSearchQuery) {
            if (isFadedOut) {
                mSearchBox->SetVisible(TRUE);
            }
            if (!isExpanded) {
                mSearchBox->Expand(FALSE /* animate */, FALSE /* requestFocus */);
            }
            SlideActionBar(FALSE /* slideUp */, TRUE /* animate */);
        }
        else {
            mSearchBox->FadeIn();
        }
    }
}

void ActionBarController::OnDialpadUp()
{
    Boolean isInSearchUi;
    mActivityUi->IsInSearchUi(&isInSearchUi);
    if (DEBUG) {
        Logger::D(TAG, "OnDialpadUp: isInSearchUi %d", isInSearchUi);
    }
    if (isInSearchUi) {
        SlideActionBar(TRUE /* slideUp */, TRUE /* animate */);
    }
    else {
        // From the lists fragment
        // TODO:
        // mSearchBox->FadeOut(mFadeOutCallback);
    }
}

void ActionBarController::SlideActionBar(
    /* [in] */ Boolean slideUp,
    /* [in] */ Boolean animate)
{
    if (DEBUG) {
        Logger::D(TAG, "Sliding actionBar - up: %d animate: %d", slideUp, animate);
    }
    if (animate) {
        AutoPtr<IValueAnimatorHelper> helper;
        CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
        AutoPtr<ArrayOf<Float> > args = ArrayOf<Float>::Alloc(2);
        AutoPtr<IValueAnimator> animator;
        if (slideUp) {
            args->Set(0, 0);
            args->Set(1, 1);
            helper->OfFloat(args, (IValueAnimator**)&animator);
        }
        else {
            args->Set(0, 1);
            args->Set(1, 0);
            helper->OfFloat(args, (IValueAnimator**)&animator);
        }
        animator->AddUpdateListener(new MyAnimatorUpdateListener(this));
        IAnimator::Probe(animator)->Start();
    }
    else {
        Int32 height;
        mActivityUi->GetActionBarHeight(&height);
        SetHideOffset(slideUp ? height : 0);
    }
    mIsActionBarSlidUp = slideUp;
}

void ActionBarController::SetAlpha(
    /* [in] */ Float alphaValue)
{
    AutoPtr<IViewPropertyAnimator> animator;
    IView::Probe(mSearchBox)->Animate((IViewPropertyAnimator**)&animator);
    animator->Alpha(alphaValue);
    animator->Start();
}

void ActionBarController::SetHideOffset(
    /* [in] */ Int32 offset)
{
    Int32 height;
    mActivityUi->GetActionBarHeight(&height);
    mIsActionBarSlidUp = offset >= height;
    AutoPtr<IActionBar> actionBar;
    mActivityUi->GetActionBar((IActionBar**)&actionBar);
    actionBar->SetHideOffset(offset);
}

Int32 ActionBarController::GetHideOffset()
{
    AutoPtr<IActionBar> actionBar;
    mActivityUi->GetActionBar((IActionBar**)&actionBar);
    Int32 offset;
    actionBar->GetHideOffset(&offset);
    return offset;
}

void ActionBarController::SaveInstanceState(
    /* [in] */ IBundle* outState)
{
    outState->PutBoolean(KEY_IS_SLID_UP, mIsActionBarSlidUp);
    Boolean isFadedOut;
    mSearchBox->IsFadedOut(&isFadedOut);
    outState->PutBoolean(KEY_IS_FADED_OUT, isFadedOut);
    Boolean isExpanded;
    mSearchBox->IsExpanded(&isExpanded);
    outState->PutBoolean(KEY_IS_EXPANDED, isExpanded);
}

void ActionBarController::RestoreInstanceState(
    /* [in] */ IBundle* inState)
{
    inState->GetBoolean(KEY_IS_SLID_UP, &mIsActionBarSlidUp);

    Boolean isSearchBoxFadedOut;
    inState->GetBoolean(KEY_IS_FADED_OUT, &isSearchBoxFadedOut);
    Boolean isFadedOut;
    if (isSearchBoxFadedOut) {
        if (mSearchBox->IsFadedOut(&isFadedOut), !isFadedOut) {
            mSearchBox->SetVisible(FALSE);
        }
    }
    else if (mSearchBox->IsFadedOut(&isFadedOut), isFadedOut) {
            mSearchBox->SetVisible(TRUE);
    }

    Boolean isSearchBoxExpanded;
    inState->GetBoolean(KEY_IS_EXPANDED, &isSearchBoxExpanded);
    Boolean isExpanded;
    if (isSearchBoxExpanded) {
        if (mSearchBox->IsExpanded(&isExpanded), !isExpanded) {
            mSearchBox->Expand(FALSE, FALSE);
        }
    }
    else if (mSearchBox->IsExpanded(&isExpanded), isExpanded) {
            mSearchBox->Collapse(FALSE);
    }
}

void ActionBarController::RestoreActionBarOffset()
{
    Logger::E("ActionBarController", "mIsActionBarSlidUp : %d", mIsActionBarSlidUp);
    SlideActionBar(mIsActionBarSlidUp /* slideUp */, FALSE /* animate */);
}

Boolean ActionBarController::GetIsActionBarSlidUp()
{
    return mIsActionBarSlidUp;
}

} // Widget
} // Dialer
} // Droid
} // Elastos
