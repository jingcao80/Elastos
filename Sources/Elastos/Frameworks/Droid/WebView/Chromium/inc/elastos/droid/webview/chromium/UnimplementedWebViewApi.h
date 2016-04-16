/*
  * Copyright (C) 2012 The Android Open Source Project
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *      http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UNIMPLEMENTEDWEBVIEWAPI_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UNIMPLEMENTEDWEBVIEWAPI_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package com.android.webview.chromium;
// import android.util.Log;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

// TODO: remove this when all WebView APIs have been implemented.
class UnimplementedWebViewApi
    : public Object
{
private:
    class UnimplementedWebViewApiException
        : public Object
        // exception class: , public UnsupportedOperationException
    {
    public:
        UnimplementedWebViewApiException();
    };

public:
    static CARAPI Invoke();

private:
    static String TAG;
    static Boolean THROW;
    // By default we keep the traces down to one frame to reduce noise, but for debugging it might
    // be useful to set this to true.
    static Boolean FULL_TRACE;
};

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UNIMPLEMENTEDWEBVIEWAPI_H_

