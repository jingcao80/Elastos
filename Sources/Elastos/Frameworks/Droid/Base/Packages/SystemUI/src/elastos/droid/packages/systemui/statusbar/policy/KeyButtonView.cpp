
#include "elastos/droid/systemui/statusbar/policy/KeyButtonView.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/hardware/input/CInputManager.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/droid/view/CKeyEvent.h"
#include "elastos/droid/view/SoundEffectConstants.h"
#include "elastos/droid/systemui/SystemUIR.h"
#include "elastos/droid/systemui/CSwipeHelper.h"
#include "elastos/droid/animation/CAnimatorSet.h"
#include "elastos/droid/animation/CObjectAnimatorHelper.h"

using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Hardware::Input::CInputManager;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::IHapticFeedbackConstants;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::SoundEffectConstants;
using Elastos::Droid::SystemUI::SystemUIR;
using Elastos::Droid::SystemUI::CSwipeHelper;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {


//==============================================================================
//                  KeyButtonView::CheckLongPressRunnable
//==============================================================================
KeyButtonView::CheckLongPressRunnable::CheckLongPressRunnable(
    /* [in] */ KeyButtonView* host)
    : mHost(host)
{
}

ECode KeyButtonView::CheckLongPressRunnable::Run()
{
    if (mHost->IsPressed()) {
        // Slog.d("KeyButtonView", "longpressed: " + this);
        if (mHost->mCode != 0) {
            mHost->SendEvent(IKeyEvent::ACTION_DOWN, IKeyEvent::FLAG_LONG_PRESS);
            mHost->SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_LONG_CLICKED);
        } else {
            // Just an old-fashioned ImageView
            mHost->PerformLongClick();
        }
    }

    return NOERROR;
}

//==============================================================================
//                  KeyButtonView
//==============================================================================

const Float KeyButtonView::GLOW_MAX_SCALE_FACTOR = 1.8f;
const Float KeyButtonView::BUTTON_QUIESCENT_ALPHA = 0.70f;

KeyButtonView::KeyButtonView()
    : mDownTime(0)
    , mCode(0)
    , mTouchSlop(0)
    , mGlowWidth(0)
    , mGlowHeight(0)
    , mGlowAlpha(0.0)
    , mGlowScale(1.0)
    , mDrawingAlpha(1.0)
    , mSupportsLongpress(TRUE)
{
    CRectF::New(0.0, 0.0, 0.0, 0.0, (IRectF**)&mRect);
    mCheckLongPress = new CheckLongPressRunnable(this);
}

KeyButtonView::KeyButtonView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : ImageView(context, attrs, 0)
    , mDownTime(0)
    , mCode(0)
    , mTouchSlop(0)
    , mGlowWidth(0)
    , mGlowHeight(0)
    , mGlowAlpha(0.0)
    , mGlowScale(1.0)
    , mDrawingAlpha(1.0)
    , mSupportsLongpress(TRUE)
{
    CRectF::New(0.0, 0.0, 0.0, 0.0, (IRectF**)&mRect);
    mCheckLongPress = (IRunnable*)new CheckLongPressRunnable(this);
}

KeyButtonView::KeyButtonView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : ImageView(context, attrs, defStyle)
    , mDownTime(0)
    , mCode(0)
    , mTouchSlop(0)
    , mGlowWidth(0)
    , mGlowHeight(0)
    , mGlowAlpha(0.0)
    , mGlowScale(1.0)
    , mDrawingAlpha(1.0)
    , mSupportsLongpress(TRUE)
{
    CRectF::New(0.0, 0.0, 0.0, 0.0, (IRectF**)&mRect);
    mCheckLongPress = (IRunnable*)new CheckLongPressRunnable(this);
}

ECode KeyButtonView::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return Init(context, attrs, 0);
}

ECode KeyButtonView::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    ASSERT_SUCCEEDED(ImageView::Init(context, attrs));

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(SystemUIR::styleable::KeyButtonView),
            ArraySize(SystemUIR::styleable::KeyButtonView));
    AutoPtr<ITypedArray> a;
    FAIL_RETURN(context->ObtainStyledAttributes(attrs, attrIds, defStyle, 0, (ITypedArray**)&a));

    a->GetInt32(SystemUIR::styleable::KeyButtonView_keyCode, 0, &mCode);
    a->GetBoolean(SystemUIR::styleable::KeyButtonView_keyRepeat, TRUE, &mSupportsLongpress);
    a->GetDrawable(SystemUIR::styleable::KeyButtonView_glowBackground, (IDrawable**)&mGlowBG);

    if (mGlowBG != NULL) {
        SetDrawingAlpha(BUTTON_QUIESCENT_ALPHA);
        mGlowBG->GetIntrinsicWidth(&mGlowWidth);
        mGlowBG->GetIntrinsicHeight(&mGlowHeight);
    }

    a->Recycle();

    SetClickable(TRUE);

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> vc;
    helper->Get(context, (IViewConfiguration**)&vc);
    vc->GetScaledTouchSlop(&mTouchSlop);
    return NOERROR;
}

void KeyButtonView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    if (mGlowBG != NULL) {
        Int32 status;
        canvas->Save(&status);
        Int32 w = GetWidth();
        Int32 h = GetHeight();
        Float aspect = (Float)mGlowWidth / mGlowHeight;
        Int32 drawW = (Int32)(h*aspect);
        Int32 drawH = h;
        Int32 margin = (drawW-w)/2;
        canvas->Scale(mGlowScale, mGlowScale, w*0.5f, h*0.5f);
        mGlowBG->SetBounds(-margin, 0, drawW-margin, drawH);
        mGlowBG->SetAlpha((Int32)(mDrawingAlpha * mGlowAlpha * 255));
        mGlowBG->Draw(canvas);
        canvas->Restore();
        ((CRectF*)mRect.Get())->mRight = (w);
        ((CRectF*)mRect.Get())->mBottom = (h);
    }

    ImageView::OnDraw(canvas);
}

ECode KeyButtonView::SetDrawingAlpha(
    /* [in] */ Float x)
{
    if (mGlowBG == NULL) return NOERROR;
    // Calling setAlpha(Int32), which is an ImageView-specific
    // method that's different from setAlpha(float). This sets
    // the alpha on this ImageView's drawable directly
    SetAlpha((Int32) (x * 255));
    mDrawingAlpha = x;
    return NOERROR;
}

Float KeyButtonView::GetDrawingAlpha()
{
    if (mGlowBG == NULL) return 0;
    return mDrawingAlpha;
}

ECode KeyButtonView::SetGlowAlpha(
    /* [in] */ Float x)
{
    if (mGlowBG == NULL) return NOERROR;
    mGlowAlpha = x;
    Invalidate();
    return NOERROR;
}

Float KeyButtonView::GetGlowAlpha()
{
    if (mGlowBG == NULL) return 0;
    return mGlowAlpha;
}

ECode KeyButtonView::SetGlowScale(
    /* [in] */ Float x)
{
    if (mGlowBG == NULL) return NOERROR;
    mGlowScale = x;
    Float w = GetWidth();
    Float h = GetHeight();
    if (GLOW_MAX_SCALE_FACTOR <= 1.0f) {
        // this only works if we know the glow will never leave our bounds
        Invalidate();
    } else {
        Float rx = (w * (GLOW_MAX_SCALE_FACTOR - 1.0f)) / 2.0f + 1.0f;
        Float ry = (h * (GLOW_MAX_SCALE_FACTOR - 1.0f)) / 2.0f + 1.0f;
        AutoPtr<IRectF> r;
        CRectF::New(GetLeft() - rx, GetTop() - ry, GetRight() + rx, GetBottom() + ry, (IRectF**)&r);
        CSwipeHelper::InvalidateGlobalRegion(THIS_PROBE(IView), r);
        // also invalidate our immediate parent to help avoid situations where nearby glows
        // interfere
        AutoPtr<IViewParent> parent = GetParent();
        if (parent) {
            View* pv = VIEW_PROBE(parent);
            pv->Invalidate();
        }
    }
    return NOERROR;
}

Float KeyButtonView::GetGlowScale()
{
    if (mGlowBG == NULL) return 0;
    return mGlowScale;
}

ECode KeyButtonView::SetPressed(
    /* [in] */ Boolean pressed)
{
    if (mGlowBG != NULL) {
        if (pressed != IsPressed()) {
            Boolean running;
            if (mPressedAnim != NULL && (mPressedAnim->IsRunning(&running), running)) {
                mPressedAnim->Cancel();
            }
            AutoPtr<IAnimatorSet> as;
            CAnimatorSet::New((IAnimatorSet**)&as);
            mPressedAnim = as;

            AutoPtr<IObjectAnimatorHelper> helper;
            CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
            if (pressed) {
                if (mGlowScale < GLOW_MAX_SCALE_FACTOR)
                    mGlowScale = GLOW_MAX_SCALE_FACTOR;
                if (mGlowAlpha < BUTTON_QUIESCENT_ALPHA)
                    mGlowAlpha = BUTTON_QUIESCENT_ALPHA;
                SetDrawingAlpha(1.0f);

                AutoPtr<ArrayOf<Float> > alphaValues = ArrayOf<Float>::Alloc(1);
                AutoPtr<ArrayOf<Float> > scaleValues = ArrayOf<Float>::Alloc(1);
                AutoPtr<IObjectAnimator> a1, a2;
                alphaValues->Set(0, 1.0f);
                helper->OfFloat(THIS_PROBE(IView), String("glowAlpha"), alphaValues, (IObjectAnimator**)&a1);
                scaleValues->Set(0, GLOW_MAX_SCALE_FACTOR);
                helper->OfFloat(THIS_PROBE(IView), String("glowScale"), scaleValues, (IObjectAnimator**)&a2);

                AutoPtr<ArrayOf<IAnimator*> > amnimtors = ArrayOf<IAnimator*>::Alloc(2);
                amnimtors->Set(0, a1);
                amnimtors->Set(1, a2);
                as->PlayTogether(amnimtors);
                as->SetDuration(50);
            } else {
                AutoPtr<ArrayOf<Float> > alphaValues = ArrayOf<Float>::Alloc(1);
                AutoPtr<ArrayOf<Float> > scaleValues = ArrayOf<Float>::Alloc(1);
                AutoPtr<ArrayOf<Float> > drawingValues = ArrayOf<Float>::Alloc(1);
                AutoPtr<IObjectAnimator> a1, a2, a3;
                alphaValues->Set(0, 0.0f);
                helper->OfFloat(THIS_PROBE(IView), String("glowAlpha"), alphaValues, (IObjectAnimator**)&a1);
                scaleValues->Set(0, 1.0f);
                helper->OfFloat(THIS_PROBE(IView), String("glowScale"), scaleValues, (IObjectAnimator**)&a2);
                drawingValues->Set(0, BUTTON_QUIESCENT_ALPHA);
                helper->OfFloat(THIS_PROBE(IView), String("drawingAlpha"), drawingValues, (IObjectAnimator**)&a3);

                AutoPtr<ArrayOf<IAnimator*> > amnimtors = ArrayOf<IAnimator*>::Alloc(3);
                amnimtors->Set(0, a1);
                amnimtors->Set(1, a2);
                amnimtors->Set(2, a3);
                as->PlayTogether(amnimtors);
                as->SetDuration(500);
            }
            as->Start();
        }
    }
    return ImageView::SetPressed(pressed);
}

Boolean KeyButtonView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    Int32 action;
    ev->GetAction(&action);
    Int32 x, y;
    Float fx, fy;
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            //Slog.d("KeyButtonView", "press");
            mDownTime = SystemClock::GetUptimeMillis();
            SetPressed(TRUE);
            if (mCode != 0) {
                SendEvent(IKeyEvent::ACTION_DOWN, 0, mDownTime);
            } else {
                // Provide the same haptic feedback that the system offers for virtual keys.
                PerformHapticFeedback(IHapticFeedbackConstants::VIRTUAL_KEY);
            }
            if (mSupportsLongpress) {
                RemoveCallbacks(mCheckLongPress);
                AutoPtr<IViewConfigurationHelper> helper;
                CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
                Int32 timeout;
                helper->GetLongPressTimeout(&timeout);
                PostDelayed(mCheckLongPress, timeout);
            }
            break;
        case IMotionEvent::ACTION_MOVE:
            ev->GetX(&fx);
            ev->GetY(&fy);
            x = (Int32)fx;
            y = (Int32)fy;
            SetPressed(x >= -mTouchSlop
                    && x < GetWidth() + mTouchSlop
                    && y >= -mTouchSlop
                    && y < GetHeight() + mTouchSlop);
            break;
        case IMotionEvent::ACTION_CANCEL:
            SetPressed(FALSE);
            if (mCode != 0) {
                SendEvent(IKeyEvent::ACTION_UP, IKeyEvent::FLAG_CANCELED);
            }
            if (mSupportsLongpress) {
                RemoveCallbacks(mCheckLongPress);
            }
            break;
        case IMotionEvent::ACTION_UP:
            Boolean doIt = IsPressed();
            SetPressed(FALSE);
            if (mCode != 0) {
                if (doIt) {
                    SendEvent(IKeyEvent::ACTION_UP, 0);
                    SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_CLICKED);
                    PlaySoundEffect(SoundEffectConstants::CLICK);
                } else {
                    SendEvent(IKeyEvent::ACTION_UP, IKeyEvent::FLAG_CANCELED);
                }
            } else {
                // no key code, just a regular ImageView
                if (doIt) {
                    PerformClick();
                }
            }
            if (mSupportsLongpress) {
                RemoveCallbacks(mCheckLongPress);
            }
            break;
    }

    return TRUE;
}

void KeyButtonView::SendEvent(
    /* [in] */ Int32 action,
    /* [in] */ Int32 flags)
{
    SendEvent(action, flags, SystemClock::GetUptimeMillis());
}

void KeyButtonView::SendEvent(
    /* [in] */ Int32 action,
    /* [in] */ Int32 flags,
    /* [in] */ Int64 when)
{
    Int32 repeatCount = (flags & IKeyEvent::FLAG_LONG_PRESS) != 0 ? 1 : 0;
    AutoPtr<IKeyEvent> ev;
    CKeyEvent::New(mDownTime, when, action, mCode, repeatCount,
            0, IKeyCharacterMap::VIRTUAL_KEYBOARD, 0,
            flags | IKeyEvent::FLAG_FROM_SYSTEM | IKeyEvent::FLAG_VIRTUAL_HARD_KEY,
            IInputDevice::SOURCE_KEYBOARD, (IKeyEvent**)&ev);

    AutoPtr<IInputManager> inputManager = CInputManager::GetInstance();
    Boolean result;
    inputManager->InjectInputEvent(
            ev, IInputManager::INJECT_INPUT_EVENT_MODE_ASYNC, &result);
}


}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
