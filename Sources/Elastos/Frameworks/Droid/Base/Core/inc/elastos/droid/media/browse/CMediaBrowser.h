#ifndef __ELASTOS_DROID_MEDIA_BROWSE_CMEDIABROWSER_H__
#define __ELASTOS_DROID_MEDIA_BROWSE_CMEDIABROWSER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Service.h"
#include "_Elastos_Droid_Media_Browse_CMediaBrowser.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::Pm::IParceledListSlice;
using Elastos::Droid::Service::Media::IIMediaBrowserService;
using Elastos::Droid::Service::Media::IIMediaBrowserServiceCallbacks;
using Elastos::Droid::Media::Session::IMediaSessionToken;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Browse {

/**
 * Browses media content offered by a link MediaBrowserService.
 * <p>
 * This object is not thread-safe. All calls should happen on the thread on which the browser
 * was constructed.
 * </p>
 */
CarClass(CMediaBrowser)
    , public Object
    , public IMediaBrowser
{
private:
    class MediaServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        MediaServiceConnection(
            /* [in] */ CMediaBrowser * host);

        /**
         * Called when a connection to the Service has been established, with
         * the {@link android.os.IBinder} of the communication channel to the
         * Service.
         *
         * @param name The concrete component name of the service that has
         * been connected.
         *
         * @param service The IBinder of the Service's communication channel,
         * which you can now make calls on.
         */
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        /**
         * Called when a connection to the Service has been lost.  This typically
         * happens when the process hosting the service has crashed or been killed.
         * This does <em>not</em> remove the ServiceConnection itself -- this
         * binding to the service will remain active, and you will receive a call
         * to {@link #onServiceConnected} when the Service is next running.
         *
         * @param name The concrete component name of the service whose
         * connection has been lost.
         */
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        /**
         * Return true if this is the current ServiceConnection.  Also logs if it's not.
         */
        CARAPI_(Boolean) IsCurrent(
            /* [in] */ const String& funcName);

        CMediaBrowser * mHost;
    };

    class ServiceCallbacks
        : public Object
        , public IIMediaBrowserServiceCallbacks
    {
    public:
        CAR_INTERFACE_DECL()

        ServiceCallbacks(
            // /* [in] */ IMediaBrowser * mediaBrowser,
            /* [in] */ CMediaBrowser * host);

        /**
         * The other side has acknowledged our connection.  The parameters to this function
         * are the initial data as requested.
         */
        // @Override
        CARAPI OnConnect(
            /* [in] */ const String& root,
            /* [in] */ IMediaSessionToken * session,
            /* [in] */ IBundle * extras);

        /**
         * The other side does not like us.  Tell the app via onConnectionFailed.
         */
        // @Override
        CARAPI OnConnectFailed();

        // @Override
        CARAPI OnLoadChildren(
            /* [in] */ const String& parentId,
            /* [in] */ IParceledListSlice * list);

    private:
        // AutoPtr<IMediaBrowser> mMediaBrowser;
        CMediaBrowser * mMediaBrowser;
    };

    class Subscription
        : public Object
    {
    public:
        Subscription(
            /* [in] */ const String& id);

    public:
        String mId;
        AutoPtr<IMediaBrowserSubscriptionCallback> mCallback;
    };

private:
    class ConnectRunnable
        : public Runnable
    {
    public:
        ConnectRunnable(
            /* [in] */ CMediaBrowser* host,
            /* [in] */ IServiceConnection * sc)
            : mHost(host)
            , mServiceConnection(sc)
        {}

        CARAPI Run();

    private:
        CMediaBrowser* mHost;
        AutoPtr<IServiceConnection> mServiceConnection;
    };

    class ServiceConnectedRunnable
        : public Runnable
    {
    public:
        ServiceConnectedRunnable(
            /* [in] */ CMediaBrowser* host,
            /* [in] */ IIMediaBrowserServiceCallbacks * callback,
            /* [in] */ const String& root,
            /* [in] */ IMediaSessionToken * session,
            /* [in] */ IBundle * extra)
            : mHost(host)
            , mCallback(callback)
            , mRoot(root)
            , mSession(session)
            , mExtra(extra)
        {}

        CARAPI Run();

    private:
        CMediaBrowser* mHost;
        AutoPtr<IIMediaBrowserServiceCallbacks> mCallback;
        String mRoot;
        AutoPtr<IMediaSessionToken> mSession;
        AutoPtr<IBundle> mExtra;
    };

    class ConnectionFailedRunnable
        : public Runnable
    {
    public:
        ConnectionFailedRunnable(
            /* [in] */ CMediaBrowser* host,
            /* [in] */ IIMediaBrowserServiceCallbacks * cb)
            : mHost(host)
            , mCallback(cb)
        {}

        CARAPI Run();

    private:
        CMediaBrowser* mHost;
        AutoPtr<IIMediaBrowserServiceCallbacks> mCallback;
    };

    class LoadChildrenRunnable
        : public Runnable
    {
    public:
        LoadChildrenRunnable(
            /* [in] */ CMediaBrowser* host,
            /* [in] */ IIMediaBrowserServiceCallbacks * cb,
            /* [in] */ const String& parentId,
            /* [in] */ IParceledListSlice * list)
            : mHost(host)
            , mCallback(cb)
            , mParentId(parentId)
            , mList(list)
        {}

        CARAPI Run();

    private:
        CMediaBrowser* mHost;
        AutoPtr<IIMediaBrowserServiceCallbacks> mCallback;
        String mParentId;
        AutoPtr<IParceledListSlice> mList;
    };

public:
    CMediaBrowser();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CMediaBrowser();

    /**
     * Creates a media browser for the specified media browse service.
     *
     * @param context The context.
     * @param serviceComponent The component name of the media browse service.
     * @param callback The connection callback.
     * @param rootHints An optional bundle of service-specific arguments to send
     * to the media browse service when connecting and retrieving the root id
     * for browsing, or null if none.  The contents of this bundle may affect
     * the information returned when browsing.
     */
    CARAPI constructor(
        /* [in] */ IContext * context,
        /* [in] */ IComponentName * serviceComponent,
        /* [in] */ IMediaBrowserConnectionCallback * cb,
        /* [in] */ IBundle * rootHints);

    /**
     * Connects to the media browse service.
     * <p>
     * The connection callback specified in the constructor will be invoked
     * when the connection completes or fails.
     * </p>
     */
    CARAPI Connect();

    /**
     * Disconnects from the media browse service.
     * After this, no more callbacks will be received.
     */
    CARAPI Disconnect();

    /**
     * Returns whether the browser is connected to the service.
     */
    CARAPI IsConnected(
        /* [out] */ Boolean * result);

    /**
     * Gets the service component that the media browser is connected to.
     */
    CARAPI GetServiceComponent(
        /* [out] */ IComponentName ** result);

    /**
     * Gets the root id.
     * <p>
     * Note that the root id may become invalid or change when when the
     * browser is disconnected.
     * </p>
     *
     * @throws IllegalStateException if not connected.
     */
    CARAPI GetRoot(
        /* [out] */ String * result);

    /**
     * Gets any extras for the media service.
     *
     * @throws IllegalStateException if not connected.
     */
    CARAPI GetExtras(
        /* [out] */ IBundle ** result);

    /**
     * Gets the media session token associated with the media browser.
     * <p>
     * Note that the session token may become invalid or change when when the
     * browser is disconnected.
     * </p>
     *
     * @return The session token for the browser, never null.
     *
     * @throws IllegalStateException if not connected.
     */
     CARAPI GetSessionToken(
        /* [out] */ IMediaSessionToken ** result);

    /**
     * Queries for information about the media items that are contained within
     * the specified id and subscribes to receive updates when they change.
     * <p>
     * The list of subscriptions is maintained even when not connected and is
     * restored after reconnection.  It is ok to subscribe while not connected
     * but the results will not be returned until the connection completes.
     * </p><p>
     * If the id is already subscribed with a different callback then the new
     * callback will replace the previous one.
     * </p>
     *
     * @param parentId The id of the parent media item whose list of children
     * will be subscribed.
     * @param callback The callback to receive the list of children.
     */
    CARAPI Subscribe(
        /* [in] */ const String& parentId,
        /* [in] */ IMediaBrowserSubscriptionCallback * cb);

    /**
     * Unsubscribes for changes to the children of the specified media id.
     * <p>
     * The query callback will no longer be invoked for results associated with
     * this id once this method returns.
     * </p>
     *
     * @param parentId The id of the parent media item whose list of children
     * will be unsubscribed.
     */
    CARAPI Unsubscribe(
        /* [in] */ const String& parentId);

private:
    /**
     * Null out the variables and unbind from the service.  This doesn't include
     * calling disconnect on the service, because we only try to do that in the
     * clean shutdown cases.
     * <p>
     * Everywhere that calls this EXCEPT for disconnect() should follow it with
     * a call to mCallback.onConnectionFailed().  Disconnect doesn't do that callback
     * for a clean shutdown, but everywhere else is a dirty shutdown and should
     * notify the app.
     */
    CARAPI_(void) ForceCloseConnection();

    /**
     * For debugging.
     */
    static String GetStateLabel(
        /* [in] */ Int32 state);

    CARAPI_(void) OnServiceConnected(
        /* [in] */ IIMediaBrowserServiceCallbacks * callback,
        /* [in] */ const String& root,
        /* [in] */ IMediaSessionToken * session,
        /* [in] */ IBundle * extra);

    CARAPI_(void) OnConnectionFailed(
        /* [in] */ IIMediaBrowserServiceCallbacks * callback);

    CARAPI_(void) OnLoadChildren(
        /* [in] */ IIMediaBrowserServiceCallbacks * callback,
        /* [in] */ const String& parentId,
        /* [in] */ IParceledListSlice * list);

    /**
     * Return true if {@code callback} is the current ServiceCallbacks.  Also logs if it's not.
     */
    CARAPI_(Boolean) IsCurrent(
        /* [in] */ IIMediaBrowserServiceCallbacks * callback,
        /* [in] */ const String& funcName);

    CARAPI_(AutoPtr<ServiceCallbacks>) GetNewServiceCallbacks();

    /**
     * Log internal state.
     * @hide
     */
    CARAPI_(void) Dump();

private:
    static String TAG; // = "MediaBrowser";
    static Boolean DBG; // = FALSE;

    static const Int32 CONNECT_STATE_DISCONNECTED; // = 0;
    static const Int32 CONNECT_STATE_CONNECTING; // = 1;
    static const Int32 CONNECT_STATE_CONNECTED; // = 2;
    static const Int32 CONNECT_STATE_SUSPENDED; // = 3;

    AutoPtr<IContext> mContext;
    AutoPtr<IComponentName> mServiceComponent;
    AutoPtr<IMediaBrowserConnectionCallback> mCallback;
    AutoPtr<IBundle> mRootHints;
    AutoPtr<IHandler> mHandler;
    // ArrayMap<String,Subscription> mSubscriptions = new ArrayMap<String, MediaBrowser.Subscription>();
    AutoPtr<IArrayMap> mSubscriptions;

    Int32 mState;
    AutoPtr<MediaServiceConnection> mServiceConnection;
    AutoPtr<IIMediaBrowserService> mServiceBinder;
    AutoPtr<IIMediaBrowserServiceCallbacks> mServiceCallbacks;
    String mRootId;
    AutoPtr<IMediaSessionToken> mMediaSessionToken;
    AutoPtr<IBundle> mExtras;
};

} // namespace Browse
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_BROWSE_CMEDIABROWSER_H__
