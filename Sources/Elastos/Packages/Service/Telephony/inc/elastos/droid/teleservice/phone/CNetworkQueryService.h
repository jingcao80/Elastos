#ifndef  __ELASTOS_DROID_PHONE_CNETWORKQUERYSERVICE_H__
#define  __ELASTOS_DROID_PHONE_CNETWORKQUERYSERVICE_H__

#include "_Elastos_Droid_TeleService_Phone_CNetworkQueryService.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/AsyncResult.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IRemoteCallbackList;
using Elastos::Core::Object;


namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Service code used to assist in querying the network for service
 * availability.
 */
CarClass(CNetworkQueryService)
    , public Elastos::Droid::App::Service
    , public INetworkQueryService
{
public:
    /**
     * Class for clients to access.  Because we know this service always
     * runs in the same process as its clients, we don't need to deal with
     * IPC.
     */
    class LocalBinder
        : public Binder
        , public INetworkQueryServiceLocalBinder
    {
    public:
        TO_STRING_IMPL("CNetworkQueryService::LocalBinder")

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ INetworkQueryService* host);

        CARAPI GetService(
            /* [out] */ IINetworkQueryService** service);

    private:
        CNetworkQueryService* mHost;
    };

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CNetworkQueryService::MyHandler")

        MyHandler(
            /* [in] */ CNetworkQueryService* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CNetworkQueryService* mHost;
    };

    class MyNetworkQueryService
        : public Object
        , public IINetworkQueryService
        , public IBinder
    {
    public:
        TO_STRING_IMPL("CNetworkQueryService::MyNetworkQueryService")

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ INetworkQueryService* host);

        /**
         * Starts a query with a INetworkQueryServiceCallback object if
         * one has not been started yet.  Ignore the new query request
         * if the query has been started already.  Either way, place the
         * callback object in the queue to be notified upon request
         * completion.
         */
        CARAPI StartNetworkQuery(
            /* [in] */ IINetworkQueryServiceCallback* cb);

        /**
         * Stops a query with a INetworkQueryServiceCallback object as
         * a token.
         */
        CARAPI StopNetworkQuery(
            /* [in] */ IINetworkQueryServiceCallback* cb);

    private:
        CNetworkQueryService* mHost;
    };


public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CNetworkQueryService();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate();

    /**
     * Required for service implementation.
     */
    //@Override
    CARAPI OnStart(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 startId);

    /**
     * Handle the bind request.
     */
    //@Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

private:
    /**
     * Broadcast the results from the query to all registered callback
     * objects.
     */
    CARAPI_(void) BroadcastQueryResults(
        /* [in] */ AsyncResult* ar);

    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

public:
    // debug data
    static const String TAG;
    static const Boolean DBG;

    // static events
    static const Int32 EVENT_NETWORK_SCAN_COMPLETED  = 100;

    // static states indicating the query status of the service
    static const Int32 QUERY_READY = -1;
    static const Int32 QUERY_IS_RUNNING = -2;

    /** state of the query service */
    Int32 mState;

    /** local handle to the phone object */
    AutoPtr<IPhone> mPhone;

    AutoPtr<IBinder> mLocalBinder;

    /**
     * Local handler to receive the network query compete callback
     * from the RIL.
     */
    AutoPtr<IHandler> mHandler;

    /**
     * List of callback objects, also used to synchronize access to
     * itself and to changes in state.
     */
    AutoPtr<IRemoteCallbackList> mCallbacks;

    /**
     * Implementation of the INetworkQueryService interface.
     */
    AutoPtr<IINetworkQueryService> mBinder;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CNETWORKQUERYSERVICE_H__