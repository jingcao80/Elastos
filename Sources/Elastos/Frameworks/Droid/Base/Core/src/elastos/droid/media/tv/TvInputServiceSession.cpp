
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Media.h"
#include "elastos/droid/media/tv/TvInputServiceSession.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/media/tv/CTvContentRating.h"
#include "elastos/droid/media/tv/TvInputService.h"
#include "elastos/droid/view/CDispatcherState.h"
#include "elastos/droid/view/CWindowManagerLayoutParams.h"
#include "elastos/droid/os/CHandler.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::View::CDispatcherState;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::EIID_IKeyEventCallback;
using Elastos::Droid::View::IDispatcherState;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IViewRootImpl;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IHashSet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

ECode TvInputServiceSession::SetOverlayViewEnabledRunnable::Run()
{
    if (mEnable == mHost->mOverlayViewEnabled) {
        return NOERROR;
    }
    mHost->mOverlayViewEnabled = mEnable;
    if (mEnable) {
        if (mHost->mWindowToken != NULL) {
            mHost->CreateOverlayView(mHost->mWindowToken, mHost->mOverlayFrame);
        }
    } else {
        mHost->RemoveOverlayView(FALSE);
    }
    return NOERROR;
}

ECode TvInputServiceSession::NotifySessionEventRunnable::Run()
{
    // try {
    if (TvInputService::DEBUG) Logger::D(TvInputService::TAG, "notifySessionEvent(%s)", mEventType.string());
    return mHost->mSessionCallback->OnSessionEvent(mEventType, mEventArgs);
    // } catch (RemoteException e) {
    //     Log.w(TvInputService::TAG, "error in sending event (event=" + eventType + ")");
    // }
}

ECode TvInputServiceSession::NotifyChannelRetunedRunnable::Run()
{
    // try {
    if (TvInputService::DEBUG) Logger::D(TvInputService::TAG, "notifyChannelRetuned");
    return mHost->mSessionCallback->OnChannelRetuned(mChannelUri);
    // } catch (RemoteException e) {
    //     Log.w(TvInputService::TAG, "error in notifyChannelRetuned");
    // }
}

ECode TvInputServiceSession::NotifyTracksChangedRunnable::Run()
{
    // try {
    if (TvInputService::DEBUG) Logger::D(TvInputService::TAG, "notifyTracksChanged");
    return mHost->mSessionCallback->OnTracksChanged(mTracks);
    // } catch (RemoteException e) {
    //     Log.w(TvInputService::TAG, "error in notifyTracksChanged");
    // }
}

ECode TvInputServiceSession::NotifyTrackSelectedRunnable::Run()
{
    // try {
    if (TvInputService::DEBUG) Logger::D(TvInputService::TAG, "notifyTrackSelected");
    return mHost->mSessionCallback->OnTrackSelected(mType, mTrackId);
    // } catch (RemoteException e) {
    //     Log.w(TvInputService::TAG, "error in notifyTrackSelected");
    // }
}

ECode TvInputServiceSession::NotifyVideoAvailableRunnable::Run()
{
    // try {
    if (TvInputService::DEBUG) Logger::D(TvInputService::TAG, "notifyVideoAvailable");
    return mHost->mSessionCallback->OnVideoAvailable();
    // } catch (RemoteException e) {
    //     Log.w(TvInputService::TAG, "error in notifyVideoAvailable");
    // }
}

ECode TvInputServiceSession::NotifyVideoUnavailableRunnable::Run()
{
    // try {
    if (TvInputService::DEBUG) Logger::D(TvInputService::TAG, "notifyVideoUnavailable");
    return mHost->mSessionCallback->OnVideoUnavailable(mReason);
    // } catch (RemoteException e) {
    //     Log.w(TvInputService::TAG, "error in notifyVideoUnavailable");
    // }
}

ECode TvInputServiceSession::NotifyContentAllowedRunnable::Run()
{
    // try {
    if (TvInputService::DEBUG) Logger::D(TvInputService::TAG, "notifyContentAllowed");
    return mHost->mSessionCallback->OnContentAllowed();
    // } catch (RemoteException e) {
    //     Log.w(TvInputService::TAG, "error in notifyContentAllowed");
    // }
}

ECode TvInputServiceSession::NotifyContentBlockedRunnable::Run()
{
    // try {
    if (TvInputService::DEBUG) Logger::D(TvInputService::TAG, "notifyContentBlocked");
    String str;
    mRating->FlattenToString(&str);
    return mHost->mSessionCallback->OnContentBlocked(str);
    // } catch (RemoteException e) {
    //     Log.w(TvInputService::TAG, "error in notifyContentBlocked");
    // }
}

ECode TvInputServiceSession::LayoutSurfaceRunnable::Run()
{
    // try {
    if (TvInputService::DEBUG) {
        Logger::D(TvInputService::TAG,
            "layoutSurface (l=%d, t=%d, r=%d, b=%d,)", mLeft, mTop, mRight, mBottm);
    }
    return mHost->mSessionCallback->OnLayoutSurface(mLeft, mTop, mRight, mBottm);
    // } catch (RemoteException e) {
    //     Log.w(TvInputService::TAG, "error in layoutSurface");
    // }
}

//==============================================================================
// TvInputServiceSession
//==============================================================================

CAR_INTERFACE_IMPL(TvInputServiceSession, Object, IKeyEventCallback)

TvInputServiceSession::TvInputServiceSession()
    : mOverlayViewEnabled(FALSE)
{
    CDispatcherState::New((IDispatcherState**)&mDispatcherState);
}

TvInputServiceSession::~TvInputServiceSession()
{
}

ECode TvInputServiceSession::constructor(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::WINDOW_SERVICE, ((IInterface**)&service));
    mWindowManager = IWindowManager::Probe(service);
    AutoPtr<ILooper> looper;
    context->GetMainLooper((ILooper**)&looper);
    CHandler::New((IHandler**)&mHandler);
    return NOERROR;
}

ECode TvInputServiceSession::SetOverlayViewEnabled(
    /* [in] */ Boolean value)
{
    AutoPtr<SetOverlayViewEnabledRunnable> run
         = new SetOverlayViewEnabledRunnable(this, value);
    Boolean b;
    return mHandler->Post(run, &b);
}

ECode TvInputServiceSession::NotifySessionEvent(
    /* [in] */ const String& eventType,
    /* [in] */ IBundle* eventArgs)
{
    if (eventType == NULL) {
        // throw new IllegalArgumentException("eventType should not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<NotifySessionEventRunnable> run
             = new NotifySessionEventRunnable(this, eventType, eventArgs);
    RunOnMainThread(run);
    return NOERROR;
}

ECode TvInputServiceSession::NotifyChannelRetuned(
    /* [in] */ IUri* channelUri)
{
    AutoPtr<NotifyChannelRetunedRunnable> run
             = new NotifyChannelRetunedRunnable(this, channelUri);
    RunOnMainThread(run);
    return NOERROR;
}

ECode TvInputServiceSession::NotifyTracksChanged(
    /* [in] */ IList* tracks)
{
    AutoPtr<IHashSet> trackIdSet;
    CHashSet::New((IHashSet**)&trackIdSet);

    AutoPtr<ArrayOf<IInterface*> > array;
    tracks->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<ITvTrackInfo> track = ITvTrackInfo::Probe((*array)[i]);
        String trackId;
        track->GetId(&trackId);
        AutoPtr<ICharSequence> csq;
        CString::New(trackId, (ICharSequence**)&csq);
        Boolean b;
        trackIdSet->Contains(csq, &b);
        if (b) {
            // throw new IllegalArgumentException("redundant track ID: " + trackId);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        trackIdSet->Add(csq);
    }
    trackIdSet->Clear();

    // TODO: Validate the track list.
    AutoPtr<NotifyTracksChangedRunnable> run
            = new NotifyTracksChangedRunnable(this, tracks);
    RunOnMainThread(run);
    return NOERROR;
}

ECode TvInputServiceSession::NotifyTrackSelected(
    /* [in] */ Int32 type,
    /* [in] */ const String& trackId)
{
    AutoPtr<NotifyTrackSelectedRunnable> run
            = new NotifyTrackSelectedRunnable(this, type, trackId);
    RunOnMainThread(run);
    return NOERROR;
}

ECode TvInputServiceSession::NotifyVideoAvailable()
{
    AutoPtr<NotifyVideoAvailableRunnable> run
            = new NotifyVideoAvailableRunnable(this);
    RunOnMainThread(run);
    return NOERROR;
}

ECode TvInputServiceSession::NotifyVideoUnavailable(
    /* [in] */ Int32 reason)
{
    if (reason < ITvInputManager::VIDEO_UNAVAILABLE_REASON_START
            || reason > ITvInputManager::VIDEO_UNAVAILABLE_REASON_END) {
        // throw new IllegalArgumentException("Unknown reason: " + reason);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<NotifyVideoUnavailableRunnable> run
            = new NotifyVideoUnavailableRunnable(this, reason);
    RunOnMainThread(run);
    return NOERROR;
}

ECode TvInputServiceSession::NotifyContentAllowed()
{
    AutoPtr<NotifyContentAllowedRunnable> run
            = new NotifyContentAllowedRunnable(this);
    RunOnMainThread(run);
    return NOERROR;
}

ECode TvInputServiceSession::NotifyContentBlocked(
    /* [in] */ ITvContentRating* rating)
{
    AutoPtr<NotifyContentBlockedRunnable> run
            = new NotifyContentBlockedRunnable(this, rating);
    RunOnMainThread(run);
    return NOERROR;
}

ECode TvInputServiceSession::LayoutSurface(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottm)
{
    if (left > right || top > bottm) {
        // throw new IllegalArgumentException("Invalid parameter");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<LayoutSurfaceRunnable> run
            = new LayoutSurfaceRunnable(this, left, top, right, bottm);
    RunOnMainThread(run);
    return NOERROR;
}

ECode TvInputServiceSession::OnRelease()
{
    return NOERROR;
}

ECode TvInputServiceSession::OnSetMain(
    /* [in] */ Boolean isMain)
{
    return NOERROR;
}

ECode TvInputServiceSession::OnSetSurface(
    /* [in] */ ISurface* surface,
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode TvInputServiceSession::OnSurfaceChanged(
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return NOERROR;
}

ECode TvInputServiceSession::OnOverlayViewSizeChanged(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return NOERROR;
}

ECode TvInputServiceSession::OnSetStreamVolume(
    /* [in] */ Float volume)
{
    return NOERROR;
}

ECode TvInputServiceSession::OnTune(
    /* [in] */ IUri* channelUri)
{
    return NOERROR;
}

ECode TvInputServiceSession::OnTune(
    /* [in] */ IUri* channelUri,
    /* [in] */ IBundle* params)
{
    return OnTune(channelUri);
}

ECode TvInputServiceSession::OnSetCaptionEnabled(
    /* [in] */ Boolean enabled)
{
    return NOERROR;
}

ECode TvInputServiceSession::OnUnblockContent(
    /* [in] */ ITvContentRating* unblockedRating)
{
    return NOERROR;
}

ECode TvInputServiceSession::OnSelectTrack(
    /* [in] */ Int32 type,
    /* [in] */ const String& trackId)
{
    return NOERROR;
}

ECode TvInputServiceSession::OnAppPrivateCommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data)
{
    return NOERROR;
}

ECode TvInputServiceSession::OnCreateOverlayView(
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    return NOERROR;
}

ECode TvInputServiceSession::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode TvInputServiceSession::OnTrackballEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode TvInputServiceSession::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode TvInputServiceSession::ReleaseResources()
{
    RemoveOverlayView(TRUE);
    OnRelease();
    if (mSurface != NULL) {
        mSurface->ReleaseSurface();
        mSurface = NULL;
    }
    return NOERROR;
}

ECode TvInputServiceSession::SetMain(
    /* [in] */ Boolean isMain)
{
    return OnSetMain(isMain);;
}

ECode TvInputServiceSession::SetSurface(
    /* [in] */ ISurface* surface)
{
    Boolean b;
    OnSetSurface(surface, &b);
    if (mSurface != NULL) {
        mSurface->ReleaseSurface();
    }
    mSurface = surface;
    // // TODO: Handle failure.
    return NOERROR;
}

ECode TvInputServiceSession::DispatchSurfaceChanged(
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (TvInputService::DEBUG) {
        Logger::D(TvInputService::TAG,
                "dispatchSurfaceChanged(format=%d, width=%d, height=%d)",
                format, width, height);
    }
    return OnSurfaceChanged(format, width, height);
}

ECode TvInputServiceSession::SetStreamVolume(
    /* [in] */ Float volume)
{
    return OnSetStreamVolume(volume);
}

ECode TvInputServiceSession::Tune(
    /* [in] */ IUri* channelUri,
    /* [in] */ IBundle* params)
{
    return OnTune(channelUri, params);
    // TODO: Handle failure.
}

ECode TvInputServiceSession::SetCaptionEnabled(
    /* [in] */ Boolean enabled)
{

    return OnSetCaptionEnabled(enabled);
}

ECode TvInputServiceSession::SelectTrack(
    /* [in] */ Int32 type,
    /* [in] */ const String& trackId)
{
    return OnSelectTrack(type, trackId);
}

ECode TvInputServiceSession::UnblockContent(
    /* [in] */ const String& unblockedRating)
{
    AutoPtr<ITvContentRating> rating;
    CTvContentRating::UnflattenFromString(unblockedRating, (ITvContentRating**)&rating);
    return OnUnblockContent(rating);
    // // TODO: Handle failure.
}

ECode TvInputServiceSession::AppPrivateCommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data)
{
    return OnAppPrivateCommand(action, data);
}

ECode TvInputServiceSession::CreateOverlayView(
    /* [in] */ IBinder* windowToken,
    /* [in] */ IRect* frame)
{
    if (mOverlayView != NULL) {
        IViewManager::Probe(mWindowManager)->RemoveView(mOverlayView);
        mOverlayView = NULL;
    }
    if (TvInputService::DEBUG) {
        // Logger::D(TvInputService::TAG, "create overlay view(" + frame + ")");
    }
    mWindowToken = windowToken;
    mOverlayFrame = frame;

    AutoPtr<CRect> f = (CRect*)frame;
    OnOverlayViewSizeChanged(f->mRight - f->mLeft, f->mBottom - f->mTop);
    if (!mOverlayViewEnabled) {
        return NOERROR;
    }
    mOverlayView = NULL;
    OnCreateOverlayView((IView**)&mOverlayView);
    if (mOverlayView == NULL) {
        return NOERROR;
    }
    // TvView's window type is TYPE_APPLICATION_MEDIA and we want to create
    // an overlay window above the media window but below the application window.
    Int32 type = IWindowManagerLayoutParams::TYPE_APPLICATION_MEDIA_OVERLAY;
    // We make the overlay view non-focusable and non-touchable so that
    // the application that owns the window token can decide whether to consume or
    // dispatch the input events.
    Int32 flag = IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
            | IWindowManagerLayoutParams::FLAG_LAYOUT_NO_LIMITS
            | IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE;

    mWindowParams = NULL;
    CWindowManagerLayoutParams::New(
            f->mRight - f->mLeft, f->mBottom - f->mTop,
            f->mLeft, f->mTop, type, flag, IPixelFormat::TRANSPARENT, (IWindowManagerLayoutParams**)&mWindowParams);

    AutoPtr<CWindowManagerLayoutParams> p = (CWindowManagerLayoutParams*)mWindowParams.Get();
    p->mPrivateFlags |= IWindowManagerLayoutParams::PRIVATE_FLAG_NO_MOVE_ANIMATION;
    p->mGravity = IGravity::START | IGravity::TOP;
    p->mToken = windowToken;
    IViewManager::Probe(mWindowManager)->AddView(mOverlayView, IViewGroupLayoutParams::Probe(mWindowParams));
    return NOERROR;
}

ECode TvInputServiceSession::RelayoutOverlayView(
    /* [in] */ IRect* frame)
{
    if (TvInputService::DEBUG) {
        // Logger::D(TvInputService::TAG, "relayoutOverlayView(" + frame + ")");
    }

    Int32 ofWidth, ofHeight;
    Int32 fWidth, fHeight;
    mOverlayFrame->GetWidth(&ofWidth);
    mOverlayFrame->GetHeight(&ofHeight);
    frame->GetWidth(&fWidth);
    frame->GetHeight(&fHeight);

    AutoPtr<CRect> f = (CRect*)frame;
    AutoPtr<CRect> of = (CRect*)mOverlayFrame.Get();

    if (mOverlayFrame == NULL || ofWidth != fWidth
            || ofHeight != fHeight) {
        // Note: relayoutOverlayView is called whenever TvView's layout is changed
        // regardless of setOverlayViewEnabled.
        OnOverlayViewSizeChanged(f->mRight - f->mLeft, f->mBottom - f->mTop);
    }
    mOverlayFrame = frame;
    if (!mOverlayViewEnabled || mOverlayView == NULL) {
        return NOERROR;
    }

    AutoPtr<CWindowManagerLayoutParams> p = (CWindowManagerLayoutParams*)mWindowParams.Get();
    p->mX = f->mLeft;
    p->mY = f->mTop;
    p->mWidth = f->mRight - f->mLeft;
    p->mHeight = f->mBottom - f->mTop;
    IViewManager::Probe(mWindowManager)->UpdateViewLayout(mOverlayView, IViewGroupLayoutParams::Probe(mWindowParams));
    return NOERROR;
}

ECode TvInputServiceSession::RemoveOverlayView(
    /* [in] */ Boolean clearWindowToken)
{
    if (TvInputService::DEBUG) {
        // Logger::D(TvInputService::TAG, "removeOverlayView(" + mOverlayView + ")");
    }
    if (clearWindowToken) {
        mWindowToken = NULL;
        mOverlayFrame = NULL;
    }
    if (mOverlayView != NULL) {
        IViewManager::Probe(mWindowManager)->RemoveView(mOverlayView);
        mOverlayView = NULL;
        mWindowParams = NULL;
    }
    return NOERROR;
}

ECode TvInputServiceSession::DispatchInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ IInputEventReceiver* receiver,
    /* [out] */ Int32* result)
{
    if (TvInputService::DEBUG) {
        // Logger::D(TvInputService::TAG, "dispatchInputEvent(" + event + ")");
    }
    Boolean isNavigationKey = FALSE;
    if (IKeyEvent::Probe(event) != NULL) {
        AutoPtr<IKeyEvent> keyEvent = IKeyEvent::Probe(event);
        Int32 code;
        keyEvent->GetKeyCode(&code);
        TvInputService::IsNavigationKey(code, &isNavigationKey);
        Boolean b;
        keyEvent->Dispatch(this, mDispatcherState, TO_IINTERFACE(this), &b);
        if (b) {
            *result = ITvInputManagerSession::DISPATCH_HANDLED;
            return NOERROR;
        }
    } else if (IMotionEvent::Probe(event) != NULL) {
        AutoPtr<IMotionEvent> motionEvent = IMotionEvent::Probe(event);
        Int32 source;
        IInputEvent::Probe(motionEvent)->GetSource(&source);
        Boolean b;
        motionEvent->IsTouchEvent(&b);
        if (b) {
            OnTouchEvent(motionEvent, &b);
            if (b) {
                *result = ITvInputManagerSession::DISPATCH_HANDLED;
                return NOERROR;
            }
        } else if ((source & IInputDevice::SOURCE_CLASS_TRACKBALL) != 0) {
            OnTrackballEvent(motionEvent, &b);
            if (b) {
                *result = ITvInputManagerSession::DISPATCH_HANDLED;
                return NOERROR;
            }
        } else {
            OnGenericMotionEvent(motionEvent, &b);
            if (b) {
                *result = ITvInputManagerSession::DISPATCH_HANDLED;
                return NOERROR;
            }
        }
    }
    Boolean b;
    if (mOverlayView == NULL || (mOverlayView->IsAttachedToWindow(&b), !b)) {
        *result = ITvInputManagerSession::DISPATCH_NOT_HANDLED;
        return NOERROR;
    }
    AutoPtr<IViewRootImpl> impl;
    mOverlayView->GetViewRootImpl((IViewRootImpl**)&impl);
    mOverlayView->HasWindowFocus(&b);
    if (!b) {
        impl->WindowFocusChanged(TRUE, TRUE);
    }
    mOverlayView->HasFocusable(&b);
    if (isNavigationKey && b) {
        // If mOverlayView has focusable views, navigation key events should be always
        // handled. If not, it can make the application UI navigation messed up.
        // For example, in the case that the left-most view is focused, a left key event
        // will not be handled in ViewRootImpl. Then, the left key event will be handled in
        // the application during the UI navigation of the TV input.
        impl->DispatchInputEvent(event);
        *result = ITvInputManagerSession::DISPATCH_HANDLED;
        return NOERROR;
    } else {
        impl->DispatchInputEvent(event, receiver);
        *result = ITvInputManagerSession::DISPATCH_IN_PROGRESS;
        return NOERROR;
    }
}

void TvInputServiceSession::SetSessionCallback(
    /* [in] */ IITvInputSessionCallback* callback)
{
    mSessionCallback = callback;
}

void TvInputServiceSession::RunOnMainThread(
    /* [in] */ IRunnable * action)
{
    AutoPtr<ILooper> looper;
    mHandler->GetLooper((ILooper**)&looper);
    Boolean b;
    looper->IsCurrentThread(&b);
    if (b && mSessionCallback != NULL) {
        action->Run();
    } else {
        // Posts the runnable if this is not called from the main thread or the session
        // is not initialized yet.
        mHandler->Post(action, &b);
    }
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
