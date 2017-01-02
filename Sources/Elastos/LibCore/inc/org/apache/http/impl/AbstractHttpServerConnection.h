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

#ifndef __ORG_APACHE_HTTP_IMPL_ABSTRACTHTTPSERVERCONNECTION_H__
#define __ORG_APACHE_HTTP_IMPL_ABSTRACTHTTPSERVERCONNECTION_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "org/apache/http/impl/HttpConnectionMetricsImpl.h"
#include "org/apache/http/impl/entity/EntityDeserializer.h"
#include "org/apache/http/impl/entity/EntitySerializer.h"

using Org::Apache::Http::IHttpServerConnection;
using Org::Apache::Http::IHttpConnection;
using Org::Apache::Http::IHttpEntityEnclosingRequest;
using Org::Apache::Http::IHttpRequestFactory;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::IHttpConnectionMetrics;
using Org::Apache::Http::Impl::Entity::EntitySerializer;
using Org::Apache::Http::Impl::Entity::EntityDeserializer;
using Org::Apache::Http::IO::IHttpMessageParser;
using Org::Apache::Http::IO::IHttpMessageWriter;
using Org::Apache::Http::IO::ISessionInputBuffer;
using Org::Apache::Http::IO::ISessionOutputBuffer;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {

/**
 * Abstract server-side HTTP connection capable of transmitting and receiving data
 * using arbitrary {@link SessionInputBuffer} and {@link SessionOutputBuffer}
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 618017 $
 *
 * @since 4.0
 */
class AbstractHttpServerConnection
    : public Object
    , public IHttpServerConnection
    , public IHttpConnection
{
public:
    AbstractHttpServerConnection();

    CAR_INTERFACE_DECL()

    CARAPI ReceiveRequestHeader(
        /* [out] */ IHttpRequest** request);

    CARAPI ReceiveRequestEntity(
        /* [in] */ IHttpEntityEnclosingRequest* request);

    CARAPI Flush();

    CARAPI SendResponseHeader(
        /* [in] */ IHttpResponse* response);

    CARAPI SendResponseEntity(
        /* [in] */ IHttpResponse* response);

    CARAPI IsStale(
        /* [out] */ Boolean* isStale);

    CARAPI GetMetrics(
        /* [out] */ IHttpConnectionMetrics** metrics);

protected:
    virtual CARAPI AssertOpen() = 0;

    CARAPI_(AutoPtr<EntityDeserializer>) CreateEntityDeserializer();

    CARAPI_(AutoPtr<EntitySerializer>) CreateEntitySerializer();

    CARAPI_(AutoPtr<IHttpRequestFactory>) CreateHttpRequestFactory();

    CARAPI_(AutoPtr<IHttpMessageParser>) CreateRequestParser(
        /* [in] */ ISessionInputBuffer* buffer,
        /* [in] */ IHttpRequestFactory* requestFactory,
        /* [in] */ IHttpParams* params);

    CARAPI_(AutoPtr<IHttpMessageWriter>) CreateResponseWriter(
        /* [in] */ ISessionOutputBuffer* buffer,
        /* [in] */ IHttpParams* params);

    CARAPI Init(
        /* [in] */ ISessionInputBuffer* inbuffer,
        /* [in] */ ISessionOutputBuffer* outbuffer,
        /* [in] */ IHttpParams* params);

    CARAPI DoFlush();

private:
    AutoPtr<EntitySerializer> mEntityserializer;
    AutoPtr<EntityDeserializer> mEntitydeserializer;

    AutoPtr<ISessionInputBuffer> mInbuffer;
    AutoPtr<ISessionOutputBuffer> mOutbuffer;
    AutoPtr<IHttpMessageParser> mRequestParser;
    AutoPtr<IHttpMessageWriter> mResponseWriter;
    AutoPtr<HttpConnectionMetricsImpl> mMetrics;
};

} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_ABSTRACTHTTPSERVERCONNECTION_H__
