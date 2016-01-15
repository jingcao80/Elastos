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

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

//===============================================================
//      WebViewChromiumFactoryProvider::TraceEventRunnable
//===============================================================

WebViewChromiumFactoryProvider::TraceEventRunnable::TraceEventRunnable(
    /* [in] */ WebViewChromiumFactoryProvider* owner)
    : mOwner(owner)
{
}

ECode WebViewChromiumFactoryProvider::TraceEventRunnable::Run()
{
    mOwner->SyncATraceState();
    return NOERROR;
}

//===============================================================
//        WebViewChromiumFactoryProvider::UiRunnable
//===============================================================

WebViewChromiumFactoryProvider::UiRunnable::UiRunnable(
    /* [in] */ WebViewChromiumFactoryProvider* owner)
    : mOwner(owner)
{
}

ECode WebViewChromiumFactoryProvider::UiRunnable::Run()
{
    Object::Autolock lock(mOwner->mLock);
    mOwner->StartChromiumLocked();

    return NOERROR;
}

//===============================================================
//        WebViewChromiumFactoryProvider::InnerStatics
//===============================================================

WebViewChromiumFactoryProvider::InnerStatics::InnerStatics(
    /* [in] */ WebViewChromiumFactoryProvider* owner)
    : mOwner(owner)
{
}

ECode WebViewChromiumFactoryProvider::InnerStatics::FindAddress(
    /* [in] */ const String& addr,
    /* [out] */ String* addrOut)
{
    VALIDATE_NOT_NULL(addrOut);
    *addrOut = ContentViewStatics::FindAddress(addr);
    return NOERROR;
}

ECode WebViewChromiumFactoryProvider::InnerStatics::GetDefaultUserAgent(
    /* [in] */ IContext* context,
    /* [out] */ String* userAgent)
{
    VALIDATE_NOT_NULL(userAgent);
    *userAgent = AwSettings::GetDefaultUserAgent();
    return NOERROR;
}

ECode WebViewChromiumFactoryProvider::InnerStatics::SetWebContentsDebuggingEnabled(
    /* [in] */ Boolean enable)
{
    // Web Contents debugging is always enabled on debug builds.
    if (!Build::IS_DEBUGGABLE) {
        mOwner->SetWebContentsDebuggingEnabled(enable);
    }

    return NOERROR;
}

// TODO enable after L release to AOSP
ECode WebViewChromiumFactoryProvider::InnerStatics::ClearClientCertPreferences(
    /* [in] */ IRunnable* onCleared)
{
    AwContentsStatics::ClearClientCertPreferences(onCleared);
    return NOERROR;
}

ECode WebViewChromiumFactoryProvider::InnerStatics::FreeMemoryForTests()
{
    if (ActivityManager.isRunningInTestHarness()) {
        MemoryPressureListener.maybeNotifyMemoryPresure(
                ComponentCallbacks2.TRIM_MEMORY_COMPLETE);
    }

    return NOERROR;
}

// TODO: Add @Override.
ECode WebViewChromiumFactoryProvider::InnerStatics::EnableSlowWholeDocumentDraw()
{
    WebViewChromium::EnableSlowWholeDocumentDraw();
    return NOERROR;
}

AutoPtr< ArrayOf<IUri> > WebViewChromiumFactoryProvider::InnerStatics::ParseFileChooserResult(
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* intent)
{
    return FileChooserParamsAdapter::ParseFileChooserResult(resultCode, intent);
}

//===============================================================
//               WebViewChromiumFactoryProvider
//===============================================================

static const String WebViewChromiumFactoryProvider::TAG("WebViewChromiumFactoryProvider");
static const String WebViewChromiumFactoryProvider::CHROMIUM_PREFS_NAME("WebViewChromiumPrefs");
static const String WebViewChromiumFactoryProvider::VERSION_CODE_PREF("lastVersionCodeUsed");
static const String WebViewChromiumFactoryProvider::COMMAND_LINE_FILE("/data/local/tmp/webview-command-line");

WebViewChromiumFactoryProvider::WebViewChromiumFactoryProvider()
{
    ThreadUtils::SetWillOverrideUiThread();
    // Load chromium library.
    Trace.traceBegin(Trace.TRACE_TAG_WEBVIEW, "AwBrowserProcess.loadLibrary()");
    AwBrowserProcess::LoadLibrary();
    Trace.traceEnd(Trace.TRACE_TAG_WEBVIEW);
    // Load glue-layer support library.
    System.loadLibrary("webviewchromium_plat_support");

    // TODO: temporary try/catch while framework builds catch up with WebView builds.
    // Remove this.
    // try {
        // Use shared preference to check for package downgrade.
        AutoPtr<IActivityThreadHelper> helper;
        CActivityThreadHelper::AcquireSingleton((IActivityThreadHelper**)&helper);
        AutoPtr<IApplication> app;
        helper->GetCurrentApplication((IApplication**)&app);
        AutoPtr<IContext> appContext = app->Probe(EIID_IContext);
        appContext->GetSharedPreferences(CHROMIUM_PREFS_NAME, IContext::MODE_PRIVATE, (ISharedPreferences**)&mWebViewPrefs);
        Int32 lastVersion;
        mWebViewPrefs->GetInt32(VERSION_CODE_PREF, 0, &lastVersion);
        Int32 currentVersion = WebViewFactory.getLoadedPackageInfo().versionCode;
        if (lastVersion > currentVersion) {
            // The WebView package has been downgraded since we last ran in this application.
            // Delete the WebView data directory's contents.
            String dataDir = PathUtils::GetDataDirectory(app);
//            Log.i(TAG, "WebView package downgraded from " + lastVersion + " to " + currentVersion +
//                       "; deleting contents of " + dataDir);
            AutoPtr<IFileUtilsHelper> fHelper;
            CFileUtilsHelper::AcquireSingleton((IFileUtilsHelper**)&fHelper);
            AutoPtr<IFile> file;
            CFile::New(dataDir, (IFile**)&file);
            fHelper->DeleteContents(file);
        }

        if (lastVersion != currentVersion) {
            AutoPtr<ISharedPreferencesEditor> edit;
            mWebViewPrefs->Edit((ISharedPreferencesEditor**)&edit);
            edit->PutInt32(VERSION_CODE_PREF, currentVersion);
            edit->Apply();
        }
    // } catch (NoSuchMethodError e) {
    //    Log.w(TAG, "Not doing version downgrade check as framework is too old.");
    // }

    // Now safe to use WebView data directory.
}

void WebViewChromiumFactoryProvider::InitPlatSupportLibrary()
{
    DrawGLFunctor::SetChromiumAwDrawGLFunction(AwContents::GetAwDrawGLFunction());
    AwContents::SetAwDrawSWFunctionTable(GraphicsUtils::GetDrawSWFunctionTable());
    AwContents::SetAwDrawGLFunctionTable(GraphicsUtils::GetDrawGLFunctionTable());
}

void WebViewChromiumFactoryProvider::InitTraceEvent()
{
    SyncATraceState();
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    AutoPtr<IRunnable> runable = new TraceEventRunnable(this);
    sp->AddChangeCallback(runable);
}

void WebViewChromiumFactoryProvider::SyncATraceState()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Int64 enabledFlags;
    sp->GetInt64(String("debug.atrace.tags.enableflags"), 0, &enabledFlags);
    TraceEvent.setATraceEnabled((enabledFlags & Trace.TRACE_TAG_WEBVIEW) != 0);
}

void WebViewChromiumFactoryProvider::EnsureChromiumStartedLocked(
    /* [in] */ Boolean onMainThread)
{
    assert Thread.holdsLock(mLock);

    if (mStarted) {  // Early-out for the common case.
        return;
    }

    AutoPtr<ILooper> looper;
    AutoPtr<ILooperHelper> looperHelp;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&looperHelp);
    if (!onMainThread) {
        looperHelp->MyLooper((ILooper**)&looper);
    }
    else {
        looperHelp->GetMainLooper((ILooper**)&looper);
    }

    Log.v(TAG, "Binding Chromium to " +
            (Looper.getMainLooper().equals(looper) ? "main":"background") +
            " looper " + looper);

    ThreadUtils::SetUiThread(looper);

    if (ThreadUtils::RunningOnUiThread()) {
        StartChromiumLocked();
        return;
    }

    // We must post to the UI thread to cover the case that the user has invoked Chromium
    // startup by using the (thread-safe) CookieManager rather than creating a WebView.
    ThreadUtils.postOnUiThread(new Runnable() {
        @Override
        public void run() {
            synchronized(mLock) {
                startChromiumLocked();
            }
        }
    });

    AutoPtr<IRunnable> uiRunnable = new UiRunnable(this);
    ThreadUtils::PostOnUiThread(uiRunnable);
    while (!mStarted) {
        //try {
            // Important: wait() releases |mLock| the UI thread can take it :-)
            mLock.Wait();
        //} catch (InterruptedException e) {
            // Keep trying... eventually the UI thread will process the task we sent it.
        //}
    }
}

void WebViewChromiumFactoryProvider::StartChromiumLocked()
{
    assert Thread.holdsLock(mLock) && ThreadUtils.runningOnUiThread();

    // The post-condition of this method is everything is ready, so notify now to cover all
    // return paths. (Other threads will not wake-up until we release |mLock|, whatever).
    mLock.NotifyAll();

    if (mStarted) {
        return;
    }

    if (Build::IS_DEBUGGABLE) {
        // Suppress the StrictMode violation as this codepath is only hit on debugglable builds.
        AutoPtr<IStrictMode> strictMode;
        CStrictMode::AcquireSingleton((IStrictMode**)&strictMode);
        Handle32 oldPolicy;
        strictMode->AllowThreadDiskReads(&oldPolicy);
        CommandLine::InitFromFile(COMMAND_LINE_FILE);
        strictMode->SetThreadPolicy(oldPolicy);
    }
    else {
        CommandLine::Init(NULL);
    }

    AutoPtr<CommandLine> cl = CommandLine::GetInstance();
    // TODO: currently in a relase build the DCHECKs only log. We either need to insall
    // a report handler with SetLogReportHandler to make them assert, or else compile
    // them out of the build altogether (b/8284203). Either way, so long they're
    // compiled in, we may as unconditionally enable them here.
    cl->AppendSwitch("enable-dcheck");

    // TODO: Remove when GL is supported by default in the upstream code.
    if (!cl->HasSwitch("disable-webview-gl-mode")) {
        cl->AppendSwitch("testing-webview-gl-mode");
    }

    // We don't need to extract any paks because for WebView, they are
    // in the system image.
    ResourceExtractor::SetMandatoryPaksToExtract("");

    //try {
        LibraryLoader::EnsureInitialized();
    //} catch(ProcessInitException e) {
    //    throw new RuntimeException("Error initializing WebView library", e);
    //}

    PathService::Override(IPathService::DIR_MODULE, "/system/lib/");
    // TODO: DIR_RESOURCE_PAKS_ANDROID needs to live somewhere sensible,
    // inlined here for simplicity setting up the HTMLViewer demo. Unfortunately
    // it can't go into base.PathService, as the native constant it refers to
    // lives in the ui/ layer. See ui/base/ui_base_paths.h
    const Int32 DIR_RESOURCE_PAKS_ANDROID = 3003;
    PathService::Override(DIR_RESOURCE_PAKS_ANDROID,
            "/system/framework/webview/paks");

    // Make sure that ResourceProvider is initialized before starting the browser process.
    AutoPtr<IActivityThreadHelper> helper;
    CActivityThreadHelper::AcquireSingleton((IActivityThreadHelper**)&helper);
    AutoPtr<IApplication> app;
    helper->GetCurrentApplication((IApplication**)&app);
    SetUpResources(app);
    InitPlatSupportLibrary();
    AwBrowserProcess::Start(app);

    if (Build::IS_DEBUGGABLE) {
        SetWebContentsDebuggingEnabled(TRUE);
    }

    InitTraceEvent();
    mStarted = TRUE;

    for (WeakReference<WebViewChromium> wvc : mWebViewsToStart) {
        WebViewChromium w = wvc.get();
        if (w != null) {
            w.startYourEngine();
        }
    }
    mWebViewsToStart.clear();
    mWebViewsToStart = null;
}

Boolean WebViewChromiumFactoryProvider::HasStarted()
{
    return mStarted;
}

void WebViewChromiumFactoryProvider::StartYourEngines(
    /* [in] */ Boolean onMainThread)
{
    Object::Autolock lock(mLock);
    EnsureChromiumStartedLocked(onMainThread);
}

AwBrowserContext WebViewChromiumFactoryProvider::GetBrowserContext()
{
    Object::Autolock lock(mLock);
    GetBrowserContextLocked();
}

AwBrowserContext WebViewChromiumFactoryProvider::GetBrowserContextLocked()
{
    assert Thread.holdsLock(mLock);
    assert mStarted;
    if (mBrowserContext == NULL) {
        mBrowserContext = new AwBrowserContext(mWebViewPrefs);
    }
    return mBrowserContext;
}

void WebViewChromiumFactoryProvider::SetWebContentsDebuggingEnabled(
    /* [in] */ Boolean enable)
{
    AutoPtr<ILooper> looper;
    AutoPtr<ILooperHelper> helper;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&helper);
    helper->MyLooper((ILooper**)&looper);
    if (looper != ThreadUtils::GetUiThreadLooper()) {
        throw new RuntimeException(
                "Toggling of Web Contents Debugging must be done on the UI thread");
    }
    if (mDevToolsServer == NULL) {
        if (!enable) return;
        mDevToolsServer = new AwDevToolsServer();
    }
    mDevToolsServer->SetRemoteDebuggingEnabled(enable);
}

void WebViewChromiumFactoryProvider::SetUpResources(
    /* [in] */ IContext* ctx)
{
    ResourceRewriter::RewriteRValues(ctx);

    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);
    AwResource::SetResources(res);
    AwResource::SetErrorPageResources(android::R::raw::loaderror,
            android::R::raw::nodomain);
    AwResource::SetConfigKeySystemUuidMapping(
            android::R::array::config_keySystemUuidMapping);
}

AutoPtr<IStatics> WebViewChromiumFactoryProvider::GetStatics()
{
    {
        Object::Autolock lock(mLock);

        if (mStaticMethods == NULL) {
            // TODO: Optimization potential: most these methods only need the native library
            // loaded and initialized, not the entire browser process started.
            // See also http://b/7009882
            EnsureChromiumStartedLocked(true);
            mStaticMethods = new InnerStatics(this);
        }
    }

    return mStaticMethods;
}

AutoPtr<IWebViewProvider> WebViewChromiumFactoryProvider::CreateWebView(
    /* [in] */ IWebView* webView,
    /* [in] */ IWebViewPrivateAccess* privateAccess)
{
    AutoPtr<WebViewChromium> wvc = new WebViewChromium(this, webView, privateAccess);

    {
        Object::Autolock lock(mLock);

        if (mWebViewsToStart != null) {
            mWebViewsToStart.add(new WeakReference<WebViewChromium>(wvc));
        }
    }

    return wvc;
}

AutoPtr<GeolocationPermissions> WebViewChromiumFactoryProvider::GetGeolocationPermissions()
{
    {
        Object::Autolock lock(mLock);

        if (mGeolocationPermissions == NULL) {
            EnsureChromiumStartedLocked(TRUE);
            mGeolocationPermissions = new GeolocationPermissionsAdapter(
                    GetBrowserContextLocked()->GetGeolocationPermissions());
        }
    }

    return mGeolocationPermissions;
}

AutoPtr<ICookieManager> WebViewChromiumFactoryProvider::GetCookieManager()
{
    {
        Object::Autolock lock(mLock);

        if (mCookieManager == NULL) {
            if (!mStarted) {
                // We can use CookieManager without starting Chromium; the native code
                // will bring up just the parts it needs to make this work on a temporary
                // basis until Chromium is started for real. The temporary cookie manager
                // needs the application context to have been set.
                ContentMain.initApplicationContext(ActivityThread.currentApplication());
            }
            mCookieManager = new CookieManagerAdapter(new AwCookieManager());
        }
    }

    return mCookieManager;
}

AutoPtr<IWebIconDatabase> WebViewChromiumFactoryProvider::GetWebIconDatabase()
{
    {
        Object::Autolock lock(mLock);

        if (mWebIconDatabase == NULL) {
            EnsureChromiumStartedLocked(TRUE);
            mWebIconDatabase = new WebIconDatabaseAdapter();
        }
    }

    return mWebIconDatabase;
}

AutoPtr<IWebStorage> WebViewChromiumFactoryProvider::GetWebStorage()
{
    {
        Object::Autolock lock(mLock);

        if (mWebStorage == NULL) {
            EnsureChromiumStartedLocked(TRUE);
            mWebStorage = new WebStorageAdapter(AwQuotaManagerBridge->GetInstance());
        }
    }

    return mWebStorage;
}

AutoPtr<IWebViewDatabase> WebViewChromiumFactoryProvider::GetWebViewDatabase(
    /* [in] */ IContext* context)
{
    {
        Object::Autolock lock(mLock);

        if (mWebViewDatabase == NULL) {
            EnsureChromiumStartedLocked(TRUE);
            AutoPtr<AwBrowserContext> browserContext = GetBrowserContextLocked();
            mWebViewDatabase = new WebViewDatabaseAdapter(
                    browserContext->GetFormDatabase(),
                    browserContext->GetHttpAuthDatabase(context));
        }
    }

    return mWebViewDatabase;
}

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos