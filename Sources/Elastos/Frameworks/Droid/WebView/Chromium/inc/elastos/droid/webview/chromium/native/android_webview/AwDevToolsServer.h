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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWDEVTOOLSSERVER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWDEVTOOLSSERVER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// import org.chromium.base.JNINamespace;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * Controller for Remote Web Debugging (Developer Tools).
 */
//@JNINamespace("android_webview")
class AwDevToolsServer
    : public Object
{
public:
    AwDevToolsServer();

    CARAPI_(void) Destroy();

    CARAPI_(void) SetRemoteDebuggingEnabled(
        /* [in] */ Boolean enabled);

private:
    CARAPI_(Int64) NativeInitRemoteDebugging();

    CARAPI_(void) NativeDestroyRemoteDebugging(
        /* [in] */ Int64 devToolsServer);

    CARAPI_(void) NativeSetRemoteDebuggingEnabled(
        /* [in] */ Int64 devToolsServer,
        /* [in] */ Boolean enabled);

    Int64 mNativeDevToolsServer;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWDEVTOOLSSERVER_H__
