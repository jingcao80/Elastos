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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWCONTENTSIOTHREADCLIENT_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWCONTENTSIOTHREADCLIENT_H__
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContentsClient.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwWebResourceResponse.h"

// import android.util.ArrayMap;

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * Delegate for handling callbacks. All methods are called on the IO thread.
 *
 * You should create a separate instance for every WebContents that requires the
 * provided functionality.
 */
//@JNINamespace("android_webview")
class AwContentsIoThreadClient
    : public Object
{
public:
    //@CalledByNative
    virtual CARAPI_(Int32) GetCacheMode() = 0;

    //@CalledByNative
    virtual CARAPI_(Boolean) ShouldBlockContentUrls() = 0;

    //@CalledByNative
    virtual CARAPI_(Boolean) ShouldBlockFileUrls() = 0;

    //@CalledByNative
    virtual CARAPI_(Boolean) ShouldBlockNetworkLoads() = 0;

    //@CalledByNative
    virtual CARAPI_(Boolean) ShouldAcceptThirdPartyCookies() = 0;

    //@CalledByNative
    virtual CARAPI_(void) OnDownloadStart(
        /* [in] */ const String& url,
        /* [in] */ const String& userAgent,
        /* [in] */ const String& contentDisposition,
        /* [in] */ const String& mimeType,
        /* [in] */ Int64 contentLength) = 0;

    //@CalledByNative
    virtual CARAPI_(void) NewLoginRequest(
        /* [in] */ const String& realm,
        /* [in] */ const String& account,
        /* [in] */ const String& args) = 0;

    virtual CARAPI_(AutoPtr<AwWebResourceResponse>) ShouldInterceptRequest(
        /* [in] */ AwContentsClient::ShouldInterceptRequestParams* params) = 0;

protected:
    // Protected methods ---------------------------------------------------------------------------

    //@CalledByNative
    virtual CARAPI_(AutoPtr<AwWebResourceResponse>) ShouldInterceptRequest(
        /* [in] */ const String& url,
        /* [in] */ Boolean isMainFrame,
        /* [in] */ Boolean hasUserGesture,
        /* [in] */ const String& method,
        /* [in] */ ArrayOf<String>* requestHeaderNames,
        /* [in] */ ArrayOf<String>* requestHeaderValues);

//callback function declaration
public:
    static CARAPI_(void*) ElaAwContentsIoThreadClientCallback_Init();

private:
    static CARAPI_(Int32) GetCacheMode(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) ShouldBlockContentUrls(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) ShouldBlockFileUrls(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) ShouldBlockNetworkLoads(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) ShouldAcceptThirdPartyCookies(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) OnDownloadStart(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& url,
        /* [in] */ const String& userAgent,
        /* [in] */ const String& contentDisposition,
        /* [in] */ const String& mimeType,
        /* [in] */ Int64 contentLength);

    static CARAPI_(void) NewLoginRequest(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& realm,
        /* [in] */ const String& account,
        /* [in] */ const String& args);

    static CARAPI_(AutoPtr<IInterface>) ShouldInterceptRequest(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& url,
        /* [in] */ Boolean isMainFrame,
        /* [in] */ Boolean hasUserGesture,
        /* [in] */ const String& method,
        /* [in] */ ArrayOf<String>* requestHeaderNames,
        /* [in] */ ArrayOf<String>* requestHeaderValues);

};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWCONTENTSIOTHREADCLIENT_H__
