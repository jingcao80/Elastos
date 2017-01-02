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

#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/net/http/ElastosHttpClientConnection.h"
#include "elastos/droid/net/http/CElastosHttpClient.h"
#include "elastos/droid/net/http/ElastosHttpClient.h"
#include "elastos/droid/net/http/HttpLog.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::Build;

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Net::CSocket;
using Elastos::Net::IInetAddress;
using Elastos::Net::ISocket;
using Elastos::Utility::Logging::Logger;

using Org::Apache::Http::EIID_IHttpConnection;
using Org::Apache::Http::EIID_IHttpInetConnection;
using Org::Apache::Http::Entity::CBasicHttpEntity;
using Org::Apache::Http::Entity::IBasicHttpEntity;
using Org::Apache::Http::Entity::IContentLengthStrategy;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::IHttpClientConnection;
using Org::Apache::Http::IHttpConnection;
using Org::Apache::Http::IHttpConnectionMetrics;
using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::IHttpEntityEnclosingRequest;
using Org::Apache::Http::IHttpInetConnection;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::IO::IHttpMessageWriter;
using Org::Apache::Http::IO::ISessionInputBuffer;
using Org::Apache::Http::IO::ISessionOutputBuffer;
using Org::Apache::Http::IStatusLine;
// using Org::Apache::Http::Impl::CHttpConnectionMetricsImpl;
// using Org::Apache::Http::Impl::Entity::CEntitySerializer;
// using Org::Apache::Http::Impl::Entity::CStrictContentLengthStrategy;
using Org::Apache::Http::Impl::Entity::IEntitySerializer;
using Org::Apache::Http::Impl::Entity::IStrictContentLengthStrategy;
using Org::Apache::Http::Impl::IHttpConnectionMetricsImpl;
// using Org::Apache::Http::Impl::IO::CChunkedInputStream;
// using Org::Apache::Http::Impl::IO::CContentLengthInputStream;
// using Org::Apache::Http::Impl::IO::CHttpRequestWriter;
// using Org::Apache::Http::Impl::IO::CIdentityInputStream;
// using Org::Apache::Http::Impl::IO::CSocketInputBuffer;
using Org::Apache::Http::Impl::IO::IChunkedInputStream;
using Org::Apache::Http::Impl::IO::IContentLengthInputStream;
using Org::Apache::Http::Impl::IO::IHttpRequestWriter;
using Org::Apache::Http::Impl::IO::IIdentityInputStream;
using Org::Apache::Http::Impl::IO::ISocketInputBuffer;
using Org::Apache::Http::Impl::IO::ISocketOutputBuffer;
// using Org::Apache::Http::Message::CBasicLineParserHelper;
using Org::Apache::Http::Message::CParserCursor;
using Org::Apache::Http::Message::IBasicLineParser;
using Org::Apache::Http::Message::IBasicLineParserHelper;
using Org::Apache::Http::Message::ILineParser;
using Org::Apache::Http::Message::IParserCursor;
using Org::Apache::Http::Params::CHttpConnectionParams;
using Org::Apache::Http::Params::ICoreConnectionPNames;
using Org::Apache::Http::Params::IHttpConnectionParams;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Utility::CCharArrayBuffer;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_INTERFACE_IMPL_3(ElastosHttpClientConnection, Object, IHttpInetConnection, IHttpConnection, IElastosHttpClientConnection)

ElastosHttpClientConnection::ElastosHttpClientConnection()
    : mMaxHeaderCount(0)
    , mMaxLineLength(0)
    , mOpen(FALSE)
{}

ECode ElastosHttpClientConnection::constructor()
{
    AutoPtr<IStrictContentLengthStrategy> newStrictContentLengthStrategy;
    // TODO: Waiting for CStrictContentLengthStrategy, CEntitySerializer
    assert(0);
    // CStrictContentLengthStrategy::New((IStrictContentLengthStrategy**)&newStrictContentLengthStrategy);
    // return CEntitySerializer::New(newStrictContentLengthStrategy, (IEntitySerializer**)&mEntityserializer);
    return NOERROR;
}

ECode ElastosHttpClientConnection::Bind(
    /* [in] */ ISocket* socket,
    /* [in] */ IHttpParams* params)
{
    if (socket == NULL) {
        Logger::E("ElastosHttpClientConnection", "Socket may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        Logger::E("ElastosHttpClientConnection", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(AssertNotOpen());
    AutoPtr<IHttpConnectionParams> helper;
    CHttpConnectionParams::AcquireSingleton((IHttpConnectionParams**)&helper);
    Boolean isTcpNoDelay;
    helper->GetTcpNoDelay(params, &isTcpNoDelay);
    socket->SetTcpNoDelay(isTcpNoDelay);
    Int32 soTimeout;
    helper->GetSoTimeout(params, &soTimeout);
    socket->SetSoTimeout(soTimeout);
    Int32 linger;
    helper->GetLinger(params, &linger);
    if (linger >= 0) {
        socket->SetSoLinger(linger > 0, linger);
    }
    mSocket = socket;
    Int32 buffersize;
    helper->GetSocketBufferSize(params, &buffersize);
    // TODO: Waiting for CSocketInputBuffer, CSocketOutputBuffer
    assert(0);
    // CSocketInputBuffer::New(socket, buffersize, params, (ISessionInputBuffer**)&mInbuffer);
    // CSocketOutputBuffer::New(socket, buffersize, params, (ISessionOutputBuffer**)&mOutbuffer);
    params->GetInt32Parameter(
            ICoreConnectionPNames::MAX_HEADER_COUNT, -1, &mMaxHeaderCount);
    params->GetInt32Parameter(
            ICoreConnectionPNames::MAX_LINE_LENGTH, -1, &mMaxLineLength);
    // TODO: Waiting for CHttpRequestWriter, CHttpConnectionMetricsImpl
    assert(0);
    // CHttpRequestWriter::New(mOutbuffer, NULL, params, (IHttpMessageWriter**)&mRequestWriter);
    // CHttpConnectionMetricsImpl::New(Ptr(mInbuffer)->Func(mInbuffer->GetMetrics), Ptr(mOutbuffer)->Func(mOutbuffer->GetMetrics), (IHttpConnectionMetricsImpl**)&mMetrics);
    mOpen = TRUE;
    return NOERROR;
}

ECode ElastosHttpClientConnection::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<StringBuilder> buffer = new StringBuilder();
    buffer->Append("CElastosHttpClientConnection[");
    Boolean isOpen;
    IsOpen(&isOpen);
    if (isOpen) {
        Int32 port;
        GetRemotePort(&port);
        buffer->Append(port);
    } else {
        buffer->Append("closed");
    }
    buffer->Append("]");
    *result = buffer->ToString();
    return NOERROR;
}

ECode ElastosHttpClientConnection::AssertNotOpen()
{
    if (mOpen) return E_ILLEGAL_STATE_EXCEPTION;
    return NOERROR;
}

ECode ElastosHttpClientConnection::AssertOpen()
{
    if (!mOpen) return E_ILLEGAL_STATE_EXCEPTION;
    return NOERROR;
}

ECode ElastosHttpClientConnection::IsOpen(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // to make this method useful, we want to check if the socket is connected
    *result = FALSE;
    if (mOpen && mSocket != NULL) {
        Boolean isConnected;
        mSocket->IsConnected(&isConnected);
        *result = isConnected;
    }

    return NOERROR;
}

ECode ElastosHttpClientConnection::GetLocalAddress(
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result)

    if (mSocket != NULL) {
        return mSocket->GetLocalAddress(result);
    } else {
        *result = NULL;
    }

    return NOERROR;
}

ECode ElastosHttpClientConnection::GetLocalPort(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (mSocket != NULL) {
        return mSocket->GetLocalPort(result);
    } else {
        *result = -1;
    }

    return NOERROR;
}

ECode ElastosHttpClientConnection::GetRemoteAddress(
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result)

    if (mSocket != NULL) {
        return mSocket->GetInetAddress(result);
    } else {
        *result = NULL;
    }

    return NOERROR;
}

ECode ElastosHttpClientConnection::GetRemotePort(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (mSocket != NULL) {
        return mSocket->GetPort(result);
    } else {
        *result = -1;
    }

    return NOERROR;
}

ECode ElastosHttpClientConnection::SetSocketTimeout(
    /* [in] */ Int32 timeout)
{
    FAIL_RETURN(AssertOpen());
    if (mSocket != NULL) {
        // It is not quite clear from the original documentation if there are any
        // other legitimate cases for a socket exception to be thrown when setting
        // SO_TIMEOUT besides the socket being already closed
        return mSocket->SetSoTimeout(timeout);
    }

    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode ElastosHttpClientConnection::GetSocketTimeout(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (mSocket != NULL) {
        return mSocket->GetSoTimeout(result);
    }

    *result = -1;
    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode ElastosHttpClientConnection::Shutdown()
{
    mOpen = FALSE;
    if (mSocket != NULL) {
        return mSocket->Close();
    }

    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode ElastosHttpClientConnection::Close()
{
    if (!mOpen)
    {
        return NOERROR;
    }

    mOpen = FALSE;
    DoFlush();

    if(!FAILED(mSocket->ShutdownOutput())) {
    }

    if(!FAILED(mSocket->ShutdownInput())) {
    }

    return mSocket->Close();
}

ECode ElastosHttpClientConnection::SendRequestHeader(
    /* [in] */ IHttpRequest* request)
{
    if (request == NULL)
    {
        Logger::E("ElastosHttpClientConnection", "HTTP request may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(AssertOpen());
    mRequestWriter->Write(IHttpMessage::Probe(request));
    // TODO: Waiting for IHttpConnectionMetricsImpl
    assert(0);
    // mMetrics->IncrementRequestCount();

    return NOERROR;
}

ECode ElastosHttpClientConnection::SendRequestEntity(
    /* [in] */ IHttpEntityEnclosingRequest* request)
{
    if (request == NULL) {
        Logger::E("ElastosHttpClientConnection", "HTTP request may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(AssertOpen());

    AutoPtr<IHttpEntity> entity;
    request->GetEntity((IHttpEntity**)&entity);
    if (entity == NULL) {
        return NOERROR;
    }
    // TODO: Waiting for IEntitySerializer
    assert(0);
    // return mEntityserializer->Serialize(mOutbuffer, request, entity);
    return NOERROR;
}

ECode ElastosHttpClientConnection::DoFlush()
{
    return mOutbuffer->Flush();
}

ECode ElastosHttpClientConnection::Flush()
{
    FAIL_RETURN(AssertOpen());
    return DoFlush();
}

ECode ElastosHttpClientConnection::ParseResponseHeader(
    /* [in] */ IHeaders* headers,
    /* [out] */ IStatusLine** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    FAIL_RETURN(AssertOpen());
    AutoPtr<ICharArrayBuffer> current;
    CCharArrayBuffer::New(64, (ICharArrayBuffer**)&current);
    Int32 count;
    mInbuffer->ReadLine(current, &count);
    if (count == -1) {
        Logger::E("ElastosHttpClientConnection", "The target server failed to respond");
        return E_NO_HTTP_RESPONSE_EXCEPTION;
    }
    // Create the status line from the status string
    AutoPtr<IStatusLine> statusline;
    AutoPtr<IBasicLineParserHelper> helper;
    // TODO: Waiting for CBasicLineParserHelper
    assert(0);
    // CBasicLineParserHelper::AcquireSingleton((IBasicLineParserHelper**)&helper);
    AutoPtr<IBasicLineParser> defaultBasicLineParser;
    // TODO: Waiting for IBasicLineParser
    assert(0);
    // helper->GetDEFAULT((IBasicLineParser**)&defaultBasicLineParser);
    AutoPtr<IParserCursor> newParserCursor;
    CParserCursor::New(0, Ptr(current)->Func(current->GetLength), (IParserCursor**)&newParserCursor);
    ILineParser::Probe(defaultBasicLineParser)->ParseStatusLine(current, newParserCursor, (IStatusLine**)&statusline);

    if (HttpLog::LOGV) HttpLog::V("read: %s", TO_CSTR(statusline));
    Int32 statusCode;
    statusline->GetStatusCode(&statusCode);
    // Parse header body
    AutoPtr<ICharArrayBuffer> previous;
    Int32 headerNumber = 0;
    while(TRUE) {
        if (current == NULL) {
            CCharArrayBuffer::New(64, (ICharArrayBuffer**)&current);
        } else {
            // This must be he buffer used to parse the status
            current->Clear();
        }
        Int32 l;
        mInbuffer->ReadLine(current, &l);
        if (l == -1 || Ptr(current)->Func(current->GetLength) < 1) {
            break;
        }
        // Parse the header name and value
        // Check for folded headers first
        // Detect LWS-char see HTTP/1.0 or HTTP/1.1 Section 2.2
        // discussion on folded headers
        Char32 first;
        current->CharAt(0, &first);
        if ((first == ' ' || first == '\t') && previous != NULL) {
            // we have continuation folded header
            // so append value
            Int32 start = 0;
            Int32 length;
            current->GetLength(&length);
            while (start < length) {
                Char32 ch;
                current->CharAt(start, &ch);
                if (ch != ' ' && ch != '\t') {
                    break;
                }
                start++;
            }
            if (mMaxLineLength > 0 &&
                    Ptr(previous)->Func(previous->GetLength) + 1 + Ptr(current)->Func(current->GetLength) - start >
                        mMaxLineLength) {
                Logger::E("ElastosHttpClientConnection", "Maximum line length limit exceeded");
                return E_IO_EXCEPTION;
            }
            previous->Append(' ');
            previous->Append(current, start, Ptr(current)->Func(current->GetLength) - start);
        } else {
            if (previous != NULL) {
                headers->ParseHeader(previous);
            }
            headerNumber++;
            previous = current;
            current = NULL;
        }
        if (mMaxHeaderCount > 0 && headerNumber >= mMaxHeaderCount) {
            Logger::E("ElastosHttpClientConnection", "Maximum header count exceeded");
            return E_IO_EXCEPTION;
        }
    }
    if (previous != NULL) {
        headers->ParseHeader(previous);
    }
    if (statusCode >= 200) {
        // TODO: Waiting for IHttpConnectionMetricsImpl
        assert(0);
        // mMetrics->IncrementResponseCount();
    }
    FUNC_RETURN(statusline);
}

ECode ElastosHttpClientConnection::ReceiveResponseEntity(
    /* [in] */ IHeaders* headers,
    /* [out] */ IHttpEntity** result)
{
    VALIDATE_NOT_NULL(result)

    FAIL_RETURN(AssertOpen());
    AutoPtr<IBasicHttpEntity> entity;
    CBasicHttpEntity::New((IBasicHttpEntity**)&entity);
    Int64 len = DetermineLength(headers);
    if (len == IContentLengthStrategy::CHUNKED) {
        IAbstractHttpEntity::Probe(entity)->SetChunked(TRUE);
        entity->SetContentLength(-1);
        AutoPtr<IChunkedInputStream> newChunkedInputStream;
        // TODO: Waiting for CChunkedInputStream
        assert(0);
        // CChunkedInputStream::New(mInbuffer, (IChunkedInputStream**)&newChunkedInputStream);
        entity->SetContent(IInputStream::Probe(newChunkedInputStream));
    }
    else if (len == IContentLengthStrategy::IDENTITY) {
        IAbstractHttpEntity::Probe(entity)->SetChunked(FALSE);
        entity->SetContentLength(-1);
        AutoPtr<IIdentityInputStream> newIdentityInputStream;
        // TODO: Waiting for CIdentityInputStream
        assert(0);
        // CIdentityInputStream::New(mInbuffer, (IIdentityInputStream**)&newIdentityInputStream);
        entity->SetContent(IInputStream::Probe(newIdentityInputStream));
    } else {
        IAbstractHttpEntity::Probe(entity)->SetChunked(FALSE);
        entity->SetContentLength(len);
        AutoPtr<IContentLengthInputStream> newContentLengthInputStream;
        // TODO: Waiting for CContentLengthInputStream
        assert(0);
        // CContentLengthInputStream::New(mInbuffer, len, (IContentLengthInputStream**)&newContentLengthInputStream);
        entity->SetContent(IInputStream::Probe(newContentLengthInputStream));
    }
    String contentTypeHeader;
    headers->GetContentType(&contentTypeHeader);
    if (contentTypeHeader != NULL) {
        IAbstractHttpEntity::Probe(entity)->SetContentType(contentTypeHeader);
    }
    String contentEncodingHeader;
    headers->GetContentEncoding(&contentEncodingHeader);
    if (contentEncodingHeader != NULL) {
        IAbstractHttpEntity::Probe(entity)->SetContentEncoding(contentEncodingHeader);
    }
    FUNC_RETURN(IHttpEntity::Probe(entity));
}

Int64 ElastosHttpClientConnection::DetermineLength(
    /* [in] */ IHeaders* headers)
{
    Int64 transferEncoding;
    headers->GetTransferEncoding(&transferEncoding);
    // We use Transfer-Encoding if present and ignore Content-Length.
    // RFC2616, 4.4 item number 3
    if (transferEncoding < IHeaders::NO_TRANSFER_ENCODING) {
        return transferEncoding;
    } else {
        Int64 contentlen;
        headers->GetContentLength(&contentlen);
        if (contentlen > IHeaders::NO_CONTENT_LENGTH) {
            return contentlen;
        } else {
            return IContentLengthStrategy::IDENTITY;
        }
    }
}

ECode ElastosHttpClientConnection::IsStale(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    FAIL_RETURN(AssertOpen());
    *result = FALSE;

    Boolean available;
    ECode ec;
    ec = mInbuffer->IsDataAvailable(1, &available);
    if (FAILED(ec)) {
        if (ec == (ECode)E_IO_EXCEPTION) {
            *result = TRUE;
            return NOERROR;
        }
        return ec;
    }
    *result = FALSE;
    return NOERROR;
}

ECode ElastosHttpClientConnection::GetMetrics(
    /* [out] */ IHttpConnectionMetrics** result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(IHttpConnectionMetrics::Probe(mMetrics))
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
