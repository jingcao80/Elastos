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

#ifndef __ORG_APACHE_HTTP_IMPL_ABSTRACTHTTPCLIENTCONNECTION_H__
#define __ORG_APACHE_HTTP_IMPL_ABSTRACTHTTPCLIENTCONNECTION_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "org/apache/http/impl/HttpConnectionMetricsImpl.h"
#include "org/apache/http/impl/entity/EntityDeserializer.h"
#include "org/apache/http/impl/entity/EntitySerializer.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IHttpClientConnection;
using Org::Apache::Http::IHttpConnection;
using Org::Apache::Http::IHttpResponseFactory;
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
 * Abstract client-side HTTP connection capable of transmitting and receiving data
 * using arbitrary {@link SessionInputBuffer} and {@link SessionOutputBuffer}
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 627457 $
 *
 * @since 4.0
 */
class AbstractHttpClientConnection
    : public Object
    , public IHttpClientConnection
    , public IHttpConnection
{
public:
    AbstractHttpClientConnection();

    CAR_INTERFACE_DECL()

    CARAPI IsResponseAvailable(
        /* [in] */ Int32 timeout,
        /* [out] */ Boolean* isAvailable);

    CARAPI SendRequestHeader(
        /* [in] */ IHttpRequest* request);

    CARAPI SendRequestEntity(
        /* [in] */ IHttpEntityEnclosingRequest* request);

    CARAPI Flush();

    CARAPI ReceiveResponseHeader(
        /* [out] */ IHttpResponse** httpResponse);

    CARAPI ReceiveResponseEntity(
        /* [in] */ IHttpResponse* response);

    CARAPI IsStale(
        /* [out] */ Boolean* isStale);

    CARAPI GetMetrics(
        /* [out] */ IHttpConnectionMetrics** metrics);

protected:
    virtual CARAPI AssertOpen() = 0;

    CARAPI_(AutoPtr<EntityDeserializer>) CreateEntityDeserializer();

    CARAPI_(AutoPtr<EntitySerializer>) CreateEntitySerializer();

    CARAPI_(AutoPtr<IHttpResponseFactory>) CreateHttpResponseFactory();

    CARAPI_(AutoPtr<IHttpMessageParser>) CreateResponseParser(
        /* [in] */ ISessionInputBuffer* buffer,
        /* [in] */ IHttpResponseFactory* responseFactory,
        /* [in] */ IHttpParams* params);

    CARAPI_(AutoPtr<IHttpMessageWriter>) CreateRequestWriter(
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
    AutoPtr<IHttpMessageParser> mResponseParser;
    AutoPtr<IHttpMessageWriter> mRequestWriter;
    AutoPtr<HttpConnectionMetricsImpl> mMetrics;
};

} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_ABSTRACTHTTPCLIENTCONNECTION_H__
