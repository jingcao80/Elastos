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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWBROWSERCONTEXT_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWBROWSERCONTEXT_H__
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewStatics.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwGeolocationPermissions.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwCookieManager.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwFormDatabase.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/HttpAuthDatabase.h"
#include "elastos/droid/webkit/webview/chromium/native/net/DefaultElastosKeyStore.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::ContentViewStatics;
using Elastos::Droid::Webkit::Webview::Chromium::Net::DefaultElastosKeyStore;
using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::AwGeolocationPermissions;
using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::AwCookieManager;
using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::AwFormDatabase;
using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::HttpAuthDatabase;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * Java side of the Browser Context: contains all the java side objects needed to host one
 * browing session (i.e. profile).
 * Note that due to running in single process mode, and limitations on renderer process only
 * being able to use a single browser context, currently there can only be one AwBrowserContext
 * instance, so at this point the class mostly exists for conceptual clarity.
 */
class AwBrowserContext
:public Object
{
public:
    AwBrowserContext(
        /* [in] */ ISharedPreferences* sharedPreferences);

    virtual CARAPI_(AutoPtr<AwGeolocationPermissions>) GetGeolocationPermissions();

    virtual CARAPI_(AutoPtr<AwCookieManager>) GetCookieManager();

    virtual CARAPI_(AutoPtr<AwFormDatabase>) GetFormDatabase();

    virtual CARAPI_(AutoPtr<HttpAuthDatabase>) GetHttpAuthDatabase(
        /* [in] */ IContext* context);

    virtual CARAPI_(AutoPtr<DefaultElastosKeyStore>) GetKeyStore();

    /**
     * @see android.webkit.WebView#pauseTimers()
     */
    virtual CARAPI_(void) PauseTimers();

    /**
     * @see android.webkit.WebView#resumeTimers()
     */
    virtual CARAPI_(void) ResumeTimers();

private:
    static const String HTTP_AUTH_DATABASE_FILE;

    AutoPtr<ISharedPreferences> mSharedPreferences;

    AutoPtr<AwGeolocationPermissions> mGeolocationPermissions;
    AutoPtr<AwCookieManager> mCookieManager;
    AutoPtr<AwFormDatabase> mFormDatabase;
    AutoPtr<HttpAuthDatabase> mHttpAuthDatabase;
    AutoPtr<DefaultElastosKeyStore> mLocalKeyStore;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWBROWSERCONTEXT_H__
