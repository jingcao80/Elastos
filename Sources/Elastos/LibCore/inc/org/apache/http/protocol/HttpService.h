
#ifndef __ORG_APACHE_HTTP_PROTOCOL_HTTPSERVICE_H_
#define __ORG_APACHE_HTTP_PROTOCOL_HTTPSERVICE_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IHttpServerConnection;
using Org::Apache::Http::IHttpResponseFactory;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::IConnectionReuseStrategy;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

class HttpService
    : public Object
    , public IHttpService
{
public:
    CAR_INTERFACE_DECL()

    virtual ~HttpService() {}

    CARAPI SetHttpProcessor(
        /* [in] */ IHttpProcessor* processor);

    CARAPI SetConnReuseStrategy(
        /* [in] */ IConnectionReuseStrategy* connStrategy);

    CARAPI SetResponseFactory(
        /* [in] */ IHttpResponseFactory* responseFactory);

    CARAPI SetHandlerResolver(
        /* [in] */ IHttpRequestHandlerResolver* handlerResolver);

    CARAPI SetExpectationVerifier(
        /* [in] */ IHttpExpectationVerifier* expectationVerifier);

    CARAPI GetParams(
        /* [out] */ IHttpParams** params);

    CARAPI SetParams(
        /* [in] */ IHttpParams* params);

    CARAPI HandleRequest(
        /* [in] */ IHttpServerConnection* conn,
        /* [in] */ IHttpContext* context);

protected:
    CARAPI Init(
        /* [in] */ IHttpProcessor* processor,
        /* [in] */ IConnectionReuseStrategy* connStrategy,
        /* [in] */ IHttpResponseFactory* responseFactory);

    CARAPI HandleException(
        /* [in] */ ECode ex,
        /* [in] */ IHttpResponse* response);

    CARAPI DoService(
        /* [in] */ IHttpRequest* request,
        /* [in] */ IHttpResponse* response,
        /* [in] */ IHttpContext* context);

private:
    AutoPtr<IHttpParams> mParams;
    AutoPtr<IHttpProcessor> mProcessor;
    AutoPtr<IHttpRequestHandlerResolver> mHandlerResolver;
    AutoPtr<IConnectionReuseStrategy> mConnStrategy;
    AutoPtr<IHttpResponseFactory> mResponseFactory;
    AutoPtr<IHttpExpectationVerifier> mExpectationVerifier;
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_HTTPSERVICE_H_
