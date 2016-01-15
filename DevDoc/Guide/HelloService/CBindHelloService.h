
#ifndef __CBINDHELLOSERVICE_H__
#define __CBINDHELLOSERVICE_H__

#include "_CBindHelloService.h"
#include "app/Service.h"
#include "os/Handler.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace HelloService {

class CBindHelloService : public Elastos::Droid::App::Service
{
public:
    CBindHelloService();

    CARAPI OnCreate();

    CARAPI OnStart(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 startId);

    CARAPI OnStartCommand(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 startId,
        /* [out] */ Int32* result);

    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

    CARAPI OnUnbind(
        /* [in] */ IIntent* intent,
        /* [out] */ Boolean* succeeded);

    CARAPI OnDestroy();

    CARAPI SayHello();

private:
    static const String TAG;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IBinder> mBinder;
};

} // namespace HelloService
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CBINDHELLOSERVICE_H__
