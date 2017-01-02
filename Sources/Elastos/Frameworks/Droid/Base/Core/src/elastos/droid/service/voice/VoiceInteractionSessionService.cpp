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

#include "elastos/droid/service/voice/VoiceInteractionSessionService.h"
#include "elastos/droid/internal/os/CHandlerCaller.h"
#include "elastos/droid/internal/os/SomeArgs.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/service/voice/VoiceInteractionSession.h"

using Elastos::Droid::Internal::Os::CHandlerCaller;
using Elastos::Droid::Internal::Os::SomeArgs;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Internal::Os::EIID_IHandlerCallerCallback;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Service::Voice::VoiceInteractionSession;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Voice {

//=========================================
//VoiceInteractionSessionService::MInterface
//=========================================

CAR_INTERFACE_IMPL_2(VoiceInteractionSessionService::MInterface, Object, IIVoiceInteractionSessionService, IBinder)

VoiceInteractionSessionService::MInterface::MInterface(
    /* [in] */ VoiceInteractionSessionService* host)
    : mHost(host)
{
}

ECode VoiceInteractionSessionService::MInterface::NewSession(
    /* [in] */ IBinder* token,
    /* [in] */ IBundle* args)
{
    AutoPtr<IMessage> msg;
    return mHost->mHandlerCaller->ObtainMessageOO(MSG_NEW_SESSION, token, args, (IMessage**)&msg);
}

//=========================================
//VoiceInteractionSessionService::MHandlerCallerCallback
//=========================================

CAR_INTERFACE_IMPL(VoiceInteractionSessionService::MHandlerCallerCallback, Object, IHandlerCallerCallback)

VoiceInteractionSessionService::MHandlerCallerCallback::MHandlerCallerCallback(
    /* [in] */ VoiceInteractionSessionService* host)
    : mHost(host)
{
}

ECode VoiceInteractionSessionService::MHandlerCallerCallback::ExecuteMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);
    Int32 what;
    msg->GetWhat(&what);
    switch(what) {
        case MSG_NEW_SESSION:{
            AutoPtr<SomeArgs> sa = (SomeArgs*)args.Get();
            mHost->DoNewSession(IBinder::Probe(sa->mArg1), IBundle::Probe(sa->mArg2));
            break;
        }
    }
    return NOERROR;
}

//=========================================
//VoiceInteractionSessionService
//=========================================

const Int32 VoiceInteractionSessionService::MSG_NEW_SESSION;

CAR_INTERFACE_IMPL(VoiceInteractionSessionService, Service, IVoiceInteractionSessionService)

VoiceInteractionSessionService::VoiceInteractionSessionService()
{
    AutoPtr<MInterface> mi = new MInterface(this);
    mInterface = (IIVoiceInteractionSessionService*)(mi.Get());
    AutoPtr<MHandlerCallerCallback> mh = new MHandlerCallerCallback(this);
    mHandlerCallerCallback = (IHandlerCallerCallback*)(mh.Get());
}

ECode VoiceInteractionSessionService::constructor()
{
    return NOERROR;
}

ECode VoiceInteractionSessionService::OnCreate()
{
    Service::OnCreate();
    AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::VOICE_INTERACTION_MANAGER_SERVICE);
    mSystemService = IIVoiceInteractionManagerService::Probe(obj);
    AutoPtr<ILooper> looper = Looper::GetMyLooper();
    CHandlerCaller::New(this, looper.Get(), mHandlerCallerCallback, TRUE, TRUE, (IHandlerCaller**)&mHandlerCaller);
    return NOERROR;
}

ECode VoiceInteractionSessionService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** b)
{
    VALIDATE_NOT_NULL(b)
    *b = IBinder::Probe(mInterface);
    REFCOUNT_ADD(*b)
    return NOERROR;
}

ECode VoiceInteractionSessionService::DoNewSession(
    /* [in] */ IBinder* token,
    /* [in] */ IBundle* args)
{
    if (mSession != NULL) {
        mSession->DoDestroy();
        mSession = NULL;
    }
    OnNewSession(args, (IVoiceInteractionSession**)&mSession);
    AutoPtr<VoiceInteractionSession> _mSession = (VoiceInteractionSession*)(mSession.Get());
    Boolean result;
    ECode ec = mSystemService->DeliverNewSession(token, _mSession->mSession, _mSession->mInteractor, &result);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    mSession->DoCreate(mSystemService, token, args);
    return NOERROR;
}

} // namespace Voice
} // namespace Service
} // namepsace Droid
} // namespace Elastos
