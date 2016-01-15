
#include <ext/frameworkdef.h>
#include "CHelloService.h"
#include <elastos/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::EIID_IHandlerCallback;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::CLooperHelper;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace HelloService {

CHelloService::HelloHandlerCallback::HelloHandlerCallback(
    /* [in] */ CHelloService *host)
    : mHost(host)
{}

CAR_INTERFACE_IMPL(CHelloService::HelloHandlerCallback, IHandlerCallback)

ECode CHelloService::HelloHandlerCallback::HandleMessage(
    /* [in] */ IMessage* msg,
    /* [in] */ Boolean* result)
{
    Logger::D(TAG, "HelloHandler HandleMessage");
    mHost->StopSelf();
    return NOERROR;
}


const String CHelloService::TAG("CHelloService");

CHelloService::CHelloService()
{
    mHandlerCallback = new HelloHandlerCallback(this);
}

ECode CHelloService::OnCreate()
{
    Logger::D(TAG, "CHelloService OnCreate");
    AutoPtr<IHandlerThread> thread;
    CHandlerThread::New(String("ServiceStartArguments"),
        IProcess::THREAD_PRIORITY_BACKGROUND, (IHandlerThread**)&thread);

    thread->Start();

    AutoPtr<ILooper> looper;
    thread->GetLooper((ILooper**)&looper);

    CHandler::New(looper, mHandlerCallback, TRUE, TRUE, (IHandler**)&mHandler);
    return NOERROR;
}

ECode CHelloService::OnStart(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 startId)
{
    Logger::D(TAG, "Service OnStart");
    return NOERROR;
}

ECode CHelloService::OnStartCommand(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 startId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(flags, (IMessage**)&msg);
    msg->SetArg1(startId);
    Boolean sendRst;
    mHandler->SendMessage(msg, &sendRst);
    *result = IService::START_NOT_STICKY;
    Logger::D(TAG, "Service OnStartCommand ");
    return NOERROR;
}

ECode CHelloService::OnDestroy()
{
    Logger::D(TAG, "Service OnDestroy");
    return NOERROR;
}

} // namespace HelloService
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
