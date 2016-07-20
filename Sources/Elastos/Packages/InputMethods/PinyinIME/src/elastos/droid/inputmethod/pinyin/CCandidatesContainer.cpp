
#include <elastos/droid/ext/frameworkdef.h>
#include "elastos/droid/inputmethod/pinyin/CCandidatesContainer.h"
#include "elastos/droid/inputmethod/pinyin/CCandidateView.h"
#include "elastos/droid/inputmethod/pinyin/Environment.h"
#include "R.h"

using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::Animation::EIID_IAnimationAnimationListener;
using Elastos::Droid::View::Animation::IAnimationSet;
using Elastos::Droid::View::Animation::CAnimationSet;
using Elastos::Droid::View::Animation::CTranslateAnimation;
using Elastos::Droid::View::Animation::CAlphaAnimation;
using Elastos::Droid::View::Animation::ITranslateAnimation;
using Elastos::Droid::View::Animation::IAlphaAnimation;
using Elastos::Droid::Widget::IViewAnimator;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

Int32 CCandidatesContainer::ARROW_ALPHA_ENABLED = 0xff;
Int32 CCandidatesContainer::ARROW_ALPHA_DISABLED = 0x40;
Int32 CCandidatesContainer::ANIMATION_TIME = 200;

CAR_OBJECT_IMPL(CCandidatesContainer);

CAR_INTERFACE_IMPL_4(CCandidatesContainer, RelativeLayout, ICandidatesContainer, IViewOnTouchListener, IAnimationAnimationListener, IArrowUpdater);

CCandidatesContainer::CCandidatesContainer()
    : mXOffsetForFlipper(0)
    , mCurrentPage(-1)
{}

ECode CCandidatesContainer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return RelativeLayout::constructor(context, attrs);
}

ECode CCandidatesContainer::Initialize(
    /* [in] */ ICandidateViewListener* cvListener,
    /* [in] */ CBalloonHint* balloonHint,
    /* [in] */ IGestureDetector* gestureDetector)
{
    mCvListener = cvListener;

    AutoPtr<IView> view;
    FindViewById(R::id::arrow_left_btn, (IView**)&view);
    mLeftArrowBtn = IImageButton::Probe(view);
    view = NULL;
    FindViewById(R::id::arrow_right_btn, (IView**)&view);
    mRightArrowBtn = IImageButton::Probe(view);
    IView::Probe(mLeftArrowBtn)->SetOnTouchListener(this);
    IView::Probe(mRightArrowBtn)->SetOnTouchListener(this);

    view = NULL;
    FindViewById(R::id::candidate_flipper, (IView**)&view);
    mFlipper = IViewFlipper::Probe(view);
    IFrameLayout::Probe(mFlipper)->SetMeasureAllChildren(TRUE);

    Invalidate();
    RequestLayout();

    Int32 count = 0;
    IViewGroup::Probe(mFlipper)->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> view;
        IViewGroup::Probe(mFlipper)->GetChildAt(i, (IView**)&view);
        CCandidateView* cv = (CCandidateView*)ICandidateView::Probe(view);
        cv->Initialize(this, balloonHint, gestureDetector, mCvListener);
    }
    return NOERROR;
}

ECode CCandidatesContainer::ShowCandidates(
    /* [in] */ CPinyinIME::DecodingInfo* decInfo,
    /* [in] */ Boolean enableActiveHighlight)
{
    if (NULL == decInfo) return NOERROR;
    mDecInfo = decInfo;
    mCurrentPage = 0;

    if (decInfo->IsCandidatesListEmpty()) {
        ShowArrow(mLeftArrowBtn, FALSE);
        ShowArrow(mRightArrowBtn, FALSE);
    }
    else {
        ShowArrow(mLeftArrowBtn, TRUE);
        ShowArrow(mRightArrowBtn, TRUE);
    }

    Int32 count = 0;
    IViewGroup::Probe(mFlipper)->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> view;
        IViewGroup::Probe(mFlipper)->GetChildAt(i, (IView**)&view);
        CCandidateView* cv = (CCandidateView*)ICandidateView::Probe(view);
        cv->SetDecodingInfo(mDecInfo);
    }
    StopAnimation();

    AutoPtr<IView> cv;
    IViewAnimator::Probe(mFlipper)->GetCurrentView((IView**)&cv);
    ((CCandidateView*)ICandidateView::Probe(cv))->ShowPage(mCurrentPage, 0, enableActiveHighlight);

    UpdateArrowStatus();
    return Invalidate();
}

Int32 CCandidatesContainer::GetCurrentPage()
{
    return mCurrentPage;
}

ECode CCandidatesContainer::EnableActiveHighlight(
    /* [in] */ Boolean enableActiveHighlight)
{
    AutoPtr<IView> cv;
    IViewAnimator::Probe(mFlipper)->GetCurrentView((IView**)&cv);
    ((CCandidateView*)ICandidateView::Probe(cv))->EnableActiveHighlight(enableActiveHighlight);
    return Invalidate();
}

ECode CCandidatesContainer::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 measuredWidth = Environment::GetInstance()->GetScreenWidth();
    Int32 measuredHeight;
    GetPaddingTop(&measuredHeight);
    measuredHeight += Environment::GetInstance()->GetHeightForCandidates();
    widthMeasureSpec = MeasureSpec::MakeMeasureSpec(measuredWidth,
            MeasureSpec::EXACTLY);
    heightMeasureSpec = MeasureSpec::MakeMeasureSpec(measuredHeight,
            MeasureSpec::EXACTLY);
    RelativeLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    if (NULL != mLeftArrowBtn) {
        IView::Probe(mLeftArrowBtn)->GetMeasuredWidth(&mXOffsetForFlipper);
    }
    return NOERROR;
}

Boolean CCandidatesContainer::ActiveCurseBackward()
{
    Boolean tmp = FALSE;
    if ((mFlipper->IsFlipping(&tmp), tmp) || NULL == mDecInfo) {
        return FALSE;
    }

    AutoPtr<IView> view;
    IViewAnimator::Probe(mFlipper)->GetCurrentView((IView**)&view);
    CCandidateView* cv = (CCandidateView*)ICandidateView::Probe(view);

    if (cv->ActiveCurseBackward()) {
        cv->Invalidate();
        return TRUE;
    }
    return PageBackward(TRUE, TRUE);
}

Boolean CCandidatesContainer::ActiveCurseForward()
{
    Boolean state = FALSE;
    if ((mFlipper->IsFlipping(&state), state) || NULL == mDecInfo) {
        return FALSE;
    }

    AutoPtr<IView> view;
    IViewAnimator::Probe(mFlipper)->GetCurrentView((IView**)&view);
    CCandidateView* cv = (CCandidateView*)ICandidateView::Probe(view);

    if (cv->ActiveCursorForward()) {
        cv->Invalidate();
        return TRUE;
    }
    return PageForward(TRUE, TRUE);
}

Boolean CCandidatesContainer::PageBackward(
    /* [in] */ Boolean animLeftRight,
    /* [in] */ Boolean enableActiveHighlight)
{
    if (NULL == mDecInfo) {
        return FALSE;
    }

    Boolean state = FALSE;
    if ((mFlipper->IsFlipping(&state), state) || 0 == mCurrentPage) {
        return FALSE;
    }

    Int32 child = 0;
    IViewAnimator::Probe(mFlipper)->GetDisplayedChild(&child);
    Int32 childNext = (child + 1) % 2;

    AutoPtr<IView> view;
    IViewGroup::Probe(mFlipper)->GetChildAt(child, (IView**)&view);
    CCandidateView* cv = (CCandidateView*)ICandidateView::Probe(view);
    view = NULL;
    IViewGroup::Probe(mFlipper)->GetChildAt(childNext, (IView**)&view);
    CCandidateView* cvNext = (CCandidateView*)ICandidateView::Probe(view);

    mCurrentPage--;
    Int32 activeCandInPage = cv->GetActiveCandiatePosInPage();
    if (animLeftRight) {
        activeCandInPage = mDecInfo->mPageStart[mCurrentPage + 1]
                - mDecInfo->mPageStart[mCurrentPage] - 1;
    }

    cvNext->ShowPage(mCurrentPage, activeCandInPage, enableActiveHighlight);
    LoadAnimation(animLeftRight, FALSE);
    StartAnimation();

    UpdateArrowStatus();
    return TRUE;
}

Boolean CCandidatesContainer::PageForward(
    /* [in] */ Boolean animLeftRight,
    /* [in] */ Boolean enableActiveHighlight)
{
    if (NULL == mDecInfo) {
        return FALSE;
    }

    Boolean state = FALSE;
    if ((mFlipper->IsFlipping(&state), state) || !mDecInfo->PreparePage(mCurrentPage + 1)) {
        return FALSE;
    }

    Int32 child = 0;
    IViewAnimator::Probe(mFlipper)->GetDisplayedChild(&child);
    Int32 childNext = (child + 1) % 2;
    AutoPtr<IView> view;
    IViewGroup::Probe(mFlipper)->GetChildAt(child, (IView**)&view);
    CCandidateView* cv = (CCandidateView*)ICandidateView::Probe(view);
    Int32 activeCandInPage = cv->GetActiveCandiatePosInPage();
    cv->EnableActiveHighlight(enableActiveHighlight);

    view = NULL;
    IViewGroup::Probe(mFlipper)->GetChildAt(childNext, (IView**)&view);
    CCandidateView* cvNext = (CCandidateView*)ICandidateView::Probe(view);
    mCurrentPage++;
    if (animLeftRight) activeCandInPage = 0;

    cvNext->ShowPage(mCurrentPage, activeCandInPage, enableActiveHighlight);
    LoadAnimation(animLeftRight, TRUE);
    StartAnimation();

    UpdateArrowStatus();
    return TRUE;
}

Int32 CCandidatesContainer::GetActiveCandiatePos()
{
    if (NULL == mDecInfo) {
        return -1;
    }

    AutoPtr<IView> view;
    IViewAnimator::Probe(mFlipper)->GetCurrentView((IView**)&view);
    return ((CCandidateView*)ICandidateView::Probe(view))->GetActiveCandiatePosGlobal();
}

ECode CCandidatesContainer::UpdateArrowStatus()
{
    if (mCurrentPage < 0) return NOERROR;
    Boolean forwardEnabled = mDecInfo->PageForwardable(mCurrentPage);
    Boolean backwardEnabled = mDecInfo->PageBackwardable(mCurrentPage);

    if (backwardEnabled) {
        EnableArrow(mLeftArrowBtn, TRUE);
    }
    else {
        EnableArrow(mLeftArrowBtn, FALSE);
    }
    if (forwardEnabled) {
        EnableArrow(mRightArrowBtn, TRUE);
    }
    else {
        EnableArrow(mRightArrowBtn, FALSE);
    }
    return NOERROR;
}

void CCandidatesContainer::EnableArrow(
    /* [in] */ IImageButton* arrowBtn,
    /* [in] */ Boolean enabled)
{
    IView::Probe(arrowBtn)->SetEnabled(enabled);
    if (enabled) {
        IView::Probe(arrowBtn)->SetAlpha(ARROW_ALPHA_ENABLED);
    }
    else {
        IView::Probe(arrowBtn)->SetAlpha(ARROW_ALPHA_DISABLED);
    }
}

void CCandidatesContainer::ShowArrow(
    /* [in] */ IImageButton* arrowBtn,
    /* [in] */ Boolean show)
{
    if (show) {
        IView::Probe(arrowBtn)->SetVisibility(IView::VISIBLE);
    }
    else {
        IView::Probe(arrowBtn)->SetVisibility(IView::INVISIBLE);
    }
}

ECode CCandidatesContainer::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 action = 0;
    event->GetAction(&action);
    if (action == IMotionEvent::ACTION_DOWN) {
        if (v == IView::Probe(mLeftArrowBtn)) {
            mCvListener->OnToRightGesture();
        }
        else if (v == IView::Probe(mRightArrowBtn)) {
            mCvListener->OnToLeftGesture();
        }
    }
    else if (action == IMotionEvent::ACTION_UP) {
        AutoPtr<IView> cv;
        IViewAnimator::Probe(mFlipper)->GetCurrentView((IView**)&cv);
        ((CCandidateView*)ICandidateView::Probe(cv))->EnableActiveHighlight(TRUE);
    }

    *result = FALSE;
    return NOERROR;
}


ECode CCandidatesContainer::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    event->OffsetLocation(-mXOffsetForFlipper, 0);
    AutoPtr<IView> view;
    IViewAnimator::Probe(mFlipper)->GetCurrentView((IView**)&view);
    return ((CCandidateView*)ICandidateView::Probe(view))->OnTouchEventReal(event, result);
}

ECode CCandidatesContainer::LoadAnimation(
    /* [in] */ Boolean animLeftRight,
    /* [in] */ Boolean forward)
{
    if (animLeftRight) {
        if (forward) {
            if (NULL == mInAnimPushLeft) {
                mInAnimPushLeft = CreateAnimation(1.0f, 0, 0, 0, 0, 1.0f,
                        ANIMATION_TIME);
                mOutAnimPushLeft = CreateAnimation(0, -1.0f, 0, 0, 1.0f, 0,
                        ANIMATION_TIME);
            }
            mInAnimInUse = mInAnimPushLeft;
            mOutAnimInUse = mOutAnimPushLeft;
        }
        else {
            if (NULL == mInAnimPushRight) {
                mInAnimPushRight = CreateAnimation(-1.0f, 0, 0, 0, 0, 1.0f,
                        ANIMATION_TIME);
                mOutAnimPushRight = CreateAnimation(0, 1.0f, 0, 0, 1.0f, 0,
                        ANIMATION_TIME);
            }
            mInAnimInUse = mInAnimPushRight;
            mOutAnimInUse = mOutAnimPushRight;
        }
    }
    else {
        if (forward) {
            if (NULL == mInAnimPushUp) {
                mInAnimPushUp = CreateAnimation(0, 0, 1.0f, 0, 0, 1.0f,
                        ANIMATION_TIME);
                mOutAnimPushUp = CreateAnimation(0, 0, 0, -1.0f, 1.0f, 0,
                        ANIMATION_TIME);
            }
            mInAnimInUse = mInAnimPushUp;
            mOutAnimInUse = mOutAnimPushUp;
        }
        else {
            if (NULL == mInAnimPushDown) {
                mInAnimPushDown = CreateAnimation(0, 0, -1.0f, 0, 0, 1.0f,
                        ANIMATION_TIME);
                mOutAnimPushDown = CreateAnimation(0, 0, 0, 1.0f, 1.0f, 0,
                        ANIMATION_TIME);
            }
            mInAnimInUse = mInAnimPushDown;
            mOutAnimInUse = mOutAnimPushDown;
        }
    }

    mInAnimInUse->SetAnimationListener(this);

    IViewAnimator::Probe(mFlipper)->SetInAnimation(mInAnimInUse);
    IViewAnimator::Probe(mFlipper)->SetOutAnimation(mOutAnimInUse);
    return NOERROR;
}

AutoPtr<IAnimation> CCandidatesContainer::CreateAnimation(
    /* [in] */ Float xFrom,
    /* [in] */ Float xTo,
    /* [in] */ Float yFrom,
    /* [in] */ Float yTo,
    /* [in] */ Float alphaFrom,
    /* [in] */ Float alphaTo,
    /* [in] */ Int64 duration)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IAnimationSet> animSet;
    CAnimationSet::New(context, NULL, (IAnimationSet**)&animSet);
    AutoPtr<IAnimation> trans;
    CTranslateAnimation::New(IAnimation::RELATIVE_TO_SELF,
            xFrom, IAnimation::RELATIVE_TO_SELF, xTo,
            IAnimation::RELATIVE_TO_SELF, yFrom, IAnimation::RELATIVE_TO_SELF,
            yTo, (IAnimation**)&trans);

    AutoPtr<IAnimation> alpha;
    CAlphaAnimation::New(alphaFrom, alphaTo, (IAnimation**)&alpha);
    animSet->AddAnimation(trans);
    animSet->AddAnimation(alpha);
    IAnimation::Probe(animSet)->SetDuration(duration);
    return IAnimation::Probe(animSet);
}

void CCandidatesContainer::StartAnimation()
{
    IViewAnimator::Probe(mFlipper)->ShowNext();
}

void CCandidatesContainer::StopAnimation()
{
    mFlipper->StopFlipping();
}

ECode CCandidatesContainer::OnAnimationEnd(
    /* [in] */  IAnimation* animation)
{
    Boolean pressed = FALSE;
    if ((IView::Probe(mLeftArrowBtn)->IsPressed(&pressed), !pressed) &&
        (IView::Probe(mRightArrowBtn)->IsPressed(&pressed), !pressed)) {
        AutoPtr<IView> cv;
        IViewAnimator::Probe(mFlipper)->GetCurrentView((IView**)&cv);
        ((CCandidateView*)ICandidateView::Probe(cv))->EnableActiveHighlight(TRUE);
    }
    return NOERROR;
}

ECode CCandidatesContainer::OnAnimationRepeat(
    /* [in] */  IAnimation* animation)
{
    return NOERROR;
}

ECode CCandidatesContainer::OnAnimationStart(
    /* [in] */  IAnimation* animation)
{
    return NOERROR;
}

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos
