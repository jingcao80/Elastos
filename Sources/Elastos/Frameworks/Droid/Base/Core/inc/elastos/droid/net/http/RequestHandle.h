
#ifndef __ELASTOS_DROID_NET_HTTP_REQUESTHANDLE_H__
#define __ELASTOS_DROID_NET_HTTP_REQUESTHANDLE_H__

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::IO::IInputStream;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * RequestHandle: handles a request session that may include multiple
 * redirects, HTTP authentication requests, etc.
 *
 * {@hide}
 */
class RequestHandle
    : public Object
    , public IRequestHandle
{
public:
    CAR_INTERFACE_DECL()

    RequestHandle();

    /**
     * Creates a new request session.
     */
    CARAPI constructor(
        /* [in] */ IRequestQueue* requestQueue,
        /* [in] */ const String& url,
        /* [in] */ IWebAddress* uri,
        /* [in] */ const String& method,
        /* [in] */ IMap* headers,
        /* [in] */ IInputStream* bodyProvider,
        /* [in] */ Int32 bodyLength,
        /* [in] */ IRequest* request);

    /**
     * Creates a new request session with a given Connection. This connection
     * is used during a synchronous load to handle this request.
     */
    CARAPI constructor(
        /* [in] */ IRequestQueue* requestQueue,
        /* [in] */ const String& url,
        /* [in] */ IWebAddress* uri,
        /* [in] */ const String& method,
        /* [in] */ IMap* headers,
        /* [in] */ IInputStream* bodyProvider,
        /* [in] */ Int32 bodyLength,
        /* [in] */ IRequest* request,
        /* [in] */ IConnection* conn);

    /**
     * Cancels this request
     */
    CARAPI Cancel();

    /**
     * Pauses the loading of this request. For example, called from the WebCore thread
     * when the plugin can take no more data.
     */
    CARAPI PauseRequest(
        /* [in] */ Boolean pause);

    /**
     * Handles SSL error(s) on the way down from the user (the user
     * has already provided their feedback).
     */
    CARAPI HandleSslErrorResponse(
        /* [in] */ Boolean proceed);

    /**
     * @return true if we've hit the max redirect count
     */
    CARAPI IsRedirectMax(
        /* [out] */ Boolean* result);

    CARAPI GetRedirectCount(
        /* [out] */ Int32* result);

    CARAPI SetRedirectCount(
        /* [in] */ Int32 count);

    /**
     * Create and queue a redirect request.
     *
     * @param redirectTo URL to redirect to
     * @param statusCode HTTP status code returned from original request
     * @param cacheHeaders Cache header for redirect URL
     * @return true if setup succeeds, false otherwise (redirect loop
     * count exceeded, body provider unable to rewind on 307 redirect)
     */
    CARAPI SetupRedirect(
        /* [in] */ const String& redirectTo,
        /* [in] */ Int32 statusCode,
        /* [in] */ IMap* cacheHeaders,
        /* [out] */ Boolean* result);

    /**
     * Create and queue an HTTP authentication-response (basic) request.
     */
    CARAPI SetupBasicAuthResponse(
        /* [in] */ Boolean isProxy,
        /* [in] */ const String& username,
        /* [in] */ const String& password);

    /**
     * Create and queue an HTTP authentication-response (digest) request.
     */
    CARAPI SetupDigestAuthResponse(
        /* [in] */ Boolean isProxy,
        /* [in] */ const String& username,
        /* [in] */ const String& password,
        /* [in] */ const String& realm,
        /* [in] */ const String& nonce,
        /* [in] */ const String& QOP,
        /* [in] */ const String& algorithm,
        /* [in] */ const String& opaque);

    /**
     * @return HTTP request method (GET, PUT, etc).
     */
    CARAPI GetMethod(
        /* [out] */ String* result);

    /**
     * @return Basic-scheme authentication response: BASE64(username:password).
     */
    static CARAPI ComputeBasicAuthResponse(
        /* [in] */ const String& username,
        /* [in] */ const String& password,
        /* [out] */ String* result);

    CARAPI WaitUntilComplete();

    CARAPI ProcessRequest();

    /**
     * @return The right authorization header (dependeing on whether it is a proxy or not).
     */
    static CARAPI AuthorizationHeader(
        /* [in] */ Boolean isProxy,
        /* [out] */ String* result);

private:
    CARAPI SetupAuthResponse();

    /**
     * @return Digest-scheme authentication response.
     */
    CARAPI ComputeDigestAuthResponse(
        /* [in] */ const String& username,
        /* [in] */ const String& password,
        /* [in] */ const String& realm,
        /* [in] */ const String& nonce,
        /* [in] */ const String& QOP,
        /* [in] */ const String& algorithm,
        /* [in] */ const String& opaque,
        /* [out] */ String* result);

    /**
     * @return Double-quoted MD5 digest.
     */
    CARAPI ComputeDigest(
        /* [in] */ const String& A1,
        /* [in] */ const String& A2,
        /* [in] */ const String& nonce,
        /* [in] */ const String& QOP,
        /* [in] */ const String& nc,
        /* [in] */ const String& cnonce,
        /* [out] */ String* result);

    /**
     * @return MD5 hash of concat(secret, ":", data).
     */
    CARAPI KD(
        /* [in] */ const String& secret,
        /* [in] */ const String& data,
        /* [out] */ String* result);

    /**
     * @return MD5 hash of param.
     */
    CARAPI H(
        /* [in] */ const String& param,
        /* [out] */ String* result);

    /**
     * @return HEX buffer representation.
     */
    CARAPI_(String) BufferToHex(
        /* [in] */ ArrayOf<Byte>* buffer);

    /**
     * Computes a random cnonce value based on the current time.
     */
    CARAPI_(String) ComputeCnonce();

    /**
     * "Double-quotes" the argument.
     */
    CARAPI_(String) DoubleQuote(
        /* [in] */ const String& param);

    /**
     * Creates and queues new request.
     */
    CARAPI CreateAndQueueNewRequest();

private:
    String mUrl;

    AutoPtr<IWebAddress> mUri;

    String mMethod;

    AutoPtr<IMap> mHeaders;

    AutoPtr<IRequestQueue> mRequestQueue;

    AutoPtr<IRequest> mRequest;

    AutoPtr<IInputStream> mBodyProvider;

    Int32 mBodyLength;

    Int32 mRedirectCount;

    // Used only with synchronous requests.
    AutoPtr<IConnection> mConnection;

    static const String AUTHORIZATION_HEADER;

    static const String PROXY_AUTHORIZATION_HEADER;
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_REQUESTHANDLE_H__
