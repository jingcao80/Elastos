#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/media/tv/CTvView.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/os/CHandler.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::View::EIID_ISurfaceHolderCallback;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IViewRootImpl;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

String CTvView::TAG("TvView");
Boolean CTvView::DEBUG = FALSE;
Int32 CTvView::VIDEO_SIZE_VALUE_UNKNOWN = 0;
Int32 CTvView::ZORDER_MEDIA = 0;
Int32 CTvView::ZORDER_MEDIA_OVERLAY = 1;
Int32 CTvView::ZORDER_ON_TOP = 2;
Int32 CTvView::CAPTION_DEFAULT = 0;
Int32 CTvView::CAPTION_ENABLED = 1;
Int32 CTvView::CAPTION_DISABLED = 2;
Object CTvView::sMainTvViewLock;
AutoPtr<IWeakReference> CTvView::NULL_TV_VIEW; // WeakReference<TvView> NULL_TV_VIEW = new WeakReference(null);
AutoPtr<IWeakReference> CTvView::sMainTvView; // WeakReference<TvView> sMainTvView = NULL_TV_VIEW;

//==============================================================================
// CTvView::SurfaceHolderCallback
//==============================================================================

CAR_INTERFACE_IMPL(CTvView::SurfaceHolderCallback, Object, ISurfaceHolderCallback)

ECode CTvView::SurfaceHolderCallback::SurfaceCreated(
    /* [in] */ ISurfaceHolder* holder)
{
    mHost->mSurface = NULL;
    holder->GetSurface((ISurface**)&mHost->mSurface);
    mHost->SetSessionSurface(mHost->mSurface);
    return NOERROR;
}

ECode CTvView::SurfaceHolderCallback::SurfaceChanged(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (DEBUG) {
        // Logger::D(TAG, "surfaceChanged(holder=" + holder + ", format=" + format + ", width="
        //     + width + ", height=" + height + ")");
    }
    mHost->mSurfaceFormat = format;
    mHost->mSurfaceWidth = width;
    mHost->mSurfaceHeight = height;
    mHost->mSurfaceChanged = TRUE;
    mHost->DispatchSurfaceChanged(mHost->mSurfaceFormat, mHost->mSurfaceWidth, mHost->mSurfaceHeight);
    return NOERROR;
}

ECode CTvView::SurfaceHolderCallback::SurfaceDestroyed(
    /* [in] */ ISurfaceHolder* holder)
{
    mHost->mSurface = NULL;
    mHost->mSurfaceChanged = FALSE;
    mHost->SetSessionSurface(NULL);
    return NOERROR;
}

//==============================================================================
// CTvView::FinishedInputEventCallback
//==============================================================================

CAR_INTERFACE_IMPL(CTvView::FinishedInputEventCallback, Object, ITvInputManagerSessionFinishedInputEventCallback)

ECode CTvView::FinishedInputEventCallback::OnFinishedInputEvent(
    /* [in] */ IInterface * token,
    /* [in] */ Boolean handled)
{
    if (DEBUG) {
        // Logger::D(TAG, "onFinishedInputEvent(token=" + token + ", handled=" + handled + ")");
    }
    if (handled) {
        return NOERROR;
    }
    // TODO: Re-order unhandled events.
    AutoPtr<IInputEvent> event = IInputEvent::Probe(token);
    Boolean b;
    mHost->DispatchUnhandledInputEvent(event, &b);
    if (b) {
        return NOERROR;
    }
    AutoPtr<IViewRootImpl> viewRootImpl;
    mHost->GetViewRootImpl((IViewRootImpl**)&viewRootImpl);
    if (viewRootImpl != NULL) {
        viewRootImpl->DispatchUnhandledInputEvent(event);
    }
    return NOERROR;
}

//==============================================================================
// CTvView::MySurfaceView
//==============================================================================

CTvView::MySurfaceView::MySurfaceView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle,
    /* [in] */ CTvView* host)
    : mHost(host)
{
    constructor(context, attrs, defStyle);
}

ECode CTvView::MySurfaceView::UpdateWindow(
    /* [in] */ Boolean force,
    /* [in] */ Boolean redrawNeeded)
{
    SurfaceView::UpdateWindow(force, redrawNeeded);
    mHost->RelayoutSessionOverlayView();
    return NOERROR;
}

//==============================================================================
// CTvView::MySessionCallback
//==============================================================================

CAR_INTERFACE_IMPL(CTvView::MySessionCallback, Object, ITvInputManagerSessionCallback)

CTvView::MySessionCallback::MySessionCallback(
    /* [in] */ const String& inputId,
    /* [in] */ IUri* channelUri,
    /* [in] */ IBundle* tuneParams,
    /* [in] */ CTvView* host)
    : mInputId(inputId)
    , mChannelUri(channelUri)
    , mTuneParams(tuneParams)
    , mHost(host)
{
}

ECode CTvView::MySessionCallback::OnSessionCreated(
    /* [in] */ ITvInputManagerSession* session)
{
    if (this != mHost->mSessionCallback) {
        // This callback is obsolete.
        if (session != NULL) {
            session->ReleaseResources();
        }
        return NOERROR;
    }
    if (DEBUG) {
        Logger::D(TAG, "onSessionCreated()");
    }
    mHost->mSession = session;
    if (session != NULL) {
        Object& lock = mHost->sMainTvViewLock;
        synchronized(lock) {
            Boolean b;
            mHost->HasWindowFocus(&b);

            if (mHost->sMainTvView.Get() != NULL) {
                AutoPtr<ITvView> cs;
                mHost->sMainTvView->Resolve(EIID_ITvView, (IInterface**)&cs);
                if (cs != NULL) {
                    if (b && ITvView::Probe(mHost) == cs) {
                        mHost->mSession->SetMain();
                    }
                }
            }
        }
        // mSurface may not be ready yet as soon as starting an application.
        // In the case, we don't send Session.setSurface(NULL) unnecessarily.
        // setSessionSurface will be called in surfaceCreated.
        if (mHost->mSurface != NULL) {
            mHost->SetSessionSurface(mHost->mSurface);
            if (mHost->mSurfaceChanged) {
                mHost->DispatchSurfaceChanged(mHost->mSurfaceFormat, mHost->mSurfaceWidth, mHost->mSurfaceHeight);
            }
        }
        mHost->CreateSessionOverlayView();
        if (mHost->mCaptionEnabled != CAPTION_DEFAULT) {
            mHost->mSession->SetCaptionEnabled(mHost->mCaptionEnabled == CAPTION_ENABLED);
        }
        mHost->mSession->Tune(mChannelUri, mTuneParams);
        if (mHost->mHasStreamVolume) {
            mHost->mSession->SetStreamVolume(mHost->mStreamVolume);
        }
    } else {
        mHost->mSessionCallback = NULL;
        if (mHost->mCallback != NULL) {
            mHost->mCallback->OnConnectionFailed(mInputId);
        }
    }
    return NOERROR;
}

ECode CTvView::MySessionCallback::OnSessionReleased(
    /* [in] */ ITvInputManagerSession* session)
{
    if (this != mHost->mSessionCallback) {
        return NOERROR;
    }
    mHost->mOverlayViewCreated = FALSE;
    mHost->mOverlayViewFrame = NULL;
    mHost->mSessionCallback = NULL;
    mHost->mSession = NULL;
    if (mHost->mCallback != NULL) {
        mHost->mCallback->OnDisconnected(mInputId);
    }
    return NOERROR;
}

ECode CTvView::MySessionCallback::OnChannelRetuned(
    /* [in] */ ITvInputManagerSession* session,
    /* [in] */ IUri* channelUri)
{
    if (this != mHost->mSessionCallback) {
        return NOERROR;
    }
    if (DEBUG) {
        // Logger::D(TAG, "onChannelChangedByTvInput(" + channelUri + ")");
    }
    if (mHost->mCallback != NULL) {
        mHost->mCallback->OnChannelRetuned(mInputId, channelUri);
    }
    return NOERROR;
}

ECode CTvView::MySessionCallback::OnTracksChanged(
    /* [in] */ ITvInputManagerSession* session,
    /* [in] */ IList* tracks)
{
    if (this != mHost->mSessionCallback) {
        return NOERROR;
    }
    if (DEBUG) {
        Logger::D(TAG, "onTracksChanged()");
    }
    if (mHost->mCallback != NULL) {
        mHost->mCallback->OnTracksChanged(mInputId, tracks);
    }
    return NOERROR;
}

ECode CTvView::MySessionCallback::OnTrackSelected(
    /* [in] */ ITvInputManagerSession* session,
    /* [in] */ Int32 type,
    /* [in] */ const String& trackId)
{
    if (this != mHost->mSessionCallback) {
        return NOERROR;
    }
    if (DEBUG) {
        Logger::D(TAG, "onTrackSelected()");
    }
    // TODO: Update the video size when the type is TYPE_VIDEO.
    if (mHost->mCallback != NULL) {
        mHost->mCallback->OnTrackSelected(mInputId, type, trackId);
    }
    return NOERROR;
}

ECode CTvView::MySessionCallback::OnVideoAvailable(
    /* [in] */ ITvInputManagerSession* session)
{
    if (this != mHost->mSessionCallback) {
        return NOERROR;
    }
    if (DEBUG) {
        Logger::D(TAG, "onVideoAvailable()");
    }
    if (mHost->mCallback != NULL) {
        mHost->mCallback->OnVideoAvailable(mInputId);
    }
    return NOERROR;
}

ECode CTvView::MySessionCallback::OnVideoUnavailable(
    /* [in] */ ITvInputManagerSession* session,
    /* [in] */ Int32 reason)
{
    if (this != mHost->mSessionCallback) {
        return NOERROR;
    }
    if (DEBUG) {
        Logger::D(TAG, "onVideoUnavailable(%d)", reason);
    }
    if (mHost->mCallback != NULL) {
        mHost->mCallback->OnVideoUnavailable(mInputId, reason);
    }
    return NOERROR;
}

ECode CTvView::MySessionCallback::OnContentAllowed(
    /* [in] */ ITvInputManagerSession* session)
{
    if (this != mHost->mSessionCallback) {
        return NOERROR;
    }
    if (DEBUG) {
        Logger::D(TAG, "onContentAllowed()");
    }
    if (mHost->mCallback != NULL) {
        mHost->mCallback->OnContentAllowed(mInputId);
    }
    return NOERROR;
}

ECode CTvView::MySessionCallback::OnContentBlocked(
    /* [in] */ ITvInputManagerSession* session,
    /* [in] */ ITvContentRating* rating)
{
    if (this != mHost->mSessionCallback) {
        return NOERROR;
    }
    if (DEBUG) {
        Logger::D(TAG, "onContentBlocked()");
    }
    if (mHost->mCallback != NULL) {
        mHost->mCallback->OnContentBlocked(mInputId, rating);
    }
    return NOERROR;
}

ECode CTvView::MySessionCallback::OnLayoutSurface(
    /* [in] */ ITvInputManagerSession* session,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    if (this != mHost->mSessionCallback) {
        return NOERROR;
    }
    if (DEBUG) {
        Logger::D(TAG, "onLayoutSurface (left= %d, top= %d, right= %d, bottom= %d)", left, top, right, bottom);
    }
    mHost->mSurfaceViewLeft = left;
    mHost->mSurfaceViewTop = top;
    mHost->mSurfaceViewRight = right;
    mHost->mSurfaceViewBottom = bottom;
    mHost->mUseRequestedSurfaceLayout = TRUE;
    mHost->RequestLayout();
    return NOERROR;
}

ECode CTvView::MySessionCallback::OnSessionEvent(
    /* [in] */ ITvInputManagerSession* session,
    /* [in] */ const String& eventType,
    /* [in] */ IBundle* eventArgs)
{
    if (this != mHost->mSessionCallback) {
        return NOERROR;
    }
    if (DEBUG) {
        Logger::D(TAG, String("onSessionEvent(") + eventType + ")");
    }
    if (mHost->mCallback != NULL) {
        mHost->mCallback->OnEvent(mInputId, eventType, eventArgs);
    }
    return NOERROR;
}

//==============================================================================
// CTvView
//==============================================================================

CAR_INTERFACE_IMPL(CTvView, Object, ITvView)

CAR_OBJECT_IMPL(CTvView)

CTvView::CTvView()
    : mOverlayViewCreated(FALSE)
    , mHasStreamVolume(FALSE)
    , mStreamVolume(0)
    , mVideoWidth(VIDEO_SIZE_VALUE_UNKNOWN)
    , mVideoHeight(VIDEO_SIZE_VALUE_UNKNOWN)
    , mSurfaceChanged(FALSE)
    , mSurfaceFormat(0)
    , mSurfaceWidth(0)
    , mSurfaceHeight(0)
    , mDefStyleAttr(0)
    , mWindowZOrder(0)
    , mUseRequestedSurfaceLayout(FALSE)
    , mSurfaceViewLeft(0)
    , mSurfaceViewRight(0)
    , mSurfaceViewTop(0)
    , mSurfaceViewBottom(0)
    , mCaptionEnabled(0)
{
    CHandler::New((IHandler**)&mHandler);
    mSurfaceHolderCallback = new SurfaceHolderCallback(this);
    mFinishedInputEventCallback = new FinishedInputEventCallback(this);
}

CTvView::~CTvView()
{
}

ECode CTvView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL, 0);
}

ECode CTvView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CTvView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    FAIL_RETURN(ViewGroup::constructor(context, attrs, defStyleAttr));
    mAttrs = attrs;
    mDefStyleAttr = defStyleAttr;
    ResetSurfaceView();
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IInterface> service;
    ctx->GetSystemService(IContext::TV_INPUT_SERVICE, (IInterface**)&service);
    mTvInputManager = ITvInputManager::Probe(service);
    return NOERROR;
}

ECode CTvView::SetCallback(
    /* [in] */ ITvViewTvInputCallback* callback)
{
    mCallback = callback;
    return NOERROR;
}

ECode CTvView::SetMain()
{
    synchronized(sMainTvViewLock) {
        GetWeakReference((IWeakReference**)&sMainTvView);
        Boolean b;
        HasWindowFocus(&b);
        if (b && mSession != NULL) {
            mSession->SetMain();
        }
    }
    return NOERROR;
}

ECode CTvView::SetZOrderMediaOverlay(
    /* [in] */ Boolean isMediaOverlay)
{
    if (isMediaOverlay) {
        mWindowZOrder = ZORDER_MEDIA_OVERLAY;
        RemoveSessionOverlayView();
    } else {
        mWindowZOrder = ZORDER_MEDIA;
        CreateSessionOverlayView();
    }
    if (mSurfaceView != NULL) {
        // ZOrderOnTop(false) removes WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM
        // from WindowLayoutParam as well as changes window type.
        mSurfaceView->SetZOrderOnTop(FALSE);
        mSurfaceView->SetZOrderMediaOverlay(isMediaOverlay);
    }
    return NOERROR;
}

ECode CTvView::SetZOrderOnTop(
    /* [in] */ Boolean onTop)
{
    if (onTop) {
        mWindowZOrder = ZORDER_ON_TOP;
        RemoveSessionOverlayView();
    } else {
        mWindowZOrder = ZORDER_MEDIA;
        CreateSessionOverlayView();
    }
    if (mSurfaceView != NULL) {
        mSurfaceView->SetZOrderMediaOverlay(FALSE);
        mSurfaceView->SetZOrderOnTop(onTop);
    }
    return NOERROR;
}

ECode CTvView::SetStreamVolume(
    /* [in] */ Float volume)
{
    if (DEBUG) Logger::D(TAG, "setStreamVolume(%f)", volume);
    mHasStreamVolume = TRUE;
    mStreamVolume = volume;
    if (mSession == NULL) {
        // Volume will be set once the connection has been made.
        return NOERROR;
    }
    mSession->SetStreamVolume(volume);
    return NOERROR;
}

ECode CTvView::Tune(
    /* [in] */ const String& inputId,
    /* [in] */ IUri* channelUri)
{
    return Tune(inputId, channelUri, NULL);
}

ECode CTvView::Tune(
    /* [in] */ const String& inputId,
    /* [in] */ IUri* channelUri,
    /* [in] */ IBundle* params)
{
    if (DEBUG) {
        // Logger::D(TAG, "tune(" + channelUri + ")");
    }
    if (inputId.IsEmpty()) {
        // throw new IllegalArgumentException("inputId cannot be NULL or an empty string");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    synchronized(sMainTvViewLock) {
        if (sMainTvView == NULL) {
            GetWeakReference((IWeakReference**)&sMainTvView);
        }
    }
    if (mSessionCallback != NULL && mSessionCallback->mInputId.Equals(inputId)) {
        if (mSession != NULL) {
            mSession->Tune(channelUri, params);
        } else {
            // Session is not created yet. Replace the channel which will be set once the
            // session is made.
            mSessionCallback->mChannelUri = channelUri;
            mSessionCallback->mTuneParams = params;
        }
    } else {
        ResetInternal();
        // When createSession() is called multiple times before the callback is called,
        // only the callback of the last createSession() call will be actually called back.
        // The previous callbacks will be ignored. For the logic, mSessionCallback
        // is newly assigned for every createSession request and compared with
        // MySessionCreateCallback.this.
        mSessionCallback = new MySessionCallback(inputId, channelUri, params, this);
        if (mTvInputManager != NULL) {
            mTvInputManager->CreateSession(inputId, mSessionCallback, mHandler);
        }
    }
    return NOERROR;
}

ECode CTvView::Reset()
{
    if (DEBUG) Logger::D(TAG, "reset()");
    synchronized(sMainTvViewLock) {
        if (sMainTvView != NULL) {
            AutoPtr<ITvView> cs;
            sMainTvView->Resolve(EIID_ITvView, (IInterface**)&cs);
            if (cs != NULL) {
                if ((ITvView*)this == cs) {
                    sMainTvView = NULL_TV_VIEW;
                }
            }
        }
    }
    ResetInternal();
    return NOERROR;
}

ECode CTvView::RequestUnblockContent(
    /* [in] */ ITvContentRating* unblockedRating)
{
    if (mSession != NULL) {
        mSession->RequestUnblockContent(unblockedRating);
    }
    return NOERROR;
}

ECode CTvView::SetCaptionEnabled(
    /* [in] */ Boolean enabled)
{
    mCaptionEnabled = enabled ? CAPTION_ENABLED : CAPTION_DISABLED;
    if (mSession != NULL) {
        mSession->SetCaptionEnabled(enabled);
    }
    return NOERROR;
}

ECode CTvView::SelectTrack(
    /* [in] */ Int32 type,
    /* [in] */ const String& trackId)
{
    if (mSession != NULL) {
        mSession->SelectTrack(type, trackId);
    }
    return NOERROR;
}

ECode CTvView::GetTracks(
    /* [in] */ Int32 type,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (mSession == NULL) {
        return NOERROR;
    }
    return mSession->GetTracks(type, result);
}

ECode CTvView::GetSelectedTrack(
    /* [in] */ Int32 type,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);
    if (mSession == NULL) {
        return NOERROR;
    }
    return mSession->GetSelectedTrack(type, result);
}

ECode CTvView::SendAppPrivateCommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data)
{
    if (action.IsEmpty()) {
        // throw new IllegalArgumentException("action cannot be NULL or an empty string");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mSession != NULL) {
        mSession->SendAppPrivateCommand(action, data);
    }
    return NOERROR;
}

ECode CTvView::DispatchUnhandledInputEvent(
    /* [in] */ IInputEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mOnUnhandledInputEventListener != NULL) {
        Boolean b;
        mOnUnhandledInputEventListener->OnUnhandledInputEvent(event, &b);
        if (b) {
            *result = TRUE;
            return NOERROR;
        }
    }
    return OnUnhandledInputEvent(event, result);
}

ECode CTvView::OnUnhandledInputEvent(
    /* [in] */ IInputEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode CTvView::SetOnUnhandledInputEventListener(
    /* [in] */ ITvViewTvInputOnUnhandledInputEventListener* listener)
{
    mOnUnhandledInputEventListener = listener;
    return NOERROR;
}

ECode CTvView::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b;
    ViewGroup::DispatchKeyEvent(event, &b);
    if (b) {
        *result = TRUE;
        return NOERROR;
    }
    if (DEBUG) {
        // Logger::D(TAG, "dispatchKeyEvent(" + event + ")");
    }
    if (mSession == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IInputEvent> copiedEvent;
    IInputEvent::Probe(event)->Copy((IInputEvent**)&copiedEvent);
    Int32 ret;
    mSession->DispatchInputEvent(copiedEvent, copiedEvent, mFinishedInputEventCallback, mHandler, &ret);
    *result = ret != ITvInputManagerSession::DISPATCH_NOT_HANDLED;
    return NOERROR;
}

ECode CTvView::DispatchTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b;
    ViewGroup::DispatchTouchEvent(event, &b);
    if (b) {
        *result = TRUE;
        return NOERROR;
    }
    if (DEBUG) {
        // Logger::D(TAG, "dispatchTouchEvent(" + event + ")");
    }
    if (mSession == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IInputEvent> copiedEvent;
    IInputEvent::Probe(event)->Copy((IInputEvent**)&copiedEvent);
    Int32 ret;
    mSession->DispatchInputEvent(copiedEvent, copiedEvent, mFinishedInputEventCallback,
            mHandler, &ret);
    *result = ret != ITvInputManagerSession::DISPATCH_NOT_HANDLED;
    return NOERROR;
}

ECode CTvView::DispatchTrackballEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b;
    ViewGroup::DispatchTrackballEvent(event, &b);
    if (b) {
        *result = TRUE;
        return NOERROR;
    }
    if (DEBUG) {
        // Logger::D(TAG, "dispatchTrackballEvent(" + event + ")");
    }
    if (mSession == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IInputEvent> copiedEvent;
    IInputEvent::Probe(event)->Copy((IInputEvent**)&copiedEvent);
    Int32 ret;
    mSession->DispatchInputEvent(copiedEvent, copiedEvent, mFinishedInputEventCallback,
            mHandler, &ret);
    *result = (ret != ITvInputManagerSession::DISPATCH_NOT_HANDLED);
    return NOERROR;
}

ECode CTvView::DispatchGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b;
    ViewGroup::DispatchGenericMotionEvent(event, &b);
    if (b) {
        *result = TRUE;
        return NOERROR;
    }
    if (DEBUG) {
        // Logger::D(TAG, "dispatchGenericMotionEvent(" + event + ")");
    }
    if (mSession == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IInputEvent> copiedEvent;
    IInputEvent::Probe(event)->Copy((IInputEvent**)&copiedEvent);
    Int32 ret;
    mSession->DispatchInputEvent(copiedEvent, copiedEvent, mFinishedInputEventCallback,
            mHandler, &ret);
    *result = (ret != ITvInputManagerSession::DISPATCH_NOT_HANDLED);
    return NOERROR;
}

ECode CTvView::DispatchWindowFocusChanged(
    /* [in] */ Boolean hasFocus)
{
    ViewGroup::DispatchWindowFocusChanged(hasFocus);
    // Other app may have shown its own main TvView.
    // Set main again to regain main session.
    synchronized(sMainTvViewLock) {
        if (sMainTvView.Get() != NULL) {
            AutoPtr<ITvView> cs;
            sMainTvView->Resolve(EIID_ITvView, (IInterface**)&cs);
            if (cs != NULL) {
                if (hasFocus && (ITvView*)this == cs && mSession != NULL) {
                    mSession->SetMain();
                }
            }
        }
    }
    return NOERROR;
}

ECode CTvView::OnAttachedToWindow()
{
    ViewGroup::OnAttachedToWindow();
    CreateSessionOverlayView();
    return NOERROR;
}

ECode CTvView::OnDetachedFromWindow()
{
    RemoveSessionOverlayView();
    ViewGroup::OnDetachedFromWindow();
    return NOERROR;
}

ECode CTvView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    if (DEBUG) {
        Logger::D(TAG, "onLayout (left= %d, top= %d, right= %d, bottom= %d)"
            , left, top, right, bottom);
    }
    if (mUseRequestedSurfaceLayout) {
        return IView::Probe(mSurfaceView)->Layout(mSurfaceViewLeft, mSurfaceViewTop, mSurfaceViewRight,
                mSurfaceViewBottom);
    } else {
        return IView::Probe(mSurfaceView)->Layout(0, 0, right - left, bottom - top);
    }
}

void CTvView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    IView::Probe(mSurfaceView)->Measure(widthMeasureSpec, heightMeasureSpec);
    Int32 width;
    IView::Probe(mSurfaceView)->GetMeasuredWidth(&width);
    Int32 height;
    IView::Probe(mSurfaceView)->GetMeasuredHeight(&height);
    Int32 childState;
    IView::Probe(mSurfaceView)->GetMeasuredState(&childState);
    SetMeasuredDimension(ResolveSizeAndState(width, widthMeasureSpec, childState),
            ResolveSizeAndState(height, heightMeasureSpec,
                    childState << MEASURED_HEIGHT_STATE_SHIFT));
}

ECode CTvView::OnVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 visibility)
{
    ViewGroup::OnVisibilityChanged(changedView, visibility);
    IView::Probe(mSurfaceView)->SetVisibility(visibility);
    if (visibility == IView::VISIBLE) {
        CreateSessionOverlayView();
    } else {
        RemoveSessionOverlayView();
    }
    return NOERROR;
}

void CTvView::ResetInternal()
{
    if (mSession != NULL) {
        ReleaseResources();
        ResetSurfaceView();
    }
}

void CTvView::ResetSurfaceView()
{
    if (mSurfaceView != NULL) {
        AutoPtr<ISurfaceHolder> holder;
        mSurfaceView->GetHolder((ISurfaceHolder**)&holder);
        holder->RemoveCallback(mSurfaceHolderCallback);
        RemoveView(IView::Probe(mSurfaceView));
    }
    mSurface = NULL;
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    mSurfaceView = new MySurfaceView(ctx, mAttrs, mDefStyleAttr, this);

    AutoPtr<ISurfaceHolder> holder;
    mSurfaceView->GetHolder((ISurfaceHolder**)&holder);
    holder->AddCallback(mSurfaceHolderCallback);
    if (mWindowZOrder == ZORDER_MEDIA_OVERLAY) {
        mSurfaceView->SetZOrderMediaOverlay(TRUE);
    } else if (mWindowZOrder == ZORDER_ON_TOP) {
        mSurfaceView->SetZOrderOnTop(TRUE);
    }
    AddView(IView::Probe(mSurfaceView));
}

void CTvView::ReleaseResources()
{
    SetSessionSurface(NULL);
    RemoveSessionOverlayView();
    mUseRequestedSurfaceLayout = FALSE;
    mSession->ReleaseResources();
    mSession = NULL;
    mSessionCallback = NULL;
}

void CTvView::SetSessionSurface(
    /* [in] */ ISurface* surface)
{
    if (mSession == NULL) {
        return;
    }
    mSession->SetSurface(surface);
}

void CTvView::DispatchSurfaceChanged(
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (mSession == NULL) {
        return;
    }
    mSession->DispatchSurfaceChanged(format, width, height);
}

void CTvView::CreateSessionOverlayView()
{
    Boolean b;
    IsAttachedToWindow(&b);
    if (mSession == NULL || !b
            || mOverlayViewCreated || mWindowZOrder != ZORDER_MEDIA) {
        return;
    }
    mOverlayViewFrame = GetViewFrameOnScreen();
    mSession->CreateOverlayView(this, mOverlayViewFrame);
    mOverlayViewCreated = TRUE;
}

void CTvView::RemoveSessionOverlayView()
{
    if (mSession == NULL || !mOverlayViewCreated) {
        return;
    }
    mSession->RemoveOverlayView();
    mOverlayViewCreated = FALSE;
    mOverlayViewFrame = NULL;
}

void CTvView::RelayoutSessionOverlayView()
{
    Boolean b;
    if (mSession == NULL || (IsAttachedToWindow(&b),!b) || !mOverlayViewCreated
            || mWindowZOrder != ZORDER_MEDIA) {
        return;
    }
    AutoPtr<IRect> viewFrame = GetViewFrameOnScreen();
    viewFrame->Equals(mOverlayViewFrame, &b);
    if (b) {
        return;
    }
    mSession->RelayoutOverlayView(viewFrame);
    mOverlayViewFrame = viewFrame;
}

AutoPtr<IRect> CTvView::GetViewFrameOnScreen()
{
    AutoPtr<ArrayOf<Int32> > location = ArrayOf<Int32>::Alloc(2);
    GetLocationOnScreen((ArrayOf<Int32>*)location);
    Int32 width, height;
    GetWidth(&width);
    GetHeight(&height);
    AutoPtr<IRect> rect;
    CRect::New((*location)[0], (*location)[1],
            (*location)[0] + width, (*location)[1] + height, (IRect**)&rect);
    return rect;
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
