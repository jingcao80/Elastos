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

#include "elastos/droid/internal/utility/AsyncService.h"

using Elastos::Droid::Os::CMessenger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(AsyncServiceInfo, Object, IAsyncServiceInfo)

const String AsyncService::TAG("AsyncService");
const Boolean AsyncService::DBG = TRUE;

CAR_INTERFACE_IMPL(AsyncService, Service, IAsyncService)

/**
 * Get the handler
 */
ECode AsyncService::GetHandler(
    /* [out] */ IHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    *handler = mHandler;
    REFCOUNT_ADD(*handler)
    return NOERROR;
}

/**
 * onCreate
 */
ECode AsyncService::OnCreate()
{
    FAIL_RETURN(Service::OnCreate());
    AutoPtr<IAsyncServiceInfo> asInfo;
    FAIL_RETURN(CreateHandler((IAsyncServiceInfo**)&asInfo));
    mAsyncServiceInfo = (AsyncServiceInfo*)asInfo.Get();
    mHandler = mAsyncServiceInfo->mHandler;
    return CMessenger::New(mHandler, (IMessenger**)&mMessenger);
}

/**
 * Sends the CMD_ASYNC_SERVICE_ON_START_INTENT message.
 */
ECode AsyncService::OnStartCommand(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 startId,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    if (DBG) Slogger::D(TAG, "onStartCommand");

    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain((IMessage**)&msg);
    msg->SetWhat(CMD_ASYNC_SERVICE_ON_START_INTENT);
    msg->SetArg1(flags);
    msg->SetArg2(startId);
    msg->SetObj(TO_IINTERFACE(intent));

    mHandler->SendMessage(msg);

    *res = mAsyncServiceInfo->mRestartFlags;
    return NOERROR;
}

/**
 * Called when service is destroyed. After returning the
 * service is dead and no more processing should be expected
 * to occur.
 */
ECode AsyncService::OnDestroy()
{
    if (DBG) Slogger::D(TAG, "onDestroy");

    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain((IMessage**)&msg);
    msg->SetWhat(CMD_ASYNC_SERVICE_DESTROY);
    return mHandler->SendMessage(msg);
}

/**
 * Returns the Messenger's binder.
 */
ECode AsyncService::OnBind(
    /* [in] */ Intent* intent,
    /* [out] */ IBinder** binder)
{
    return mMessenger->GetBinder(binder);
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
