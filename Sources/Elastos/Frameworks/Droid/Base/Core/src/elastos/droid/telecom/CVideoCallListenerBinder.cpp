
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/CVideoCallListenerBinder.h"
#include "elastos/droid/telecom/VideoCallImpl.h"
#include "elastos/droid/internal/os/CSomeArgsHelper.h"

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Os::ISomeArgsHelper;
using Elastos::Droid::Internal::Os::CSomeArgsHelper;
using Elastos::Droid::Internal::Telecom::EIID_IIVideoCallback;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// CVideoCallListenerBinder::
//===============================================================
CAR_OBJECT_IMPL(CVideoCallListenerBinder)

CAR_INTERFACE_IMPL_2(CVideoCallListenerBinder, Object, IVideoCallListenerBinder, IIVideoCallback)

ECode CVideoCallListenerBinder::constructor(
    /* [in] */ IHandler* handler)
{
    mHandler = handler;
    return NOERROR;
}

ECode CVideoCallListenerBinder::ReceiveSessionModifyRequest(
    /* [in] */ IVideoProfile* videoProfile)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(VideoCallImpl::MSG_RECEIVE_SESSION_MODIFY_REQUEST,
            videoProfile, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CVideoCallListenerBinder::ReceiveSessionModifyResponse(
    /* [in] */ Int32 status,
    /* [in] */ IVideoProfile* requestProfile,
    /* [in] */ IVideoProfile* responseProfile)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);

    AutoPtr<IInteger32> pStatus;
    CInteger32::New(status, (IInteger32**)&pStatus);
    args->SetObjectArg(1, pStatus);

    args->SetObjectArg(2, requestProfile);
    args->SetObjectArg(3, responseProfile);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(VideoCallImpl::MSG_RECEIVE_SESSION_MODIFY_RESPONSE,
            args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CVideoCallListenerBinder::HandleCallSessionEvent(
    /* [in] */ Int32 event)
{
    AutoPtr<IInteger32> pEvent;
    CInteger32::New(event, (IInteger32**)&pEvent);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(VideoCallImpl::MSG_HANDLE_CALL_SESSION_EVENT,
            pEvent, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CVideoCallListenerBinder::ChangePeerDimensions(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);

    AutoPtr<IInteger32> pWidth;
    CInteger32::New(width, (IInteger32**)&pWidth);
    args->SetObjectArg(1, pWidth);

    AutoPtr<IInteger32> pHeight;
    CInteger32::New(height, (IInteger32**)&pHeight);
    args->SetObjectArg(2, pHeight);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(VideoCallImpl::MSG_CHANGE_PEER_DIMENSIONS,
            args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CVideoCallListenerBinder::ChangeCallDataUsage(
    /* [in] */ Int32 dataUsage)
{
    AutoPtr<IInteger32> pDataUsage;
    CInteger32::New(dataUsage, (IInteger32**)&pDataUsage);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(VideoCallImpl::MSG_CHANGE_CALL_DATA_USAGE,
            pDataUsage, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CVideoCallListenerBinder::ChangeCameraCapabilities(
    /* [in] */ ICameraCapabilities* cameraCapabilities)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(VideoCallImpl::MSG_CHANGE_CAMERA_CAPABILITIES,
            cameraCapabilities, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos