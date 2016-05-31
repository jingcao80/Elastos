
#include "elastos/droid/app/IntentService.h"
#include "elastos/droid/os/CHandlerThread.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Core::StringBuilder;
using Elastos::Core::IThread;

namespace Elastos {
namespace Droid {
namespace App {

//==================================================================
// IntentService::ServiceHandler
//==================================================================
IntentService::ServiceHandler::ServiceHandler(
    /* [in] */ IntentService* host)
    : mHost(host)
{
}

ECode IntentService::ServiceHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    IIntent* it = IIntent::Probe(obj);
    mHost->OnHandleIntent(it);
    Int32 arg1;
    msg->GetArg1(&arg1);
    mHost->StopSelf(arg1);
    return NOERROR;
}

//==================================================================
// IntentService
//==================================================================
CAR_INTERFACE_IMPL(IntentService, Service, IIntentService)

IntentService::IntentService()
    : mRedelivery(FALSE)
{}

IntentService::~IntentService()
{}

ECode IntentService::constructor(
    /* [in] */ const String& name)
{
    FAIL_RETURN(Service::constructor())
    mName = name;
    return NOERROR;
}

ECode IntentService::SetIntentRedelivery(
    /* [in] */ Boolean enabled)
{
    mRedelivery = enabled;
    return NOERROR;
}

ECode IntentService::OnCreate()
{
    // TODO: It would be nice to have an option to hold a partial wakelock
    // during processing, and to have a static startService(Context, Intent)
    // method that would launch the service & hand off a wakelock.
    Service::OnCreate();
    AutoPtr<IHandlerThread> thread;
    StringBuilder sb("IntentService[");
    sb += mName;
    sb += "]";
    CHandlerThread::New(sb.ToString(), (IHandlerThread**)&thread);
    IThread::Probe(thread)->Start();

    thread->GetLooper((ILooper**)&mServiceLooper);
    mServiceHandler = new ServiceHandler(this);
    mServiceHandler->constructor(mServiceLooper);
    return NOERROR;
}

ECode IntentService::OnStart(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 startId)
{
    AutoPtr<IMessage> msg;
    mServiceHandler->ObtainMessage((IMessage**)&msg);
    msg->SetArg1(startId);
    msg->SetObj(intent);
    Boolean bval;
    mServiceHandler->SendMessage(msg, &bval);
    return NOERROR;
}

ECode IntentService::OnStartCommand(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 startId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    OnStart(intent, startId);
    *result = mRedelivery ? IService::START_REDELIVER_INTENT : IService::START_NOT_STICKY;
    return NOERROR;
}

ECode IntentService::OnDestroy()
{
    mServiceLooper->Quit();
    return NOERROR;
}

ECode IntentService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder)
    *binder = NULL;
    return NOERROR;
}


} // namespace App
} // namespace Droid
} // namespace Elastos
