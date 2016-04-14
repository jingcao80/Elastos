
#include "elastos/droid/net/http/RequestQueue.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/net/CWebAddress.h"
#include "elastos/droid/net/Network.h"
#include "elastos/droid/net/NetworkInfo.h"
#include "elastos/droid/net/Proxy.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include "elastos/droid/net/WebAddress.h"
#include "elastos/droid/net/http/CLoggingEventHandler.h"
#include "elastos/droid/net/http/CRequestHandle.h"
#include "elastos/droid/net/http/Connection.h"
#include "elastos/droid/net/http/ConnectionThread.h"
#include "elastos/droid/net/http/HttpConnection.h"
#include "elastos/droid/net/http/HttpLog.h"
#include "elastos/droid/net/http/HttpsConnection.h"
#include "elastos/droid/net/http/Request.h"
#include "elastos/droid/net/http/RequestHandle.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Net::CWebAddress;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::IWebAddress;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Utility::ILog;

using Elastos::Core::CObject;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::Thread;
using Elastos::IO::IInputStream;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Utility::CLinkedHashMap;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::ILinkedHashMap;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::IList;
using Elastos::Utility::IListIterator;
using Elastos::Utility::IMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

using Org::Apache::Http::CHttpHost;
using Org::Apache::Http::IHttpHost;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

//============================================================
// RequestQueue::ActivePool
//============================================================
CAR_INTERFACE_IMPL(RequestQueue::ActivePool, Object, IRequestQueueConnectionManager)

RequestQueue::ActivePool::ActivePool(
    /* [in] */ Int32 connectionCount,
    /* [in] */ RequestQueue* host)
    : mTotalRequest(0)
    , mTotalConnection(0)
    , mConnectionCount(connectionCount)
    , mHost(host)
{
    mIdleCache = new IdleCache();
    mThreads = ArrayOf<ConnectionThread*>::Alloc(mConnectionCount);
    for (Int32 i = 0; i < mConnectionCount; i++) {
        (*mThreads)[i] = new ConnectionThread();
        (*mThreads)[i]->constructor(mHost->mContext, i, this, mHost);
    }
}

ECode RequestQueue::ActivePool::Startup()
{
    for (Int32 i = 0; i < mConnectionCount; i++) {
        ((*mThreads)[i])->Start();
    }

    return NOERROR;
}

ECode RequestQueue::ActivePool::Shutdown()
{
    for (Int32 i = 0; i < mConnectionCount; i++) {
        ((*mThreads)[i])->RequestStop();
    }

    return NOERROR;
}

ECode RequestQueue::ActivePool::StartConnectionThread()
{
    synchronized(mHost) {
        mHost->Notify();
    }
    return NOERROR;
}

ECode RequestQueue::ActivePool::StartTiming()
{
    for (Int32 i = 0; i < mConnectionCount; i++) {
        ConnectionThread* rt = (*mThreads)[i];
        rt->mCurrentThreadTime = -1;
        rt->mTotalThreadTime = 0;
    }
    mTotalRequest = 0;
    mTotalConnection = 0;

    return NOERROR;
}

ECode RequestQueue::ActivePool::StopTiming()
{
    Int32 totalTime = 0;
    for (Int32 i = 0; i < mConnectionCount; i++) {
        ConnectionThread* rt = (*mThreads)[i];
        if (rt->mCurrentThreadTime != -1) {
            totalTime += rt->mTotalThreadTime;
        }
        rt->mCurrentThreadTime = 0;
    }
    Logger::D(String("Http"), String("Http thread used ") + StringUtils::ToString(totalTime) + String(" ms ")
        + String(" for ") + StringUtils::ToString(mTotalRequest) + String(" requests and ")
        + StringUtils::ToString(mTotalConnection) + String(" new connections"));

    return NOERROR;
}

ECode RequestQueue::ActivePool::LogState()
{
    AutoPtr<StringBuilder> dump = new StringBuilder();
    for (Int32 i = 0; i < mConnectionCount; i++) {
        String str;
        ((*mThreads)[i])->ToString(&str);
        dump->Append(str + String("\n"));
    }
    HttpLog::V(dump->ToString());

    return NOERROR;
}

ECode RequestQueue::ActivePool::GetProxyHost(
    /* [out] */ IHttpHost** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mHost->mProxyHost;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode RequestQueue::ActivePool::DisablePersistence()
{
    for (Int32 i = 0; i < mConnectionCount; i++) {
        Connection* connection = (Connection*)(*mThreads)[i]->mConnection.Get();
        if (connection != NULL) {
            connection->SetCanPersist(false);
        }
    }
    mIdleCache->Clear();

    return NOERROR;
}

ECode RequestQueue::ActivePool::GetThread(
    /* [in] */ IHttpHost* host,
    /* [out] */ ConnectionThread** thread)
{
    VALIDATE_NOT_NULL(thread);

    synchronized(mHost) {
        for (Int32 i = 0; i < mThreads->GetLength(); i++) {
            ConnectionThread* ct = (*mThreads)[i];
            Connection* connection = (Connection*)ct->mConnection.Get();
            Boolean isEquals;
            IObject::Probe(connection->mHost)->Equals(host, &isEquals);
            if (connection != NULL && isEquals) {
                *thread = ct;
                REFCOUNT_ADD(*thread);
                return NOERROR;
            }
        }
    }

    *thread = NULL;
    return NOERROR;
}

ECode RequestQueue::ActivePool::GetConnection(
    /* [in] */ IContext* context,
    /* [in] */ IHttpHost* host,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);

    host = mHost->DetermineHost(host);
    AutoPtr<IConnection> tmp;
    mIdleCache->GetConnection(host, (IConnection**)&tmp);
    Connection* con;
    con = (Connection*) tmp.Get();
    if (con == NULL) {
        mTotalConnection++;
        AutoPtr<IConnection> tmp;
        Connection::GetConnection(mHost->mContext, host, mHost->mProxyHost, mHost, (IConnection**)&tmp);
        con = (Connection*) tmp.Get();
    }
    *result = con;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode RequestQueue::ActivePool::RecycleConnection(
    /* [in] */ IConnection* connection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return mIdleCache->CacheConnection(Ptr((Connection*)connection)->Func(((Connection*)connection)->GetHost), connection, result);
}

//===============================================
// RequestQueue::SyncFeeder
//===============================================
CAR_INTERFACE_IMPL(RequestQueue::SyncFeeder, Object, IRequestFeeder)

ECode RequestQueue::SyncFeeder::GetRequest(
    /* [out] */ IRequest** req)
{
    VALIDATE_NOT_NULL(req);

    AutoPtr<IRequest> r = mRequest;
    mRequest = NULL;
    *req = r;
    REFCOUNT_ADD(*req);
    return NOERROR;
}

ECode RequestQueue::SyncFeeder::GetRequest(
    /* [in] */ IHttpHost* host,
    /* [out] */ IRequest** req)
{
    VALIDATE_NOT_NULL(req)

    return GetRequest(req);
}

ECode RequestQueue::SyncFeeder::HaveRequest(
    /* [in] */ IHttpHost* host,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mRequest != NULL;
    return NOERROR;
}

ECode RequestQueue::SyncFeeder::RequeueRequest(
    /* [in] */ IRequest* request)
{
    VALIDATE_NOT_NULL(request)

    mRequest = request;
    return NOERROR;
}

//===============================================
// RequestQueue::InnerSub_BroadcastReceiver
//===============================================
ECode RequestQueue::InnerSub_BroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return mHost->SetProxyConfig();
}

RequestQueue::InnerSub_BroadcastReceiver::InnerSub_BroadcastReceiver(
    /* [in] */ RequestQueue* host)
    : mHost(host)
{}

//===============================================
// RequestQueue
//===============================================
const Int32 RequestQueue::CONNECTION_COUNT = 4;

CAR_INTERFACE_IMPL_2(RequestQueue, Object, IRequestQueue, IRequestFeeder)

ECode RequestQueue::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, CONNECTION_COUNT);
}

ECode RequestQueue::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 connectionCount)
{
    mContext = context;

    CLinkedHashMap::New(32, (ILinkedHashMap**)&mPending);

    mActivePool = new ActivePool(connectionCount, this);
    mActivePool->Startup();

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    mConnectivityManager = IConnectivityManager::Probe(obj);

    return NOERROR;
}

ECode RequestQueue::EnablePlatformNotifications()
{
    synchronized(this) {
        if (HttpLog::LOGV) {
            HttpLog::V(String("RequestQueue.enablePlatformNotifications() network"));
        }

        if (mProxyChangeReceiver == NULL) {
            mProxyChangeReceiver = new InnerSub_BroadcastReceiver(this);
            AutoPtr<IIntentFilter> filter;
            CIntentFilter::New(IProxy::PROXY_CHANGE_ACTION, (IIntentFilter**)&filter);
            AutoPtr<IIntent> intent;
            mContext->RegisterReceiver(mProxyChangeReceiver, filter, (IIntent**)&intent);
        }
        // we need to resample the current proxy setup
        SetProxyConfig();
    }
    return NOERROR;
}

ECode RequestQueue::DisablePlatformNotifications()
{
    synchronized(this){
        if (HttpLog::LOGV) {
            HttpLog::V(String("RequestQueue.disablePlatformNotifications() network"));
        }

        if (mProxyChangeReceiver != NULL) {
            mContext->UnregisterReceiver(mProxyChangeReceiver);
            mProxyChangeReceiver = NULL;
        }
    }
    return NOERROR;
}

ECode RequestQueue::SetProxyConfig()
{
    AutoPtr<INetworkInfo> info;
    mConnectivityManager->GetActiveNetworkInfo((INetworkInfo**)&info);
    if (info != NULL && Ptr(info)->Func(INetworkInfo::GetType) == IConnectivityManager::TYPE_WIFI) {
        mProxyHost = NULL;
    } else {
        String host;
        Proxy::GetHost(mContext, &host);
        if (HttpLog::LOGV) {
            HttpLog::V(String("RequestQueue.setProxyConfig ") + host);
        }
        if (host.IsNullOrEmpty()) {
            mProxyHost = NULL;
        } else {
            mActivePool->DisablePersistence();
            Int32 port;
            Proxy::GetPort(mContext, &port);
            CHttpHost::New(host, port, String("http"), (IHttpHost**)&mProxyHost);
        }
    }
    return NOERROR;
}

ECode RequestQueue::GetProxyHost(
    /* [out] */ IHttpHost** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mProxyHost;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode RequestQueue::QueueRequest(
    /* [in] */ const String& url,
    /* [in] */ const String& method,
    /* [in] */ IMap* headers,
    /* [in] */ IEventHandler* eventHandler,
    /* [in] */ IInputStream* bodyProvider,
    /* [in] */ Int32 bodyLength,
    /* [out] */ IRequestHandle** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IWebAddress> uri;
    CWebAddress::New(url, (IWebAddress**)&uri);
    return QueueRequest(url, uri, method, headers, eventHandler,
        bodyProvider, bodyLength, result);
}

ECode RequestQueue::QueueRequest(
    /* [in] */ const String& url,
    /* [in] */ IWebAddress* uri,
    /* [in] */ const String& method,
    /* [in] */ IMap* headers,
    /* [in] */ IEventHandler* inEventHandler,
    /* [in] */ IInputStream* bodyProvider,
    /* [in] */ Int32 bodyLength,
    /* [out] */ IRequestHandle** result)
{
    VALIDATE_NOT_NULL(result);

    if (HttpLog::LOGV) {
        String suri = Object::ToString(uri);
        HttpLog::V(String("RequestQueue.queueRequest ") + suri);
    }

    // Ensure there is an eventHandler set
    AutoPtr<IEventHandler> eventHandler = inEventHandler;
    if (eventHandler == NULL) {
        CLoggingEventHandler::New((IEventHandler**)&eventHandler);
    }

    /* Create and queue request */
    Request* req;
    String host;
    uri->GetHost(&host);
    Int32 port;
    uri->GetPort(&port);
    String scheme;
    uri->GetScheme(&scheme);
    AutoPtr<IHttpHost> httpHost;
    CHttpHost::New(host, port, scheme, (IHttpHost**)&httpHost);

    // set up request
    String uriPath;
    uri->GetPath(&uriPath);
    req = new Request();
    req->constructor(method, httpHost, mProxyHost, uriPath, bodyProvider,
        bodyLength, eventHandler, headers);

    QueueRequest(req, FALSE);

    mActivePool->mTotalRequest++;

    // dump();
    mActivePool->StartConnectionThread();

    return CRequestHandle::New(this, url, uri, method, headers, bodyProvider,
        bodyLength, req, result);
}

ECode RequestQueue::QueueSynchronousRequest(
    /* [in] */ const String& url,
    /* [in] */ IWebAddress* uri,
    /* [in] */ const String& method,
    /* [in] */ IMap* headers,
    /* [in] */ IEventHandler* eventHandler,
    /* [in] */ IInputStream* bodyProvider,
    /* [in] */ Int32 bodyLength,
    /* [out] */ IRequestHandle** result)
{
    VALIDATE_NOT_NULL(result);

    if (HttpLog::LOGV) {
        String suri = Object::ToString(uri);
        HttpLog::V(String("RequestQueue.dispatchSynchronousRequest ") + suri);
    }

    AutoPtr<IHttpHost> host;
    String shost;
    uri->GetHost(&shost);
    Int32 port;
    uri->GetPort(&port);
    String scheme;
    uri->GetScheme(&scheme);
    CHttpHost::New(shost, port, scheme, (IHttpHost**)&host);

    String uriPath;
    uri->GetPath(&uriPath);
    Request* req = new Request();
    req->constructor(method, host, mProxyHost, uriPath,
        bodyProvider, bodyLength, eventHandler, headers);

    // Open a new connection that uses our special RequestFeeder
    // implementation.
    AutoPtr<IHttpHost> newHost;
    newHost = DetermineHost(host);
    AutoPtr<IConnection> conn;
    Connection::GetConnection(mContext, newHost, mProxyHost,
        new RequestQueue::SyncFeeder(), (IConnection**)&conn);

    // TODO: I would like to process the request here but LoadListener
    // needs a RequestHandle to process some messages.
    return CRequestHandle::New(this, url, uri, method, headers, bodyProvider,
            bodyLength, req, conn, result);
}

AutoPtr<IHttpHost> RequestQueue::DetermineHost(
    /* [in] */ IHttpHost* host)
{
    // There used to be a comment in ConnectionThread about t-mob's proxy
    // being really bad about https. But, HttpsConnection actually looks
    // for a proxy and connects through it anyway. I think that this check
    // is still valid because if a site is https, we will use
    // HttpsConnection rather than HttpConnection if the proxy address is
    // not secure.
    String schemeName;
    host->GetSchemeName(&schemeName);
    if (mProxyHost == NULL || schemeName.Equals("https")) {
        return host;
    }
    return mProxyHost;
}

ECode RequestQueue::RequestsPending(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(this) {
        *result = !Ptr(mPending)->Func(ILinkedHashMap::IsEmpty);
    }
    return NOERROR;
}

ECode RequestQueue::Dump()
{
    synchronized(this) {
        HttpLog::V(String("dump()"));
        StringBuilder dump;
        Int32 count = 0;
        AutoPtr<IIterator> iter;

        // mActivePool.log(dump);

        Boolean isEmpty;
        if (IMap::Probe(mPending)->IsEmpty(&isEmpty), !isEmpty) {
            AutoPtr<ISet> entrySet;
            mPending->GetEntrySet((ISet**)&entrySet);
            entrySet->GetIterator((IIterator**)&iter);
            Boolean hasNext;
            while (iter->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                iter->GetNext((IInterface**)&obj);
                AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
                obj = NULL;
                entry->GetKey((IInterface**)&obj);
                String hostName;
                IHttpHost::Probe(obj)->GetHostName(&hostName);
                StringBuilder line("p");
                line.Append(count++);
                line.Append(" ");
                line.Append(hostName);
                line.Append(" ");
                AutoPtr<ILinkedList> reqList;
                obj = NULL;
                entry->GetValue((IInterface**)&obj);
                AutoPtr<IListIterator> reqIter;
                ILinkedList::Probe(obj)->GetListIterator(0, (IListIterator**)&reqIter);
                while (IIterator::Probe(reqIter)->HasNext(&hasNext), hasNext) {
                    obj = NULL;
                    IIterator::Probe(reqIter)->GetNext((IInterface**)&obj);
                    String s;
                    IObject::Probe(obj)->ToString(&s);
                    line.Append(s + " ");
                }
                dump += line;
                dump.Append("\n");
            }
        }
        String s = dump.ToString();
        HttpLog::V(s);
    }

    return NOERROR;
}

ECode RequestQueue::GetRequest(
    /* [out] */ IRequest** req)
{
    VALIDATE_NOT_NULL(req);

    synchronized(this) {

        AutoPtr<IRequest> ret;

        if (!Ptr(mPending)->Func(mPending->IsEmpty)) {
            ret = RemoveFirst(IHashMap::Probe(mPending));
        }
        if (HttpLog::LOGV) {
            HttpLog::V("RequestQueue.getRequest() => %s", TO_CSTR(ret));
        }
        *req = ret;
        REFCOUNT_ADD(*req)
    }
    return NOERROR;
}

ECode RequestQueue::GetRequest(
    /* [in] */ IHttpHost* host,
    /* [out] */ IRequest** req)
{
    VALIDATE_NOT_NULL(req);

    synchronized(this) {
        AutoPtr<IRequest> ret;

        Boolean isContain;
        mPending->ContainsKey(host, &isContain);
        if (isContain) {
            AutoPtr<IInterface> obj;
            mPending->Get(IInterface::Probe(host), (IInterface**)&obj);
            AutoPtr<ILinkedList> reqList = ILinkedList::Probe(obj);
            obj = NULL;
            reqList->RemoveFirst((IInterface**)&obj);
            ret = IRequest::Probe(obj);
            if (Ptr(reqList)->Func(reqList->IsEmpty)) {
                mPending->Remove(IInterface::Probe(host));
            }
        }
        if (HttpLog::LOGV) {
            String shost = Object::ToString(host);
            String sret = Object::ToString(ret);
            HttpLog::V("RequestQueue.getRequest(%s) => %s",
                    TO_CSTR(host),
                    TO_CSTR(ret));
        }
        *req = ret;
        REFCOUNT_ADD(*req)
    }
    return NOERROR;
}

ECode RequestQueue::HaveRequest(
    /* [in] */ IHttpHost* host,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(this) {
        mPending->ContainsKey(IInterface::Probe(host), result);
    }
    return NOERROR;
}

ECode RequestQueue::RequeueRequest(
    /* [in] */ IRequest* request)
{
    return QueueRequest(request, TRUE);
}

ECode RequestQueue::Shutdown()
{
    return mActivePool->Shutdown();
}

ECode RequestQueue::QueueRequest(
    /* [in] */ IRequest* request,
    /* [in] */ Boolean head)
{
    synchronized(this) {
        AutoPtr<IHttpHost> host = ((Request*)request)->mProxyHost == NULL
            ? ((Request*)request)->mHost : ((Request*)request)->mProxyHost;
        AutoPtr<ILinkedList> reqList;
        Boolean isContain;
        mPending->ContainsKey(host, &isContain);
        if (isContain) {
            AutoPtr<IInterface> obj;
            mPending->Get(host, (IInterface**)&obj);
            reqList = ILinkedList::Probe(obj);
        } else {
            CLinkedList::New((ILinkedList**)&reqList);
            mPending->Put(IInterface::Probe(host), IInterface::Probe(reqList));
        }
        if (head) {
            reqList->AddFirst(IInterface::Probe(request));
        } else {
            reqList->Add(IInterface::Probe(request));
        }
    }
    return NOERROR;
}

ECode RequestQueue::StartTiming()
{
    return mActivePool->StartTiming();
}

ECode RequestQueue::StopTiming()
{
    return mActivePool->StopTiming();
}

AutoPtr<IRequest> RequestQueue::RemoveFirst(
    /* [in] */ IHashMap* requestQueue)
{
    AutoPtr<IRequest> ret;
    AutoPtr<IIterator> iter;
    Ptr(requestQueue)->Func(requestQueue->GetEntrySet)->GetIterator((IIterator**)&iter);
    if (Ptr(iter)->Func(iter->HasNext)) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
        AutoPtr<ILinkedList> reqList = ILinkedList::Probe(Ptr(entry)->Func(entry->GetValue));
        obj = NULL;
        reqList->RemoveFirst((IInterface**)&obj);
        ret = IRequest::Probe(obj);
        if(Ptr(reqList)->Func(reqList->IsEmpty)) {
            requestQueue->Remove(Ptr(entry)->Func(entry->GetKey));
        }
    }
    return ret;
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
