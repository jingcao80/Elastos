#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_ASYNCSERVICE_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_ASYNCSERVICE_H__

#include "elastos/droid/app/Service.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

class AsyncService
    : public Service
    , public IAsyncService
{
public:
    class AsyncServiceInfo
        : public Object
        , public IAsyncServiceInfo
    {
    public:
        CAR_INTERFACE_DECL()

        /** Message Handler that will receive messages */
        AutoPtr<IHandler> mHandler;

        /**
         * The flags returned by onStartCommand on how to restart.
         * For instance @see android.app.Service#START_STICKY
         */
        Int32 mRestartFlags;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Get the handler
     */
    CARAPI GetHandler(
        /* [out] */ IHandler** handler);

    /**
     * onCreate
     */
    CARAPI OnCreate();

    /**
     * Sends the CMD_ASYNC_SERVICE_ON_START_INTENT message.
     */
    CARAPI OnStartCommand(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 startId,
        /* [out] */ Int32* res);

    /**
     * Called when service is destroyed. After returning the
     * service is dead and no more processing should be expected
     * to occur.
     */
    CARAPI OnDestroy();

    /**
     * Returns the Messenger's binder.
     */
    CARAPI OnBind(
        /* [in] */ Intent* intent,
        /* [out] */ IBinder** binder);

protected:
    static const Boolean DBG;

    /** Messenger transport */
    AutoPtr<IMessenger> mMessenger;

    /** Message Handler that will receive messages */
    AutoPtr<IHandler> mHandler;

    AutoPtr<AsyncServiceInfo> mAsyncServiceInfo;

private:
    static const String TAG;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_ASYNCSERVICE_H__
