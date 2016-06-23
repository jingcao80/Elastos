
#include "elastos/droid/systemui/statusbar/policy/CKeyButtonView.h"
#include "elastos/droid/systemui/statusbar/policy/CKeyButtonRipple.h"
#include "../../R.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Math.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::CObjectAnimator;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::Hardware::Input::CInputManagerHelper;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Hardware::Input::IInputManagerHelper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IHapticFeedbackConstants;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::ISoundEffectConstants;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityNodeInfoAccessibilityAction;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfoAccessibilityAction;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

//==============================================================================
//                  CKeyButtonView::CheckLongPressRunnable
//==============================================================================
CKeyButtonView::CheckLongPressRunnable::CheckLongPressRunnable(
    /* [in] */ CKeyButtonView* host)
    : mHost(host)
{
}

ECode CKeyButtonView::CheckLongPressRunnable::Run()
{
    Boolean tmp = FALSE;
    if (mHost->IsPressed(&tmp), tmp) {
        if (DEBUG) Logger::D(TAG, " CheckLongPressRunnable longpressed %s", TO_CSTR(mHost));
        if (mHost->IsLongClickable(&tmp), tmp) {
            // Just an old-fashioned ImageView
            mHost->PerformLongClick(&tmp);
        }
        else {
            mHost->SendEvent(IKeyEvent::ACTION_DOWN, IKeyEvent::FLAG_LONG_PRESS);
            mHost->SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_LONG_CLICKED);
        }
    }

    return NOERROR;
}

//==============================================================================
//                  CKeyButtonView
//==============================================================================
const String CKeyButtonView::TAG("CKeyButtonView");
const Boolean CKeyButtonView::DEBUG = FALSE;
const Float CKeyButtonView::DEFAULT_QUIESCENT_ALPHA = 1.f;

CAR_INTERFACE_IMPL(CKeyButtonView, ImageView, IKeyButtonView)

CAR_OBJECT_IMPL(CKeyButtonView)

CKeyButtonView::CKeyButtonView()
    : mDownTime(0)
    , mCode(0)
    , mTouchSlop(0)
    , mDrawingAlpha(1.0)
    , mQuiescentAlpha(DEFAULT_QUIESCENT_ALPHA)
    , mSupportsLongpress(TRUE)
{
}

ECode CKeyButtonView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CKeyButtonView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    ASSERT_SUCCEEDED(ImageView::constructor(context, attrs));

    CObjectAnimator::New((IAnimator**)&mAnimateToQuiescent);
    mCheckLongPress = new CheckLongPressRunnable(this);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::KeyButtonView);
    AutoPtr<ITypedArray> a;
    FAIL_RETURN(context->ObtainStyledAttributes(attrs, attrIds, defStyle, 0, (ITypedArray**)&a));

    a->GetInt32(R::styleable::KeyButtonView_keyCode, 0, &mCode);
    a->GetBoolean(R::styleable::KeyButtonView_keyRepeat, TRUE, &mSupportsLongpress);

    SetDrawingAlpha(mQuiescentAlpha);

    a->Recycle();

    SetClickable(TRUE);

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> vc;
    helper->Get(context, (IViewConfiguration**)&vc);
    vc->GetScaledTouchSlop(&mTouchSlop);

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    mAudioManager = IAudioManager::Probe(obj);
    AutoPtr<IDrawable> d;
    CKeyButtonRipple::New(context, this, (IDrawable**)&d);
    SetBackground(d);
    return NOERROR;
}

ECode CKeyButtonView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    ImageView::OnInitializeAccessibilityNodeInfo(info);
    if (mCode != 0) {
        AutoPtr<IAccessibilityNodeInfoAccessibilityAction> an;
        CAccessibilityNodeInfoAccessibilityAction::New(IAccessibilityNodeInfo::ACTION_CLICK, NULL,
                (IAccessibilityNodeInfoAccessibilityAction**)&an);
        info->AddAction(an);
        if (mSupportsLongpress) {
            an = NULL;
            CAccessibilityNodeInfoAccessibilityAction::New(IAccessibilityNodeInfo::ACTION_LONG_CLICK, NULL,
                    (IAccessibilityNodeInfoAccessibilityAction**)&an);
            info->AddAction(an);
        }
    }
    return NOERROR;
}

ECode CKeyButtonView::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (action == IAccessibilityNodeInfo::ACTION_CLICK && mCode != 0) {
        SendEvent(IKeyEvent::ACTION_DOWN, 0, SystemClock::GetUptimeMillis());
        SendEvent(IKeyEvent::ACTION_UP, 0);
        SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_CLICKED);
        PlaySoundEffect(ISoundEffectConstants::CLICK);
        *result = TRUE;
        return NOERROR;
    }
    else if (action == IAccessibilityNodeInfo::ACTION_LONG_CLICK && mCode != 0 && mSupportsLongpress) {
        SendEvent(IKeyEvent::ACTION_DOWN, IKeyEvent::FLAG_LONG_PRESS);
        SendEvent(IKeyEvent::ACTION_UP, 0);
        SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_LONG_CLICKED);
        *result = TRUE;
        return NOERROR;
    }
    return ImageView::PerformAccessibilityAction(action, arguments, result);
}

ECode CKeyButtonView::SetQuiescentAlpha(
    /* [in] */ Float alpha,
    /* [in] */ Boolean animate)
{
    mAnimateToQuiescent->Cancel();
    alpha = Elastos::Core::Math::Min(Elastos::Core::Math::Max(alpha, 0.f), 1.f);
    if (alpha == mQuiescentAlpha && alpha == mDrawingAlpha) return NOERROR;
    mQuiescentAlpha = alpha;
    if (DEBUG) Logger::D(TAG, "New quiescent alpha = %f", mQuiescentAlpha);
    if (animate) {
        AutoPtr<IObjectAnimator> o = AnimateToQuiescent();
        mAnimateToQuiescent = IAnimator::Probe(o);
        mAnimateToQuiescent->Start();
    }
    else {
        SetDrawingAlpha(mQuiescentAlpha);
    }
    return NOERROR;
}

AutoPtr<IObjectAnimator> CKeyButtonView::AnimateToQuiescent()
{
    AutoPtr<ArrayOf<Float> > param = ArrayOf<Float>::Alloc(1);
    (*param)[0] = mQuiescentAlpha;
    AutoPtr<IObjectAnimatorHelper> aHelper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&aHelper);

    AutoPtr<IObjectAnimator> o;
    aHelper->OfFloat(TO_IINTERFACE(this), String("drawingAlpha"), param,
            (IObjectAnimator**)&o);

    return o;
}

ECode CKeyButtonView::GetQuiescentAlpha(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mQuiescentAlpha;
    return NOERROR;
}

ECode CKeyButtonView::GetDrawingAlpha(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDrawingAlpha;
    return NOERROR;
}

ECode CKeyButtonView::SetDrawingAlpha(
    /* [in] */ Float x)
{
    SetImageAlpha((Int32) (x * 255));
    mDrawingAlpha = x;
    return NOERROR;
}

ECode CKeyButtonView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 action;
    ev->GetAction(&action);
    Int32 x, y;
    Float fx, fy;
    Boolean tmp = FALSE;
    switch (action) {
        case IMotionEvent::ACTION_DOWN: {
            mDownTime = SystemClock::GetUptimeMillis();
            SetPressed(TRUE);
            if (mCode != 0) {
                SendEvent(IKeyEvent::ACTION_DOWN, 0, mDownTime);
            }
            else {
                // Provide the same haptic feedback that the system offers for virtual keys.
                PerformHapticFeedback(IHapticFeedbackConstants::VIRTUAL_KEY, &tmp);
            }
            if (mSupportsLongpress) {
                RemoveCallbacks(mCheckLongPress, &tmp);
                AutoPtr<IViewConfigurationHelper> helper;
                CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
                Int32 timeout;
                helper->GetLongPressTimeout(&timeout);
                PostDelayed(mCheckLongPress, timeout, &tmp);
            }
            break;
        }

        case IMotionEvent::ACTION_MOVE: {
            ev->GetX(&fx);
            ev->GetY(&fy);
            x = (Int32)fx;
            y = (Int32)fy;
            Int32 w = 0, h = 0;
            GetWidth(&w);
            GetHeight(&h);
            SetPressed(x >= -mTouchSlop
                    && x < w + mTouchSlop
                    && y >= -mTouchSlop
                    && y < h + mTouchSlop);
            break;
        }

        case IMotionEvent::ACTION_CANCEL: {
            SetPressed(FALSE);
            if (mCode != 0) {
                SendEvent(IKeyEvent::ACTION_UP, IKeyEvent::FLAG_CANCELED);
            }
            if (mSupportsLongpress) {
                RemoveCallbacks(mCheckLongPress, &tmp);
            }
            break;
        }

        case IMotionEvent::ACTION_UP: {
            Boolean doIt = FALSE;
            IsPressed(&doIt);
            SetPressed(FALSE);
            if (mCode != 0) {
                if (doIt) {
                    SendEvent(IKeyEvent::ACTION_UP, 0);
                    SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_CLICKED);
                    PlaySoundEffect(ISoundEffectConstants::CLICK);
                }
                else {
                    SendEvent(IKeyEvent::ACTION_UP, IKeyEvent::FLAG_CANCELED);
                }
            }
            else {
                // no key code, just a regular ImageView
                if (doIt) {
                    PerformClick(&tmp);
                }
            }
            if (mSupportsLongpress) {
                RemoveCallbacks(mCheckLongPress, &tmp);
            }
            break;
        }
    }

    *result = TRUE;
    return NOERROR;
}

ECode CKeyButtonView::PlaySoundEffect(
    /* [in] */ Int32 soundConstant)
{
    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Int32 user = 0;
    helper->GetCurrentUser(&user);
    mAudioManager->PlaySoundEffect(soundConstant, user);
    return NOERROR;
}

ECode CKeyButtonView::SendEvent(
    /* [in] */ Int32 action,
    /* [in] */ Int32 flags)
{
    SendEvent(action, flags, SystemClock::GetUptimeMillis());
    return NOERROR;
}

void CKeyButtonView::SendEvent(
    /* [in] */ Int32 action,
    /* [in] */ Int32 flags,
    /* [in] */ Int64 when)
{
    Int32 repeatCount = (flags & IKeyEvent::FLAG_LONG_PRESS) != 0 ? 1 : 0;
    AutoPtr<IInputEvent> ev;
    CKeyEvent::New(mDownTime, when, action, mCode, repeatCount,
        0, IKeyCharacterMap::VIRTUAL_KEYBOARD, 0,
        flags | IKeyEvent::FLAG_FROM_SYSTEM | IKeyEvent::FLAG_VIRTUAL_HARD_KEY,
        IInputDevice::SOURCE_KEYBOARD, (IInputEvent**)&ev);

    AutoPtr<IInputManagerHelper> helper;
    CInputManagerHelper::AcquireSingleton((IInputManagerHelper**)&helper);
    AutoPtr<IInputManager> inputManager;
    helper->GetInstance((IInputManager**)&inputManager);
    Boolean result;
    inputManager->InjectInputEvent(ev, IInputManager::INJECT_INPUT_EVENT_MODE_ASYNC, &result);
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
