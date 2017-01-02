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

#include "elastos/droid/webkit/webview/chromium/native/content/browser/WebContentsObserverElastos.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/WebContentsObserverElastos_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewCore.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//=====================================================================
//                      WebContentsObserverElastos
//=====================================================================

WebContentsObserverElastos::WebContentsObserverElastos(
    /* [in] */ ContentViewCore* contentViewCore)
{
    ThreadUtils::AssertOnUiThread();
    mNativeWebContentsObserverElastos = NativeInit(contentViewCore->GetWebContents());
}

WebContentsObserverElastos::WebContentsObserverElastos(
    /* [in] */ WebContents* webContents)
{
    ThreadUtils::AssertOnUiThread();
    mNativeWebContentsObserverElastos = NativeInit(webContents);
}

ECode WebContentsObserverElastos::RenderProcessGone(
    /* [in] */ Boolean wasOomProtected)
{
    return NOERROR;
}

ECode WebContentsObserverElastos::DidStartLoading(
    /* [in] */ const String& url)
{
    return NOERROR;
}

ECode WebContentsObserverElastos::DidStopLoading(
    /* [in] */ const String& url)
{
    return NOERROR;
}

ECode WebContentsObserverElastos::DidFailLoad(
    /* [in] */ Boolean isProvisionalLoad,
    /* [in] */ Boolean isMainFrame,
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& description,
    /* [in] */ const String& failingUrl)
{
    return NOERROR;
}

ECode WebContentsObserverElastos::DidNavigateMainFrame(
    /* [in] */ const String& url,
    /* [in] */ const String& baseUrl,
    /* [in] */ Boolean isNavigationToDifferentPage,
    /* [in] */ Boolean isFragmentNavigation)
{
    return NOERROR;
}

ECode WebContentsObserverElastos::DidFirstVisuallyNonEmptyPaint()
{
    return NOERROR;
}

ECode WebContentsObserverElastos::DidNavigateAnyFrame(
    /* [in] */ const String& url,
    /* [in] */ const String& baseUrl,
    /* [in] */ Boolean isReload)
{
    return NOERROR;
}

ECode WebContentsObserverElastos::DidStartProvisionalLoadForFrame(
    /* [in] */ Int64 frameId,
    /* [in] */ Int64 parentFrameId,
    /* [in] */ Boolean isMainFrame,
    /* [in] */ const String& validatedUrl,
    /* [in] */ Boolean isErrorPage,
    /* [in] */ Boolean isIframeSrcdoc)
{
    return NOERROR;
}

ECode WebContentsObserverElastos::DidCommitProvisionalLoadForFrame(
    /* [in] */ Int64 frameId,
    /* [in] */ Boolean isMainFrame,
    /* [in] */ const String& url,
    /* [in] */ Int32 transitionType)
{
    return NOERROR;
}

ECode WebContentsObserverElastos::DidFinishLoad(
    /* [in] */ Int64 frameId,
    /* [in] */ const String& validatedUrl,
    /* [in] */ Boolean isMainFrame)
{
    return NOERROR;
}

ECode WebContentsObserverElastos::DocumentLoadedInFrame(
    /* [in] */ Int64 frameId)
{
    return NOERROR;
}

ECode WebContentsObserverElastos::NavigationEntryCommitted()
{
    return NOERROR;
}

ECode WebContentsObserverElastos::DidAttachInterstitialPage()
{
    return NOERROR;
}

ECode WebContentsObserverElastos::DidDetachInterstitialPage()
{
    return NOERROR;
}

ECode WebContentsObserverElastos::DidChangeThemeColor(
    /* [in] */ Int32 color)
{
    return NOERROR;
}

ECode WebContentsObserverElastos::DetachFromWebContents()
{
    if (mNativeWebContentsObserverElastos != 0) {
        NativeDestroy(mNativeWebContentsObserverElastos);
        mNativeWebContentsObserverElastos = 0;
    }

    return NOERROR;
}

Handle64 WebContentsObserverElastos::NativeInit(
    /* [in] */ WebContents* webContents)
{
    return Elastos_WebContentsObserverAndroid_nativeInit(TO_IINTERFACE(this), TO_IINTERFACE(webContents));
}

ECode WebContentsObserverElastos::NativeDestroy(
    /* [in] */ Handle64 nativeWebContentsObserverElastos)
{
    Elastos_WebContentsObserverAndroid_nativeDestroy(TO_IINTERFACE(this),
            nativeWebContentsObserverElastos);
    return NOERROR;
}

void WebContentsObserverElastos::RenderProcessGone(
    /* [in] */ IInterface* obj,
    /* [in] */ Boolean wasOomProtected)
{
    AutoPtr<WebContentsObserverElastos> mObj = (WebContentsObserverElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("WebContentsObserverElastos", "WebContentsObserverElastos::RenderProcessGone, mObj is NULL");
        return;
    }
    mObj->RenderProcessGone(wasOomProtected);
}

void WebContentsObserverElastos::DidStartLoading(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& url)
{
    AutoPtr<WebContentsObserverElastos> mObj = (WebContentsObserverElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("WebContentsObserverElastos", "WebContentsObserverElastos::DidStartLoading, mObj is NULL");
        return;
    }
    mObj->DidStartLoading(url);
}

void WebContentsObserverElastos::DidStopLoading(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& url)
{
    AutoPtr<WebContentsObserverElastos> mObj = (WebContentsObserverElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("WebContentsObserverElastos", "WebContentsObserverElastos::DidStopLoading, mObj is NULL");
        return;
    }
    mObj->DidStopLoading(url);
}

void WebContentsObserverElastos::DidFailLoad(
    /* [in] */ IInterface* obj,
    /* [in] */ Boolean isProvisionalLoad,
    /* [in] */ Boolean isMainFrame,
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& description,
    /* [in] */ const String& failingUrl)
{
    AutoPtr<WebContentsObserverElastos> mObj = (WebContentsObserverElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("WebContentsObserverElastos", "WebContentsObserverElastos::DidFailLoad, mObj is NULL");
        return;
    }
    mObj->DidFailLoad(isProvisionalLoad, isMainFrame, errorCode, description, failingUrl);
}

void WebContentsObserverElastos::DidNavigateMainFrame(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& url,
    /* [in] */ const String& baseUrl,
    /* [in] */ Boolean isNavigationToDifferentPage,
    /* [in] */ Boolean isFragmentNavigation)
{
    AutoPtr<WebContentsObserverElastos> mObj = (WebContentsObserverElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("WebContentsObserverElastos", "WebContentsObserverElastos::DidNavigateMainFrame, mObj is NULL");
        return;
    }
    mObj->DidNavigateMainFrame(url, baseUrl, isNavigationToDifferentPage, isFragmentNavigation);
}

void WebContentsObserverElastos::DidFirstVisuallyNonEmptyPaint(
    /* [in] */ IInterface* obj)
{
    AutoPtr<WebContentsObserverElastos> mObj = (WebContentsObserverElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("WebContentsObserverElastos", "WebContentsObserverElastos::DidFirstVisuallyNonEmptyPaint, mObj is NULL");
        return;
    }
    mObj->DidFirstVisuallyNonEmptyPaint();
}

void WebContentsObserverElastos::DidNavigateAnyFrame(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& url,
    /* [in] */ const String& baseUrl,
    /* [in] */ Boolean isReload)
{
    AutoPtr<WebContentsObserverElastos> mObj = (WebContentsObserverElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("WebContentsObserverElastos", "WebContentsObserverElastos::DidNavigateAnyFrame, mObj is NULL");
        return;
    }
    mObj->DidNavigateAnyFrame(url, baseUrl, isReload);
}

void WebContentsObserverElastos::DidStartProvisionalLoadForFrame(
    /* [in] */ IInterface* obj,
    /* [in] */ Int64 frameId,
    /* [in] */ Int64 parentFrameId,
    /* [in] */ Boolean isMainFrame,
    /* [in] */ const String& validatedUrl,
    /* [in] */ Boolean isErrorPage,
    /* [in] */ Boolean isIframeSrcdoc)
{
    AutoPtr<WebContentsObserverElastos> mObj = (WebContentsObserverElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("WebContentsObserverElastos", "WebContentsObserverElastos::DidStartProvisionalLoadForFrame, mObj is NULL");
        return;
    }
    mObj->DidStartProvisionalLoadForFrame(frameId, parentFrameId, isMainFrame, validatedUrl, isErrorPage, isIframeSrcdoc);
}

void WebContentsObserverElastos::DidCommitProvisionalLoadForFrame(
    /* [in] */ IInterface* obj,
    /* [in] */ Int64 frameId,
    /* [in] */ Boolean isMainFrame,
    /* [in] */ const String& url,
    /* [in] */ Int32 transitionType)
{
    AutoPtr<WebContentsObserverElastos> mObj = (WebContentsObserverElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("WebContentsObserverElastos", "WebContentsObserverElastos::DidCommitProvisionalLoadForFrame, mObj is NULL");
        return;
    }
    mObj->DidCommitProvisionalLoadForFrame(frameId, isMainFrame, url, transitionType);
}

void WebContentsObserverElastos::DidFinishLoad(
    /* [in] */ IInterface* obj,
    /* [in] */ Int64 frameId,
    /* [in] */ const String& validatedUrl,
    /* [in] */ Boolean isMainFrame)
{
    AutoPtr<WebContentsObserverElastos> mObj = (WebContentsObserverElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("WebContentsObserverElastos", "WebContentsObserverElastos::DidFinishLoad, mObj is NULL");
        return;
    }
    mObj->DidFinishLoad(frameId, validatedUrl, isMainFrame);
}

void WebContentsObserverElastos::DocumentLoadedInFrame(
    /* [in] */ IInterface* obj,
    /* [in] */ Int64 frameId)
{
    AutoPtr<WebContentsObserverElastos> mObj = (WebContentsObserverElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("WebContentsObserverElastos", "WebContentsObserverElastos::DocumentLoadedInFrame, mObj is NULL");
        return;
    }
    mObj->DocumentLoadedInFrame(frameId);
}

void WebContentsObserverElastos::NavigationEntryCommitted(
    /* [in] */ IInterface* obj)
{
    AutoPtr<WebContentsObserverElastos> mObj = (WebContentsObserverElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("WebContentsObserverElastos", "WebContentsObserverElastos::NavigationEntryCommitted, mObj is NULL");
        return;
    }
    mObj->NavigationEntryCommitted();
}

void WebContentsObserverElastos::DidAttachInterstitialPage(
    /* [in] */ IInterface* obj)
{
    AutoPtr<WebContentsObserverElastos> mObj = (WebContentsObserverElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("WebContentsObserverElastos", "WebContentsObserverElastos::DidAttachInterstitialPage, mObj is NULL");
        return;
    }
    mObj->DidAttachInterstitialPage();
}

void WebContentsObserverElastos::DidDetachInterstitialPage(
    /* [in] */ IInterface* obj)
{
    AutoPtr<WebContentsObserverElastos> mObj = (WebContentsObserverElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("WebContentsObserverElastos", "WebContentsObserverElastos::DidDetachInterstitialPage, mObj is NULL");
        return;
    }
    mObj->DidDetachInterstitialPage();
}

void WebContentsObserverElastos::DidChangeThemeColor(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 color)
{
    AutoPtr<WebContentsObserverElastos> mObj = (WebContentsObserverElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("WebContentsObserverElastos", "WebContentsObserverElastos::DidChangeThemeColor, mObj is NULL");
        return;
    }
    mObj->DidChangeThemeColor(color);
}

void WebContentsObserverElastos::DetachFromWebContents(
    /* [in] */ IInterface* obj)
{
    AutoPtr<WebContentsObserverElastos> mObj = (WebContentsObserverElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("WebContentsObserverElastos", "WebContentsObserverElastos::DetachFromWebContents, mObj is NULL");
        return;
    }
    mObj->DetachFromWebContents();
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
