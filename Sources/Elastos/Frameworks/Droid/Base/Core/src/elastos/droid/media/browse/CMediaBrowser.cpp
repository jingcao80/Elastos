
#include "Elastos.Droid.Utility.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/media/browse/CMediaBrowser.h"
#include "elastos/droid/content/CIntent.h"

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::Utility::CCollections;
using Elastos::Utility::ISet;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastos::Utility::ICollections;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Service::Media::IMediaBrowserService;
using Elastos::Droid::Service::Media::EIID_IIMediaBrowserServiceCallbacks;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Browse {

String CMediaBrowser::TAG = String("MediaBrowser");
Boolean CMediaBrowser::DBG = FALSE;

const Int32 CMediaBrowser::CONNECT_STATE_DISCONNECTED = 0;
const Int32 CMediaBrowser::CONNECT_STATE_CONNECTING = 1;
const Int32 CMediaBrowser::CONNECT_STATE_CONNECTED = 2;
const Int32 CMediaBrowser::CONNECT_STATE_SUSPENDED = 3;

CAR_INTERFACE_IMPL(CMediaBrowser::MediaServiceConnection, Object, IServiceConnection)

CMediaBrowser::MediaServiceConnection::MediaServiceConnection(
    /* [in] */ CMediaBrowser * host)
    : mHost(host)
{
}

ECode CMediaBrowser::MediaServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* binder)
{
    if (DBG) {
        // Logger::D(TAG, String("MediaServiceConnection.onServiceConnected name=") + name
        //         + " binder=" + binder);
        mHost->Dump();
    }

    // Make sure we are still the current connection, and that they haven't called
    // disconnect().
    if (!IsCurrent(String("onServiceConnected"))) {
        return NOERROR;
    }

    // Save their binder
    mHost->mServiceBinder = IIMediaBrowserService::Probe(binder);

    // We make a new mServiceCallbacks each time we connect so that we can drop
    // responses from previous connections.
    mHost->mServiceCallbacks = mHost->GetNewServiceCallbacks();
    mHost->mState = CMediaBrowser::CONNECT_STATE_CONNECTING;

    // Call connect, which is async. When we get a response from that we will
    // say that we're connected.
    // try {
        if (DBG) {
            Logger::D(TAG, String("ServiceCallbacks.onConnect..."));
            mHost->Dump();
        }
        String pkg;
        mHost->mContext->GetPackageName(&pkg);
        mHost->mServiceBinder->Connect(pkg, mHost->mRootHints, mHost->mServiceCallbacks);
    // } catch (RemoteException ex) {
    //     // Connect failed, which isn't good. But the auto-reconnect on the service
    //     // will take over and we will come back.  We will also get the
    //     // onServiceDisconnected, which has all the cleanup code.  So let that do it.
    //     Log.w(TAG, "RemoteException during connect for " + mServiceComponent);
    //     if (DBG) {
    //         Logger::D(TAG, String("ServiceCallbacks.onConnect..."));
    //         mHost->Dump();
    //     }
    // }
    return NOERROR;
}

ECode CMediaBrowser::MediaServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    if (DBG) {
        // Log.d(TAG, "MediaServiceConnection.onServiceDisconnected name=" + name
        //         + " this=" + this + " mServiceConnection=" + mServiceConnection);
        mHost->Dump();
    }

    // Make sure we are still the current connection, and that they haven't called
    // disconnect().
    if (!IsCurrent(String("onServiceDisconnected"))) {
        return NOERROR;
    }

    // Clear out what we set in onServiceConnected
    mHost->mServiceBinder = NULL;
    mHost->mServiceCallbacks = NULL;

    // And tell the app that it's suspended.
    mHost->mState = CMediaBrowser::CONNECT_STATE_SUSPENDED;
    mHost->mCallback->OnConnectionSuspended();
    return NOERROR;
}

Boolean CMediaBrowser::MediaServiceConnection::IsCurrent(
    /* [in] */ const String& funcName)
{
    if (mHost->mServiceConnection.Get() != this) {
        if (mHost->mState != CMediaBrowser::CONNECT_STATE_DISCONNECTED) {
            // Check mState, because otherwise this log is noisy.
            // Log.i(TAG, funcName + " for " + mServiceComponent + " with mServiceConnection="
            //         + mServiceConnection + " this=" + this);
        }
        return FALSE;
    }
    return TRUE;
}

CAR_INTERFACE_IMPL(CMediaBrowser::ServiceCallbacks, Object, IIMediaBrowserServiceCallbacks)

CMediaBrowser::ServiceCallbacks::ServiceCallbacks(
    /* [in] */ CMediaBrowser * host)
    : mMediaBrowser(host)
{
}

ECode CMediaBrowser::ServiceCallbacks::OnConnect(
    /* [in] */ const String& root,
    /* [in] */ IMediaSessionToken * session,
    /* [in] */ IBundle * extras)
{
    if (mMediaBrowser != NULL) {
        mMediaBrowser->OnServiceConnected(this, root, session, extras);
    }
    return NOERROR;
}

ECode CMediaBrowser::ServiceCallbacks::OnConnectFailed()
{
    if (mMediaBrowser != NULL) {
        mMediaBrowser->OnConnectionFailed(this);
    }
    return NOERROR;
}

ECode CMediaBrowser::ServiceCallbacks::OnLoadChildren(
    /* [in] */ const String& parentId,
    /* [in] */ IParceledListSlice * list)
{
    if (mMediaBrowser != NULL) {
        mMediaBrowser->OnLoadChildren(this, parentId, list);
    }
    return NOERROR;
}

CMediaBrowser::Subscription::Subscription(
    /* [in] */ const String& id)
{
    mId = id;
}

ECode CMediaBrowser::ConnectRunnable::Run()
{
    // Ensure that nobody else came in or tried to connect again.
    if (mServiceConnection == mHost->mServiceConnection) {
        mHost->ForceCloseConnection();
        mHost->mCallback->OnConnectionFailed();
    }
    return NOERROR;
}

ECode CMediaBrowser::ServiceConnectedRunnable::Run()
{
    // Check to make sure there hasn't been a disconnect or a different
    // ServiceConnection.
    if (!mHost->IsCurrent(mCallback, String("onConnect"))) {
        return NOERROR;
    }
    // Don't allow them to call us twice.
    if (mHost->mState != CMediaBrowser::CONNECT_STATE_CONNECTING) {
        Logger::W(TAG, String("onConnect from service while mState=")
                + mHost->GetStateLabel(mHost->mState) + "... ignoring");
        return NOERROR;
    }
    mHost->mRootId = mRoot;
    mHost->mMediaSessionToken = mSession;
    mHost->mExtras = mExtra;
    mHost->mState = CMediaBrowser::CONNECT_STATE_CONNECTED;

    if (DBG) {
        Logger::D(TAG, String("ServiceCallbacks.onConnect..."));
        mHost->Dump();
    }
    mHost->mCallback->OnConnected();

    // we may receive some subscriptions before we are connected, so re-subscribe
    // everything now
    AutoPtr<ISet> sets;
    IMap::Probe(mHost->mSubscriptions)->GetKeySet((ISet**)&sets);
    AutoPtr<ArrayOf<IInterface*> > array;
    sets->ToArray((ArrayOf<IInterface*>**)&array);
    for(Int32 i = 0; i < array->GetLength(); i++) {
        String id;
        ICharSequence::Probe((*array)[i])->ToString(&id);
        // try {
            mHost->mServiceBinder->AddSubscription(id, mHost->mServiceCallbacks);
        // } catch (RemoteException ex) {
        //     // Process is crashing.  We will disconnect, and upon reconnect we will
        //     // automatically reregister. So nothing to do here.
        //     Log.d(TAG, "addSubscription failed with RemoteException parentId=" + id);
        // }
    }
    return NOERROR;
}

ECode CMediaBrowser::ConnectionFailedRunnable::Run()
{
    // Logger::E(TAG, String("onConnectFailed for ") + mServiceComponent);

    // Check to make sure there hasn't been a disconnect or a different
    // ServiceConnection.
    if (!mHost->IsCurrent(mCallback, String("onConnectFailed"))) {
        return NOERROR;
    }
    // Don't allow them to call us twice.
    if (mHost->mState != CMediaBrowser::CONNECT_STATE_CONNECTING) {
        Logger::W(TAG, String("onConnect from service while mState=")
                + GetStateLabel(mHost->mState) + "... ignoring");
        return NOERROR;
    }

    // Clean up
    mHost->ForceCloseConnection();

    // Tell the app.
    mHost->mCallback->OnConnectionFailed();
    return NOERROR;
}

ECode CMediaBrowser::LoadChildrenRunnable::Run()
{
    // Check that there hasn't been a disconnect or a different
    // ServiceConnection.
    if (!mHost->IsCurrent(mCallback, String("onLoadChildren"))) {
        return NOERROR;
    }

    AutoPtr<IList> data;
    mList->GetList((IList**)&data);
    if (DBG) {
        // Logger::D(TAG, String("onLoadChildren for ") + mServiceComponent + " id=" + parentId);
    }
    if (data == NULL) {
        AutoPtr<ICollections> coll;
        CCollections::AcquireSingleton((ICollections**)&coll);
        coll->GetEmptyList((IList**)&data);
    }

    // Check that the subscription is still subscribed.
    AutoPtr<Subscription> subscription;
    AutoPtr<ICharSequence> csq;
    CString::New(mParentId, (ICharSequence**)&csq);
    Int32 index;
    mHost->mSubscriptions->GetIndexOfKey(csq, &index);
    mHost->mSubscriptions->GetValueAt(index, (IInterface**)&subscription);

    if (subscription == NULL) {
        if (DBG) {
            Logger::D(TAG, String("onLoadChildren for id that isn't subscribed id=")
                    + mParentId);
        }
        return NOERROR;
    }

    // Tell the app.
    subscription->mCallback->OnChildrenLoaded(mParentId, data);
    return NOERROR;
}


CAR_INTERFACE_IMPL(CMediaBrowser, Object, IMediaBrowser)

CAR_OBJECT_IMPL(CMediaBrowser)

CMediaBrowser::CMediaBrowser()
    : mState(CONNECT_STATE_DISCONNECTED)
    , mRootId(String(NULL))
{}

CMediaBrowser::~CMediaBrowser()
{
}

ECode CMediaBrowser::constructor(
    /* [in] */ IContext * context,
    /* [in] */ IComponentName * serviceComponent,
    /* [in] */ IMediaBrowserConnectionCallback * cb,
    /* [in] */ IBundle * rootHints)
{
    if (context == NULL) {
        // throw new IllegalArgumentException("context must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (serviceComponent == NULL) {
        // throw new IllegalArgumentException("service component must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (cb == NULL) {
        // throw new IllegalArgumentException("connection callback must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mContext = context;
    mServiceComponent = serviceComponent;
    mCallback = cb;
    mRootHints = rootHints;

    return NOERROR;
}

ECode CMediaBrowser::Connect()
{
    if (mState != CMediaBrowser::CONNECT_STATE_DISCONNECTED) {
        // throw new IllegalStateException("connect() called while not disconnected (state="
        //         + getStateLabel(mState) + ")");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    // TODO: remove this extra check.
    if (DBG) {
        if (mServiceConnection != NULL) {
            // throw new RuntimeException("mServiceConnection should be null. Instead it is "
            //         + mServiceConnection);
            return E_RUNTIME_EXCEPTION;
        }
    }
    if (mServiceBinder != NULL) {
        // throw new RuntimeException("mServiceBinder should be null. Instead it is "
        //         + mServiceBinder);
        return E_RUNTIME_EXCEPTION;
    }
    if (mServiceCallbacks != NULL) {
        // throw new RuntimeException("mServiceCallbacks should be null. Instead it is "
        //         + mServiceCallbacks);
        return E_RUNTIME_EXCEPTION;
    }

    mState = CMediaBrowser::CONNECT_STATE_CONNECTING;

    AutoPtr<IIntent> intent;
    CIntent::New(IMediaBrowserService::SERVICE_INTERFACE, (IIntent**)&intent);
    intent->SetComponent(mServiceComponent);

    AutoPtr<IServiceConnection> thisConnection = mServiceConnection = new MediaServiceConnection(this);

    Boolean bound = FALSE;
    // try {
        mContext->BindService(intent, mServiceConnection, IContext::BIND_AUTO_CREATE, &bound);
    // } catch (Exception ex) {
    //     Log.e(TAG, "Failed binding to service " + mServiceComponent);
    // }

    if (!bound) {
        // Tell them that it didn't work.  We are already on the main thread,
        // but we don't want to do callbacks inside of connect().  So post it,
        // and then check that we are on the same ServiceConnection.  We know
        // we won't also get an onServiceConnected or onServiceDisconnected,
        // so we won't be doing double callbacks.
        AutoPtr<ConnectRunnable> runnable = new ConnectRunnable(this, thisConnection);
        Boolean b;
        mHandler->Post(runnable, &b);
    }

    if (DBG) {
        Logger::D(TAG, String("connect..."));
        Dump();
    }
    return NOERROR;
}

ECode CMediaBrowser::Disconnect()
{
    // It's ok to call this any state, because allowing this lets apps not have
    // to check isConnected() unnecessarily.  They won't appreciate the extra
    // assertions for this.  We do everything we can here to go back to a sane state.
    if (mServiceCallbacks != NULL) {
        // try {
            mServiceBinder->Disconnect(mServiceCallbacks);
        // } catch (RemoteException ex) {
        //     // We are disconnecting anyway.  Log, just for posterity but it's not
        //     // a big problem.
        //     Log.w(TAG, "RemoteException during connect for " + mServiceComponent);
        // }
    }
    ForceCloseConnection();

    if (DBG) {
        Logger::D(TAG, String("disconnect..."));
        Dump();
    }
    return NOERROR;
}

ECode CMediaBrowser::IsConnected(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mState == CMediaBrowser::CONNECT_STATE_CONNECTED);
    return NOERROR;
}

ECode CMediaBrowser::GetServiceComponent(
    /* [out] */ IComponentName ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    Boolean b;
    IsConnected(&b);
    if(!b) {
        // throw new IllegalStateException("getServiceComponent() called while not connected" +
        //         " (state=" + mState + ")");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *result = mServiceComponent;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaBrowser::GetRoot(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b;
    IsConnected(&b);
    if(!b) {
        // throw new IllegalStateException("getSessionToken() called while not connected (state="
        //         + getStateLabel(mState) + ")");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *result = mRootId;
    return NOERROR;
}

ECode CMediaBrowser::GetExtras(
    /* [out] */ IBundle ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    Boolean b;
    IsConnected(&b);
    if(!b) {
        // throw new IllegalStateException("getExtras() called while not connected (state="
        //         + getStateLabel(mState) + ")");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *result = mExtras;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

 ECode CMediaBrowser::GetSessionToken(
    /* [out] */ IMediaSessionToken ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    Boolean b;
    IsConnected(&b);
    if(!b) {
        // throw new IllegalStateException("getSessionToken() called while not connected (state="
        //         + mState + ")");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *result = mMediaSessionToken;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaBrowser::Subscribe(
    /* [in] */ const String& parentId,
    /* [in] */ IMediaBrowserSubscriptionCallback * cb)
{
    // Check arguments.
    if (parentId == NULL) {
        // throw new IllegalArgumentException("parentId is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (cb == NULL) {
        // throw new IllegalArgumentException("callback is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Update or create the subscription.
    AutoPtr<Subscription> sub;
    AutoPtr<ICharSequence> csq;
    CString::New(parentId, (ICharSequence**)&csq);
    Int32 index;
    mSubscriptions->GetIndexOfKey(csq, &index);
    mSubscriptions->GetValueAt(index, (IInterface**)&sub);
    Boolean newSubscription = sub == NULL;
    if (newSubscription) {
        sub = new Subscription(parentId);
        mSubscriptions->Append(csq, (IInterface*)(IObject*)sub);
    }
    sub->mCallback = cb;

    // If we are connected, tell the service that we are watching.  If we aren't
    // connected, the service will be told when we connect.
    if (mState == CMediaBrowser::CONNECT_STATE_CONNECTED && newSubscription) {
        // try {
            mServiceBinder->AddSubscription(parentId, mServiceCallbacks);
        // } catch (RemoteException ex) {
        //     // Process is crashing.  We will disconnect, and upon reconnect we will
        //     // automatically reregister. So nothing to do here.
        //     Log.d(TAG, "addSubscription failed with RemoteException parentId=" + parentId);
        // }
    }
    return NOERROR;
}

ECode CMediaBrowser::Unsubscribe(
    /* [in] */ const String& parentId)
{
    // Check arguments.
    if (parentId == NULL) {
        // throw new IllegalArgumentException("parentId is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Remove from our list.
    AutoPtr<Subscription> sub;
    AutoPtr<ICharSequence> csq;
    CString::New(parentId, (ICharSequence**)&csq);
    Int32 index;
    mSubscriptions->GetIndexOfKey(csq, &index);
    mSubscriptions->RemoveAt(index, (IInterface**)&sub);

    // Tell the service if necessary.
    if (mState == CMediaBrowser::CONNECT_STATE_CONNECTED && sub != NULL) {
        // try {
            mServiceBinder->RemoveSubscription(parentId, mServiceCallbacks);
        // } catch (RemoteException ex) {
        //     // Process is crashing.  We will disconnect, and upon reconnect we will
        //     // automatically reregister. So nothing to do here.
        //     Log.d(TAG, "removeSubscription failed with RemoteException parentId=" + parentId);
        // }
    }
    return NOERROR;
}


void CMediaBrowser::ForceCloseConnection()
{
    if (mServiceConnection != NULL) {
        mContext->UnbindService(mServiceConnection);
    }
    mState = CMediaBrowser::CONNECT_STATE_DISCONNECTED;
    mServiceConnection = NULL;
    mServiceBinder = NULL;
    mServiceCallbacks = NULL;
    mRootId = NULL;
    mMediaSessionToken = NULL;
}

String CMediaBrowser::GetStateLabel(
    /* [in] */ Int32 state)
{
    switch (state) {
        case CMediaBrowser::CONNECT_STATE_DISCONNECTED:
            return String("CONNECT_STATE_DISCONNECTED");
        case CMediaBrowser::CONNECT_STATE_CONNECTING:
            return String("CONNECT_STATE_CONNECTING");
        case CMediaBrowser::CONNECT_STATE_CONNECTED:
            return String("CONNECT_STATE_CONNECTED");
        case CMediaBrowser::CONNECT_STATE_SUSPENDED:
            return String("CONNECT_STATE_SUSPENDED");
        default:
            return String("UNKNOWN/") + StringUtils::ToString(state);
    }
}

void CMediaBrowser::OnServiceConnected(
    /* [in] */ IIMediaBrowserServiceCallbacks * callback,
    /* [in] */ const String& root,
    /* [in] */ IMediaSessionToken * session,
    /* [in] */ IBundle * extra)
{
    Boolean b;
    AutoPtr<ServiceConnectedRunnable> runnable = new ServiceConnectedRunnable(this, callback, root, session, extra);
    mHandler->Post(runnable, &b);
}

void CMediaBrowser::OnConnectionFailed(
    /* [in] */ IIMediaBrowserServiceCallbacks * callback)
{
    Boolean b;
    AutoPtr<ConnectionFailedRunnable> runnable = new ConnectionFailedRunnable(this, callback);
    mHandler->Post(runnable, &b);
}

void CMediaBrowser::OnLoadChildren(
    /* [in] */ IIMediaBrowserServiceCallbacks * callback,
    /* [in] */ const String& parentId,
    /* [in] */ IParceledListSlice * list)
{
    Boolean b;
    AutoPtr<LoadChildrenRunnable> runnable = new LoadChildrenRunnable(this, callback, parentId, list);
    mHandler->Post(runnable, &b);
}

Boolean CMediaBrowser::IsCurrent(
    /* [in] */ IIMediaBrowserServiceCallbacks * callback,
    /* [in] */ const String& funcName)
{
    if (mServiceCallbacks.Get() != callback) {
        if (mState != CMediaBrowser::CONNECT_STATE_DISCONNECTED) {
            // Logger::I(TAG, funcName + " for " + mServiceComponent + " with mServiceConnection="
            //         + mServiceCallbacks + " this=" + this);
        }
        return FALSE;
    }
    return TRUE;
}

AutoPtr<CMediaBrowser::ServiceCallbacks> CMediaBrowser::GetNewServiceCallbacks()
{
    AutoPtr<ServiceCallbacks> cb = new ServiceCallbacks(this);
    return cb;
}

void CMediaBrowser::Dump()
{
    Logger::D(TAG, "MediaBrowser...");
    // Logger::D(TAG, "  mServiceComponent=" + mServiceComponent);
    // Logger::D(TAG, "  mCallback=" + mCallback);
    // Logger::D(TAG, "  mRootHints=" + mRootHints);
    // Logger::D(TAG, "  mState=" + getStateLabel(mState));
    // Logger::D(TAG, "  mServiceConnection=" + mServiceConnection);
    // Logger::D(TAG, "  mServiceBinder=" + mServiceBinder);
    // Logger::D(TAG, "  mServiceCallbacks=" + mServiceCallbacks);
    // Logger::D(TAG, "  mRootId=" + mRootId);
    // Logger::D(TAG, "  mMediaSessionToken=" + mMediaSessionToken);
}

} // namespace Browse
} // namespace Media
} // namepsace Droid
} // namespace Elastos
