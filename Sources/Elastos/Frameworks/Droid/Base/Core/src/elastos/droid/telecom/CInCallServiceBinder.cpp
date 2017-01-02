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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/CInCallServiceBinder.h"
#include "elastos/droid/telecom/InCallService.h"
#include "elastos/droid/internal/os/CSomeArgsHelper.h"

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Os::ISomeArgsHelper;
using Elastos::Droid::Internal::Os::CSomeArgsHelper;
using Elastos::Droid::Internal::Telecom::EIID_IIInCallService;
using Elastos::Droid::Telecom::InCallService;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Telecom {

CAR_OBJECT_IMPL(CInCallServiceBinder)

CAR_INTERFACE_IMPL_2(CInCallServiceBinder, Object, IIInCallService, IBinder)

ECode CInCallServiceBinder::constructor(
    /* [in] */ IHandler* handler)
{
    mHandler = handler;
    return NOERROR;
}

ECode CInCallServiceBinder::SetInCallAdapter(
    /* [in] */ IIInCallAdapter* inCallAdapter)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(InCallService::MSG_SET_IN_CALL_ADAPTER, inCallAdapter, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CInCallServiceBinder::AddCall(
    /* [in] */ IParcelableCall* call)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(InCallService::MSG_ADD_CALL, call, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CInCallServiceBinder::UpdateCall(
    /* [in] */ IParcelableCall* call)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(InCallService::MSG_UPDATE_CALL, call, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CInCallServiceBinder::SetPostDial(
    /* [in] */ const String& callId,
    /* [in] */ const String& remaining)
{
    // TODO: Unused
    return NOERROR;
}

ECode CInCallServiceBinder::SetPostDialWait(
    /* [in] */ const String& callId,
    /* [in] */ const String& remaining)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    args->SetObjectArg(1, pId);
    AutoPtr<ICharSequence> pRemain;
    CString::New(remaining, (ICharSequence**)&pRemain);
    args->SetObjectArg(2, pRemain);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(InCallService::MSG_SET_POST_DIAL_WAIT, args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CInCallServiceBinder::OnAudioStateChanged(
    /* [in] */ IAudioState* audioState)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(InCallService::MSG_ON_AUDIO_STATE_CHANGED, audioState, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CInCallServiceBinder::BringToForeground(
    /* [in] */ Boolean showDialpad)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(InCallService::MSG_BRING_TO_FOREGROUND, showDialpad ? 1 : 0, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos
