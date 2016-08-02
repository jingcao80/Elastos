
#ifndef __ELASTOS_DROID_NET_HTTP_REQUESTQUEUE_H__
#define __ELASTOS_DROID_NET_HTTP_REQUESTQUEUE_H__

#include <Elastos.CoreLibrary.Apache.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/http/ConnectionThread.h"
#include "elastos/droid/net/http/IdleCache.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::Http::ConnectionThread;
using Elastos::Droid::Net::Http::IdleCache;

using Elastos::IO::IInputStream;
using Elastos::Utility::IHashMap;
using Elastos::Utility::ILinkedHashMap;
using Elastos::Utility::IMap;

using Org::Apache::Http::IHttpHost;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * {@hide}
 */
class RequestQueue
    : public Object
    , public IRequestQueue
    , public IRequestFeeder
{
public:
    class ActivePool
        : public Object
        , public IRequestQueueConnectionManager
    {
        friend class RequestQueue;
    public:
        CAR_INTERFACE_DECL()

        ActivePool(
            /* [in] */ Int32 connectionCount,
            /* [in] */ RequestQueue* host);

        CARAPI Startup();

        CARAPI Shutdown();

        CARAPI StartConnectionThread();

        CARAPI StartTiming();

        CARAPI StopTiming();

        CARAPI LogState();

        CARAPI GetProxyHost(
            /* [out] */ IHttpHost** result);

        /**
        * Turns off persistence on all live connections
        */
        CARAPI DisablePersistence();

        /* Linear lookup -- okay for small thread counts.  Might use
        private HashMap<HttpHost, LinkedList<ConnectionThread>> mActiveMap;
        if this turns out to be a hotspot */
        CARAPI GetThread(
            /* [in] */ IHttpHost* host,
            /* [out] */ ConnectionThread** result);

        CARAPI GetConnection(
            /* [in] */ IContext* context,
            /* [in] */ IHttpHost* host,
            /* [out] */ IConnection** result);

        CARAPI RecycleConnection(
            /* [in] */ IConnection* connection,
            /* [out] */ Boolean* result);

    public:
        /** Threads used to process requests */
        AutoPtr<ArrayOf<ConnectionThread*> > mThreads;

        AutoPtr<IdleCache> mIdleCache;

    private:
        Int32 mTotalRequest;

        Int32 mTotalConnection;

        Int32 mConnectionCount;

        RequestQueue* mHost;
    };

private:
    class SyncFeeder
        : public Object
        , public IRequestFeeder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI GetRequest(
            /* [out] */ IRequest** result);

        CARAPI GetRequest(
            /* [in] */ IHttpHost* host,
            /* [out] */ IRequest** result);

        CARAPI HaveRequest(
            /* [in] */ IHttpHost* host,
            /* [out] */ Boolean* result);

        CARAPI RequeueRequest(
            /* [in] */ IRequest* r);

    private:
        // This is used in the case where the request fails and needs to be
        // requeued into the RequestFeeder.
        AutoPtr<IRequest> mRequest;
    };

    class InnerSub_BroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("RequestQueue::InnerSub_BroadcastReceiver")

        InnerSub_BroadcastReceiver(
            /* [in] */ RequestQueue* host);

        CARAPI OnReceive(
            /* [in] */ IContext* ctx,
            /* [in] */ IIntent* intent);
    private:
        RequestQueue* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * A RequestQueue class instance maintains a set of queued
     * requests.  It orders them, makes the requests against HTTP
     * servers, and makes callbacks to supplied eventHandlers as data
     * is read.  It supports request prioritization, connection reuse
     * and pipelining.
     *
     * @param context application context
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * A RequestQueue class instance maintains a set of queued
     * requests.  It orders them, makes the requests against HTTP
     * servers, and makes callbacks to supplied eventHandlers as data
     * is read.  It supports request prioritization, connection reuse
     * and pipelining.
     *
     * @param context application context
     * @param connectionCount The number of simultaneous connections
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 connectionCount);

    /**
     * Enables data state and proxy tracking
     */
    CARAPI EnablePlatformNotifications();

    /**
     * If platform notifications have been enabled, call this method
     * to disable before destroying RequestQueue
     */
    CARAPI DisablePlatformNotifications();

    /**
     * used by webkit
     * @return proxy host if set, null otherwise
     */
    CARAPI GetProxyHost(
        /* [out] */ IHttpHost** result);

    /**
     * Queues an HTTP request
     * @param url The url to load.
     * @param method "GET" or "POST."
     * @param headers A hashmap of http headers.
     * @param eventHandler The event handler for handling returned
     * data.  Callbacks will be made on the supplied instance.
     * @param bodyProvider InputStream providing HTTP body, null if none
     * @param bodyLength length of body, must be 0 if bodyProvider is null
     */
    CARAPI QueueRequest(
        /* [in] */ const String& url,
        /* [in] */ const String& method,
        /* [in] */ IMap* headers,
        /* [in] */ IEventHandler* eventHandler,
        /* [in] */ IInputStream* bodyProvider,
        /* [in] */ Int32 bodyLength,
        /* [out] */ IRequestHandle** result);

    /**
     * Queues an HTTP request
     * @param url The url to load.
     * @param uri The uri of the url to load.
     * @param method "GET" or "POST."
     * @param headers A hashmap of http headers.
     * @param eventHandler The event handler for handling returned
     * data.  Callbacks will be made on the supplied instance.
     * @param bodyProvider InputStream providing HTTP body, null if none
     * @param bodyLength length of body, must be 0 if bodyProvider is null
     */
    CARAPI QueueRequest(
        /* [in] */ const String& url,
        /* [in] */ IWebAddress* uri,
        /* [in] */ const String& method,
        /* [in] */ IMap* headers,
        /* [in] */ IEventHandler* eventHandler,
        /* [in] */ IInputStream* bodyProvider,
        /* [in] */ Int32 bodyLength,
        /* [out] */ IRequestHandle** result);

    CARAPI QueueSynchronousRequest(
        /* [in] */ const String& url,
        /* [in] */ IWebAddress* uri,
        /* [in] */ const String& method,
        /* [in] */ IMap* headers,
        /* [in] */ IEventHandler* eventHandler,
        /* [in] */ IInputStream* bodyProvider,
        /* [in] */ Int32 bodyLength,
        /* [out] */ IRequestHandle** result);

    /**
     * @return true iff there are any non-active requests pending
     */
    CARAPI RequestsPending(
        /* [out] */ Boolean* result);

    /**
     * debug tool: prints request queue to log
     */
    CARAPI Dump();

    /*
     * RequestFeeder implementation
     */
    CARAPI GetRequest(
        /* [out] */ IRequest** result);

    /**
     * @return a request for given host if possible
     */
    CARAPI GetRequest(
        /* [in] */ IHttpHost* host,
        /* [out] */ IRequest** result);

    /**
     * @return true if a request for this host is available
     */
    CARAPI HaveRequest(
        /* [in] */ IHttpHost* host,
        /* [out] */ Boolean* result);

    /**
     * Put request back on head of queue
     */
    CARAPI RequeueRequest(
        /* [in] */ IRequest* request);

    /**
     * This must be called to cleanly shutdown RequestQueue
     */
    CARAPI Shutdown();

    CARAPI QueueRequest(
        /* [in] */ IRequest* request,
        /* [in] */ Boolean head);

    CARAPI StartTiming();

    CARAPI StopTiming();

private:
    /**
     * Because our IntentReceiver can run within a different thread,
     * synchronize setting the proxy
     */
    CARAPI SetProxyConfig();

    // Chooses between the proxy and the request's host.
    CARAPI_(AutoPtr<IHttpHost>) DetermineHost(
        /* [in] */ IHttpHost* host);

    /* helper */
    CARAPI_(AutoPtr<IRequest>) RemoveFirst(
        /* [in] */ IHashMap* requestQueue);

private:
    /**
     * Requests, indexed by HttpHost (scheme, host, port)
     */
    /* const */ AutoPtr<ILinkedHashMap> mPending;

    /* const */ AutoPtr<IContext> mContext;

    /* const */ AutoPtr<ActivePool> mActivePool;

    /* const */ AutoPtr<IConnectivityManager> mConnectivityManager;

    AutoPtr<IHttpHost> mProxyHost;

    AutoPtr<IBroadcastReceiver> mProxyChangeReceiver;

    /* default simultaneous connection count */
    static const Int32 CONNECTION_COUNT;

    friend InnerSub_BroadcastReceiver;
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_REQUESTQUEUE_H__
