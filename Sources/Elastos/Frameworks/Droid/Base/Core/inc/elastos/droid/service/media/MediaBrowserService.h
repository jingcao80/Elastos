//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SERVICE_MEDIA_MEDIABROWSERSERVICE_H__
#define __ELASTOS_DROID_SERVICE_MEDIA_MEDIABROWSERSERVICE_H__

// TODO: Need app/Service
// #include "elastos/droid/app/Service.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.Utility.h"
#include "MediaBrowserServiceResult.h"

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
// TODO: Need app/Service
// using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Media::Session::IMediaSessionToken;
using Elastos::Core::IRunnable;
using Elastos::Core::EIID_IRunnable;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IHashSet;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Media {

/**
 * Base class for media browse services.
 * <p>
 * Media browse services enable applications to browse media content provided by an application
 * and ask the application to start playing it.  They may also be used to control content that
 * is already playing by way of a {@link MediaSession}.
 * </p>
 *
 * To extend this class, you must declare the service in your manifest file with
 * an intent filter with the {@link #SERVICE_INTERFACE} action.
 *
 * For example:
 * </p><pre>
 * &lt;service android:name=".MyMediaBrowserService"
 *          android:label="&#64;string/service_name" >
 *     &lt;intent-filter>
 *         &lt;action android:name="android.media.browse.MediaBrowserService" />
 *     &lt;/intent-filter>
 * &lt;/service>
 * </pre>
 *
 */
// TODO: Need app/Service
class MediaBrowserService
    : public /*Service*/ Object
{
private:
    class ConnectionRecord
        : public Object
    {
    public:
        String pkg;
        AutoPtr<IBundle> rootHints;
        AutoPtr<IIMediaBrowserServiceCallbacks> callbacks;
        AutoPtr<IMediaBrowserServiceBrowserRoot> root;
        AutoPtr<IHashSet> subscriptions;
    };

    class ConnectRunnable
        : public Object
        , public IRunnable
    {
    public:
        ConnectRunnable(
            /* [in] */ MediaBrowserService *mediaBrowserService,
            /* [in] */ const String& pkg,
            /* [in] */ IBundle * rootHints,
            /* [in] */ IIMediaBrowserServiceCallbacks * callbacks,
            /* [in] */ Int32 uid);

        CAR_INTERFACE_DECL()

        CARAPI Run();

    private:
        AutoPtr<MediaBrowserService> mMediaBrowserService;
        AutoPtr<IIMediaBrowserServiceCallbacks> mCallbacks;
        String mPkg;
        AutoPtr<IBundle> mRootHints;
        Int32 mUid;
    };

    class DisconnectRunnable
        : public Object
        , public IRunnable
    {
    public:
        DisconnectRunnable(
            /* [in] */ MediaBrowserService *mediaBrowserService,
            /* [in] */ IIMediaBrowserServiceCallbacks * callbacks);

        CAR_INTERFACE_DECL()

        CARAPI Run();

    private:
        AutoPtr<MediaBrowserService> mMediaBrowserService;
        AutoPtr<IIMediaBrowserServiceCallbacks> mCallbacks;
    };

    class AddSubscriptionRunnable
        : public Object
        , public IRunnable
    {
    public:
        AddSubscriptionRunnable(
            /* [in] */ MediaBrowserService *mediaBrowserService,
            /* [in] */ const String& id,
            /* [in] */ IIMediaBrowserServiceCallbacks * callbacks);

        CAR_INTERFACE_DECL()

        CARAPI Run();

    private:
        AutoPtr<MediaBrowserService> mMediaBrowserService;
        AutoPtr<IIMediaBrowserServiceCallbacks> mCallbacks;
        String mId;
    };

    class RemoveSubscriptionRunnable
        : public Object
        , public IRunnable
    {
    public:
        RemoveSubscriptionRunnable(
            /* [in] */ MediaBrowserService *mediaBrowserService,
            /* [in] */ const String& id,
            /* [in] */ IIMediaBrowserServiceCallbacks * callbacks);

        CAR_INTERFACE_DECL()

        CARAPI Run();

    private:
        AutoPtr<MediaBrowserService> mMediaBrowserService;
        AutoPtr<IIMediaBrowserServiceCallbacks> mCallbacks;
        String mId;
    };

    class NotifyChildrenChangedRunnable
        : public Object
        , public IRunnable
    {
    public:
        NotifyChildrenChangedRunnable(
            /* [in] */ MediaBrowserService *mediaBrowserService,
            /* [in] */ const String& parentId);

        CAR_INTERFACE_DECL()

        CARAPI Run();

    private:
        AutoPtr<MediaBrowserService> mMediaBrowserService;
        String mParentId;
    };

    class ServiceBinder
        : public Object
        , public IIMediaBrowserService
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        ServiceBinder(
            /* [in] */ MediaBrowserService *mediaBrowserService);

        CARAPI Connect(
            /* [in] */ const String& pkg,
            /* [in] */ IBundle * rootHints,
            /* [in] */ IIMediaBrowserServiceCallbacks * cb);

        CARAPI Disconnect(
            /* [in] */ IIMediaBrowserServiceCallbacks * cb);

        CARAPI AddSubscription(
            /* [in] */ const String& id,
            /* [in] */ IIMediaBrowserServiceCallbacks * cb);

        CARAPI RemoveSubscription(
            /* [in] */ const String& id,
            /* [in] */ IIMediaBrowserServiceCallbacks * cb);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<MediaBrowserService> mMediaBrowserService;
    };

    class PerformLoadChildrenResult
        : public MediaBrowserServiceResult
    {
    public:
        PerformLoadChildrenResult(
            /* [in] */ MediaBrowserService *mediaBrowserService,
            /* [in] */ const String& parentId,
            /* [in] */ ConnectionRecord * connection);

        CARAPI OnResultSent(
            /* [in] */ IInterface * result);

    private:
        AutoPtr<MediaBrowserService> mMediaBrowserService;
        String mParentId;
        AutoPtr<ConnectionRecord> mConnection;
    };

public:
    CARAPI OnCreate();

    CARAPI OnBind(
        /* [in] */ IIntent * intent,
        /* [out] */ IBinder ** result);

    CARAPI Dump(
        /* [in] */ IFileDescriptor * fd,
        /* [in] */ IPrintWriter * writer,
        /* [in] */ ArrayOf<String> * args);

    /**
     * Called to get the root information for browsing by a particular client.
     * <p>
     * The implementation should verify that the client package has
     * permission to access browse media information before returning
     * the root id; it should return null if the client is not
     * allowed to access this information.
     * </p>
     *
     * @param clientPackageName The package name of the application
     * which is requesting access to browse media.
     * @param clientUid The uid of the application which is requesting
     * access to browse media.
     * @param rootHints An optional bundle of service-specific arguments to send
     * to the media browse service when connecting and retrieving the root id
     * for browsing, or null if none.  The contents of this bundle may affect
     * the information returned when browsing.
     */
    CARAPI OnGetRoot(
        /* [in] */ const String& clientPackageName,
        /* [in] */ Int32 clientUid,
        /* [in] */ IBundle * rootHints,
        /* [out] */ IMediaBrowserServiceBrowserRoot ** result);

    /**
     * Called to get information about the children of a media item.
     * <p>
     * Implementations must call result.{@link Result#sendResult result.sendResult} with the list
     * of children. If loading the children will be an expensive operation that should be performed
     * on another thread, result.{@link Result#detach result.detach} may be called before returning
     * from this function, and then {@link Result#sendResult result.sendResult} called when
     * the loading is complete.
     *
     * @param parentId The id of the parent media item whose
     * children are to be queried.
     * @return The list of children, or null if the id is invalid.
     */
    CARAPI OnLoadChildren(
        /* [in] */ const String& parentId,
        /* [in] */ IMediaBrowserServiceResult * result);

    /**
     * Call to set the media session.
     * <p>
     * This must be called before onCreate returns.
     *
     * @return The media session token, must not be null.
     */
    CARAPI SetSessionToken(
        /* [in] */ IMediaSessionToken * token);

    /**
     * Gets the session token, or null if it has not yet been created
     * or if it has been destroyed.
     */
    CARAPI GetSessionToken(
        /* [out] */ IMediaSessionToken ** token);

    /**
     * Notifies all connected media browsers that the children of
     * the specified parent id have changed in some way.
     * This will cause browsers to fetch subscribed content again.
     *
     * @param parentId The id of the parent media item whose
     * children changed.
     */
    CARAPI NotifyChildrenChanged(
        /* [in] */ const String& parentId);

private:
    /**
     * Return whether the given package is one of the ones that is owned by the uid.
     */
    CARAPI_(Boolean) IsValidPackage(
        /* [in] */ const String& pkg,
        /* [in] */ Int32 uid);

    /**
     * Save the subscription and if it is a new subscription send the results.
     */
    CARAPI_(void) AddSubscription(
        /* [in] */ const String& id,
        /* [in] */ ConnectionRecord * connection);

    /**
     * Call onLoadChildren and then send the results back to the connection.
     * <p>
     * Callers must make sure that this connection is still connected.
     */
    CARAPI PerformLoadChildren(
        /* [in] */ const String& parentId,
        /* [in] */ ConnectionRecord * connection);

private:
    static String TAG; // = "MediaBrowserService";
    static Boolean DBG; // = false;
    AutoPtr<IArrayMap> mConnections;
    AutoPtr<IHandler> mHandler;
    AutoPtr<ServiceBinder> mBinder;
    AutoPtr<IMediaSessionToken> mSession;
};

} // namespace Media
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_MEDIA_MEDIABROWSERSERVICE_H__
