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

#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContentsIoThreadClient.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/api/AwContentsIoThreadClient_dec.h"

#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

//@CalledByNative
AutoPtr<AwWebResourceResponse> AwContentsIoThreadClient::ShouldInterceptRequest(
    /* [in] */ const String& url,
    /* [in] */ Boolean isMainFrame,
    /* [in] */ Boolean hasUserGesture,
    /* [in] */ const String& method,
    /* [in] */ ArrayOf<String>* requestHeaderNames,
    /* [in] */ ArrayOf<String>* requestHeaderValues)
{
    AutoPtr<AwContentsClient::ShouldInterceptRequestParams> params =
        new AwContentsClient::ShouldInterceptRequestParams();
    params->url = url;
    params->isMainFrame = isMainFrame;
    params->hasUserGesture = hasUserGesture;
    params->method = method;
    //params->requestHeaders = new ArrayMap<String, String>(requestHeaderNames.length);
    for (Int32 i = 0; i < requestHeaderNames->GetLength(); ++i) {
        //params->requestHeaders.put(requestHeaderNames[i], requestHeaderValues[i]);
        params->requestHeaders[(*requestHeaderNames)[i]] = (*requestHeaderValues)[i];
    }
    return ShouldInterceptRequest(params);
}
//callback function definition
Int32 AwContentsIoThreadClient::GetCacheMode(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwContentsIoThreadClient> mObj = (AwContentsIoThreadClient*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwContentsIoThreadClient", "AwContentsIoThreadClient::GetCacheMode, mObj is NULL");
        return 0;
    }
    return mObj->GetCacheMode();
}

Boolean AwContentsIoThreadClient::ShouldBlockContentUrls(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwContentsIoThreadClient> mObj = (AwContentsIoThreadClient*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwContentsIoThreadClient", "AwContentsIoThreadClient::ShouldBlockContentUrls, mObj is NULL");
        return FALSE;
    }
    return mObj->ShouldBlockContentUrls();
}

Boolean AwContentsIoThreadClient::ShouldBlockFileUrls(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwContentsIoThreadClient> mObj = (AwContentsIoThreadClient*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwContentsIoThreadClient", "AwContentsIoThreadClient::ShouldBlockFileUrls, mObj is NULL");
        return FALSE;
    }
    return mObj->ShouldBlockFileUrls();
}

Boolean AwContentsIoThreadClient::ShouldBlockNetworkLoads(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwContentsIoThreadClient> mObj = (AwContentsIoThreadClient*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwContentsIoThreadClient", "AwContentsIoThreadClient::ShouldBlockNetworkLoads, mObj is NULL");
        return FALSE;
    }
    return mObj->ShouldBlockNetworkLoads();
}

Boolean AwContentsIoThreadClient::ShouldAcceptThirdPartyCookies(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwContentsIoThreadClient> mObj = (AwContentsIoThreadClient*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwContentsIoThreadClient", "AwContentsIoThreadClient::ShouldAcceptThirdPartyCookies, mObj is NULL");
        return FALSE;
    }
    return mObj->ShouldAcceptThirdPartyCookies();
}

void AwContentsIoThreadClient::OnDownloadStart(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& url,
    /* [in] */ const String& userAgent,
    /* [in] */ const String& contentDisposition,
    /* [in] */ const String& mimeType,
    /* [in] */ Int64 contentLength)
{
    AutoPtr<AwContentsIoThreadClient> mObj = (AwContentsIoThreadClient*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwContentsIoThreadClient", "AwContentsIoThreadClient::OnDownloadStart, mObj is NULL");
        return;
    }
    mObj->OnDownloadStart(url, userAgent, contentDisposition, mimeType, contentLength);
}

void AwContentsIoThreadClient::NewLoginRequest(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& realm,
    /* [in] */ const String& account,
    /* [in] */ const String& args)
{
    AutoPtr<AwContentsIoThreadClient> mObj = (AwContentsIoThreadClient*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwContentsIoThreadClient", "AwContentsIoThreadClient::NewLoginRequest, mObj is NULL");
        return;
    }
    mObj->NewLoginRequest(realm, account, args);
}

AutoPtr<IInterface> AwContentsIoThreadClient::ShouldInterceptRequest(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& url,
    /* [in] */ Boolean isMainFrame,
    /* [in] */ Boolean hasUserGesture,
    /* [in] */ const String& method,
    /* [in] */ ArrayOf<String>* requestHeaderNames,
    /* [in] */ ArrayOf<String>* requestHeaderValues)
{
    AutoPtr<AwContentsIoThreadClient> mObj = (AwContentsIoThreadClient*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwContentsIoThreadClient", "AwContentsIoThreadClient::ShouldInterceptRequest, mObj is NULL");
        return NULL;
    }
    AutoPtr<AwWebResourceResponse> wrr =  mObj->ShouldInterceptRequest(url, isMainFrame, hasUserGesture, method, requestHeaderNames, requestHeaderValues);
    return TO_IINTERFACE(wrr.Get());
}


} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
