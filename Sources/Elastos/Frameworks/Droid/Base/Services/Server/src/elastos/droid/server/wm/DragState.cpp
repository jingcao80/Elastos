
#include "elastos/droid/server/wm/DragState.h"
#include "elastos/droid/server/wm/CWindowManagerService.h"
#include "elastos/droid/server/wm/InputMonitor.h"
#include "elastos/droid/server/wm/DisplayContent.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Graphics::CRegion;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::View::IInputChannelHelper;
using Elastos::Droid::View::CInputChannelHelper;
using Elastos::Droid::View::IInputChannel;
using Elastos::Droid::View::ISurfaceControlHelper;
using Elastos::Droid::View::IDragEventHelper;
using Elastos::Droid::View::CDragEventHelper;
using Elastos::Droid::View::CSurfaceControlHelper;
using Elastos::Droid::View::ISurfaceHelper;
using Elastos::Droid::Os::Process;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

DragState::DragState(
    /* [in] */ CWindowManagerService* service,
    /* [in] */ IBinder* token,
    /* [in] */ ISurfaceControl* surfaceControl,
    /* [in] */ Int32 flags,
    /* [in] */ IBinder* localWin)
    : mService(service)
    , mToken(token)
    , mSurfaceControl(surfaceControl)
    , mFlags(flags)
    , mLocalWin(localWin)
{
    ASSERT_SUCCEEDED(CRegion::New((IRegion**)&mTmpRegion));
}

void DragState::Reset()
{
    if (mSurfaceControl != NULL) {
        mSurfaceControl->Destroy();
    }
    mSurfaceControl = NULL;
    mFlags = 0;
    mLocalWin = NULL;
    mToken = NULL;
    mData = NULL;
    mThumbOffsetX = mThumbOffsetY = 0;
    mNotifiedWindows.Clear();
}

void DragState::Register(
    /* [in] */ IDisplay* display)
{
    mDisplay = display;
    if (CWindowManagerService::DEBUG_DRAG) {
        Slogger::D(CWindowManagerService::TAG, "registering drag input channel");
    }
    if (mClientChannel != NULL) {
        Slogger::E(CWindowManagerService::TAG, "Duplicate register of drag input channel");
    }
    else {
        AutoPtr<IInputChannelHelper> helper;
        CInputChannelHelper::AcquireSingleton((IInputChannelHelper**)&helper);
        AutoPtr<ArrayOf<IInputChannel*> > inputChannels;
        ASSERT_SUCCEEDED(helper->OpenInputChannelPair(String("drag"), (ArrayOf<IInputChannel*>**)&inputChannels))
        mServerChannel = (*inputChannels)[0];
        mClientChannel = (*inputChannels)[1];
        mService->mInputManager->RegisterInputChannel(mServerChannel, NULL);
        AutoPtr<ILooper> hLooper;
        mService->mH->GetLooper((ILooper**)&hLooper);
        mInputEventReceiver = new DragInputEventReceiver();
        mInputEventReceiver->constructor(mClientChannel, hLooper, mService);

        mDragApplicationHandle = new InputApplicationHandle(NULL);
        mDragApplicationHandle->mName = String("drag");
        mDragApplicationHandle->mDispatchingTimeoutNanos =
                CWindowManagerService::DEFAULT_INPUT_DISPATCHING_TIMEOUT_NANOS;

        Int32 displayId;
        mDisplay->GetDisplayId(&displayId);
        mDragWindowHandle = new InputWindowHandle(mDragApplicationHandle, NULL, displayId);
        mDragWindowHandle->mName = String("drag");
        mDragWindowHandle->mInputChannel = mServerChannel;
        mDragWindowHandle->mLayer = GetDragLayerLw();
        mDragWindowHandle->mLayoutParamsFlags = 0;
        mDragWindowHandle->mLayoutParamsPrivateFlags = 0;
        mDragWindowHandle->mLayoutParamsType = IWindowManagerLayoutParams::TYPE_DRAG;
        mDragWindowHandle->mDispatchingTimeoutNanos =
                CWindowManagerService::DEFAULT_INPUT_DISPATCHING_TIMEOUT_NANOS;
        mDragWindowHandle->mVisible = TRUE;
        mDragWindowHandle->mCanReceiveKeys = FALSE;
        mDragWindowHandle->mHasFocus = TRUE;
        mDragWindowHandle->mHasWallpaper = FALSE;
        mDragWindowHandle->mPaused = FALSE;
        mDragWindowHandle->mOwnerPid = Process::MyPid();
        mDragWindowHandle->mOwnerUid = Process::MyUid();
        mDragWindowHandle->mInputFeatures = 0;
        mDragWindowHandle->mScaleFactor = 1.0f;

        // The drag window cannot receive new touches.
        mDragWindowHandle->mTouchableRegion->SetEmpty();

        // The drag window covers the entire display
        mDragWindowHandle->mFrameLeft = 0;
        mDragWindowHandle->mFrameTop = 0;
        AutoPtr<IPoint> p;
        CPoint::New((IPoint**)&p);
        mDisplay->GetRealSize(p);
        p->GetX(&mDragWindowHandle->mFrameRight);
        p->GetY(&mDragWindowHandle->mFrameBottom);

        // Pause rotations before a drag.
        // if (WindowManagerService.DEBUG_ORIENTATION) {
        //     Slog.d(WindowManagerService.TAG, "Pausing rotation during drag");
        // }
        mService->PauseRotationLocked();
    }
}

void DragState::Unregister()
{
    // if (WindowManagerService.DEBUG_DRAG) Slog.d(WindowManagerService.TAG, "unregistering drag input channel");
    if (mClientChannel == NULL) {
        Slogger::E(CWindowManagerService::TAG, "Unregister of nonexistent drag input channel");
    }
    else {
        mService->mInputManager->UnregisterInputChannel(mServerChannel);
        mInputEventReceiver->Dispose();
        mInputEventReceiver = NULL;
        mClientChannel->Dispose();
        mServerChannel->Dispose();
        mClientChannel = NULL;
        mServerChannel = NULL;

        mDragWindowHandle = NULL;
        mDragApplicationHandle = NULL;

        // Resume rotations after a drag.
        if (CWindowManagerService::DEBUG_ORIENTATION) {
            Slogger::D(CWindowManagerService::TAG, "Resuming rotation after drag");
        }
        mService->ResumeRotationLocked();
    }
}

Int32 DragState::GetDragLayerLw()
{
    Int32 result;
    mService->mPolicy->WindowTypeToLayerLw(IWindowManagerLayoutParams::TYPE_DRAG, &result);
    return result * CWindowManagerService::TYPE_LAYER_MULTIPLIER
            + CWindowManagerService::TYPE_LAYER_OFFSET;
}

void DragState::BroadcastDragStartedLw(
    /* [in] */ Float touchX,
    /* [in] */ Float touchY)
{
    // Cache a base-class instance of the clip metadata so that parceling
    // works correctly in calling out to the apps.
    mDataDescription = NULL;
    if (mData != NULL) {
        mData->GetDescription((IClipDescription**)&mDataDescription);
    }
    mNotifiedWindows.Clear();
    mDragInProgress = TRUE;

    // if (WindowManagerService.DEBUG_DRAG) {
    //     Slog.d(WindowManagerService.TAG, "broadcasting DRAG_STARTED at (" + touchX + ", " + touchY + ")");
    // }

    Int32 id;
    mDisplay->GetDisplayId(&id);
    AutoPtr<DisplayContent> displayContent = mService->GetDisplayContentLocked(id);
    if (displayContent != NULL) {
        AutoPtr<List<AutoPtr<WindowState> > > windows = displayContent->GetWindowList();
        List<AutoPtr<WindowState> >::Iterator it = windows->Begin();
        for (; it != windows->End(); ++it) {
            SendDragStartedLw(*it, touchX, touchY, mDataDescription);
        }
    }
}

void DragState::SendDragStartedLw(
    /* [in] */ WindowState* newWin,
    /* [in] */ Float touchX,
    /* [in] */ Float touchY,
    /* [in] */ IClipDescription* desc)
{
    // Don't actually send the event if the drag is supposed to be pinned
    // to the originating window but 'newWin' is not that window.
    if ((mFlags & IView::DRAG_FLAG_GLOBAL) == 0) {
        AutoPtr<IBinder> winBinder = IBinder::Probe(newWin->mClient);
        if (winBinder != mLocalWin) {
            if (CWindowManagerService::DEBUG_DRAG) {
                Slogger::D(CWindowManagerService::TAG, "Not dispatching local DRAG_STARTED to %p", newWin);
            }
            return;
        }
    }

    if (mDragInProgress && newWin->IsPotentialDragTarget()) {
        AutoPtr<IDragEvent> event = ObtainDragEvent(newWin, IDragEvent::ACTION_DRAG_STARTED,
                touchX, touchY, NULL, desc, NULL, FALSE);
        // try {
        if (FAILED(newWin->mClient->DispatchDragEvent(event))) {
            if (Process::MyPid() != newWin->mSession->mPid) {
                event->Recycle();
            }
            return;
        }
        // track each window that we've notified that the drag is starting
        mNotifiedWindows.PushBack(newWin);
        // } catch (RemoteException e) {
        //     Slog.w(WindowManagerService.TAG, "Unable to drag-start window " + newWin);
        // } finally {
        //     // if the callee was local, the dispatch has already recycled the event
        //     if (Process.myPid() != newWin.mSession.mPid) {
        //         event.recycle();
        //     }
        // }
        if (Process::MyPid() != newWin->mSession->mPid) {
            event->Recycle();
        }
    }
}

void DragState::SendDragStartedIfNeededLw(
    /* [in] */ WindowState* newWin)
{
    if (mDragInProgress) {
        // If we have sent the drag-started, we needn't do so again
        List<AutoPtr<WindowState> >::Iterator it = mNotifiedWindows.Begin();
        for (; it != mNotifiedWindows.End(); ++it) {
            if ((*it).Get() == newWin) {
                return;
            }
        }
        // if (WindowManagerService.DEBUG_DRAG) {
        //     Slog.d(WindowManagerService.TAG, "need to send DRAG_STARTED to new window " + newWin);
        // }
        SendDragStartedLw(newWin, mCurrentX, mCurrentY, mDataDescription);
    }
}

void DragState::BroadcastDragEndedLw()
{
    // if (WindowManagerService.DEBUG_DRAG) {
    //     Slog.d(WindowManagerService.TAG, "broadcasting DRAG_ENDED");
    // }
    AutoPtr<IDragEventHelper> dragEventHelper;
    CDragEventHelper::AcquireSingleton((IDragEventHelper**)&dragEventHelper);
    AutoPtr<IDragEvent> evt;
    ASSERT_SUCCEEDED(dragEventHelper->Obtain(IDragEvent::ACTION_DRAG_ENDED,
            0, 0, NULL, NULL, NULL, mDragResult, (IDragEvent**)&evt));
    List<AutoPtr<WindowState> >::Iterator it = mNotifiedWindows.Begin();
    for (; it != mNotifiedWindows.End(); ++it) {
        AutoPtr<WindowState> ws = *it;
        // try {
        ws->mClient->DispatchDragEvent(evt);
        // } catch (RemoteException e) {
        //     Slog.w(WindowManagerService.TAG, "Unable to drag-end window " + ws);
        // }
    }
    mNotifiedWindows.Clear();
    mDragInProgress = FALSE;
    evt->Recycle();
}

void DragState::EndDragLw()
{
    mService->mDragState->BroadcastDragEndedLw();

    // stop intercepting input
    mService->mDragState->Unregister();
    mService->mInputMonitor->UpdateInputWindowsLw(TRUE /*force*/);

    // free our resources and drop all the object references
    mService->mDragState->Reset();
    mService->mDragState = NULL;
}

void DragState::NotifyMoveLw(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Int32 myPid = Process::MyPid();

    // Move the surface to the given touch
    if (CWindowManagerService::SHOW_LIGHT_TRANSACTIONS) Slogger::I(
            CWindowManagerService::TAG, ">>> OPEN TRANSACTION notifyMoveLw");
    AutoPtr<ISurfaceControlHelper> surfaceHelper;
    CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&surfaceHelper);
    surfaceHelper->OpenTransaction();
    // try {
    mSurfaceControl->SetPosition(x - mThumbOffsetX, y - mThumbOffsetY);
    // if (WindowManagerService.SHOW_TRANSACTIONS) Slog.i(WindowManagerService.TAG, "  DRAG "
    //         + mSurfaceControl + ": pos=(" +
    //         (int)(x - mThumbOffsetX) + "," + (int)(y - mThumbOffsetY) + ")");
    // } finally {
    surfaceHelper->CloseTransaction();
    if (CWindowManagerService::SHOW_LIGHT_TRANSACTIONS) Slogger::I(
            CWindowManagerService::TAG, "<<< CLOSE TRANSACTION notifyMoveLw");
    // }

    // Tell the affected window
    AutoPtr<WindowState> touchedWin = GetTouchedWinAtPointLw(x, y);
    if (touchedWin == NULL) {
        if (CWindowManagerService::DEBUG_DRAG) Slogger::D(CWindowManagerService::TAG, "No touched win at x=%f y=%f", x, y);
        return;
    }
    if ((mFlags & IView::DRAG_FLAG_GLOBAL) == 0) {
        AutoPtr<IBinder> touchedBinder = IBinder::Probe(touchedWin->mClient);
        if (touchedBinder != mLocalWin) {
            // This drag is pinned only to the originating window, but the drag
            // point is outside that window.  Pretend it's over empty space.
            touchedWin = NULL;
        }
    }
    // try {
    // have we dragged over a new window?
    if ((touchedWin != mTargetWindow) && (mTargetWindow != NULL)) {
        // if (WindowManagerService.DEBUG_DRAG) {
        //     Slog.d(WindowManagerService.TAG, "sending DRAG_EXITED to " + mTargetWindow);
        // }
        // force DRAG_EXITED_EVENT if appropriate
        AutoPtr<IDragEvent> evt = ObtainDragEvent(mTargetWindow,
                IDragEvent::ACTION_DRAG_EXITED,
                x, y, NULL, NULL, NULL, FALSE);
        if (FAILED(mTargetWindow->mClient->DispatchDragEvent(evt))) {
            Slogger::W(CWindowManagerService::TAG, "can't send drag notification to windows");
            goto fail;
        }
        if (myPid != mTargetWindow->mSession->mPid) {
            evt->Recycle();
        }
    }
    if (touchedWin != NULL) {
        // if (false && WindowManagerService.DEBUG_DRAG) {
        //     Slog.d(WindowManagerService.TAG, "sending DRAG_LOCATION to " + touchedWin);
        // }
        AutoPtr<IDragEvent> evt = ObtainDragEvent(touchedWin,
                IDragEvent::ACTION_DRAG_LOCATION,
                x, y, NULL, NULL, NULL, FALSE);
        if (FAILED(touchedWin->mClient->DispatchDragEvent(evt))) {
            Slogger::W(CWindowManagerService::TAG, "can't send drag notification to windows");
            goto fail;
        }
        if (myPid != touchedWin->mSession->mPid) {
            evt->Recycle();
        }
    }
    // } catch (RemoteException e) {
    //     Slog.w(WindowManagerService.TAG, "can't send drag notification to windows");
    // }

fail:
    mTargetWindow = touchedWin;
}

Boolean DragState::NotifyDropLw(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    AutoPtr<WindowState> touchedWin = GetTouchedWinAtPointLw(x, y);
    if (touchedWin == NULL) {
        // "drop" outside a valid window -- no recipient to apply a
        // timeout to, and we can send the drag-ended message immediately.
        mDragResult = FALSE;
        return TRUE;
    }

    // if (WindowManagerService.DEBUG_DRAG) {
    //     Slog.d(WindowManagerService.TAG, "sending DROP to " + touchedWin);
    // }
    Int32 myPid = Process::MyPid();
    AutoPtr<IBinder> token = IBinder::Probe(touchedWin->mClient);
    AutoPtr<IDragEvent> evt = ObtainDragEvent(touchedWin, IDragEvent::ACTION_DROP,
            x, y, NULL, NULL, mData, FALSE);
    // try {
    if (FAILED(touchedWin->mClient->DispatchDragEvent(evt))) {
        // Slog.w(WindowManagerService.TAG, "can't send drop notification to win " + touchedWin);
        if (myPid != touchedWin->mSession->mPid) {
            evt->Recycle();
        }
        return TRUE;
    }

    // 5 second timeout for this window to respond to the drop
    AutoPtr<IMessage> msg;
    mService->mH->ObtainMessage(CWindowManagerService::H::DRAG_END_TIMEOUT,
        token.Get(), (IMessage**)&msg);
    mService->mH->RemoveMessages(CWindowManagerService::H::DRAG_END_TIMEOUT, token.Get());
    Boolean result;
    mService->mH->SendMessageDelayed(msg, 5000, &result);

    // } catch (RemoteException e) {
    //     Slog.w(WindowManagerService.TAG, "can't send drop notification to win " + touchedWin);
    //     return true;
    // } finally {
    if (myPid != touchedWin->mSession->mPid) {
        evt->Recycle();
    }
    // }
    mToken = token;
    return FALSE;
}

AutoPtr<WindowState> DragState::GetTouchedWinAtPointLw(
    /* [in] */ Float xf,
    /* [in] */ Float yf)
{
    AutoPtr<WindowState> touchedWin;
    Int32 x = (Int32) xf;
    Int32 y = (Int32) yf;

    Int32 id;
    mDisplay->GetDisplayId(&id);
    AutoPtr<DisplayContent> displayContent = mService->GetDisplayContentLocked(id);
    if (displayContent == NULL) {
        return NULL;
    }

    AutoPtr<List<AutoPtr<WindowState> > > windows = displayContent->GetWindowList();
    List<AutoPtr<WindowState> >::ReverseIterator rit = windows->RBegin();
    for (; rit != windows->REnd(); ++rit) {
        AutoPtr<WindowState> child = *rit;
        Int32 flags;
        child->mAttrs->GetFlags(&flags);
        Boolean isVisible;
        child->IsVisibleLw(&isVisible);
        if (!isVisible) {
            // not visible == don't tell about drags
            continue;
        }
        if ((flags & IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE) != 0) {
            // not touchable == don't tell about drags
            continue;
        }

        child->GetTouchableRegion(mTmpRegion);

        Int32 touchFlags = flags &
                (IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
                        | IWindowManagerLayoutParams::FLAG_NOT_TOUCH_MODAL);
        Boolean result;
        mTmpRegion->Contains(x, y, &result);
        if (result || touchFlags == 0) {
            // Found it
            touchedWin = child;
            break;
        }
    }

    return touchedWin;
}

AutoPtr<IDragEvent> DragState::ObtainDragEvent(
    /* [in] */ WindowState* win,
    /* [in] */ Int32 action,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IInterface* localState,
    /* [in] */ IClipDescription* description,
    /* [in] */ IClipData* data,
    /* [in] */ Boolean result)
{
    Int32 left, top;
    win->mFrame->GetLeft(&left);
    win->mFrame->GetTop(&top);
    Float winX = x - left;
    Float winY = y - top;
    if (win->mEnforceSizeCompat) {
        winX *= win->mGlobalScale;
        winY *= win->mGlobalScale;
    }

    AutoPtr<IDragEventHelper> dragEventHelper;
    CDragEventHelper::AcquireSingleton((IDragEventHelper**)&dragEventHelper);
    AutoPtr<IDragEvent> evt;
    ASSERT_SUCCEEDED(dragEventHelper->Obtain(action, winX, winY, localState,
            description, data, result, (IDragEvent**)&evt));
    return evt;
}


} // Wm
} // Server
} // Droid
} // Elastos
