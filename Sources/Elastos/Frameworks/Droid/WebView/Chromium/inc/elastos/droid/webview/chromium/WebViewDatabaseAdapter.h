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

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBVIEWDATABASEADAPTER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBVIEWDATABASEADAPTER_H_

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwFormDatabase.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/HttpAuthDatabase.h"

using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::AwFormDatabase;
using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::HttpAuthDatabase;
using Elastos::Droid::Webkit::IWebViewDatabase;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

/**
  * Chromium implementation of WebViewDatabase -- forwards calls to the
  * chromium internal implementation.
  */
class WebViewDatabaseAdapter
    : public Object
    , public IWebViewDatabase
{
public:
    CAR_INTERFACE_DECL()

    WebViewDatabaseAdapter(
        /* [in] */ AwFormDatabase* formDatabase,
        /* [in] */ HttpAuthDatabase* httpAuthDatabase);

    // @Override
    CARAPI HasUsernamePassword(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ClearUsernamePassword();

    // @Override
    CARAPI HasHttpAuthUsernamePassword(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ClearHttpAuthUsernamePassword();

    // @Override
    CARAPI HasFormData(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ClearFormData();

private:
    AutoPtr<AwFormDatabase> mFormDatabase;
    AutoPtr<HttpAuthDatabase> mHttpAuthDatabase;
};

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBVIEWDATABASEADAPTER_H_

