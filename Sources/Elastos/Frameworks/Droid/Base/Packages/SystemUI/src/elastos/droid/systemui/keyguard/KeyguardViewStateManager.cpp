
#include "elastos/droid/systemui/keyguard/KeyguardViewStateManager.h"
#include "elastos/droid/os/Looper.h"

using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::View::IViewPropertyAnimator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode KeyguardViewStateManager::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return mHost->mKeyguardSecurityContainer->OnPause();
}

ECode KeyguardViewStateManager::MyAnimatorListenerAdapter2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    Boolean res;
    if (IView::Probe(mHost->mKeyguardSecurityContainer)->IsShown(&res), res) {
        mHost->mKeyguardSecurityContainer->OnResume(0);
    }
    return NOERROR;
}

ECode KeyguardViewStateManager::MyRunnable::Run()
{
    if (mHost->mKeyguardWidgetPager != NULL) {
        mHost->mKeyguardWidgetPager->HideOutlinesAndSidePages();
    }
    return NOERROR;
}

ECode KeyguardViewStateManager::MyRunnable2::Run()
{
    return mHost->mKeyguardSecurityContainer->ShowUsabilityHint();
}

const String KeyguardViewStateManager::TAG("KeyguardViewStateManager");

const Int32 KeyguardViewStateManager::SCREEN_ON_HINT_DURATION = 1000;
const Int32 KeyguardViewStateManager::SCREEN_ON_RING_HINT_DELAY = 300;
const Boolean KeyguardViewStateManager::SHOW_INITIAL_PAGE_HINTS = FALSE;

CAR_INTERFACE_IMPL_3(KeyguardViewStateManager, Object, IKeyguardViewStateManager,
        ISlidingChallengeLayoutOnChallengeScrolledListener,
        IChallengeLayoutOnBouncerStateChangedListener)

KeyguardViewStateManager::KeyguardViewStateManager()
    : mLastScrollState(ISlidingChallengeLayout::SCROLL_STATE_IDLE)
    , mChallengeTop(0)
    , mPageListeningToSlider(-1)
    , mCurrentPage(-1)
    , mPageIndexOnPageBeginMoving(-1)
{
    CHandler::New(Looper::GetMyLooper(), (IHandler**)&mMainQueue);

    mTmpPoint = ArrayOf<Int32>::Alloc(2);
    mTmpLoc = ArrayOf<Int32>::Alloc(2);

    mPauseListener = new MyAnimatorListenerAdapter(this);

    AutoPtr<IAnimatorListener> mResumeListener = new MyAnimatorListenerAdapter2(this);

    mHideHintsRunnable = new MyRunnable(this);
}

ECode KeyguardViewStateManager::constructor(
    /* [in] */ IKeyguardHostView* hostView)
{
    mKeyguardHostView = hostView;
    return NOERROR;
}

ECode KeyguardViewStateManager::SetPagedView(
    /* [in] */ IKeyguardWidgetPager* pagedView)
{
    mKeyguardWidgetPager = pagedView;
    UpdateEdgeSwiping();
    return NOERROR;
}

ECode KeyguardViewStateManager::SetChallengeLayout(
    /* [in] */ IChallengeLayout* layout)
{
    mChallengeLayout = layout;
    UpdateEdgeSwiping();
    return NOERROR;
}

void KeyguardViewStateManager::UpdateEdgeSwiping()
{
    if (mChallengeLayout != NULL && mKeyguardWidgetPager != NULL) {
        Boolean res;
        if (mChallengeLayout->IsChallengeOverlapping(&res), res) {
            IPagedView::Probe(mKeyguardWidgetPager)->SetOnlyAllowEdgeSwipes(TRUE);
        }
        else {
            IPagedView::Probe(mKeyguardWidgetPager)->SetOnlyAllowEdgeSwipes(FALSE);
        }
    }
}

ECode KeyguardViewStateManager::IsChallengeShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mChallengeLayout != NULL) {
        return mChallengeLayout->IsChallengeShowing(result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode KeyguardViewStateManager::IsChallengeOverlapping(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mChallengeLayout != NULL) {
        return mChallengeLayout->IsChallengeOverlapping(result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode KeyguardViewStateManager::SetSecurityViewContainer(
    /* [in] */ IKeyguardSecurityView* container)
{
    mKeyguardSecurityContainer = container;
    return NOERROR;
}

ECode KeyguardViewStateManager::ShowBouncer(
    /* [in] */  Boolean show)
{
    return mChallengeLayout->ShowBouncer();
}

ECode KeyguardViewStateManager::IsBouncing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return mChallengeLayout->IsBouncing(result);
}

ECode KeyguardViewStateManager::FadeOutSecurity(
    /* [in] */ Int32 duration)
{
    AutoPtr<IViewPropertyAnimator> res;
    IView::Probe(mKeyguardSecurityContainer)->Animate((IViewPropertyAnimator**)&res);
    res->Alpha(0.0f);
    res->SetDuration(duration);
    return res->SetListener(mPauseListener);
}

ECode KeyguardViewStateManager::FadeInSecurity(
    /* [in] */ Int32 duration)
{
    AutoPtr<IViewPropertyAnimator> res;
    IView::Probe(mKeyguardSecurityContainer)->Animate((IViewPropertyAnimator**)&res);
    res->Alpha(1.0f);
    res->SetDuration(duration);
    return res->SetListener(mResumeListener);
}

ECode KeyguardViewStateManager::OnPageBeginMoving()
{
    Boolean res;
    if (mChallengeLayout->IsChallengeOverlapping(&res) &&
            ISlidingChallengeLayout::Probe(mChallengeLayout) != NULL) {
        AutoPtr<ISlidingChallengeLayout> scl = ISlidingChallengeLayout::Probe(mChallengeLayout);
        scl->FadeOutChallenge();
        IPagedView::Probe(mKeyguardWidgetPager)->GetCurrentPage(
                &mPageIndexOnPageBeginMoving);
    }
    // We use mAppWidgetToShow to show a particular widget after you add it--
    // once the user swipes a page we clear that behavior
    if (mKeyguardHostView != NULL) {
        mKeyguardHostView->ClearAppWidgetToShow();
        IKeyguardViewBase::Probe(mKeyguardHostView)->SetOnDismissAction(NULL);
    }
    if (mHideHintsRunnable != NULL) {
        mMainQueue->RemoveCallbacks(mHideHintsRunnable);
        mHideHintsRunnable = NULL;
    }
    return NOERROR;
}

ECode KeyguardViewStateManager::OnPageEndMoving()
{
    mPageIndexOnPageBeginMoving = -1;
    return NOERROR;
}

ECode KeyguardViewStateManager::OnPageSwitching(
    /* [in] */ IView* newPage,
    /* [in] */ Int32 newPageIndex)
{
    if (mKeyguardWidgetPager != NULL &&
            ISlidingChallengeLayout::Probe(mChallengeLayout) != NULL) {
        Boolean isCameraPage = ICameraWidgetFrame::Probe(newPage) != NULL;
        if (isCameraPage) {
            AutoPtr<ICameraWidgetFrame> camera = ICameraWidgetFrame::Probe(newPage);
            Boolean res;
            IPagedView::Probe(mKeyguardWidgetPager)->IsWarping(&res);
            camera->SetUseFastTransition(res);
        }
        AutoPtr<ISlidingChallengeLayout> scl = ISlidingChallengeLayout::Probe(mChallengeLayout);
        scl->SetChallengeInteractive(!isCameraPage);
        Int32 currentFlags;
        IView::Probe(mKeyguardWidgetPager)->GetSystemUiVisibility(&currentFlags);
        Int32 newFlags = isCameraPage ? (currentFlags | IView::STATUS_BAR_DISABLE_SEARCH)
                : (currentFlags & ~IView::STATUS_BAR_DISABLE_SEARCH);
        IView::Probe(mKeyguardWidgetPager)->SetSystemUiVisibility(newFlags);
    }

    // If the page we're settling to is the same as we started on, and the action of
    // moving the page hid the security, we restore it immediately.
    Int32 page;
    IPagedView::Probe(mKeyguardWidgetPager)->GetNextPage(&page);
    if (mPageIndexOnPageBeginMoving == page &&
            ISlidingChallengeLayout::Probe(mChallengeLayout) != NULL) {
        AutoPtr<ISlidingChallengeLayout> scl = ISlidingChallengeLayout::Probe(mChallengeLayout);
        scl->FadeInChallenge();
        mKeyguardWidgetPager->SetWidgetToResetOnPageFadeOut(-1);
    }
    mPageIndexOnPageBeginMoving = -1;
    return NOERROR;
}

ECode KeyguardViewStateManager::OnPageSwitched(
    /* [in] */ IView* newPage,
    /* [in] */ Int32 newPageIndex)
{
    // Reset the previous page size and ensure the current page is sized appropriately.
    // We only modify the page state if it is not currently under control by the slider.
    // This prevents conflicts.

    // If the page hasn't switched, don't bother with any of this
    if (mCurrentPage == newPageIndex) return NOERROR;

    if (mKeyguardWidgetPager != NULL && mChallengeLayout != NULL) {
        AutoPtr<IKeyguardWidgetFrame> prevPage;
        mKeyguardWidgetPager->GetWidgetPageAt(mCurrentPage, (IKeyguardWidgetFrame**)&prevPage);
        Int32 page;
        if (prevPage != NULL && mCurrentPage != mPageListeningToSlider && mCurrentPage
                != (mKeyguardWidgetPager->GetWidgetToResetOnPageFadeOut(&page), page)) {
            prevPage->ResetSize();
        }

        AutoPtr<IKeyguardWidgetFrame> newCurPage;
        mKeyguardWidgetPager->GetWidgetPageAt(newPageIndex, (IKeyguardWidgetFrame**)&newCurPage);
        Boolean challengeOverlapping;
        mChallengeLayout->IsChallengeOverlapping(&challengeOverlapping);
        Boolean res;
        if (challengeOverlapping && (newCurPage->IsSmall(&res), !res)
                && mPageListeningToSlider != newPageIndex) {
            newCurPage->ShrinkWidget(TRUE);
        }
    }

    mCurrentPage = newPageIndex;
    return NOERROR;
}

ECode KeyguardViewStateManager::OnPageBeginWarp()
{
    FadeOutSecurity(ISlidingChallengeLayout::CHALLENGE_FADE_OUT_DURATION);
    Int32 index;
    IPagedView::Probe(mKeyguardWidgetPager)->GetPageWarpIndex(&index);
    AutoPtr<IView> frame;
    IPagedView::Probe(mKeyguardWidgetPager)->GetPageAt(index, (IView**)&frame);
    return IKeyguardWidgetFrame::Probe(frame)->ShowFrame(TO_IINTERFACE(this));
}

ECode KeyguardViewStateManager::OnPageEndWarp()
{
    FadeInSecurity(ISlidingChallengeLayout::CHALLENGE_FADE_IN_DURATION);
    Int32 index;
    IPagedView::Probe(mKeyguardWidgetPager)->GetPageWarpIndex(&index);
    AutoPtr<IView> frame;
    IPagedView::Probe(mKeyguardWidgetPager)->GetPageAt(index, (IView**)&frame);
    return IKeyguardWidgetFrame::Probe(frame)->HideFrame(TO_IINTERFACE(this));
}

Int32 KeyguardViewStateManager::GetChallengeTopRelativeToFrame(
    /* [in] */ IKeyguardWidgetFrame* frame,
    /* [in] */ Int32 top)
{
    (*mTmpPoint)[0] = 0;
    (*mTmpPoint)[1] = top;
    MapPoint(IView::Probe(mChallengeLayout), IView::Probe(frame), mTmpPoint);
    return (*mTmpPoint)[1];
}

void KeyguardViewStateManager::MapPoint(
    /* [in] */ IView* fromView,
    /* [in] */ IView* toView,
    /* [in] */ ArrayOf<Int32>* pt)
{
    fromView->GetLocationInWindow(mTmpLoc);

    Int32 x = (*mTmpLoc)[0];
    Int32 y = (*mTmpLoc)[1];

    toView->GetLocationInWindow(mTmpLoc);
    Int32 vX = (*mTmpLoc)[0];
    Int32 vY = (*mTmpLoc)[1];

    (*pt)[0] += x - vX;
    (*pt)[1] += y - vY;
}

void KeyguardViewStateManager::UserActivity()
{
    if (mKeyguardHostView != NULL) {
        mKeyguardHostView->OnUserActivityTimeoutChanged();
        IKeyguardSecurityContainerSecurityCallback::Probe(mKeyguardHostView)->UserActivity();
    }
}

ECode KeyguardViewStateManager::OnScrollStateChanged(
    /* [in] */ Int32 scrollState)
{
    if (mKeyguardWidgetPager == NULL || mChallengeLayout == NULL) return NOERROR;

    Boolean challengeOverlapping;
    mChallengeLayout->IsChallengeOverlapping(&challengeOverlapping);

    if (scrollState == ISlidingChallengeLayout::SCROLL_STATE_IDLE) {
        AutoPtr<IKeyguardWidgetFrame> frame;
        mKeyguardWidgetPager->GetWidgetPageAt(mPageListeningToSlider, (IKeyguardWidgetFrame**)&frame);
        if (frame == NULL) return NOERROR;

        if (!challengeOverlapping) {
            Boolean res;
            if (IPagedView::Probe(mKeyguardWidgetPager)->IsPageMoving(&res), !res) {
                frame->ResetSize();
                UserActivity();
            }
            else {
                mKeyguardWidgetPager->SetWidgetToResetOnPageFadeOut(mPageListeningToSlider);
            }
        }
        Boolean res;
        if (frame->IsSmall(&res), res) {
            // This is to make sure that if the scroller animation gets cut off midway
            // that the frame doesn't stay in a partial down position.
            Int32 height;
            frame->GetSmallFrameHeight(&height);
            frame->SetFrameHeight(height);
        }
        if (scrollState != ISlidingChallengeLayout::SCROLL_STATE_FADING) {
            frame->HideFrame(TO_IINTERFACE(this));
        }
        UpdateEdgeSwiping();

        if (mChallengeLayout->IsChallengeShowing(&res), res) {
            mKeyguardSecurityContainer->OnResume(
                    IKeyguardSecurityView::VIEW_REVEALED);
        }
        else {
            mKeyguardSecurityContainer->OnPause();
        }
        mPageListeningToSlider = -1;
    }
    else if (mLastScrollState == ISlidingChallengeLayout::SCROLL_STATE_IDLE) {
        // Whether dragging or settling, if the last state was idle, we use this signal
        // to update the current page who will receive events from the sliding challenge.
        // We resize the frame as appropriate.
        IPagedView::Probe(mKeyguardWidgetPager)->GetNextPage(&mPageListeningToSlider);
        AutoPtr<IKeyguardWidgetFrame> frame;
        mKeyguardWidgetPager->GetWidgetPageAt(mPageListeningToSlider, (IKeyguardWidgetFrame**)&frame);
        if (frame == NULL) return NOERROR;

        // Skip showing the frame and shrinking the widget if we are
        Boolean res;
        if (mChallengeLayout->IsBouncing(&res), !res) {
            if (scrollState != ISlidingChallengeLayout::SCROLL_STATE_FADING) {
                frame->ShowFrame(TO_IINTERFACE(this));
            }

            // As soon as the security begins sliding, the widget becomes small (if it wasn't
            // small to begin with).
            if (frame->IsSmall(&res), !res) {
                // We need to fetch the final page, in case the pages are in motion.
                IPagedView::Probe(mKeyguardWidgetPager)->GetNextPage(&mPageListeningToSlider);
                frame->ShrinkWidget(FALSE);
            }
        }
        else {
            if (frame->IsSmall(&res), !res) {
                // We need to fetch the final page, in case the pages are in motion.
                IPagedView::Probe(mKeyguardWidgetPager)->GetNextPage(&mPageListeningToSlider);
            }
        }

        // View is on the move.  Pause the security view until it completes.
        mKeyguardSecurityContainer->OnPause();
    }
    mLastScrollState = scrollState;
    return NOERROR;
}

ECode KeyguardViewStateManager::OnScrollPositionChanged(
    /* [in] */ Float scrollPosition,
    /* [in] */ Int32 challengeTop)
{
    mChallengeTop = challengeTop;
    AutoPtr<IKeyguardWidgetFrame> frame;
    mKeyguardWidgetPager->GetWidgetPageAt(mPageListeningToSlider, (IKeyguardWidgetFrame**)&frame);
    if (frame != NULL && mLastScrollState != ISlidingChallengeLayout::SCROLL_STATE_FADING) {
        frame->AdjustFrame(GetChallengeTopRelativeToFrame(frame, mChallengeTop));
    }
    return NOERROR;
}

ECode KeyguardViewStateManager::ShowUsabilityHints()
{
    AutoPtr<IRunnable> r = new MyRunnable2(this);
    Boolean res;
    mMainQueue->PostDelayed(r , SCREEN_ON_RING_HINT_DELAY, &res);

    if (SHOW_INITIAL_PAGE_HINTS) {
        mKeyguardWidgetPager->ShowInitialPageHints();
    }
    if (mHideHintsRunnable != NULL) {
        mMainQueue->PostDelayed(mHideHintsRunnable,
                SCREEN_ON_HINT_DURATION, &res);
    }
    return NOERROR;
}

ECode KeyguardViewStateManager::OnBouncerStateChanged(
    /* [in] */ Boolean bouncerActive)
{
    if (bouncerActive) {
        mKeyguardWidgetPager->ZoomOutToBouncer();
    }
    else {
        mKeyguardWidgetPager->ZoomInFromBouncer();
        if (mKeyguardHostView != NULL) {
            IKeyguardViewBase::Probe(mKeyguardHostView)->SetOnDismissAction(NULL);
        }
    }
    return NOERROR;
}


} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
