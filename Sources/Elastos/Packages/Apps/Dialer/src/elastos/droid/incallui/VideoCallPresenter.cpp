
#include "Elastos.Droid.Os.h"
#include "elastos/droid/incallui/VideoCallPresenter.h"
#include "elastos/droid/incallui/AudioModeProvider.h"
#include "elastos/droid/incallui/InCallPresenter.h"
#include "elastos/droid/incallui/InCallVideoCallListenerNotifier.h"
#include "elastos/droid/incallui/InCallCameraManager.h"
#include "elastos/droid/incallui/AudioModeProvider.h"
#include "elastos/droid/incallui/TelecomAdapter.h"
#include "elastos/droid/contacts/common/CallUtil.h"
#include "elastos/utility/Objects.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include "R.h"

using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Contacts::Common::CallUtil;
using Elastos::Droid::InCallUI::EIID_IIncomingCallListener;
using Elastos::Droid::InCallUI::EIID_IInCallOrientationListener;
using Elastos::Droid::InCallUI::EIID_IInCallStateListener;
using Elastos::Droid::InCallUI::EIID_IInCallDetailsListener;
using Elastos::Droid::InCallUI::EIID_ISurfaceChangeListener;
using Elastos::Droid::InCallUI::EIID_IVideoEventListener;
using Elastos::Droid::InCallUI::EIID_ISessionModificationListener;
using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Internal::Utility::CPreconditions;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Telecom::IAudioState;
using Elastos::Droid::View::ISurface;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace InCallUI {

//================================================================
// VideoCallPresenter::PreviewSurfaceState
//================================================================
const Int32 VideoCallPresenter::PreviewSurfaceState::NONE;
const Int32 VideoCallPresenter::PreviewSurfaceState::CAMERA_SET;
const Int32 VideoCallPresenter::PreviewSurfaceState::CAPABILITIES_RECEIVED;
const Int32 VideoCallPresenter::PreviewSurfaceState::SURFACE_SET;


//================================================================
// VideoCallPresenter::OnUpgradeToVideoFailRunnable
//================================================================
ECode VideoCallPresenter::OnUpgradeToVideoFailRunnable::Run()
{
    mHost->mPrimaryCall->SetSessionModificationState(Call::SessionModificationState::NO_REQUEST);
    return NOERROR;
}


//================================================================
// VideoCallPresenter
//================================================================
const Int64 VideoCallPresenter::SESSION_MODIFICATION_RESET_DELAY_MS;

CAR_INTERFACE_IMPL_7(VideoCallPresenter, Presenter
        , IIncomingCallListener
        , IInCallOrientationListener
        , IInCallStateListener
        , IInCallDetailsListener
        , ISurfaceChangeListener
        , IVideoEventListener
        , ISessionModificationListener)

VideoCallPresenter::VideoCallPresenter()
    : mMinimumVideoDimension(0.0)
    , mIsVideoCall(FALSE)
    , mDeviceOrientation(0)
    , mPreviewSurfaceState(PreviewSurfaceState::NONE)
    , mIsFullScreen(FALSE)
    , mPreVideoAudioMode(AudioModeProvider::AUDIO_MODE_INVALID)
{}

Int32 VideoCallPresenter::GetDeviceOrientation()
{
    return mDeviceOrientation;
}

void VideoCallPresenter::Init(
    /* [in] */ IContext* context)
{
    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditions);
    if (FAILED(preconditions->CheckNotNull(context))) return;
    mContext = context;
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetDimension(Elastos::Droid::Dialer::R::dimen::video_preview_small_dimension, &mMinimumVideoDimension);
    mSessionModificationResetHandler = NULL;
    CHandler::New((IHandler**)&mSessionModificationResetHandler);
}

ECode VideoCallPresenter::OnUiReady(
    /* [in] */ IUi* ui)
{
    FAIL_RETURN(Presenter::OnUiReady(ui))

    // Register for call state changes last
    InCallPresenter::GetInstance()->AddListener(IInCallStateListener::Probe(this));
    InCallPresenter::GetInstance()->AddIncomingCallListener(IIncomingCallListener::Probe(this));
    InCallPresenter::GetInstance()->AddOrientationListener(IInCallOrientationListener::Probe(this));

    // Register for surface and video events from {@link InCallVideoCallListener}s.
    InCallVideoCallListenerNotifier::GetInstance()->AddSurfaceChangeListener(this);
    InCallVideoCallListenerNotifier::GetInstance()->AddVideoEventListener(this);
    InCallVideoCallListenerNotifier::GetInstance()->AddSessionModificationListener(this);
    mIsVideoCall = FALSE;
    return NOERROR;
}

ECode VideoCallPresenter::OnUiUnready(
    /* [in] */ IUi* ui)
{
    FAIL_RETURN(Presenter::OnUiUnready(ui))

    InCallPresenter::GetInstance()->RemoveListener(this);
    InCallPresenter::GetInstance()->RemoveIncomingCallListener(this);
    InCallPresenter::GetInstance()->RemoveOrientationListener(this);
    InCallVideoCallListenerNotifier::GetInstance()->RemoveSurfaceChangeListener(this);
    InCallVideoCallListenerNotifier::GetInstance()->RemoveVideoEventListener(this);
    InCallVideoCallListenerNotifier::GetInstance()->RemoveSessionModificationListener(this);
    return NOERROR;
}

AutoPtr<IInCallServiceVideoCall> VideoCallPresenter::GetVideoCall()
{
    return mVideoCall;
}

void VideoCallPresenter::OnSurfaceCreated(
    /* [in] */ Int32 surface)
{
    AutoPtr<IUi> temp;
    GetUi((IUi**)&temp);
    AutoPtr<IVideoCallUi> ui = IVideoCallUi::Probe(temp);

    if (ui == NULL || mVideoCall == NULL) {
        return;
    }

    // If the preview surface has just been created and we have already received camera
    // capabilities, but not yet set the surface, we will set the surface now.
    if (surface == IVideoCallFragment::SURFACE_PREVIEW &&
            mPreviewSurfaceState == PreviewSurfaceState::CAPABILITIES_RECEIVED) {

        mPreviewSurfaceState = PreviewSurfaceState::SURFACE_SET;
        AutoPtr<ISurface> sur;
        ui->GetPreviewVideoSurface((ISurface**)&sur);
        mVideoCall->SetPreviewSurface(sur);
    }
    else if (surface == IVideoCallFragment::SURFACE_DISPLAY) {
        AutoPtr<ISurface> sur;
        ui->GetDisplayVideoSurface((ISurface**)&sur);
        mVideoCall->SetDisplaySurface(sur);
    }
}

void VideoCallPresenter::OnSurfaceChanged(
    /* [in] */ Int32 surface,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return;
}

void VideoCallPresenter::OnSurfaceDestroyed(
    /* [in] */ Int32 surface)
{
    AutoPtr<IUi> temp;
    GetUi((IUi**)&temp);
    AutoPtr<IVideoCallUi> ui = IVideoCallUi::Probe(temp);
    if (ui == NULL || mVideoCall == NULL) {
        return;
    }

    if (surface == IVideoCallFragment::SURFACE_DISPLAY) {
        mVideoCall->SetDisplaySurface(NULL);
    }
    else if (surface == IVideoCallFragment::SURFACE_PREVIEW) {
        mVideoCall->SetPreviewSurface(NULL);
    }
}

void VideoCallPresenter::OnSurfaceClick(
    /* [in] */ Int32 surfaceId)
{
    mIsFullScreen = !mIsFullScreen;
    InCallPresenter::GetInstance()->SetFullScreenVideoState(mIsFullScreen);
}

ECode VideoCallPresenter::OnIncomingCall(
    /* [in] */ InCallState oldState,
    /* [in] */ InCallState newState,
    /* [in] */ ICall* call)
{
    // same logic should happen as with onStateChange()
    return OnStateChange(oldState, newState, (ICallList*)CallList::GetInstance().Get());
}

ECode VideoCallPresenter::OnStateChange(
    /* [in] */ InCallState oldState,
    /* [in] */ InCallState newState,
    /* [in] */ ICallList* _callList)
{
    // Bail if video calling is disabled for the device.
    AutoPtr<CallList> callList = (CallList*)_callList;
    if (!CallUtil::IsVideoEnabled(mContext)) {
        return NOERROR;
    }

    if (newState == InCallState_NO_CALLS) {
        ExitVideoMode();
    }

    // Determine the primary active call).
    AutoPtr<Call> primary;
    if (newState == InCallState_INCOMING) {
        primary = callList->GetIncomingCall();
    }
    else if (newState == InCallState_OUTGOING) {
        primary = callList->GetOutgoingCall();
    }
    else if (newState == InCallState_INCALL) {
        primary = callList->GetActiveCall();
    }

    Boolean primaryChanged = !Objects::Equals(mPrimaryCall, primary);
    if (primaryChanged) {
        mPrimaryCall = primary;

        if (primary != NULL) {
            CheckForVideoCallChange();
            mIsVideoCall = mPrimaryCall->IsVideoCall(mContext);
            if (mIsVideoCall) {
                EnterVideoMode();
            }
            else {
                ExitVideoMode();
            }
        }
        else if (primary == NULL) {
            // If no primary call, ensure we exit video state and clean up the video surfaces.
            ExitVideoMode();
        }
    }
    return NOERROR;
}

ECode VideoCallPresenter::OnDetailsChanged(
    /* [in] */ ICall* call,
    /* [in] */ ICallDetails* details)
{
    // If the details change is not for the currently active call no update is required.
    Boolean equals;
    if (IObject::Probe(call)->Equals((ICall*)mPrimaryCall.Get(), &equals), !equals) {
        return NOERROR;
    }

    CheckForVideoStateChange();
    return NOERROR;
}

void VideoCallPresenter::CheckForVideoCallChange()
{
    AutoPtr<IInCallServiceVideoCall> videoCall;
    mPrimaryCall->GetTelecommCall()->GetVideoCall((IInCallServiceVideoCall**)&videoCall);
    if (!Objects::Equals(videoCall, mVideoCall)) {
        ChangeVideoCall(videoCall);
    }
}

void VideoCallPresenter::CheckForVideoStateChange()
{
    Boolean newVideoState = mPrimaryCall->IsVideoCall(mContext);

    // Check if video state changed
    if (mIsVideoCall != newVideoState) {
        mIsVideoCall = newVideoState;

        if (mIsVideoCall) {
            EnterVideoMode();
        } else {
            ExitVideoMode();
        }
    }
}

void VideoCallPresenter::ChangeVideoCall(
    /* [in] */ IInCallServiceVideoCall* videoCall)
{
    // Null out the surfaces on the previous video call.
    if (mVideoCall != NULL) {
        mVideoCall->SetDisplaySurface(NULL);
        mVideoCall->SetPreviewSurface(NULL);
    }

    mVideoCall = videoCall;
}

void VideoCallPresenter::EnterVideoMode()
{
    AutoPtr<IUi> temp;
    GetUi((IUi**)&temp);
    AutoPtr<IVideoCallUi> ui = IVideoCallUi::Probe(temp);
    if (ui == NULL) {
        return;
    }

    ui->ShowVideoUi(TRUE);
    InCallPresenter::GetInstance()->SetInCallAllowsOrientationChange(TRUE);

    // Communicate the current camera to telephony and make a request for the camera
    // capabilities.
    if (mVideoCall != NULL) {
        // Do not reset the surfaces if we just restarted the activity due to an orientation
        // change.
        Boolean result;
        if (ui->IsActivityRestart(&result), result) {
            return;
        }

        mPreviewSurfaceState = PreviewSurfaceState::CAMERA_SET;
        AutoPtr<InCallCameraManager> cameraManager = InCallPresenter::GetInstance()->GetInCallCameraManager();
        mVideoCall->SetCamera(cameraManager->GetActiveCameraId());
        mVideoCall->RequestCameraCapabilities();

        if (ui->IsDisplayVideoSurfaceCreated(&result), result) {
            AutoPtr<ISurface> surface;
            ui->GetDisplayVideoSurface((ISurface**)&surface);
            mVideoCall->SetDisplaySurface(surface);
        }
    }

    mPreVideoAudioMode = AudioModeProvider::GetInstance()->GetAudioMode();
    TelecomAdapter::GetInstance()->SetAudioRoute(IAudioState::ROUTE_SPEAKER);
}

void VideoCallPresenter::ExitVideoMode()
{
    AutoPtr<IUi> temp;
    GetUi((IUi**)&temp);
    AutoPtr<IVideoCallUi> ui = IVideoCallUi::Probe(temp);
    if (ui == NULL) {
        return;
    }
    InCallPresenter::GetInstance()->SetInCallAllowsOrientationChange(FALSE);
    ui->ShowVideoUi(FALSE);

    if (mPreVideoAudioMode != AudioModeProvider::AUDIO_MODE_INVALID) {
        TelecomAdapter::GetInstance()->SetAudioRoute(mPreVideoAudioMode);
        mPreVideoAudioMode = AudioModeProvider::AUDIO_MODE_INVALID;
    }
}

ECode VideoCallPresenter::OnPeerPauseStateChanged(
    /* [in] */ ICall* call,
    /* [in] */ Boolean paused)
{
    Boolean equals;
    if (IObject::Probe(call)->Equals((ICall*)mPrimaryCall.Get(), &equals), !equals) {
        return NOERROR;
    }
    return NOERROR;

    // TODO(vt): Show/hide the peer contact photo.
}

ECode VideoCallPresenter::OnUpdatePeerDimensions(
    /* [in] */ ICall* call,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Boolean equals;
    if (IObject::Probe(call)->Equals((ICall*)mPrimaryCall.Get(), &equals), !equals) {
        return NOERROR;
    }
    return NOERROR;

    // TODO(vt): Change display surface aspect ratio.
}

ECode VideoCallPresenter::OnCameraDimensionsChange(
    /* [in] */ ICall* call,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    AutoPtr<IUi> temp;
    GetUi((IUi**)&temp);
    AutoPtr<IVideoCallUi> ui = IVideoCallUi::Probe(temp);
    if (ui == NULL) {
        return NOERROR;
    }

    Boolean equals;
    if (IObject::Probe(call)->Equals((ICall*)mPrimaryCall.Get(), &equals), !equals) {
        return NOERROR;
    }

    mPreviewSurfaceState = PreviewSurfaceState::CAPABILITIES_RECEIVED;

    // Configure the preview surface to the correct aspect ratio.
    Float aspectRatio = 1.0f;
    if (width > 0 && height > 0) {
        aspectRatio = (Float)width / (Float)height;
    }
    SetPreviewSize(mDeviceOrientation, aspectRatio);

    // Check if the preview surface is ready yet; if it is, set it on the {@code VideoCall}.
    // If it not yet ready, it will be set when when creation completes.
    Boolean result;
    if (ui->IsPreviewVideoSurfaceCreated(&result), result) {
        mPreviewSurfaceState = PreviewSurfaceState::SURFACE_SET;
        AutoPtr<ISurface> surface;
        ui->GetPreviewVideoSurface((ISurface**)&surface);
        mVideoCall->SetPreviewSurface(surface);
    }
    return NOERROR;
}

ECode VideoCallPresenter::OnDeviceOrientationChanged(
    /* [in] */ Int32 orientation)
{
    mDeviceOrientation = orientation;
    return NOERROR;
}

ECode VideoCallPresenter::OnUpgradeToVideoRequest(
    /* [in] */ ICall* call)
{
    mPrimaryCall->SetSessionModificationState(
            Call::SessionModificationState::RECEIVED_UPGRADE_TO_VIDEO_REQUEST);
    return NOERROR;
}

ECode VideoCallPresenter::OnUpgradeToVideoSuccess(
    /* [in] */ ICall* call)
{
    if (mPrimaryCall == NULL || !Call::AreSame(mPrimaryCall, (Call*)call)) {
        return NOERROR;
    }

    mPrimaryCall->SetSessionModificationState(Call::SessionModificationState::NO_REQUEST);
    return NOERROR;
}

ECode VideoCallPresenter::OnUpgradeToVideoFail(
    /* [in] */ ICall* _call)
{
    AutoPtr<Call> call = (Call*)_call;
    if (mPrimaryCall == NULL || !Call::AreSame(mPrimaryCall, call)) {
        return NOERROR;
    }

    call->SetSessionModificationState(Call::SessionModificationState::REQUEST_FAILED);

    // Start handler to change state from REQUEST_FAILED to NO_REQUEST after an interval.
    AutoPtr<IRunnable> runnable = (IRunnable*)new OnUpgradeToVideoFailRunnable(this);
    Boolean result;
    mSessionModificationResetHandler->PostDelayed(runnable, SESSION_MODIFICATION_RESET_DELAY_MS, &result);
    return NOERROR;
}

ECode VideoCallPresenter::OnDowngradeToAudio(
    /* [in] */ ICall* call)
{
    // Implementing to satsify interface.
    return NOERROR;
}

void VideoCallPresenter::SetPreviewSize(
    /* [in] */ Int32 orientation,
    /* [in] */ Float aspectRatio)
{
    AutoPtr<IUi> temp;
    GetUi((IUi**)&temp);
    AutoPtr<IVideoCallUi> ui = IVideoCallUi::Probe(temp);
    if (ui == NULL) {
        return;
    }

    Int32 height;
    Int32 width;

    if (orientation == IConfiguration::ORIENTATION_LANDSCAPE) {
        width = (Int32)(mMinimumVideoDimension * aspectRatio);
        height = (Int32)mMinimumVideoDimension;
    }
    else {
        width = (Int32)mMinimumVideoDimension;
        height = (Int32)(mMinimumVideoDimension * aspectRatio);
    }
    ui->SetPreviewSize(width, height);
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
