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

#include "Elastos.Droid.Graphics.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/service/dreams/DreamService.h"
#include "elastos/droid/service/dreams/CDreamServiceWrapper.h"
#include "elastos/droid/graphics/drawable/CColorDrawable.h"
#include "elastos/droid/internal/policy/CPolicyManager.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/utility/MathUtils.h"
#include "elastos/droid/view/CWindowManagerGlobalHelper.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Internal::Policy::CPolicyManager;
using Elastos::Droid::Internal::Policy::IPolicyManager;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Utility::MathUtils;
using Elastos::Droid::View::CWindowManagerGlobalHelper;
using Elastos::Droid::View::EIID_IWindowCallback;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IWindowManagerGlobal;
using Elastos::Droid::View::IWindowManagerGlobalHelper;
using Elastos::Droid::R;
using Elastos::Core::IRunnable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Dreams {

///////////////////////////DreamService::DreamServiceWrapper::AttachRunnable//////////

DreamService::DreamServiceWrapper::AttachRunnable::AttachRunnable(
    /* [in] */ DreamServiceWrapper* host)
    : mHost(host)
{
}

ECode DreamService::DreamServiceWrapper::AttachRunnable::Run()
{
    return mHost->mHost->Attach(mHost->mWindowToken, mHost->mCanDoze);
}

///////////////////////////DreamService::DreamServiceWrapper::DetachRunnable//////////

DreamService::DreamServiceWrapper::DetachRunnable::DetachRunnable(
    /* [in] */ DreamServiceWrapper* host)
    : mHost(host)
{
}

ECode DreamService::DreamServiceWrapper::DetachRunnable::Run()
{
    return mHost->mHost->Detach();
}

///////////////////////////DreamService::DreamServiceWrapper::WakeUpRunnable////////

DreamService::DreamServiceWrapper::WakeUpRunnable::WakeUpRunnable(
    /* [in] */ DreamServiceWrapper* host)
    : mHost(host)
{
}

ECode DreamService::DreamServiceWrapper::WakeUpRunnable::Run()
{
    return mHost->mHost->WakeUp(TRUE /*fromSystem*/);
}

///////////////////////////DreamService::DreamServiceWrapper////////////////////////////////////

CAR_INTERFACE_IMPL_2(DreamService::DreamServiceWrapper, Object, IIDreamService, IBinder)

DreamService::DreamServiceWrapper::DreamServiceWrapper()
    : mCanDoze(FALSE)
{}

ECode DreamService::DreamServiceWrapper::constructor(
    /* [in] */ IDreamService* host)
{
    mHost = (DreamService*)host;
    return NOERROR;
}

ECode DreamService::DreamServiceWrapper::Attach(
    /* [in] */ IBinder* windowToken,
    /* [in] */ Boolean canDoze)
{
    mWindowToken = windowToken;
    mCanDoze = canDoze;
    AutoPtr<AttachRunnable> runnable = new AttachRunnable(this);
    Boolean result = FALSE;
    return mHost->mHandler->Post(runnable.Get(), &result);
}

ECode DreamService::DreamServiceWrapper::Detach()
{
    AutoPtr<DetachRunnable> runnable = new DetachRunnable(this);
    Boolean result = FALSE;
    return mHost->mHandler->Post(runnable.Get(), &result);
}

ECode DreamService::DreamServiceWrapper::WakeUp()
{
    AutoPtr<WakeUpRunnable> runnable = new WakeUpRunnable(this);
    Boolean result = FALSE;
    return mHost->mHandler->Post(runnable.Get(), &result);
}

///////////////////////////DreamService::DreamService_Attach_Runnable///////////////////////////////////////

DreamService::DreamService_Attach_Runnable::DreamService_Attach_Runnable(
    /* [in] */ DreamService* host)
    : mHost(host)
{
}

ECode DreamService::DreamService_Attach_Runnable::Run()
{
    if (mHost->mWindow != NULL || mHost->mWindowless) {
        if (mHost->mDebug) Slogger::V(TAG, "Calling onDreamingStarted()");
            mHost->mStarted = TRUE;
            mHost->OnDreamingStarted();
    }
    return NOERROR;
}

///////////////////////////DreamService//////////////////////////////////////////////////////////////////////

const String DreamService::TAG("DreamService");/*DreamService.class.getSimpleName() + "[" + getClass().getSimpleName() + "]";*/

CAR_INTERFACE_IMPL_2(DreamService, Service, IDreamService, IWindowCallback)

DreamService::DreamService()
    : mInteractive(FALSE)
    , mLowProfile(TRUE)
    , mFullscreen(FALSE)
    , mScreenBright(TRUE)
    , mStarted(FALSE)
    , mWaking(FALSE)
    , mFinished(FALSE)
    , mCanDoze(FALSE)
    , mDozing(FALSE)
    , mWindowless(FALSE)
    , mDozeScreenState(IDisplay::STATE_UNKNOWN)
    , mDozeScreenBrightness(IPowerManager::BRIGHTNESS_DEFAULT)
    , mDebug(FALSE)
{
    CHandler::New((IHandler**)&mHandler);
}

ECode DreamService::constructor()
{
    AutoPtr<IInterface> obj = ServiceManager::GetService(IDreamService::DREAM_SERVICE);
    mSandman = IIDreamManager::Probe(obj);
    return NOERROR;
}

ECode DreamService::SetDebug(
    /* [in] */ Boolean dbg)
{
    mDebug = dbg;
    return NOERROR;
}

ECode DreamService::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // TODO: create more flexible version of mInteractive that allows use of KEYCODE_BACK
    Int32 keyCode;
    if (!mInteractive) {
        if (mDebug) Slogger::V(TAG, "Waking up on keyEvent");
        WakeUp();
        *result = TRUE;
        return NOERROR;
    }
    else if (event->GetKeyCode(&keyCode), keyCode == IKeyEvent::KEYCODE_BACK) {
        if (mDebug) Slogger::V(TAG, "Waking up on back key");
        WakeUp();
        *result = TRUE;
        return NOERROR;
    }
    return mWindow->SuperDispatchKeyEvent(event, result);
}

ECode DreamService::DispatchKeyShortcutEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (!mInteractive) {
        if (mDebug)  Slogger::V(TAG, "Waking up keyShortcutEvent");
        WakeUp();
        *result = TRUE;
        return NOERROR;
    }
    return mWindow->SuperDispatchKeyShortcutEvent(event, result);
}

ECode DreamService::DispatchTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // TODO: create more flexible version of mInteractive that allows clicks
    // but finish()es on any other kind of activity
    if (!mInteractive) {
        if (mDebug) Slogger::V(TAG, "Waking up on touchEvent");
        WakeUp();
        *result = TRUE;
        return NOERROR;
    }
    return mWindow->SuperDispatchTouchEvent(event, result);
}

ECode DreamService::DispatchTrackballEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (!mInteractive) {
        if (mDebug) Slogger::V(TAG, "Waking up trackballEvent");
        WakeUp();
        *result = TRUE;
        return NOERROR;
    }
    return mWindow->SuperDispatchTrackballEvent(event, result);
}

ECode DreamService::DispatchGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (!mInteractive) {
        if (mDebug) Slogger::V(TAG, "Waking up genericMotionEvent");
        WakeUp();
        *result = TRUE;
        return NOERROR;
    }
    return mWindow->SuperDispatchGenericMotionEvent(event, result);
}

ECode DreamService::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode DreamService::OnCreatePanelView(
    /* [in] */ Int32 featureId,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode DreamService::OnCreatePanelMenu(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode DreamService::OnPreparePanel(
    /* [in] */ Int32 featureId,
    /* [in] */ IView* view,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode DreamService::OnMenuOpened(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode DreamService::OnMenuItemSelected(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode DreamService::OnWindowAttributesChanged(
    /* [in] */ IWindowManagerLayoutParams* attrs)
{
    return NOERROR;
}

ECode DreamService::OnContentChanged()
{
    return NOERROR;
}

ECode DreamService::OnWindowFocusChanged(
    /* [in] */ Boolean hasFocus)
{
    return NOERROR;
}

ECode DreamService::OnAttachedToWindow()
{
    return NOERROR;
}

ECode DreamService::OnDetachedFromWindow()
{
    return NOERROR;
}

ECode DreamService::OnPanelClosed(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenu* menu)
{
    return NOERROR;
}

ECode DreamService::OnSearchRequested(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode DreamService::OnWindowStartingActionMode(
    /* [in] */ IActionModeCallback* callback,
    /* [out] */ IActionMode** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode DreamService::OnActionModeStarted(
    /* [in] */ IActionMode* mode)
{
    return NOERROR;
}

ECode DreamService::OnActionModeFinished(
    /* [in] */ IActionMode* mode)
{
    return NOERROR;
}

ECode DreamService::GetWindowManager(
    /* [out] */ IWindowManager** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IWindowManager> wm;
    mWindow->GetWindowManager((IWindowManager**)&wm);
    if (mWindow != NULL) {
        *result = wm.Get();
        REFCOUNT_ADD(*result)
    }
    else {
        *result = NULL;
    }
    return NOERROR;
}

ECode DreamService::GetWindow(
    /* [out] */ IWindow** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mWindow;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DreamService::SetContentView(
    /* [in] */ Int32 layoutResID)
{
    AutoPtr<IWindow> win;
    GetWindow((IWindow**)&win);
    win->SetContentView(layoutResID);
    return NOERROR;
}

ECode DreamService::SetContentView(
    /* [in] */ IView* view)
{
    AutoPtr<IWindow> win;
    GetWindow((IWindow**)&win);
    win->SetContentView(view);
    return NOERROR;
}

ECode DreamService::SetContentView(
    /* [in] */ IView* view,
    /* [in] */ IViewGroupLayoutParams* params)
{
    AutoPtr<IWindow> win;
    GetWindow((IWindow**)&win);
    win->SetContentView(view, params);
    return NOERROR;
}

ECode DreamService::AddContentView(
    /* [in] */ IView* view,
    /* [in] */ IViewGroupLayoutParams* params)
{
    AutoPtr<IWindow> win;
    GetWindow((IWindow**)&win);
    win->AddContentView(view, params);
    return NOERROR;
}

ECode DreamService::FindViewById(
    /* [in] */ Int32 id,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IWindow> win;
    GetWindow((IWindow**)&win);
    win->FindViewById(id, result);
    return NOERROR;
}

ECode DreamService::SetInteractive(
    /* [in] */ Boolean interactive)
{
    mInteractive = interactive;
    return NOERROR;
}

ECode DreamService::IsInteractive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mInteractive;
    return NOERROR;
}

ECode DreamService::SetLowProfile(
    /* [in] */ Boolean lowProfile)
{
    if (mLowProfile != lowProfile) {
        mLowProfile = lowProfile;
        Int32 flag = IView::SYSTEM_UI_FLAG_LOW_PROFILE;
        ApplySystemUiVisibilityFlags(mLowProfile ? flag : 0, flag);
    }
    return NOERROR;
}

ECode DreamService::IsLowProfile(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return GetSystemUiVisibilityFlagValue(IView::SYSTEM_UI_FLAG_LOW_PROFILE, mLowProfile, result);
}

ECode DreamService::SetFullscreen(
    /* [in] */ Boolean fullscreen)
{
    if (mFullscreen != fullscreen) {
        mFullscreen = fullscreen;
        Int32 flag = IWindowManagerLayoutParams::FLAG_FULLSCREEN;
        ApplyWindowFlags(mFullscreen ? flag : 0, flag);
    }
    return NOERROR;
}

ECode DreamService::IsFullscreen(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFullscreen;
    return NOERROR;
}

ECode DreamService::SetScreenBright(
    /* [in] */ Boolean screenBright)
{
    if (mScreenBright != screenBright) {
        mScreenBright = screenBright;
        Int32 flag = IWindowManagerLayoutParams::FLAG_KEEP_SCREEN_ON;
        ApplyWindowFlags(mScreenBright ? flag : 0, flag);
    }
    return NOERROR;
}

ECode DreamService::IsScreenBright(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return GetWindowFlagValue(IWindowManagerLayoutParams::FLAG_KEEP_SCREEN_ON, mScreenBright, result);
}

ECode DreamService::SetWindowless(
    /* [in] */ Boolean windowless)
{
    mWindowless = windowless;
    return NOERROR;
}

ECode DreamService::IsWindowless(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWindowless;
    return NOERROR;
}

ECode DreamService::CanDoze(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCanDoze;
    return NOERROR;
}

ECode DreamService::StartDozing()
{
    if (mCanDoze && !mDozing) {
        mDozing = TRUE;
        UpdateDoze();
    }
    return NOERROR;
}

ECode DreamService::UpdateDoze()
{
    if (mDozing) {
        ECode ec = mSandman->StartDozing(mWindowToken, mDozeScreenState, mDozeScreenBrightness);
        if (FAILED(ec)) {
            return E_REMOTE_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode DreamService::StopDozing()
{
    if (mDozing) {
        mDozing = FALSE;
        ECode ec = mSandman->StopDozing(mWindowToken);
        if (FAILED(ec)) {
            return E_REMOTE_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode DreamService::IsDozing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDozing;
    return NOERROR;
}

ECode DreamService::GetDozeScreenState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDozeScreenState;
    return NOERROR;
}

ECode DreamService::SetDozeScreenState(
    /* [in] */ Int32 state)
{
    if (mDozeScreenState != state) {
        mDozeScreenState = state;
        UpdateDoze();
    }
    return NOERROR;
}

ECode DreamService::GetDozeScreenBrightness(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDozeScreenBrightness;
    return NOERROR;
}

ECode DreamService::SetDozeScreenBrightness(
    /* [in] */ Int32 brightness)
{
    if (brightness != IPowerManager::BRIGHTNESS_DEFAULT) {
        ClampAbsoluteBrightness(brightness, &brightness);
    }
    if (mDozeScreenBrightness != brightness) {
        mDozeScreenBrightness = brightness;
        UpdateDoze();
    }
    return NOERROR;
}

ECode DreamService::OnCreate()
{
    if (mDebug){
        Slogger::V(TAG, "onCreate()");
    }
    return Service::OnCreate();
}

ECode DreamService::OnDreamingStarted()
{
    if (mDebug){
        Slogger::V(TAG, "onDreamingStarted()");
    }
    // hook for subclasses
    return NOERROR;
}

ECode DreamService::OnDreamingStopped()
{
    if (mDebug){
        Slogger::V(TAG, "onDreamingStopped()");
    }
    // hook for subclasses
    return NOERROR;
}

ECode DreamService::OnWakeUp()
{
    return Finish();
}

ECode DreamService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** result)
{
    VALIDATE_NOT_NULL(result);
    if (mDebug){
        String str;
        intent->ToString(&str);
        Slogger::V(TAG, "onBind() intent = %s", str.string());
    }
    return CDreamServiceWrapper::New(this, result);
}

ECode DreamService::Finish()
{
    if (mDebug){
        Slogger::V(TAG, "finish(): mFinished=%s", mFinished ? "TRUE" : "FALSE");
    }
    if (!mFinished) {
        mFinished = TRUE;

        if (mWindowToken == NULL) {
            Slogger::W(TAG, "Finish was called before the dream was attached.");
        }
        else {
            ECode ec = mSandman->FinishSelf(mWindowToken, TRUE /*immediate*/);
            if (FAILED(ec)) {
                return E_REMOTE_EXCEPTION;
            }
        }

        StopSelf(); // if launched via any other means
    }
    return NOERROR;
}

ECode DreamService::WakeUp()
{
    return WakeUp(FALSE);
}


ECode DreamService::OnDestroy()
{
    if (mDebug) Slogger::V(TAG, "onDestroy()");
    // hook for subclasses

    // Just in case destroy came in before detach, let's take care of that now
    Detach();

    Service::OnDestroy();
    return NOERROR;
}

ECode DreamService::WakeUp(
    /* [in] */ Boolean fromSystem)
{
    if (mDebug) {

        Slogger::V(TAG, "wakeUp(): fromSystem=%s, mWaking=%s, mFinished=%s",
            fromSystem ? "TRUE" : "FALSE",
            mWaking ? "TRUE" : "FALSE",
            mFinished ? "TRUE" : "FALSE");
    }

    if (!mWaking && !mFinished) {
        mWaking = TRUE;

        // As a minor optimization, invoke the callback first in case it simply
        // calls finish() immediately so there wouldn't be much point in telling
        // the system that we are finishing the dream gently.
        OnWakeUp();

        // Now tell the system we are waking gently, unless we already told
        // it we were finishing immediately.
        if (!fromSystem && !mFinished) {
            if (mWindowToken == NULL) {
                Slogger::W(TAG, "WakeUp was called before the dream was attached.");
            }
            else {
                ECode ec = mSandman->FinishSelf(mWindowToken, FALSE /*immediate*/);
                if (FAILED(ec)) {
                    return E_REMOTE_EXCEPTION;
                }
            }
        }
    }
    return NOERROR;
}

ECode DreamService::Detach()
{
    if (mStarted) {
        if (mDebug) Slogger::V(TAG, "detach(): Calling onDreamingStopped()");
        mStarted = FALSE;
        OnDreamingStopped();
    }

    if (mWindow != NULL) {
        // force our window to be removed synchronously
        if (mDebug) Slogger::V(TAG, "detach(): Removing window from window manager");

        AutoPtr<IWindowManager> wm;
        mWindow->GetWindowManager((IWindowManager**)&wm);
        AutoPtr<IView> v;
        mWindow->GetDecorView((IView**)&v);
        wm->RemoveViewImmediate(v);
        mWindow = NULL;
    }

    if (mWindowToken != NULL) {
        // the following will print a log message if it finds any other leaked windows
        AutoPtr<IWindowManagerGlobalHelper> helper;
        CWindowManagerGlobalHelper::AcquireSingleton((IWindowManagerGlobalHelper**)&helper);
        AutoPtr<IWindowManagerGlobal> g;
        helper->GetInstance((IWindowManagerGlobal**)&g);
        assert(0);
        g->CloseAll(mWindowToken, String("DreamService")/*this.getClass().getName()*/, String("Dream"));
        mWindowToken = NULL;
        mCanDoze = FALSE;
    }
    return NOERROR;
}

ECode DreamService::Attach(
    /* [in] */ IBinder* windowToken,
    /* [in] */ Boolean canDoze)
{
    if (mWindowToken != NULL) {
        Slogger::E(TAG, "attach() called when already attached with token=%p", mWindowToken.Get());
        return E_NULL_POINTER_EXCEPTION;
    }
    if (mFinished || mWaking) {
        Slogger::W(TAG, "attach() called after dream already finished");
        ECode ec = mSandman->FinishSelf(windowToken, TRUE /*immediate*/);
        if (FAILED(ec)) return E_REMOTE_EXCEPTION;
        return E_NULL_POINTER_EXCEPTION;
    }

    mWindowToken = windowToken;
    mCanDoze = canDoze;
    if (mWindowless && !mCanDoze) {
        Slogger::E(TAG, "Only doze dreams can be windowless");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (!mWindowless) {
        AutoPtr<IPolicyManager> pm;
        CPolicyManager::AcquireSingleton((IPolicyManager**)&pm);
        pm->MakeNewWindow(this, (IWindow**)&mWindow);
        mWindow->SetCallback(this);
        Boolean isRequest = FALSE;
        mWindow->RequestFeature(IWindow::FEATURE_NO_TITLE, &isRequest);
        AutoPtr<IDrawable> drawable;
        CColorDrawable::New(0xFF000000, (IDrawable**)&drawable);
        mWindow->SetBackgroundDrawable(drawable.Get());
        mWindow->SetFormat(IPixelFormat::OPAQUE);

        String wt;
        IObject::Probe(windowToken)->ToString(&wt);
        if (mDebug) Slogger::V(TAG, "Attaching window token: %s to window of type %d",
                wt.string(), IWindowManagerLayoutParams::TYPE_DREAM);

        AutoPtr<IWindowManagerLayoutParams> lp;
        mWindow->GetAttributes((IWindowManagerLayoutParams**)&lp);
        lp->SetType(IWindowManagerLayoutParams::TYPE_DREAM);
        lp->SetToken(windowToken);
        lp->SetWindowAnimations(R::style::Animation_Dream);
        Int32 flags;
        lp->GetFlags(&flags);
        flags |= ( IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN
                    | IWindowManagerLayoutParams::FLAG_LAYOUT_INSET_DECOR
                    | IWindowManagerLayoutParams::FLAG_SHOW_WHEN_LOCKED
                    | IWindowManagerLayoutParams::FLAG_DISMISS_KEYGUARD
                    | IWindowManagerLayoutParams::FLAG_ALLOW_LOCK_WHILE_SCREEN_ON
                    | (mFullscreen ? IWindowManagerLayoutParams::FLAG_FULLSCREEN : 0)
                    | (mScreenBright ? IWindowManagerLayoutParams::FLAG_KEEP_SCREEN_ON : 0)
                    );
        lp->SetFlags(flags);
        mWindow->SetAttributes(lp.Get());
        // Workaround: Currently low-profile and in-window system bar backgrounds don't go
        // along well. Dreams usually don't need such bars anyways, so disable them by default.
        mWindow->ClearFlags(IWindowManagerLayoutParams::FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
        mWindow->SetWindowManager(NULL, windowToken, String("dream"), TRUE);

        ApplySystemUiVisibilityFlags(
                (mLowProfile ? IView::SYSTEM_UI_FLAG_LOW_PROFILE : 0),
                IView::SYSTEM_UI_FLAG_LOW_PROFILE);

        AutoPtr<IWindowManager> wm;
        GetWindowManager((IWindowManager**)&wm);
        AutoPtr<IView> vi;
        mWindow->GetDecorView((IView**)&vi);
        AutoPtr<IWindowManagerLayoutParams> wmlp;
        mWindow->GetAttributes((IWindowManagerLayoutParams**)&wmlp);
        ECode ec = IViewManager::Probe(wm)->AddView(vi.Get(), IViewGroupLayoutParams::Probe(wmlp.Get()));
        if (FAILED(ec)) {
            // This can happen because the dream manager service will remove the token
            // immediately without necessarily waiting for the dream to start.
            // We should receive a finish message soon.
            Slogger::I(TAG, "attach() called after window token already removed, dream will finish soon");
            mWindow = NULL;
            return E_WINDOWMANAGER_BAD_TOKEN_EXCEPTION;
        }
    }
    // We need to defer calling onDreamingStarted until after onWindowAttached,
    // which is posted to the handler by addView, so we post onDreamingStarted
    // to the handler also.  Need to watch out here in case detach occurs before
    // this callback is invoked.
    AutoPtr<DreamService_Attach_Runnable> runnable = new DreamService_Attach_Runnable(this);
    Boolean result = FALSE;
    mHandler->Post(IRunnable::Probe(runnable), &result);
    return NOERROR;
}

ECode DreamService::GetWindowFlagValue(
    /* [in] */ Int32 flag,
    /* [in] */ Boolean defaultValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mWindow == NULL) {
        *result = defaultValue;
    }
    else {
        AutoPtr<IWindowManagerLayoutParams> p;
        mWindow->GetAttributes((IWindowManagerLayoutParams**)&p);
        Int32 flags;
        p->GetFlags(&flags);
        *result = (flags & flag) != 0;
    }
    return NOERROR;
}

ECode DreamService::ApplyWindowFlags(
    /* [in] */ Int32 flags,
    /* [in] */ Int32 mask)
{
    if (mWindow != NULL) {
        AutoPtr<IWindowManagerLayoutParams> lp;
        mWindow->GetAttributes((IWindowManagerLayoutParams**)&lp);
        Int32 flag;
        lp->GetFlags(&flag);
        ApplyFlags(flag, flags, mask, &flag);
        lp->SetFlags(flag);
        mWindow->SetAttributes(lp);
        AutoPtr<IView> v;
        mWindow->GetDecorView((IView**)&v);
        AutoPtr<IWindowManager> wm;
        mWindow->GetWindowManager((IWindowManager**)&wm);
        IViewManager::Probe(wm)->UpdateViewLayout(v, IViewGroupLayoutParams::Probe(lp));
    }
    return NOERROR;
}

ECode DreamService::GetSystemUiVisibilityFlagValue(
    /* [in] */ Int32 flag,
    /* [in] */ Boolean defaultValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IView> v;
    if (mWindow == NULL) {
        v = NULL;
    }
    else {
        mWindow->GetDecorView((IView**)&v);
    }
    if (v == NULL) {
        *result = defaultValue;
    }
    else {
        Int32 visibility;
        v->GetSystemUiVisibility(&visibility);
        *result = (visibility & flag) != 0;
    }
    return NOERROR;
}

ECode DreamService::ApplySystemUiVisibilityFlags(
    /* [in] */ Int32 flags,
    /* [in] */ Int32 mask)
{
    AutoPtr<IView> v;
    if (mWindow == NULL) {
        v = NULL;
    }
    else {
        mWindow->GetDecorView((IView**)&v);
    }
    if (v != NULL) {
        Int32 visibility;
        v->GetSystemUiVisibility(&visibility);
        ApplyFlags(visibility, flags, mask, &visibility);
        v->SetSystemUiVisibility(visibility);
    }
    return NOERROR;
}

ECode DreamService::ApplyFlags(
    /* [in] */ Int32 oldFlags,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 mask,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (oldFlags & ~mask) | (flags & mask);
    return NOERROR;
}

ECode DreamService::ClampAbsoluteBrightness(
    /* [in] */ Int32 value,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return Elastos::Droid::Utility::MathUtils::Constrain(value, IPowerManager::BRIGHTNESS_OFF, IPowerManager::BRIGHTNESS_ON);
}

} // namespace Dreams
} // namespace Service
} // namepsace Droid
} // namespace Elastos
