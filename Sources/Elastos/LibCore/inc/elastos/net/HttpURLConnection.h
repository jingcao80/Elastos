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

#ifndef __ELASTOS_NET_HTTPURLCONNECTION_H__
#define __ELASTOS_NET_HTTPURLCONNECTION_H__

#include "URLConnection.h"

namespace Elastos {
namespace Net {


class HttpURLConnection
    : public URLConnection
    , public IHttpURLConnection
{
public:
    CAR_INTERFACE_DECL()

    virtual CARAPI Disconnect() = 0;

    virtual CARAPI GetErrorStream(
        /* [out] */ IInputStream** stream);

    static CARAPI_(Boolean) GetFollowRedirects();

//        public java.security.Permission getPermission();

    virtual CARAPI GetRequestMethod(
        /* [out] */ String* method);

    virtual CARAPI GetResponseCode(
        /* [out] */ Int32* responseCode);

    virtual CARAPI GetResponseMessage(
        /* [out] */ String* message);

    static CARAPI SetFollowRedirects(
        /* [in] */ Boolean followRedirects);

    CARAPI SetRequestMethod(
        /* [in] */ const String& method);

    virtual CARAPI UsingProxy(
        /* [out] */ Boolean* usingProxy) = 0;

    CARAPI GetContentEncoding(
        /* [out] */ String* encoding);

    CARAPI GetInstanceFollowRedirects(
        /* [out] */ Boolean* followRedirects);

    CARAPI SetInstanceFollowRedirects(
        /* [in] */ Boolean followRedirects);

    CARAPI GetHeaderFieldDate(
        /* [in] */ const String& field,
        /* [in] */ Int64 defaultValue,
        /* [out] */ Int64* value);

    CARAPI SetFixedLengthStreamingMode(
        /* [in] */ Int64 contentLength);

    CARAPI SetFixedLengthStreamingMode(
        /* [in] */ Int32 contentLength);

    CARAPI SetChunkedStreamingMode(
        /* [in] */ Int32 chunkLength);

protected:
    HttpURLConnection();

    CARAPI constructor(
        /* [in] */ IURL* url);

protected:
    String mMethod;

    Int32 mResponseCode;

    String mResponseMessage;

    Boolean mInstanceFollowRedirects;

    Int32 mChunkLength;

    Int32 mFixedContentLength;
    Int32 mFixedContentLengthInt64;

private:
    static Boolean sFollowRedirects;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_HTTPURLCONNECTION_H__
