
#ifndef __ELASTOS_DROID_NET_HTTP_REQUEST_H__
#define __ELASTOS_DROID_NET_HTTP_REQUEST_H__

#include "Elastos.Droid.Net.h"
#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::IO::IInputStream;
using Elastos::Utility::IMap;

using Org::Apache::Http::IHttpHost;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::IHttpRequestInterceptor;
using Org::Apache::Http::Message::IBasicHttpRequest;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * Represents an HTTP request for a given host.
 *
 * {@hide}
 */
class Request
    : public Object
    , public IRequest
{
public:
    CAR_INTERFACE_DECL()

    Request();

    /**
     * Instantiates a new Request.
     * @param method GET/POST/PUT
     * @param host The server that will handle this request
     * @param path path part of URI
     * @param bodyProvider InputStream providing HTTP body, null if none
     * @param bodyLength length of body, must be 0 if bodyProvider is null
     * @param eventHandler request will make progress callbacks on
     * this interface
     * @param headers reqeust headers
     */
    CARAPI constructor(
        /* [in] */ const String& method,
        /* [in] */ IHttpHost* host,
        /* [in] */ IHttpHost* proxyHost,
        /* [in] */ const String& path,
        /* [in] */ IInputStream* bodyProvider,
        /* [in] */ Int32 bodyLength,
        /* [in] */ IEventHandler* eventHandler,
        /* [in] */ IMap* headers);

    /**
     * @param pause True if the load should be paused.
     */
    CARAPI SetLoadingPaused(
        /* [in] */ Boolean pause);

    /**
     * @param connection Request served by this connection
     */
    CARAPI SetConnection(
        /* [in] */ IConnection* connection);

    /* package */
    CARAPI GetEventHandler(
        /* [out] */ IEventHandler** result);

    /**
     * Add header represented by given pair to request.  Header will
     * be formatted in request as "name: value\r\n".
     * @param name of header
     * @param value of header
     */
    CARAPI AddHeader(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    /**
     * Add all headers in given map to this request.  This is a helper
     * method: it calls addHeader for each pair in the map.
     */
    CARAPI AddHeaders(
        /* [in] */ IMap* headers);

    /**
     * Send the request line and headers
     */
    CARAPI SendRequest(
        /* [in] */ IElastosHttpClientConnection* httpClientConnection);

    /**
     * Receive a single http response.
     *
     * @param httpClientConnection the request to receive the response for.
     */
    CARAPI ReadResponse(
        /* [in] */ IElastosHttpClientConnection* httpClientConnection);

    /**
     * Data will not be sent to or received from server after cancel()
     * call.  Does not close connection--use close() below for that.
     *
     * Called by RequestHandle from non-network thread
     */
    CARAPI Cancel();

    CARAPI GetHostPort(
        /* [out] */ String* result);

    CARAPI GetUri(
        /* [out] */ String* result);

    /**
     * for debugging
     */
    CARAPI ToString(
        /* [out] */ String* result);

    /**
     * If this request has been sent once and failed, it must be reset
     * before it can be sent again.
     */
    CARAPI Reset();

    /**
     * Pause thread request completes.  Used for synchronous requests,
     * and testing
     */
    CARAPI WaitUntilComplete();

    CARAPI Complete();

    /**
     * Handles SSL error(s) on the way down from the user (the user
     * has already provided their feedback).
     */
    CARAPI HandleSslErrorResponse(
        /* [in] */ Boolean proceed);

    /**
     * Helper: calls error() on eventhandler with appropriate message
     * This should not be called before the mConnection is set.
     */
    CARAPI Error(
        /* [in] */ Int32 errorId,
        /* [in] */ Int32 resourceId);

private:
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
    static CARAPI_(Boolean) CanResponseHaveBody(
        /* [in] */ IHttpRequest* request,
        /* [in] */ Int32 status);

    /**
     * Supply an InputStream that provides the body of a request.  It's
     * not great that the caller must also provide the length of the data
     * returned by that InputStream, but the client needs to know up
     * front, and I'm not sure how to get this out of the InputStream
     * itself without a costly readthrough.  I'm not sure skip() would
     * do what we want.  If you know a better way, please let me know.
     */
    CARAPI SetBodyProvider(
        /* [in] */ IInputStream* bodyProvider,
        /* [in] */ Int32 bodyLength);

    static CARAPI_(AutoPtr<IHttpRequestInterceptor>) InitRequestContentProcessor();

public:
    /** The eventhandler to call as the request progresses */
    AutoPtr<IEventHandler> mEventHandler;

    /** The Apache http request */
    AutoPtr<IBasicHttpRequest> mHttpRequest;

    /** The path component of this request */
    String mPath;

    /** Host serving this request */
    AutoPtr<IHttpHost> mHost;

    /** Set if I'm using a proxy server */
    AutoPtr<IHttpHost> mProxyHost;

    /** True if request has been cancelled */
    /* volatile */ Boolean mCancelled;

    Int32 mFailCount;

private:
    /**
     * Processor used to set content-length and transfer-encoding
     * headers.
     */
    static AutoPtr<IHttpRequestInterceptor> mRequestContentProcessor;

    AutoPtr<IConnection> mConnection;

    // is http/1.1 feature.
    Int32 mReceivedBytes;

    AutoPtr<IInputStream> mBodyProvider;

    Int32 mBodyLength;

    static const String HOST_HEADER;

    static const String ACCEPT_ENCODING_HEADER;

    static const String CONTENT_LENGTH_HEADER;

    /* Used to synchronize waitUntilComplete() requests */
    /* const */ AutoPtr<IObject> mClientResource;

    /** True if loading should be paused **/
    Boolean mLoadingPaused;
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_REQUEST_H__
