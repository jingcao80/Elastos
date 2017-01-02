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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_FRAMEHOST_NAVIGATIONCONTROLLERIMPL_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_FRAMEHOST_NAVIGATIONCONTROLLERIMPL_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content_public/browser/NavigationController.h"

using Elastos::Droid::Webkit::Webview::Chromium::Content_Public::Browser::NavigationController;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Framehost {

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
// import org.chromium.content_public.browser.NavigationController;

/**
 * The NavigationControllerImpl Java wrapper to allow communicating with the native
 * NavigationControllerImpl object.
 */
//@JNINamespace("content")
// TODO(tedchoc): Remove the package restriction once this class moves to a non-public content
//                package whose visibility will be enforced via DEPS.
/* package */
class NavigationControllerImpl
    : public NavigationController
{
public:
    //@Override
    CARAPI_(Boolean) CanGoBack();

    //@Override
    CARAPI_(Boolean) CanGoForward();

    //@Override
    CARAPI_(Boolean) CanGoToOffset(
        /* [in] */ Int32 offset);

    //@Override
    CARAPI_(void) GoToOffset(
        /* [in] */ Int32 offset);

    //@Override
    CARAPI_(void) GoToNavigationIndex(
        /* [in] */ Int32 index);

    //@Override
    CARAPI_(void) GoBack();

    //@Override
    CARAPI_(void) GoForward();

    static CARAPI_(void*) ElaNavigationControllerImplCallback_Init();

private:
    static CARAPI_(void) Destroy(
        /* [in] */ IInterface* obj);

    NavigationControllerImpl(
        /* [in] */ Handle64 nativeNavigationControllerAndroid);

    //@CalledByNative return NavigationControllerImpl
    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ Handle64 nativeNavigationControllerAndroid);

    //@CalledByNative
    CARAPI_(void) Destroy();

    CARAPI_(Boolean) NativeCanGoBack(
        /* [in] */ Handle64 nativeNavigationControllerAndroid);

    CARAPI_(Boolean) NativeCanGoForward(
        /* [in] */ Handle64 nativeNavigationControllerAndroid);

    CARAPI_(Boolean) NativeCanGoToOffset(
        /* [in] */ Handle64 nativeNavigationControllerAndroid,
        /* [in] */ Int32 offset);

    CARAPI_(void) NativeGoBack(
        /* [in] */ Handle64 nativeNavigationControllerAndroid);

    CARAPI_(void) NativeGoForward(
        /* [in] */ Handle64 nativeNavigationControllerAndroid);

    CARAPI_(void) NativeGoToOffset(
        /* [in] */ Handle64 nativeNavigationControllerAndroid,
        /* [in] */ Int32 offset);

    CARAPI_(void) NativeGoToNavigationIndex(
        /* [in] */ Handle64 nativeNavigationControllerAndroid,
        /* [in] */ Int32 index);

private:
    Handle64 mNativeNavigationControllerAndroid;
};

} // namespace Framehost
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_FRAMEHOST_NAVIGATIONCONTROLLERIMPL_H__
