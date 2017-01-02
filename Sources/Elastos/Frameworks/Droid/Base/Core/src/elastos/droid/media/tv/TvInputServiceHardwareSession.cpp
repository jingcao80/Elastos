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

#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.View.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/media/tv/TvInputServiceHardwareSession.h"
#include "elastos/droid/media/tv/TvInputService.h"
#include "elastos/droid/internal/os/SomeArgs.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Os::SomeArgs;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

ECode TvInputServiceHardwareSession::HardwareSessionCallback::OnSessionCreated(
    /* [in] */ ITvInputManagerSession* session)
{
    mHost->mHardwareSession = session;
    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    if (session != NULL) {
        args->mArg1 = mHost->mProxySession;
        args->mArg2 = mHost->mProxySessionCallback;
        AutoPtr<IBinder> binder;
        session->GetToken((IBinder**)&binder);
        args->mArg3 = binder;
    } else {
        args->mArg1 = NULL;
        args->mArg2 = mHost->mProxySessionCallback;
        args->mArg3 = NULL;
        mHost->OnRelease();
    }
    AutoPtr<IMessage> msg;
    mHost->mServiceHandler->ObtainMessage(
        TvInputService::ServiceHandler::DO_NOTIFY_SESSION_CREATED,
        (IInterface*)(IObject*)args, (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode TvInputServiceHardwareSession::HardwareSessionCallback::OnVideoAvailable(
    /* [in] */ ITvInputManagerSession* session)
{
    if (mHost->mHardwareSession.Get() == session) {
        mHost->OnHardwareVideoAvailable();
    }
    return NOERROR;
}

ECode TvInputServiceHardwareSession::HardwareSessionCallback::OnVideoUnavailable(
    /* [in] */ ITvInputManagerSession* session,
    /* [in] */ Int32 reason)
{
    if (mHost->mHardwareSession.Get() == session) {
        mHost->OnHardwareVideoUnavailable(reason);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(TvInputServiceHardwareSession, TvInputServiceSession, ITvInputServiceHardwareSession)

TvInputServiceHardwareSession::TvInputServiceHardwareSession()
{
    mHardwareSessionCallback = new HardwareSessionCallback(this);
}

TvInputServiceHardwareSession::~TvInputServiceHardwareSession()
{
}

ECode TvInputServiceHardwareSession::constructor(
    /* [in] */ IContext* context)
{
    return TvInputServiceSession::constructor(context);
}

ECode TvInputServiceHardwareSession::GetHardwareInputId(
    /* [out] */ String* result)
{
    return NOERROR;
}

ECode TvInputServiceHardwareSession::OnHardwareVideoAvailable()
{
    return NOERROR;
}

ECode TvInputServiceHardwareSession::OnHardwareVideoUnavailable(
    /* [in] */ Int32 reason)
{
    return NOERROR;
}

ECode TvInputServiceHardwareSession::OnSetSurface(
    /* [in] */ ISurface* surface,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Logger::E(TvInputService::TAG, "onSetSurface() should not be called in HardwareProxySession.");
    *result = FALSE;
    return NOERROR;
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
