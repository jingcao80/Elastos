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

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBVIEWCHROMIUM_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBVIEWCHROMIUM_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContents.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/LoadUrlParams.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/SmartClipProvider.h"
#include "elastos/droid/webkit/webview/chromium/ContentSettingsAdapter.h"
#include "elastos/droid/webkit/webview/chromium/DrawGLFunctor.h"
#include "elastos/droid/webkit/webview/chromium/WebViewContentsClientAdapter.h"
#include "elastos/droid/webkit/webview/chromium/WebViewChromiumFactoryProvider.h"
#include <Elastos.Droid.Webkit.h>
#include "elastos/utility/concurrent/FutureTask.h"

using Elastos::Droid::Content::IContextWrapper;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPicture;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Net::Http::ISslCertificate;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
//using Elastos::Droid::Print::IPrintDocumentAdapter;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeProvider;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::View::InputMethod::IInputConnection;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::AwContents;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::LoadUrlParams;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::SmartClipProvider;
using Elastos::Droid::Webkit::IDownloadListener;
using Elastos::Droid::Webkit::IWebBackForwardList;
using Elastos::Droid::Webkit::IWebChromeClient;
using Elastos::Droid::Webkit::IWebSettings;
using Elastos::Droid::Webkit::IWebView;
using Elastos::Droid::Webkit::IWebViewClient;
using Elastos::Droid::Webkit::IWebViewHitTestResult;
using Elastos::Droid::Webkit::IWebViewPictureListener;
using Elastos::Droid::Webkit::IWebViewProvider;
using Elastos::Droid::Webkit::IWebViewProviderScrollDelegate;
using Elastos::Droid::Webkit::IWebViewProviderViewDelegate;
using Elastos::Droid::Webkit::Webview::Chromium::ContentSettingsAdapter;
using Elastos::Droid::Webkit::Webview::Chromium::DrawGLFunctor;
using Elastos::Droid::Webkit::Webview::Chromium::WebViewContentsClientAdapter;
using Elastos::Droid::Webkit::Webview::Chromium::IWebViewChromiumFactoryProvider;
using Elastos::Core::IClassLoader;
using Elastos::Core::IInteger32;
using Elastos::Core::IRunnable;
using Elastos::IO::IBufferedWriter;
using Elastos::IO::IFile;
using Elastos::Utility::Concurrent::FutureTask;
using Elastos::Utility::Concurrent::ICallable;
using Elastos::Utility::IMap;
using Elastos::Utility::IQueue;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

/**
  * This class is the delegate to which WebViewProxy forwards all API calls.
  *
  * Most of the actual functionality is implemented by AwContents (or ContentViewCore within
  * it). This class also contains WebView-specific APIs that require the creation of other
  * adapters (otherwise org.chromium.content would depend on the webview.chromium package)
  * and a small set of no-op deprecated APIs.
  */
class WebViewChromium
    : public Object
    , public SmartClipProvider
    , public IWebViewProvider
    , public IWebViewProviderScrollDelegate
    , public IWebViewProviderViewDelegate
{
private:
    class WebViewChromiumRunQueue
        : public Object
    {
    private:
        class InnerDrainQueueRunnable
            : public Object
            , public IRunnable
        {
        public:
            CAR_INTERFACE_DECL()

            InnerDrainQueueRunnable(
                /* [in] */ WebViewChromiumRunQueue* owner);

            // @Override
            CARAPI Run();

        private:
            WebViewChromiumRunQueue* mOwner;
        };

    public:
        WebViewChromiumRunQueue(
            /* [in] */ WebViewChromium* owner);

        virtual CARAPI AddTask(
            /* [in] */ IRunnable* task);

        virtual CARAPI DrainQueue();

    private:
        WebViewChromium* mOwner;
        AutoPtr<IQueue> mQueue;
    };

    class InnerInitForRealRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerInitForRealRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Boolean privateBrowsing);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Boolean mPrivateBrowsing;
    };

    class InnerContextWrapper
        : public Object
        , public IContextWrapper
    {
    private:
        class InnerClassLoader
            : public Object
            , public IClassLoader
        {
        public:
            CAR_INTERFACE_DECL()

            InnerClassLoader(
                /* [in] */ InnerContextWrapper* owner,
                /* [in] */ IClassLoader* appCl,
                /* [in] */ IClassLoader* webViewCl);

        protected:
            // @Override
            CARAPI FindClass(
                /* [in] */ const String& name,
                /* [out] */ IClassInfo** klass);

            // @Override
            CARAPI LoadClass(
                /* [in] */ const String& name,
                /* [out] */ IClassInfo** klass);

        private:
            InnerContextWrapper* mOwner;
            IClassLoader* mAppCl;
            IClassLoader* mWebViewCl;
        };

    public:
        CAR_INTERFACE_DECL()

        InnerContextWrapper();

        // @Override
        CARAPI GetClassLoader(
            /* [out] */ IClassLoader** result);

        // @Override
        //CARAPI GetSystemService(
        //    /* [in] */ const String& name,
        //    /* [out] */ IInterface** object);

        // @Override
        // just overwrite for compile
        CARAPI GetBaseContext(
            /* [out] */ IContext** context);
    };

    class InnerThreadViolationRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerThreadViolationRunnable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class InnerSetHorizontalScrollbarOverlayRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSetHorizontalScrollbarOverlayRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Boolean overlay);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Boolean mOverlay;
    };

    class InnerSetVerticalScrollbarOverlayRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSetVerticalScrollbarOverlayRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Boolean overlay);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Boolean mOverlay;
    };

    class InnerOverlayHorizontalScrollbarCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOverlayHorizontalScrollbarCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerOverlayVerticalScrollbarCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOverlayVerticalScrollbarCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerGetCertificateCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerGetCertificateCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerSetHttpAuthUsernamePasswordRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSetHttpAuthUsernamePasswordRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const String& host,
            /* [in] */ const String& realm,
            /* [in] */ const String& username,
            /* [in] */ const String& password);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const String mHost;
        const String mRealm;
        const String mUsername;
        const String mPassword;
    };

    class InnerGetHttpAuthUsernamePasswordCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerGetHttpAuthUsernamePasswordCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const String& host,
            /* [in] */ const String& realm);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const String mHost;
        const String mRealm;
    };

    class InnerDestroyRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerDestroyRunnable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class InnerSetNetworkAvailableRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSetNetworkAvailableRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Boolean networkUp);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Boolean mNetworkUp;
    };

    class InnerSaveStateCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSaveStateCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ IBundle* outState);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        IBundle* mOutState;
    };

    class InnerRestoreStateCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerRestoreStateCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ IBundle* inState);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        IBundle* mInState;
    };

    class InnerEvaluateJavaScriptRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerEvaluateJavaScriptRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const String& url,
            /* [in] */ const String& javascriptScheme);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const String mUrl;
        const String mJavascriptScheme;
    };

    class InnerLoadUrlRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerLoadUrlRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ LoadUrlParams* loadUrlParams);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        LoadUrlParams* mLoadUrlParams;
    };

    class InnerSaveWebArchiveRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSaveWebArchiveRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const String& basename,
            /* [in] */ Boolean autoname,
            /* [in] */ IValueCallback* callback);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const String mBasename;
        const Boolean mAutoname;
        IValueCallback* mCallback;
    };

    class InnerStopLoadingRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerStopLoadingRunnable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class InnerReloadRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerReloadRunnable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class InnerCanGoBackCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerCanGoBackCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerGoBackRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerGoBackRunnable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class InnerCanGoForwardCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerCanGoForwardCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerGoForwardRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerGoForwardRunnable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class InnerCanGoBackOrForwardCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerCanGoBackOrForwardCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Int32 steps);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const Int32 mSteps;
    };

    class InnerGoBackOrForwardRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerGoBackOrForwardRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Int32 steps);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Int32 mSteps;
    };

    class InnerPageUpCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerPageUpCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Boolean top);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const Boolean mTop;
    };

    class InnerPageDownCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerPageDownCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Boolean bottom);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const Boolean mBottom;
    };

    class InnerClearViewRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerClearViewRunnable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class InnerCapturePictureCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerCapturePictureCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerInvokeZoomPickerRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerInvokeZoomPickerRunnable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class InnerGetHitTestResultCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerGetHitTestResultCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerRequestFocusNodeHrefRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerRequestFocusNodeHrefRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ IMessage* hrefMsg);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        IMessage* mHrefMsg;
    };

    class InnerRequestImageRefRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerRequestImageRefRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ IMessage* msg);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        IMessage* mMsg;
    };

    class InnerGetUrlCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerGetUrlCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerGetOriginalUrlCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerGetOriginalUrlCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerGetTitleCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerGetTitleCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerGetFaviconCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerGetFaviconCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerPauseTimersRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerPauseTimersRunnable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class InnerResumeTimersRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerResumeTimersRunnable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class InnerOnPauseRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnPauseRunnable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class InnerOnResumeRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnResumeRunnable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class InnerIsPausedCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerIsPausedCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerClearCacheRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerClearCacheRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Boolean includeDiskFiles);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Boolean mIncludeDiskFiles;
    };

    class InnerClearFormDataRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerClearFormDataRunnable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class InnerClearHistoryRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerClearHistoryRunnable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class InnerClearSslPreferencesRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerClearSslPreferencesRunnable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class InnerCopyBackForwardListCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerCopyBackForwardListCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerFindNextRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerFindNextRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Boolean forwards);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Boolean mForwards;
    };

    class InnerFindAllAsyncRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerFindAllAsyncRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const String& searchString);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const String mSearchString;
    };

    class InnerNotifyFindDialogDismissedRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerNotifyFindDialogDismissedRunnable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class InnerClearMatchesRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerClearMatchesRunnable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class InnerDocumentHasImagesRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerDocumentHasImagesRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ IMessage* response);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        IMessage* mResponse;
    };

    class InnerSetPictureListenerRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSetPictureListenerRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ IWebViewPictureListener* listener);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        IWebViewPictureListener* mListener;
    };

    class InnerAddJavascriptInterfaceRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerAddJavascriptInterfaceRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Object* obj,
            /* [in] */ const String& interfaceName);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        Object* mObj;
        String mInterfaceName;
    };

    class InnerRemoveJavascriptInterfaceRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerRemoveJavascriptInterfaceRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const String& interfaceName);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        String mInterfaceName;
    };

    class InnerFlingScrollRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerFlingScrollRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Int32 vx,
            /* [in] */ Int32 vy);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Int32 mVx;
        const Int32 mVy;
    };

    class InnerZoomInCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerZoomInCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerZoomOutCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerZoomOutCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerShouldDelayChildPressedStateCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerShouldDelayChildPressedStateCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerGetAccessibilityNodeProviderCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerGetAccessibilityNodeProviderCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerOnInitializeAccessibilityNodeInfoRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnInitializeAccessibilityNodeInfoRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ IAccessibilityNodeInfo* info);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        IAccessibilityNodeInfo* mInfo;
    };

    class InnerOnInitializeAccessibilityEventRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnInitializeAccessibilityEventRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ IAccessibilityEvent* event);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        IAccessibilityEvent* mEvent;
    };

    class InnerPerformAccessibilityActionCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerPerformAccessibilityActionCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Int32 action,
            /* [in] */ IBundle* arguments);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const Int32 mAction;
        IBundle* mArguments;
    };

    class InnerSetOverScrollModeRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSetOverScrollModeRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Int32 mode);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Int32 mMode;
    };

    class InnerSetScrollBarStyleRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSetScrollBarStyleRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Int32 style);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Int32 mStyle;
    };

    class InnerOnOverScrolledRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnOverScrolledRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Int32 scrollX,
            /* [in] */ Int32 scrollY,
            /* [in] */ Boolean clampedX,
            /* [in] */ Boolean clampedY);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Int32 mScrollX;
        const Int32 mScrollY;
        const Boolean mClampedX;
        const Boolean mClampedY;
    };

    class InnerOnWindowVisibilityChangedRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnWindowVisibilityChangedRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Int32 visibility);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Int32 mVisibility;
    };

    class InnerOnDrawRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnDrawRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ ICanvas* canvas);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        ICanvas* mCanvas;
    };

    class InnerOnConfigurationChangedRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnConfigurationChangedRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ IConfiguration* newConfig);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        IConfiguration* mNewConfig;
    };

    class InnerOnKeyMultipleCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnKeyMultipleCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Int32 keyCode,
            /* [in] */ Int32 repeatCount,
            /* [in] */ IKeyEvent* event);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const Int32 mKeyCode;
        const Int32 mRepeatCount;
        IKeyEvent* mEvent;
    };

    class InnerOnKeyDownCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnKeyDownCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const Int32 mKeyCode;
        IKeyEvent* mEvent;
    };

    class InnerOnKeyUpCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnKeyUpCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const Int32 mKeyCode;
        IKeyEvent* mEvent;
    };

    class InnerOnDetachedFromWindowRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnDetachedFromWindowRunnable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class InnerOnVisibilityChangedRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnVisibilityChangedRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ IView* changedView,
            /* [in] */ Int32 visibility);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        IView* mChangedView;
        const Int32 mVisibility;
    };

    class InnerOnWindowFocusChangedRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnWindowFocusChangedRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Boolean hasWindowFocus);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        Boolean mHasWindowFocus;
    };

    class InnerOnFocusChangedRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnFocusChangedRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Boolean focused,
            /* [in] */ Int32 direction,
            /* [in] */ IRect* previouslyFocusedRect);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Boolean mFocused;
        const Int32 mDirection;
        IRect* mPreviouslyFocusedRect;
    };

    class InnerOnSizeChangedRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnSizeChangedRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Int32 w,
            /* [in] */ Int32 h,
            /* [in] */ Int32 ow,
            /* [in] */ Int32 oh);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Int32 mW;
        const Int32 mH;
        const Int32 mOw;
        const Int32 mOh;
    };

    class InnerDispatchKeyEventCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerDispatchKeyEventCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ IKeyEvent* event);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        IKeyEvent* mEvent;
    };

    class InnerOnTouchEventCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnTouchEventCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ IMotionEvent* ev);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        IMotionEvent* mEv;
    };

    class InnerOnHoverEventCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnHoverEventCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ IMotionEvent* event);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        IMotionEvent* mEvent;
    };

    class InnerOnGenericMotionEventCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnGenericMotionEventCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ IMotionEvent* event);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        IMotionEvent* mEvent;
    };

    class InnerRequestFocusCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerRequestFocusCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Int32 direction,
            /* [in] */ IRect* previouslyFocusedRect);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const Int32 mDirection;
        IRect* mPreviouslyFocusedRect;
    };

    class InnerOnMeasureRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnMeasureRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Int32 widthMeasureSpec,
            /* [in] */ Int32 heightMeasureSpec);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Int32 mWidthMeasureSpec;
        const Int32 mHeightMeasureSpec;
    };

    class InnerRequestChildRectangleOnScreenCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerRequestChildRectangleOnScreenCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ IView* child,
            /* [in] */ IRect* rect,
            /* [in] */ Boolean immediate);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        IView* mChild;
        IRect* mRect;
        const Boolean mImmediate;
    };

    class InnerSetBackgroundColorRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSetBackgroundColorRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Int32 color);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Int32 mColor;
    };

    class InnerSetLayerTypeRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSetLayerTypeRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ Int32 layerType,
            /* [in] */ IPaint* paint);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Int32 mLayerType;
        IPaint* mPaint;
    };

    class InnerComputeHorizontalScrollRangeCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerComputeHorizontalScrollRangeCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerComputeHorizontalScrollOffsetCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerComputeHorizontalScrollOffsetCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerComputeVerticalScrollRangeCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerComputeVerticalScrollRangeCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerComputeVerticalScrollOffsetCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerComputeVerticalScrollOffsetCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerComputeVerticalScrollExtentCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerComputeVerticalScrollExtentCallable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InnerComputeScrollRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerComputeScrollRunnable(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class WebViewNativeGLDelegate
        : public AwContents::NativeGLDelegate
    {
    public:
        WebViewNativeGLDelegate(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI_(Boolean) RequestDrawGL(
            /* [in] */ ICanvas* canvas,
            /* [in] */ Boolean waitForCompletion,
            /* [in] */ IView* containerView);

        // @Override
        CARAPI_(void) DetachGLFunctor();

    private:
        WebViewChromium* mOwner;
    };

    class InternalAccessAdapter
        : public AwContents::InternalAccessDelegate
    {
    public:
        InternalAccessAdapter(
            /* [in] */ WebViewChromium* owner);

        // @Override
        CARAPI_(Boolean) DrawChild(
            /* [in] */ ICanvas* arg0,
            /* [in] */ IView* arg1,
            /* [in] */ Int64 arg2);

        // @Override
        CARAPI_(Boolean) Super_onKeyUp(
            /* [in] */ Int32 arg0,
            /* [in] */ IKeyEvent* arg1);

        // @Override
        CARAPI_(Boolean) Super_dispatchKeyEventPreIme(
            /* [in] */ IKeyEvent* arg0);

        // @Override
        CARAPI_(Boolean) Super_dispatchKeyEvent(
            /* [in] */ IKeyEvent* event);

        // @Override
        CARAPI_(Boolean) Super_onGenericMotionEvent(
            /* [in] */ IMotionEvent* arg0);

        // @Override
        CARAPI_(void) Super_onConfigurationChanged(
            /* [in] */ IConfiguration* arg0);

        // @Override
        CARAPI_(Int32) Super_getScrollBarStyle();

        // @Override
        CARAPI_(Boolean) AwakenScrollBars();

        // @Override
        CARAPI_(Boolean) Super_awakenScrollBars(
            /* [in] */ Int32 arg0,
            /* [in] */ Boolean arg1);

        // @Override
        CARAPI OnScrollChanged(
            /* [in] */ Int32 l,
            /* [in] */ Int32 t,
            /* [in] */ Int32 oldl,
            /* [in] */ Int32 oldt);

        // @Override
        CARAPI_(void) OverScrollBy(
            /* [in] */ Int32 deltaX,
            /* [in] */ Int32 deltaY,
            /* [in] */ Int32 scrollX,
            /* [in] */ Int32 scrollY,
            /* [in] */ Int32 scrollRangeX,
            /* [in] */ Int32 scrollRangeY,
            /* [in] */ Int32 maxOverScrollX,
            /* [in] */ Int32 maxOverScrollY,
            /* [in] */ Boolean isTouchEvent);

        // @Override
        CARAPI_(void) Super_scrollTo(
            /* [in] */ Int32 scrollX,
            /* [in] */ Int32 scrollY);

        // @Override
        CARAPI_(void) SetMeasuredDimension(
            /* [in] */ Int32 measuredWidth,
            /* [in] */ Int32 measuredHeight);

        // @Override
        CARAPI_(Boolean) Super_onHoverEvent(
            /* [in] */ IMotionEvent* event);

    private:
        WebViewChromium* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    // This does not touch any global / non-threadsafe state, but note that
    // init is ofter called right after and is NOT threadsafe.
    WebViewChromium(
        /* [in] */ IWebViewChromiumFactoryProvider* factory,
        /* [in] */ IWebView* webView,
        /* [in] */ IWebViewPrivateAccess* webViewPrivate);

    static CARAPI EnableSlowWholeDocumentDraw();

    static CARAPI CompleteWindowCreation(
        /* [in] */ IWebView* parent,
        /* [in] */ IWebView* child);

    // WebViewProvider methods --------------------------------------------------------------------
    // @Override
    // BUG=6790250 |javaScriptInterfaces| was only ever used by the obsolete DumpRenderTree
    // so is ignored. TODO: remove it from WebViewProvider.
    virtual CARAPI Init(
        /* [in] */ IMap* javaScriptInterfaces,
        /* [in] */ Boolean privateBrowsing);

    virtual CARAPI StartYourEngine();

    // @Override
    CARAPI SetHorizontalScrollbarOverlay(
        /* [in] */ Boolean overlay);

    // @Override
    CARAPI SetVerticalScrollbarOverlay(
        /* [in] */ Boolean overlay);

    // @Override
    CARAPI OverlayHorizontalScrollbar(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OverlayVerticalScrollbar(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetVisibleTitleHeight(
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetCertificate(
        /* [out] */ ISslCertificate** result);

    // @Override
    CARAPI SetCertificate(
        /* [in] */ ISslCertificate* certificate);

    // @Override
    CARAPI SavePassword(
        /* [in] */ const String& host,
        /* [in] */ const String& username,
        /* [in] */ const String& password);

    // @Override
    CARAPI SetHttpAuthUsernamePassword(
        /* [in] */ const String& host,
        /* [in] */ const String& realm,
        /* [in] */ const String& username,
        /* [in] */ const String& password);

    // @Override
    CARAPI GetHttpAuthUsernamePassword(
        /* [in] */ const String& host,
        /* [in] */ const String& realm,
        /* [out] */ ArrayOf<String>** result);

    // @Override
    CARAPI Destroy();

    // @Override
    CARAPI SetNetworkAvailable(
        /* [in] */ Boolean networkUp);

    // @Override
    CARAPI SaveState(
        /* [in] */ IBundle* outState,
        /* [out] */ IWebBackForwardList** result);

    // @Override
    CARAPI SavePicture(
        /* [in] */ IBundle* b,
        /* [in] */ IFile* dest,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI RestorePicture(
        /* [in] */ IBundle* b,
        /* [in] */ IFile* src,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI RestoreState(
        /* [in] */ IBundle* inState,
        /* [out] */ IWebBackForwardList** result);

    // @Override
    CARAPI LoadUrl(
        /* [in] */ const String& url,
        /* [in] */ IMap* additionalHttpHeaders);

    // @Override
    CARAPI LoadUrl(
        /* [in] */ const String& url);

    // @Override
    CARAPI PostUrl(
        /* [in] */ const String& url,
        /* [in] */ ArrayOf<Byte>* postData);

    // @Override
    CARAPI LoadData(
        /* [in] */ const String& data,
        /* [in] */ const String& mimeType,
        /* [in] */ const String& encoding);

    // @Override
    CARAPI LoadDataWithBaseURL(
        /* [in] */ const String& baseUrl,
        /* [in] */ const String& data,
        /* [in] */ const String& mimeType,
        /* [in] */ const String& encoding,
        /* [in] */ const String& historyUrl);

    virtual CARAPI EvaluateJavaScript(
        /* [in] */ const String& script,
        /* [in] */ IValueCallback* resultCallback);

    // @Override
    CARAPI SaveWebArchive(
        /* [in] */ const String& filename);

    // @Override
    CARAPI SaveWebArchive(
        /* [in] */ const String& basename,
        /* [in] */ Boolean autoname,
        /* [in] */ IValueCallback* callback);

    // @Override
    CARAPI StopLoading();

    // @Override
    CARAPI Reload();

    // @Override
    CARAPI CanGoBack(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GoBack();

    // @Override
    CARAPI CanGoForward(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GoForward();

    // @Override
    CARAPI CanGoBackOrForward(
        /* [in] */ Int32 steps,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GoBackOrForward(
        /* [in] */ Int32 steps);

    // @Override
    CARAPI IsPrivateBrowsingEnabled(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI PageUp(
        /* [in] */ Boolean top,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI PageDown(
        /* [in] */ Boolean bottom,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ClearView();

    // @Override
    CARAPI CapturePicture(
        /* [out] */ IPicture** result);

    // @Override
    CARAPI GetScale(
        /* [out] */ Float* result);

    // @Override
    CARAPI SetInitialScale(
        /* [in] */ Int32 scaleInPercent);

    // @Override
    CARAPI InvokeZoomPicker();

    // @Override
    CARAPI GetHitTestResult(
        /* [out] */ IWebViewHitTestResult** result);

    // @Override
    CARAPI RequestFocusNodeHref(
        /* [in] */ IMessage* hrefMsg);

    // @Override
    CARAPI RequestImageRef(
        /* [in] */ IMessage* msg);

    // @Override
    CARAPI GetUrl(
        /* [out] */ String* result);

    // @Override
    CARAPI GetOriginalUrl(
        /* [out] */ String* result);

    // @Override
    CARAPI GetTitle(
        /* [out] */ String* result);

    // @Override
    CARAPI GetFavicon(
        /* [out] */ IBitmap** result);

    // @Override
    CARAPI GetTouchIconUrl(
        /* [out] */ String* result);

    // @Override
    CARAPI GetProgress(
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetContentHeight(
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetContentWidth(
        /* [out] */ Int32* result);

    // @Override
    CARAPI PauseTimers();

    // @Override
    CARAPI ResumeTimers();

    // @Override
    CARAPI OnPause();

    // @Override
    CARAPI OnResume();

    // @Override
    CARAPI IsPaused(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI FreeMemory();

    // @Override
    CARAPI ClearCache(
        /* [in] */ Boolean includeDiskFiles);

    /**
      * This is a poorly named method, but we keep it for historical reasons.
      */
    // @Override
    CARAPI ClearFormData();

    // @Override
    CARAPI ClearHistory();

    // @Override
    CARAPI ClearSslPreferences();

    // @Override
    CARAPI CopyBackForwardList(
        /* [out] */ IWebBackForwardList** result);

    // @Override
    CARAPI SetFindListener(
        /* [in] */ IWebViewFindListener* listener);

    // @Override
    CARAPI FindNext(
        /* [in] */ Boolean forwards);

    // @Override
    CARAPI FindAll(
        /* [in] */ const String& searchString,
        /* [out] */ Int32* result);

    // @Override
    CARAPI FindAllAsync(
        /* [in] */ const String& searchString);

    // @Override
    CARAPI ShowFindDialog(
        /* [in] */ const String& text,
        /* [in] */ Boolean showIme,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI NotifyFindDialogDismissed();

    // @Override
    CARAPI ClearMatches();

    // @Override
    CARAPI DocumentHasImages(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetWebViewClient(
        /* [in] */ IWebViewClient* client);

    // @Override
    CARAPI SetDownloadListener(
        /* [in] */ IDownloadListener* listener);

    // @Override
    CARAPI SetWebChromeClient(
        /* [in] */ IWebChromeClient* client);

    // @Override
    CARAPI SetPictureListener(
        /* [in] */ IWebViewPictureListener* listener);

    // @Override
    CARAPI AddJavascriptInterface(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& interfaceName);

    // @Override
    CARAPI RemoveJavascriptInterface(
        /* [in] */ const String& interfaceName);

    // @Override
    CARAPI GetSettings(
        /* [out] */ IWebSettings** result);

    // @Override
    CARAPI SetMapTrackballToArrowKeys(
        /* [in] */ Boolean setMap);

    // @Override
    CARAPI FlingScroll(
        /* [in] */ Int32 vx,
        /* [in] */ Int32 vy);

    // @Override
    CARAPI GetZoomControls(
        /* [out] */ IView** result);

    // @Override
    CARAPI CanZoomIn(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI CanZoomOut(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ZoomIn(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ZoomOut(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ZoomBy(
        /* [in] */ Float factor,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI DumpViewHierarchyWithProperties(
        /* [in] */ IBufferedWriter* out,
        /* [in] */ Int32 level);

    // @Override
    CARAPI FindHierarchyView(
        /* [in] */ const String& className,
        /* [in] */ Int32 hashCode,
        /* [out] */ IView** result);

    // WebViewProvider glue methods ---------------------------------------------------------------
    // @Override
    // This needs to be kept thread safe!
    CARAPI GetViewDelegate(
        /* [out] */ IWebViewProviderViewDelegate** result);

    // @Override
    // This needs to be kept thread safe!
    CARAPI GetScrollDelegate(
        /* [out] */ IWebViewProviderScrollDelegate** result);

    // WebViewProvider.ViewDelegate implementation ------------------------------------------------
    // TODO: remove from WebViewProvider and use default implementation from
    // ViewGroup.
    // @Override
    CARAPI ShouldDelayChildPressedState(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetAccessibilityNodeProvider(
        /* [out] */ IAccessibilityNodeProvider** result);

    // @Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    // @Override
    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    // @Override
    CARAPI PerformAccessibilityAction(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetOverScrollMode(
        /* [in] */ Int32 mode);

    // @Override
    CARAPI SetScrollBarStyle(
        /* [in] */ Int32 style);

    // @Override
    CARAPI OnDrawVerticalScrollBar(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IDrawable* scrollBar,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    // @Override
    CARAPI OnOverScrolled(
        /* [in] */ Int32 scrollX,
        /* [in] */ Int32 scrollY,
        /* [in] */ Boolean clampedX,
        /* [in] */ Boolean clampedY);

    // @Override
    CARAPI OnWindowVisibilityChanged(
        /* [in] */ Int32 visibility);

    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    // @Override
    CARAPI SetLayoutParams(
        /* [in] */ IViewGroupLayoutParams* layoutParams);

    // @Override
    CARAPI PerformLongClick(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    // @Override
    CARAPI OnCreateInputConnection(
        /* [in] */ IEditorInfo* outAttrs,
        /* [out] */ IInputConnection** result);

    // @Override
    CARAPI OnKeyMultiple(
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 repeatCount,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI OnDetachedFromWindow();

    // @Override
    CARAPI OnVisibilityChanged(
        /* [in] */ IView* changedView,
        /* [in] */ Int32 visibility);

    // @Override
    CARAPI OnWindowFocusChanged(
        /* [in] */ Boolean hasWindowFocus);

    // @Override
    CARAPI OnFocusChanged(
        /* [in] */ Boolean focused,
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

    // @Override
    CARAPI SetFrame(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 ow,
        /* [in] */ Int32 oh);

    // @Override
    CARAPI OnScrollChanged(
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 oldl,
        /* [in] */ Int32 oldt);

    // @Override
    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnHoverEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnTrackballEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI RequestFocus(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI RequestChildRectangleOnScreen(
        /* [in] */ IView* child,
        /* [in] */ IRect* rect,
        /* [in] */ Boolean immediate,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetBackgroundColor(
        /* [in] */ Int32 color);

    // @Override
    CARAPI SetLayerType(
        /* [in] */ Int32 layerType,
        /* [in] */ IPaint* paint);

    // Remove from superclass
    virtual CARAPI PreDispatchDraw(
        /* [in] */ ICanvas* canvas);

    virtual CARAPI OnStartTemporaryDetach();

    virtual CARAPI OnFinishTemporaryDetach();

    // WebViewProvider.ScrollDelegate implementation ----------------------------------------------
    // @Override
    CARAPI ComputeHorizontalScrollRange(
        /* [out] */ Int32* result);

    // @Override
    CARAPI ComputeHorizontalScrollOffset(
        /* [out] */ Int32* result);

    // @Override
    CARAPI ComputeVerticalScrollRange(
        /* [out] */ Int32* result);

    // @Override
    CARAPI ComputeVerticalScrollOffset(
        /* [out] */ Int32* result);

    // @Override
    CARAPI ComputeVerticalScrollExtent(
        /* [out] */ Int32* result);

    // @Override
    CARAPI ComputeScroll();

    // TODO(sgurun) this is only to have master-gpl compiling.
    virtual CARAPI_(AutoPtr<IInterface>) CreatePrintDocumentAdapter();  /*IPrintDocumentAdapter*/

    //@Override TODO(sgurun) commenting this out to have master-gpl compiling.
    virtual CARAPI_(AutoPtr<IInterface/*IPrintDocumentAdapter*/>) CreatePrintDocumentAdapter(
        /* [in] */ const String& documentName);

    // Implements SmartClipProvider
    // @Override
    CARAPI ExtractSmartClipData(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    // Implements SmartClipProvider
    // @Override
    CARAPI SetSmartClipResultHandler(
        /* [in] */ IHandler* resultHandler);

public:
    // The WebView that this WebViewChromium is the provider for.
    AutoPtr<IWebView> mWebView;
    // Lets us access protected View-derived methods on the WebView instance we're backing.
    AutoPtr<IWebViewPrivateAccess> mWebViewPrivate;

private:
    CARAPI RunBlockingFuture(
        /* [in] */ FutureTask* task,
        /* [out] */ IInterface** result);

    // We have a 4 second timeout to try to detect deadlocks to detect and aid in debuggin
    // deadlocks.
    // Do not call this method while on the UI thread!
    CARAPI RunVoidTaskOnUiThreadBlocking(
        /* [in] */ IRunnable* r);

    CARAPI_(AutoPtr<IInterface>) RunOnUiThreadBlocking(
        /* [in] */ ICallable* c);

    // Wrap Context so that we can use resources from the webview resource apk.
    static CARAPI_(AutoPtr<IContext>) ResourcesContextWrapper(
        /* [in] */ IContext* ctx);

    CARAPI InitForReal();

    // exception, note it temporarily
    // CARAPI_(AutoPtr<RuntimeException>) CreateThreadException();

    CARAPI CheckNeedsPost(
        /* [out] */ Boolean* result);

    //  Intentionally not static, as no need to check thread on static methods
    CARAPI CheckThread();

    static CARAPI_(String) FixupMimeType(
        /* [in] */ const String& mimeType);

    static CARAPI_(String) FixupData(
        /* [in] */ const String& data);

    static CARAPI_(String) FixupBase(
        /* [in] */ const String& url);

    static CARAPI_(String) FixupHistory(
        /* [in] */ const String& url);

    static CARAPI_(Boolean) IsBase64Encoded(
        /* [in] */ const String& encoding);

    CARAPI LoadUrlOnUiThread(
        /* [in] */ LoadUrlParams* loadUrlParams);

    /**
     * Returns true if the supplied {@link WebChromeClient} supports fullscreen.
     *
     * <p>For fullscreen support, implementations of {@link WebChromeClient#onShowCustomView}
     * and {@link WebChromeClient#onHideCustomView()} are required.
     */
    CARAPI_(Boolean) DoesSupportFullscreen(
        /* [in] */ IWebChromeClient* client);

private:
    AutoPtr<WebViewChromiumRunQueue> mRunQueue;
    static const String TAG;
    // The client adapter class.
    AutoPtr<WebViewContentsClientAdapter> mContentsClientAdapter;
    // Variables for functionality provided by this adapter ---------------------------------------
    AutoPtr<ContentSettingsAdapter> mWebSettings;
    // The WebView wrapper for ContentViewCore and required browser compontents.
    AutoPtr<AwContents> mAwContents;
    // Non-null if this webview is using the GL accelerated draw path.
    AutoPtr<DrawGLFunctor> mGLfunctor;
    /*const*/ AutoPtr<IWebViewHitTestResult> mHitTestResult;
    /*const*/ Int32 mAppTargetSdkVersion;
    AutoPtr<IWebViewChromiumFactoryProvider> mFactory;
    static Boolean sRecordWholeDocumentEnabledByApi;
};

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBVIEWCHROMIUM_H_

