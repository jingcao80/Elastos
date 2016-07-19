
#include "elastos/droid/systemui/statusbar/phone/StatusBarKeyguardViewManager.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/droid/os/SystemClock.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::SystemUI::Keyguard::IKeyguardUpdateMonitor;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

const Int64 StatusBarKeyguardViewManager::HIDE_TIMING_CORRECTION_MS = -3 * 16;
const Int64 StatusBarKeyguardViewManager::NAV_BAR_SHOW_DELAY_BOUNCER = 320;
String StatusBarKeyguardViewManager::TAG("StatusBarKeyguardViewManager");

StatusBarKeyguardViewManager::Runnable1::Runnable1(
    /* [in] */ StatusBarKeyguardViewManager* host,
    /* [in] */ IIKeyguardShowCallback* callback)
    : mHost(host)
    , mCallback(callback)
{}

ECode StatusBarKeyguardViewManager::Runnable1::Run()
{
    AutoPtr<IBinder> token;
    IView::Probe(mHost->mContainer)->GetWindowToken((IBinder**)&token);
    if (FAILED(mCallback->OnShown(token))) {
        Slogger::W(TAG, "Exception calling onShown():");
    }
    return NOERROR;
}

StatusBarKeyguardViewManager::Runnable2::Runnable2(
    /* [in] */ StatusBarKeyguardViewManager* host)
    : mHost(host)
{}

ECode StatusBarKeyguardViewManager::Runnable2::Run()
{
    AutoPtr<INavigationBarView> view;
    mHost->mPhoneStatusBar->GetNavigationBarView((INavigationBarView**)&view);
    IView::Probe(view)->SetVisibility(IView::VISIBLE);
    return NOERROR;
}

StatusBarKeyguardViewManager::Runnable3::Runnable3(
    /* [in] */ StatusBarKeyguardViewManager* host)
    : mHost(host)
{}

ECode StatusBarKeyguardViewManager::Runnable3::Run()
{
    mHost->mViewMediatorCallback->ReadyForKeyguardDone();
    return NOERROR;
}

StatusBarKeyguardViewManager::Runnable4::Runnable4(
    /* [in] */ StatusBarKeyguardViewManager* host)
    : mHost(host)
{}

ECode StatusBarKeyguardViewManager::Runnable4::Run()
{
    mHost->mStatusBarWindowManager->SetKeyguardOccluded(TRUE);
    mHost->Reset();
    return NOERROR;
}

StatusBarKeyguardViewManager::Runnable5::Runnable5(
    /* [in] */ StatusBarKeyguardViewManager* host)
    : mHost(host)
{}

ECode StatusBarKeyguardViewManager::Runnable5::Run()
{
    mHost->mStatusBarWindowManager->SetKeyguardShowing(FALSE);
    mHost->mStatusBarWindowManager->SetKeyguardFadingAway(TRUE);
    mHost->mBouncer->Hide(TRUE /* destroyView */);
    mHost->UpdateStates();
    mHost->mScrimController->AnimateKeyguardFadingOut(
            IPhoneStatusBar::FADE_KEYGUARD_START_DELAY,
            IPhoneStatusBar::FADE_KEYGUARD_DURATION, NULL);
    return NOERROR;
}

StatusBarKeyguardViewManager::Runnable6::Runnable6(
    /* [in] */ StatusBarKeyguardViewManager* host)
    : mHost(host)
{}

ECode StatusBarKeyguardViewManager::Runnable6::Run()
{
    Boolean tmp = FALSE;
    mHost->mPhoneStatusBar->HideKeyguard(&tmp);
    mHost->mStatusBarWindowManager->SetKeyguardFadingAway(FALSE);
    mHost->mViewMediatorCallback->KeyguardGone();
    mHost->ExecuteAfterKeyguardGoneAction();
    return NOERROR;
}

StatusBarKeyguardViewManager::Runnable7::Runnable7(
    /* [in] */ StatusBarKeyguardViewManager* host)
    : mHost(host)
{}

ECode StatusBarKeyguardViewManager::Runnable7::Run()
{
    mHost->mStatusBarWindowManager->SetKeyguardFadingAway(FALSE);
    mHost->mPhoneStatusBar->FinishKeyguardFadingAway();
    return NOERROR;
}


CAR_INTERFACE_IMPL(StatusBarKeyguardViewManager, Object, IStatusBarKeyguardViewManager)
StatusBarKeyguardViewManager::StatusBarKeyguardViewManager(
    /* [in] */ IContext* context,
    /* [in] */ IViewMediatorCallback* callback,
    /* [in] */ ILockPatternUtils* lockPatternUtils)
    : mScreenOn(FALSE)
    , mShowing(FALSE)
    , mOccluded(FALSE)
    , mFirstUpdate(TRUE)
    , mLastShowing(FALSE)
    , mLastOccluded(FALSE)
    , mLastBouncerShowing(FALSE)
    , mLastBouncerDismissible(FALSE)
{
    mMakeNavigationBarVisibleRunnable = new Runnable2(this);
    mContext = context;
    mViewMediatorCallback = callback;
    mLockPatternUtils = lockPatternUtils;
}

ECode StatusBarKeyguardViewManager::RegisterStatusBar(
    /* [in] */ IPhoneStatusBar* phoneStatusBar,
    /* [in] */ IViewGroup* container,
    /* [in] */ IStatusBarWindowManager* statusBarWindowManager,
    /* [in] */ IScrimController* scrimController)
{
    mPhoneStatusBar = phoneStatusBar;
    mContainer = container;
    mStatusBarWindowManager = statusBarWindowManager;
    mScrimController = scrimController;
    mBouncer = new KeyguardBouncer();
    mBouncer->constructor(mContext, mViewMediatorCallback, mLockPatternUtils,
            mStatusBarWindowManager, container);
    return NOERROR;
}

ECode StatusBarKeyguardViewManager::Show(
    /* [in] */ IBundle* options)
{
    mShowing = TRUE;
    mStatusBarWindowManager->SetKeyguardShowing(TRUE);
    Reset();
    return NOERROR;
}

void StatusBarKeyguardViewManager::ShowBouncerOrKeyguard()
{
    Boolean bval;
    if (mBouncer->NeedsFullscreenBouncer(&bval), bval) {
        // The keyguard might be showing (already). So we need to hide it.
        Boolean tmp = FALSE;
        mPhoneStatusBar->HideKeyguard(&tmp);
        mBouncer->Show();
    }
    else {
        mPhoneStatusBar->ShowKeyguard();
        mBouncer->Hide(FALSE /* destroyView */);
        mBouncer->Prepare();
    }
}

void StatusBarKeyguardViewManager::ShowBouncer()
{
    if (mShowing) {
        mBouncer->Show();
    }
    UpdateStates();
}

ECode StatusBarKeyguardViewManager::DismissWithAction(
    /* [in] */ IKeyguardHostViewOnDismissAction* r,
    /* [in] */ Boolean afterKeyguardGone)
{
    if (mShowing) {
        if (!afterKeyguardGone) {
            mBouncer->ShowWithDismissAction(r);
        }
        else {
            mBouncer->Show();
            mAfterKeyguardGoneAction = r;
        }
    }
    UpdateStates();
    return NOERROR;
}

ECode StatusBarKeyguardViewManager::Reset()
{
    if (mShowing) {
        if (mOccluded) {
            Boolean tmp = FALSE;
            mPhoneStatusBar->HideKeyguard(&tmp);
            mBouncer->Hide(FALSE /* destroyView */);
        }
        else {
            ShowBouncerOrKeyguard();
        }
        UpdateStates();
    }
    return NOERROR;
}

ECode StatusBarKeyguardViewManager::OnScreenTurnedOff()
{
    mScreenOn = FALSE;
    mPhoneStatusBar->OnScreenTurnedOff();
    mBouncer->OnScreenTurnedOff();
    return NOERROR;
}

ECode StatusBarKeyguardViewManager::OnScreenTurnedOn(
    /* [in] */ IIKeyguardShowCallback* callback)
{
    mScreenOn = TRUE;
    mPhoneStatusBar->OnScreenTurnedOn();
    if (callback != NULL) {
        CallbackAfterDraw(callback);
    }
    return NOERROR;
}

void StatusBarKeyguardViewManager::CallbackAfterDraw(
    /* [in] */ IIKeyguardShowCallback* callback)
{
    Boolean tmp = FALSE;
    AutoPtr<Runnable1> run = new Runnable1(this, callback);
    IView::Probe(mContainer)->Post(run, &tmp);
}

ECode StatusBarKeyguardViewManager::VerifyUnlock()
{
    Dismiss();
    return NOERROR;
}

ECode StatusBarKeyguardViewManager::SetNeedsInput(
    /* [in] */ Boolean needsInput)
{
    mStatusBarWindowManager->SetKeyguardNeedsInput(needsInput);
    return NOERROR;
}

ECode StatusBarKeyguardViewManager::UpdateUserActivityTimeout()
{
    Int64 timeout;
    mBouncer->GetUserActivityTimeout(&timeout);
    mStatusBarWindowManager->SetKeyguardUserActivityTimeout(timeout);
    return NOERROR;
}

ECode StatusBarKeyguardViewManager::SetOccluded(
    /* [in] */ Boolean occluded)
{
    if (occluded && !mOccluded && mShowing) {
        Boolean tmp = FALSE;
        if (mPhoneStatusBar->IsInLaunchTransition(&tmp), tmp) {
            mOccluded = TRUE;
            AutoPtr<Runnable4> run = new Runnable4(this);
            mPhoneStatusBar->FadeKeyguardAfterLaunchTransition(NULL /* beforeFading */, run);
            return NOERROR;
        }
    }
    mOccluded = occluded;
    mStatusBarWindowManager->SetKeyguardOccluded(occluded);
    Reset();
    return NOERROR;
}

ECode StatusBarKeyguardViewManager::IsOccluded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOccluded;
    return NOERROR;
}

ECode StatusBarKeyguardViewManager::StartPreHideAnimation(
    /* [in] */ IRunnable* finishRunnable)
{
    Boolean bval;
    if (mBouncer->IsShowing(&bval), bval) {
        mBouncer->StartPreHideAnimation(finishRunnable);
    }
    else if (finishRunnable != NULL) {
        finishRunnable->Run();
    }
    return NOERROR;
}

ECode StatusBarKeyguardViewManager::Hide(
    /* [in] */ Int64 startTime,
    /* [in] */ Int64 fadeoutDuration)
{
    mShowing = FALSE;

    Int64 uptimeMillis = SystemClock::GetUptimeMillis();
    Int64 delay = Elastos::Core::Math::Max((Int64)0, startTime + HIDE_TIMING_CORRECTION_MS - uptimeMillis);

    Boolean tmp = FALSE;
    if (mPhoneStatusBar->IsInLaunchTransition(&tmp), tmp) {
        AutoPtr<Runnable5> run1 = new Runnable5(this);
        AutoPtr<Runnable6> run2 = new Runnable6(this);
        mPhoneStatusBar->FadeKeyguardAfterLaunchTransition(run1, run2);
    }
    else {
        mPhoneStatusBar->SetKeyguardFadingAway(delay, fadeoutDuration);
        Boolean staying = FALSE;
        mPhoneStatusBar->HideKeyguard(&staying);
        if (!staying) {
            mStatusBarWindowManager->SetKeyguardFadingAway(TRUE);
            AutoPtr<Runnable7> run = new Runnable7(this);
            mScrimController->AnimateKeyguardFadingOut(delay, fadeoutDuration, run);
        }
        else {
            mScrimController->AnimateGoingToFullShade(delay, fadeoutDuration);
            mPhoneStatusBar->FinishKeyguardFadingAway();
        }
        mStatusBarWindowManager->SetKeyguardShowing(FALSE);
        mBouncer->Hide(TRUE /* destroyView */);
        mViewMediatorCallback->KeyguardGone();
        ExecuteAfterKeyguardGoneAction();
        UpdateStates();
    }
    return NOERROR;
}

void StatusBarKeyguardViewManager::ExecuteAfterKeyguardGoneAction()
{
    if (mAfterKeyguardGoneAction != NULL) {
        assert(0 && "TODO: Need the app Keyguard.");
        // mAfterKeyguardGoneAction->OnDismiss();
        mAfterKeyguardGoneAction = NULL;
    }
}

ECode StatusBarKeyguardViewManager::Dismiss()
{
    if (mScreenOn) {
        ShowBouncer();
    }
    return NOERROR;
}

ECode StatusBarKeyguardViewManager::IsSecure(
    /* [out] */ Boolean* result)
{
    return mBouncer->IsSecure(result);
}

ECode StatusBarKeyguardViewManager::IsShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mShowing;
    return NOERROR;
}

ECode StatusBarKeyguardViewManager::OnBackPressed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean bval;
    if (mBouncer->IsShowing(&bval), bval) {
        Reset();
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode StatusBarKeyguardViewManager::IsBouncerShowing(
    /* [out] */ Boolean* result)
{
    return mBouncer->IsShowing(result);
}

Int64 StatusBarKeyguardViewManager::GetNavBarShowDelay()
{
    Boolean tmp = FALSE;
    if (mPhoneStatusBar->IsKeyguardFadingAway(&tmp), tmp) {
        Int64 t = 0;
        mPhoneStatusBar->GetKeyguardFadingAwayDelay(&t);
        return t;
    }

    // Keyguard is not going away, thus we are showing the navigation bar because the
    // bouncer is appearing.
    return NAV_BAR_SHOW_DELAY_BOUNCER;
}

void StatusBarKeyguardViewManager::UpdateStates()
{
    Int32 vis = 0;
    IView::Probe(mContainer)->GetSystemUiVisibility(&vis);
    Boolean showing = mShowing;
    Boolean occluded = mOccluded;
    Boolean bouncerShowing, bouncerDismissible;
    mBouncer->IsShowing(&bouncerShowing);
    mBouncer->NeedsFullscreenBouncer(&bouncerDismissible);
    bouncerDismissible = !bouncerDismissible;

    if ((bouncerDismissible || !showing) != (mLastBouncerDismissible || !mLastShowing)
            || mFirstUpdate) {
        if (bouncerDismissible || !showing) {
            IView::Probe(mContainer)->SetSystemUiVisibility(vis & ~IView::STATUS_BAR_DISABLE_BACK);
        }
        else {
            IView::Probe(mContainer)->SetSystemUiVisibility(vis | IView::STATUS_BAR_DISABLE_BACK);
        }
    }
    if ((!(showing && !occluded) || bouncerShowing)
            != (!(mLastShowing && !mLastOccluded) || mLastBouncerShowing) || mFirstUpdate) {
        AutoPtr<INavigationBarView> view;
        mPhoneStatusBar->GetNavigationBarView((INavigationBarView**)&view);
        if (view != NULL) {
            if (!(showing && !occluded) || bouncerShowing) {
                IView::Probe(mContainer)->PostOnAnimationDelayed(mMakeNavigationBarVisibleRunnable,
                        GetNavBarShowDelay());
            }
            else {
                Boolean tmp = FALSE;
                IView::Probe(mContainer)->RemoveCallbacks(mMakeNavigationBarVisibleRunnable, &tmp);
                IView::Probe(view)->SetVisibility(IView::GONE);
            }
        }
    }

    if (bouncerShowing != mLastBouncerShowing || mFirstUpdate) {
        mStatusBarWindowManager->SetBouncerShowing(bouncerShowing);
        IBaseStatusBar::Probe(mPhoneStatusBar)->SetBouncerShowing(bouncerShowing);
        mScrimController->SetBouncerShowing(bouncerShowing);
    }

    Slogger::D(TAG, "TODO [UpdateStates] : Need the app Keyguard.");
    AutoPtr<IKeyguardUpdateMonitor> updateMonitor;
    // KeyguardUpdateMonitor updateMonitor = KeyguardUpdateMonitor.getInstance(mContext);
    if (updateMonitor != NULL) {
        if ((showing && !occluded) != (mLastShowing && !mLastOccluded) || mFirstUpdate) {
            updateMonitor->SendKeyguardVisibilityChanged(showing && !occluded);
        }
        if (bouncerShowing != mLastBouncerShowing || mFirstUpdate) {
            updateMonitor->SendKeyguardBouncerChanged(bouncerShowing);
        }
    }

    mFirstUpdate = FALSE;
    mLastShowing = showing;
    mLastOccluded = occluded;
    mLastBouncerShowing = bouncerShowing;
    mLastBouncerDismissible = bouncerDismissible;
}

ECode StatusBarKeyguardViewManager::OnMenuPressed(
    /* [out] */ Boolean* result)
{
    return mBouncer->OnMenuPressed(result);
}

ECode StatusBarKeyguardViewManager::InterceptMediaKey(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    return mBouncer->InterceptMediaKey(event, result);
}

ECode StatusBarKeyguardViewManager::OnActivityDrawn()
{
    Boolean tmp = FALSE;
    if (mPhoneStatusBar->IsCollapsing(&tmp), tmp) {
        AutoPtr<Runnable3> run = new Runnable3(this);
        mPhoneStatusBar->AddPostCollapseAction(run);
    }
    else {
        mViewMediatorCallback->ReadyForKeyguardDone();
    }
    return NOERROR;
}

ECode StatusBarKeyguardViewManager::ShouldDisableWindowAnimationsForUnlock(
    /* [out] */ Boolean* result)
{
    return mPhoneStatusBar->IsInLaunchTransition(result);
}

ECode StatusBarKeyguardViewManager::IsGoingToNotificationShade(
    /* [out] */ Boolean* result)
{
    return mPhoneStatusBar->IsGoingToNotificationShade(result);
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
