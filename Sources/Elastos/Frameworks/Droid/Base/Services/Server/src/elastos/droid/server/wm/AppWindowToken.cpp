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

#include "elastos/droid/server/wm/AppWindowToken.h"
#include "elastos/droid/server/wm/CWindowManagerService.h"
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Content::Pm::IActivityInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

static const String TAG("AppWindowToken");

AppWindowToken::AppWindowToken()
    : mAnimator(NULL)
    , mVoiceInteraction(FALSE)
    , mGroupId(-1)
    , mAppFullscreen(FALSE)
    , mRequestedOrientation(IActivityInfo::SCREEN_ORIENTATION_UNSPECIFIED)
    , mLayoutConfigChanges(FALSE)
    , mShowWhenLocked(FALSE)
    , mInputDispatchingTimeoutNanos(0)
    , mLastTransactionSequence(0)
    , mNumInterestingWindows(0)
    , mNumDrawnWindows(0)
    , mInPendingTransaction(FALSE)
    , mAllDrawn(FALSE)
    , mDeferClearAllDrawn(FALSE)
    , mWillBeHidden(FALSE)
    , mHiddenRequested(FALSE)
    , mClientHidden(FALSE)
    , mReportedVisible(FALSE)
    , mReportedDrawn(FALSE)
    , mRemoved(FALSE)
    , mStartingWindow(NULL)
    , mStartingView(NULL)
    , mStartingDisplayed(FALSE)
    , mStartingMoved(FALSE)
    , mFirstWindowDrawn(FALSE)
    , mDeferRemoval(FALSE)
    , mLaunchTaskBehind(FALSE)
    , mEnteringAnimation(FALSE)
{
}

AppWindowToken::~AppWindowToken()
{
    mInputApplicationHandle = NULL;
    mAppAnimator = NULL;
    mAnimator = NULL;
    mStartingWindow = NULL;
    mStartingView = NULL;
}

ECode AppWindowToken::constructor(
    /* [in] */ CWindowManagerService* service,
    /* [in] */ IApplicationToken* token,
    /* [in] */ Boolean voiceInteraction)
{
    WindowToken::constructor(service, IBinder::Probe(token), IWindowManagerLayoutParams::TYPE_APPLICATION, TRUE);

    mAppToken = token;
    mVoiceInteraction = voiceInteraction;
    mAnimator = service->mAnimator;

    mAppWindowToken = this;
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    mInputApplicationHandle = new InputApplicationHandle(wr);
    mAppAnimator = new AppWindowAnimator(this);
    return CArrayList::New((WindowList**)&mAllAppWindows);
}

void AppWindowToken::SendAppVisibilityToClients()
{
    Int32 N;
    mAllAppWindows->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mAllAppWindows->Get(i, (IInterface**)&obj);
        WindowState* win = To_WindowState(obj);
        if (win == mStartingWindow && mClientHidden) {
            // Don't hide the starting window.
            continue;
        }
        // try {
        if (CWindowManagerService::DEBUG_VISIBILITY) {
            Slogger::V(CWindowManagerService::TAG,
                "Setting visibility of %p: %d", TO_CSTR(win), !mClientHidden);
        }
        win->mClient->DispatchAppVisibility(!mClientHidden);
        // } catch (RemoteException e) {
        // }
    }
}

void AppWindowToken::UpdateReportedVisibilityLocked()
{
    if (mAppToken == NULL) {
        return;
    }

    Int32 numInteresting = 0;
    Int32 numVisible = 0;
    Int32 numDrawn = 0;
    Boolean nowGone = TRUE;

    if (CWindowManagerService::DEBUG_VISIBILITY) {
        Slogger::V(CWindowManagerService::TAG, "Update reported visibility: %s", TO_CSTR(this));
    }
    Int32 N;
    mAllAppWindows->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mAllAppWindows->Get(i, (IInterface**)&obj);
        WindowState* win = To_WindowState(obj);
        Int32 winType;
        win->mAttrs->GetType(&winType);
        if (win == mStartingWindow || win->mAppFreezing
                || win->mViewVisibility != IView::VISIBLE
                || winType == IWindowManagerLayoutParams::TYPE_APPLICATION_STARTING
                || win->mDestroying) {
            continue;
        }
        if (CWindowManagerService::DEBUG_VISIBILITY) {
            Slogger::V(CWindowManagerService::TAG, "Win %s: isDrawn=%d, isAnimating=%d",
                TO_CSTR(win), win->IsDrawnLw(), win->mWinAnimator->IsAnimating());
            if (!win->IsDrawnLw()) {
                Slogger::V(CWindowManagerService::TAG, "Not displayed: s=%p pv=%d mDrawState=%d ah=%d th=%d a=%d",
                    win->mWinAnimator->mSurfaceControl.Get(), win->mPolicyVisibility,
                    win->mWinAnimator->mDrawState, win->mAttachedHidden,
                    (win->mAppToken != NULL ? win->mAppToken->mHiddenRequested : FALSE),
                    win->mWinAnimator->mAnimating);
            }
        }
        numInteresting++;
        if (win->IsDrawnLw()) {
            numDrawn++;
            if (!win->mWinAnimator->IsAnimating()) {
                numVisible++;
            }
            nowGone = FALSE;
        }
        else if (win->mWinAnimator->IsAnimating()) {
            nowGone = FALSE;
        }
    }

    Boolean nowDrawn = numInteresting > 0 && numDrawn >= numInteresting;
    Boolean nowVisible = numInteresting > 0 && numVisible >= numInteresting;
    if (!nowGone) {
        // If the app is not yet gone, then it can only become visible/drawn.
        if (!nowDrawn) {
            nowDrawn = mReportedDrawn;
        }
        if (!nowVisible) {
            nowVisible = mReportedVisible;
        }
    }

    if (CWindowManagerService::DEBUG_VISIBILITY) {
        Slogger::V(CWindowManagerService::TAG, "VIS %s: interesting=%d, visible=%d",
            TO_CSTR(this), numInteresting, numVisible);
    }

    if (nowDrawn != mReportedDrawn) {
        if (nowDrawn) {
            AutoPtr<IMessage> msg;
            mService->mH->ObtainMessage(CWindowManagerService::H::REPORT_APPLICATION_TOKEN_DRAWN,
                TO_IINTERFACE(this), (IMessage**)&msg);
            Boolean result;
            mService->mH->SendMessage(msg, &result);
        }
        mReportedDrawn = nowDrawn;
    }
    if (nowVisible != mReportedVisible) {
        if (CWindowManagerService::DEBUG_VISIBILITY) {
            Slogger::V(CWindowManagerService::TAG, "Visibility changed in %s: vis=%d", TO_CSTR(this), nowVisible);
        }
        mReportedVisible = nowVisible;
        AutoPtr<IMessage> msg;
        mService->mH->ObtainMessage(CWindowManagerService::H::REPORT_APPLICATION_TOKEN_WINDOWS,
                nowVisible ? 1 : 0, nowGone ? 1 : 0, TO_IINTERFACE(this), (IMessage**)&msg);
        Boolean result;
        mService->mH->SendMessage(msg, &result);
    }
}

AutoPtr<WindowState> AppWindowToken::FindMainWindow()
{
    Int32 j;
    mWindows->GetSize(&j);
    while (j > 0) {
        j--;
        AutoPtr<IInterface> obj;
        mWindows->Get(j, (IInterface**)&obj);
        WindowState* win = To_WindowState(obj);
        Int32 winType;
        win->mAttrs->GetType(&winType);
        if (winType == IWindowManagerLayoutParams::TYPE_BASE_APPLICATION
            || winType == IWindowManagerLayoutParams::TYPE_APPLICATION_STARTING) {
            return win;
        }
    }
    return NULL;
}

Boolean AppWindowToken::IsVisible()
{
    Int32 N;
    mAllAppWindows->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mAllAppWindows->Get(i, (IInterface**)&obj);
        WindowState* win = To_WindowState(obj);
        if (!win->mAppFreezing
                && (win->mViewVisibility == IView::VISIBLE ||
                    (win->mWinAnimator->IsAnimating() &&
                            !mService->mAppTransition->IsTransitionSet()))
                && !win->mDestroying && win->IsDrawnLw()) {
            return TRUE;
        }
    }
    return FALSE;
}

void AppWindowToken::RemoveAllWindows()
{
    Int32 N;
    mAllAppWindows->GetSize(&N);
    for (Int32 winNdx = N - 1; winNdx >= 0; --winNdx) {
        // try {
        AutoPtr<IInterface> obj;
        mAllAppWindows->Get(winNdx, (IInterface**)&obj);
        WindowState* win = To_WindowState(obj);
        if (CWindowManagerService::DEBUG_WINDOW_MOVEMENT) {
            Slogger::W(CWindowManagerService::TAG, "removeAllWindows: removing win=%s", TO_CSTR(win));
        }
        win->mService->RemoveWindowLocked(win->mSession, win);
        // } catch (IndexOutOfBoundsException e) {
        //     Slog.e(WindowManagerService.TAG, "Error while removing window : " + e);
        // }
    }
}

ECode AppWindowToken::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    if (mStringName.IsNull()) {
        StringBuilder sb;
        sb.Append("AppWindowToken{");
        sb.Append(StringUtils::ToHexString((Int32)this));
        sb.Append(" token=");
        String info;
        mToken->ToString(&info);
        sb.Append(info);
        sb.AppendChar('}');
        mStringName = sb.ToString();
    }
    *str = mStringName;
    return NOERROR;
}

} // Wm
} // Server
} // Droid
} // Elastos
