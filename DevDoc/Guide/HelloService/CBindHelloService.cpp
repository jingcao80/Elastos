
#include <ext/frameworkdef.h>
#include "CBindHelloService.h"
#include <elastos/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::EIID_IHandlerCallback;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace HelloService {

const String CBindHelloService::TAG("CBindHelloService");

CBindHelloService::CBindHelloService()
{
    CHelloBinder::New((IBinder**)&mBinder);
}

ECode CBindHelloService::OnCreate()
{
    Logger::D(TAG, "OnCreate");
    return NOERROR;
}

ECode CBindHelloService::OnStart(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 startId)
{
    Logger::D(TAG, "Service OnStart");
    return NOERROR;
}

ECode CBindHelloService::OnStartCommand(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 startId,
    /* [out] */ Int32* result)
{
    return NOERROR;
}

ECode CBindHelloService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    Logger::D(TAG, "Service OnBind");
    VALIDATE_NOT_NULL(binder);
    *binder = mBinder;
    INTERFACE_ADDREF(*binder);
    return NOERROR;
}

ECode CBindHelloService::OnUnbind(
    /* [in] */ IIntent* intent,
    /* [out] */ Boolean* succeeded)
{
    Logger::D(TAG, "Service OnUnbind");
    return NOERROR;
}

ECode CBindHelloService::OnDestroy()
{
    Logger::D(TAG, "Service OnDestroy");
    return NOERROR;
}

ECode CBindHelloService::SayHello()
{
    Logger::D(TAG, "SayHello");
    return NOERROR;
}

} // namespace HelloService
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
