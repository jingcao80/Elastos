
#include "CCandidatesContainer.h"
#include "CDecodingInfo.h"
#include "CPinyinEnvironmentHelper.h"
#include "elastos/droid/view/animation/CAnimationSet.h"
#include "elastos/droid/view/animation/CTranslateAnimation.h"
#include "elastos/droid/view/animation/CAlphaAnimation.h"

using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::Animation::EIID_IAnimationListener;
using Elastos::Droid::View::Animation::IAnimationSet;
using Elastos::Droid::View::Animation::CAnimationSet;
using Elastos::Droid::View::Animation::CTranslateAnimation;
using Elastos::Droid::View::Animation::CAlphaAnimation;
using Elastos::Droid::View::Animation::ITranslateAnimation;
using Elastos::Droid::View::Animation::IAlphaAnimation;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

Int32 CCandidatesContainer::ARROW_ALPHA_ENABLED = 0xff;
Int32 CCandidatesContainer::ARROW_ALPHA_DISABLED = 0x40;
Int32 CCandidatesContainer::ANIMATION_TIME = 200;

CAR_OBJECT_IMPL(CCandidatesContainer);

CAR_INTERFACE_IMPL(CCandidatesContainer, RelativeLayout, ICandidatesContainer);

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
    /* [in] */ IBalloonHint* balloonHint,
    /* [in] */ IGestureDetector* gestureDetector)
{
    mCvListener = cvListener;

    mLeftArrowBtn = IImageButton::Probe(FindViewById(R::id::arrow_left_btn));
    mRightArrowBtn = IImageButton::Probe(FindViewById(R::id::arrow_right_btn));
    mLeftArrowBtn->SetOnTouchListener(this);
    mRightArrowBtn->SetOnTouchListener(this);

    mFlipper = IViewFlipper::Probe(FindViewById(R::id::candidate_flipper));
    mFlipper->SetMeasureAllChildren(TRUE);

    Invalidate();
    RequestLayout();

    Int32 count = 0;
    mFlipper->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> view;
        mFlipper->GetChildAt(i, (IView**)&view);
        ICandidateView* cv = ICandidateView::Probe(view);
        cv->Initialize(this, balloonHint, gestureDetector, mCvListener);
    }
    return NOERROR;
}

ECode CCandidatesContainer::ShowCandidates(
    /* [in] */ IDecodingInfo* decInfo,
    /* [in] */ Boolean enableActiveHighlight)
{
    if (NULL == decInfo) return NOERROR;
    mDecInfo = decInfo;
    mCurrentPage = 0;

    Boolean empty = FALSE;
    if (decInfo->IsCandidatesListEmpty(&empty), empty) {
        ShowArrow(mLeftArrowBtn, FALSE);
        ShowArrow(mRightArrowBtn, FALSE);
    }
    else {
        ShowArrow(mLeftArrowBtn, TRUE);
        ShowArrow(mRightArrowBtn, TRUE);
    }

    Int32 count = 0;
    mFlipper->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> view;
        mFlipper->GetChildAt(i, (IView**)&view);
        ICandidateView* cv = ICandidateView::Probe(view);
        cv->SetDecodingInfo(mDecInfo);
    }
    StopAnimation();

    AutoPtr<IView> cv;
    mFlipper->GetCurrentView((IView**)&cv);
    ICandidateView::Probe(cv)->ShowPage(mCurrentPage, 0, enableActiveHighlight);

    UpdateArrowStatus();
    return Invalidate();
}

ECode CCandidatesContainer::GetCurrentPage(
    /* [out] */ Int32* page)
{
    VALIDATE_NOT_NULL(page);
    *page = mCurrentPage;
    return NOERROR;
}

ECode CCandidatesContainer::EnableActiveHighlight(
    /* [in] */ Boolean enableActiveHighlight)
{
    AutoPtr<IView> cv;
    mFlipper->GetCurrentView((IView**)&cv);
    ICandidateView::Probe(cv)->EnableActiveHighlight(enableActiveHighlight);
    return Invalidate();
}

void CCandidatesContainer::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    AutoPtr<IEnvironment> env;
    Environment::GetInstance((IEnvironment**)&env);
    Int32 measuredWidth = 0, value = 0;
    env->GetScreenWidth(&measuredWidth);
    Int32 measuredHeight = GetPaddingTop();
    measuredHeight += (env->GetHeightForCandidates(&value), value);
    widthMeasureSpec = MeasureSpec::MakeMeasureSpec(measuredWidth,
            MeasureSpec::EXACTLY);
    heightMeasureSpec = MeasureSpec::MakeMeasureSpec(measuredHeight,
            MeasureSpec::EXACTLY);
    RelativeLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    if (NULL != mLeftArrowBtn) {
        mLeftArrowBtn->GetMeasuredWidth(&mXOffsetForFlipper);
    }
}

ECode CCandidatesContainer::ActiveCurseBackward(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    if ((mFlipper->IsFlipping(&tmp), tmp) || NULL == mDecInfo) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IView> view;
    mFlipper->GetCurrentView((IView**)&view);
    ICandidateView* cv = ICandidateView::Probe(view);
    assert(cv != NULL);

    Boolean state = FALSE;
    if (cv->ActiveCurseBackward(&state), state) {
        cv->Invalidate();
        *result = TRUE;
        return NOERROR;
    }
    return PageBackward(TRUE, TRUE, result);
}

ECode CCandidatesContainer::ActiveCurseForward(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean state = FALSE;
    if ((mFlipper->IsFlipping(&state), state) || NULL == mDecInfo) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IView> view;
    mFlipper->GetCurrentView((IView**)&view);
    ICandidateView* cv = ICandidateView::Probe(view);

    if (cv->ActiveCursorForward(&state), state) {
        cv->Invalidate();
        *result = TRUE;
        return NOERROR;
    }
    return PageForward(TRUE, TRUE, result);
}

ECode CCandidatesContainer::PageBackward(
    /* [in] */ Boolean animLeftRight,
    /* [in] */ Boolean enableActiveHighlight,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (NULL == mDecInfo) {
        *result = FALSE;
        return NOERROR;
    }

    Boolean state = FALSE;
    if ((mFlipper->IsFlipping(&state), state) || 0 == mCurrentPage) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 child = 0;
    mFlipper->GetDisplayedChild(&child);
    Int32 childNext = (child + 1) % 2;

    AutoPtr<IView> view;
    mFlipper->GetChildAt(child, (IView**)&view);
    AutoPtr<ICandidateView> cv = ICandidateView::Probe(view);
    view = NULL;
    mFlipper->GetChildAt(childNext, (IView**)&view);
    AutoPtr<ICandidateView> cvNext = ICandidateView::Probe(view);

    mCurrentPage--;
    Int32 activeCandInPage = 0;
    cv->GetActiveCandiatePosInPage(&activeCandInPage);
    if (animLeftRight) {
        activeCandInPage = ((CDecodingInfo*)mDecInfo.Get())->mPageStart[mCurrentPage + 1]
                - ((CDecodingInfo*)mDecInfo.Get())->mPageStart[mCurrentPage] - 1;
    }

    cvNext->ShowPage(mCurrentPage, activeCandInPage, enableActiveHighlight);
    LoadAnimation(animLeftRight, FALSE);
    StartAnimation();

    UpdateArrowStatus();
    *result = TRUE;
    return NOERROR;
}

ECode CCandidatesContainer::PageForward(
    /* [in] */ Boolean animLeftRight,
    /* [in] */ Boolean enableActiveHighlight,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (NULL == mDecInfo) {
        *result = FALSE;
        return NOERROR;
    }

    Boolean state = FALSE;
    if ((mFlipper->IsFlipping(&state), state) || !(mDecInfo->PreparePage(mCurrentPage + 1, &state), state)) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 child = 0;
    mFlipper->GetDisplayedChild(&child);
    Int32 childNext = (child + 1) % 2;
    AutoPtr<IView> view;
    mFlipper->GetChildAt(child, (IView**)&view);
    AutoPtr<ICandidateView> cv = ICandidateView::Probe(view);
    Int32 activeCandInPage = 0;
    cv->GetActiveCandiatePosInPage(&activeCandInPage);
    cv->EnableActiveHighlight(enableActiveHighlight);

    view = NULL;
    mFlipper->GetChildAt(childNext, (IView**)&view);
    AutoPtr<ICandidateView> cvNext = ICandidateView::Probe(view);
    mCurrentPage++;
    if (animLeftRight) activeCandInPage = 0;

    cvNext->ShowPage(mCurrentPage, activeCandInPage, enableActiveHighlight);
    LoadAnimation(animLeftRight, TRUE);
    StartAnimation();

    UpdateArrowStatus();
    *result = TRUE;
    return NOERROR;
}

ECode CCandidatesContainer::GetActiveCandiatePos(
    /* [out] */ Int32* pos)
{
    VALIDATE_NOT_NULL(pos);
    if (NULL == mDecInfo) {
        *pos = -1;
        return NOERROR;
    }

    AutoPtr<IView> view;
    mFlipper->GetCurrentView((IView**)&view);
    return ICandidateView::Probe(view)->GetActiveCandiatePosGlobal(pos);
}

ECode CCandidatesContainer::UpdateArrowStatus()
{
    if (mCurrentPage < 0) return NOERROR;
    Boolean forwardEnabled = FALSE;
    mDecInfo->PageForwardable(mCurrentPage, &forwardEnabled);
    Boolean backwardEnabled = FALSE;
    mDecInfo->PageBackwardable(mCurrentPage, &backwardEnabled);

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
    arrowBtn->SetEnabled(enabled);
    if (enabled) {
        arrowBtn->SetAlpha(ARROW_ALPHA_ENABLED);
    }
    else {
        arrowBtn->SetAlpha(ARROW_ALPHA_DISABLED);
    }
}

void CCandidatesContainer::ShowArrow(
    /* [in] */ IImageButton* arrowBtn,
    /* [in] */ Boolean show)
{
    if (show) {
        arrowBtn->SetVisibility(IView::VISIBLE);
    }
    else {
        arrowBtn->SetVisibility(IView::INVISIBLE);
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
        if (v == mLeftArrowBtn) {
            mCvListener->OnToRightGesture();
        }
        else if (v == mRightArrowBtn) {
            mCvListener->OnToLeftGesture();
        }
    }
    else if (action == IMotionEvent::ACTION_UP) {
        AutoPtr<IView> cv;
        mFlipper->GetCurrentView((IView**)&cv);
        ICandidateView::Probe(cv)->EnableActiveHighlight(TRUE);
    }

    *result = FALSE;
    return NOERROR;
}


Boolean CCandidatesContainer::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    event->OffsetLocation(-mXOffsetForFlipper, 0);
    AutoPtr<IView> view;
    mFlipper->GetCurrentView((IView**)&view);
    Boolean result = FALSE;
    ICandidateView::Probe(view)->OnTouchEventReal(event, &result);
    return TRUE;
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

    mFlipper->SetInAnimation(mInAnimInUse);
    mFlipper->SetOutAnimation(mOutAnimInUse);
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
    AutoPtr<IAnimationSet> animSet;
    CAnimationSet::New(GetContext(), NULL, (IAnimationSet**)&animSet);
    AutoPtr<IAnimation> trans;
    CTranslateAnimation::New(IAnimation::RELATIVE_TO_SELF,
            xFrom, IAnimation::RELATIVE_TO_SELF, xTo,
            IAnimation::RELATIVE_TO_SELF, yFrom, IAnimation::RELATIVE_TO_SELF,
            yTo, (IAnimation**)&trans);

    AutoPtr<IAnimation> alpha;
    CAlphaAnimation::New(alphaFrom, alphaTo, (IAnimation**)&alpha);
    animSet->AddAnimation(trans);
    animSet->AddAnimation(alpha);
    animSet->SetDuration(duration);
    return animSet;
}

void CCandidatesContainer::StartAnimation()
{
    mFlipper->ShowNext();
}

void CCandidatesContainer::StopAnimation()
{
    mFlipper->StopFlipping();
}

ECode CCandidatesContainer::OnAnimationEnd(
    /* [in] */  IAnimation* animation)
{
    Boolean pressed = FALSE;
    if ((mLeftArrowBtn->IsPressed(&pressed), !pressed) && (mRightArrowBtn->IsPressed(&pressed), !pressed)) {
        AutoPtr<IView> cv;
        mFlipper->GetCurrentView((IView**)&cv);
        ICandidateView::Probe(cv)->EnableActiveHighlight(TRUE);
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

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos
