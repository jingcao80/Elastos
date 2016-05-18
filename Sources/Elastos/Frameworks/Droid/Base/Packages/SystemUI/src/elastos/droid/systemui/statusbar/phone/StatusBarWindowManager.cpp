
#include "elastos/droid/systemui/statusbar/phone/StatusBarWindowManager.h"
#include "../../R.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewManager;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

StatusBarWindowManager::State::State()
    : mKeyguardShowing(FALSE)
    , mKeyguardOccluded(FALSE)
    , mKeyguardNeedsInput(FALSE)
    , mStatusBarExpanded(FALSE)
    , mStatusBarFocusable(FALSE)
    , mKeyguardUserActivityTimeout(0)
    , mBouncerShowing(FALSE)
    , mKeyguardFadingAway(FALSE)
    , mQsExpanded(FALSE)
    , mStatusBarState(0)
{}

Boolean StatusBarWindowManager::State::IsKeyguardShowingAndNotOccluded()
{
    return mKeyguardShowing && !mKeyguardOccluded;
}

CAR_INTERFACE_IMPL(StatusBarWindowManager, Object, IStatusBarWindowManager)
StatusBarWindowManager::StatusBarWindowManager(
    /* [in] */ IContext* context)
    : mBarHeight(0)
    , mKeyguardScreenRotation(FALSE)
{
    mCurrentState = new State();
    mContext = context;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&obj);
    mWindowManager = IWindowManager::Probe(obj);
    mKeyguardScreenRotation = ShouldEnableKeyguardScreenRotation();
}

Boolean StatusBarWindowManager::ShouldEnableKeyguardScreenRotation()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean value = FALSE, tmp = FALSE;
    sp->GetBoolean(String("lockscreen.rot_override"), FALSE, &value);
    return value || (res->GetBoolean(R::bool_::config_enableLockScreenRotation, &tmp), tmp);
}

ECode StatusBarWindowManager::Add(
    /* [in] */ IView* statusBarView,
    /* [in] */ Int32 barHeight)
{
    // Now that the status bar window encompasses the sliding panel and its
    // translucent backdrop, the entire thing is made TRANSLUCENT and is
    // hardware-accelerated.
    CWindowManagerLayoutParams::New(
            IViewGroupLayoutParams::MATCH_PARENT,
            barHeight,
            IWindowManagerLayoutParams::TYPE_STATUS_BAR,
            IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
                    | IWindowManagerLayoutParams::FLAG_TOUCHABLE_WHEN_WAKING
                    | IWindowManagerLayoutParams::FLAG_SPLIT_TOUCH
                    | IWindowManagerLayoutParams::FLAG_WATCH_OUTSIDE_TOUCH
                    | IWindowManagerLayoutParams::FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS,
            IPixelFormat::TRANSLUCENT, (IWindowManagerLayoutParams**)&mLp);

    Int32 flags = 0;
    mLp->GetFlags(&flags);
    mLp->SetFlags(flags | IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED);
    mLp->SetGravity(IGravity::TOP);
    mLp->SetSoftInputMode(IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_RESIZE);

    AutoPtr<ICharSequence> title;
    CString::New(String("StatusBar"), (ICharSequence**)&title);
    mLp->SetTitle(title);

    String name;
    mContext->GetPackageName(&name);
    mLp->SetPackageName(name);
    mStatusBarView = statusBarView;
    mBarHeight = barHeight;
    IViewManager::Probe(mWindowManager)->AddView(mStatusBarView, IViewGroupLayoutParams::Probe(mLp));
    return NOERROR;
}

void StatusBarWindowManager::ApplyKeyguardFlags(
    /* [in] */ State* state)
{
    Int32 flags = 0, privateFlags = 0;
    mLp->GetFlags(&flags);
    mLp->GetPrivateFlags(&privateFlags);
    if (state->mKeyguardShowing) {
        mLp->SetFlags(flags | IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER);
        mLp->SetPrivateFlags(privateFlags | IWindowManagerLayoutParams::PRIVATE_FLAG_KEYGUARD);
    }
    else {
        mLp->SetFlags(flags & ~IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER);
        mLp->SetPrivateFlags(privateFlags & ~IWindowManagerLayoutParams::PRIVATE_FLAG_KEYGUARD);
    }
}

void StatusBarWindowManager::AdjustScreenOrientation(
    /* [in] */ State* state)
{
    if (state->IsKeyguardShowingAndNotOccluded()) {
        if (mKeyguardScreenRotation) {
            mLp->SetScreenOrientation(IActivityInfo::SCREEN_ORIENTATION_USER);
        }
        else {
            mLp->SetScreenOrientation(IActivityInfo::SCREEN_ORIENTATION_NOSENSOR);
        }
    }
    else {
        mLp->SetScreenOrientation(IActivityInfo::SCREEN_ORIENTATION_UNSPECIFIED);
    }
}

void StatusBarWindowManager::ApplyFocusableFlag(
    /* [in] */ State* state)
{
    Int32 flags = 0;
    mLp->GetFlags(&flags);
    if (state->IsKeyguardShowingAndNotOccluded() && state->mKeyguardNeedsInput
            && state->mBouncerShowing) {
        mLp->SetFlags(flags & ~IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE);
        mLp->SetFlags(flags & ~IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM);
    }
    else if (state->IsKeyguardShowingAndNotOccluded() || state->mStatusBarFocusable) {
        mLp->SetFlags(flags & ~IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE);
        mLp->SetFlags(flags | IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM);
    }
    else {
        mLp->SetFlags(flags | IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE);
        mLp->SetFlags(flags & ~IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM);
    }
}

void StatusBarWindowManager::ApplyHeight(
    /* [in] */ State* state)
{
    Boolean expanded = state->IsKeyguardShowingAndNotOccluded() || state->mStatusBarExpanded
            || state->mKeyguardFadingAway || state->mBouncerShowing;
    if (expanded) {
        IViewGroupLayoutParams::Probe(mLp)->SetHeight(IViewGroupLayoutParams::MATCH_PARENT);
    }
    else {
        IViewGroupLayoutParams::Probe(mLp)->SetHeight(mBarHeight);
    }
}

void StatusBarWindowManager::ApplyFitsSystemWindows(
    /* [in] */ State* state)
{
    mStatusBarView->SetFitsSystemWindows(!state->IsKeyguardShowingAndNotOccluded());
}

void StatusBarWindowManager::ApplyUserActivityTimeout(
    /* [in] */ State* state)
{
    if (state->IsKeyguardShowingAndNotOccluded()
            && state->mStatusBarState == IStatusBarState::KEYGUARD
            && !state->mQsExpanded) {
        mLp->SetUserActivityTimeout(state->mKeyguardUserActivityTimeout);
    } else {
        mLp->SetUserActivityTimeout(-1);
    }
}

void StatusBarWindowManager::ApplyInputFeatures(
    /* [in] */ State* state)
{
    Int32 inputFeatures = 0;
    mLp->GetInputFeatures(&inputFeatures);
    if (state->IsKeyguardShowingAndNotOccluded()
            && state->mStatusBarState == IStatusBarState::KEYGUARD
            && !state->mQsExpanded) {
        mLp->SetInputFeatures(inputFeatures | IWindowManagerLayoutParams::INPUT_FEATURE_DISABLE_USER_ACTIVITY);
    }
    else {
        mLp->SetInputFeatures(inputFeatures & ~IWindowManagerLayoutParams::INPUT_FEATURE_DISABLE_USER_ACTIVITY);
    }
}

void StatusBarWindowManager::Apply(
    /* [in] */ State* state)
{
    ApplyKeyguardFlags(state);
    ApplyFocusableFlag(state);
    AdjustScreenOrientation(state);
    ApplyHeight(state);
    ApplyUserActivityTimeout(state);
    ApplyInputFeatures(state);
    ApplyFitsSystemWindows(state);
    IViewManager::Probe(mWindowManager)->UpdateViewLayout(mStatusBarView, IViewGroupLayoutParams::Probe(mLp));
}

ECode StatusBarWindowManager::SetKeyguardShowing(
    /* [in] */ Boolean showing)
{
    mCurrentState->mKeyguardShowing = showing;
    Apply(mCurrentState);
    return NOERROR;
}

ECode StatusBarWindowManager::SetKeyguardOccluded(
    /* [in] */ Boolean occluded)
{
    mCurrentState->mKeyguardOccluded = occluded;
    Apply(mCurrentState);
    return NOERROR;
}

ECode StatusBarWindowManager::SetKeyguardNeedsInput(
    /* [in] */ Boolean needsInput)
{
    mCurrentState->mKeyguardNeedsInput = needsInput;
    Apply(mCurrentState);
    return NOERROR;
}

ECode StatusBarWindowManager::SetStatusBarExpanded(
    /* [in] */ Boolean expanded)
{
    mCurrentState->mStatusBarExpanded = expanded;
    mCurrentState->mStatusBarFocusable = expanded;
    Apply(mCurrentState);
    return NOERROR;
}

ECode StatusBarWindowManager::SetStatusBarFocusable(
    /* [in] */ Boolean focusable)
{
    mCurrentState->mStatusBarFocusable = focusable;
    Apply(mCurrentState);
    return NOERROR;
}

ECode StatusBarWindowManager::SetKeyguardUserActivityTimeout(
    /* [in] */ Int64 timeout)
{
    mCurrentState->mKeyguardUserActivityTimeout = timeout;
    Apply(mCurrentState);
    return NOERROR;
}

ECode StatusBarWindowManager::SetBouncerShowing(
    /* [in] */ Boolean showing)
{
    mCurrentState->mBouncerShowing = showing;
    Apply(mCurrentState);
    return NOERROR;
}

ECode StatusBarWindowManager::SetKeyguardFadingAway(
    /* [in] */ Boolean keyguardFadingAway)
{
    mCurrentState->mKeyguardFadingAway = keyguardFadingAway;
    Apply(mCurrentState);
    return NOERROR;
}

ECode StatusBarWindowManager::SetQsExpanded(
    /* [in] */ Boolean expanded)
{
    mCurrentState->mQsExpanded = expanded;
    Apply(mCurrentState);
    return NOERROR;
}

ECode StatusBarWindowManager::SetStatusBarState(
    /* [in] */ Int32 state)
{
    mCurrentState->mStatusBarState = state;
    Apply(mCurrentState);
    return NOERROR;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
