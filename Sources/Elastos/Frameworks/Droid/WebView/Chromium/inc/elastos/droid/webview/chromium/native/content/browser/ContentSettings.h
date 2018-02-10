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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CONTENTSETTINGS_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CONTENTSETTINGS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

class ContentViewCore;

/**
 * Manages settings state for a ContentView. A ContentSettings instance is obtained
 * from ContentViewCore.getContentSettings().
 */
//@JNINamespace("content")
class ContentSettings
    : public Object
{

public:
    /**
     * Package constructor to prevent clients from creating a new settings
     * instance. Must be called on the UI thread.
     */
    ContentSettings(
        /* [in] */ ContentViewCore* contentViewCore,
        /* [in] */ HANDLE nativeContentView);

    /**
     * Return true if JavaScript is enabled. Must be called on the UI thread.
     *
     * @return True if JavaScript is enabled.
     */
    CARAPI_(Boolean) GetJavaScriptEnabled();

    static CARAPI_(void*) ElaContentSettingsCallback_Init();

private:
    static CARAPI_(void) OnNativeContentSettingsDestroyed(
        /* [in] */ IInterface* obj,
        /* [in] */ HANDLE nativeContentSettings);

    /**
     * Notification from the native side that it is being destroyed.
     * @param nativeContentSettings the native instance that is going away.
     */
    //@CalledByNative
    CARAPI_(void) OnNativeContentSettingsDestroyed(
        /* [in] */ HANDLE nativeContentSettings);

    // Initialize the ContentSettings native side.
    CARAPI_(HANDLE) NativeInit(
        /* [in] */ HANDLE contentViewPtr);

    CARAPI_(Boolean) NativeGetJavaScriptEnabled(
        /* [in] */ HANDLE nativeContentSettings);

private:
    static const String TAG;

    // The native side of this object. Ownership is retained native-side by the WebContents
    // instance that backs the associated ContentViewCore.
    HANDLE mNativeContentSettings;

    AutoPtr<ContentViewCore> mContentViewCore;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CONTENTSETTINGS_H__
