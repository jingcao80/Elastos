
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/VideoCallImpl.h"
#include "elastos/droid/telecomm/telecom/CVideoCallListenerBinder.h"
#include "elastos/droid/os/CLooperHelper.h"

using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::IBinder;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

const Int32 VideoCallImpl::MSG_RECEIVE_SESSION_MODIFY_REQUEST = 1;
const Int32 VideoCallImpl::MSG_RECEIVE_SESSION_MODIFY_RESPONSE = 2;
const Int32 VideoCallImpl::MSG_HANDLE_CALL_SESSION_EVENT = 3;
const Int32 VideoCallImpl::MSG_CHANGE_PEER_DIMENSIONS = 4;
const Int32 VideoCallImpl::MSG_CHANGE_CALL_DATA_USAGE = 5;
const Int32 VideoCallImpl::MSG_CHANGE_CAMERA_CAPABILITIES = 6;

//===============================================================
// VideoCallImpl::ProxyDeathRecipient::
//===============================================================
CAR_INTERFACE_IMPL(VideoCallImpl::ProxyDeathRecipient, Object, IProxyDeathRecipient)

VideoCallImpl::ProxyDeathRecipient::ProxyDeathRecipient(
    /* [in] */ VideoCallImpl* host)
    : mHost(host)
{}

ECode VideoCallImpl::ProxyDeathRecipient::ProxyDied()
{
    AutoPtr<IProxy> pro = (IProxy*)(mHost->mVideoProvider->Probe(EIID_IProxy));
    if (pro != NULL) {
        Boolean b = FALSE;
        pro->UnlinkToDeath(this, 0, &b);
    }
    return NOERROR;
}

//===============================================================
// VideoCallImpl::MyHandler::
//===============================================================
VideoCallImpl::MyHandler::MyHandler(
    /* [in] */ ILooper* loop,
    /* [in] */ VideoCallImpl* host)
    : Handler(loop)
    , mHost(host)
{}

ECode VideoCallImpl::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    if (mHost->mVideoCallListener == NULL) {
        return NOERROR;
    }

    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<ISomeArgs> args;
    switch (what) {
        case MSG_RECEIVE_SESSION_MODIFY_REQUEST: {
            AutoPtr<IVideoProfile> vp = IVideoProfile::Probe(obj);
            mHost->mVideoCallListener->OnSessionModifyRequestReceived(vp);
            break;
        }
        case MSG_RECEIVE_SESSION_MODIFY_RESPONSE: {
            args = ISomeArgs::Probe(obj);
            AutoPtr<IInterface> arg1;
            args->GetObjectArg(1, (IInterface**)&arg1);
            AutoPtr<IInteger32> pArg1 = IInteger32::Probe(arg1);
            Int32 status = 0;
            pArg1->GetValue(&status);
            AutoPtr<IInterface> arg2;
            args->GetObjectArg(2, (IInterface**)&arg2);
            AutoPtr<IVideoProfile> requestProfile = IVideoProfile::Probe(arg2);
            AutoPtr<IInterface> arg3;
            args->GetObjectArg(3, (IInterface**)&arg3);
            AutoPtr<IVideoProfile> responseProfile = IVideoProfile::Probe(arg3);

            mHost->mVideoCallListener->OnSessionModifyResponseReceived(
                    status, requestProfile, responseProfile);
            args->Recycle();
            break;
        }
        case MSG_HANDLE_CALL_SESSION_EVENT: {
            AutoPtr<IInteger32> pObj = IInteger32::Probe(obj);
            Int32 value = 0;
            pObj->GetValue(&value);
            mHost->mVideoCallListener->OnCallSessionEvent(value);
            break;
        }
        case MSG_CHANGE_PEER_DIMENSIONS: {
            args = ISomeArgs::Probe(obj);

            AutoPtr<IInterface> arg1;
            args->GetObjectArg(1, (IInterface**)&arg1);
            AutoPtr<IInteger32> pArg1 = IInteger32::Probe(arg1);
            Int32 width = 0;
            pArg1->GetValue(&width);

            AutoPtr<IInterface> arg2;
            args->GetObjectArg(2, (IInterface**)&arg2);
            AutoPtr<IInteger32> pArg2 = IInteger32::Probe(arg2);
            Int32 height = 0;
            pArg2->GetValue(&height);
            mHost->mVideoCallListener->OnPeerDimensionsChanged(width, height);
            args->Recycle();
            break;
        }
        case MSG_CHANGE_CALL_DATA_USAGE: {
            Int32 arg1 = 0;
            msg->GetArg1(&arg1);
            mHost->mVideoCallListener->OnCallDataUsageChanged(arg1);
            break;
        }
        case MSG_CHANGE_CAMERA_CAPABILITIES: {
            AutoPtr<ICameraCapabilities> pObj = ICameraCapabilities::Probe(obj);
            mHost->mVideoCallListener->OnCameraCapabilitiesChanged(pObj);
            break;
        }
        default:
            break;
    }
    return NOERROR;
}

//===============================================================
// VideoCallImpl::
//===============================================================
CAR_INTERFACE_IMPL_2(VideoCallImpl, Object, IInCallServiceVideoCall, IVideoCallImpl)

VideoCallImpl::VideoCallImpl()
{
    mDeathRecipient = new ProxyDeathRecipient(this);
    AutoPtr<ILooperHelper> hlp;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&hlp);
    AutoPtr<ILooper> loop;
    hlp->GetMainLooper((ILooper**)&loop);
    mHandler = new MyHandler(loop, this);
}

ECode VideoCallImpl::constructor(
    /* [in] */ IIVideoProvider* videoProvider)
{
    mVideoProvider = videoProvider;
    AutoPtr<IProxy> pro = (IProxy*)(mVideoProvider->Probe(EIID_IProxy));
    if (pro != NULL) {
        pro->LinkToDeath(mDeathRecipient, 0);
    }

    CVideoCallListenerBinder::New(mHandler, (IVideoCallListenerBinder**)&mBinder);
    mVideoProvider->SetVideoCallback(IBinder::Probe(mBinder));
    return NOERROR;
}

ECode VideoCallImpl::SetVideoCallListener(
    /* [in] */ IVideoCallListener* videoCallListener)
{
    mVideoCallListener = videoCallListener;
    return NOERROR;
}

ECode VideoCallImpl::SetCamera(
    /* [in] */ const String& cameraId)
{
    mVideoProvider->SetCamera(cameraId);
    return NOERROR;
}

ECode VideoCallImpl::SetPreviewSurface(
    /* [in] */ ISurface* surface)
{
    mVideoProvider->SetPreviewSurface(surface);
    return NOERROR;
}

ECode VideoCallImpl::SetDisplaySurface(
    /* [in] */ ISurface* surface)
{
    mVideoProvider->SetDisplaySurface(surface);
    return NOERROR;
}

ECode VideoCallImpl::SetDeviceOrientation(
    /* [in] */ Int32 rotation)
{
    mVideoProvider->SetDeviceOrientation(rotation);
    return NOERROR;
}

ECode VideoCallImpl::SetZoom(
    /* [in] */ Float value)
{
    mVideoProvider->SetZoom(value);
    return NOERROR;
}

ECode VideoCallImpl::SendSessionModifyRequest(
    /* [in] */ IVideoProfile* requestProfile)
{
    mVideoProvider->SendSessionModifyRequest(requestProfile);
    return NOERROR;
}

ECode VideoCallImpl::SendSessionModifyResponse(
    /* [in] */ IVideoProfile* responseProfile)
{
    mVideoProvider->SendSessionModifyResponse(responseProfile);
    return NOERROR;
}

ECode VideoCallImpl::RequestCameraCapabilities()
{
    mVideoProvider->RequestCameraCapabilities();
    return NOERROR;
}

ECode VideoCallImpl::RequestCallDataUsage()
{
    mVideoProvider->RequestCallDataUsage();
    return NOERROR;
}

ECode VideoCallImpl::SetPauseImage(
    /* [in] */ const String& uri)
{
    mVideoProvider->SetPauseImage(uri);
    return NOERROR;
}

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos