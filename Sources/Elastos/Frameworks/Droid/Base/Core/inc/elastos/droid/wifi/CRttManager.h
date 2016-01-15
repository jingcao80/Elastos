
#ifndef __ELASTOS_DROID_NET_WIFI_CRTTMANAGER_H__
#define __ELASTOS_DROID_NET_WIFI_CRTTMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_Wifi_CRttManager.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Utility::Concurrent::ICountDownLatch;

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CRttManager)
    , public Object
    , public IRttManager
{
private:
    class ServiceHandler : public Handler
    {
    public:
        ServiceHandler(
            /* [in] */ ILooper* looper);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        CARAPI ReportSuccess(
            /* [in] */ IObject* listener,
            /* [in] */ IMessage* msg);

        CARAPI ReportFailure(
            /* [in] */ IObject* listener,
            /* [in] */ IMessage* msg);
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIRttManager* service);

    CARAPI GetCapabilities(
        /* [out] */ IRttManagerCapabilities** result);

    CARAPI StartRanging(
        /* [in] */ ArrayOf<IRttManagerRttParams*>* params,
        /* [in] */ IRttManagerRttListener* listener);

    CARAPI StopRanging(
        /* [in] */ IRttManagerRttListener* listener);

private:
    CARAPI_(void) Init();

    CARAPI ValidateChannel();

    static CARAPI_(Int32) PutListener(
        /* [in] */ IObject* listener);

    static CARAPI_(AutoPtr<IObject>) GetListener(
        /* [in] */ Int32 key);

    static CARAPI_(Int32) GetListenerKey(
        /* [in] */ IObject* listener);

    static CARAPI_(AutoPtr<IObject>) RemoveListener(
        /* [in] */ Int32 key);

    static CARAPI_(Int32) RemoveListener(
        /* [in] */ IObject* listener);

private:
    static const Boolean DBG;
    static const String TAG;

    AutoPtr<IContext> mContext;
    AutoPtr<IIRttManager> mService;

    static const Int32 INVALID_KEY = 0;
    static Int32 sListenerKey;

    static AutoPtr<ISparseArray> sListenerMap;
    static Object sListenerMapLock;

    static AutoPtr<IAsyncChannel> sAsyncChannel;
    static AutoPtr<ICountDownLatch> sConnected;

    static Object sThreadRefLock;
    static Int32 sThreadRefCount;
    static AutoPtr<IHandlerThread> sHandlerThread;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CRTTMANAGER_H__
