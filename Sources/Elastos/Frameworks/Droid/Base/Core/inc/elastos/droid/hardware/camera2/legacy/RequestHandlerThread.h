
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_REQUESTHANDLERTHREAD_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_REQUESTHANDLERTHREAD_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/HandlerThread.h"

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIdleHandler;
using Elastos::Droid::Os::HandlerThread;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Os::IConditionVariable;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

class RequestHandlerThread
    : public HandlerThread
    , public IRequestHandlerThread
{
private:
    class MyHandler
        : public Object
        , public IIdleHandler
    {
    public:
        CAR_INTERFACE_DECL()

        MyHandler(
            /* [in] */ RequestHandlerThread* host);

        CARAPI QueueIdle(
            /* [out] */ Boolean* result);

    private:
        RequestHandlerThread* mHost;
    };
public:
    CAR_INTERFACE_DECL()

    RequestHandlerThread();

    virtual ~RequestHandlerThread() {}

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ IHandlerCallback* callback);

    // Blocks until thread has started
    CARAPI WaitUntilStarted();

    // May return null if the handler is not set up yet.
    CARAPI GetHandler(
        /* [out] */ IHandler** outhand);

    // Blocks until thread has started
    CARAPI WaitAndGetHandler(
        /* [out] */ IHandler** outhand);

    // Atomic multi-type message existence check
    CARAPI HasAnyMessages(
        /* [in] */ ArrayOf<Int32>* what,
        /* [out] */ Boolean* value);

    // Atomic multi-type message remove
    CARAPI RemoveMessages(
        /* [in] */ ArrayOf<Int32>* what);

    // Blocks until thread is idling
    CARAPI WaitUntilIdle();

protected:
    CARAPI_(void) OnLooperPrepared();

private:
    AutoPtr<IConditionVariable> mStarted;
    AutoPtr<IConditionVariable> mIdle;
    AutoPtr<IHandlerCallback> mCallback;
    /*volatile*/ AutoPtr<IHandler> mHandler;

    AutoPtr<IIdleHandler> mIdleHandler;
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_REQUESTHANDLERTHREAD_H__
