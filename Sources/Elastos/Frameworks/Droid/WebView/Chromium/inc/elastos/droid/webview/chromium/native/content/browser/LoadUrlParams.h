// wuweizuo automatic build .h file from .java file.
// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_LOADURLPARAMS_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_LOADURLPARAMS_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content_public/Referrer.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IMap;
using Elastos::Droid::Webkit::Webview::Chromium::Content_Public::Referrer;

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
// import org.chromium.content_public.Referrer;
// import java.util.Locale;
// import java.util.Map;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
  * Holds parameters for ContentViewCore.LoadUrl. Parameters should match
  * counterparts in NavigationController::LoadURLParams, including default
  * values.
  */
// @JNINamespace("content")
class LoadUrlParams
    : public Object
{
public:
    /**
      * Creates an instance with default page transition type.
      * @param url the url to be loaded
      */
    LoadUrlParams(
        /* [in] */ const String& url);

    /**
      * Creates an instance with the given page transition type.
      * @param url the url to be loaded
      * @param transitionType the PageTransitionType constant corresponding to the load
      */
    LoadUrlParams(
        /* [in] */ const String& url,
        /* [in] */ Int32 transitionType);

    /**
      * Helper method to create a LoadUrlParams object for data url.
      * @param data Data to be loaded.
      * @param mimeType Mime type of the data.
      * @param isBase64Encoded True if the data is encoded in Base 64 format.
      */
    static CARAPI_(AutoPtr<LoadUrlParams>) CreateLoadDataParams(
        /* [in] */ const String& data,
        /* [in] */ const String& mimeType,
        /* [in] */ Boolean isBase64Encoded);

    /**
      * Helper method to create a LoadUrlParams object for data url.
      * @param data Data to be loaded.
      * @param mimeType Mime type of the data.
      * @param isBase64Encoded True if the data is encoded in Base 64 format.
      * @param charset The character set for the data. Pass null if the mime type
      *                does not require a special charset.
      */
    static CARAPI_(AutoPtr<LoadUrlParams>) CreateLoadDataParams(
        /* [in] */ const String& data,
        /* [in] */ const String& mimeType,
        /* [in] */ Boolean isBase64Encoded,
        /* [in] */ const String& charset);

    /**
      * Helper method to create a LoadUrlParams object for data url with base
      * and virtual url.
      * @param data Data to be loaded.
      * @param mimeType Mime type of the data.
      * @param isBase64Encoded True if the data is encoded in Base 64 format.
      * @param baseUrl Base url of this data load. Note that for WebView compatibility,
      *                baseUrl and historyUrl are ignored if this is a data: url.
      *                Defaults to about:blank if null.
      * @param historyUrl History url for this data load. Note that for WebView compatibility,
      *                   this is ignored if baseUrl is a data: url. Defaults to about:blank
      *                   if null.
      */
    static CARAPI_(AutoPtr<LoadUrlParams>) CreateLoadDataParamsWithBaseUrl(
        /* [in] */ const String& data,
        /* [in] */ const String& mimeType,
        /* [in] */ Boolean isBase64Encoded,
        /* [in] */ const String& baseUrl,
        /* [in] */ const String& historyUrl);

    /**
      * Helper method to create a LoadUrlParams object for data url with base
      * and virtual url.
      * @param data Data to be loaded.
      * @param mimeType Mime type of the data.
      * @param isBase64Encoded True if the data is encoded in Base 64 format.
      * @param baseUrl Base url of this data load. Note that for WebView compatibility,
      *                baseUrl and historyUrl are ignored if this is a data: url.
      *                Defaults to about:blank if null.
      * @param historyUrl History url for this data load. Note that for WebView compatibility,
      *                   this is ignored if baseUrl is a data: url. Defaults to about:blank
      *                   if null.
      * @param charset The character set for the data. Pass null if the mime type
      *                does not require a special charset.
      */
    static CARAPI_(AutoPtr<LoadUrlParams>) CreateLoadDataParamsWithBaseUrl(
        /* [in] */ const String& data,
        /* [in] */ const String& mimeType,
        /* [in] */ Boolean isBase64Encoded,
        /* [in] */ const String& baseUrl,
        /* [in] */ const String& historyUrl,
        /* [in] */ const String& charset);

    /**
      * Helper method to create a LoadUrlParams object for an HTTP POST load.
      * @param url URL of the load.
      * @param postData Post data of the load. Can be null.
      */
    static CARAPI_(AutoPtr<LoadUrlParams>) CreateLoadHttpPostParams(
        /* [in] */ const String& url,
        /* [in] */ ArrayOf<Byte>* postData);

    /**
      * Sets the url.
      */
    virtual CARAPI SetUrl(
        /* [in] */ const String& url);

    /**
      * Return the url.
      */
    virtual CARAPI_(String) GetUrl();

    /**
      * Return the base url for a data url, otherwise null.
      */
    virtual CARAPI_(String) GetBaseUrl();

    /**
      * Set load type of this load. Defaults to LOAD_TYPE_DEFAULT.
      * @param loadType One of LOAD_TYPE static constants above.
      */
    virtual CARAPI SetLoadType(
        /* [in] */ Int32 loadType);

    /**
      * Set transition type of this load. Defaults to PAGE_TRANSITION_LINK.
      * @param transitionType One of PAGE_TRANSITION static constants in ContentView.
      */
    virtual CARAPI SetTransitionType(
        /* [in] */ Int32 transitionType);

    /**
      * Return the transition type.
      */
    virtual CARAPI_(Int32) GetTransitionType();

    /**
      * @return the referrer of this load
      */
    virtual CARAPI SetReferrer(
        /* [in] */ Referrer* referrer);

    /**
      * Sets the referrer of this load.
      */
    virtual CARAPI_(AutoPtr<Referrer>) GetReferrer();

    /**
      * Set extra headers for this load.
      * @param extraHeaders Extra HTTP headers for this load. Note that these
      *                     headers will never overwrite existing ones set by Chromium.
      */
    virtual CARAPI SetExtraHeaders(
        /* [in] */ IMap* extraHeaders);

    /**
      * Return the extra headers as a map.
      */
    virtual CARAPI_(AutoPtr<IMap>) GetExtraHeaders();

    /**
      * Return the extra headers as a single String separated by "\n", or null if no extra header is
      * set. This form is suitable for passing to native
      * NavigationController::LoadUrlParams::extra_headers. This will return the headers set in an
      * exploded form through setExtraHeaders(). Embedders that work with extra headers in opaque
      * collapsed form can use the setVerbatimHeaders() / getVerbatimHeaders() instead.
      */
    virtual CARAPI_(String) GetExtraHeadersString();

    /**
      * Return the extra headers as a single String separated by "\r\n", or null if no extra header
      * is set. This form is suitable for passing to native
      * net::HttpRequestHeaders::AddHeadersFromString.
      */
    virtual CARAPI_(String) GetExtraHttpRequestHeadersString();

    /**
      * Sets the verbatim extra headers string. This is an alternative to storing the headers in
      * a map (setExtraHeaders()) for the embedders that use collapsed headers strings.
      */
    virtual CARAPI SetVerbatimHeaders(
        /* [in] */ const String& headers);

    /**
      * @return the verbatim extra headers string
      */
    virtual CARAPI_(String) GetVerbatimHeaders();

    /**
      * Set user agent override option of this load. Defaults to UA_OVERRIDE_INHERIT.
      * @param uaOption One of UA_OVERRIDE static constants above.
      */
    virtual CARAPI SetOverrideUserAgent(
        /* [in] */ Int32 uaOption);

    /**
      * Set the post data of this load. This field is ignored unless load type is
      * LOAD_TYPE_BROWSER_INITIATED_HTTP_POST.
      * @param postData Post data for this http post load.
      */
    virtual CARAPI SetPostData(
        /* [in] */ ArrayOf<Byte>* postData);

    /**
      * @return the data to be sent through POST
      */
    virtual CARAPI_(AutoPtr< ArrayOf<Byte> >) GetPostData();

    /**
      * Set the base url for data load. It is used both to resolve relative URLs
      * and when applying JavaScript's same origin policy. It is ignored unless
      * load type is LOAD_TYPE_DATA.
      * @param baseUrl The base url for this data load.
      */
    virtual CARAPI SetBaseUrlForDataUrl(
        /* [in] */ const String& baseUrl);

    /**
      * Set the virtual url for data load. It is the url displayed to the user.
      * It is ignored unless load type is LOAD_TYPE_DATA.
      * @param virtualUrl The virtual url for this data load.
      */
    virtual CARAPI SetVirtualUrlForDataUrl(
        /* [in] */ const String& virtualUrl);

    /**
      * Set whether the load should be able to access local resources. This
      * defaults to false.
      */
    virtual CARAPI SetCanLoadLocalResources(
        /* [in] */ Boolean canLoad);

    virtual CARAPI_(Int32) GetLoadUrlType();

    /**
      * @param rendererInitiated Whether or not this load was initiated from a renderer.
      */
    virtual CARAPI SetIsRendererInitiated(
        /* [in] */ Boolean rendererInitiated);

    /**
      * @return Whether or not this load was initiated from a renderer or not.
      */
    virtual CARAPI_(Boolean) GetIsRendererInitiated();

    virtual CARAPI_(Boolean) IsBaseUrlDataScheme();

    static CARAPI_(void*) ElaLoadUrlParamsCallback_Init();

private:
    CARAPI_(String) GetExtraHeadersString(
        /* [in] */ const String& delimiter,
        /* [in] */ Boolean addTerminator);

    // @SuppressWarnings("unused")
    // @CalledByNative
    static CARAPI_(void) InitializeConstants(
        /* [in] */ Int32 load_type_default,
        /* [in] */ Int32 load_type_browser_initiated_http_post,
        /* [in] */ Int32 load_type_data,
        /* [in] */ Int32 ua_override_inherit,
        /* [in] */ Int32 ua_override_false,
        /* [in] */ Int32 ua_override_true);

    /**
      * Parses |url| as a GURL on the native side, and
      * returns true if it's scheme is data:.
      */
    static CARAPI_(Boolean) NativeIsDataScheme(
        /* [in] */ const String& url);

public:
    // Should match NavigationController::LoadUrlType exactly. See comments
    // there for proper usage. initializeConstants() checks that the values
    // are correct.
    static const Int32 LOAD_TYPE_DEFAULT = 0;
    static const Int32 LOAD_TYPE_BROWSER_INITIATED_HTTP_POST = 1;
    static const Int32 LOAD_TYPE_DATA = 2;
    // Should match NavigationController::UserAgentOverrideOption exactly.
    // See comments there for proper usage. initializeConstants() checks that
    // the values are correct.
    static const Int32 UA_OVERRIDE_INHERIT = 0;
    static const Int32 UA_OVERRIDE_FALSE = 1;
    static const Int32 UA_OVERRIDE_TRUE = 2;
    // Fields with counterparts in NavigationController::LoadURLParams.
    // Package private so that ContentViewCore.loadUrl can pass them down to
    // native code. Should not be accessed directly anywhere else outside of
    // this class.
    String mUrl;
    Int32 mLoadUrlType;
    Int32 mTransitionType;
    AutoPtr<Referrer> mReferrer;
    Int32 mUaOverrideOption;
    AutoPtr< ArrayOf<Byte> > mPostData;
    String mBaseUrlForDataUrl;
    String mVirtualUrlForDataUrl;
    Boolean mCanLoadLocalResources;
    Boolean mIsRendererInitiated;

private:
    AutoPtr<IMap> mExtraHeaders;
    String mVerbatimHeaders;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_LOADURLPARAMS_H_
