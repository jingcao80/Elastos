
#include "elastos/droid/server/wm/CSession.h"
#include "elastos/droid/server/wm/DragState.h"
#include "elastos/droid/server/wm/InputMonitor.h"
#include "elastos/droid/server/wm/DisplayContent.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/ServiceManager.h"
#include <Elastos.Droid.Internal.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Internal::View::IIInputMethodManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::View::EIID_IWindowSession;
using Elastos::Droid::View::CSurface;
using Elastos::Droid::View::ISurfaceControlHelper;
using Elastos::Droid::View::CSurfaceControlHelper;
using Elastos::Droid::View::CSurfaceSession;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

CSession::CSession()
    : mUid(0)
    , mPid(0)
    , mNumWindow(0)
    , mClientDead(FALSE)
    , mLastReportedAnimatorScale(0)
{}

CAR_INTERFACE_IMPL_2(CSession, Object, IWindowSession, IProxyDeathRecipient)

CAR_OBJECT_IMPL(CSession)

ECode CSession::constructor(
    /* [in] */ IIWindowManager* wmService,
    /* [in] */ IIWindowSessionCallback* callback,
    /* [in] */ IInputMethodClient* client,
    /* [in] */ IIInputContext* inputContext)
{
    mService = (CWindowManagerService*)wmService;
    mCallback = callback;
    mNumWindow = 0;
    mClient = client;
    mInputContext = inputContext;
    mUid = Binder::GetCallingUid();
    mPid = Binder::GetCallingPid();
    mService->GetCurrentAnimatorScale(&mLastReportedAnimatorScale);
    StringBuilder sb;
    sb.Append("Session{");
    sb.Append(StringUtils::ToString((Int32)this));
    sb.Append(" ");
    sb.Append(mPid);
    if (mUid < IProcess::FIRST_APPLICATION_UID) {
        sb.Append(":");
        sb.Append(mUid);
    }
    else {
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        sb.Append(":u");
        Int32 id;
        helper->GetUserId(mUid, &id);
        sb.Append(id);
        sb.AppendChar('a');
        helper->GetAppId(mUid, &id);
        sb.Append(id);
    }
    sb.Append("}");
    sb.ToString(&mStringName);

    {
        AutoLock lock(mService->mWindowMapLock);
        if (mService->mInputMethodManager == NULL && mService->mHaveInputMethods) {
            mService->mInputMethodManager = IIInputMethodManager::Probe(
                    ServiceManager::GetService(IContext::INPUT_METHOD_SERVICE).Get());
        }
    }

    Int64 ident = Binder::ClearCallingIdentity();
    ECode ec = NOERROR;
    AutoPtr<IProxy> proxy;
    // try {
    // Note: it is safe to call in to the input method manager
    // here because we are not holding our lock.
    if (mService->mInputMethodManager != NULL) {
        ec = mService->mInputMethodManager->AddClient(client, inputContext, mUid, mPid);
        FAIL_GOTO(ec, _Exit_)
    }
    else {
        ec = client->SetUsingInputMethod(FALSE);
        FAIL_GOTO(ec, _Exit_)
    }

    proxy = (IProxy*)mClient->Probe(EIID_IProxy);
    if (proxy != NULL) {
        ec = proxy->LinkToDeath(this, 0);
    }

_Exit_:
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        if (mService->mInputMethodManager != NULL) {
            mService->mInputMethodManager->RemoveClient(client);
        }
    }

    Binder::RestoreCallingIdentity(ident);
    return ec;
}

ECode CSession::Add(
    /* [in] */ IIWindow* window,
    /* [in] */ Int32 seq,
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [in] */ Int32 viewVisibility,
    /* [in] */ IRect* inContentInsets,
    /* [in] */ IInputChannel* inInputChannel,
    /* [out] */ IRect** outContentInsets,
    /* [out] */ IInputChannel** outInputChannel,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(outContentInsets);
    VALIDATE_NOT_NULL(outInputChannel);
    VALIDATE_NOT_NULL(result);
    assert(outContentInsets && (inContentInsets == NULL || inContentInsets != *outContentInsets));
    assert(outInputChannel && (inInputChannel == NULL || inInputChannel != *outInputChannel));
    return AddToDisplay(window, seq, attrs, viewVisibility, IDisplay::DEFAULT_DISPLAY,
                inContentInsets, inInputChannel, outContentInsets,
                outInputChannel, result);
}

ECode CSession::AddToDisplay(
    /* [in] */ IIWindow* window,
    /* [in] */ Int32 seq,
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [in] */ Int32 viewVisibility,
    /* [in] */ Int32 displayId,
    /* [in] */ IRect* inContentInsets,
    /* [in] */ IInputChannel* inInputChannel,
    /* [out] */ IRect** outContentInsets,
    /* [out] */ IInputChannel** outInputChannel,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(outContentInsets);
    VALIDATE_NOT_NULL(outInputChannel);
    VALIDATE_NOT_NULL(result);
    assert(mService);
    assert(outContentInsets && (inContentInsets == NULL || inContentInsets != *outContentInsets));
    assert(outInputChannel && (inInputChannel == NULL || inInputChannel != *outInputChannel));
    *result = mService->AddWindow(this, window, seq, attrs, viewVisibility, displayId,
            inContentInsets, inInputChannel, outContentInsets, outInputChannel);
    return NOERROR;
}

ECode CSession::AddWithoutInputChannel(
    /* [in] */ IIWindow* window,
    /* [in] */ Int32 seq,
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [in] */ Int32 viewVisibility,
    /* [in] */ IRect* inContentInsets,
    /* [out] */ IRect** outContentInsets,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(outContentInsets);
    VALIDATE_NOT_NULL(result);
    assert(outContentInsets && (inContentInsets == NULL || inContentInsets != *outContentInsets));
    return AddToDisplayWithoutInputChannel(window, seq, attrs, viewVisibility,
            IDisplay::DEFAULT_DISPLAY, inContentInsets, outContentInsets, result);
}

ECode CSession::AddToDisplayWithoutInputChannel(
    /* [in] */ IIWindow* window,
    /* [in] */ Int32 seq,
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [in] */ Int32 viewVisibility,
    /* [in] */ Int32 layerStackId,
    /* [in] */ IRect* inContentInsets,
    /* [out] */ IRect** outContentInsets,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(outContentInsets)
    VALIDATE_NOT_NULL(result)
    assert(mService);
    assert(outContentInsets && (inContentInsets == NULL || inContentInsets != *outContentInsets));

    *result = mService->AddWindow(this, window, seq, attrs, viewVisibility, layerStackId,
            inContentInsets, NULL, outContentInsets, NULL);
    return NOERROR;
}

ECode CSession::Remove(
    /* [in] */ IIWindow* window)
{
    mService->RemoveWindow(this, window);
    return NOERROR;
}

ECode CSession::Relayout(
    /* [in] */ IIWindow* window,
    /* [in] */ Int32 seq,
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [in] */ Int32 requestedWidth,
    /* [in] */ Int32 requestedHeight,
    /* [in] */ Int32 viewFlags,
    /* [in] */ Int32 flags,
    /* [in] */ IRect* inFrame,
    /* [in] */ IRect* inOverscanInsets,
    /* [in] */ IRect* inContentInsets,
    /* [in] */ IRect* inVisibleInsets,
    /* [in] */ IRect* inStableInsets,
    /* [in] */ IConfiguration* inConfig,
    /* [in] */ ISurface* inSurface,
    /* [out] */ IRect** outFrame,
    /* [out] */ IRect** outOverscanInsets,
    /* [out] */ IRect** outContentInsets,
    /* [out] */ IRect** outVisibleInsets,
    /* [out] */ IRect** outStableInsets,
    /* [out] */ IConfiguration** outConfig,
    /* [out] */ Int32* result,
    /* [out] */ ISurface** outSurface)
{
    VALIDATE_NOT_NULL(outFrame)
    VALIDATE_NOT_NULL(outOverscanInsets)
    VALIDATE_NOT_NULL(outContentInsets)
    VALIDATE_NOT_NULL(outVisibleInsets)
    VALIDATE_NOT_NULL(outStableInsets)
    VALIDATE_NOT_NULL(outConfig)
    VALIDATE_NOT_NULL(outSurface)
    VALIDATE_NOT_NULL(result)

    assert(outFrame && (inFrame == NULL || inFrame != *outFrame));
    assert(outContentInsets && (inContentInsets == NULL || inContentInsets != *outContentInsets));
    assert(outVisibleInsets && (inVisibleInsets == NULL || inVisibleInsets != *outVisibleInsets));
    assert(outConfig && (inConfig == NULL || inConfig != *outConfig));
    assert(outSurface && (inSurface == NULL || inSurface != *outSurface));

    // if (false) Slog.d(WindowManagerService.TAG, ">>>>>> ENTERED relayout from "
    //         + Binder.getCallingPid());
    assert(mService);
    *result = mService->RelayoutWindow(this, window, seq, attrs,
            requestedWidth, requestedHeight, viewFlags, flags,
            inFrame, inOverscanInsets, inContentInsets, inVisibleInsets, inStableInsets, inConfig, inSurface,
            outFrame, outOverscanInsets, outContentInsets, outVisibleInsets, outStableInsets, outConfig, outSurface);
    // if (false) Slog.d(WindowManagerService.TAG, "<<<<<< EXITING relayout to "
    //         + Binder.getCallingPid());
    return NOERROR;
}

ECode CSession::PerformDeferredDestroy(
    /* [in] */ IIWindow* window)
{
    mService->PerformDeferredDestroyWindow(this, window);
    return NOERROR;
}

ECode CSession::OutOfMemory(
    /* [in] */ IIWindow* window,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mService->OutOfMemoryWindow(this, window);
    return NOERROR;
}

ECode CSession::SetTransparentRegion(
    /* [in] */ IIWindow* window,
    /* [in] */ IRegion* region)
{
    mService->SetTransparentRegionWindow(this, window, region);
    return NOERROR;
}

ECode CSession::SetInsets(
    /* [in] */ IIWindow* window,
    /* [in] */ Int32 touchableInsets,
    /* [in] */ IRect* contentInsets,
    /* [in] */ IRect* visibleInsets,
    /* [in] */ IRegion* touchableRegion)
{
    mService->SetInsetsWindow(this, window, touchableInsets, contentInsets,
            visibleInsets, touchableRegion);
    return NOERROR;
}

ECode CSession::GetDisplayFrame(
    /* [in] */ IIWindow* window,
    /* [out] */ IRect** outDisplayFrame)
{
    VALIDATE_NOT_NULL(outDisplayFrame);
    FAIL_RETURN(CRect::New(outDisplayFrame));
    mService->GetWindowDisplayFrame(this, window, *outDisplayFrame);
    return NOERROR;
}

ECode CSession::FinishDrawing(
    /* [in] */ IIWindow* window)
{
    // if (WindowManagerService.localLOGV) Slog.v(
    //         WindowManagerService.TAG, "IWindow finishDrawing called for " + window);
    mService->FinishDrawingWindow(this, window);
    return NOERROR;
}

ECode CSession::SetInTouchMode(
    /* [in] */ Boolean mode)
{
    AutoLock lock(mService->mWindowMapLock);
    mService->mInTouchMode = mode;
    return NOERROR;
}

ECode CSession::GetInTouchMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mService->mInTouchMode;
    return NOERROR;
}

ECode CSession::PerformHapticFeedback(
    /* [in] */ IIWindow* window,
    /* [in] */ Int32 effectId,
    /* [in] */ Boolean always,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mService->mWindowMapLock);
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<WindowState> win;
    if (FAILED(mService->WindowForClientLocked(this, window, TRUE, (WindowState**)&win))) {
        Binder::RestoreCallingIdentity(ident);
        return NOERROR;
    }
    ECode ec = mService->mPolicy->PerformHapticFeedbackLw(
            win, effectId, always, result);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return ec;
}

ECode CSession::PrepareDrag(
    /* [in] */ IIWindow* window,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ ISurface** outSurface,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(outSurface)
    VALIDATE_NOT_NULL(binder)
    FAIL_RETURN(CSurface::New(outSurface))
    AutoPtr<IBinder> b = mService->PrepareDragSurface(window, mSurfaceSession, flags,
            width, height, *outSurface);
    *binder = b;
    REFCOUNT_ADD(*binder)
    return NOERROR;
}

ECode CSession::PerformDrag(
    /* [in] */ IIWindow* window,
    /* [in] */ IBinder* dragToken,
    /* [in] */ Float touchX,
    /* [in] */ Float touchY,
    /* [in] */ Float thumbCenterX,
    /* [in] */ Float thumbCenterY,
    /* [in] */ IClipData* data,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // if (WindowManagerService.DEBUG_DRAG) {
    //     Slog.d(WindowManagerService.TAG, "perform drag: win=" + window + " data=" + data);
    // }

    AutoLock lock(mService->mWindowMapLock);
    if (mService->mDragState == NULL) {
        Slogger::W(CWindowManagerService::TAG, "No drag prepared");
        *result = false;
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("performDrag() without prepareDrag()");
    }

    if (dragToken != mService->mDragState->mToken) {
        Slogger::W(CWindowManagerService::TAG, "Performing mismatched drag");
        *result = false;
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("performDrag() does not match prepareDrag()");
    }

    AutoPtr<WindowState> callingWin;
    mService->WindowForClientLocked(NULL, window, FALSE, (WindowState**)&callingWin);
    if (callingWin == NULL) {
        Slogger::W(CWindowManagerService::TAG, "Bad requesting window %p", window);
        *result = false;
        return NOERROR;  // !!! TODO: throw here?
    }

    // !!! TODO: if input is not still focused on the initiating window, fail
    // the drag initiation (e.g. an alarm window popped up just as the application
    // called performDrag()
    mService->mH->RemoveMessages(
        CWindowManagerService::H::DRAG_START_TIMEOUT, window);

    // !!! TODO: extract the current touch (x, y) in screen coordinates.  That
    // will let us eliminate the (touchX,touchY) parameters from the API.

    // !!! FIXME: put all this heavy stuff onto the mH looper, as well as
    // the actual drag event dispatch stuff in the dragstate

    AutoPtr<DisplayContent> displayContent = callingWin->GetDisplayContent();
    if (displayContent == NULL) {
       *result = FALSE;
       return NOERROR;
    }
    AutoPtr<IDisplay> display = displayContent->GetDisplay();
    mService->mDragState->Register(display);
    mService->mInputMonitor->UpdateInputWindowsLw(TRUE /*force*/);
    Boolean ret;
    if (mService->mInputManager->TransferTouchFocus(callingWin->mInputChannel,
            mService->mDragState->mServerChannel, &ret), !ret) {
        Slogger::E(CWindowManagerService::TAG, "Unable to transfer touch focus");
        mService->mDragState->Unregister();
        mService->mDragState = NULL;
        mService->mInputMonitor->UpdateInputWindowsLw(TRUE /*force*/);
        *result = FALSE;
        return NOERROR;
    }

    mService->mDragState->mData = data;
    mService->mDragState->mCurrentX = touchX;
    mService->mDragState->mCurrentY = touchY;
    mService->mDragState->BroadcastDragStartedLw(touchX, touchY);

    // remember the thumb offsets for later
    mService->mDragState->mThumbOffsetX = thumbCenterX;
    mService->mDragState->mThumbOffsetY = thumbCenterY;

    // Make the surface visible at the proper location
    AutoPtr<ISurfaceControl> surfaceControl = mService->mDragState->mSurfaceControl;
    if (CWindowManagerService::SHOW_LIGHT_TRANSACTIONS) {
        Slogger::I(CWindowManagerService::TAG, ">>> OPEN TRANSACTION performDrag");
    }
    AutoPtr<ISurfaceControlHelper> helper;
    CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&helper);
    helper->OpenTransaction();
    // try {
    surfaceControl->SetPosition(touchX - thumbCenterX,
            touchY - thumbCenterY);
    surfaceControl->SetAlpha(0.7071);
    surfaceControl->SetLayer(mService->mDragState->GetDragLayerLw());
    Int32 layerStack;
    display->GetLayerStack(&layerStack);
    surfaceControl->SetLayerStack(layerStack);
    surfaceControl->Show();
    // } finally {
    helper->CloseTransaction();
    if (CWindowManagerService::SHOW_LIGHT_TRANSACTIONS) {
        Slogger::I(CWindowManagerService::TAG, "<<< CLOSE TRANSACTION performDrag");
    }

    *result = TRUE;
    return NOERROR;
}

ECode CSession::ReportDropResult(
    /* [in] */ IIWindow* window,
    /* [in] */ Boolean consumed)
{
    AutoPtr<IBinder> token = IBinder::Probe(window);
    if (CWindowManagerService::DEBUG_DRAG) {
        Slogger::D(CWindowManagerService::TAG, "Drop result=%d reported by %p", consumed, token.Get());
    }

    AutoLock lock(mService->mWindowMapLock);
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    if (mService->mDragState == NULL) {
        // Most likely the drop recipient ANRed and we ended the drag
        // out from under it.  Log the issue and move on.
        Slogger::W(CWindowManagerService::TAG, "Drop result given but no drag in progress");
        Binder::RestoreCallingIdentity(ident);
        return NOERROR;
    }

    if (mService->mDragState->mToken != token) {
        // We're in a drag, but the wrong window has responded.
        Slogger::W(CWindowManagerService::TAG, "Invalid drop-result claim by %p", window);
        Binder::RestoreCallingIdentity(ident);
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("reportDropResult() by non-recipient");
    }

    // The right window has responded, even if it's no longer around,
    // so be sure to halt the timeout even if the later WindowState
    // lookup fails.
    mService->mH->RemoveMessages(
        CWindowManagerService::H::DRAG_END_TIMEOUT, window);

    AutoPtr<WindowState> callingWin;
    mService->WindowForClientLocked(NULL, window, FALSE, (WindowState**)&callingWin);
    if (callingWin == NULL) {
        Slogger::W(CWindowManagerService::TAG, "Bad result-reporting window %p", window);
        Binder::RestoreCallingIdentity(ident);
        return NOERROR;  // !!! TODO: throw here?
    }

    mService->mDragState->mDragResult = consumed;
    mService->mDragState->EndDragLw();
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode CSession::DragRecipientEntered(
    /* [in] */ IIWindow* window)
{
    if (CWindowManagerService::DEBUG_DRAG) {
        Slogger::D(CWindowManagerService::TAG, "Drag into new candidate view @ %p", IBinder::Probe(window));
    }
    return NOERROR;
}

ECode CSession::DragRecipientExited(
    /* [in] */ IIWindow* window)
{
    if (CWindowManagerService::DEBUG_DRAG) {
        Slogger::D(CWindowManagerService::TAG, "Drag from old candidate view @ %p", IBinder::Probe(window));
    }
    return NOERROR;
}

ECode CSession::SetWallpaperPosition(
    /* [in] */ IBinder* window,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float xstep,
    /* [in] */ Float ystep)
{
    AutoLock lock(mService->mWindowMapLock);
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<WindowState> win;
    if (FAILED(mService->WindowForClientLocked(this, window, TRUE, (WindowState**)&win))) {
        Binder::RestoreCallingIdentity(ident);
        return NOERROR;
    }
    mService->SetWindowWallpaperPositionLocked(win, x, y, xstep, ystep);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }

    return NOERROR;
}

ECode CSession::WallpaperOffsetsComplete(
    /* [in] */ IBinder* window)
{
    mService->WallpaperOffsetsComplete(window);
    return NOERROR;
}

ECode CSession::SetWallpaperDisplayOffset(
    /* [in] */ IBinder* windowToken,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    AutoLock lock(mService->mWindowMapLock);
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<WindowState> ws;
    mService->WindowForClientLocked(this, windowToken, TRUE, (WindowState**)&ws);
    mService->SetWindowWallpaperDisplayOffsetLocked(ws, x, y);
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

ECode CSession::SendWallpaperCommand(
    /* [in] */ IBinder* window,
    /* [in] */ const String& action,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean sync,
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)

    AutoLock lock(mService->mWindowMapLock);
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<WindowState> win;
    if (FAILED(mService->WindowForClientLocked(this, window, TRUE, (WindowState**)&win))) {
        Binder::RestoreCallingIdentity(ident);
        return NOERROR;
    }
    AutoPtr<IBundle> bundle = mService->SendWindowWallpaperCommandLocked(
            win, action, x, y, z, extras, sync);
    *result = bundle;
    REFCOUNT_ADD(*result);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode CSession::WallpaperCommandComplete(
    /* [in] */ IBinder* window,
    /* [in] */ IBundle* result)
{
    mService->WallpaperCommandComplete(window, result);
    return NOERROR;
}

ECode CSession::SetUniverseTransform(
    /* [in] */ IBinder* window,
    /* [in] */ Float alpha,
    /* [in] */ Float offx,
    /* [in] */ Float offy,
    /* [in] */ Float dsdx,
    /* [in] */ Float dtdx,
    /* [in] */ Float dsdy,
    /* [in] */ Float dtdy)
{
    AutoLock lock(mService->mWindowMapLock);
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<WindowState> win;
    if (FAILED(mService->WindowForClientLocked(this, window, TRUE, (WindowState**)&win))) {
        Binder::RestoreCallingIdentity(ident);
        return NOERROR;
    }
    mService->SetUniverseTransformLocked(
            win, alpha, offx, offy, dsdx, dtdx, dsdy, dtdy);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return NOERROR;
}

ECode CSession::OnRectangleOnScreenRequested(
    /* [in] */ IBinder* token,
    /* [in] */ IRect* rectangle)
{
    AutoLock lock(mService->mWindowMapLock);
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    mService->OnRectangleOnScreenRequested(token, rectangle);
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return NOERROR;
}

ECode CSession::GetWindowId(
    /* [in] */ IBinder* window,
    /* [out] */ IIWindowId** winId)
{
    VALIDATE_NOT_NULL(winId)
    *winId = mService->GetWindowId(window);
    REFCOUNT_ADD(*winId)
    return NOERROR;
}

ECode CSession::GetLastWallpaperX(
    /* [out] */ Int32* x)
{
    AutoLock lock(mService->mWindowMapLock);
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    mService->GetLastWallpaperX(x);
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

/**
 * Get the current y offset for the wallpaper
 */
ECode CSession::GetLastWallpaperY(
    /* [out] */ Int32* y)
{
    AutoLock lock(mService->mWindowMapLock);
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    mService->GetLastWallpaperY(y);
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}
void CSession::WindowAddedLocked()
{
    if (mSurfaceSession == NULL) {
        // if (WindowManagerService.localLOGV) Slog.v(
        //         WindowManagerService.TAG, "First window added to " + this + ", creating SurfaceSession");
        ASSERT_SUCCEEDED(CSurfaceSession::New((ISurfaceSession**)&mSurfaceSession));
        if (CWindowManagerService::SHOW_TRANSACTIONS) {
            Slogger::I(CWindowManagerService::TAG, "  NEW SURFACE SESSION %p", mSurfaceSession.Get());
        }
        mService->mSessions->Add((IWindowSession*)this);
        Float scale;
        mService->GetCurrentAnimatorScale(&scale);
        if (mLastReportedAnimatorScale != scale) {
            mService->DispatchNewAnimatorScaleLocked(this);
        }
    }
    mNumWindow++;
}

void CSession::WindowRemovedLocked()
{
    mNumWindow--;
    KillSessionLocked();
}

void CSession::KillSessionLocked()
{
    if (mNumWindow <= 0 && mClientDead) {
        mService->mSessions->Remove((IWindowSession*)this);
        if (mSurfaceSession != NULL) {
            // if (WindowManagerService.localLOGV) Slog.v(
            //     WindowManagerService.TAG, "Last window removed from " + this
            //     + ", destroying " + mSurfaceSession);
            if (CWindowManagerService::SHOW_TRANSACTIONS) {
                Slogger::I(CWindowManagerService::TAG, "  KILL SURFACE SESSION %p",
                        mSurfaceSession.Get());
            }
            // try {
            ECode ec = mSurfaceSession->Kill();
            if (FAILED(ec)) {
                Slogger::W(CWindowManagerService::TAG,
                        "Exception thrown when killing surface session %p in session %p: 0x%08x"
                        , mSurfaceSession.Get(), this, ec);
            }
            // } catch (Exception e) {
            //     Slog.w(WindowManagerService.TAG, "Exception thrown when killing surface session "
            //         + mSurfaceSession + " in session " + this
            //         + ": " + e.toString());
            // }
            mSurfaceSession = NULL;
        }
    }
}

ECode CSession::ProxyDied()
{
    Slogger::W(CWindowManagerService::TAG, "CSession::ProxyDied()");
    // Note: it is safe to call in to the input method manager
    // here because we are not holding our lock.
    if (mService->mInputMethodManager != NULL) {
        mService->mInputMethodManager->RemoveClient(mClient);
    }

    AutoLock lock(mService->mWindowMapLock);

    AutoPtr<IProxy> proxy = (IProxy*)mClient->Probe(EIID_IProxy);
    assert(proxy != NULL);
    Boolean res;
    proxy->UnlinkToDeath(this, 0, &res);
    mClientDead = TRUE;
    KillSessionLocked();

    return NOERROR;
}

} // Wm
} // Server
} // Droid
} // Elastos
