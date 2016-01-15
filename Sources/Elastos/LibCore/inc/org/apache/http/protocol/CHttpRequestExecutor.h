
#ifndef __ORG_APACHE_HTTP_PROTOCOL_CHTTPREQUESTEXECUTOR_H_
#define __ORG_APACHE_HTTP_PROTOCOL_CHTTPREQUESTEXECUTOR_H_

#include "_Org_Apache_Http_Protocol_CHttpRequestExecutor.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IHttpClientConnection;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::IHttpResponse;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

/**
 * Sends HTTP requests and receives the responses.
 * Takes care of request preprocessing and response postprocessing
 * by the respective interceptors.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 576073 $
 *
 * @since 4.0
 */
CarClass(CHttpRequestExecutor)
    , public Object
    , public IHttpRequestExecutor
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Decide whether a response comes with an entity.
     * The implementation in this class is based on RFC 2616.
     * Unknown methods and response codes are supposed to
     * indicate responses with an entity.
     * <br/>
     * Derived executors can override this method to handle
     * methods and response codes not specified in RFC 2616.
     *
     * @param request   the request, to obtain the executed method
     * @param response  the response, to obtain the status code
     */
    CARAPI CanResponseHaveBody(
        /* [in] */ IHttpRequest* request,
        /* [in] */ IHttpResponse* response,
        /* [out] */ Boolean* result);

    /**
     * Synchronously send a request and obtain the response.
     *
     * @param request   the request to send. It will be preprocessed.
     * @param conn      the open connection over which to send
     *
     * @return  the response to the request, postprocessed
     *
     * @throws HttpException      in case of a protocol or processing problem
     * @throws IOException        in case of an I/O problem
     */
    CARAPI Execute(
        /* [in] */ IHttpRequest* request,
        /* [in] */ IHttpClientConnection* conn,
        /* [in] */ IHttpContext* context,
        /* [out] */ IHttpResponse** response);

    /**
     * Prepare a request for sending.
     *
     * @param request   the request to prepare
     * @param processor the processor to use
     * @param context   the context for sending the request
     *
     * @throws HttpException      in case of a protocol or processing problem
     * @throws IOException        in case of an I/O problem
     */
    CARAPI PreProcess(
        /* [in] */ IHttpRequest* request,
        /* [in] */ IHttpProcessor* processor,
        /* [in] */ IHttpContext* context);

    /**
     * Send a request over a connection.
     * This method also handles the expect-continue handshake if necessary.
     * If it does not have to handle an expect-continue handshake, it will
     * not use the connection for reading or anything else that depends on
     * data coming in over the connection.
     *
     * @param request   the request to send, already
     *                  {@link #preProcess preprocessed}
     * @param conn      the connection over which to send the request,
     *                  already established
     * @param context   the context for sending the request
     *
     * @return  a terminal response received as part of an expect-continue
     *          handshake, or
     *          <code>null</code> if the expect-continue handshake is not used
     *
     * @throws HttpException      in case of a protocol or processing problem
     * @throws IOException        in case of an I/O problem
     */
    CARAPI DoSendRequest(
        /* [in] */ IHttpRequest* request,
        /* [in] */ IHttpClientConnection* conn,
        /* [in] */ IHttpContext* context,
        /* [out] */ IHttpResponse** response);

    /**
     * Wait for and receive a response.
     * This method will automatically ignore intermediate responses
     * with status code 1xx.
     *
     * @param request   the request for which to obtain the response
     * @param conn      the connection over which the request was sent
     * @param context   the context for receiving the response
     *
     * @return  the final response, not yet post-processed
     *
     * @throws HttpException      in case of a protocol or processing problem
     * @throws IOException        in case of an I/O problem
     */
    CARAPI DoReceiveResponse(
        /* [in] */ IHttpRequest* request,
        /* [in] */ IHttpClientConnection* conn,
        /* [in] */ IHttpContext* context,
        /* [out] */ IHttpResponse** response);

    /**
     * Finish a response.
     * This includes post-processing of the response object.
     * It does <i>not</i> read the response entity, if any.
     * It does <i>not</i> allow for immediate re-use of the
     * connection over which the response is coming in.
     *
     * @param response  the response object to finish
     * @param processor the processor to use
     * @param context   the context for post-processing the response
     *
     * @throws HttpException      in case of a protocol or processing problem
     * @throws IOException        in case of an I/O problem
     */
    CARAPI PostProcess(
        /* [in] */ IHttpResponse* response,
        /* [in] */ IHttpProcessor* processor,
        /* [in] */ IHttpContext* context);

    CARAPI constructor();

};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_CHTTPREQUESTEXECUTOR_H_
