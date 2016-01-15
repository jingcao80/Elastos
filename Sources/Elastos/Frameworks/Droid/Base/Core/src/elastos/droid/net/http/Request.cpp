
#include "_Elastos.Droid.Content.h"
#include "_Elastos.Droid.Core.h"
#include "_Elastos.Droid.Utility.h"
#include <Elastos.CoreLibrary.Apache.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Zip.h>
#include "elastos/droid/net/http/Request.h"
#include "elastos/droid/net/Proxy.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/net/http/CHeaders.h"
#include "elastos/droid/net/http/CHttpsConnection.h"
#include "elastos/droid/net/http/Connection.h"
#include "elastos/droid/net/http/ElastosHttpClient.h"
#include "elastos/droid/net/http/ElastosHttpClientConnection.h"
#include "elastos/droid/net/http/Headers.h"
#include "elastos/droid/net/http/HttpLog.h"
#include "elastos/droid/net/http/HttpsConnection.h"
#include "elastos/droid/net/http/Request.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Process.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Utility::IPair;

using Elastos::Core::CObject;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::IO::IInputStream;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Zip::CGZIPInputStream;
using Elastos::Utility::Zip::IGZIPInputStream;

using Org::Apache::Http::EIID_IHttpEntityEnclosingRequest;
using Org::Apache::Http::Entity::CBasicHttpEntity;
using Org::Apache::Http::Entity::CInputStreamEntity;
using Org::Apache::Http::Entity::IAbstractHttpEntity;
using Org::Apache::Http::Entity::IBasicHttpEntity;
using Org::Apache::Http::Entity::IInputStreamEntity;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::IHttpEntityEnclosingRequest;
using Org::Apache::Http::IHttpHost;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::IHttpStatus;
using Org::Apache::Http::IProtocolVersion;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::IStatusLine;
using Org::Apache::Http::Message::CBasicHttpRequest;
using Org::Apache::Http::Message::CBasicHttpEntityEnclosingRequest;
using Org::Apache::Http::Message::IBasicHttpEntityEnclosingRequest;
using Org::Apache::Http::Message::IBasicHttpRequest;
using Org::Apache::Http::Protocol::CRequestContent;
using Org::Apache::Http::Protocol::IHttpContext;
using Org::Apache::Http::Protocol::IRequestContent;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_INTERFACE_IMPL(Request, Object, IRequest)

const String Request::HOST_HEADER("Host");
const String Request::ACCEPT_ENCODING_HEADER("Accept-Encoding");
const String Request::CONTENT_LENGTH_HEADER("content-length");
AutoPtr<IRequestContent> Request::mRequestContentProcessor = InitRequestContentProcessor();

AutoPtr<IRequestContent> Request::InitRequestContentProcessor()
{
    AutoPtr<IRequestContent> rev;
    // TODO: Waiting for CRequestContent
    assert(0);
    // CRequestContent::New((IRequestContent**)&rev);
    return rev;
}

Request::Request()
    : mCancelled(FALSE)
    , mFailCount(0)
    , mReceivedBytes(0)
    , mLoadingPaused(FALSE)
{
    Elastos::Core::CObject::New((IObject**)&mClientResource);
}

ECode Request::constructor(
    /* [in] */ const String& method,
    /* [in] */ IHttpHost* host,
    /* [in] */ IHttpHost* proxyHost,
    /* [in] */ const String& path,
    /* [in] */ IInputStream* bodyProvider,
    /* [in] */ Int32 bodyLength,
    /* [in] */ IEventHandler* eventHandler,
    /* [in] */ IMap* headers)
{
    if (bodyProvider == NULL && !method.EqualsIgnoreCase("POST")) {
        // TODO: Waiting for CBasicHttpRequest
        assert(0);
        // CBasicHttpRequest::New(method, Ptr(this)->Func(GetUri), (IBasicHttpRequest**)&mHttpRequest);
    } else {
        // TODO: Waiting for CBasicHttpEntityEnclosingRequest
        assert(0);
        // CBasicHttpEntityEnclosingRequest(method, Ptr(this)->Func(GetUri), (IBasicHttpRequest**)&mHttpRequest);

        // it is ok to have null entity for BasicHttpEntityEnclosingRequest.
        // By using BasicHttpEntityEnclosingRequest, it will set up the
        // correct content-length, content-type and content-encoding.
        if (bodyProvider != NULL) {
            SetBodyProvider(bodyProvider, bodyLength);
        }
    }
    AddHeader(HOST_HEADER, Ptr(this)->Func(GetHostPort));

    /* FIXME: if webcore will make the root document a
       high-priority request, we can ask for gzip encoding only on
       high priority reqs (saving the trouble for images, etc) */
    AddHeader(ACCEPT_ENCODING_HEADER, String("gzip"));
    AddHeaders(headers);
}

ECode Request::SetLoadingPaused(
    /* [in] */ Boolean pause)
{
    synchronized(this) {
        mLoadingPaused = pause;

        // Wake up the paused thread if we're unpausing the load.
        if (!mLoadingPaused) {
            Notify();
        }
    }

    return NOERROR;
}

ECode Request::SetConnection(
    /* [in] */ IConnection* connection)
{
    mConnection = connection;
    return NOERROR;
}

ECode Request::GetEventHandler(
    /* [out] */ IEventHandler** result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(mEventHandler)
}

ECode Request::AddHeader(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    if (name.IsNullOrEmpty()) {
        String damage("Null http header name");
        HttpLog::E(damage);
        return E_NULL_POINTER_EXCEPTION;
    }
    if (value.IsNullOrEmpty() || value.GetLength() == 0) {
        String damage = String("Null or empty value for header \"") + name + String("\"");
        HttpLog::E(damage);
        return E_RUNTIME_EXCEPTION;
    }

    // TODO: Waiting for BasicHttpRequest
    assert(0);
    return NOERROR;
    // return mHttpRequest->AddHeader(name, value);
}

ECode Request::AddHeaders(
    /* [in] */ IMap* headers)
{
    if (headers == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ICollection> collection;
    FOR_EACH(it, collection) {
        AutoPtr<IInterface> pair;
        it->GetNext((IInterface**)&pair);
        AutoPtr<IInterface> first;
        IPair::Probe(pair)->GetFirst((IInterface**)&first);
        AutoPtr<IInterface> second;
        IPair::Probe(pair)->GetSecond((IInterface**)&second);
        String name;
        ICharSequence::Probe(first)->ToString(&name);
        String value;
        ICharSequence::Probe(second)->ToString(&value);
        AddHeader(name, value);
    }

    return NOERROR;
}

ECode Request::SendRequest(
    /* [in] */ IElastosHttpClientConnection* httpClientConnection)
{
    if (mCancelled){
        return NOERROR; // don't send cancelled requests
    }

    if (HttpLog::LOGV) {
        String name;
        mHost->GetSchemeName(&name);
        String hostPort;
        GetHostPort(&hostPort);
        HttpLog::V("Request.sendRequest() %s://%s", name.string(), hostPort.string());
        // HttpLog.v(mHttpRequest.getRequestLine().toString());
        if (FALSE) {
            AutoPtr<IIterator> i;
            // TODO: Waiting for IBasicHttpRequest
            assert(0);
            // mHttpRequest->HeaderIterator((IIterator**)&i);
            Boolean hasNext;
            i->HasNext(&hasNext);
            while(hasNext) {
                AutoPtr<IInterface> header;
                i->GetNext((IInterface**)&header);
                String name;
                IHeader::Probe(header)->GetName(&name);
                String value;
                IHeader::Probe(header)->GetValue(&value);
                HttpLog::V("%s: %s", name.string(), value.string());
            }
        }
    }

    AutoPtr<IHttpContext> context;
    ((Connection*)mConnection.Get())->GetHttpContext((IHttpContext**)&context);
    // TODO: Waiting for IRequestContent
    assert(0);
    // mRequestContentProcessor->Process(mHttpRequest, context);
    AutoPtr<IHttpRequest> req = IHttpRequest::Probe(mHttpRequest);
    httpClientConnection->SendRequestHeader(req);
    AutoPtr<IHttpEntityEnclosingRequest> httpReq = IHttpEntityEnclosingRequest::Probe(mHttpRequest);
    if (httpReq != NULL) {
        httpClientConnection->SendRequestEntity(httpReq);
    }

    if (HttpLog::LOGV) {
        String name;
        mHost->GetSchemeName(&name);
        String hostPort;
        this->GetHostPort(&hostPort);
        HttpLog::V("Request.requestSent() %s://%s%s", name.string(), hostPort.string(), mPath.string());
    }
}

ECode Request::ReadResponse(
    /* [in] */ IElastosHttpClientConnection* httpClientConnection)
{
    if (mCancelled){
        return NOERROR; // don't send cancelled requests
    }

    AutoPtr<IStatusLine> statusLine;
    Boolean hasBody = FALSE;
    httpClientConnection->Flush();
    Int32 statusCode = 0;

    AutoPtr<IHeaders> header;
    CHeaders::New((IHeaders**)&header);
    do {
        statusLine = NULL;
        FAIL_RETURN(httpClientConnection->ParseResponseHeader(header, (IStatusLine**)&statusLine));
        statusLine->GetStatusCode(&statusCode);
    } while (statusCode < IHttpStatus::SC_OK);
    if (HttpLog::LOGV) {
        String str;
        IObject::Probe(statusLine)->ToString(&str);
        HttpLog::V(String("Request.readResponseStatus() ") +
                StringUtils::ToString(str.GetLength()) + String(" ") + str);
    }

    AutoPtr<IProtocolVersion> v;
    statusLine->GetProtocolVersion((IProtocolVersion**)&v);
    Int32 major;
    Int32 minor;
    v->GetMajor(&major);
    v->GetMinor(&minor);
    String phrase;
    statusLine->GetReasonPhrase(&phrase);
    mEventHandler->Status(major, minor, statusCode, phrase);
    mEventHandler->Headers(header);
    AutoPtr<IHttpEntity> entity;
    AutoPtr<IHttpRequest> req = IHttpRequest::Probe(mHttpRequest);
    hasBody = CanResponseHaveBody(req, statusCode);

    if (hasBody) {
        httpClientConnection->ReceiveResponseEntity(header, (IHttpEntity**)&entity);
    }

    // restrict the range request to the servers claiming that they are
    // accepting ranges in bytes
    String ranges;
    header->GetAcceptRanges(&ranges);
    Boolean supportPartialContent = ranges.EqualsIgnoreCase("bytes");

    if (entity != NULL) {
        AutoPtr<IInputStream> is;
        entity->GetContent((IInputStream**)&is);

        // process gzip content encoding
        AutoPtr<IHeader> contentEncoding;
        entity->GetContentEncoding((IHeader**)&contentEncoding);
        AutoPtr<IInputStream> nis;
        AutoPtr<ArrayOf<Byte> > buf;
        Int32 count = 0;

        String value;
        if (contentEncoding != NULL &&
            (contentEncoding->GetValue(&value), value).Equals("gzip")) {
            ECode ec = CGZIPInputStream::New(is, (IInputStream**)&nis);
            if(FAILED(ec)) {
                // don't throw if we have a non-OK status code
                if (statusCode == IHttpStatus::SC_OK
                        || statusCode == IHttpStatus::SC_PARTIAL_CONTENT) {
                    if (supportPartialContent && count > 0) {
                        // if there is uncommited content, we should commit them
                        // as we will continue the request
                        mEventHandler->Data(buf, count);
                    }

                    if (nis != NULL) {
                        nis->Close();
                    }

                    return ec;
                }
            }
        } else {
            nis = is;
        }

        /* accumulate enough data to make it worth pushing it
         * up the stack */
        ((Connection*)mConnection.Get())->GetBuf((ArrayOf<Byte>**)&buf);
        Int32 len = 0;
        Int32 lowWater = buf->GetLength() / 2;
        while (len != -1) {
                synchronized(mClientResource) {
                    while (mLoadingPaused) {
                        // Put this (network loading) thread to sleep if WebCore
                        // has asked us to. This can happen with plugins for
                        // example, if we are streaming data but the plugin has
                        // filled its internal buffers.
                        if(FAILED(Wait())) {
                            HttpLog::E(String("Interrupted exception whilst network thread paused at WebCore's request."));
                        }
                    }
                }

            ECode ec;
            ec = nis->Read(buf, count, buf->GetLength() - count, &len);
            if(FAILED(ec)) {
                /* InflaterInputStream throws an EOFException when the
                   server truncates gzipped content.  Handle this case
                   as we do truncated non-gzipped content: no error */
                if (count > 0) {
                    // if there is uncommited content, we should commit them
                    mEventHandler->Data(buf, count);
                }
                if (HttpLog::LOGV) HttpLog::V(String("readResponse() handling "));
            }

            if (len != -1) {
                count += len;
                if (supportPartialContent) mReceivedBytes += len;
            }
            if (len == -1 || count >= lowWater) {
                if (HttpLog::LOGV) {
                    HttpLog::V(String("Request.readResponse() ") + StringUtils::ToString(count));
                }
                mEventHandler->Data(buf, count);
                count = 0;
            }
        }

        if (nis != NULL) {
            nis->Close();
        }
    }

    AutoPtr<IProtocolVersion>  ver;
    statusLine->GetProtocolVersion((IProtocolVersion**)&ver);
    Int32 type;
    header->GetConnectionType(&type);
    ((Connection*)mConnection.Get())->SetCanPersist(entity, ver, type);
    mEventHandler->EndData();
    Complete();

    if (HttpLog::LOGV) {
        String name;
        mHost->GetSchemeName(&name);
        HttpLog::V("Request.readResponse(): done %s://%s%s",
            name.string(), Ptr(this)->Func(GetHostPort).string(), mPath.string());
    }

    return NOERROR;
}

ECode Request::Cancel()
{
    synchronized(mClientResource) {
        if (HttpLog::LOGV) {
            HttpLog::V("Request.cancel(): %s", Ptr(this)->Func(GetUri).string());
        }

        // Ensure that the network thread is not blocked by a hanging request from WebCore to
        // pause the load.
        mLoadingPaused = FALSE;
        Notify();

        mCancelled = TRUE;
        if (mConnection != NULL) {
            ((Connection*)mConnection.Get())->Cancel();
        }
    }
    return NOERROR;
}

ECode Request::GetHostPort(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String myScheme;
    mHost->GetSchemeName(&myScheme);
    Int32 myPort;
    mHost->GetPort(&myPort);

    String port;
    // Only send port when we must... many servers can't deal with it
    if (myPort != 80 && myScheme.Equals("http") ||
        myPort != 443 && myScheme.Equals("https")) {
        mHost->ToHostString(&port);
    } else {
        mHost->GetHostName(&port);
    }

    FUNC_RETURN(port)
}

ECode Request::GetUri(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String name;
    if (mProxyHost == NULL ||
            (mHost->GetSchemeName(&name), name).Equals("https")) {
        FUNC_RETURN(mPath)
    }
    mHost->GetSchemeName(&name);
    FUNC_RETURN(name + String("://") + Ptr(this)->Func(GetHostPort) + mPath)
}

ECode Request::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mPath;
    return NOERROR;
}

ECode Request::Reset()
{
    /* clear content-length header */
    // TODO: Waiting for IBasicHttpRequest
    assert(0);
    // mHttpRequest->RemoveHeaders(CONTENT_LENGTH_HEADER);

    if (mBodyProvider != NULL) {
        if(FAILED(mBodyProvider->Reset())) {
            if (HttpLog::LOGV) {
                HttpLog::V("failed to reset body provider %s", Ptr(this)->Func(GetUri).string());
            }
        }

        SetBodyProvider(mBodyProvider, mBodyLength);
    }

    if (mReceivedBytes > 0) {
        // reset the fail count as we continue the request
        mFailCount = 0;
        // set the "Range" header to indicate that the retry will continue
        // instead of restarting the request
        HttpLog::V(String("*** Request.reset() to range:") + StringUtils::ToString(mReceivedBytes));
        // TODO: Waiting for IBasicHttpRequest
        assert(0);
        // mHttpRequest->SetHeader(String("Range"), String("bytes=") + StringUtils::ToString(mReceivedBytes) + String("-"));
    }

    return NOERROR;
}

ECode Request::WaitUntilComplete()
{
    synchronized(mClientResource) {
        if (HttpLog::LOGV) HttpLog::V(String("Request.waitUntilComplete()"));
        ISynchronize::Probe(mClientResource)->Wait();
        if (HttpLog::LOGV) HttpLog::V(String("Request.waitUntilComplete() done waiting"));
    }
    return NOERROR;
}

ECode Request::Complete()
{
    synchronized(mClientResource) {
        return ISynchronize::Probe(mClientResource)->NotifyAll();
    }
    return NOERROR;
}

Boolean Request::CanResponseHaveBody(
    /* [in] */ IHttpRequest* request,
    /* [in] */ Int32 status)
{
    AutoPtr<IRequestLine> line;
    request->GetRequestLine((IRequestLine**)&line);
    String str;
    line->GetMethod(&str);
    if (str.EqualsIgnoreCase("HEAD")) {
        return FALSE;
    }
    return status >= IHttpStatus::SC_OK
        && status != IHttpStatus::SC_NO_CONTENT
        && status != IHttpStatus::SC_NOT_MODIFIED;
}

ECode Request::SetBodyProvider(
    /* [in] */ IInputStream* bodyProvider,
    /* [in] */ Int32 bodyLength)
{
    if (!Ptr(bodyProvider)->Func(IInputStream::IsMarkSupported)) {
        Logger::E("Request", "bodyProvider must support mark()");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // Mark beginning of stream
    bodyProvider->Mark(0x7FFFFFFF/*IInteger::MAX_VALUE*/);

    AutoPtr<IBasicHttpEntityEnclosingRequest> request
        = IBasicHttpEntityEnclosingRequest::Probe(mHttpRequest);
    AutoPtr<IAbstractHttpEntity> abstractHttpEntity;
    CInputStreamEntity::New(bodyProvider, bodyLength, (IAbstractHttpEntity**)&abstractHttpEntity);
    AutoPtr<IInputStreamEntity> entity = IInputStreamEntity::Probe(abstractHttpEntity);
    // TODO: Waiting for IBasicHttpEntityEnclosingRequest
    assert(0);
    return NOERROR;
    // return request->SetEntity(entity);
}

ECode Request::HandleSslErrorResponse(
    /* [in] */ Boolean proceed)
{
    AutoPtr<IHttpsConnection> connection;
    connection = IHttpsConnection::Probe(mConnection);
    if (connection != NULL) {
        ((HttpsConnection*)connection.Get())->RestartConnection(proceed);
    }

    return NOERROR;
}

ECode Request::Error(
    /* [in] */ Int32 errorId,
    /* [in] */ Int32 resourceId)
{
    AutoPtr<ICharSequence> cs;
    ((Connection*)mConnection.Get())->mContext->GetText(resourceId, (ICharSequence**)&cs);
    String str;
    cs->ToString(&str);
    return mEventHandler->Error(errorId, str);
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
