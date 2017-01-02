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

#include "org/apache/http/impl/AbstractHttpClientConnection.h"
#include "org/apache/http/impl/DefaultHttpResponseFactory.h"
#include "org/apache/http/impl/io/SocketInputBuffer.h"
#include "org/apache/http/impl/entity/LaxContentLengthStrategy.h"
#include "org/apache/http/impl/entity/StrictContentLengthStrategy.h"
#include "org/apache/http/impl/io/HttpResponseParser.h"
#include "org/apache/http/impl/io/HttpRequestWriter.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::IStatusLine;
using Org::Apache::Http::Impl::Entity::LaxContentLengthStrategy;
using Org::Apache::Http::Impl::Entity::StrictContentLengthStrategy;
using Org::Apache::Http::Impl::IO::HttpResponseParser;
using Org::Apache::Http::Impl::IO::HttpRequestWriter;
using Org::Apache::Http::Impl::IO::SocketInputBuffer;
using Org::Apache::Http::Impl::IO::EIID_SocketInputBuffer;
using Org::Apache::Http::IO::IHttpTransportMetrics;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {

AbstractHttpClientConnection::AbstractHttpClientConnection()
{
    mEntityserializer = CreateEntitySerializer();
    mEntitydeserializer = CreateEntityDeserializer();
}

CAR_INTERFACE_IMPL_2(AbstractHttpClientConnection, Object, IHttpClientConnection, IHttpConnection)

AutoPtr<EntityDeserializer> AbstractHttpClientConnection::CreateEntityDeserializer()
{
    AutoPtr<IContentLengthStrategy> strategy = (IContentLengthStrategy*)new LaxContentLengthStrategy();
    AutoPtr<EntityDeserializer> deserializer = new EntityDeserializer(strategy);
    return deserializer;
}

AutoPtr<EntitySerializer> AbstractHttpClientConnection::CreateEntitySerializer()
{
    AutoPtr<IContentLengthStrategy> strategy = (IContentLengthStrategy*)new StrictContentLengthStrategy();
    AutoPtr<EntitySerializer> serializer = new EntitySerializer(strategy);
    return serializer;
}

AutoPtr<IHttpResponseFactory> AbstractHttpClientConnection::CreateHttpResponseFactory()
{
    AutoPtr<IHttpResponseFactory> factory = (IHttpResponseFactory*)new DefaultHttpResponseFactory();
    return factory;
}

AutoPtr<IHttpMessageParser> AbstractHttpClientConnection::CreateResponseParser(
    /* [in] */ ISessionInputBuffer* buffer,
    /* [in] */ IHttpResponseFactory* responseFactory,
    /* [in] */ IHttpParams* params)
{
    // override in derived class to specify a line parser
    AutoPtr<IHttpMessageParser> parser
            = (IHttpMessageParser*)new HttpResponseParser(buffer, NULL, responseFactory, params);
    return parser;
}

AutoPtr<IHttpMessageWriter> AbstractHttpClientConnection::CreateRequestWriter(
    /* [in] */ ISessionOutputBuffer* buffer,
    /* [in] */ IHttpParams* params)
{
    // override in derived class to specify a line formatter
    AutoPtr<IHttpMessageWriter> writer
            = (IHttpMessageWriter*)new HttpRequestWriter(buffer, NULL, params);
    return writer;
}

ECode AbstractHttpClientConnection::Init(
    /* [in] */ ISessionInputBuffer* inbuffer,
    /* [in] */ ISessionOutputBuffer* outbuffer,
    /* [in] */ IHttpParams* params)
{
    if (inbuffer == NULL) {
        Logger::E("AbstractHttpClientConnection", "Input session buffer may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (outbuffer == NULL) {
        Logger::E("AbstractHttpClientConnection", "Output session buffer may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mInbuffer = inbuffer;
    mOutbuffer = outbuffer;
    AutoPtr<IHttpResponseFactory> factory = CreateHttpResponseFactory();
    mResponseParser = CreateResponseParser(inbuffer, factory, params);
    mRequestWriter = CreateRequestWriter(outbuffer, params);
    AutoPtr<IHttpTransportMetrics> inMetrics, outMetrics;
    inbuffer->GetMetrics((IHttpTransportMetrics**)&inMetrics);
    outbuffer->GetMetrics((IHttpTransportMetrics**)&outMetrics);
    mMetrics = new HttpConnectionMetricsImpl(inMetrics, outMetrics);
    return NOERROR;
}

ECode AbstractHttpClientConnection::IsResponseAvailable(
    /* [in] */ Int32 timeout,
    /* [out] */ Boolean* isAvailable)
{
    VALIDATE_NOT_NULL(isAvailable)
    *isAvailable = FALSE;
    FAIL_RETURN(AssertOpen())
    return mInbuffer->IsDataAvailable(timeout, isAvailable);
}

ECode AbstractHttpClientConnection::SendRequestHeader(
    /* [in] */ IHttpRequest* request)
{
    if (request == NULL) {
        Logger::E("AbstractHttpClientConnection", "HTTP request may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(AssertOpen())
    mRequestWriter->Write(IHttpMessage::Probe(request));
    mMetrics->IncrementRequestCount();
    return NOERROR;
}

ECode AbstractHttpClientConnection::SendRequestEntity(
    /* [in] */ IHttpEntityEnclosingRequest* request)
{
    if (request == NULL) {
        Logger::E("AbstractHttpClientConnection", "HTTP request may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(AssertOpen())
    AutoPtr<IHttpEntity> entity;
    if (request->GetEntity((IHttpEntity**)&entity), entity == NULL) {
        return NOERROR;
    }
    mEntityserializer->Serialize(mOutbuffer, IHttpMessage::Probe(request), entity);
    return NOERROR;
}

ECode AbstractHttpClientConnection::DoFlush()
{
    return mOutbuffer->Flush();
}

ECode AbstractHttpClientConnection::Flush()
{
    FAIL_RETURN(AssertOpen())
    return DoFlush();
}

ECode AbstractHttpClientConnection::ReceiveResponseHeader(
    /* [out] */ IHttpResponse** httpResponse)
{
    VALIDATE_NOT_NULL(httpResponse)
    *httpResponse = NULL;
    FAIL_RETURN(AssertOpen())
    AutoPtr<IHttpMessage> message;
    mResponseParser->Parse((IHttpMessage**)&message);
    AutoPtr<IHttpResponse> response = IHttpResponse::Probe(message);
    AutoPtr<IStatusLine> statusLine;
    response->GetStatusLine((IStatusLine**)&statusLine);
    Int32 code;
    if (statusLine->GetStatusCode(&code), code >= 200) {
        mMetrics->IncrementResponseCount();
    }
    *httpResponse = response;
    REFCOUNT_ADD(*httpResponse)
    return NOERROR;
}

ECode AbstractHttpClientConnection::ReceiveResponseEntity(
    /* [in] */ IHttpResponse* response)
{
    if (response == NULL) {
        Logger::E("AbstractHttpClientConnection", "HTTP request may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(AssertOpen())
    AutoPtr<IHttpEntity> entity;
    mEntitydeserializer->Deserialize(mInbuffer, IHttpMessage::Probe(response)
            , (IHttpEntity**)&entity);
    response->SetEntity(entity);
    return NOERROR;
}

ECode AbstractHttpClientConnection::IsStale(
    /* [out] */ Boolean* isStale)
{
    VALIDATE_NOT_NULL(isStale)
    Boolean isOpen;
    if (IsOpen(&isOpen), !isOpen) {
        *isStale = TRUE;
    }
    // try {
    // BEGIN android-added
    //     don't reuse connections when the socket input stream is exhausted
    AutoPtr<SocketInputBuffer> buffer = reinterpret_cast<SocketInputBuffer*>(mInbuffer->Probe(EIID_SocketInputBuffer));
    if (buffer != NULL) {
        return buffer->IsStale(isStale);
    }
    // END android-added
    Boolean result;
    if (FAILED(mInbuffer->IsDataAvailable(1, &result))) {
        *isStale = TRUE;
        return NOERROR;
    }
    *isStale = FALSE;
    return NOERROR;
    // } catch (IOException ex) {
    //     return true;
    // }
}

ECode AbstractHttpClientConnection::GetMetrics(
    /* [out] */ IHttpConnectionMetrics** metrics)
{
    VALIDATE_NOT_NULL(metrics)
    *metrics = mMetrics;
    REFCOUNT_ADD(*metrics)
    return NOERROR;
}

} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org