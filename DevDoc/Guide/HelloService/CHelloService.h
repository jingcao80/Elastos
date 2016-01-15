
#ifndef __CHELLOSERCICE_H__
#define __CHELLOSERCICE_H__

#include "_CHelloService.h"
#include "app/Service.h"
#include "os/Handler.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IHandlerCallback;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace HelloService {

class CHelloService : public Elastos::Droid::App::Service
{
public:
    class HelloHandlerCallback
        : public IHandlerCallback
        , public ElRefBase
    {
    public :
        HelloHandlerCallback(
            /* [in] */ CHelloService* host);

        CAR_INTERFACE_DECL();

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

    private:
        CHelloService* mHost;
    };

public:
    CHelloService();

    CARAPI OnCreate();

    CARAPI OnStart(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 startId);

    CARAPI OnStartCommand(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 startId,
        /* [out] */ Int32* result);

   CARAPI OnDestroy();

private:
    static const String TAG;
    AutoPtr<IHandlerCallback> mHandlerCallback;
    AutoPtr<IHandler> mHandler;
};

} // namespace HelloService
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CHELLOSERCICE_H__
