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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_WEBCONTENTS_WEBCONTENTSIMPL_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_WEBCONTENTS_WEBCONTENTSIMPL_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content_public/browser/NavigationController.h"
#include "elastos/droid/webkit/webview/chromium/native/content_public/browser/WebContents.h"

using Elastos::Droid::Webkit::Webview::Chromium::Content_Public::Browser::NavigationController;
using Elastos::Droid::Webkit::Webview::Chromium::Content_Public::Browser::WebContents;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Webcontents {

/**
 * The WebContentsImpl Java wrapper to allow communicating with the native WebContentsImpl
 * object.
 */
//@JNINamespace("content")
//TODO(tedchoc): Remove the package restriction once this class moves to a non-public content
//               package whose visibility will be enforced via DEPS.
/* package */
class WebContentsImpl
    : public WebContents
{
public:
    //@Override
    CARAPI_(AutoPtr<NavigationController>) GetNavigationController();

    //@Override
    CARAPI_(String) GetTitle();

    //@Override
    CARAPI_(String) GetVisibleUrl();

    //@Override
    CARAPI_(void) Stop();

    static CARAPI_(void*) ElaWebContentsImplCallback_Init();

private:
    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ HANDLE nativeWebContentsAndroid,
        /* [in] */ IInterface* navigationController);

    static CARAPI_(void) Destroy(
        /* [in] */ IInterface* obj);

    static CARAPI_(HANDLE) GetNativePointer(
        /* [in] */ IInterface* obj);

    WebContentsImpl(
        /* [in] */ HANDLE nativeWebContentsAndroid,
        /* [in] */ NavigationController* navigationController);

    //@CalledByNative
    static CARAPI_(AutoPtr<WebContentsImpl>) Create(
        /* [in] */ HANDLE nativeWebContentsAndroid,
        /* [in] */ NavigationController* navigationController);

    //@CalledByNative
    CARAPI_(void) Destroy();

    //@CalledByNative
    CARAPI_(HANDLE) GetNativePointer();

    CARAPI_(String) NativeGetTitle(
        /* [in] */ HANDLE nativeWebContentsAndroid);

    CARAPI_(String) NativeGetVisibleURL(
        /* [in] */ HANDLE nativeWebContentsAndroid);

    CARAPI_(void) NativeStop(
        /* [in] */ HANDLE nativeWebContentsAndroid);

private:
    HANDLE mNativeWebContentsAndroid;
    AutoPtr<NavigationController> mNavigationController;
};

} // namespace Webcontents
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_WEBCONTENTS_WEBCONTENTSIMPL_H__
