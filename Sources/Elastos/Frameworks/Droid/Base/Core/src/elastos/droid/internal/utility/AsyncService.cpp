
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
