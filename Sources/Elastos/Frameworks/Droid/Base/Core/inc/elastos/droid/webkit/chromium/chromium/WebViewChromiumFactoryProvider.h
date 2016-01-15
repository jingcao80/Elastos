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

#ifndef __ELASTOS_DROID_CHROMIUM_CHROMIUM_WEBVIEWCHROMIUMFACTORYPROVIDER_H__
#define __ELASTOS_DROID_CHROMIUM_CHROMIUM_WEBVIEWCHROMIUMFACTORYPROVIDER_H__

// import android.content.pm.PackageManager;
// import android.content.res.Resources;
// import android.app.ActivityManager;
// import android.app.ActivityThread;
// import android.content.ComponentCallbacks2;
// import android.content.Context;
// import android.content.Intent;
// import android.content.SharedPreferences;
// import android.net.Uri;
// import android.os.Build;
// import android.os.FileUtils;
// import android.os.Looper;
// import android.os.StrictMode;
// import android.os.SystemProperties;
// import android.os.Trace;
// import android.util.Log;
// import android.webkit.CookieManager;
// import android.webkit.GeolocationPermissions;
// import android.webkit.WebIconDatabase;
// import android.webkit.WebStorage;
// import android.webkit.WebView;
// import android.webkit.WebViewDatabase;
// import android.webkit.WebViewFactory;
// import android.webkit.WebViewFactoryProvider;
// import android.webkit.WebViewProvider;

// import org.chromium.android_webview.AwBrowserContext;
// import org.chromium.android_webview.AwBrowserProcess;
// import org.chromium.android_webview.AwContents;
// import org.chromium.android_webview.AwContentsStatics;
// import org.chromium.android_webview.AwCookieManager;
// import org.chromium.android_webview.AwDevToolsServer;
// import org.chromium.android_webview.AwFormDatabase;
// import org.chromium.android_webview.AwGeolocationPermissions;
// import org.chromium.android_webview.AwQuotaManagerBridge;
// import org.chromium.android_webview.AwResource;
// import org.chromium.android_webview.AwSettings;
// import org.chromium.base.CommandLine;
// import org.chromium.base.MemoryPressureListener;
// import org.chromium.base.PathService;
// import org.chromium.base.PathUtils;
// import org.chromium.base.ThreadUtils;
// import org.chromium.base.TraceEvent;
// import org.chromium.base.library_loader.LibraryLoader;
// import org.chromium.base.library_loader.ProcessInitException;
// import org.chromium.content.app.ContentMain;
// import org.chromium.content.browser.ContentViewStatics;
// import org.chromium.content.browser.ResourceExtractor;

// import java.io.File;
// import java.lang.ref.WeakReference;
// import java.util.ArrayList;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

class WebViewChromiumFactoryProvider : public IWebViewFactoryProvider
{
private:
    class TraceEventRunnable
        : public Object
        , public IRunnable
    {
    public:
        TraceEventRunnable(
            /* [in] */ WebViewChromiumFactoryProvider* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromiumFactoryProvider* mOwner;
    };

    class UiRunnable
        : public Object
        , public IRunnable
    {
    public:
        UiRunnable(
            /* [in] */ WebViewChromiumFactoryProvider* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromiumFactoryProvider* mOwner;
    };

    class InnerStatics
        : public Object
        , public IStatics
    {
    public:
        InnerStatics(
            /* [in] */ WebViewChromiumFactoryProvider* owner);

        CAR_INTERFACE_DECL();

        CARAPI FindAddress(
            /* [in] */ const String& addr,
            /* [out] */ String* addrOut);

        CARAPI GetDefaultUserAgent(
            /* [in] */ IContext* context,
            /* [out] */ String* userAgent);

        CARAPI SetWebContentsDebuggingEnabled(
            /* [in] */ Boolean enable);

        // TODO enable after L release to AOSP
        CARAPI ClearClientCertPreferences(
            /* [in] */ IRunnable* onCleared);

        CARAPI FreeMemoryForTests();

        // TODO: Add @Override.
        CARAPI EnableSlowWholeDocumentDraw();

        CARAPI ParseFileChooserResult(
            /* [in] */ Int32 resultCode,
            /* [in] */ IIntent* intent,
            /* [out] */ ArrayOf<IUri>** arrayUri);

    private:
        WebViewChromiumFactoryProvider* mOwner;
    };

public:
    WebViewChromiumFactoryProvider();

    CARAPI_(AutoPtr<IStatics>) GetStatics();

    CARAPI_(AutoPtr<IWebViewProvider>) CreateWebView(
        /* [in] */ IWebView* webView,
        /* [in] */ IWebViewPrivateAccess* privateAccess);

    CARAPI_(AutoPtr<GeolocationPermissions>) GetGeolocationPermissions();

    CARAPI_(AutoPtr<ICookieManager>) GetCookieManager();

    CARAPI_(AutoPtr<IWebIconDatabase>) GetWebIconDatabase();

    CARAPI_(AutoPtr<IWebStorage>) GetWebStorage();

    CARAPI_(AutoPtr<IWebViewDatabase>) GetWebViewDatabase(
        /* [in] */ IContext* context);

    CARAPI_(Boolean) HasStarted();

    CARAPI_(void) StartYourEngines(
        /* [in] */ Boolean onMainThread);

    CARAPI_(AutoPtr<AwBrowserContext>) GetBrowserContext();

private:
    CARAPI_(void) InitPlatSupportLibrary();

    static CARAPI_(void) InitTraceEvent();

    static CARAPI_(void) SyncATraceState();

    CARAPI_(void) EnsureChromiumStartedLocked(
        /* [in] */ Boolean onMainThread);

    CARAPI_(void) StartChromiumLocked();

    CARAPI_(AutoPtr<AwBrowserContext>) GetBrowserContextLocked();

    CARAPI_(void) SetWebContentsDebuggingEnabled(
        /* [in] */ Boolean enable);

    CARAPI_(void) SetUpResources(
        /* [in] */ IContext* ctx);

private:
    static const String TAG;

    static const String CHROMIUM_PREFS_NAME;
    static const String VERSION_CODE_PREF;
    static const String COMMAND_LINE_FILE;

    // Guards accees to the other members, and is notifyAll() signalled on the UI thread
    // when the chromium process has been started.
    Object mLock;

    // Initialization guarded by mLock.
    AutoPtr<AwBrowserContext> mBrowserContext;
    AutoPtr<IStatics> mStaticMethods;
    AutoPtr<GeolocationPermissionsAdapter> mGeolocationPermissions;
    AutoPtr<CookieManagerAdapter> mCookieManager;
    AutoPtr<WebIconDatabaseAdapter> mWebIconDatabase;
    AutoPtr<WebStorageAdapter> mWebStorage;
    AutoPtr<WebViewDatabaseAdapter> mWebViewDatabase;
    AutoPtr<AwDevToolsServer> mDevToolsServer;

    // private ArrayList<WeakReference<WebViewChromium>> mWebViewsToStart =
    //           new ArrayList<WeakReference<WebViewChromium>>();

    // Read/write protected by mLock.
    Boolean mStarted;

    AutoPtr<SharedPreferences> mWebViewPrefs;
};

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CHROMIUM_CHROMIUM_WEBVIEWCHROMIUMFACTORYPROVIDER_H__
