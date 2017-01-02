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

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/CVideoCallback.h"
#include "elastos/droid/telecom/VideoCallbackServant.h"
#include "elastos/droid/internal/os/CSomeArgsHelper.h"

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Os::ISomeArgsHelper;
using Elastos::Droid::Internal::Os::CSomeArgsHelper;
using Elastos::Droid::Internal::Telecom::EIID_IIVideoCallback;

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// CVideoCallback::
//===============================================================
CAR_OBJECT_IMPL(CVideoCallback)

CAR_INTERFACE_IMPL(CVideoCallback, Object, IIVideoCallback)

ECode CVideoCallback::constructor(
    /* [in] */ IHandler* handler)
{
    mHandler = handler;
    return NOERROR;
}

ECode CVideoCallback::ReceiveSessionModifyRequest(
    /* [in] */ IVideoProfile* videoProfile)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(VideoCallbackServant::MSG_RECEIVE_SESSION_MODIFY_REQUEST, videoProfile, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CVideoCallback::ReceiveSessionModifyResponse(
    /* [in] */ Int32 status,
    /* [in] */ IVideoProfile* requestedProfile,
    /* [in] */ IVideoProfile* responseProfile)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);

    args->SetInt32Arg(1, status);
    args->SetObjectArg(1, requestedProfile);
    args->SetObjectArg(2, responseProfile);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(VideoCallbackServant::MSG_RECEIVE_SESSION_MODIFY_RESPONSE,
            args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CVideoCallback::HandleCallSessionEvent(
    /* [in] */ Int32 event)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);
    args->SetInt32Arg(1, event);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(VideoCallbackServant::MSG_HANDLE_CALL_SESSION_EVENT,
            args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CVideoCallback::ChangePeerDimensions(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);

    args->SetInt32Arg(1, width);
    args->SetInt32Arg(2, height);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(VideoCallbackServant::MSG_CHANGE_PEER_DIMENSIONS,
            args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CVideoCallback::ChangeCallDataUsage(
    /* [in] */ Int32 dataUsage)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);
    args->SetInt32Arg(1, dataUsage);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(VideoCallbackServant::MSG_CHANGE_CALL_DATA_USAGE,
            args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CVideoCallback::ChangeCameraCapabilities(
    /* [in] */ ICameraCapabilities* cameraCapabilities)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(VideoCallbackServant::MSG_CHANGE_CAMERA_CAPABILITIES,
            cameraCapabilities, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos