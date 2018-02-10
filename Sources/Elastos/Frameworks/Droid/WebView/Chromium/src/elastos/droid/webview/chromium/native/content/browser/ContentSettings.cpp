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

#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentSettings.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/ContentSettings_dec.h"
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

const String ContentSettings::TAG("ContentSettings");

/**
 * Package constructor to prevent clients from creating a new settings
 * instance. Must be called on the UI thread.
 */
ContentSettings::ContentSettings(
    /* [in] */ ContentViewCore* contentViewCore,
    /* [in] */ HANDLE nativeContentView)
{
    ThreadUtils::AssertOnUiThread();
    mContentViewCore = contentViewCore;
    mNativeContentSettings = NativeInit(nativeContentView);
    assert(mNativeContentSettings != 0);
}

/**
 * Notification from the native side that it is being destroyed.
 * @param nativeContentSettings the native instance that is going away.
 */
//@CalledByNative
void ContentSettings::OnNativeContentSettingsDestroyed(
    /* [in] */ HANDLE nativeContentSettings)
{
    assert(mNativeContentSettings == nativeContentSettings);
    mNativeContentSettings = 0;
}

/**
 * Return true if JavaScript is enabled. Must be called on the UI thread.
 *
 * @return True if JavaScript is enabled.
 */
Boolean ContentSettings::GetJavaScriptEnabled()
{
    ThreadUtils::AssertOnUiThread();
    return mNativeContentSettings != 0 ?
            NativeGetJavaScriptEnabled(mNativeContentSettings) : FALSE;
}

// Initialize the ContentSettings native side.
HANDLE ContentSettings::NativeInit(
    /* [in] */ HANDLE contentViewPtr)
{
    return Elastos_ContentSettings_nativeInit(TO_IINTERFACE(this), contentViewPtr);
}

Boolean ContentSettings::NativeGetJavaScriptEnabled(
    /* [in] */ HANDLE nativeContentSettings)
{
    return Elastos_ContentSettings_nativeGetJavaScriptEnabled(TO_IINTERFACE(this), (HANDLE)nativeContentSettings);
}

void ContentSettings::OnNativeContentSettingsDestroyed(
    /* [in] */ IInterface* obj,
    /* [in] */ HANDLE nativeContentSettings)
{
    AutoPtr<ContentSettings> mObj = (ContentSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentSettings::OnNativeContentSettingsDestroyed, mObj is NULL");
        return;
    }
    mObj->OnNativeContentSettingsDestroyed(nativeContentSettings);
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
