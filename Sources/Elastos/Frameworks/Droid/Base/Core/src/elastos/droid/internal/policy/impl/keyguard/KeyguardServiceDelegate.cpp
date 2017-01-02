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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/policy/impl/keyguard/KeyguardServiceDelegate.h"
#include "elastos/droid/internal/policy/impl/keyguard/CKeyguardShowDelegate.h"
#include "elastos/droid/internal/policy/impl/keyguard/CKeyguardExitDelegate.h"
#include "elastos/droid/content/Context.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/view/CView.h"
#include "elastos/droid/view/CWindowManagerLayoutParams.h"
#include "elastos/droid/internal/policy/impl/keyguard/CKeyguardServiceWrapper.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/Manifest.h"
#include "elastos/core/CoreUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Context;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Internal::Policy::IIKeyguardExitCallback;
using Elastos::Droid::Internal::Policy::IIKeyguardShowCallback;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::CView;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Core::CoreUtils;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {
namespace Keyguard {

static const String TAG("KeyguardServiceDelegate");

//===========================================
// InnerKeyguardState
//===========================================
KeyguardServiceDelegate::InnerKeyguardState::InnerKeyguardState()
    : enabled(FALSE)
    , dismissable(FALSE)
    , offReason(0)
    , currentUser(0)
    , screenIsOn(FALSE)
    , bootCompleted(FALSE)
    , showing(TRUE)
    , showingAndNotOccluded(TRUE)
    , inputRestricted(FALSE)
    , occluded(FALSE)
    , secure(TRUE)
    , dreaming(FALSE)
    , systemIsReady(FALSE)
{
    // Assume keyguard is showing and secure until we know for sure. This is here in
    // the event something checks before the service is actually started.
    // KeyguardService itself should default to this state until the real state is known.
    // showing = TRUE;
    // showingAndNotOccluded = TRUE;
    // secure = TRUE;
}

//=================================
//InnerServiceConnection
//===================================
CAR_INTERFACE_IMPL(KeyguardServiceDelegate::InnerServiceConnection, Object, IServiceConnection)

KeyguardServiceDelegate::InnerServiceConnection::InnerServiceConnection(
    /* [in] */ KeyguardServiceDelegate* host)
    :mHost(host)
{
}

//@Override
ECode KeyguardServiceDelegate::InnerServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    if (KeyguardServiceDelegate::DEBUG) {
        Logger::V(TAG, "*** Keyguard connected (yay!)");
    }
    mHost->mKeyguardService = NULL;
    IIKeyguardService* keyguardService = IIKeyguardService::Probe(service);
    CKeyguardServiceWrapper::New(keyguardService, (IIKeyguardService**)&(mHost->mKeyguardService));
    if (mHost->mKeyguardState->systemIsReady) {
        // If the system is ready, it means keyguard crashed and restarted.
        mHost->mKeyguardService->OnSystemReady();
        // This is used to hide the scrim once keyguard displays.
        AutoPtr<IIKeyguardShowCallback> keyguardShowCallback;
        CKeyguardShowDelegate::New(mHost, NULL, (IIKeyguardShowCallback**)&keyguardShowCallback);
        mHost->mKeyguardService->OnScreenTurnedOn(keyguardShowCallback);
    }
    if (mHost->mKeyguardState->bootCompleted) {
        mHost->mKeyguardService->OnBootCompleted();
        mHost->SendStateChangeBroadcast(TRUE);
    }
    return NOERROR;
}

//@Override
ECode KeyguardServiceDelegate::InnerServiceConnection::OnServiceDisconnected(
   /* [in] */ IComponentName* name)
{
    if (KeyguardServiceDelegate::DEBUG) Logger::V(TAG, "*** Keyguard disconnected (boo!)");
    mHost->mKeyguardService = NULL;
    mHost->SendStateChangeBroadcast(FALSE);
    return NOERROR;
}

//==============================
// ShowScrimRunnable
//=============================
KeyguardServiceDelegate::ShowScrimRunnable::ShowScrimRunnable(
   /* [in] */ KeyguardServiceDelegate* host)
    :mHost(host)
{
}

ECode KeyguardServiceDelegate::ShowScrimRunnable::Run()
{
    mHost->mScrim->SetVisibility(IView::VISIBLE);
    return NOERROR;
}

//==============================
// HideScrimRunnable
//=============================
KeyguardServiceDelegate::HideScrimRunnable::HideScrimRunnable(
   /* [in] */ KeyguardServiceDelegate* host)
    :mHost(host)
{
}

ECode KeyguardServiceDelegate::HideScrimRunnable::Run()
{
    mHost->mScrim->SetVisibility(IView::GONE);
    return NOERROR;
}

//==============================
// KeyguardServiceDelegate
//=============================
CAR_INTERFACE_IMPL(KeyguardServiceDelegate, Object, IKeyguardServiceDelegate)

// TODO: propagate changes to these to {@link KeyguardTouchDelegate}
const String KeyguardServiceDelegate::KEYGUARD_PACKAGE("Elastos.Droid.SystemUI");
const String KeyguardServiceDelegate::KEYGUARD_CLASS("Elastos.Droid.SystemUI.Keyguard.CKeyguardService");

const String KeyguardServiceDelegate::ACTION_STATE_CHANGE("com.android.internal.action.KEYGUARD_SERVICE_STATE_CHANGED");
const String KeyguardServiceDelegate::EXTRA_ACTIVE("active");

const Boolean KeyguardServiceDelegate::DEBUG = TRUE;

KeyguardServiceDelegate::KeyguardServiceDelegate()
{
}

ECode KeyguardServiceDelegate::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ ILockPatternUtils* lockPatternUtils)
{
    mKeyguardState = new InnerKeyguardState();
    mKeyguardConnection = new InnerServiceConnection(this);

    mScrim = CreateScrim(ctx);
    return NOERROR;
}

ECode KeyguardServiceDelegate::BindService(
    /* [in] */ IContext* context)
{
    if (DEBUG) Logger::I(TAG, "BindService: %s, %s", KEYGUARD_PACKAGE.string(), KEYGUARD_CLASS.string());
    AutoPtr<IIntent> intent;
    FAIL_RETURN(CIntent::New((IIntent**)&intent));
    intent->SetClassName(KEYGUARD_PACKAGE, KEYGUARD_CLASS);
    Boolean result = FALSE;
    ECode ec = context->BindServiceAsUser(
        intent, mKeyguardConnection, IContext::BIND_AUTO_CREATE, UserHandle::OWNER, &result);
    if (FAILED(ec) || !result) {
        if (DEBUG) Logger::V(TAG, "*** Keyguard: can't bind to %s, ec=%08x",
            KEYGUARD_CLASS.string(), ec);
        mKeyguardState->showing = FALSE;
        mKeyguardState->showingAndNotOccluded = FALSE;
        mKeyguardState->secure = FALSE;
    }
    else {
        if (DEBUG) Logger::V(TAG, "*** Keyguard started");
    }
    return NOERROR;
}

ECode KeyguardServiceDelegate::SendStateChangeBroadcast(
    /* [in] */ Boolean bound)
{
    AutoPtr<IIntent> i;
    CIntent::New(ACTION_STATE_CHANGE, (IIntent**)&i);
    i->PutBooleanExtra(EXTRA_ACTIVE, bound);
    AutoPtr<IContext> ctx;
    mScrim->GetContext((IContext**)&ctx);
    return ctx->SendBroadcastAsUser(i, UserHandle::ALL,
            Elastos::Droid::Manifest::permission::CONTROL_KEYGUARD);
}

ECode KeyguardServiceDelegate::IsShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mKeyguardService != NULL) {
        Boolean isShowing = FALSE;
        mKeyguardService->IsShowing(&isShowing);
        mKeyguardState->showing = isShowing;
    }
    *result = mKeyguardState->showing;
    return NOERROR;
}

ECode KeyguardServiceDelegate::IsShowingAndNotOccluded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mKeyguardService != NULL) {
        Boolean isShowingAndNotOccluded = FALSE;
        mKeyguardService->IsShowingAndNotOccluded(&isShowingAndNotOccluded);
        mKeyguardState->showingAndNotOccluded = isShowingAndNotOccluded;
    }
    *result = mKeyguardState->showingAndNotOccluded;
    return NOERROR;
}

ECode KeyguardServiceDelegate::IsInputRestricted(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mKeyguardService != NULL) {
        Boolean isInputRestricted = FALSE;
        mKeyguardService->IsInputRestricted(&isInputRestricted);
        mKeyguardState->inputRestricted = isInputRestricted;
    }
    *result = mKeyguardState->inputRestricted;
    return NOERROR;
}

ECode KeyguardServiceDelegate::VerifyUnlock(
    /* [in] */ IOnKeyguardExitResult* onKeyguardExitResult)
{
    if (mKeyguardService != NULL) {
        AutoPtr<IIKeyguardExitCallback> onKeyguardExitCallback;
        CKeyguardExitDelegate::New(this, onKeyguardExitResult, (IIKeyguardExitCallback**)&onKeyguardExitCallback);
        mKeyguardService->VerifyUnlock(onKeyguardExitCallback);
    }
    return NOERROR;
}

ECode KeyguardServiceDelegate::KeyguardDone(
    /* [in] */ Boolean authenticated,
    /* [in] */ Boolean wakeup)
{
    if (mKeyguardService != NULL) {
        mKeyguardService->KeyguardDone(authenticated, wakeup);
    }
    return NOERROR;
}

ECode KeyguardServiceDelegate::SetOccluded(
    /* [in] */ Boolean isOccluded,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    if (mKeyguardService != NULL) {
        mKeyguardService->SetOccluded(isOccluded, result);
    }
    mKeyguardState->occluded = isOccluded;
    return NOERROR;
}

ECode KeyguardServiceDelegate::Dismiss()
{
    if (mKeyguardService != NULL) {
        mKeyguardService->Dismiss();
    }
    return NOERROR;
}

ECode KeyguardServiceDelegate::IsSecure(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mKeyguardService != NULL) {
        mKeyguardService->IsSecure(result);
        mKeyguardState->secure = *result;
    }
    return NOERROR;
}

ECode KeyguardServiceDelegate::OnDreamingStarted()
{
    if (mKeyguardService != NULL) {
        mKeyguardService->OnDreamingStarted();
    }
    mKeyguardState->dreaming = TRUE;
    return NOERROR;
}

ECode KeyguardServiceDelegate::OnDreamingStopped()
{
    if (mKeyguardService != NULL) {
        mKeyguardService->OnDreamingStopped();
    }
    mKeyguardState->dreaming = FALSE;
    return NOERROR;
}

ECode KeyguardServiceDelegate::OnScreenTurnedOn(
    /* [in] */ IKeyguardServiceDelegateShowListener* showListener)
{
    if (mKeyguardService != NULL) {
        if (DEBUG) Logger::V(TAG, "onScreenTurnedOn(showListener = %s )", TO_CSTR(showListener));
        AutoPtr<IIKeyguardShowCallback> keyguardShowCallback;
        CKeyguardShowDelegate::New(this, showListener, (IIKeyguardShowCallback**)&keyguardShowCallback);
        mKeyguardService->OnScreenTurnedOn(keyguardShowCallback);
    } else {
        // try again when we establish a connection
        //Slog.w(TAG, "onScreenTurnedOn(): no keyguard service!");
        Logger::W(TAG, "onScreenTurnedOn(): no keyguard service!");
        // This shouldn't happen, but if it does, invoke the listener immediately
        // to avoid a dark screen...
        showListener->OnShown(NULL);
    }
    mKeyguardState->screenIsOn = TRUE;
    return NOERROR;
}

ECode KeyguardServiceDelegate::OnScreenTurnedOff(
    /* [in] */ Int32 why)
{
    if (mKeyguardService != NULL) {
        mKeyguardService->OnScreenTurnedOff(why);
    }
    mKeyguardState->offReason = why;
    mKeyguardState->screenIsOn = FALSE;
    return NOERROR;
}

ECode KeyguardServiceDelegate::SetKeyguardEnabled(
    /* [in] */ Boolean enabled)
{
    if (mKeyguardService != NULL) {
        mKeyguardService->SetKeyguardEnabled(enabled);
    }
    mKeyguardState->enabled = enabled;
    return NOERROR;
}

ECode KeyguardServiceDelegate::IsDismissable(
    /* out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mKeyguardService != NULL) {
        mKeyguardState->dismissable = mKeyguardService->IsDismissable(result);
    }
    return NOERROR;
}

ECode KeyguardServiceDelegate::OnSystemReady()
{
    if (mKeyguardService != NULL) {
        mKeyguardService->OnSystemReady();
    }
    else {
        mKeyguardState->systemIsReady = TRUE;
    }
    return NOERROR;
}

ECode KeyguardServiceDelegate::DoKeyguardTimeout(
    /* in] */ IBundle* options)
{
    if (mKeyguardService != NULL) {
        mKeyguardService->DoKeyguardTimeout(options);
    }
    return NOERROR;
}

ECode KeyguardServiceDelegate::ShowAssistant()
{
    if (mKeyguardService != NULL) {
        mKeyguardService->ShowAssistant();
    }
    return NOERROR;
}

ECode KeyguardServiceDelegate::SetCurrentUser(
    /* [in] */ Int32 newUserId)
{
    if (mKeyguardService != NULL) {
        mKeyguardService->SetCurrentUser(newUserId);
    }
    mKeyguardState->currentUser = newUserId;
    return NOERROR;
}

ECode KeyguardServiceDelegate::StartKeyguardExitAnimation(
    /* [in] */ Int64 startTime,
    /* [in] */ Int64 fadeoutDuration)
{
    if (mKeyguardService != NULL) {
        mKeyguardService->StartKeyguardExitAnimation(startTime, fadeoutDuration);
    }
    return NOERROR;
}

AutoPtr<IView> KeyguardServiceDelegate::CreateScrim(
   /* [in] */ IContext* context)
{
    AutoPtr<IView> view;
    CView::New(context, (IView**)&view);

    Int32 flags = IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN
            | IWindowManagerLayoutParams::FLAG_LAYOUT_INSET_DECOR
            | IWindowManagerLayoutParams::FLAG_FORCE_NOT_FULLSCREEN
            | IWindowManagerLayoutParams::FLAG_SHOW_WALLPAPER
            ;

    const Int32 stretch = IViewGroupLayoutParams::MATCH_PARENT;
    const Int32 type = IWindowManagerLayoutParams::TYPE_KEYGUARD_SCRIM;
    //WindowManager.LayoutParams lp = new WindowManager.LayoutParams( stretch, stretch, type, flags, PixelFormat.TRANSLUCENT);
    AutoPtr<IWindowManagerLayoutParams> lp;
    CWindowManagerLayoutParams::New(stretch, stretch, type, flags, IPixelFormat::TRANSLUCENT, (IWindowManagerLayoutParams**)&lp);
    lp->SetSoftInputMode(IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_RESIZE);
    lp->SetScreenOrientation(IActivityInfo::SCREEN_ORIENTATION_NOSENSOR);
    Int32 privateFlags = 0;
    lp->GetPrivateFlags(&privateFlags);
    lp->SetPrivateFlags(privateFlags | IWindowManagerLayoutParams::PRIVATE_FLAG_FAKE_HARDWARE_ACCELERATED);
    AutoPtr<ICharSequence> title = CoreUtils::Convert("KeyguardScrim");
    lp->SetTitle(title);

    AutoPtr<IInterface> wm;
    context->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&wm);
    AutoPtr<IViewManager> viewManager = IViewManager::Probe(wm);
    AutoPtr<IViewGroupLayoutParams> viewGroupLayoutParams = IViewGroupLayoutParams::Probe(lp);
    viewManager->AddView(view, viewGroupLayoutParams);
    view->SetVisibility(IView::GONE);
    // Disable pretty much everything in statusbar until keyguard comes back and we know
    // the state of the world.
    view->SetSystemUiVisibility(IView::STATUS_BAR_DISABLE_HOME
            | IView::STATUS_BAR_DISABLE_BACK
            | IView::STATUS_BAR_DISABLE_RECENT
            | IView::STATUS_BAR_DISABLE_EXPAND
            | IView::STATUS_BAR_DISABLE_SEARCH);
    return view;
}

ECode KeyguardServiceDelegate::ShowScrim()
{
    AutoPtr<IRunnable> run = new ShowScrimRunnable(this);
    Boolean result;
    mScrim->Post(run, &result);
    return NOERROR;
}

ECode KeyguardServiceDelegate::HideScrim()
{
    AutoPtr<IRunnable> run = new HideScrimRunnable(this);
    Boolean result;
    mScrim->Post(run, &result);
    return NOERROR;
}

ECode KeyguardServiceDelegate::OnBootCompleted()
{
    if (mKeyguardService != NULL) {
        mKeyguardService->OnBootCompleted();
        SendStateChangeBroadcast(true);
    }
    mKeyguardState->bootCompleted = TRUE;
    return NOERROR;
}

ECode KeyguardServiceDelegate::OnActivityDrawn()
{
    if (mKeyguardService != NULL) {
        mKeyguardService->OnActivityDrawn();
    }
    return NOERROR;
}

} // namespace Keyguard
} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos
