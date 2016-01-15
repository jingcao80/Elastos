
#ifndef __ELASTOS_DROID_NET_HTTP_CELASTOSHTTPCLIENTHELPER_H__
#define __ELASTOS_DROID_NET_HTTP_CELASTOSHTTPCLIENTHELPER_H__

#include "_Elastos_Droid_Net_Http_CElastosHttpClientHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;

using Elastos::IO::IInputStream;
using Org::Apache::Http::Entity::IAbstractHttpEntity;
using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::IHttpRequest;

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
CarClass(CElastosHttpClientHelper)
    , public Singleton
    , public IElastosHttpClientHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    // Gzip of data shorter than this probably won't be worthwhile
    CARAPI GetDEFAULT_SYNC_MIN_GZIP_BYTES(
        /* [out] */ Int64* result);

    CARAPI SetDEFAULT_SYNC_MIN_GZIP_BYTES(
        /* [in] */ Int64 DEFAULT_SYNC_MIN_GZIP_BYTES);

    /**
     * Create a new HttpClient with reasonable defaults (which you can update).
     *
     * @param userAgent to report in your HTTP requests
     * @param context to use for caching SSL sessions (may be null for no caching)
     * @return AndroidHttpClient for you to use for all your requests.
     */
    CARAPI NewInstance(
        /* [in] */ const String& userAgent,
        /* [in] */ IContext* context,
        /* [out] */ IElastosHttpClient** result);

    /**
     * Create a new HttpClient with reasonable defaults (which you can update).
     * @param userAgent to report in your HTTP requests.
     * @return AndroidHttpClient for you to use for all your requests.
     */
    CARAPI NewInstance(
        /* [in] */ const String& userAgent,
        /* [out] */ IElastosHttpClient** result);

    /**
     * Modifies a request to indicate to the server that we would like a
     * gzipped response.  (Uses the "Accept-Encoding" HTTP header.)
     * @param request the request to modify
     * @see #getUngzippedContent
     */
    CARAPI ModifyRequestToAcceptGzipResponse(
        /* [in] */ IHttpRequest* request);

    /**
     * Gets the input stream from a response entity.  If the entity is gzipped
     * then this will get a stream over the uncompressed data.
     *
     * @param entity the entity whose content should be read
     * @return the input stream to read from
     * @throws IOException
     */
    CARAPI GetUngzippedContent(
        /* [in] */ IHttpEntity* entity,
        /* [out] */ IInputStream** result);

    /**
     * Compress data to send to server.
     * Creates a Http Entity holding the gzipped data.
     * The data will not be compressed if it is too short.
     * @param data The bytes to compress
     * @return Entity holding the data
     */
    CARAPI GetCompressedEntity(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ IContentResolver* resolver,
        /* [out] */ IAbstractHttpEntity** result);

    /**
     * Retrieves the minimum size for compressing data.
     * Shorter data will not be compressed.
     */
    CARAPI GetMinGzipSize(
        /* [in] */ IContentResolver* resolver,
        /* [out] */ Int64* result);

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
    CARAPI ParseDate(
        /* [in] */ const String& dateString,
        /* [out] */ Int64* result);
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_HTTP_CELASTOSHTTPCLIENTHELPER_H__
