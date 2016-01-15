
#include "wm/InputMonitor.h"
#include "wm/DragState.h"
#include "wm/CWindowManagerService.h"
#include "elastos/droid/app/ActivityManagerNative.h"

using Elastos::Droid::Graphics::CRect;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

InputMonitor::InputMonitor(
    /* [in] */ CWindowManagerService* service)
    : mService(service)
    , mInputDispatchFrozen(FALSE)
    , mInputDispatchEnabled(FALSE)
    , mUpdateInputWindowsNeeded(TRUE)
    , mInputWindowHandleCount(0)
    , mInputDevicesReady(FALSE)
{
    CRect::New((IRect**)&mTmpRect);
}

void InputMonitor::NotifyInputChannelBroken(
    /* [in] */ InputWindowHandle* inputWindowHandle)
{
    if (inputWindowHandle == NULL) {
        return;
    }

    AutoLock lock(mService->mWindowMapLock);
    WindowState* windowState = (WindowState*)inputWindowHandle->mWindowState;
    if (windowState != NULL) {
        // Slog.i(WindowManagerService.TAG, "WINDOW DIED " + windowState);
        mService->RemoveWindowLocked(windowState->mSession, windowState);
    }
}

Int64 InputMonitor::NotifyANR(
    /* [in] */ InputApplicationHandle* inputApplicationHandle,
    /* [in] */ InputWindowHandle* inputWindowHandle,
    /* [in] */ const String& reason)
{
    AutoPtr<AppWindowToken> appWindowToken;
    AutoPtr<WindowState> windowState;
    Boolean aboveSystem = FALSE;

    {
        AutoLock lock(mService->mWindowMapLock);

        if (inputWindowHandle != NULL) {
            windowState = (WindowState*)inputWindowHandle->mWindowState;
            if (windowState != NULL) {
                appWindowToken = windowState->mAppToken;
            }
        }
        if (appWindowToken == NULL && inputApplicationHandle != NULL) {
            appWindowToken = (AppWindowToken*)inputApplicationHandle->mAppWindowToken;
        }

        if (windowState != NULL) {
            String title;
            windowState->mAttrs->GetTitle(&title);
            Slogger::I(CWindowManagerService::TAG, "Input event dispatching timed out sending to %s.  Reason: %s"
                    , title.string(), reason.string());
            // Figure out whether this window is layered above system windows.
            // We need to do this here to help the activity manager know how to
            // layer its ANR dialog.
            Int32 systemAlertLayer;
            mService->mPolicy->WindowTypeToLayerLw(
                    IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT, &systemAlertLayer);
            aboveSystem = windowState->mBaseLayer > systemAlertLayer;
        }
        else if (appWindowToken != NULL) {
            Slogger::I(CWindowManagerService::TAG, "Input event dispatching timed out sending to application %s.  Reason: %s",
                    appWindowToken->mStringName.string(), reason.string());
        }
        else {
            Slogger::I(CWindowManagerService::TAG, "Input event dispatching timed out..  Reason: %s", reason.string());
        }

        mService->SaveANRStateLocked(appWindowToken, windowState, reason);
    }

    if (appWindowToken != NULL && appWindowToken->mAppToken != NULL) {
        // try {
        // Notify the activity manager about the timeout and let it decide whether
        // to abort dispatching or keep waiting.
        Boolean abort;
        appWindowToken->mAppToken->KeyDispatchingTimedOut(reason, &abort);
        if (!abort) {
            // The activity manager declined to abort dispatching.
            // Wait a bit longer and timeout again later.
            return appWindowToken->mInputDispatchingTimeoutNanos;
        }
        // } catch (RemoteException ex) {
        // }
    }
    else if (windowState != NULL) {
        // try {
        // Notify the activity manager about the timeout and let it decide whether
        // to abort dispatching or keep waiting.
        AutoPtr<IIActivityManager> activityManager = ActivityManagerNative::GetDefault();
        Int64 timeout;
        activityManager->InputDispatchingTimedOut(
                windowState->mSession->mPid, aboveSystem, reason, &timeout);
        if (timeout >= 0) {
            // The activity manager declined to abort dispatching.
            // Wait a bit longer and timeout again later.
            return timeout;
        }
        // } catch (RemoteException ex) {
        // }
    }
    return 0; // abort dispatching
}

void InputMonitor::AddInputWindowHandleLw(
    /* [in] */ InputWindowHandle* windowHandle)
{
    if (mInputWindowHandles == NULL) {
        mInputWindowHandles = ArrayOf<InputWindowHandle*>::Alloc(16);
    }
    if (mInputWindowHandleCount >= mInputWindowHandles->GetLength()) {
        AutoPtr<ArrayOf<InputWindowHandle*> > temp = mInputWindowHandles;
        mInputWindowHandles = ArrayOf<InputWindowHandle*>::Alloc(mInputWindowHandleCount * 2);
        mInputWindowHandles->Copy(temp);
    }
    mInputWindowHandles->Set(mInputWindowHandleCount++, windowHandle);
}

void InputMonitor::AddInputWindowHandleLw(
    /* [in] */ InputWindowHandle* inputWindowHandle,
    /* [in] */ WindowState* child,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 privateFlags,
    /* [in] */ Int32 type,
    /* [in] */ Boolean isVisible,
    /* [in] */ Boolean hasFocus,
    /* [in] */ Boolean hasWallpaper)
{
    // Add a window to our list of input windows.
    inputWindowHandle->mName = child->ToString();
    Boolean modal = (flags & (IWindowManagerLayoutParams::FLAG_NOT_TOUCH_MODAL
            | IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE)) == 0;
    if (modal && child->mAppToken != NULL) {
        // Limit the outer touch to the activity stack region.
        flags |= IWindowManagerLayoutParams::FLAG_NOT_TOUCH_MODAL;
        child->GetStackBounds(mTmpRect);
        inputWindowHandle->mTouchableRegion->Set(mTmpRect);
    }
    else {
        // Not modal or full screen modal
        child->GetTouchableRegion(inputWindowHandle->mTouchableRegion);
    }
    inputWindowHandle->mLayoutParamsFlags = flags;
    inputWindowHandle->mLayoutParamsPrivateFlags = privateFlags;
    inputWindowHandle->mLayoutParamsType = type;
    inputWindowHandle->mDispatchingTimeoutNanos = child->GetInputDispatchingTimeoutNanos();
    inputWindowHandle->mVisible = isVisible;
    inputWindowHandle->mCanReceiveKeys = child->CanReceiveKeys();
    inputWindowHandle->mHasFocus = hasFocus;
    inputWindowHandle->mHasWallpaper = hasWallpaper;
    inputWindowHandle->mPaused = child->mAppToken != NULL ?
            child->mAppToken->mPaused : FALSE;
    inputWindowHandle->mLayer = child->mLayer;
    inputWindowHandle->mOwnerPid = child->mSession->mPid;
    inputWindowHandle->mOwnerUid = child->mSession->mUid;
    child->mAttrs->GetInputFeatures(&inputWindowHandle->mInputFeatures);

    AutoPtr<IRect> frame = child->mFrame;
    frame->GetLeft(&inputWindowHandle->mFrameLeft);
    frame->GetTop(&inputWindowHandle->mFrameTop);
    frame->GetRight(&inputWindowHandle->mFrameRight);
    frame->GetBottom(&inputWindowHandle->mFrameBottom);

    if (child->mGlobalScale != 1) {
        // If we are scaling the window, input coordinates need
        // to be inversely scaled to map from what is on screen
        // to what is actually being touched in the UI.
        inputWindowHandle->mScaleFactor = 1.0f / child->mGlobalScale;
    }
    else {
        inputWindowHandle->mScaleFactor = 1;
    }

    AddInputWindowHandleLw(inputWindowHandle);
}

void InputMonitor::ClearInputWindowHandlesLw()
{
    while (mInputWindowHandleCount != 0) {
        mInputWindowHandles->Set(--mInputWindowHandleCount, NULL);
    }
}

void InputMonitor::SetUpdateInputWindowsNeededLw()
{
    mUpdateInputWindowsNeeded = TRUE;
}

void InputMonitor::UpdateInputWindowsLw(
    /* [in] */ Boolean force)
{
    if (!force && !mUpdateInputWindowsNeeded) {
        return;
    }
    mUpdateInputWindowsNeeded = FALSE;

    // if (false) Slog.d(WindowManagerService.TAG, ">>>>>> ENTERED updateInputWindowsLw");

    // Populate the input window list with information about all of the windows that
    // could potentially receive input.
    // As an optimization, we could try to prune the list of windows but this turns
    // out to be difficult because only the native code knows for sure which window
    // currently has touch focus.
    AutoPtr<WindowStateAnimator> universeBackground = mService->mAnimator->mUniverseBackground;
    Int32 aboveUniverseLayer = mService->mAnimator->mAboveUniverseLayer;
    Boolean addedUniverse = FALSE;

    // If there's a drag in flight, provide a pseudowindow to catch drag input
    Boolean inDrag = (mService->mDragState != NULL);
    if (inDrag) {
        // if (WindowManagerService.DEBUG_DRAG) {
        //     Log.d(WindowManagerService.TAG, "Inserting drag window");
        // }
        AutoPtr<InputWindowHandle> dragWindowHandle = mService->mDragState->mDragWindowHandle;
        if (dragWindowHandle != NULL) {
            AddInputWindowHandleLw(dragWindowHandle);
        }
        else {
            Slogger::W(CWindowManagerService::TAG, "Drag is in progress but there is no drag window handle.");
        }
    }

    List<AutoPtr<FakeWindowImpl> >::Iterator it = mService->mFakeWindows.Begin();
    for (; it != mService->mFakeWindows.End(); ++it) {
        AddInputWindowHandleLw((*it)->mWindowHandle);
    }

    // Add all windows on the default display.
    Int32 numDisplays;
    mService->mDisplayContents->GetSize(&numDisplays);
    for (Int32 displayNdx = 0; displayNdx < numDisplays; ++displayNdx) {
        AutoPtr<IInterface> value;
        mService->mDisplayContents->ValueAt((IInterface**)&value);
        AutoPtr<DisplayContent> displayContent = (DisplayContent*)(IObject*)value.Get();
        AutoPtr<WindowList> windows = displayContent->GetWindowList();
        WindowList::ReverseIterator winRit = windows->RBegin();
        for (; winRit != windows->REnd(); ++winRit) {
            AutoPtr<WindowState> child = *rit;
            AutoPtr<IInputChannel> inputChannel = child->mInputChannel;
            AutoPtr<IInputWindowHandle> inputWindowHandle = child->mInputWindowHandle;
            if (inputChannel == NULL || inputWindowHandle == NULL || child->mRemoved) {
                // Skip this window because it cannot possibly receive input.
                continue;
            }

            Int32 flags;
            child->mAttrs->GetFlags(&flags);
            Int32 privateFlags;
            child->mAttrs->GetPrivateFlags(&privateFlags);
            Int32 type;
            child->mAttrs->GetType(&type);

            Boolean hasFocus = (child == mInputFocus);
            Boolean isVisible = child->IsVisibleLw();
            Boolean hasWallpaper = (child == mService->mWallpaperTarget)
                    && (privateFlags & IWindowManagerLayoutParams::PRIVATE_FLAG_KEYGUARD) == 0;
            Boolean onDefaultDisplay = (child->GetDisplayId() == IDisplay::DEFAULT_DISPLAY);

            // If there's a drag in progress and 'child' is a potential drop target,
            // make sure it's been told about the drag
            if (inDrag && isVisible && onDefaultDisplay) {
                mService->mDragState->SendDragStartedIfNeededLw(child);
            }

            if (universeBackground != NULL && !addedUniverse
                    && child->mBaseLayer < aboveUniverseLayer && onDefaultDisplay) {
                AutoPtr<WindowState> u = universeBackground->mWin;
                if (u->mInputChannel != NULL && u->mInputWindowHandle != NULL) {
                    Int32 uFlags;
                    u->mAttrs->GetFlags(&uFlags);
                    Int32 uPrivateFlags;
                    u->mAttrs->GetPrivateFlags(&uPrivateFlags);
                    Int32 uType;
                    u->mAttrs->GetType(&uType);
                    AddInputWindowHandleLw(u->mInputWindowHandle, u, uFlags, uPrivateFlags, uType,
                            TRUE, u == mInputFocus, FALSE);
                }
                addedUniverse = TRUE;
            }

            if (child->mWinAnimator != universeBackground) {
                AddInputWindowHandleLw(inputWindowHandle, child, flags, privateFlags, type,
                        isVisible, hasFocus, hasWallpaper);
            }
        }
    }

    // Send windows to native code.
    mService->mInputManager->SetInputWindows(mInputWindowHandles);

    // Clear the list in preparation for the next round.
    ClearInputWindowHandlesLw();

    // if (false) Slog.d(WindowManagerService.TAG, "<<<<<<< EXITED updateInputWindowsLw");
}

void InputMonitor::NotifyConfigurationChanged()
{
    mService->SendNewConfiguration();

    AutoLock lock(mInputDevicesReadyMonitor);
    if (!mInputDevicesReady) {
        mInputDevicesReady = TRUE;
        mInputDevicesReadyMonitor.NotifyAll();
    }
}

Boolean InputMonitor::WaitForInputDevicesReady(
    /* [in] */ Int64 timeoutMillis)
{
    AutoLock lock(mInputDevicesReadyMonitor);
    if (!mInputDevicesReady) {
        // try {
        mInputDevicesReadyMonitor.Wait(timeoutMillis);
        // } catch (InterruptedException ex) {
        // }
    }
    return mInputDevicesReady;
}

void InputMonitor::NotifyLidSwitchChanged(
    /* [in] */ Int64 whenNanos,
    /* [in] */ Boolean lidOpen)
{
    mService->mPolicy->NotifyLidSwitchChanged(whenNanos, lidOpen);
}

void InputMonitor::NotifyCameraLensCoverSwitchChanged(
    /* [in] */ Int64 whenNanos,
    /* [in] */ Boolean lensCovered)
{
    mService->mPolicy->NotifyCameraLensCoverSwitchChanged(whenNanos, lensCovered);
}

Int32 InputMonitor::InterceptKeyBeforeQueueing(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags)
{
    Int32 result = 0;
    mService->mPolicy->InterceptMotionBeforeQueueingNonInteractive(whenNanos, policyFlags, &result);
    return result;
}

Int32 InputMonitor::InterceptMotionBeforeQueueingWhenScreenOff(
    /* [in] */ Int32 policyFlags)
{
    Int32 result;
    mService->mPolicy->InterceptMotionBeforeQueueingWhenScreenOff(policyFlags, &result);
    return result;
}

Int64 InputMonitor::InterceptKeyBeforeDispatching(
    /* [in] */ InputWindowHandle* focus,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags)
{
    AutoPtr<WindowState> windowState = focus != NULL ?
            (WindowState*)focus->mWindowState : NULL;
    Int64 result = 0;
    mService->mPolicy->InterceptKeyBeforeDispatching(windowState, event, policyFlags, &result);
    return result;
}

AutoPtr<IKeyEvent> InputMonitor::DispatchUnhandledKey(
    /* [in] */ InputWindowHandle* focus,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags)
{
    AutoPtr<WindowState> windowState = focus != NULL ?
            (WindowState*)focus->mWindowState : NULL;
    AutoPtr<IKeyEvent> keyEvent;
    mService->mPolicy->DispatchUnhandledKey(windowState, event, policyFlags,
            (IKeyEvent**)&keyEvent);
    return keyEvent;
}

Int32 InputMonitor::GetPointerLayer()
{
    Int32 result;
    mService->mPolicy->WindowTypeToLayerLw(IWindowManagerLayoutParams::TYPE_POINTER, &result);
    return result * CWindowManagerService::TYPE_LAYER_MULTIPLIER
            + CWindowManagerService::TYPE_LAYER_OFFSET;
}

void InputMonitor::SetInputFocusLw(
    /* [in] */ WindowState* newWindow,
    /* [in] */ Boolean updateInputWindows)
{
    if (CWindowManagerService::DEBUG_FOCUS_LIGHT || CWindowManagerService::DEBUG_INPUT) {
        Slogger::D(CWindowManagerService::TAG, "Input focus has changed to %p", newWindow);
    }

    if (newWindow != mInputFocus) {
        if (newWindow != NULL && newWindow->CanReceiveKeys()) {
            // Displaying a window implicitly causes dispatching to be unpaused.
            // This is to protect against bugs if someone pauses dispatching but
            // forgets to resume.
            newWindow->mToken->mPaused = FALSE;
        }

        mInputFocus = newWindow;
        SetUpdateInputWindowsNeededLw();

        if (updateInputWindows) {
            UpdateInputWindowsLw(FALSE /*force*/);
        }
    }
}

void InputMonitor::SetFocusedAppLw(
    /* [in] */ AppWindowToken* newApp)
{
    // Focused app has changed.
    if (newApp == NULL) {
        mService->mInputManager->SetFocusedApplication(NULL);
    }
    else {
        AutoPtr<InputApplicationHandle> handle = newApp->mInputApplicationHandle;
        handle->mName = newApp->ToString();
        handle->mDispatchingTimeoutNanos = newApp->mInputDispatchingTimeoutNanos;

        mService->mInputManager->SetFocusedApplication(handle);
    }
}

void InputMonitor::PauseDispatchingLw(
    /* [in] */ WindowToken* window)
{
    if (! window->mPaused) {
        // if (WindowManagerService.DEBUG_INPUT) {
        //     Slog.v(WindowManagerService.TAG, "Pausing WindowToken " + window);
        // }

        window->mPaused = TRUE;
        UpdateInputWindowsLw(TRUE /*force*/);
    }
}

void InputMonitor::ResumeDispatchingLw(
    /* [in] */ WindowToken* window)
{
    if (window->mPaused) {
        // if (WindowManagerService.DEBUG_INPUT) {
        //     Slog.v(WindowManagerService.TAG, "Resuming WindowToken " + window);
        // }

        window->mPaused = FALSE;
        UpdateInputWindowsLw(TRUE /*force*/);
    }
}

void InputMonitor::FreezeInputDispatchingLw()
{
    if (! mInputDispatchFrozen) {
        // if (WindowManagerService.DEBUG_INPUT) {
        //     Slog.v(WindowManagerService.TAG, "Freezing input dispatching");
        // }

        mInputDispatchFrozen = TRUE;
        UpdateInputDispatchModeLw();
    }
}

void InputMonitor::ThawInputDispatchingLw()
{
    if (mInputDispatchFrozen) {
        // if (WindowManagerService.DEBUG_INPUT) {
        //     Slog.v(WindowManagerService.TAG, "Thawing input dispatching");
        // }

        mInputDispatchFrozen = FALSE;
        UpdateInputDispatchModeLw();
    }
}

void InputMonitor::SetEventDispatchingLw(
    /* [in] */ Boolean enabled)
{
    if (mInputDispatchEnabled != enabled) {
        // if (WindowManagerService.DEBUG_INPUT) {
        //     Slog.v(WindowManagerService.TAG, "Setting event dispatching to " + enabled);
        // }

        mInputDispatchEnabled = enabled;
        UpdateInputDispatchModeLw();
    }
}

void InputMonitor::UpdateInputDispatchModeLw()
{
    mService->mInputManager->SetInputDispatchMode(
            mInputDispatchEnabled, mInputDispatchFrozen);
}


} // Wm
} // Server
} // Droid
} // Elastos
