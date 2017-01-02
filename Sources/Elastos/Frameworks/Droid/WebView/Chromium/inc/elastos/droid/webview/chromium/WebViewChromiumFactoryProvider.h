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

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBVIEWCHROMIUMFACTORYPROVIDER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBVIEWCHROMIUMFACTORYPROVIDER_H_

#include <Elastos.Droid.Webkit.h>
#include <Elastos.Droid.Webkit.Webview.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwBrowserContext.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwDevToolsServer.h"
#include "elastos/droid/webkit/webview/chromium/CookieManagerAdapter.h"
#include "elastos/droid/webkit/webview/chromium/GeolocationPermissionsAdapter.h"
#include "elastos/droid/webkit/webview/chromium/WebIconDatabaseAdapter.h"
#include "elastos/droid/webkit/webview/chromium/WebStorageAdapter.h"
#include "elastos/droid/webkit/webview/chromium/WebViewDatabaseAdapter.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::AwBrowserContext;
using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::AwDevToolsServer;
using Elastos::Droid::Webkit::ICookieManager;
using Elastos::Droid::Webkit::IWebIconDatabase;
using Elastos::Droid::Webkit::IWebStorage;
using Elastos::Droid::Webkit::IWebViewFactoryProvider;
using Elastos::Droid::Webkit::IWebViewFactoryProviderStatics;
using Elastos::Droid::Webkit::Webview::Chromium::CookieManagerAdapter;
using Elastos::Droid::Webkit::Webview::Chromium::GeolocationPermissionsAdapter;
using Elastos::Droid::Webkit::Webview::Chromium::WebIconDatabaseAdapter;
using Elastos::Droid::Webkit::Webview::Chromium::WebStorageAdapter;
using Elastos::Droid::Webkit::Webview::Chromium::WebViewDatabaseAdapter;
using Elastos::Droid::Webkit::Webview::Chromium::IWebViewChromiumFactoryProvider;
using Elastos::Droid::Webkit::IWebView;
using Elastos::Droid::Webkit::IWebViewPrivateAccess;
using Elastos::Droid::Webkit::IWebViewProvider;
using Elastos::Droid::Webkit::IGeolocationPermissions;
using Elastos::Core::IRunnable;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

class ECO_PUBLIC WebViewChromiumFactoryProvider
    : public Object
    , public IWebViewFactoryProvider
    , public IWebViewChromiumFactoryProvider
{
private:
    class InnerSyncATraceStateRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSyncATraceStateRunnable();

        // @Override
        CARAPI Run();
    };

    class InnerStartChromiumLockedRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerStartChromiumLockedRunnable(
            /* [in] */ WebViewChromiumFactoryProvider* owner);

        // @Override
        CARAPI Run();

    private:
        WebViewChromiumFactoryProvider* mOwner;
    };

    class InnerWebViewFactoryProviderStatics
        : public Object
        , public IWebViewFactoryProviderStatics
    {
    public:
        CAR_INTERFACE_DECL()

        InnerWebViewFactoryProviderStatics(
            /* [in] */ WebViewChromiumFactoryProvider* owner);

        // @Override
        CARAPI FindAddress(
            /* [in] */ const String& addr,
            /* [out] */ String* result);

        // @Override
        CARAPI GetDefaultUserAgent(
            /* [in] */ IContext* context,
            /* [out] */ String* result);

        // @Override
        CARAPI SetWebContentsDebuggingEnabled(
            /* [in] */ Boolean enable);

        // TODO enable after L release to AOSP
        //@Override
        CARAPI ClearClientCertPreferences(
            /* [in] */ IRunnable* onCleared);

        // @Override
        CARAPI FreeMemoryForTests();

        // TODO: Add @Override.
        CARAPI EnableSlowWholeDocumentDraw();

        // @Override
        CARAPI ParseFileChooserResult(
            /* [in] */ Int32 resultCode,
            /* [in] */ IIntent* intent,
            /* [out] */ ArrayOf<IUri*>** result);

    private:
        WebViewChromiumFactoryProvider* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    WebViewChromiumFactoryProvider();

    CARAPI constructor();

    virtual CARAPI HasStarted(
        /* [out] */ Boolean* result);

    virtual CARAPI StartYourEngines(
        /* [in] */ Boolean onMainThread);

    virtual CARAPI GetBrowserContext(
        /* [out] */ IInterface** result);

    // @Override
    CARAPI GetStatics(
        /* [out] */ IWebViewFactoryProviderStatics** result);

    // @Override
    CARAPI CreateWebView(
        /* [in] */ IWebView* webView,
        /* [in] */ IWebViewPrivateAccess* privateAccess,
        /* [out] */ IWebViewProvider** result);

    // @Override
    CARAPI GetGeolocationPermissions(
        /* [out] */ IGeolocationPermissions** result);

    // @Override
    CARAPI GetCookieManager(
        /* [out] */ ICookieManager** result);

    // @Override
    CARAPI GetWebIconDatabase(
        /* [out] */ IWebIconDatabase** result);

    // @Override
    CARAPI GetWebStorage(
        /* [out] */ IWebStorage** result);

    // @Override
    CARAPI GetWebViewDatabase(
        /* [in] */ IContext* context,
        /* [out] */ IWebViewDatabase** result);

private:
    CARAPI InitPlatSupportLibrary();

    static CARAPI InitTraceEvent();

    static CARAPI SyncATraceState();

    CARAPI EnsureChromiumStartedLocked(
        /* [in] */ Boolean onMainThread);

    CARAPI StartChromiumLocked();

    CARAPI_(AutoPtr<AwBrowserContext>) GetBrowserContextLocked();

    CARAPI SetWebContentsDebuggingEnabled(
        /* [in] */ Boolean enable);

    CARAPI SetUpResources(
        /* [in] */ IContext* ctx);

private:
    static const String TAG;
    static const String CHROMIUM_PREFS_NAME;
    static const String VERSION_CODE_PREF;
    static const String COMMAND_LINE_FILE;
    // Guards accees to the other members, and is notifyAll() signalled on the UI thread
    // when the chromium process has been started.
    /*const*/ AutoPtr<Object> mLock;
    // Initialization guarded by mLock.
    AutoPtr<AwBrowserContext> mBrowserContext;
    AutoPtr<IWebViewFactoryProviderStatics> mStaticMethods;
    AutoPtr<GeolocationPermissionsAdapter> mGeolocationPermissions;
    AutoPtr<CookieManagerAdapter> mCookieManager;
    AutoPtr<WebIconDatabaseAdapter> mWebIconDatabase;
    AutoPtr<WebStorageAdapter> mWebStorage;
    AutoPtr<WebViewDatabaseAdapter> mWebViewDatabase;
    AutoPtr<AwDevToolsServer> mDevToolsServer;
    AutoPtr<IList> mWebViewsToStart;
    // Read/write protected by mLock.
    Boolean mStarted;
    AutoPtr<ISharedPreferences> mWebViewPrefs;
};

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBVIEWCHROMIUMFACTORYPROVIDER_H_

