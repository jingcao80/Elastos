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

#include <Elastos.CoreLibrary.Apache.h>
#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.Net.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/net/http/Connection.h"
#include "elastos/droid/net/http/ElastosHttpClient.h"
#include "elastos/droid/net/http/ElastosHttpClientConnection.h"
#include "elastos/droid/net/http/ErrorStrings.h"
#include "elastos/droid/net/http/HttpLog.h"
#include "elastos/droid/net/http/CHttpsConnection.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/etl/List.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::SystemClock;

using Elastos::Core::StringUtils;
using Elastos::Core::Thread;
using Elastos::Utility::Etl::List;

// using Org::Apache::Http::CHttpConnection;
using Org::Apache::Http::CHttpVersionHelper;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::IHttpConnection;
using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::IHttpHost;
using Org::Apache::Http::IHttpVersion;
using Org::Apache::Http::IHttpVersionHelper;
using Org::Apache::Http::IProtocolVersion;
using Org::Apache::Http::Protocol::CBasicHttpContext;
using Org::Apache::Http::Protocol::IExecutionContext;
using Org::Apache::Http::Protocol::IHttpContext;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_INTERFACE_IMPL(Connection, Object, IConnection)

const AutoPtr<ArrayOf<String> > Connection::STATES = InitSTATES();
const Int32 Connection::SOCKET_TIMEOUT = 60000;
const Int32 Connection::SEND = 0;
const Int32 Connection::READ = 1;
const Int32 Connection::DRAIN = 2;
const Int32 Connection::DONE = 3;
Int32 Connection::sSTATE_NORMAL = 0;
Int32 Connection::sSTATE_CANCEL_REQUESTED = 1;
const Int32 Connection::RETRY_REQUEST_LIMIT = 2;
const Int32 Connection::MIN_PIPE = 2;
const Int32 Connection::MAX_PIPE = 3;
const String Connection::HTTP_CONNECTION("http.connection");

Connection::Connection()
    : mCanPersist(FALSE)
    , mActive(sSTATE_NORMAL)
{}

Connection::~Connection()
{}

AutoPtr<ArrayOf<String> > Connection::InitSTATES()
{
    AutoPtr<ArrayOf<String> > sArray = ArrayOf<String>::Alloc(4);

    (*sArray)[0] = String("SEND");
    (*sArray)[1] = String("READ");
    (*sArray)[2] = String("DRAIN");
    (*sArray)[3] = String("DONE");

    return sArray;
}

ECode Connection::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IHttpHost* host,
    /* [in] */ IRequestFeeder* requestFeeder)
{
    mContext = context;
    mHost = host;
    mRequestFeeder = requestFeeder;
    mCanPersist = FALSE;
    CBasicHttpContext::New(NULL, (IHttpContext**)&mHttpContext);
    return NOERROR;
}

ECode Connection::GetHost(
    /* [out] */ IHttpHost** result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(mHost)
}

ECode Connection::GetConnection(
    /* [in] */ IContext* context,
    /* [in] */ IHttpHost* host,
    /* [in] */ IHttpHost* proxy,
    /* [in] */ IRequestFeeder* requestFeeder,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)

    if (Ptr(host)->Func(host->GetSchemeName).Equals("http")) {
        // TODO: Waiting for CHttpConnection
        assert(0);
        // return CHttpConnection::New(context, host, requestFeeder, result);
        return NOERROR;
    }
    // Otherwise, default to https
    return CHttpsConnection::New(context, host, proxy, requestFeeder, result);
}

ECode Connection::GetCertificate(
    /* [out] */ ISslCertificate** result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(mCertificate);
}

ECode Connection::Cancel()
{
    mActive = sSTATE_CANCEL_REQUESTED;
    CloseConnection();
    if (HttpLog::LOGV)
        HttpLog::V("Connection.cancel(): connection closed %s", TO_CSTR(mHost));
    return NOERROR;
}

ECode Connection::ProcessRequests(
    /* [in] */ IRequest* firstRequest)
{
    AutoPtr<IRequest> req;
    Boolean empty;
    Int32 error = IEventHandler::OK;
    Boolean exception = FALSE;

    // LinkedList<Request> pipe = new LinkedList<Request>();
    List<Request*> pipe;

    Int32 minPipe = MIN_PIPE;
    Int32 maxPipe = MAX_PIPE;
    Int32 state = SEND;

    while (state != DONE) {
        if (HttpLog::LOGV)
            HttpLog::V("%s pipe %d", (*STATES)[state].string(), pipe.GetSize());

        /* If a request was cancelled, give other cancel requests
           some time to go through so we don't uselessly restart
           connections */
        if (mActive == sSTATE_CANCEL_REQUESTED) {
            Thread::Sleep(100);
            mActive = sSTATE_NORMAL;
        }

        switch (state) {
            case SEND: {
                if (pipe.GetSize() == maxPipe) {
                    state = READ;
                    break;
                }
                /* get a request */
                if (firstRequest == NULL) {
                    mRequestFeeder->GetRequest(mHost, (IRequest**)&req);
                } else {
                    req = firstRequest;
                    firstRequest = NULL;
                }
                if (req == NULL) {
                    state = DRAIN;
                    break;
                }
                ((Request*)req.Get())->SetConnection(this);

                /* Don't work on cancelled requests. */
                if (((Request*)req.Get())->mCancelled) {
                    if (HttpLog::LOGV)
                        HttpLog::V("processRequests(): skipping cancelled request %s", TO_CSTR(req));
                    ((Request*)req.Get())->Complete();
                    break;
                }

                Boolean lamp = FALSE;
                if (mHttpClientConnection == NULL) lamp = TRUE;
                else if (!Ptr(mHttpClientConnection)->Func(mHttpClientConnection->IsOpen)){
                    lamp = TRUE;
                }
                if (lamp) {
                    /* If this call fails, the address is bad or
                       the net is down.  Punt for now.

                       FIXME: blow out entire queue here on
                       connection failure if net up? */

                    if (!OpenHttpConnection(req)) {
                        state = DONE;
                        break;
                    }
                }

                /* we have a connection, let the event handler
                 * know of any associated certificate,
                 * potentially none.
                 */
                ((Request*)req.Get())->mEventHandler->Certificate(mCertificate);


                /* FIXME: don't increment failure count if old
                   connection?  There should not be a penalty for
                   attempting to reuse an old connection */
                // TODO:
                ECode ec = ((Request*)req.Get())->SendRequest(mHttpClientConnection);
                if (ec == (ECode)E_HTTP_EXCEPTION) {
                    error = IEventHandler::ERROR;
                    exception = TRUE;
                }
                else if (ec == (ECode)E_IO_EXCEPTION){
                    error = IEventHandler::ERROR_IO;
                    exception = TRUE;
                }
                else if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
                    error = IEventHandler::ERROR_IO;
                    exception = TRUE;
                }

                if (exception) {
                    if (HttpFailure(req, error, exception) &&
                        !((Request*)req.Get())->mCancelled) {
                        /* retry request if not permanent failure
                           or cancelled */
                        pipe.PushBack((Request*)req.Get());
                    }
                    exception = FALSE;
                    state = ClearPipe(pipe) ? DONE : SEND;
                    minPipe = maxPipe = 1;
                    break;
                }

                pipe.PushBack((Request*)req.Get());
                if (!mCanPersist) state = READ;
                break;

            }
            case DRAIN:
            case READ: {
                mRequestFeeder->HaveRequest(mHost, &empty);
                empty = !empty;
                Int32 pipeSize = pipe.GetSize();
                if (state != DRAIN && pipeSize < minPipe &&
                    !empty && mCanPersist) {
                    state = SEND;
                    break;
                }
                else if (pipeSize == 0) {
                    /* Done if no other work to do */
                    state = empty ? DONE : SEND;
                    break;
                }

                req = (IRequest*)pipe.GetFront();
                pipe.PopFront();
                if (HttpLog::LOGV) HttpLog::V("processRequests() reading %s", TO_CSTR(req));

                ECode ec = ((Request*)req.Get())->ReadResponse(mHttpClientConnection);
                if (ec == (ECode)E_PARSE_EXCEPTION) {
                    error = IEventHandler::ERROR_IO;
                    exception = TRUE;
                }
                else if (ec == (ECode)E_IO_EXCEPTION){
                    error = IEventHandler::ERROR_IO;
                    exception = TRUE;
                }
                else if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
                    error = IEventHandler::ERROR_IO;
                    exception = TRUE;
                }

                if (exception) {
                    if (HttpFailure(req, error, exception) &&
                        !((Request*)req.Get())->mCancelled) {
                        /* retry request if not permanent failure
                           or cancelled */
                        ((Request*)req.Get())->Reset();
                        pipe.PushBack((Request*)req.Get());
                    }
                    exception = FALSE;
                    mCanPersist = FALSE;
                }
                if (!mCanPersist) {
                    if (HttpLog::LOGV)
                        HttpLog::V("processRequests(): no persist, closing %s", TO_CSTR(mHost));

                    CloseConnection();

                    AutoPtr<IInterface> tmpObj;
                    mHttpContext->RemoveAttribute(HTTP_CONNECTION, (IInterface**)&tmpObj);
                    ClearPipe(pipe);
                    minPipe = maxPipe = 1;
                    state = SEND;
                }
                break;
            }
        }
    }
    return NOERROR;
}

Boolean Connection::ClearPipe(
    /* [in] */ List<Request*>& pipe)
{
    Boolean empty = TRUE;
    if (HttpLog::LOGV) HttpLog::V(
            "Connection.clearPipe(): clearing pipe %d", pipe.GetSize());
    {    AutoLock syncLock(mRequestFeeder);
        Request* tReq;

        List<Request*>::Iterator itor;
        for (itor = pipe.Begin(); itor != pipe.End(); itor++) {
            tReq = *itor;
            if (HttpLog::LOGV) HttpLog::V(
                    "clearPipe() adding back %s %s", TO_CSTR(mHost), Object::ToString((IRequest*)tReq).string());
            mRequestFeeder->RequeueRequest(tReq);
            empty = FALSE;
        }
        if (empty) {
            empty = mRequestFeeder->HaveRequest(mHost, &empty);
            empty = !empty;
        }
    }
    return empty;
}

Boolean Connection::OpenHttpConnection(
    /* [in] */ IRequest* req)
{
    Int64 now = SystemClock::GetUptimeMillis();
    Int32 error = IEventHandler::OK;
    Boolean exception = FALSE;


    // reset the certificate to null before opening a connection
    mCertificate = NULL;
    ECode ec = OpenConnection(req, (IElastosHttpClientConnection**)&mHttpClientConnection);
    if (mHttpClientConnection != NULL) {
        mHttpClientConnection->SetSocketTimeout(SOCKET_TIMEOUT);
        mHttpContext->SetAttribute(HTTP_CONNECTION, mHttpClientConnection);
    } else {
        // we tried to do SSL tunneling, failed,
        // and need to drop the request;
        // we have already informed the handler
        ((Request*)req)->mFailCount = RETRY_REQUEST_LIMIT;
        return FALSE;
    }

    if (ec == (ECode)E_UNKNOWN_HOST_EXCEPTION) {
        if (HttpLog::LOGV) HttpLog::V("Failed to open connection");
        error = IEventHandler::ERROR_LOOKUP;
        exception = TRUE;
    }
    else if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        if (HttpLog::LOGV) HttpLog::V("Illegal argument exception");
        error = IEventHandler::ERROR_CONNECT;
        ((Request*)req)->mFailCount = RETRY_REQUEST_LIMIT;
        exception = TRUE;
    }
    else if (ec == (ECode)E_SSL_CONNECTION_CLOSED_BY_USER_EXCEPTION) {
        // hack: if we have an SSL connection failure,
        // we don't want to reconnect
        ((Request*)req)->mFailCount = RETRY_REQUEST_LIMIT;
        // no error message
        return FALSE;
    }
    else if (ec == (ECode)E_SSL_HANDSHAKE_EXCEPTION) {
        // hack: if we have an SSL connection failure,
        // we don't want to reconnect
        ((Request*)req)->mFailCount = RETRY_REQUEST_LIMIT;
        if (HttpLog::LOGV)
            HttpLog::V("SSL exception performing handshake");
        error = IEventHandler::ERROR_FAILED_SSL_HANDSHAKE;
        exception = TRUE;
    }
    else if (ec == (ECode)E_IO_EXCEPTION) {
        error = IEventHandler::ERROR_CONNECT;
        exception = TRUE;
    }

    if (HttpLog::LOGV) {
        Int64 now2 = SystemClock::GetUptimeMillis();
        HttpLog::V("Connection.openHttpConnection() %d %s", (now2 - now), TO_CSTR(mHost));
    }

    if (error == IEventHandler::OK) {
        return TRUE;
    } else {
        if (((Request*)req)->mFailCount < RETRY_REQUEST_LIMIT) {
            // requeue
            mRequestFeeder->RequeueRequest(req);
            ((Request*)req)->mFailCount++;
        } else {
            HttpFailure(req, error, exception);
        }
        return error == IEventHandler::OK;
    }
}

Boolean Connection::HttpFailure(
    /* [in] */ IRequest* req,
    /* [in] */ Int32 errorId,
    /* [in] */ ECode e)
{
    Boolean ret = TRUE;

    if (HttpLog::LOGV)
        HttpLog::V("httpFailure() ******* %d count %d %s %s", e, ((Request*)req)->mFailCount,
            TO_CSTR(mHost), Ptr(((Request*)req))->Func(((Request*)req)->GetUri).string());

    if (++((Request*)req)->mFailCount >= RETRY_REQUEST_LIMIT) {
        ret = FALSE;
        String error;
        if (errorId < 0) {
            ErrorStrings::GetString(errorId, mContext, &error);
        } else {
            error.AppendFormat("%d", e);
        }
        ((Request*)req)->mEventHandler->Error(errorId, error);
        ((Request*)req)->Complete();
    }

    CloseConnection();
    AutoPtr<IInterface> tmpObj;
    mHttpContext->RemoveAttribute(HTTP_CONNECTION, (IInterface**)&tmpObj);

    return ret;
}

ECode Connection::GetHttpContext(
    /* [out] */ IHttpContext** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mHttpContext;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

Boolean Connection::KeepAlive(
    /* [in] */ IHttpEntity* entity,
    /* [in] */ IProtocolVersion* ver,
    /* [in] */ Int32 connType,
    /* [in] */ IHttpContext* context)
{
    AutoPtr<IInterface> obj;
    context->GetAttribute(IExecutionContext::HTTP_CONNECTION, (IInterface**)&obj);
    AutoPtr<IHttpConnection> conn = IHttpConnection::Probe(obj);

    if (conn != NULL && !Ptr(conn)->Func(conn->IsOpen)) return FALSE;
    // do NOT check for stale connection, that is an expensive operation

    AutoPtr<IHttpVersionHelper> helper;
    CHttpVersionHelper::AcquireSingleton((IHttpVersionHelper**)&helper);
    Boolean isLessEquals;
    if (entity != NULL) {
        if (Ptr(entity)->Func(entity->GetContentLength) < 0) {
            ver->LessEquals(IProtocolVersion::Probe(Ptr(helper)->Func(helper->GerHttpVersion10)), &isLessEquals);
            if (!Ptr(entity)->Func(entity->IsChunked) || isLessEquals) {
                // if the content length is not known and is not chunk
                // encoded, the connection cannot be reused
                return FALSE;
            }
        }
    }
    // Check for 'Connection' directive
    if (connType == IHeaders::CONN_CLOSE) {
        return FALSE;
    } else if (connType == IHeaders::CONN_KEEP_ALIVE) {
        return TRUE;
    }
    // Resorting to protocol version default close connection policy
    ver->LessEquals(IProtocolVersion::Probe(Ptr(helper)->Func(helper->GerHttpVersion10)), &isLessEquals);
    return !isLessEquals;
}

ECode Connection::SetCanPersist(
    /* [in] */ IHttpEntity* entity,
    /* [in] */ IProtocolVersion* ver,
    /* [in] */ Int32 connType)
{
    mCanPersist = KeepAlive(entity, ver, connType, mHttpContext);
    return NOERROR;
}

ECode Connection::SetCanPersist(
    /* [in] */ Boolean canPersist)
{
    mCanPersist = canPersist;
    return NOERROR;
}

ECode Connection::GetCanPersist(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mCanPersist;
    return NOERROR;
}

ECode Connection::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    return IObject::Probe(mHost)->ToString(result);
}

ECode Connection::GetBuf(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    if (mBuf == NULL) {
        mBuf = ArrayOf<Byte>::Alloc(8192);
    }

    *result = mBuf;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
