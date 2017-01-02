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

#include "elastos/droid/inputmethod/pinyin/CSkbContainer.h"
#include "elastos/droid/inputmethod/pinyin/SoftKeyboard.h"
#include "elastos/droid/inputmethod/pinyin/SkbPool.h"
#include "elastos/droid/inputmethod/pinyin/CBalloonHint.h"
#include "elastos/droid/inputmethod/pinyin/CPinyinIME.h"
#include "elastos/droid/inputmethod/pinyin/CSoftKeyboardView.h"
#include "elastos/droid/inputmethod/pinyin/InputModeSwitcher.h"
#include "R.h"
#include <elastos/droid/os/SystemClock.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
// using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::Widget::CPopupWindow;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {


//=====================================================
// CSkbContainer::LongPressTimer
//=====================================================
CAR_INTERFACE_IMPL(CSkbContainer::InnerListener, Object, IViewOnTouchListener)

CSkbContainer::InnerListener::InnerListener(
    /* [in] */ CSkbContainer* host)
    : mHost(host)
{}

ECode CSkbContainer::InnerListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    return mHost->OnTouch(v, event, result);
}

//=====================================================
// CSkbContainer::LongPressTimer
//=====================================================
const Int32 CSkbContainer::LongPressTimer::LONG_PRESS_TIMEOUT1 = 500;
const Int32 CSkbContainer::LongPressTimer::LONG_PRESS_TIMEOUT2 = 100;
const Int32 CSkbContainer::LongPressTimer::LONG_PRESS_TIMEOUT3 = 100;
const Int32 CSkbContainer::LongPressTimer::LONG_PRESS_KEYNUM1 = 1;
const Int32 CSkbContainer::LongPressTimer::LONG_PRESS_KEYNUM2 = 3;

CSkbContainer::LongPressTimer::LongPressTimer(
    /* [in] */ CSkbContainer* skbContainer)
    : mSkbContainer(skbContainer)
    , mResponseTimes(0)
{}

void CSkbContainer::LongPressTimer::StartTimer()
{
    Boolean result;
    PostAtTime(this, SystemClock::GetUptimeMillis() + LONG_PRESS_TIMEOUT1, &result);
    mResponseTimes = 0;
}

Boolean CSkbContainer::LongPressTimer::RemoveTimer()
{
    RemoveCallbacks(this);
    return TRUE;
}

ECode CSkbContainer::LongPressTimer::Run()
{
    if (mSkbContainer->mWaitForTouchUp) {
        mResponseTimes++;
        if (mSkbContainer->mSoftKeyDown->Repeatable()) {
            if (mSkbContainer->mSoftKeyDown->IsUserDefKey()) {
                if (1 == mResponseTimes) {
                    if (mSkbContainer->mInputModeSwitcher->TryHandleLongPressSwitch(mSkbContainer->mSoftKeyDown->mKeyCode)) {
                        mSkbContainer->mDiscardEvent = TRUE;
                        mSkbContainer->ResetKeyPress(0);
                    }
                }
            }
            else {
                mSkbContainer->ResponseKeyEvent(mSkbContainer->mSoftKeyDown);
                Int64 timeout;
                if (mResponseTimes < LONG_PRESS_KEYNUM1) {
                    timeout = LONG_PRESS_TIMEOUT1;
                }
                else if (mResponseTimes < LONG_PRESS_KEYNUM2) {
                    timeout = LONG_PRESS_TIMEOUT2;
                }
                else {
                    timeout = LONG_PRESS_TIMEOUT3;
                }
                Boolean result;
                PostAtTime(this, SystemClock::GetUptimeMillis() + timeout, &result);
            }
        }
        else {
            if (1 == mResponseTimes) {
                mSkbContainer->PopupSymbols();
            }
        }
    }
    return NOERROR;
}


//=====================================================
// CSkbContainer
//=====================================================
const Int32 CSkbContainer::Y_BIAS_CORRECTION = -10;
const Int32 CSkbContainer::MOVE_TOLERANCE = 6;
Boolean CSkbContainer::POPUPWINDOW_FOR_PRESSED_UI = FALSE;

CAR_OBJECT_IMPL(CSkbContainer);

CAR_INTERFACE_IMPL(CSkbContainer, RelativeLayout, ISkbContainer);

CSkbContainer::CSkbContainer()
    : mSkbLayout(0)
    , mLastCandidatesShowing(FALSE)
    , mPopupSkbShow(FALSE)
    , mPopupSkbNoResponse(FALSE)
    , mPopupX(0)
    , mPopupY(0)
    , mWaitForTouchUp(FALSE)
    , mDiscardEvent(FALSE)
    , mYBiasCorrection(0)
    , mXLast(0)
    , mYLast(0)
{
    mSkvPosInContainer = ArrayOf<Int32>::Alloc(2);
    mXyPosTmp = ArrayOf<Int32>::Alloc(2);
}

ECode CSkbContainer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(RelativeLayout::constructor(context, attrs));

    mEnvironment = Environment::GetInstance();

    mLongPressTimer = new LongPressTimer(this);

    // If it runs on an emulator, no bias correction
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String temp;
    sp->Get(String("ro.kernel.qemu"), &temp);
    if (temp.Equals("1")) {
        mYBiasCorrection = 0;
    }
    else {
        mYBiasCorrection = Y_BIAS_CORRECTION;
    }
    CBalloonHint::NewByFriend(context, this, MeasureSpec::AT_MOST, (CBalloonHint**)&mBalloonPopup);
    if (POPUPWINDOW_FOR_PRESSED_UI) {
        CBalloonHint::NewByFriend(context, this, MeasureSpec::AT_MOST, (CBalloonHint**)&mBalloonOnKey);
    }

    CPopupWindow::New(mContext, (IPopupWindow**)&mPopupSkb);
    mPopupSkb->SetBackgroundDrawable(NULL);
    mPopupSkb->SetClippingEnabled(FALSE);
    return NOERROR;
}

void CSkbContainer::SetService(
    /* [in] */ IInputMethodService* service)
{
    mService = service;
}

void CSkbContainer::SetInputModeSwitcher(
    /* [in] */ InputModeSwitcher* inputModeSwitcher)
{
    mInputModeSwitcher = inputModeSwitcher;
}

void CSkbContainer::SetGestureDetector(
    /* [in] */ IGestureDetector* gestureDetector)
{
    mGestureDetector = gestureDetector;
}

Boolean CSkbContainer::IsCurrentSkbSticky()
{
    if (NULL == mMajorView) return TRUE;

    AutoPtr<SoftKeyboard> skb = mMajorView->GetSoftKeyboard();
    if (NULL != skb) {
        return skb->GetStickyFlag();
    }
    return TRUE;
}

void CSkbContainer::ToggleCandidateMode(
    /* [in] */ Boolean candidatesShowing)
{
    if (NULL == mMajorView || !mInputModeSwitcher->IsChineseText()
            || mLastCandidatesShowing == candidatesShowing) return;
    mLastCandidatesShowing = candidatesShowing;

    AutoPtr<SoftKeyboard> skb = mMajorView->GetSoftKeyboard();
    if (NULL == skb) return;

    Int32 state = mInputModeSwitcher->GetTooggleStateForCnCand();
    if (!candidatesShowing) {
        skb->DisableToggleState(state, FALSE);
        AutoPtr<InputModeSwitcher::ToggleStates> states = mInputModeSwitcher->GetToggleStates();
        skb->EnableToggleStates(states);
    }
    else {
        skb->EnableToggleState(state, FALSE);
    }

    mMajorView->Invalidate();
}

void CSkbContainer::UpdateInputMode()
{
    Int32 skbLayout = mInputModeSwitcher->GetSkbLayout();
    if (mSkbLayout != skbLayout) {
        mSkbLayout = skbLayout;
        UpdateSkbLayout();
    }

    mLastCandidatesShowing = FALSE;

    if (NULL == mMajorView) return;

    AutoPtr<SoftKeyboard> skb = mMajorView->GetSoftKeyboard();
    if (NULL == skb) return;
    AutoPtr<InputModeSwitcher::ToggleStates> states = mInputModeSwitcher->GetToggleStates();
    skb->EnableToggleStates(states);
    Invalidate();
}

void CSkbContainer::UpdateSkbLayout()
{
    Int32 screenWidth = mEnvironment->GetScreenWidth();
    Int32 skbHeight = mEnvironment->GetSkbHeight();

    AutoPtr<IResources> r;
    mContext->GetResources((IResources**)&r);
    if (NULL == mSkbFlipper) {
        AutoPtr<IView> view;
        FindViewById(R::id::alpha_floatable, (IView**)&view);
        mSkbFlipper = IViewFlipper::Probe(view);
    }
    AutoPtr<IView> view;
    IViewGroup::Probe(mSkbFlipper)->GetChildAt(0, (IView**)&view);
    mMajorView = (CSoftKeyboardView*)ISoftKeyboardView::Probe(view);

    AutoPtr<SoftKeyboard> majorSkb;
    AutoPtr<SkbPool> skbPool = SkbPool::GetInstance();

    switch (mSkbLayout) {
    case R::xml::skb_qwerty:
        majorSkb = skbPool->GetSoftKeyboard(R::xml::skb_qwerty,
                R::xml::skb_qwerty, screenWidth, skbHeight, mContext);
        break;

    case R::xml::skb_sym1:
        majorSkb = skbPool->GetSoftKeyboard(R::xml::skb_sym1, R::xml::skb_sym1,
                screenWidth, skbHeight, mContext);
        break;

    case R::xml::skb_sym2:
        majorSkb = skbPool->GetSoftKeyboard(R::xml::skb_sym2, R::xml::skb_sym2,
                screenWidth, skbHeight, mContext);
        break;

    case R::xml::skb_smiley:
        majorSkb = skbPool->GetSoftKeyboard(R::xml::skb_smiley,
                R::xml::skb_smiley, screenWidth, skbHeight, mContext);
        break;

    case R::xml::skb_phone:
        majorSkb = skbPool->GetSoftKeyboard(R::xml::skb_phone,
                R::xml::skb_phone, screenWidth, skbHeight, mContext);
        break;
    default:
        break;
    }

    if (NULL == majorSkb || !mMajorView->SetSoftKeyboard(majorSkb)) {
        return;
    }
    mMajorView->SetBalloonHint(mBalloonOnKey, mBalloonPopup, FALSE);
    mMajorView->Invalidate();
}

void CSkbContainer::ResponseKeyEvent(
    /* [in] */ SoftKey* sKey)
{
    if (NULL == sKey) return;
    ((CPinyinIME*)IPinyinIME::Probe(mService))->ResponseSoftKeyEvent(sKey);
}

AutoPtr<CSoftKeyboardView> CSkbContainer::InKeyboardView(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ ArrayOf<Int32>* positionInParent)
{
    if (mPopupSkbShow) {
        Int32 value = 0;
        if (mPopupX <= x && mPopupX + (mPopupSkb->GetWidth(&value), value) > x
                && mPopupY <= y && mPopupY + (mPopupSkb->GetHeight(&value), value) > y) {
            (*positionInParent)[0] = mPopupX;
            (*positionInParent)[1] = mPopupY;
            mPopupSkbView->SetOffsetToSkbContainer(positionInParent);
            return mPopupSkbView;
        }
        return NULL;
    }

    return mMajorView;
}

void CSkbContainer::PopupSymbols()
{
    Int32 popupResId = mSoftKeyDown->GetPopupResId();
    if (popupResId > 0) {
        Int32 skbContainerWidth;
        GetWidth(&skbContainerWidth);
        Int32 skbContainerHeight;
        GetHeight(&skbContainerHeight);
        // The paddings of the background are not included.
        Int32 miniSkbWidth = (Int32) (skbContainerWidth * 0.8);
        Int32 miniSkbHeight = (Int32) (skbContainerHeight * 0.23);

        AutoPtr<SkbPool> skbPool = SkbPool::GetInstance();
        AutoPtr<SoftKeyboard> skb = skbPool->GetSoftKeyboard(popupResId, popupResId,
                miniSkbWidth, miniSkbHeight, mContext);
        if (NULL == skb) return;

        mPopupX = (skbContainerWidth - skb->GetSkbTotalWidth()) / 2;
        mPopupY = (skbContainerHeight - skb->GetSkbTotalHeight()) / 2;

        if (NULL == mPopupSkbView) {
            CSoftKeyboardView::NewByFriend(mContext, NULL, (CSoftKeyboardView**)&mPopupSkbView);
            mPopupSkbView->OnMeasure(IViewGroupLayoutParams::WRAP_CONTENT,
                    IViewGroupLayoutParams::WRAP_CONTENT);
        }

        AutoPtr<InnerListener> listener = new InnerListener(this);
        mPopupSkbView->SetOnTouchListener(listener);
        mPopupSkbView->SetSoftKeyboard(skb);
        mPopupSkbView->SetBalloonHint(mBalloonOnKey, mBalloonPopup, TRUE);

        mPopupSkb->SetContentView(mPopupSkbView);
        Int32 value = 0;
        mPopupSkb->SetWidth(skb->GetSkbCoreWidth()
                + (mPopupSkbView->GetPaddingLeft(&value), value)
                + (mPopupSkbView->GetPaddingRight(&value), value));
        mPopupSkb->SetHeight(skb->GetSkbCoreHeight()
                + (mPopupSkbView->GetPaddingTop(&value), value)
                + (mPopupSkbView->GetPaddingBottom(&value), value));

        GetLocationInWindow(mXyPosTmp);
        mPopupSkb->ShowAtLocation(this, IGravity::NO_GRAVITY, mPopupX, mPopupY
                + (*mXyPosTmp)[1]);
        mPopupSkbShow = TRUE;
        mPopupSkbNoResponse = TRUE;
        // Invalidate itself to dim the current soft keyboards.
        DimSoftKeyboard(TRUE);
        ResetKeyPress(0);
    }
}

void CSkbContainer::DimSoftKeyboard(
    /* [in] */ Boolean dimSkb)
{
    mMajorView->DimSoftKeyboard(dimSkb);
}

void CSkbContainer::DismissPopupSkb()
{
    mPopupSkb->Dismiss();
    mPopupSkbShow = FALSE;
    DimSoftKeyboard(FALSE);
    ResetKeyPress(0);
}

void CSkbContainer::ResetKeyPress(
    /* [in] */ Int64 delay)
{
    mLongPressTimer->RemoveTimer();

    if (NULL != mSkv) {
        mSkv->ResetKeyPress(delay);
    }
}

Boolean CSkbContainer::HandleBack(
    /* [in] */ Boolean realAction)
{
    if (mPopupSkbShow) {
        if (!realAction) return TRUE;

        DismissPopupSkb();
        mDiscardEvent = TRUE;
        return TRUE;
    }
    return FALSE;
}

void CSkbContainer::DismissPopups()
{
    HandleBack(TRUE);
    ResetKeyPress(0);
}

ECode CSkbContainer::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    AutoPtr<Environment> env = Environment::GetInstance();
    Int32 measuredWidth = env->GetScreenWidth();
    Int32 measuredHeight;
    GetPaddingTop(&measuredHeight);
    measuredHeight += env->GetSkbHeight();
    widthMeasureSpec = MeasureSpec::MakeMeasureSpec(measuredWidth,
            MeasureSpec::EXACTLY);
    heightMeasureSpec = MeasureSpec::MakeMeasureSpec(measuredHeight,
            MeasureSpec::EXACTLY);
    RelativeLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    return NOERROR;
}

ECode CSkbContainer::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(RelativeLayout::OnTouchEvent(event, result));

    Boolean flipping = FALSE;
    if (mSkbFlipper->IsFlipping(&flipping), flipping) {
        ResetKeyPress(0);
        *result = TRUE;
        return NOERROR;
    }

    Float value = 0.f;
    Int32 x = (Int32) (event->GetX(&value), value);
    Int32 y = (Int32) (event->GetY(&value), value);
    // Bias correction
    y = y + mYBiasCorrection;

    // Ignore short-distance movement event to get better performance.
    Int32 action = 0;
    if (event->GetAction(&action), action == IMotionEvent::ACTION_MOVE) {
        if (Elastos::Core::Math::Abs(x - mXLast) <= MOVE_TOLERANCE
                && Elastos::Core::Math::Abs(y - mYLast) <= MOVE_TOLERANCE) {
            *result = TRUE;
            return NOERROR;
        }
    }

    mXLast = x;
    mYLast = y;

    if (!mPopupSkbShow) {
        Boolean res = FALSE;
        if (mGestureDetector->OnTouchEvent(event, &res), res) {
            ResetKeyPress(0);
            mDiscardEvent = TRUE;
            *result = TRUE;
            return NOERROR;
        }
    }

    switch (action) {
        case IMotionEvent::ACTION_DOWN: {
            ResetKeyPress(0);

            mWaitForTouchUp = TRUE;
            mDiscardEvent = FALSE;

            mSkv = NULL;
            mSoftKeyDown = NULL;
            mSkv = InKeyboardView(x, y, mSkvPosInContainer);
            if (NULL != mSkv) {
                mSoftKeyDown = mSkv->OnKeyPress(x - (*mSkvPosInContainer)[0], y
                        - (*mSkvPosInContainer)[1], mLongPressTimer, FALSE);
            }
            break;
        }

        case IMotionEvent::ACTION_MOVE: {
            Int32 w, h;
            if (x < 0 || x >= (GetWidth(&w), w) || y < 0 || y >= (GetHeight(&h), h)) {
                break;
            }
            if (mDiscardEvent) {
                ResetKeyPress(0);
                break;
            }

            if (mPopupSkbShow && mPopupSkbNoResponse) {
                break;
            }

            AutoPtr<CSoftKeyboardView> skv = InKeyboardView(x, y, mSkvPosInContainer);
            if (NULL != skv) {
                if (skv != mSkv) {
                    mSkv = skv;
                    mSoftKeyDown = mSkv->OnKeyPress(x - (*mSkvPosInContainer)[0], y
                            - (*mSkvPosInContainer)[1], mLongPressTimer, TRUE);
                }
                else if (NULL != skv) {
                    if (NULL != mSkv) {
                        mSoftKeyDown = mSkv->OnKeyMove(
                                x - (*mSkvPosInContainer)[0], y
                                        - (*mSkvPosInContainer)[1]);
                        if (NULL == mSoftKeyDown) {
                            mDiscardEvent = TRUE;
                        }
                    }
                }
            }
            break;
        }

        case IMotionEvent::ACTION_UP: {
            if (mDiscardEvent) {
                ResetKeyPress(0);
                break;
            }

            mWaitForTouchUp = FALSE;

            // The view which got the {@link MotionEvent#ACTION_DOWN} event is
            // always used to handle this event.
            if (NULL != mSkv) {
                mSkv->OnKeyRelease(x - (*mSkvPosInContainer)[0], y
                        - (*mSkvPosInContainer)[1]);
            }

            if (!mPopupSkbShow || !mPopupSkbNoResponse) {
                ResponseKeyEvent(mSoftKeyDown);
            }

            if (mSkv == mPopupSkbView && !mPopupSkbNoResponse) {
                DismissPopupSkb();
            }
            mPopupSkbNoResponse = FALSE;
            break;
        }

        case IMotionEvent::ACTION_CANCEL:{
            break;
        }
    }

    if (NULL == mSkv) {
        *result = FALSE;
        return NOERROR;
    }

    *result = TRUE;
    return NOERROR;
}

ECode CSkbContainer::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // Translate the event to fit to the container.
    AutoPtr<IMotionEventHelper> helper;
    CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
    AutoPtr<IMotionEvent> newEv;
    Int64 tm = 0;
    Int32 value = 0;
    Float fv = 0.f;
    helper->Obtain(
            (event->GetDownTime(&tm), tm),
            (IInputEvent::Probe(event)->GetEventTime(&tm), tm),
            (event->GetAction(&value), value),
            (event->GetX(&fv), fv) + mPopupX,
            (event->GetY(&fv), fv) + mPopupY,
            (event->GetPressure(&fv), fv),
            (event->GetSize(&fv), fv),
            (event->GetMetaState(&value), value),
            (event->GetXPrecision(&fv), fv),
            (event->GetYPrecision(&fv), fv),
            (IInputEvent::Probe(event)->GetDeviceId(&value), value),
            (event->GetEdgeFlags(&value), value),
            (IMotionEvent**)&newEv);
    return OnTouchEvent(newEv, result);
}

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos
