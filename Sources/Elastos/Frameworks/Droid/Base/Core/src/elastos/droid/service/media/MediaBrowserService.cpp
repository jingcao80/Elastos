
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/service/media/MediaBrowserService.h"
#include "elastos/droid/service/media/CMediaBrowserServiceResult.h"
#include "elastos/droid/os/CBinderHelper.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IParceledListSlice;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Media {

String MediaBrowserService::TAG("MediaBrowserService");
Boolean MediaBrowserService::DBG = FALSE;

//==========================================================
//       MediaBrowserService::ConnectRunnable
//==========================================================

CAR_INTERFACE_IMPL(MediaBrowserService::ConnectRunnable, Object, IRunnable)

MediaBrowserService::ConnectRunnable::ConnectRunnable(
    /* [in] */ MediaBrowserService* mediaBrowserService,
    /* [in] */ const String& pkg,
    /* [in] */ IBundle * rootHints,
    /* [in] */ IIMediaBrowserServiceCallbacks * callbacks,
    /* [in] */ Int32 uid)
{
    mMediaBrowserService = mediaBrowserService;
    mPkg = pkg;
    mRootHints = rootHints;
    mUid = uid;
}

ECode MediaBrowserService::ConnectRunnable::Run()
{
    AutoPtr<IBinder> b = IBinder::Probe(mCallbacks);

    // Clear out the old subscriptions.  We are getting new ones.
    (IMap::Probe(mMediaBrowserService->mConnections))->Remove(b);

    AutoPtr<ConnectionRecord> connection = new ConnectionRecord();
    connection->pkg = mPkg;
    connection->rootHints = mRootHints;
    connection->callbacks = mCallbacks;

    mMediaBrowserService->OnGetRoot(mPkg, mUid, mRootHints, (IMediaBrowserServiceBrowserRoot**)&connection->root);

    // If they didn't return something, don't allow this client.
    if (connection->root == NULL) {
        Logger::I(TAG, String("No root for client ") + mPkg + " from service "
                + /*getClass().getName()*/ "MediaBrowserService");
        // try {
            mCallbacks->OnConnectFailed();
        // } catch (RemoteException ex) {
        //     Log.w(TAG, "Calling onConnectFailed() failed. Ignoring. "
        //             + "pkg=" + pkg);
        // }
    } else {
        // try {
            mMediaBrowserService->mConnections->Append(b, connection->Probe(EIID_IInterface));
            AutoPtr<IBundle> bundle;
            connection->root->GetExtras((IBundle**)&bundle);
            String id;
            connection->root->GetRootId(&id);
            mCallbacks->OnConnect(id, mMediaBrowserService->mSession, bundle);
        // } catch (RemoteException ex) {
        //     Log.w(TAG, "Calling onConnect() failed. Dropping client. "
        //             + "pkg=" + pkg);
        //     mConnections.remove(b);
        // }
    }
    return NOERROR;
}

//==========================================================
//       MediaBrowserService::DisconnectRunnable
//==========================================================

CAR_INTERFACE_IMPL(MediaBrowserService::DisconnectRunnable, Object, IRunnable)

MediaBrowserService::DisconnectRunnable::DisconnectRunnable(
    /* [in] */ MediaBrowserService* mediaBrowserService,
    /* [in] */ IIMediaBrowserServiceCallbacks * callbacks)
{
    mMediaBrowserService = mediaBrowserService;
    mCallbacks = callbacks;
}

ECode MediaBrowserService::DisconnectRunnable::Run()
{
    AutoPtr<IBinder> b = IBinder::Probe(mCallbacks);

    // Clear out the old subscriptions.  We are getting new ones.
    AutoPtr<ConnectionRecord> old;
    IMap::Probe(mMediaBrowserService->mConnections)->Remove(b, (IInterface**)&old);
    if (old != NULL) {
        // TODO
    }
    return NOERROR;
}

//==========================================================
//       MediaBrowserService::AddSubscriptionRunnable
//==========================================================

CAR_INTERFACE_IMPL(MediaBrowserService::AddSubscriptionRunnable, Object, IRunnable)

MediaBrowserService::AddSubscriptionRunnable::AddSubscriptionRunnable(
    /* [in] */ MediaBrowserService* mediaBrowserService,
    /* [in] */ const String& id,
    /* [in] */ IIMediaBrowserServiceCallbacks * callbacks)
{
    mMediaBrowserService = mediaBrowserService;
    mCallbacks = callbacks;
    mId = id;
}

ECode MediaBrowserService::AddSubscriptionRunnable::Run()
{
    AutoPtr<IBinder> b = IBinder::Probe(mCallbacks);

    // Get the record for the connection
    Int32 index;
    mMediaBrowserService->mConnections->GetIndexOfKey(b, &index);
    AutoPtr<ConnectionRecord> connection;
    mMediaBrowserService->mConnections->GetValueAt(index, (IInterface**)&connection);
    if (connection == NULL) {
        Logger::W(TAG, String("addSubscription for callback that isn't registered id=")
            + mId);
        return NOERROR;
    }

    mMediaBrowserService->AddSubscription(mId, connection);
    return NOERROR;
}

//==========================================================
//       MediaBrowserService::RemoveSubscriptionRunnable
//==========================================================

CAR_INTERFACE_IMPL(MediaBrowserService::RemoveSubscriptionRunnable, Object, IRunnable)

MediaBrowserService::RemoveSubscriptionRunnable::RemoveSubscriptionRunnable(
    /* [in] */ MediaBrowserService* mediaBrowserService,
    /* [in] */ const String& id,
    /* [in] */ IIMediaBrowserServiceCallbacks * callbacks)
{
    mMediaBrowserService = mediaBrowserService;
    mCallbacks = callbacks;
    mId = id;
}

ECode MediaBrowserService::RemoveSubscriptionRunnable::Run()
{
    AutoPtr<IBinder> b = IBinder::Probe(mCallbacks);

    Int32 index;
    mMediaBrowserService->mConnections->GetIndexOfKey(b, &index);
    AutoPtr<ConnectionRecord> connection;
    mMediaBrowserService->mConnections->GetValueAt(index, (IInterface**)&connection);
    if (connection == NULL) {
        Logger::W(TAG, String("removeSubscription for callback that isn't registered id=")
                + mId);
        return NOERROR;
    }
    Boolean result;
    AutoPtr<ICharSequence> csq;
    CString::New(mId, (ICharSequence**)&csq);
    connection->subscriptions->Remove(csq, &result);
    if (!result) {
        Logger::W(TAG, String("removeSubscription called for ") + mId
                + " which is not subscribed");
    }
    return NOERROR;
}

//==========================================================
//       MediaBrowserService::NotifyChildrenChangedRunnable
//==========================================================

CAR_INTERFACE_IMPL(MediaBrowserService::NotifyChildrenChangedRunnable, Object, IRunnable)

MediaBrowserService::NotifyChildrenChangedRunnable::NotifyChildrenChangedRunnable(
    /* [in] */ MediaBrowserService* mediaBrowserService,
    /* [in] */ const String& parentId)
{
    mMediaBrowserService = mediaBrowserService;
    mParentId = parentId;
}

ECode MediaBrowserService::NotifyChildrenChangedRunnable::Run()
{
    AutoPtr<ISet> sets;
    IMap::Probe(mMediaBrowserService->mConnections)->GetKeySet((ISet**)&sets);
    AutoPtr<ArrayOf<IInterface*> > array;
    sets->ToArray((ArrayOf<IInterface*>**)&array);
    for(Int32 i = 0; i < array->GetLength(); i++){
        AutoPtr<IBinder> binder = IBinder::Probe((*array)[i]);
        Int32 index;
        mMediaBrowserService->mConnections->GetIndexOfKey(binder, &index);
        AutoPtr<ConnectionRecord> connection;
        mMediaBrowserService->mConnections->GetValueAt(index, (IInterface**)&connection);

        Boolean result;
        AutoPtr<ICharSequence> csq;
        CString::New(mParentId, (ICharSequence**)&csq);
        connection->subscriptions->Contains(csq, &result);
        if (result) {
            mMediaBrowserService->PerformLoadChildren(mParentId, connection);
        }
    }
    return NOERROR;
}

//==========================================================
//       MediaBrowserService::PerformLoadChildrenResult
//==========================================================

MediaBrowserService::PerformLoadChildrenResult::PerformLoadChildrenResult(
    /* [in] */ MediaBrowserService* mediaBrowserService,
    /* [in] */ const String& parentId,
    /* [in] */ ConnectionRecord * connection)
{
    mMediaBrowserService = mediaBrowserService;
    mParentId = parentId;
    mConnection = connection;
}

ECode MediaBrowserService::PerformLoadChildrenResult::OnResultSent(
    /* [in] */ IInterface * list)
{
    if (list == NULL) {
        // throw new IllegalStateException("onLoadChildren sent null list for id "
        //         + mParentId);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Int32 index;
    mMediaBrowserService->mConnections->GetIndexOfKey(IBinder::Probe(mConnection->callbacks), &index);
    AutoPtr<ConnectionRecord> connection;
    mMediaBrowserService->mConnections->GetValueAt(index, (IInterface**)&connection);

    if (connection != mConnection) {
        if (mMediaBrowserService->DBG) {
            Logger::D(TAG, String("Not sending onLoadChildren result for connection that has")
                    + " been disconnected. pkg=" + mConnection->pkg + " id=" + mParentId);
        }
        return NOERROR;
    }

    AutoPtr<IParceledListSlice> pls;
// TODO: Need CParceledListSlice::New(list)
    // ParceledListSlice<IMediaBrowserMediaItem> pls = new ParceledListSlice(list);
    // try {
        mConnection->callbacks->OnLoadChildren(mParentId, pls);
    // } catch (RemoteException ex) {
    //     // The other side is in the process of crashing.
    //     Log.w(TAG, "Calling onLoadChildren() failed for id=" + mParentId
    //             + " package=" + connection.pkg);
    // }
    return NOERROR;
}

//==========================================================
//       MediaBrowserService
//==========================================================

CAR_INTERFACE_IMPL_2(MediaBrowserService::ServiceBinder, Object, IIMediaBrowserService, IBinder)

MediaBrowserService::ServiceBinder::ServiceBinder(
    /* [in] */ MediaBrowserService *mediaBrowserService)
{
    mMediaBrowserService = mediaBrowserService;
}

ECode MediaBrowserService::ServiceBinder::Connect(
    /* [in] */ const String& pkg,
    /* [in] */ IBundle * rootHints,
    /* [in] */ IIMediaBrowserServiceCallbacks * cb)
{
    Int32 uid;
    AutoPtr<IBinderHelper> helper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&helper);
    helper->GetCallingUid(&uid);
    if (!mMediaBrowserService->IsValidPackage(pkg, uid)) {
        // throw new IllegalArgumentException("Package/uid mismatch: uid=" + uid
        //         + " package=" + pkg);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean b;
    AutoPtr<ConnectRunnable> run = new ConnectRunnable(mMediaBrowserService, pkg, rootHints, cb, uid);
    return mMediaBrowserService->mHandler->Post(IRunnable::Probe(run), &b);
}

ECode MediaBrowserService::ServiceBinder::Disconnect(
    /* [in] */ IIMediaBrowserServiceCallbacks * cb)
{
    Boolean b;
    AutoPtr<DisconnectRunnable> run = new DisconnectRunnable(mMediaBrowserService, cb);
    return mMediaBrowserService->mHandler->Post(IRunnable::Probe(run), &b);
}

ECode MediaBrowserService::ServiceBinder::AddSubscription(
    /* [in] */ const String& id,
    /* [in] */ IIMediaBrowserServiceCallbacks * cb)
{
    Boolean b;
    AutoPtr<AddSubscriptionRunnable> run = new AddSubscriptionRunnable(mMediaBrowserService, id, cb);
    return mMediaBrowserService->mHandler->Post(IRunnable::Probe(run), &b);
}

ECode MediaBrowserService::ServiceBinder::RemoveSubscription(
    /* [in] */ const String& id,
    /* [in] */ IIMediaBrowserServiceCallbacks * cb)
{
    Boolean b;
    AutoPtr<RemoveSubscriptionRunnable> run = new RemoveSubscriptionRunnable(mMediaBrowserService, id, cb);
    return mMediaBrowserService->mHandler->Post(IRunnable::Probe(run), &b);
}

ECode MediaBrowserService::ServiceBinder::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("ServiceBinder");
    return NOERROR;
}

ECode MediaBrowserService::OnCreate()
{
// TODO: Need app/Service
    // Service::OnCreate();
    mBinder = new ServiceBinder(this);
    return NOERROR;
}

ECode MediaBrowserService::OnBind(
    /* [in] */ IIntent * intent,
    /* [out] */ IBinder ** result)
{
    VALIDATE_NOT_NULL(result)
    String action;
    intent->GetAction(&action);
    if (action.Equals(IMediaBrowserService::SERVICE_INTERFACE)) {
        *result = IBinder::Probe(mBinder);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    *result = NULL;
    return NOERROR;
}

ECode MediaBrowserService::Dump(
    /* [in] */ IFileDescriptor * fd,
    /* [in] */ IPrintWriter * writer,
    /* [in] */ ArrayOf<String> * args)
{
    return NOERROR;
}

ECode MediaBrowserService::OnGetRoot(
    /* [in] */ const String& clientPackageName,
    /* [in] */ Int32 clientUid,
    /* [in] */ IBundle * rootHints,
    /* [out] */ IMediaBrowserServiceBrowserRoot ** result)
{
    return NOERROR;
}

ECode MediaBrowserService::OnLoadChildren(
    /* [in] */ const String& parentId,
    /* [in] */ IMediaBrowserServiceResult * result)
{
    return NOERROR;
}

ECode MediaBrowserService::SetSessionToken(
    /* [in] */ IMediaSessionToken * token)
{
    if (token == NULL) {
        // throw new IllegalStateException(this.getClass().getName()
        //         + ".onCreateSession() set invalid MediaSession.Token");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mSession = token;
    return NOERROR;
}

ECode MediaBrowserService::GetSessionToken(
    /* [out] */ IMediaSessionToken ** token)
{
    VALIDATE_NOT_NULL(token)
    *token = mSession;
    REFCOUNT_ADD(*token);
    return NOERROR;
}

ECode MediaBrowserService::NotifyChildrenChanged(
    /* [in] */ const String& parentId)
{
    if (parentId == NULL) {
        // throw new IllegalArgumentException("parentId cannot be null in notifyChildrenChanged");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean b;
    AutoPtr<NotifyChildrenChangedRunnable> run = new NotifyChildrenChangedRunnable(this, parentId);
    return mHandler->Post(IRunnable::Probe(run), &b);
}

Boolean MediaBrowserService::IsValidPackage(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid)
{
    if (pkg == NULL) {
        return FALSE;
    }
    AutoPtr<IPackageManager> pm;
// TODO: Need app/Service
    // GetPackageManager((IPackageManager**)&pm);
    AutoPtr<ArrayOf<String> > packages;
    pm->GetPackagesForUid(uid, (ArrayOf<String>**)&packages);
    Int32 N = packages->GetLength();
    for (Int32 i=0; i<N; i++) {
        if ((*packages)[i].Equals(pkg)) {
            return TRUE;
        }
    }
    return FALSE;
}

void MediaBrowserService::AddSubscription(
    /* [in] */ const String& id,
    /* [in] */ ConnectionRecord * connection)
{
    // Save the subscription
    Boolean added;
    AutoPtr<ICharSequence> csq;
    CString::New(id, (ICharSequence**)&csq);
    connection->subscriptions->Add(csq, &added);

    // If this is a new subscription, send the results
    if (added) {
        PerformLoadChildren(id, connection);
    }
}

ECode MediaBrowserService::PerformLoadChildren(
    /* [in] */ const String& parentId,
    /* [in] */ ConnectionRecord * connection)
{
    // Result<List<MediaBrowser.MediaItem>> result
    //         = new Result<List<MediaBrowser.MediaItem>>(parentId) {
    //     @Override
    //     void onResultSent(List<MediaBrowser.MediaItem> list) {
    //         if (list == null) {
    //             throw new IllegalStateException("onLoadChildren sent null list for id "
    //                     + parentId);
    //         }
    //         if (mConnections.get(connection.callbacks.asBinder()) != connection) {
    //             if (DBG) {
    //                 Log.d(TAG, "Not sending onLoadChildren result for connection that has"
    //                         + " been disconnected. pkg=" + connection.pkg + " id=" + parentId);
    //             }
    //             return;
    //         }

    //         ParceledListSlice<MediaBrowser.MediaItem> pls = new ParceledListSlice(list);
    //         try {
    //             connection.callbacks.onLoadChildren(parentId, pls);
    //         } catch (RemoteException ex) {
    //             // The other side is in the process of crashing.
    //             Log.w(TAG, "Calling onLoadChildren() failed for id=" + parentId
    //                     + " package=" + connection.pkg);
    //         }
    //     }
    // };

    AutoPtr<PerformLoadChildrenResult> result = new PerformLoadChildrenResult(this, parentId, connection);

    OnLoadChildren(parentId, IMediaBrowserServiceResult::Probe(result));

    if (!((CMediaBrowserServiceResult*)IMediaBrowserServiceResult::Probe(result))->IsDone()) {
        // throw new IllegalStateException("onLoadChildren must call detach() or sendResult()"
        //         + " before returning for package=" + connection.pkg + " id=" + parentId);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}


} // namespace Media
} // namespace Service
} // namepsace Droid
} // namespace Elastos
