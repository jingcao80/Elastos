
#include "Elastos.CoreLibrary.IO.h"
#include "HttpURLConnection.h"
#include "StringUtils.h"
#include "Math.h"

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Net {

static Int32 DEFAULT_CHUNK_LENGTH = 1024;
Boolean HttpURLConnection::sFollowRedirects = TRUE;

CAR_INTERFACE_IMPL(HttpURLConnection, URLConnection, IHttpURLConnection)

HttpURLConnection::HttpURLConnection()
    : URLConnection()
    , mMethod("GET")
    , mResponseCode(-1)
    , mInstanceFollowRedirects(sFollowRedirects)
    , mChunkLength(-1)
    , mFixedContentLength(-1)
    , mFixedContentLengthInt64(-1)
{}

ECode HttpURLConnection::constructor(
    /* [in] */ IURL* url)
{
    return URLConnection::constructor(url);
}

ECode HttpURLConnection::GetErrorStream(
    /* [out] */ IInputStream** stream)
{
    VALIDATE_NOT_NULL(stream)
    *stream = NULL;

    return NOERROR;
}

Boolean HttpURLConnection::GetFollowRedirects()
{
    return sFollowRedirects;
}

//        public java.security.Permission getPermission();

ECode HttpURLConnection::GetRequestMethod(
    /* [out] */ String* method)
{
    VALIDATE_NOT_NULL(method)
    assert(method != NULL);
    *method = mMethod;

    return NOERROR;
}

ECode HttpURLConnection::GetResponseCode(
    /* [out] */ Int32* responseCode)
{
    VALIDATE_NOT_NULL(responseCode)

    // Call getInputStream() first since getHeaderField() doesn't return
    // exceptions
    AutoPtr<IInputStream> is;
    FAIL_RETURN(GetInputStream((IInputStream**)&is));
    String response;
    GetHeaderField(0, &response);
    if (response.IsNull()) {
        *responseCode = -1;
        return NOERROR;
    }
    response = response.Trim();
    Int32 mark = response.IndexOf(" ") + 1;
    if (mark == 0) {
        *responseCode = -1;
        return NOERROR;
    }
    Int32 last = mark + 3;
    if (last > response.GetLength()) {
        last = response.GetLength();
    }
    mResponseCode = StringUtils::ParseInt32(response.Substring(mark, last));
    if ((last + 1) <= response.GetLength()) {
        mResponseMessage = response.Substring(last + 1);
    }
    *responseCode = mResponseCode;
    return NOERROR;
}

ECode HttpURLConnection::GetResponseMessage(
    /* [out] */ String* message)
{
    VALIDATE_NOT_NULL(message)

    if (!mResponseMessage.IsNull()) {
        *message = mResponseMessage;
        return NOERROR;
    }
    Int32 code;
    FAIL_RETURN(GetResponseCode(&code));
    *message = mResponseMessage;
    return NOERROR;
}

ECode HttpURLConnection::SetFollowRedirects(
    /* [in] */ Boolean followRedirects)
{
//    SecurityManager security = System.getSecurityManager();
//    if (security != null) {
//        security.checkSetFactory();
//    }
    sFollowRedirects = followRedirects;
    return NOERROR;
}

ECode HttpURLConnection::SetRequestMethod(
    /* [in] */ const String& method)
{
    if (mConnected) {
//        throw new ProtocolException("Connection already established");
        return E_PROTOCOL_EXCEPTION;
    }
    //Todo:
    // for (String permittedUserMethod : HttpURLConnectionImpl.PERMITTED_USER_METHODS)
    static String PERMITTED_USER_METHODS[] = {
        String("OPTIONS"),
        String("GET"),
        String("HEAD"),
        String("POST"),
        String("PUT"),
        String("DELETE"),
        String("TRACE")
    };
    for (Int32 i = 0; i < 7; i++) {
        String permittedUserMethod = PERMITTED_USER_METHODS[i];
        if (method.Equals(permittedUserMethod)) {
            // if there is a supported method that matches the desired
            // method, then set the current method and return
            mMethod = permittedUserMethod;
            return NOERROR;
        }
    }
    // if none matches, then throw ProtocolException
    return E_PROTOCOL_EXCEPTION;
}

ECode HttpURLConnection::GetContentEncoding(
    /* [out] */ String* encoding)
{
    return URLConnection::GetContentEncoding(encoding);
}

ECode HttpURLConnection::GetInstanceFollowRedirects(
    /* [out] */ Boolean* followRedirects)
{
    VALIDATE_NOT_NULL(followRedirects)
    *followRedirects = mInstanceFollowRedirects;

    return NOERROR;
}

ECode HttpURLConnection::SetInstanceFollowRedirects(
    /* [in] */ Boolean followRedirects)
{
    mInstanceFollowRedirects = followRedirects;

    return NOERROR;
}

ECode HttpURLConnection::GetHeaderFieldDate(
    /* [in] */ const String& field,
    /* [in] */ Int64 defaultValue,
    /* [out] */ Int64* value)
{
    return URLConnection::GetHeaderFieldDate(field, defaultValue, value);
}

ECode HttpURLConnection::SetFixedLengthStreamingMode(
    /* [in] */ Int64 contentLength)
{
    if (mConnected) {
//        throw new IllegalStateException("Already connected");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mChunkLength > 0) {
//        throw new IllegalStateException("Already in chunked mode");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (contentLength < 0) {
//        throw new IllegalArgumentException("contentLength < 0"
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mFixedContentLength = contentLength;

    mFixedContentLength = (Int32) Elastos::Core::Math::Min(
        (Int32)contentLength, Elastos::Core::Math::INT32_MAX_VALUE);
    mFixedContentLengthInt64 = contentLength;
    return NOERROR;
}

ECode HttpURLConnection::SetFixedLengthStreamingMode(
    /* [in] */ Int32 contentLength)
{
    return SetFixedLengthStreamingMode((Int64)contentLength);
}

ECode HttpURLConnection::SetChunkedStreamingMode(
    /* [in] */ Int32 chunkLength)
{
    if (mConnected) {
//        throw new IllegalStateException("Already connected");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mFixedContentLength > 0) {
//        throw new IllegalStateException("Already in chunked mode");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (chunkLength < 0) {
//        throw new IllegalArgumentException("contentLength < 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (chunkLength <= 0) {
        mChunkLength = DEFAULT_CHUNK_LENGTH;
    }
    else {
        mChunkLength = chunkLength;
    }

    return NOERROR;
}

} // namespace Net
} // namespace Elastos
