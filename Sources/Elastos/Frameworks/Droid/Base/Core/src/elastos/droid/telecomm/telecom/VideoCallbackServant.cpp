
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/VideoCallbackServant.h"

using Elastos::Droid::Internal::Os::ISomeArgs;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

const Int32 VideoCallbackServant::MSG_RECEIVE_SESSION_MODIFY_REQUEST = 0;
const Int32 VideoCallbackServant::MSG_RECEIVE_SESSION_MODIFY_RESPONSE = 1;
const Int32 VideoCallbackServant::MSG_HANDLE_CALL_SESSION_EVENT = 2;
const Int32 VideoCallbackServant::MSG_CHANGE_PEER_DIMENSIONS = 3;
const Int32 VideoCallbackServant::MSG_CHANGE_CALL_DATA_USAGE = 4;
const Int32 VideoCallbackServant::MSG_CHANGE_CAMERA_CAPABILITIES = 5;

//===============================================================
// VideoCallbackServant::MyHandler::
//===============================================================
VideoCallbackServant::MyHandler::MyHandler(
    /* [in] */ VideoCallbackServant* host)
    : mHost(host)
{}

ECode VideoCallbackServant::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    InternalHandleMessage(msg);
    return NOERROR;
}

void VideoCallbackServant::MyHandler::InternalHandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case MSG_RECEIVE_SESSION_MODIFY_REQUEST: {
            AutoPtr<IVideoProfile> pObj = IVideoProfile::Probe(obj);
            mHost->mDelegate->ReceiveSessionModifyRequest(pObj);
            break;
        }
        case MSG_RECEIVE_SESSION_MODIFY_RESPONSE: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);

            Int32 argi1 = 0;
            args->GetInt32Arg(1, &argi1);
            AutoPtr<IInterface> arg1;
            args->GetObjectArg(1, (IInterface**)&arg1);
            AutoPtr<IVideoProfile> pArg1 = IVideoProfile::Probe(arg1);
            AutoPtr<IInterface> arg2;
            args->GetObjectArg(2, (IInterface**)&arg2);
            AutoPtr<IVideoProfile> pArg2 = IVideoProfile::Probe(arg2);
            mHost->mDelegate->ReceiveSessionModifyResponse(
                    argi1,
                    pArg1,
                    pArg2);

            args->Recycle();
            break;
        }
        case MSG_HANDLE_CALL_SESSION_EVENT: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);

            Int32 argi1 = 0;
            args->GetInt32Arg(1, &argi1);
            mHost->mDelegate->HandleCallSessionEvent(argi1);

            args->Recycle();
            break;
        }
        case MSG_CHANGE_PEER_DIMENSIONS: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);

            Int32 argi1 = 0;
            args->GetInt32Arg(1, &argi1);
            Int32 argi2 = 0;
            args->GetInt32Arg(2, &argi2);
            mHost->mDelegate->ChangePeerDimensions(argi1, argi2);

            args->Recycle();
            break;
        }
        case MSG_CHANGE_CALL_DATA_USAGE: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);

            Int32 argi1 = 0;
            args->GetInt32Arg(1, &argi1);
            mHost->mDelegate->ChangeCallDataUsage(argi1);

            args->Recycle();
            break;
        }
        case MSG_CHANGE_CAMERA_CAPABILITIES: {
            AutoPtr<ICameraCapabilities> pObj = ICameraCapabilities::Probe(obj);
            mHost->mDelegate->ChangeCameraCapabilities(pObj);
            break;
        }
    }
}

//===============================================================
// VideoCallbackServant::
//===============================================================
CAR_INTERFACE_IMPL(VideoCallbackServant, Object, IVideoCallbackServant)

VideoCallbackServant::VideoCallbackServant()
{
}

ECode VideoCallbackServant::constructor(
    /* [in] */ IIVideoCallback* delegate)
{
    AutoPtr<MyHandler> h = new MyHandler(this);
    h->constructor();
    mHandler = h.Get();

    mDelegate = delegate;
    return NOERROR;
}

ECode VideoCallbackServant::GetStub(
    /* [out] */ IIVideoCallback** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStub;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos