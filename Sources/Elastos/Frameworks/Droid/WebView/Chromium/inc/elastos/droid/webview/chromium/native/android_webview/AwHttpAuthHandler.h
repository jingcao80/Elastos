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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWHTTPAUTHHANDLER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWHTTPAUTHHANDLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

//@JNINamespace("android_webview")
class AwHttpAuthHandler
    : public Object
{
public:
    CARAPI_(void) Proceed(
        /* [in] */ const String& username,
        /* [in] */ const String& password);

    CARAPI_(void) Cancel();

    CARAPI_(Boolean) IsFirstAttempt();

    //@CalledByNative return AwHttpAuthHandler
    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ Handle64 nativeAwAuthHandler,
        /* [in] */ Boolean firstAttempt);

    //@CalledByNative
    CARAPI_(void) HandlerDestroyed();

private:
    AwHttpAuthHandler(
        /* [in] */ Handle64 nativeAwHttpAuthHandler,
        /* [in] */ Boolean firstAttempt);

    CARAPI_(void) NativeProceed(
        /* [in] */ Handle64 nativeAwHttpAuthHandler,
        /* [in] */ const String& username,
        /* [in] */ const String& password);

    CARAPI_(void) NativeCancel(
        /* [in] */ Handle64 nativeAwHttpAuthHandler);

//callback function declaration
public:
    static CARAPI_(void*) ElaAwHttpAuthHandlerCallback_Init();

private:
    static CARAPI_(void) HandlerDestroyed(
        /* [in] */ IInterface* obj);

private:
    Handle64 mNativeAwHttpAuthHandler;
    const Boolean mFirstAttempt;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWHTTPAUTHHANDLER_H__
