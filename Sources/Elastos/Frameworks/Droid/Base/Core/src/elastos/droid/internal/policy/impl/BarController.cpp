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

#include "elastos/droid/app/CStatusBarManager.h"
#include "elastos/droid/internal/policy/impl/BarController.h"
#include "elastos/droid/view/WindowManagerPolicyControl.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/core/AutoLock.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::App::CStatusBarManager;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::WindowManagerPolicyControl;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Core::AutoLock;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

//=====================================================================
//                    BarController::InnerRunnable1
//=====================================================================
CAR_INTERFACE_IMPL(BarController::InnerRunnable1, Object, IRunnable)
BarController::InnerRunnable1::InnerRunnable1(
    /* [in] */ BarController* owner,
    /* [in] */ Int32 state)
    : mOwner(owner)
    , mState(state)
{}

ECode BarController::InnerRunnable1::Run()
{
    //try {
    AutoPtr<IIStatusBarService> statusbar = mOwner->GetStatusBarService();
    ECode ec = NOERROR;
    if (statusbar != NULL) {
        ec = statusbar->SetWindowState(mOwner->mStatusBarManagerId, mState);
    }
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        if (DEBUG) {
            Slogger::W(mOwner->mTag, "Error posting window state 0x%x", ec);
        }
        // re-acquire status bar service next time it is needed.
        mOwner->mStatusBarService = NULL;
    //}
    }
    return NOERROR;
}

//=====================================================================
//                            BarController
//=====================================================================
CAR_INTERFACE_IMPL(BarController, Object, IBarController)

const Boolean BarController::DEBUG = FALSE;
const Int32 BarController::TRANSIENT_BAR_NONE;
const Int32 BarController::TRANSIENT_BAR_SHOW_REQUESTED;
const Int32 BarController::TRANSIENT_BAR_SHOWING;
const Int32 BarController::TRANSIENT_BAR_HIDING;
const Int32 BarController::TRANSLUCENT_ANIMATION_DELAY_MS;

BarController::BarController()
    : mTransientFlag(0)
    , mUnhideFlag(0)
    , mTranslucentFlag(0)
    , mStatusBarManagerId(0)
    , mTranslucentWmFlag(0)
    , mState(IStatusBarManager::WINDOW_STATE_SHOWING)
    , mTransientBarState(0)
    , mPendingShow(FALSE)
    , mLastTranslucent(0)
{}

ECode BarController::constructor(
    /* [in] */ const String& tag,
    /* [in] */ Int32 transientFlag,
    /* [in] */ Int32 unhideFlag,
    /* [in] */ Int32 translucentFlag,
    /* [in] */ Int32 statusBarManagerId,
    /* [in] */ Int32 translucentWmFlag)
{
    // ==================before translated======================
    mTag = String("BarController.") + tag;
    mTransientFlag = transientFlag;
    mUnhideFlag = unhideFlag;
    mTranslucentFlag = translucentFlag;
    mStatusBarManagerId = statusBarManagerId;
    mTranslucentWmFlag = translucentWmFlag;
    CHandler::New((IHandler**)&mHandler);
    return NOERROR;
}

ECode BarController::SetWindow(
    /* [in] */ IWindowState* win)
{
    mWin = win;
    return NOERROR;
}

ECode BarController::ShowTransient()
{
    if (mWin != NULL) {
        SetTransientBarState(TRANSIENT_BAR_SHOW_REQUESTED);
    }
    return NOERROR;
}

ECode BarController::IsTransientShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mTransientBarState == TRANSIENT_BAR_SHOWING);
    return NOERROR;
}

ECode BarController::IsTransientShowRequested(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mTransientBarState == TRANSIENT_BAR_SHOW_REQUESTED);
    return NOERROR;
}

ECode BarController::WasRecentlyTranslucent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ((SystemClock::GetUptimeMillis() - mLastTranslucent) < TRANSLUCENT_ANIMATION_DELAY_MS);
    return NOERROR;
}

ECode BarController::AdjustSystemUiVisibilityLw(
    /* [in] */ Int32 oldVis,
    /* [in] */ Int32 vis)
{
    if (mWin != NULL
            && mTransientBarState == TRANSIENT_BAR_SHOWING
            && (vis & mTransientFlag) == 0) {
        // sysui requests hide
        SetTransientBarState(TRANSIENT_BAR_HIDING);
        Boolean res;
        SetBarShowingLw(FALSE, &res);
    }
    else if (mWin != NULL && (oldVis & mUnhideFlag) != 0 && (vis & mUnhideFlag) == 0) {
        // sysui ready to unhide
        Boolean res;
        SetBarShowingLw(TRUE, &res);
    }
    return NOERROR;
}

ECode BarController::ApplyTranslucentFlagLw(
    /* [in] */ IWindowState* win,
    /* [in] */ Int32 vis,
    /* [in] */ Int32 oldVis,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (mWin != NULL) {
        AutoPtr<IWindowManagerLayoutParams> attrs;
        if (win != NULL) {
            win->GetAttrs((IWindowManagerLayoutParams**)&attrs);
        }
        Int32 privateFlags;
        if (win != NULL && (attrs->GetPrivateFlags(&privateFlags),
                privateFlags & IWindowManagerLayoutParams::PRIVATE_FLAG_INHERIT_TRANSLUCENT_DECOR) == 0) {
            Int32 fl = WindowManagerPolicyControl::GetWindowFlags(win, NULL);
            if ((fl & mTranslucentWmFlag) != 0) {
                vis |= mTranslucentFlag;
            }
            else {
                vis &= ~mTranslucentFlag;
            }
            if ((fl & IWindowManagerLayoutParams::FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS) != 0) {
                vis |= IView::SYSTEM_UI_TRANSPARENT;
            }
            else {
                vis &= ~IView::SYSTEM_UI_TRANSPARENT;
            }
        }
        else {
            vis = (vis & ~mTranslucentFlag) | (oldVis & mTranslucentFlag);
            vis = (vis & ~IView::SYSTEM_UI_TRANSPARENT) | (oldVis & IView::SYSTEM_UI_TRANSPARENT);
        }
    }
    *result = vis;
    return NOERROR;
}

ECode BarController::SetBarShowingLw(
    /* [in] */ Boolean show,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mWin == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    if (show && mTransientBarState == TRANSIENT_BAR_HIDING) {
        mPendingShow = TRUE;
        *result = FALSE;
        return NOERROR;
    }
    Boolean wasVis;
    mWin->IsVisibleLw(&wasVis);
    Boolean wasAnim;
    mWin->IsAnimatingLw(&wasAnim);
    Boolean change;
    if (show) {
        mWin->ShowLw(TRUE, &change);
    }
    else {
        mWin->HideLw(TRUE, &change);
    }
    Int32 state = ComputeStateLw(wasVis, wasAnim, mWin, change);
    Boolean stateChanged = UpdateStateLw(state);
    *result = (change || stateChanged);
    return NOERROR;
}

Int32 BarController::ComputeStateLw(
    /* [in] */ Boolean wasVis,
    /* [in] */ Boolean wasAnim,
    /* [in] */ IWindowState* win,
    /* [in] */ Boolean change)
{
    Boolean hasDrawnLw;
    win->HasDrawnLw(&hasDrawnLw);
    if (hasDrawnLw) {
        Boolean vis;
        win->IsVisibleLw(&vis);
        Boolean anim;
        win->IsAnimatingLw(&anim);
        if (mState == IStatusBarManager::WINDOW_STATE_HIDING && !change && !vis) {
            return IStatusBarManager::WINDOW_STATE_HIDDEN;
        }
        else if (mState == IStatusBarManager::WINDOW_STATE_HIDDEN && vis) {
            return IStatusBarManager::WINDOW_STATE_SHOWING;
        }
        else if (change) {
            if (wasVis && vis && !wasAnim && anim) {
                return IStatusBarManager::WINDOW_STATE_HIDING;
            }
            else {
                return IStatusBarManager::WINDOW_STATE_SHOWING;
            }
        }
    }
    return mState;
}

Boolean BarController::UpdateStateLw(
    /* [in] */ Int32 state)
{
    if (state != mState) {
        mState = state;
        if (DEBUG) {
            Slogger::D(mTag, "mState: %s", CStatusBarManager::WindowStateToString(state).string());
        }
        AutoPtr<IRunnable> run = new InnerRunnable1(this, state);
        Boolean res;
        mHandler->Post(run, &res);
        return TRUE;
    }
    return FALSE;
}

ECode BarController::CheckHiddenLw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean drawn;
    if (mWin != NULL && (mWin->HasDrawnLw(&drawn), drawn)) {
        Boolean isVisibleLw;
        mWin->IsVisibleLw(&isVisibleLw);
        Boolean isAnimatingLw;
        mWin->IsAnimatingLw(&isAnimatingLw);
        if (!isVisibleLw && !isAnimatingLw) {
            UpdateStateLw(IStatusBarManager::WINDOW_STATE_HIDDEN);
        }
        mWin->IsVisibleLw(&isVisibleLw);
        if (mTransientBarState == TRANSIENT_BAR_HIDING && !isVisibleLw) {
            // Finished animating out, clean up and reset style
            SetTransientBarState(TRANSIENT_BAR_NONE);
            if (mPendingShow) {
                Boolean res;
                SetBarShowingLw(TRUE, &res);
                mPendingShow = FALSE;
            }
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode BarController::CheckShowTransientBarLw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean isDisplayedLw;
    if (mTransientBarState == TRANSIENT_BAR_SHOWING) {
        if (DEBUG) Slogger::D(mTag, "Not showing transient bar, already shown");
        *result = FALSE;
        return NOERROR;
    }
    else if (mTransientBarState == TRANSIENT_BAR_SHOW_REQUESTED) {
        if (DEBUG) Slogger::D(mTag, "Not showing transient bar, already requested");
        *result = FALSE;
        return NOERROR;
    }
    else if (mWin == NULL) {
        if (DEBUG) Slogger::D(mTag, "Not showing transient bar, bar doesn't exist");
        *result = FALSE;
        return NOERROR;
    }
    else if (mWin->IsDisplayedLw(&isDisplayedLw), isDisplayedLw) {
        if (DEBUG) Slogger::D(mTag, "Not showing transient bar, bar already visible");
        *result = FALSE;
        return NOERROR;
    }
    else {
        *result = TRUE;
        return NOERROR;
    }
}

ECode BarController::UpdateVisibilityLw(
    /* [in] */ Boolean transientAllowed,
    /* [in] */ Int32 oldVis,
    /* [in] */ Int32 vis,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (mWin == NULL) {
        *result = vis;
        return NOERROR;
    }
    Boolean isTransientShowing, isTransientShowRequested;
    if ((IsTransientShowing(&isTransientShowing), isTransientShowing) ||
            (IsTransientShowRequested(&isTransientShowRequested), isTransientShowRequested)) { // transient bar requested
        if (transientAllowed) {
            vis |= mTransientFlag;
            if ((oldVis & mTransientFlag) == 0) {
                vis |= mUnhideFlag;  // tell sysui we're ready to unhide
            }
            SetTransientBarState(TRANSIENT_BAR_SHOWING);  // request accepted
        }
        else {
            SetTransientBarState(TRANSIENT_BAR_NONE);  // request denied
        }
    }
    if (mTransientBarState != TRANSIENT_BAR_NONE) {
        vis |= mTransientFlag;  // ignore clear requests until transition completes
        vis &= ~IView::SYSTEM_UI_FLAG_LOW_PROFILE;  // never show transient bars in low profile
    }
    if ((vis & mTranslucentFlag) != 0
            || (oldVis & mTranslucentFlag) != 0
            || ((vis | oldVis) & IView::SYSTEM_UI_FLAG_FULLSCREEN) != 0) {
        mLastTranslucent = SystemClock::GetUptimeMillis();
    }
    *result = vis;
    return NOERROR;
}

void BarController::SetTransientBarState(
    /* [in] */ Int32 state)
{
    if (mWin != NULL && state != mTransientBarState) {
        if (mTransientBarState == TRANSIENT_BAR_SHOWING || state == TRANSIENT_BAR_SHOWING) {
            mLastTranslucent = SystemClock::GetUptimeMillis();
        }
        mTransientBarState = state;
        if (DEBUG) Slogger::D(mTag, "mTransientBarState: %s", TransientBarStateToString(state).string());
    }
}

AutoPtr<IIStatusBarService> BarController::GetStatusBarService()
{
    AutoLock lock(mServiceAquireLock);
    if (mStatusBarService == NULL) {
        AutoPtr<IInterface> service = ServiceManager::GetService(String("statusbar"));
        mStatusBarService = IIStatusBarService::Probe(service);
    }
    return mStatusBarService;
}

String BarController::TransientBarStateToString(
    /* [in] */ Int32 state)
{
    if (state == TRANSIENT_BAR_HIDING) return String("TRANSIENT_BAR_HIDING");
    if (state == TRANSIENT_BAR_SHOWING) return String("TRANSIENT_BAR_SHOWING");
    if (state == TRANSIENT_BAR_SHOW_REQUESTED) return String("TRANSIENT_BAR_SHOW_REQUESTED");
    if (state == TRANSIENT_BAR_NONE) return String("TRANSIENT_BAR_NONE");
    //throw new IllegalArgumentException("Unknown state " + state);
    return String(NULL);
}

ECode BarController::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    if (mWin != NULL) {
        pw->Print(prefix);
        pw->Println(mTag);
        pw->Print(prefix);
        pw->Print(String("  "));
        pw->Print(String("mState"));
        pw->Print('=');
        pw->Println(CStatusBarManager::WindowStateToString(mState));
        pw->Print(prefix);
        pw->Print(String("  "));
        pw->Print(String("mTransientBar"));
        pw->Print('=');
        pw->Println(TransientBarStateToString(mTransientBarState));
    }
    return NOERROR;
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos


