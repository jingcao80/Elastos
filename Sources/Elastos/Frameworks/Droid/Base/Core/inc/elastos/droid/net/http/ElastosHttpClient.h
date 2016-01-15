
#ifndef __ELASTOS_DROID_NET_HTTP_ELASTOSHTTPCLIENT_H__
#define __ELASTOS_DROID_NET_HTTP_ELASTOSHTTPCLIENT_H__

#include <Elastos.CoreLibrary.Apache.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
// TODO: Waiting for DefaultHttpClient
// #include <org/apache/http/impl/client/DefaultHttpClient.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;

using Elastos::IO::IInputStream;
using Org::Apache::Http::Client::IHttpClient;
using Org::Apache::Http::Client::IResponseHandler;
using Org::Apache::Http::Client::Methods::IHttpUriRequest;
using Org::Apache::Http::Conn::IClientConnectionManager;
using Org::Apache::Http::Entity::IAbstractHttpEntity;
using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::IHttpHost;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::IHttpRequestInterceptor;
using Org::Apache::Http::IHttpResponse;
// using Org::Apache::Http::Impl::Client::DefaultHttpClient;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Protocol::IBasicHttpProcessor;
using Org::Apache::Http::Protocol::IHttpContext;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * Implementation of the Apache {@link DefaultHttpClient} that is configured with
 * reasonable default settings and registered schemes for Android.
 * Don't create this directly, use the {@link #newInstance} factory method.
 *
 * <p>This client processes cookies but does not retain them by default.
 * To retain cookies, simply add a cookie store to the HttpContext:</p>
 *
 * <pre>context.setAttribute(ClientContext.COOKIE_STORE, cookieStore);</pre>
 */
class ElastosHttpClient
    : public Object
    , public IHttpClient
    , public IElastosHttpClient
{
private:
    /**
    * Logging tag and level.
    */
    class LoggingConfiguration
        : public Object
    {
    private:
        LoggingConfiguration(
            /* [in] */ const String& tag,
            /* [in] */ Int32 level);

        /**
        * Returns true if logging is turned on for this configuration.
        */
        CARAPI_(Boolean) IsLoggable();

        /**
        * Prints a message using this configuration.
        */
        CARAPI Println(
            /* [in] */ const String& message);

        const String TAG;

        const Int32 LEVEL;

        friend class ElastosHttpClient;
    };

    /**
    * Logs cURL commands equivalent to requests.
    */
    class CurlLogger
        : public Object
        , public IHttpRequestInterceptor
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI Process(
            /* [in] */ IHttpRequest* request,
            /* [in] */ IHttpContext* context);

    };

    class InnerSub_HttpRequestInterceptor
        : public Object
        , public IHttpRequestInterceptor
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI Process(
            /* [in] */ IHttpRequest* request,
            /* [in] */ IHttpContext* context);
    };

    class InnerSub_DefaultHttpClient
#if 0 // TODO: Waiting for DefaultHttpClient
        : public DefaultHttpClient
#else
        : public Object
#endif
    {
    public:
        // @Override
        CARAPI CreateHttpProcessor(
            /* [out] */ IBasicHttpProcessor** processor);

        CARAPI CreateHttpContext(
            /* [out] */ IHttpContext** context);
    };

public:
    CAR_INTERFACE_DECL()

    ElastosHttpClient();

    // @Override
    // finalize();
    virtual ~ElastosHttpClient();

    // Gzip of data shorter than this probably won't be worthwhile
    static CARAPI GetDEFAULT_SYNC_MIN_GZIP_BYTES(
        /* [out] */ Int64* result);

    static CARAPI SetDEFAULT_SYNC_MIN_GZIP_BYTES(
        /* [in] */ Int64 DEFAULT_SYNC_MIN_GZIP_BYTES);

    /**
     * Create a new HttpClient with reasonable defaults (which you can update).
     *
     * @param userAgent to report in your HTTP requests
     * @param context to use for caching SSL sessions (may be null for no caching)
     * @return AndroidHttpClient for you to use for all your requests.
     */
    static CARAPI NewInstance(
        /* [in] */ const String& userAgent,
        /* [in] */ IContext* context,
        /* [out] */ IElastosHttpClient** result);

    /**
     * Create a new HttpClient with reasonable defaults (which you can update).
     * @param userAgent to report in your HTTP requests.
     * @return AndroidHttpClient for you to use for all your requests.
     */
    static CARAPI NewInstance(
        /* [in] */ const String& userAgent,
        /* [out] */ IElastosHttpClient** result);


    /**
     * Modifies a request to indicate to the server that we would like a
     * gzipped response.  (Uses the "Accept-Encoding" HTTP header.)
     * @param request the request to modify
     * @see #getUngzippedContent
     */
    static CARAPI ModifyRequestToAcceptGzipResponse(
        /* [in] */ IHttpRequest* request);

    /**
     * Gets the input stream from a response entity.  If the entity is gzipped
     * then this will get a stream over the uncompressed data.
     *
     * @param entity the entity whose content should be read
     * @return the input stream to read from
     * @throws IOException
     */
    static CARAPI GetUngzippedContent(
        /* [in] */ IHttpEntity* entity,
        /* [out] */ IInputStream** result);

    /**
     * Release resources associated with this client.  You must call this,
     * or significant resources (sockets and memory) may be leaked.
     */
    CARAPI Close();

    CARAPI GetParams(
        /* [out] */ IHttpParams** result);

    CARAPI GetConnectionManager(
        /* [out] */ IClientConnectionManager** result);

    CARAPI Execute(
        /* [in] */ IHttpUriRequest* request,
        /* [out] */ IHttpResponse** result);

    CARAPI Execute(
        /* [in] */ IHttpUriRequest* request,
        /* [in] */ IHttpContext* context,
        /* [out] */ IHttpResponse** result);

    CARAPI Execute(
        /* [in] */ IHttpHost* target,
        /* [in] */ IHttpRequest* request,
        /* [out] */ IHttpResponse** result);

    CARAPI Execute(
        /* [in] */ IHttpHost* target,
        /* [in] */ IHttpRequest* request,
        /* [in] */ IHttpContext* context,
        /* [out] */ IHttpResponse** result);

    CARAPI Execute(
        /* [in] */ IHttpUriRequest* request,
        /* [in] */ IResponseHandler* responseHandler,
        /* [out] */ IInterface** result);

    CARAPI Execute(
        /* [in] */ IHttpUriRequest* request,
        /* [in] */ IResponseHandler* responseHandler,
        /* [in] */ IHttpContext* context,
        /* [out] */ IInterface** result);

    CARAPI Execute(
        /* [in] */ IHttpHost* target,
        /* [in] */ IHttpRequest* request,
        /* [in] */ IResponseHandler* responseHandler,
        /* [out] */ IInterface** result);

    CARAPI Execute(
        /* [in] */ IHttpHost* target,
        /* [in] */ IHttpRequest* request,
        /* [in] */ IResponseHandler* responseHandler,
        /* [in] */ IHttpContext* context,
        /* [out] */ IInterface** result);

    /**
     * Compress data to send to server.
     * Creates a Http Entity holding the gzipped data.
     * The data will not be compressed if it is too short.
     * @param data The bytes to compress
     * @return Entity holding the data
     */
    static CARAPI GetCompressedEntity(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ IContentResolver* resolver,
        /* [out] */ IAbstractHttpEntity** result);

    /**
     * Retrieves the minimum size for compressing data.
     * Shorter data will not be compressed.
     */
    static CARAPI GetMinGzipSize(
        /* [in] */ IContentResolver* resolver,
        /* [out] */ Int64* result);

    /**
     * Enables cURL request logging for this client.
     *
     * @param name to log messages with
     * @param level at which to log messages (see {@link android.util.Log})
     */
    CARAPI EnableCurlLogging(
        /* [in] */ const String& name,
        /* [in] */ Int32 level);

    /**
     * Disables cURL logging for this client.
     */
    CARAPI DisableCurlLogging();

    /**
     * Returns the date of the given HTTP date string. This method can identify
     * and parse the date formats emitted by common HTTP servers, such as
     * <a href="http://www.ietf.org/rfc/rfc0822.txt">RFC 822</a>,
     * <a href="http://www.ietf.org/rfc/rfc0850.txt">RFC 850</a>,
     * <a href="http://www.ietf.org/rfc/rfc1036.txt">RFC 1036</a>,
     * <a href="http://www.ietf.org/rfc/rfc1123.txt">RFC 1123</a> and
     * <a href="http://www.opengroup.org/onlinepubs/007908799/xsh/asctime.html">ANSI
     * C's asctime()</a>.
     *
     * @return the number of milliseconds since Jan. 1, 1970, midnight GMT.
     * @throws IllegalArgumentException if {@code dateString} is not a date or
     *     of an unsupported format.
     */
    static CARAPI ParseDate(
        /* [in] */ const String& dateString,
        /* [out] */ Int64* result);

    // Gzip of data shorter than this probably won't be worthwhile
    static Int64 DEFAULT_SYNC_MIN_GZIP_BYTES;

private:
    static CARAPI_(AutoPtr<ArrayOf<String> >) InitTextContentTypes();

    static CARAPI_(AutoPtr<IHttpRequestInterceptor>) InitThreadCheckInterceptor();

    CARAPI constructor(
        /* [in] */ IClientConnectionManager* ccm,
        /* [in] */ IHttpParams* params);

    /**
     * Generates a cURL command equivalent to the given request.
     */
    static CARAPI_(String) ToCurl(
        /* [in] */ IHttpUriRequest* request,
        /* [in] */ Boolean logAuthToken);

    static CARAPI_(Boolean) IsBinaryContent(
        /* [in] */ IHttpUriRequest* request);

private:
    // Default connection and socket timeout of 60 seconds.  Tweak to taste.
    static const Int32 SOCKET_OPERATION_TIMEOUT;

    static const String TAG;

    static AutoPtr<ArrayOf<String> > sTextContentTypes;

    AutoPtr<IHttpClient> mDelegate;

    Boolean mLeakedException;

    /** cURL logging configuration. */
    /* volatile */ AutoPtr<LoggingConfiguration> mCurlConfiguration;

    /** Interceptor throws an exception if the executing thread is blocked */
    static const AutoPtr<IHttpRequestInterceptor> sThreadCheckInterceptor;
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_ELASTOSHTTPCLIENT_H__
