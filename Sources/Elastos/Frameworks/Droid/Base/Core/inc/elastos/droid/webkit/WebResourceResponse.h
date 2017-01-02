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

#ifndef __ELASTOS_DROID_WEBKIT_WEBRESOURCERESPONSE_H__
#define __ELASTOS_DROID_WEBKIT_WEBRESOURCERESPONSE_H__

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IMap;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * Encapsulates a resource response. Applications can return an instance of this
 * class from {@link WebViewClient#shouldInterceptRequest} to provide a custom
 * response when the WebView requests a particular resource.
 */
class WebResourceResponse
    : public Object
    , public IWebResourceResponse
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ const String& mimeType,
        /* [in] */ const String& encoding,
        /* [in] */ IInputStream* data);

    CARAPI constructor(
        /* [in] */ const String& mimeType,
        /* [in] */ const String& encoding,
        /* [in] */ Int32 statusCode,
        /* [in] */ const String& reasonPhrase,
        /* [in] */ IMap* responseHeaders,
        /* [in] */ IInputStream* data);

    /**
     * Sets the resource response's MIME type, for example text/html.
     *
     * @param mimeType the resource response's MIME type
     */
    CARAPI SetMimeType(
        /* [in] */ const String& mimeType);

    /**
     * Gets the resource response's MIME type.
     *
     * @return the resource response's MIME type
     */
    CARAPI GetMimeType(
        /* [out] */ String* type);

    /**
     * Sets the resource response's encoding, for example UTF-8. This is used
     * to decode the data from the input stream.
     *
     * @param encoding the resource response's encoding
     */
    CARAPI SetEncoding(
        /* [in] */ const String& encoding);

    /**
     * Gets the resource response's encoding.
     *
     * @return the resource response's encoding
     */
    CARAPI GetEncoding(
        /* [out] */ String* encoding);

    /**
     * Sets the resource response's status code and reason phrase.
     *
     * @param statusCode the status code needs to be in the ranges [100, 299], [400, 599].
     *                   Causing a redirect by specifying a 3xx code is not supported.
     * @param reasonPhrase the phrase describing the status code, for example "OK". Must be non-null
     *                     and not empty.
     */
    CARAPI SetStatusCodeAndReasonPhrase(
        /* [in] */ Int32 statusCode,
        /* [in] */ const String& reasonPhrase);

    /**
     * Gets the resource response's status code.
     *
     * @return the resource response's status code.
     */
    CARAPI GetStatusCode(
        /* [out] */ Int32* statusCode);

    /**
     * Gets the description of the resource response's status code.
     *
     * @return the description of the resource response's status code.
     */
    CARAPI GetReasonPhrase(
        /* [out] */ String* reasonPhrase);

    /**
     * Sets the headers for the resource response.
     *
     * @param headers mapping of header name -> header value.
     */
    CARAPI SetResponseHeaders(
        /* [in] */ IMap* headers);

    /**
     * Gets the headers for the resource response.
     *
     * @return the headers for the resource response.
     */
    CARAPI GetResponseHeaders(
        /* [out] */ IMap** headers);

    /**
     * Sets the input stream that provides the resource respone's data. Callers
     * must implement {@link InputStream#read(byte[]) InputStream.read(byte[])}.
     *
     * @param data the input stream that provides the resource response's data
     */
    CARAPI SetData(
        /* [in] */ IInputStream* data);

    /**
     * Gets the input stream that provides the resource respone's data.
     *
     * @return the input stream that provides the resource response's data
     */
    CARAPI GetData(
        /* [out] */ IInputStream** data);

    CARAPI ToString(
        /* [out] */ String* info);

protected:
    WebResourceResponse();

protected:
    // Accessed by jni, do not rename without modifying the jni code.
    String mMimeType;
    String mEncoding;
    Int32 mStatusCode;
    String mReasonPhrase;
    AutoPtr<IMap> mResponseHeaders;
    AutoPtr<IInputStream> mInputStream;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_WEBRESOURCERESPONSE_H__
