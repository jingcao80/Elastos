
#include "elastos/droid/server/wm/WindowAnimator.h"
#include "elastos/droid/server/wm/AppWindowAnimator.h"
#include "elastos/droid/server/wm/ScreenRotationAnimation.h"
#include "elastos/droid/server/wm/AccessibilityController.h"
#include "elastos/droid/server/wm/DisplayContent.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::ISurfaceControlHelper;
using Elastos::Droid::View::CSurfaceControlHelper;
using Elastos::Droid::View::Animation::IAlphaAnimation;
using Elastos::Droid::View::Animation::CAlphaAnimation;
using Elastos::Droid::Utility::ISparseInt32Array;
using Elastos::Droid::Utility::CSparseInt32Array;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

//==============================================================================
//                  WindowAnimator::AnimationRunnable
//==============================================================================

ECode WindowAnimator::AnimationRunnable::Run()
{
    AutoLock lock(mHost->mService->mWindowMapLock);
    mHost->mService->mAnimationScheduled = FALSE;
    mHost->AnimateLocked();
    return NOERROR;
}


//==============================================================================
//                  WindowAnimator
//==============================================================================

const String WindowAnimator::TAG("WindowAnimator");
const Int64 WindowAnimator::KEYGUARD_ANIM_TIMEOUT_MS;
const Int32 WindowAnimator::KEYGUARD_NOT_SHOWN;
const Int32 WindowAnimator::KEYGUARD_ANIMATING_IN;
const Int32 WindowAnimator::KEYGUARD_SHOWN;
const Int32 WindowAnimator::KEYGUARD_ANIMATING_OUT;

WindowAnimator::WindowAnimator(
    /* [in] */ CWindowManagerService* service)
    : mService(service)
    , mAnimating(FALSE)
    , mCurrentTime(0)
    , mAboveUniverseLayer(0)
    , mBulkUpdateParams(0)
    , mInitialized(FALSE)
    , mKeyguardGoingAway(FALSE)
    , mKeyguardGoingAwayToNotificationShade(FALSE)
    , mKeyguardGoingAwayDisableWindowAnimations(FALSE)
    , mForceHiding(KEYGUARD_NOT_SHOWN)
    , mAnimTransactionSequence(0)
    , mBlurUiEnabled(FALSE)
{
    mContext = service->mContext;
    mPolicy = service->mPolicy;
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetBoolean(Elastos::Droid::R::bool_::config_ui_blur_enabled, &mBlurUiEnabled);
    mAnimationRunnable = new AnimationRunnable(this);
}

String WindowAnimator::ForceHidingToString()
{
    switch (mForceHiding) {
        case KEYGUARD_NOT_SHOWN:    return String("KEYGUARD_NOT_SHOWN");
        case KEYGUARD_ANIMATING_IN: return String("KEYGUARD_ANIMATING_IN");
        case KEYGUARD_SHOWN:        return String("KEYGUARD_SHOWN");
        case KEYGUARD_ANIMATING_OUT:return String("KEYGUARD_ANIMATING_OUT");
        default: return String("KEYGUARD STATE UNKNOWN ") + StringUtils::ToString(mForceHiding);
    }
}

void WindowAnimator::AddDisplayLocked(
    /* [in] */ Int32 displayId)
{
    // Create the DisplayContentsAnimator object by retrieving it.
    GetDisplayContentsAnimatorLocked(displayId);
    if (displayId == IDisplay::DEFAULT_DISPLAY) {
        mInitialized = TRUE;
    }
}

void WindowAnimator::RemoveDisplayLocked(
    /* [in] */ Int32 displayId)
{
    AutoPtr<DisplayContentsAnimator> displayAnimator;
    HashMap<Int32, AutoPtr<DisplayContentsAnimator> >::Iterator it
            =  mDisplayContentsAnimators.Find(displayId);
    if (it != mDisplayContentsAnimators.End()) {
        displayAnimator = it->mSecond;
        if (displayAnimator != NULL) {
            if (displayAnimator->mScreenRotationAnimation != NULL) {
                displayAnimator->mScreenRotationAnimation->Kill();
                displayAnimator->mScreenRotationAnimation = NULL;
            }
        }

        mDisplayContentsAnimators.Erase(it);
    }
}

void WindowAnimator::HideWallpapersLocked(
    /* [in] */ WindowState* w)
{
    AutoPtr<WindowState> wallpaperTarget = mService->mWallpaperTarget;
    AutoPtr<WindowState> lowerWallpaperTarget = mService->mLowerWallpaperTarget;
    List< AutoPtr<WindowToken> > wallpaperTokens = mService->mWallpaperTokens;
    if ((wallpaperTarget.Get() == w && lowerWallpaperTarget == NULL) || wallpaperTarget == NULL) {
        List<AutoPtr<WindowToken> >::ReverseIterator rit = wallpaperTokens.RBegin();
        for (; rit != wallpaperTokens.REnd(); ++rit) {
            AutoPtr<WindowToken> token = *rit;
            Int32 numWindows;
            token->mWindows->GetSize(&numWindows);
            for (Int32 j = numWindows - 1; j >= 0; j--) {
                AutoPtr<IInterface> obj;
                token->mWindows->Get(j, (IInterface**)&obj);
                AutoPtr<WindowState> wallpaper = To_WindowState(obj);
                AutoPtr<WindowStateAnimator> winAnimator = wallpaper->mWinAnimator;
                if (!winAnimator->mLastHidden) {
                    winAnimator->Hide();
                    mService->DispatchWallpaperVisibility(wallpaper, FALSE);
                    SetPendingLayoutChanges(IDisplay::DEFAULT_DISPLAY,
                            IWindowManagerPolicy::FINISH_LAYOUT_REDO_WALLPAPER);
                }
            }
            // if (WindowManagerService.DEBUG_WALLPAPER_LIGHT && !token.hidden) Slog.d(TAG,
            //         "Hiding wallpaper " + token + " from " + w
            //         + " target=" + wallpaperTarget + " lower=" + lowerWallpaperTarget
            //         + "\n" + Debug.getCallers(5, "  "));
            token->mHidden = TRUE;
        }
    }
}

void WindowAnimator::UpdateAppWindowsLocked(
    /* [in] */ Int32 displayId)
{
    List< AutoPtr<TaskStack> >& stacks = mService->GetDisplayContentLocked(displayId)->GetStacks();
    List<AutoPtr<TaskStack> >::ReverseIterator stackRit = stacks.RBegin();
    for (; stackRit != stacks.REnd(); ++stackRit) {
        AutoPtr<TaskStack> stack = *stackRit;
        List< AutoPtr<Task> >& tasks = stack->GetTasks();
        List<AutoPtr<Task> >::ReverseIterator taskRit = tasks.RBegin();
        for (; taskRit != tasks.REnd(); ++taskRit) {
            AppTokenList tokens = (*taskRit)->mAppTokens;
            AppTokenList::ReverseIterator tokenRit = tokens.RBegin();
            for (; tokenRit != tokens.REnd(); ++tokenRit) {
                AutoPtr<AppWindowAnimator> appAnimator = (*tokenRit)->mAppAnimator;
                Boolean wasAnimating = appAnimator->mAnimation != NULL
                        && appAnimator->mAnimation != AppWindowAnimator::sDummyAnimation;
                if (appAnimator->StepAnimationLocked(mCurrentTime)) {
                    mAnimating = TRUE;
                }
                else if (wasAnimating) {
                    // stopped animating, do one more pass through the layout
                    AutoPtr<IInterface> obj;
                    appAnimator->mWeakAppToken->Resolve(EIID_IInterface, (IInterface**)&obj);
                    String str;
                    ((AppWindowToken*)(IObject*)obj.Get())->ToString(&str);
                    SetAppLayoutChanges(appAnimator,
                            IWindowManagerPolicy::FINISH_LAYOUT_REDO_WALLPAPER,
                            String("appToken ") + str + " done");
                    if (CWindowManagerService::DEBUG_ANIM) Slogger::V(TAG,
                            "updateWindowsApps...: done animating %p", str.string());
                }
            }
        }

        AppTokenList& exitingAppTokens = stack->mExitingAppTokens;
        AppTokenList::Iterator tokenIt = exitingAppTokens.Begin();
        for (; tokenIt != exitingAppTokens.End(); ++tokenIt) {
            AutoPtr<AppWindowAnimator> appAnimator = (*tokenIt)->mAppAnimator;
            Boolean wasAnimating = appAnimator->mAnimation != NULL
                    && appAnimator->mAnimation != AppWindowAnimator::sDummyAnimation;
            if (appAnimator->StepAnimationLocked(mCurrentTime)) {
                mAnimating = TRUE;
            }
            else if (wasAnimating) {
                // stopped animating, do one more pass through the layout
                AutoPtr<IInterface> obj;
                appAnimator->mWeakAppToken->Resolve(EIID_IInterface, (IInterface**)&obj);
                String str;
                ((AppWindowToken*)(IObject*)obj.Get())->ToString(&str);
                SetAppLayoutChanges(appAnimator, IWindowManagerPolicy::FINISH_LAYOUT_REDO_WALLPAPER,
                        String("exiting appToken ") + str + " done");
                if (CWindowManagerService::DEBUG_ANIM) Slogger::V(TAG,
                        "updateWindowsApps...: done animating exiting %p", str.string());
            }
        }
    }
}

void WindowAnimator::UpdateWindowsLocked(
    /* [in] */ Int32 displayId)
{
    ++mAnimTransactionSequence;
    AutoPtr<WindowList> windows = mService->GetWindowListLocked(displayId);
    List<AutoPtr<WindowStateAnimator> > unForceHiding;
    Boolean wallpaperInUnForceHiding = FALSE;
    AutoPtr<WindowState> wallpaper;

    if (mKeyguardGoingAway) {
        Int32 N;
        windows->GetSize(&N);
        for (Int32 i = N - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            windows->Get(i, (IInterface**)&obj);
            WindowState* win = To_WindowState(obj);
            Boolean isKeyguardHostWindow;
            if (mPolicy->IsKeyguardHostWindow(win->mAttrs, &isKeyguardHostWindow), !isKeyguardHostWindow) {
                continue;
            }
            AutoPtr<WindowStateAnimator> winAnimator = win->mWinAnimator;
            Int32 privateFlags;
            if (win->mAttrs->GetPrivateFlags(&privateFlags), (privateFlags & IWindowManagerLayoutParams::PRIVATE_FLAG_KEYGUARD) != 0) {
                if (!winAnimator->mAnimating) {
                    // Create a new animation to delay until keyguard is gone on its own.
                    winAnimator->mAnimation = NULL;
                    CAlphaAnimation::New(1.0f, 1.0f, (IAlphaAnimation**)&(winAnimator->mAnimation));
                    winAnimator->mAnimation->SetDuration(mBlurUiEnabled ? 0 : KEYGUARD_ANIM_TIMEOUT_MS);
                    winAnimator->mAnimationIsEntrance = FALSE;
                }
            }
            else {
                mKeyguardGoingAway = FALSE;
                winAnimator->ClearAnimation();
            }
            break;
        }
    }

    mForceHiding = KEYGUARD_NOT_SHOWN;

    AutoPtr<WindowState> imeTarget = mService->mInputMethodTarget;
    AutoPtr<IWindowManagerLayoutParams> lp;
    if (imeTarget) {
        imeTarget->GetAttrs((IWindowManagerLayoutParams**)&lp);
    }
    Int32 flags;
    Boolean showImeOverKeyguard = imeTarget != NULL && imeTarget->IsVisibleNow() &&
            (lp->GetFlags(&flags), (flags & IWindowManagerLayoutParams::FLAG_SHOW_WHEN_LOCKED) != 0);
    AutoPtr<IWindowState> ws;
    mPolicy->GetWinShowWhenLockedLw((IWindowState**)&ws);
    AutoPtr<WindowState> winShowWhenLocked = (WindowState*)ws.Get();
    AutoPtr<AppWindowToken> appShowWhenLocked = winShowWhenLocked == NULL ? NULL : winShowWhenLocked->mAppToken;

    Int32 N;
    windows->GetSize(&N);
    for (Int32 i = N - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        windows->Get(i, (IInterface**)&obj);
        AutoPtr<WindowState> win = To_WindowState(obj);
        AutoPtr<WindowStateAnimator> winAnimator = win->mWinAnimator;
        Int32 flags;
        win->mAttrs->GetFlags(&flags);

        if (winAnimator->mSurfaceControl != NULL) {
            Boolean wasAnimating = winAnimator->mWasAnimating;
            Boolean nowAnimating = winAnimator->StepAnimationLocked(mCurrentTime);

            if (CWindowManagerService::DEBUG_WALLPAPER) {
                Slogger::V(TAG, "%s: wasAnimating=%d, nowAnimating=%d", TO_CSTR(win), wasAnimating, nowAnimating);
            }

            if (wasAnimating && !winAnimator->mAnimating && mService->mWallpaperTarget == win) {
                mBulkUpdateParams |= CWindowManagerService::LayoutFields::SET_WALLPAPER_MAY_CHANGE;
                SetPendingLayoutChanges(IDisplay::DEFAULT_DISPLAY,
                        IWindowManagerPolicy::FINISH_LAYOUT_REDO_WALLPAPER);
                if (CWindowManagerService::DEBUG_LAYOUT_REPEATS) {
                    mService->DebugLayoutRepeats(String("updateWindowsAndWallpaperLocked 2"),
                            GetPendingLayoutChanges(IDisplay::DEFAULT_DISPLAY));
                }
            }

            Boolean isForceHiding;
            if (mPolicy->IsForceHiding(win->mAttrs, &isForceHiding), isForceHiding) {
                if (!wasAnimating && nowAnimating) {
                    // if (WindowManagerService.DEBUG_ANIM ||
                    //         WindowManagerService.DEBUG_VISIBILITY) Slog.v(TAG,
                    //         "Animation started that could impact force hide: " + win);
                    mBulkUpdateParams |= CWindowManagerService::LayoutFields::SET_FORCE_HIDING_CHANGED;
                    SetPendingLayoutChanges(displayId, IWindowManagerPolicy::FINISH_LAYOUT_REDO_WALLPAPER);
                    if (CWindowManagerService::DEBUG_LAYOUT_REPEATS) {
                        mService->DebugLayoutRepeats(String("updateWindowsAndWallpaperLocked 3"),
                                GetPendingLayoutChanges(displayId));
                    }
                    mService->mFocusMayChange = TRUE;
                }
                else if (mKeyguardGoingAway && !nowAnimating) {
                    // Timeout!!
                    Slogger::E(TAG, "Timeout waiting for animation to startup");
                    mPolicy->StartKeyguardExitAnimation(0, 0);
                    mKeyguardGoingAway = FALSE;
                }
                if (win->IsReadyForDisplay()) {
                    if (mBlurUiEnabled) {
                        mForceHiding = KEYGUARD_NOT_SHOWN;
                    }
                    else {
                        if (nowAnimating) {
                            if (winAnimator->mAnimationIsEntrance) {
                                mForceHiding = KEYGUARD_ANIMATING_IN;
                            }
                            else {
                                mForceHiding = KEYGUARD_ANIMATING_OUT;
                            }
                        }
                        else {
                            mForceHiding = win->IsDrawnLw() ? KEYGUARD_SHOWN : KEYGUARD_NOT_SHOWN;
                        }
                    }
                }
                // if (WindowManagerService.DEBUG_VISIBILITY) Slog.v(TAG,
                //         "Force hide " + mForceHiding
                //         + " hasSurface=" + win.mHasSurface
                //         + " policyVis=" + win.mPolicyVisibility
                //         + " destroying=" + win.mDestroying
                //         + " attHidden=" + win.mAttachedHidden
                //         + " vis=" + win.mViewVisibility
                //         + " hidden=" + win.mRootToken.hidden
                //         + " anim=" + win.mWinAnimator.mAnimation);
            }
            else {
                Boolean result;
                mPolicy->CanBeForceHidden(win, win->mAttrs, &result);
                if (result) {
                    Boolean hideWhenLocked = !((win->mIsImWindow && showImeOverKeyguard) ||
                            (appShowWhenLocked != NULL && appShowWhenLocked.Get() == win->mAppToken));
                    Boolean changed;
                    if (((mForceHiding == KEYGUARD_ANIMATING_IN)
                                && (!winAnimator->IsAnimating() || hideWhenLocked))
                            || ((mForceHiding == KEYGUARD_SHOWN) && hideWhenLocked)) {
                        changed = win->HideLw(FALSE, FALSE);
                        // if (WindowManagerService.DEBUG_VISIBILITY && changed) Slog.v(TAG,
                        //         "Now policy hidden: " + win);
                    }
                    else {
                        changed = win->ShowLw(FALSE, FALSE);
                        // if (WindowManagerService.DEBUG_VISIBILITY && changed) Slog.v(TAG,
                        //         "Now policy shown: " + win);
                        if (changed) {
                            if ((mBulkUpdateParams & CWindowManagerService::LayoutFields::SET_FORCE_HIDING_CHANGED) != 0
                                    && win->IsVisibleNow() /*w.isReadyForDisplay()*/) {
                                unForceHiding.PushBack(winAnimator);
                                if ((flags & IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER) != 0) {
                                    wallpaperInUnForceHiding = TRUE;
                                }
                            }
                            AutoPtr<WindowState> currentFocus = mService->mCurrentFocus;
                            if (currentFocus == NULL || currentFocus->mLayer < win->mLayer) {
                                // We are showing on to of the current
                                // focus, so re-evaluate focus to make
                                // sure it is correct.
                                if (CWindowManagerService::DEBUG_FOCUS_LIGHT) Slogger::V(TAG,
                                        "updateWindowsLocked: setting mFocusMayChange true");
                                mService->mFocusMayChange = TRUE;
                            }
                        }
                    }
                    if (changed && (flags & IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER) != 0) {
                        mBulkUpdateParams |= CWindowManagerService::LayoutFields::SET_WALLPAPER_MAY_CHANGE;
                        SetPendingLayoutChanges(IDisplay::DEFAULT_DISPLAY,
                                IWindowManagerPolicy::FINISH_LAYOUT_REDO_WALLPAPER);
                        if (CWindowManagerService::DEBUG_LAYOUT_REPEATS) {
                            mService->DebugLayoutRepeats(String("updateWindowsAndWallpaperLocked 4"),
                                    GetPendingLayoutChanges(IDisplay::DEFAULT_DISPLAY));
                        }
                    }
                }
            }
        }

        AutoPtr<AppWindowToken> atoken = win->mAppToken;
        if (winAnimator->mDrawState == WindowStateAnimator::READY_TO_SHOW) {
            if (atoken == NULL || atoken->mAllDrawn) {
                if (winAnimator->PerformShowLocked()) {
                    SetPendingLayoutChanges(displayId, IWindowManagerPolicy::FINISH_LAYOUT_REDO_ANIM);
                    if (CWindowManagerService::DEBUG_LAYOUT_REPEATS) {
                        mService->DebugLayoutRepeats(String("updateWindowsAndWallpaperLocked 5"),
                                GetPendingLayoutChanges(displayId));
                    }
                }
            }
        }
        AutoPtr<AppWindowAnimator> appAnimator = winAnimator->mAppAnimator;
        if (appAnimator != NULL && appAnimator->mThumbnail != NULL) {
            if (appAnimator->mThumbnailTransactionSeq != mAnimTransactionSequence) {
                appAnimator->mThumbnailTransactionSeq = mAnimTransactionSequence;
                appAnimator->mThumbnailLayer = 0;
            }
            if (appAnimator->mThumbnailLayer < winAnimator->mAnimLayer) {
                appAnimator->mThumbnailLayer = winAnimator->mAnimLayer;
            }
        }
        if (win->mIsWallpaper) {
            wallpaper = win;
        }
    } // end forall windows

    // If we have windows that are being show due to them no longer
    // being force-hidden, apply the appropriate animation to them.
    if (unForceHiding.Begin() != unForceHiding.End()) {
        Boolean startKeyguardExit = TRUE;
        List<AutoPtr<WindowStateAnimator> >::ReverseIterator animRIt = unForceHiding.RBegin();
        for (; animRIt != unForceHiding.REnd(); ++animRIt) {
            AutoPtr<IAnimation> a;
            if (!mKeyguardGoingAwayDisableWindowAnimations) {
                mPolicy->CreateForceHideEnterAnimation(wallpaperInUnForceHiding,
                        mKeyguardGoingAwayToNotificationShade, (IAnimation**)&a);
            }
            if (a != NULL) {
                AutoPtr<WindowStateAnimator> winAnimator = *animRIt;
                winAnimator->SetAnimation(a);
                winAnimator->mKeyguardGoingAwayAnimation = TRUE;
                if (startKeyguardExit && mKeyguardGoingAway) {
                    // Do one time only.
                    Int64 offset, duration;
                    a->GetStartOffset(&offset);
                    a->GetDuration(&duration);
                    mPolicy->StartKeyguardExitAnimation(mCurrentTime + offset, duration);
                    mKeyguardGoingAway = FALSE;
                    startKeyguardExit = FALSE;
                }
            }
        }

        // Wallpaper is going away in un-force-hide motion, animate it as well.
        if (!wallpaperInUnForceHiding && wallpaper != NULL
                && !mKeyguardGoingAwayDisableWindowAnimations) {
            AutoPtr<IAnimation> a;
            mPolicy->CreateForceHideWallpaperExitAnimation(mKeyguardGoingAwayToNotificationShade,
                    mKeyguardGoingAwayShowingMedia, (IAnimation**)&a);
            if (a != NULL) {
                AutoPtr<WindowStateAnimator> animator = wallpaper->mWinAnimator;
                animator->SetAnimation(a);
            }
        }
    }
}

void WindowAnimator::UpdateWallpaperLocked(
    /* [in] */ Int32 displayId)
{
    mService->GetDisplayContentLocked(displayId)->ResetAnimationBackgroundAnimator();

    AutoPtr<WindowList> windows = mService->GetWindowListLocked(displayId);
    AutoPtr<WindowState> detachedWallpaper;

    Int32 N;
    windows->GetSize(&N);
    for (Int32 i = N - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        windows->Get(i, (IInterface**)&obj);
        WindowState* win = To_WindowState(obj);
        AutoPtr<WindowStateAnimator> winAnimator = win->mWinAnimator;
        if (winAnimator->mSurfaceControl == NULL) {
            continue;
        }

        Int32 flags;
        win->mAttrs->GetFlags(&flags);

        // If this window is animating, make a note that we have
        // an animating window and take care of a request to run
        // a detached wallpaper animation.
        if (winAnimator->mAnimating) {
            if (winAnimator->mAnimation != NULL) {
                Boolean value;
                if ((flags & IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER) != 0
                        && (winAnimator->mAnimation->GetDetachWallpaper(&value), value)) {
                    detachedWallpaper = win;
                }
                Int32 color;
                winAnimator->mAnimation->GetBackgroundColor(&color);
                if (color != 0) {
                    AutoPtr<TaskStack> stack = win->GetStack();
                    if (stack != NULL) {
                        stack->SetAnimationBackground(winAnimator, color);
                    }
                }
            }
            mAnimating = TRUE;
        }

        // If this window's app token is running a detached wallpaper
        // animation, make a note so we can ensure the wallpaper is
        // displayed behind it.
        AutoPtr<AppWindowAnimator> appAnimator = winAnimator->mAppAnimator;
        if (appAnimator != NULL && appAnimator->mAnimation != NULL
                && appAnimator->mAnimating) {
            Boolean value;
            if ((flags & IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER) != 0
                    && (appAnimator->mAnimation->GetDetachWallpaper(&value), value)) {
                detachedWallpaper = win;
            }

            Int32 color;
            appAnimator->mAnimation->GetBackgroundColor(&color);
            if (color != 0) {
                AutoPtr<TaskStack> stack = win->GetStack();
                if (stack != NULL) {
                    stack->SetAnimationBackground(winAnimator, color);
                }
            }
        }
    } // end forall windows

    if (mWindowDetachedWallpaper != detachedWallpaper) {
        // if (WindowManagerService.DEBUG_WALLPAPER) Slog.v(TAG,
        //         "Detached wallpaper changed from " + mWindowDetachedWallpaper
        //         + " to " + detachedWallpaper);
        mWindowDetachedWallpaper = detachedWallpaper;
        mBulkUpdateParams |= CWindowManagerService::LayoutFields::SET_WALLPAPER_MAY_CHANGE;
    }
}

void WindowAnimator::TestTokenMayBeDrawnLocked(
    /* [in] */ Int32 displayId)
{
    // See if any windows have been drawn, so they (and others
    // associated with them) can now be shown.
    List< AutoPtr<Task> >& tasks = mService->GetDisplayContentLocked(displayId)->GetTasks();
    List<AutoPtr<Task> >::Iterator it = tasks.Begin();
    for (; it != tasks.End(); ++it) {
        AppTokenList tokens = (*it)->mAppTokens;
        AppTokenList::Iterator tokenIt = tokens.Begin();
        for (; tokenIt != tokens.End(); ++tokenIt) {
            AutoPtr<AppWindowToken> wtoken = *tokenIt;
            AutoPtr<AppWindowAnimator> appAnimator = wtoken->mAppAnimator;
            Boolean allDrawn = wtoken->mAllDrawn;
            if (allDrawn != appAnimator->mAllDrawn) {
                appAnimator->mAllDrawn = allDrawn;
                if (allDrawn) {
                    // The token has now changed state to having all
                    // windows shown...  what to do, what to do?
                    if (appAnimator->mFreezingScreen) {
                        appAnimator->ShowAllWindowsLocked();
                        mService->UnsetAppFreezingScreenLocked(wtoken, FALSE, TRUE);
                        // if (WindowManagerService.DEBUG_ORIENTATION) Slogger::I(TAG,
                        //         "Setting mOrientationChangeComplete=true because wtoken "
                        //         + wtoken + " numInteresting=" + wtoken.numInterestingWindows
                        //         + " numDrawn=" + wtoken.numDrawnWindows);
                        // This will set mOrientationChangeComplete and cause a pass through layout.
                        SetAppLayoutChanges(appAnimator,
                            IWindowManagerPolicy::FINISH_LAYOUT_REDO_WALLPAPER,
                            String("testTokenMayBeDrawnLocked: freezingScreen"));
                    }
                    else {
                        SetAppLayoutChanges(appAnimator,
                            IWindowManagerPolicy::FINISH_LAYOUT_REDO_ANIM,
                            String("testTokenMayBeDrawnLocked"));

                        // We can now show all of the drawn windows!
                        List<AutoPtr<AppWindowToken> >::Iterator awtIt;
                        awtIt = Find(mService->mOpeningApps.Begin(), mService->mOpeningApps.End(), wtoken);
                        if (awtIt == mService->mOpeningApps.End()) {
                            mAnimating |= appAnimator->ShowAllWindowsLocked();
                        }
                    }
                }
            }
        }
    }
}

void WindowAnimator::PerformAnimationsLocked(
    /* [in] */ Int32 displayId)
{
    UpdateWindowsLocked(displayId);
    UpdateWallpaperLocked(displayId);
}

void WindowAnimator::AnimateLocked()
{
    if (!mInitialized) {
        return;
    }

    mCurrentTime = SystemClock::GetUptimeMillis();
    mBulkUpdateParams = CWindowManagerService::LayoutFields::SET_ORIENTATION_CHANGE_COMPLETE;
    Boolean wasAnimating = mAnimating;
    mAnimating = FALSE;
    // if (CWindowManagerService::DEBUG_WINDOW_TRACE) {
    //     Slogger::I(TAG, "!!! animate: entry time=%lld", mCurrentTime);
    // }

    // if (CWindowManagerService::SHOW_TRANSACTIONS) {
    //     Slogger::I(TAG, ">>> OPEN TRANSACTION animateLocked");
    // }
    AutoPtr<ISurfaceControlHelper> helper;
    CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&helper);
    helper->OpenTransaction();
    helper->SetAnimationTransaction();
    // try {
    HashMap<Int32, AutoPtr<DisplayContentsAnimator> >::Iterator it;
    for (it = mDisplayContentsAnimators.Begin(); it != mDisplayContentsAnimators.End(); ++it) {
        Int32 displayId = it->mFirst;
        UpdateAppWindowsLocked(displayId);
        AutoPtr<DisplayContentsAnimator> displayAnimator = it->mSecond;

        AutoPtr<ScreenRotationAnimation> screenRotationAnimation =
                displayAnimator->mScreenRotationAnimation;
        if (screenRotationAnimation != NULL && screenRotationAnimation->IsAnimating()) {
            if (screenRotationAnimation->StepAnimationLocked(mCurrentTime)) {
                mAnimating = TRUE;
            }
            else {
                mBulkUpdateParams |= CWindowManagerService::LayoutFields::SET_UPDATE_ROTATION;
                screenRotationAnimation->Kill();
                displayAnimator->mScreenRotationAnimation = NULL;

                //TODO (multidisplay): Accessibility supported only for the default display.
                if (mService->mAccessibilityController != NULL
                        && displayId == IDisplay::DEFAULT_DISPLAY) {
                    // We just finished rotation animation which means we did not
                    // anounce the rotation and waited for it to end, announce now.
                    AutoPtr<DisplayContent> dc = mService->GetDefaultDisplayContentLocked();
                    mService->mAccessibilityController->OnRotationChangedLocked(dc, mService->mRotation);
                }
            }
        }

        // Update animations of all applications, including those
        // associated with exiting/removed apps
        UpdateWindowsLocked(displayId);
        UpdateWallpaperLocked(displayId);

        AutoPtr<WindowList> windows = mService->GetWindowListLocked(displayId);
        Int32 N;
        windows->GetSize(&N);
        for (Int32 j = 0; j < N; j++) {
            AutoPtr<IInterface> obj;
            windows->Get(j, (IInterface**)&obj);
            To_WindowState(obj)->mWinAnimator->PrepareSurfaceLocked(TRUE);
        }
    }

    for (it = mDisplayContentsAnimators.Begin(); it != mDisplayContentsAnimators.End(); ++it) {
        Int32 displayId = it->mFirst;
        TestTokenMayBeDrawnLocked(displayId);

        AutoPtr<ScreenRotationAnimation> screenRotationAnimation = it->mSecond->mScreenRotationAnimation;
        if (screenRotationAnimation != NULL) {
            screenRotationAnimation->UpdateSurfacesInTransaction();
        }

        mAnimating |= mService->GetDisplayContentLocked(displayId)->AnimateDimLayers();
        mAnimating |= mService->GetDisplayContentLocked(displayId)->AnimateBlurLayers();

        //TODO (multidisplay): Magnification is supported only for the default display.
        if (mService->mAccessibilityController != NULL
                && displayId == IDisplay::DEFAULT_DISPLAY) {
            mService->mAccessibilityController->DrawMagnifiedRegionBorderIfNeededLocked();
        }
    }

    if (mAnimating) {
        mService->ScheduleAnimationLocked();
    }

    mService->SetFocusedStackLayer();

    if (mService->mWatermark != NULL) {
        mService->mWatermark->DrawIfNeeded();
    }
    // } catch (RuntimeException e) {
    //     Log.wtf(TAG, "Unhandled exception in Window Manager", e);
    // } finally {
    helper->CloseTransaction();
    // if (CWindowManagerService::SHOW_TRANSACTIONS) {
    //     Slogger::I(TAG, "<<< CLOSE TRANSACTION animateLocked");
    // }
    // }

    Boolean hasPendingLayoutChanges = FALSE;
    Int32 numDisplays;
    mService->mDisplayContents->GetSize(&numDisplays);
    for (Int32 displayNdx = 0; displayNdx < numDisplays; ++displayNdx) {
        AutoPtr<IInterface> value;
        mService->mDisplayContents->ValueAt(displayNdx, (IInterface**)&value);
        AutoPtr<DisplayContent> displayContent = (DisplayContent*)(IObject*)value.Get();
        Int32 pendingChanges = GetPendingLayoutChanges(displayContent->GetDisplayId());
        if ((pendingChanges & IWindowManagerPolicy::FINISH_LAYOUT_REDO_WALLPAPER) != 0) {
            mBulkUpdateParams |= CWindowManagerService::LayoutFields::SET_WALLPAPER_ACTION_PENDING;
        }
        if (pendingChanges != 0) {
            hasPendingLayoutChanges = TRUE;
        }
    }

    Boolean doRequest = FALSE;
    if (mBulkUpdateParams != 0) {
        doRequest = mService->CopyAnimToLayoutParamsLocked();
    }

    if (hasPendingLayoutChanges || doRequest) {
        mService->RequestTraversalLocked();
    }

    if (!mAnimating && wasAnimating) {
        mService->RequestTraversalLocked();
    }
    // if (CWindowManagerService::DEBUG_WINDOW_TRACE) {
    //     Slogger::I(TAG, "!!! animate: exit mAnimating=%d mBulkUpdateParams=%d mPendingLayoutChanges(DEFAULT_DISPLAY)=%d",
    //             mAnimating, mBulkUpdateParams, GetPendingLayoutChanges(IDisplay::DEFAULT_DISPLAY));
    // }
}

String WindowAnimator::BulkUpdateParamsToString(
    /* [in] */ Int32 bulkUpdateParams)
{
    StringBuilder builder(128);
    if ((bulkUpdateParams & CWindowManagerService::LayoutFields::SET_UPDATE_ROTATION) != 0) {
        builder.Append(" UPDATE_ROTATION");
    }
    if ((bulkUpdateParams & CWindowManagerService::LayoutFields::SET_WALLPAPER_MAY_CHANGE) != 0) {
        builder.Append(" WALLPAPER_MAY_CHANGE");
    }
    if ((bulkUpdateParams & CWindowManagerService::LayoutFields::SET_FORCE_HIDING_CHANGED) != 0) {
        builder.Append(" FORCE_HIDING_CHANGED");
    }
    if ((bulkUpdateParams & CWindowManagerService::LayoutFields::SET_ORIENTATION_CHANGE_COMPLETE) != 0) {
        builder.Append(" ORIENTATION_CHANGE_COMPLETE");
    }
    if ((bulkUpdateParams & CWindowManagerService::LayoutFields::SET_TURN_ON_SCREEN) != 0) {
        builder.Append(" TURN_ON_SCREEN");
    }
    String result;
    builder.ToString(&result);
    return result;
}

Int32 WindowAnimator::GetPendingLayoutChanges(
    /* [in] */ Int32 displayId)
{
    if (displayId < 0) {
        return 0;
    }
    return mService->GetDisplayContentLocked(displayId)->mPendingLayoutChanges;
}

void WindowAnimator::SetPendingLayoutChanges(
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 changes)
{
    if (displayId >= 0) {
        mService->GetDisplayContentLocked(displayId)->mPendingLayoutChanges |= changes;
    }
}

void WindowAnimator::SetAppLayoutChanges(
    /* [in] */ AppWindowAnimator* appAnimator,
    /* [in] */ Int32 changes,
    /* [in] */ const String& s)
{
    // Used to track which displays layout changes have been done.
    AutoPtr<ISparseInt32Array> displays;
    CSparseInt32Array::New(2, (ISparseInt32Array**)&displays);
    AutoPtr<IInterface> obj;
    appAnimator->mWeakAppToken->Resolve(EIID_IInterface, (IInterface**)&obj);
    AutoPtr<WindowList> windows = ((AppWindowToken*)(IObject*)obj.Get())->mAllAppWindows;
    Int32 N;
    windows->GetSize(&N);
    for (Int32 i = N - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        windows->Get(i, (IInterface**)&obj);
        Int32 displayId = To_WindowState(obj)->GetDisplayId();
        Int32 key;
        if (displayId >= 0 && (displays->IndexOfKey(displayId, &key), key < 0)) {
            SetPendingLayoutChanges(displayId, changes);
            if (CWindowManagerService::DEBUG_LAYOUT_REPEATS) {
                mService->DebugLayoutRepeats(s, GetPendingLayoutChanges(displayId));
            }
            // Keep from processing this display again.
            displays->Put(displayId, changes);
        }
    }
}

AutoPtr<WindowAnimator::DisplayContentsAnimator> WindowAnimator::GetDisplayContentsAnimatorLocked(
    /* [in] */ Int32 displayId)
{
    AutoPtr<DisplayContentsAnimator> displayAnimator;
    HashMap<Int32, AutoPtr<DisplayContentsAnimator> >::Iterator it
            = mDisplayContentsAnimators.Find(displayId);
    if (it != mDisplayContentsAnimators.End()) {
        displayAnimator = it->mSecond;
    }
    if (displayAnimator == NULL) {
        displayAnimator = new DisplayContentsAnimator();
        mDisplayContentsAnimators[displayId] = displayAnimator;
    }
    return displayAnimator;
}

void WindowAnimator::SetScreenRotationAnimationLocked(
    /* [in] */ Int32 displayId,
    /* [in] */ ScreenRotationAnimation* animation)
{
    if (displayId >= 0) {
        GetDisplayContentsAnimatorLocked(displayId)->mScreenRotationAnimation = animation;
    }
}

AutoPtr<ScreenRotationAnimation> WindowAnimator::GetScreenRotationAnimationLocked(
    /* [in] */ Int32 displayId)
{
    if (displayId < 0) {
        return NULL;
    }
    return GetDisplayContentsAnimatorLocked(displayId)->mScreenRotationAnimation;
}

} // Wm
} // Server
} // Droid
} // Elastos
