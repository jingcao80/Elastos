
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Webkit.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/webkit/webview/chromium/R_Chromium.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/webkit/FindActionModeCallback.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwBrowserContext.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContentsClient.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContentsStatics.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwSettings.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/JavascriptInterface.h"
#include "elastos/droid/webkit/webview/chromium/UnimplementedWebViewApi.h"
#include "elastos/droid/webkit/webview/chromium/WebBackForwardListChromium.h"
#include "elastos/droid/webkit/webview/chromium/WebViewChromium.h"
#include "elastos/core/CoreUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::EIID_IContextWrapper;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CBase64;
using Elastos::Droid::Utility::IBase64;
using Elastos::Droid::View::CView;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewRootImpl;
using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::AwBrowserContext;
using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::AwContentsClient;
using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::AwContentsStatics;
using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::AwSettings;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::JavascriptInterface;
using Elastos::Droid::Webkit::CWebViewFactory;
using Elastos::Droid::Webkit::CWebViewHitTestResult;
using Elastos::Droid::Webkit::FindActionModeCallback;
using Elastos::Droid::Webkit::IWebViewFactory;
using Elastos::Droid::Webkit::Webview::Chromium::UnimplementedWebViewApi;
using Elastos::Droid::Webkit::Webview::Chromium::WebBackForwardListChromium;
using Elastos::Droid::Widget::CTextView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Core::CString;
using Elastos::Core::CThread;
using Elastos::Core::EIID_IClassLoader;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Concurrent::CConcurrentLinkedQueue;
using Elastos::Utility::Concurrent::CTimeUnitHelper;
using Elastos::Utility::Concurrent::EIID_ICallable;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastos::Utility::Concurrent::ITimeUnitHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

//=====================================================================
//  WebViewChromium::WebViewChromiumRunQueue::InnerDrainQueueRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::WebViewChromiumRunQueue::InnerDrainQueueRunnable, Object, IRunnable)

WebViewChromium::WebViewChromiumRunQueue::InnerDrainQueueRunnable::InnerDrainQueueRunnable(
    /* [in] */ WebViewChromiumRunQueue* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::WebViewChromiumRunQueue::InnerDrainQueueRunnable::Run()
{
    // ==================before translated======================
    // drainQueue();

    assert(NULL != mOwner);
    mOwner->DrainQueue();
    return NOERROR;
}

//=====================================================================
//               WebViewChromium::WebViewChromiumRunQueue
//=====================================================================
WebViewChromium::WebViewChromiumRunQueue::WebViewChromiumRunQueue(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mQueue = new ConcurrentLinkedQueue<Runnable>();

    CConcurrentLinkedQueue::New((IQueue**)&mQueue);
}

ECode WebViewChromium::WebViewChromiumRunQueue::AddTask(
    /* [in] */ IRunnable* task)
{
    VALIDATE_NOT_NULL(task);
    // ==================before translated======================
    // mQueue.add(task);
    // if (mFactory.hasStarted()) {
    //     ThreadUtils.runOnUiThread(new Runnable() {
    //         @Override
    //         public void run() {
    //             drainQueue();
    //         }
    //     });
    // }

    assert(NULL != mOwner);
    mQueue->Add(task);
    Boolean hasStarted = FALSE;
    mOwner->mFactory->HasStarted(&hasStarted);
    if (hasStarted) {
        AutoPtr<IRunnable> runnable = new WebViewChromiumRunQueue::InnerDrainQueueRunnable(this);
        ThreadUtils::RunOnUiThread(runnable);
    }
    return NOERROR;
}

ECode WebViewChromium::WebViewChromiumRunQueue::DrainQueue()
{
    // ==================before translated======================
    // if (mQueue == null || mQueue.isEmpty()) {
    //     return;
    // }
    //
    // Runnable task = mQueue.poll();
    // while(task != null) {
    //     task.run();
    //     task = mQueue.poll();
    // }

    Boolean empty = FALSE;
    mQueue->IsEmpty(&empty);
    if (mQueue == NULL || empty) {
        return NOERROR;
    }

    AutoPtr<IInterface> taskTmp;
    mQueue->Poll((IInterface**)&taskTmp);
    IRunnable* task = IRunnable::Probe(taskTmp);
    while(task != NULL) {
        task->Run();
        taskTmp = NULL;
        mQueue->Poll((IInterface**)&taskTmp);
        task = IRunnable::Probe(taskTmp);
    }
    return NOERROR;
}

//=====================================================================
//               WebViewChromium::InnerInitForRealRunnable
//=====================================================================
WebViewChromium::InnerInitForRealRunnable::InnerInitForRealRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Boolean privateBrowsing)
    : mOwner(owner)
    , mPrivateBrowsing(privateBrowsing)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerInitForRealRunnable::Run()
{
    // ==================before translated======================
    // initForReal();
    // if (privateBrowsing) {
    //     // Intentionally irreversibly disable the webview instance, so that private
    //     // user data cannot leak through misuse of a non-privateBrowing WebView
    //     // instance. Can't just null out mAwContents as we never null-check it
    //     // before use.
    //     destroy();
    // }

    mOwner->InitForReal();
    if (mPrivateBrowsing) {
        // Intentionally irreversibly disable the webview instance, so that private
        // user data cannot leak through misuse of a non-privateBrowing WebView
        // instance. Can't just null out mAwContents as we never null-check it
        // before use.
        mOwner->Destroy();
    }
    return NOERROR;
}

//=====================================================================
//       WebViewChromium::InnerContextWrapper::InnerClassLoader
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerContextWrapper::InnerClassLoader, Object, IClassLoader)

WebViewChromium::InnerContextWrapper::InnerClassLoader::InnerClassLoader(
    /* [in] */ InnerContextWrapper* owner,
    /* [in] */ IClassLoader* appCl,
    /* [in] */ IClassLoader* webViewCl)
    : mOwner(owner)
    , mAppCl(appCl)
    , mWebViewCl(webViewCl)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerContextWrapper::InnerClassLoader::FindClass(
    /* [in] */ const String& name,
    /* [out] */ IClassInfo** klass)
{
    VALIDATE_NOT_NULL(klass);
    // ==================before translated======================
    // // First look in the WebViewProvider class loader.
    // try {
    //     return webViewCl.loadClass(name);
    // } catch (ClassNotFoundException e) {
    //     // Look in the app class loader; allowing it to throw ClassNotFoundException.
    //     return appCl.loadClass(name);
    // }

    assert(NULL != mAppCl);
    assert(NULL != mWebViewCl);
    // First look in the WebViewProvider class loader.
    //try {
        return mWebViewCl->LoadClass(name, klass);
    //}
    //catch (ClassNotFoundException e) {
        // Look in the app class loader; allowing it to throw ClassNotFoundException.
        //return mAppCl->LoadClass(name, (IClassInfo**)&klass);
    //}
    return NOERROR;
}

ECode WebViewChromium::InnerContextWrapper::InnerClassLoader::LoadClass(
    /* [in] */ const String& name,
    /* [out] */ IClassInfo** klass)
{
    VALIDATE_NOT_NULL(klass);
    *klass = NULL;
    //-- only overwrite pure func to make compile
    return NOERROR;
}


//=====================================================================
//                WebViewChromium::InnerContextWrapper
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerContextWrapper, Object, IContextWrapper)

WebViewChromium::InnerContextWrapper::InnerContextWrapper()
{
}

ECode WebViewChromium::InnerContextWrapper::GetClassLoader(
    /* [out] */ IClassLoader** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // final ClassLoader appCl = getBaseContext().getClassLoader();
    // final ClassLoader webViewCl = this.getClass().getClassLoader();
    // return new ClassLoader() {
    //     @Override
    //     protected Class<?> findClass(String name) throws ClassNotFoundException {
    //         // First look in the WebViewProvider class loader.
    //         try {
    //             return webViewCl.loadClass(name);
    //         } catch (ClassNotFoundException e) {
    //             // Look in the app class loader; allowing it to throw ClassNotFoundException.
    //             return appCl.loadClass(name);
    //         }
    //     }
    // };

    assert(0);
    AutoPtr<IContext> baseContext;
    GetBaseContext((IContext**)&baseContext);
    AutoPtr<IClassLoader> appCl;
    baseContext->GetClassLoader((IClassLoader**)&appCl);

    AutoPtr<IClassLoader> webViewCl;
    //final ClassLoader webViewCl = this.getClass().getClassLoader();

    *result = new InnerContextWrapper::InnerClassLoader(this, appCl, webViewCl);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//ECode WebViewChromium::InnerContextWrapper::GetSystemService(
//    /* [in] */ const String& name,
//    /* [out] */ IInterface** object)
//{
//    VALIDATE_NOT_NULL(object);
//    // ==================before translated======================
//    // if (name.equals(Context.LAYOUT_INFLATER_SERVICE)) {
//    //     LayoutInflater i = (LayoutInflater) getBaseContext().getSystemService(name);
//    //     return i.cloneInContext(this);
//    // } else {
//    //     return getBaseContext().getSystemService(name);
//    // }

//    assert(0);
//    AutoPtr<IContext> baseContext;
//    GetBaseContext((IContext**)&baseContext);
//    if (name.Equals(IContext::LAYOUT_INFLATER_SERVICE)) {
//        AutoPtr<IInterface> iTmp;
//        baseContext->GetSystemService(name, (IInterface**)&iTmp);
//        AutoPtr<ILayoutInflater> i = ILayoutInflater::Probe(iTmp);

//        AutoPtr<IContext> contextTmp = this;
//        AutoPtr<ILayoutInflater> cloneTmp;
//        i->CloneInContext(contextTmp, (ILayoutInflater**)&cloneTmp);
//        *object = cloneTmp->Probe(EIID_IInterface);
//        REFCOUNT_ADD(*object);
//    }
//    else {
//        baseContext->GetSystemService(name, object);
//    }
//    return NOERROR;
//}

ECode WebViewChromium::InnerContextWrapper::GetBaseContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context);
    *context = NULL;
    // just overwrite for compile
    return NOERROR;
}

//=====================================================================
//           WebViewChromium::InnerThreadViolationRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerThreadViolationRunnable, Object, IRunnable)

WebViewChromium::InnerThreadViolationRunnable::InnerThreadViolationRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerThreadViolationRunnable::Run()
{
    // ==================before translated======================
    // throw threadViolation;
    return NOERROR;
}

//=====================================================================
//     WebViewChromium::InnerSetHorizontalScrollbarOverlayRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerSetHorizontalScrollbarOverlayRunnable, Object, IRunnable)

WebViewChromium::InnerSetHorizontalScrollbarOverlayRunnable::InnerSetHorizontalScrollbarOverlayRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Boolean overlay)
    : mOwner(owner)
    , mOverlay(overlay)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerSetHorizontalScrollbarOverlayRunnable::Run()
{
    // ==================before translated======================
    // setHorizontalScrollbarOverlay(overlay);

    assert(NULL != mOwner);
    mOwner->SetHorizontalScrollbarOverlay(mOverlay);
    return NOERROR;
}

//=====================================================================
//      WebViewChromium::InnerSetVerticalScrollbarOverlayRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerSetVerticalScrollbarOverlayRunnable, Object, IRunnable)

WebViewChromium::InnerSetVerticalScrollbarOverlayRunnable::InnerSetVerticalScrollbarOverlayRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Boolean overlay)
    : mOwner(owner)
    , mOverlay(overlay)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerSetVerticalScrollbarOverlayRunnable::Run()
{
    // ==================before translated======================
    // setVerticalScrollbarOverlay(overlay);

    assert(NULL != mOwner);
    mOwner->SetVerticalScrollbarOverlay(mOverlay);
    return NOERROR;
}

//=====================================================================
//      WebViewChromium::InnerOverlayHorizontalScrollbarCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOverlayHorizontalScrollbarCallable, Object, ICallable)

WebViewChromium::InnerOverlayHorizontalScrollbarCallable::InnerOverlayHorizontalScrollbarCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOverlayHorizontalScrollbarCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return overlayHorizontalScrollbar();

    assert(NULL != mOwner);
    Boolean resTmp = FALSE;
    mOwner->OverlayHorizontalScrollbar(&resTmp);
    AutoPtr<IBoolean> resTmp1 = CoreUtils::Convert(resTmp);
    *result = TO_IINTERFACE(resTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//        WebViewChromium::InnerOverlayVerticalScrollbarCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOverlayVerticalScrollbarCallable, Object, ICallable)

WebViewChromium::InnerOverlayVerticalScrollbarCallable::InnerOverlayVerticalScrollbarCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOverlayVerticalScrollbarCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return overlayVerticalScrollbar();

    assert(NULL != mOwner);
    Boolean resTmp = FALSE;
    mOwner->OverlayVerticalScrollbar(&resTmp);
    AutoPtr<IBoolean> resTmp1 = CoreUtils::Convert(resTmp);
    *result = TO_IINTERFACE(resTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//           WebViewChromium::InnerGetCertificateCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerGetCertificateCallable, Object, ICallable)

WebViewChromium::InnerGetCertificateCallable::InnerGetCertificateCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerGetCertificateCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getCertificate();

    assert(NULL != mOwner);
    AutoPtr<ISslCertificate> resTmp;
    mOwner->GetCertificate((ISslCertificate**)&resTmp);
    *result = TO_IINTERFACE(resTmp);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//       WebViewChromium::InnerSetHttpAuthUsernamePasswordRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerSetHttpAuthUsernamePasswordRunnable, Object, IRunnable)

WebViewChromium::InnerSetHttpAuthUsernamePasswordRunnable::InnerSetHttpAuthUsernamePasswordRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const String& host,
    /* [in] */ const String& realm,
    /* [in] */ const String& username,
    /* [in] */ const String& password)
    : mOwner(owner)
    , mHost(host)
    , mRealm(realm)
    , mUsername(username)
    , mPassword(password)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerSetHttpAuthUsernamePasswordRunnable::Run()
{
    // ==================before translated======================
    // setHttpAuthUsernamePassword(host, realm, username, password);

    assert(NULL != mOwner);
    mOwner->SetHttpAuthUsernamePassword(mHost, mRealm, mUsername, mPassword);
    return NOERROR;
}

//=====================================================================
//      WebViewChromium::InnerGetHttpAuthUsernamePasswordCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerGetHttpAuthUsernamePasswordCallable, Object, ICallable)

WebViewChromium::InnerGetHttpAuthUsernamePasswordCallable::InnerGetHttpAuthUsernamePasswordCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const String& host,
    /* [in] */ const String& realm)
    : mOwner(owner)
    , mHost(host)
    , mRealm(realm)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerGetHttpAuthUsernamePasswordCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getHttpAuthUsernamePassword(host, realm);

    assert(NULL != mOwner);
    AutoPtr< ArrayOf<String> > resTmp;
    mOwner->GetHttpAuthUsernamePassword(mHost, mRealm, (ArrayOf<String>**)&resTmp);

    AutoPtr<IList> list;
    CArrayList::New(resTmp->GetLength(), (IList**)&list);
    String str;
    for (Int32 idx=0; idx<resTmp->GetLength(); ++idx) {
        str = (*resTmp)[idx];
        AutoPtr<ICharSequence> charSequenceTmp;
        CString::New(str, (ICharSequence**)&charSequenceTmp);
        AutoPtr<IInterface> interfaceTmp = TO_IINTERFACE(charSequenceTmp);
        list->Set(idx, charSequenceTmp);
    }

    *result = TO_IINTERFACE(list);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//                 WebViewChromium::InnerDestroyRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerDestroyRunnable, Object, IRunnable)

WebViewChromium::InnerDestroyRunnable::InnerDestroyRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerDestroyRunnable::Run()
{
    // ==================before translated======================
    // destroy();

    assert(NULL != mOwner);
    mOwner->Destroy();
    return NOERROR;
}

//=====================================================================
//        WebViewChromium::InnerSetNetworkAvailableRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerSetNetworkAvailableRunnable, Object, IRunnable)

WebViewChromium::InnerSetNetworkAvailableRunnable::InnerSetNetworkAvailableRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Boolean networkUp)
    : mOwner(owner)
    , mNetworkUp(networkUp)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerSetNetworkAvailableRunnable::Run()
{
    // ==================before translated======================
    // setNetworkAvailable(networkUp);

    assert(NULL != mOwner);
    mOwner->SetNetworkAvailable(mNetworkUp);
    return NOERROR;
}

//=====================================================================
//                WebViewChromium::InnerSaveStateCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerSaveStateCallable, Object, ICallable)

WebViewChromium::InnerSaveStateCallable::InnerSaveStateCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ IBundle* outState)
    : mOwner(owner)
    , mOutState(outState)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerSaveStateCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return saveState(outState);

    assert(NULL != mOwner);
    AutoPtr<IWebBackForwardList> tmp;
    mOwner->SaveState(mOutState, (IWebBackForwardList**)&tmp);
    *result = TO_IINTERFACE(tmp);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//              WebViewChromium::InnerRestoreStateCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerRestoreStateCallable, Object, ICallable)

WebViewChromium::InnerRestoreStateCallable::InnerRestoreStateCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ IBundle* inState)
    : mOwner(owner)
    , mInState(inState)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerRestoreStateCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return restoreState(inState);

    assert(NULL != mOwner);
    AutoPtr<IWebBackForwardList> tmp;
    mOwner->RestoreState(mInState, (IWebBackForwardList**)&tmp);
    *result = TO_IINTERFACE(tmp);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//           WebViewChromium::InnerEvaluateJavaScriptRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerEvaluateJavaScriptRunnable, Object, ICallable)

WebViewChromium::InnerEvaluateJavaScriptRunnable::InnerEvaluateJavaScriptRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const String& url,
    /* [in] */ const String& javascriptScheme)
    : mOwner(owner)
    , mUrl(url)
    , mJavascriptScheme(javascriptScheme)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerEvaluateJavaScriptRunnable::Run()
{
    // ==================before translated======================
    // mAwContents.evaluateJavaScriptEvenIfNotYetNavigated(
    //         url.substring(JAVASCRIPT_SCHEME.length()));

    assert(NULL != mOwner);
    mOwner->mAwContents->EvaluateJavaScriptEvenIfNotYetNavigated(
        mUrl.Substring(mJavascriptScheme.GetLength()));
    return NOERROR;
}

//=====================================================================
//               WebViewChromium::InnerLoadUrlRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerLoadUrlRunnable, Object, IRunnable)

WebViewChromium::InnerLoadUrlRunnable::InnerLoadUrlRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ LoadUrlParams* loadUrlParams)
    : mOwner(owner)
    , mLoadUrlParams(loadUrlParams)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerLoadUrlRunnable::Run()
{
    // ==================before translated======================
    // mAwContents.loadUrl(loadUrlParams);

    assert(NULL != mOwner);
    mOwner->mAwContents->LoadUrl(mLoadUrlParams);
    return NOERROR;
}

//=====================================================================
//            WebViewChromium::InnerSaveWebArchiveRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerSaveWebArchiveRunnable, Object, IRunnable)

WebViewChromium::InnerSaveWebArchiveRunnable::InnerSaveWebArchiveRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const String& basename,
    /* [in] */ Boolean autoname,
    /* [in] */ IValueCallback* callback)
    : mOwner(owner)
    , mBasename(basename)
    , mAutoname(autoname)
    , mCallback(callback)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerSaveWebArchiveRunnable::Run()
{
    // ==================before translated======================
    // saveWebArchive(basename, autoname, callback);

    assert(NULL != mOwner);
    mOwner->SaveWebArchive(mBasename, mAutoname, mCallback);
    return NOERROR;
}

//=====================================================================
//               WebViewChromium::InnerStopLoadingRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerStopLoadingRunnable, Object, IRunnable)

WebViewChromium::InnerStopLoadingRunnable::InnerStopLoadingRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerStopLoadingRunnable::Run()
{
    // ==================before translated======================
    // stopLoading();

    assert(NULL != mOwner);
    mOwner->StopLoading();
    return NOERROR;
}

//=====================================================================
//               WebViewChromium::InnerReloadRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerReloadRunnable, Object, IRunnable)

WebViewChromium::InnerReloadRunnable::InnerReloadRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerReloadRunnable::Run()
{
    // ==================before translated======================
    // reload();

    assert(NULL != mOwner);
    mOwner->Reload();
    return NOERROR;
}

//=====================================================================
//               WebViewChromium::InnerCanGoBackCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerCanGoBackCallable, Object, ICallable)

WebViewChromium::InnerCanGoBackCallable::InnerCanGoBackCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerCanGoBackCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return canGoBack();

    assert(NULL != mOwner);
    Boolean canGoBackTmp = FALSE;
    mOwner->CanGoBack(&canGoBackTmp);
    AutoPtr<IBoolean> canGoBackTmp1 = CoreUtils::Convert(canGoBackTmp);
    *result = TO_IINTERFACE(canGoBackTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//                   WebViewChromium::InnerGoBackRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerGoBackRunnable, Object, IRunnable)

WebViewChromium::InnerGoBackRunnable::InnerGoBackRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerGoBackRunnable::Run()
{
    // ==================before translated======================
    // goBack();

    assert(NULL != mOwner);
    return mOwner->GoBack();
}

//=====================================================================
//              WebViewChromium::InnerCanGoForwardCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerCanGoForwardCallable, Object, ICallable)

WebViewChromium::InnerCanGoForwardCallable::InnerCanGoForwardCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerCanGoForwardCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return canGoForward();

    assert(NULL != mOwner);
    Boolean canGoForwardTmp = FALSE;
    mOwner->CanGoForward(&canGoForwardTmp);
    AutoPtr<IBoolean> canGoForwardTmp1 = CoreUtils::Convert(canGoForwardTmp);
    *result = TO_IINTERFACE(canGoForwardTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//                 WebViewChromium::InnerGoForwardRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerGoForwardRunnable, Object, IRunnable)

WebViewChromium::InnerGoForwardRunnable::InnerGoForwardRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerGoForwardRunnable::Run()
{
    // ==================before translated======================
    // goForward();

    assert(NULL != mOwner);
    return mOwner->GoForward();
}

//=====================================================================
//           WebViewChromium::InnerCanGoBackOrForwardCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerCanGoBackOrForwardCallable, Object, ICallable)

WebViewChromium::InnerCanGoBackOrForwardCallable::InnerCanGoBackOrForwardCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Int32 steps)
    : mOwner(owner)
    , mSteps(steps)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerCanGoBackOrForwardCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return canGoBackOrForward(steps);

    assert(NULL != mOwner);
    Boolean canGoBackOrForwardTmp = FALSE;
    mOwner->CanGoBackOrForward(mSteps, &canGoBackOrForwardTmp);
    AutoPtr<IBoolean> canGoBackOrForwardTmp1 = CoreUtils::Convert(canGoBackOrForwardTmp);
    *result = TO_IINTERFACE(canGoBackOrForwardTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//              WebViewChromium::InnerGoBackOrForwardRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerGoBackOrForwardRunnable, Object, IRunnable)

WebViewChromium::InnerGoBackOrForwardRunnable::InnerGoBackOrForwardRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Int32 steps)
    : mOwner(owner)
    , mSteps(steps)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerGoBackOrForwardRunnable::Run()
{
    // ==================before translated======================
    // goBackOrForward(steps);

    assert(NULL != mOwner);
    return mOwner->GoBackOrForward(mSteps);
}

//=====================================================================
//                    WebViewChromium::InnerPageUpCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerPageUpCallable, Object, ICallable)

WebViewChromium::InnerPageUpCallable::InnerPageUpCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Boolean top)
    : mOwner(owner)
    , mTop(top)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerPageUpCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return pageUp(top);

    assert(NULL != mOwner);
    Boolean pageUpTmp = FALSE;
    mOwner->PageUp(mTop, &pageUpTmp);
    AutoPtr<IBoolean> pageUpTmp1 = CoreUtils::Convert(pageUpTmp);
    *result = TO_IINTERFACE(pageUpTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//                    WebViewChromium::InnerPageDownCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerPageDownCallable, Object, ICallable)

WebViewChromium::InnerPageDownCallable::InnerPageDownCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Boolean bottom)
    : mOwner(owner)
    , mBottom(bottom)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerPageDownCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return pageDown(bottom);

    assert(NULL != mOwner);
    Boolean pageDownTmp = FALSE;
    mOwner->PageDown(mBottom, &pageDownTmp);
    AutoPtr<IBoolean> pageDownTmp1 = CoreUtils::Convert(pageDownTmp);
    *result = TO_IINTERFACE(pageDownTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//                   WebViewChromium::InnerClearViewRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerClearViewRunnable, Object, IRunnable)

WebViewChromium::InnerClearViewRunnable::InnerClearViewRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerClearViewRunnable::Run()
{
    // ==================before translated======================
    // clearView();

    assert(NULL != mOwner);
    return mOwner->ClearView();
}

//=====================================================================
//              WebViewChromium::InnerCapturePictureCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerCapturePictureCallable, Object, ICallable)

WebViewChromium::InnerCapturePictureCallable::InnerCapturePictureCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerCapturePictureCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return capturePicture();

    assert(NULL != mOwner);
    AutoPtr<IPicture> picture;
    mOwner->CapturePicture((IPicture**)&picture);
    *result = TO_IINTERFACE(picture);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//             WebViewChromium::InnerInvokeZoomPickerRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerInvokeZoomPickerRunnable, Object, IRunnable)

WebViewChromium::InnerInvokeZoomPickerRunnable::InnerInvokeZoomPickerRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerInvokeZoomPickerRunnable::Run()
{
    // ==================before translated======================
    // invokeZoomPicker();

    assert(NULL != mOwner);
    return mOwner->InvokeZoomPicker();
}

//=====================================================================
//            WebViewChromium::InnerGetHitTestResultCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerGetHitTestResultCallable, Object, ICallable)

WebViewChromium::InnerGetHitTestResultCallable::InnerGetHitTestResultCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerGetHitTestResultCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getHitTestResult();

    assert(NULL != mOwner);
    AutoPtr<IWebViewHitTestResult> hitTestResult;
    mOwner->GetHitTestResult((IWebViewHitTestResult**)&hitTestResult);
    *result = TO_IINTERFACE(hitTestResult);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//           WebViewChromium::InnerRequestFocusNodeHrefRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerRequestFocusNodeHrefRunnable, Object, IRunnable)

WebViewChromium::InnerRequestFocusNodeHrefRunnable::InnerRequestFocusNodeHrefRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ IMessage* hrefMsg)
    : mOwner(owner)
    , mHrefMsg(hrefMsg)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerRequestFocusNodeHrefRunnable::Run()
{
    // ==================before translated======================
    // requestFocusNodeHref(hrefMsg);

    assert(NULL != mOwner);
    return mOwner->RequestFocusNodeHref(mHrefMsg);
}

//=====================================================================
//              WebViewChromium::InnerRequestImageRefRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerRequestImageRefRunnable, Object, IRunnable)

WebViewChromium::InnerRequestImageRefRunnable::InnerRequestImageRefRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ IMessage* msg)
    : mOwner(owner)
    , mMsg(msg)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerRequestImageRefRunnable::Run()
{
    // ==================before translated======================
    // requestImageRef(msg);

    assert(NULL != mOwner);
    return mOwner->RequestImageRef(mMsg);
}

//=====================================================================
//                    WebViewChromium::InnerGetUrlCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerGetUrlCallable, Object, ICallable)

WebViewChromium::InnerGetUrlCallable::InnerGetUrlCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerGetUrlCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getUrl();

    assert(NULL != mOwner);
    String urlTmp;
    mOwner->GetUrl(&urlTmp);
    AutoPtr<ICharSequence> charSequenceTmp;
    CString::New(urlTmp, (ICharSequence**)&charSequenceTmp);
    *result = TO_IINTERFACE(charSequenceTmp);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//              WebViewChromium::InnerGetOriginalUrlCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerGetOriginalUrlCallable, Object, ICallable)

WebViewChromium::InnerGetOriginalUrlCallable::InnerGetOriginalUrlCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerGetOriginalUrlCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getOriginalUrl();

    assert(NULL != mOwner);
    String urlTmp;
    mOwner->GetOriginalUrl(&urlTmp);
    AutoPtr<ICharSequence> charSequenceTmp;
    CString::New(urlTmp, (ICharSequence**)&charSequenceTmp);
    *result = TO_IINTERFACE(charSequenceTmp);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//                WebViewChromium::InnerGetTitleCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerGetTitleCallable, Object, ICallable)

WebViewChromium::InnerGetTitleCallable::InnerGetTitleCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerGetTitleCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getTitle();

    assert(NULL != mOwner);
    String titleTmp;
    mOwner->GetTitle(&titleTmp);
    AutoPtr<ICharSequence> charSequenceTmp;
    CString::New(titleTmp, (ICharSequence**)&charSequenceTmp);
    *result = TO_IINTERFACE(charSequenceTmp);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//               WebViewChromium::InnerGetFaviconCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerGetFaviconCallable, Object, ICallable)

WebViewChromium::InnerGetFaviconCallable::InnerGetFaviconCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerGetFaviconCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getFavicon();

    assert(NULL != mOwner);
    AutoPtr<IBitmap> faviconTmp;
    mOwner->GetFavicon((IBitmap**)&faviconTmp);
    *result = TO_IINTERFACE(faviconTmp);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//             WebViewChromium::InnerPauseTimersRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerPauseTimersRunnable, Object, IRunnable)

WebViewChromium::InnerPauseTimersRunnable::InnerPauseTimersRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerPauseTimersRunnable::Run()
{
    // ==================before translated======================
    // pauseTimers();

    assert(NULL != mOwner);
    return mOwner->PauseTimers();
}

//=====================================================================
//              WebViewChromium::InnerResumeTimersRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerResumeTimersRunnable, Object, IRunnable)

WebViewChromium::InnerResumeTimersRunnable::InnerResumeTimersRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerResumeTimersRunnable::Run()
{
    // ==================before translated======================
    // resumeTimers();

    assert(NULL != mOwner);
    return mOwner->ResumeTimers();
}

//=====================================================================
//                WebViewChromium::InnerOnPauseRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOnPauseRunnable, Object, IRunnable)

WebViewChromium::InnerOnPauseRunnable::InnerOnPauseRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOnPauseRunnable::Run()
{
    // ==================before translated======================
    // onPause();

    assert(NULL != mOwner);
    return mOwner->OnPause();
}

//=====================================================================
//                WebViewChromium::InnerOnResumeRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOnResumeRunnable, Object, IRunnable)

WebViewChromium::InnerOnResumeRunnable::InnerOnResumeRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOnResumeRunnable::Run()
{
    // ==================before translated======================
    // onResume();

    assert(NULL != mOwner);
    return mOwner->OnResume();
}

//=====================================================================
//               WebViewChromium::InnerIsPausedCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerIsPausedCallable, Object, ICallable)

WebViewChromium::InnerIsPausedCallable::InnerIsPausedCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerIsPausedCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return isPaused();

    assert(NULL != mOwner);
    Boolean isPausedTmp = FALSE;
    mOwner->IsPaused(&isPausedTmp);
    AutoPtr<IBoolean> isPausedTmp1 = CoreUtils::Convert(isPausedTmp);
    *result = TO_IINTERFACE(isPausedTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//              WebViewChromium::InnerClearCacheRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerClearCacheRunnable, Object, IRunnable)

WebViewChromium::InnerClearCacheRunnable::InnerClearCacheRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Boolean includeDiskFiles)
    : mOwner(owner)
    , mIncludeDiskFiles(includeDiskFiles)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerClearCacheRunnable::Run()
{
    // ==================before translated======================
    // clearCache(includeDiskFiles);

    assert(NULL != mOwner);
    return mOwner->ClearCache(mIncludeDiskFiles);
}

//=====================================================================
//            WebViewChromium::InnerClearFormDataRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerClearFormDataRunnable, Object, IRunnable)

WebViewChromium::InnerClearFormDataRunnable::InnerClearFormDataRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerClearFormDataRunnable::Run()
{
    // ==================before translated======================
    // clearFormData();

    assert(NULL != mOwner);
    return mOwner->ClearFormData();
}

//=====================================================================
//             WebViewChromium::InnerClearHistoryRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerClearHistoryRunnable, Object, IRunnable)

WebViewChromium::InnerClearHistoryRunnable::InnerClearHistoryRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerClearHistoryRunnable::Run()
{
    // ==================before translated======================
    // clearHistory();

    assert(NULL != mOwner);
    return mOwner->ClearHistory();
}

//=====================================================================
//          WebViewChromium::InnerClearSslPreferencesRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerClearSslPreferencesRunnable, Object, IRunnable)

WebViewChromium::InnerClearSslPreferencesRunnable::InnerClearSslPreferencesRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerClearSslPreferencesRunnable::Run()
{
    // ==================before translated======================
    // clearSslPreferences();

    assert(NULL != mOwner);
    return mOwner->ClearSslPreferences();
}

//=====================================================================
//          WebViewChromium::InnerCopyBackForwardListCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerCopyBackForwardListCallable, Object, ICallable)

WebViewChromium::InnerCopyBackForwardListCallable::InnerCopyBackForwardListCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerCopyBackForwardListCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return copyBackForwardList();

    assert(NULL != mOwner);
    AutoPtr<IWebBackForwardList> listTmp;
    mOwner->CopyBackForwardList((IWebBackForwardList**)&listTmp);
    *result = TO_IINTERFACE(listTmp);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//                WebViewChromium::InnerFindNextRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerFindNextRunnable, Object, IRunnable)

WebViewChromium::InnerFindNextRunnable::InnerFindNextRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Boolean forwards)
    : mOwner(owner)
    , mForwards(forwards)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerFindNextRunnable::Run()
{
    // ==================before translated======================
    // findNext(forwards);

    assert(NULL != mOwner);
    return mOwner->FindNext(mForwards);;
}

//=====================================================================
//             WebViewChromium::InnerFindAllAsyncRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerFindAllAsyncRunnable, Object, IRunnable)

WebViewChromium::InnerFindAllAsyncRunnable::InnerFindAllAsyncRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const String& searchString)
    : mOwner(owner)
    , mSearchString(searchString)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerFindAllAsyncRunnable::Run()
{
    // ==================before translated======================
    // findAllAsync(searchString);

    assert(NULL != mOwner);
    return mOwner->FindAllAsync(mSearchString);
}

//=====================================================================
//       WebViewChromium::InnerNotifyFindDialogDismissedRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerNotifyFindDialogDismissedRunnable, Object, IRunnable)

WebViewChromium::InnerNotifyFindDialogDismissedRunnable::InnerNotifyFindDialogDismissedRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerNotifyFindDialogDismissedRunnable::Run()
{
    // ==================before translated======================
    // notifyFindDialogDismissed();

    assert(NULL != mOwner);
    return mOwner->NotifyFindDialogDismissed();
}

//=====================================================================
//              WebViewChromium::InnerClearMatchesRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerClearMatchesRunnable, Object, IRunnable)

WebViewChromium::InnerClearMatchesRunnable::InnerClearMatchesRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerClearMatchesRunnable::Run()
{
    // ==================before translated======================
    // clearMatches();

    assert(NULL != mOwner);
    return mOwner->ClearMatches();
}

//=====================================================================
//            WebViewChromium::InnerDocumentHasImagesRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerDocumentHasImagesRunnable, Object, IRunnable)

WebViewChromium::InnerDocumentHasImagesRunnable::InnerDocumentHasImagesRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ IMessage* response)
    : mOwner(owner)
    , mResponse(response)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerDocumentHasImagesRunnable::Run()
{
    // ==================before translated======================
    // documentHasImages(response);

    assert(NULL != mOwner);
    return mOwner->DocumentHasImages(mResponse);
}

//=====================================================================
//           WebViewChromium::InnerSetPictureListenerRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerSetPictureListenerRunnable, Object, IRunnable)

WebViewChromium::InnerSetPictureListenerRunnable::InnerSetPictureListenerRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ IWebViewPictureListener* listener)
    : mOwner(owner)
    , mListener(listener)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerSetPictureListenerRunnable::Run()
{
    // ==================before translated======================
    // setPictureListener(listener);

    assert(NULL != mOwner);
    return mOwner->SetPictureListener(mListener);
}

//=====================================================================
//          WebViewChromium::InnerAddJavascriptInterfaceRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerAddJavascriptInterfaceRunnable, Object, IRunnable)

WebViewChromium::InnerAddJavascriptInterfaceRunnable::InnerAddJavascriptInterfaceRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Object* obj,
    /* [in] */ const String& interfaceName)
    : mOwner(owner)
    , mObj(obj)
    , mInterfaceName(interfaceName)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerAddJavascriptInterfaceRunnable::Run()
{
    // ==================before translated======================
    // addJavascriptInterface(obj, interfaceName);

    assert(NULL != mOwner);
    return mOwner->AddJavascriptInterface(TO_IINTERFACE(mObj), mInterfaceName);
}

//=====================================================================
//       WebViewChromium::InnerRemoveJavascriptInterfaceRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerRemoveJavascriptInterfaceRunnable, Object, IRunnable)

WebViewChromium::InnerRemoveJavascriptInterfaceRunnable::InnerRemoveJavascriptInterfaceRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ const String& interfaceName)
    : mOwner(owner)
    , mInterfaceName(interfaceName)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerRemoveJavascriptInterfaceRunnable::Run()
{
    // ==================before translated======================
    // removeJavascriptInterface(interfaceName);

    assert(NULL != mOwner);
    return mOwner->RemoveJavascriptInterface(mInterfaceName);
}

//=====================================================================
//             WebViewChromium::InnerFlingScrollRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerFlingScrollRunnable, Object, IRunnable)

WebViewChromium::InnerFlingScrollRunnable::InnerFlingScrollRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Int32 vx,
    /* [in] */ Int32 vy)
    : mOwner(owner)
    , mVx(vx)
    , mVy(vy)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerFlingScrollRunnable::Run()
{
    // ==================before translated======================
    // flingScroll(vx, vy);

    assert(NULL != mOwner);
    return mOwner->FlingScroll(mVx, mVy);
}

//=====================================================================
//               WebViewChromium::InnerZoomInCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerZoomInCallable, Object, ICallable)

WebViewChromium::InnerZoomInCallable::InnerZoomInCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerZoomInCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return zoomIn();

    assert(NULL != mOwner);
    Boolean zoomInTmp = FALSE;
    mOwner->ZoomIn(&zoomInTmp);
    AutoPtr<IBoolean> zoomInTmp1 = CoreUtils::Convert(zoomInTmp);
    *result = TO_IINTERFACE(zoomInTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//              WebViewChromium::InnerZoomOutCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerZoomOutCallable, Object, ICallable)

WebViewChromium::InnerZoomOutCallable::InnerZoomOutCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerZoomOutCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return zoomOut();

    assert(NULL != mOwner);
    Boolean zoomOutTmp = FALSE;
    mOwner->ZoomOut(&zoomOutTmp);
    AutoPtr<IBoolean> zoomOutTmp1 = CoreUtils::Convert(zoomOutTmp);
    *result = TO_IINTERFACE(zoomOutTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//     WebViewChromium::InnerShouldDelayChildPressedStateCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerShouldDelayChildPressedStateCallable, Object, ICallable)

WebViewChromium::InnerShouldDelayChildPressedStateCallable::InnerShouldDelayChildPressedStateCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerShouldDelayChildPressedStateCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return shouldDelayChildPressedState();

    assert(NULL != mOwner);
    Boolean stateTmp = FALSE;
    mOwner->ShouldDelayChildPressedState(&stateTmp);
    AutoPtr<IBoolean> stateTmp1 = CoreUtils::Convert(stateTmp);
    *result = TO_IINTERFACE(stateTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//      WebViewChromium::InnerGetAccessibilityNodeProviderCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerGetAccessibilityNodeProviderCallable, Object, ICallable)

WebViewChromium::InnerGetAccessibilityNodeProviderCallable::InnerGetAccessibilityNodeProviderCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerGetAccessibilityNodeProviderCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getAccessibilityNodeProvider();

    assert(NULL != mOwner);
    AutoPtr<IAccessibilityNodeProvider> providerTmp;
    mOwner->GetAccessibilityNodeProvider((IAccessibilityNodeProvider**)&providerTmp);
    *result = TO_IINTERFACE(providerTmp);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//   WebViewChromium::InnerOnInitializeAccessibilityNodeInfoRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOnInitializeAccessibilityNodeInfoRunnable, Object, IRunnable)

WebViewChromium::InnerOnInitializeAccessibilityNodeInfoRunnable::InnerOnInitializeAccessibilityNodeInfoRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ IAccessibilityNodeInfo* info)
    : mOwner(owner)
    , mInfo(info)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOnInitializeAccessibilityNodeInfoRunnable::Run()
{
    // ==================before translated======================
    // onInitializeAccessibilityNodeInfo(info);

    assert(NULL != mOwner);
    return mOwner->OnInitializeAccessibilityNodeInfo(mInfo);
}

//=====================================================================
//     WebViewChromium::InnerOnInitializeAccessibilityEventRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOnInitializeAccessibilityEventRunnable, Object, IRunnable)

WebViewChromium::InnerOnInitializeAccessibilityEventRunnable::InnerOnInitializeAccessibilityEventRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ IAccessibilityEvent* event)
    : mOwner(owner)
    , mEvent(event)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOnInitializeAccessibilityEventRunnable::Run()
{
    // ==================before translated======================
    // onInitializeAccessibilityEvent(event);

    assert(NULL != mOwner);
    return mOwner->OnInitializeAccessibilityEvent(mEvent);
}

//=====================================================================
//     WebViewChromium::InnerPerformAccessibilityActionCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerPerformAccessibilityActionCallable, Object, ICallable)

WebViewChromium::InnerPerformAccessibilityActionCallable::InnerPerformAccessibilityActionCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments)
    : mOwner(owner)
    , mAction(action)
    , mArguments(arguments)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerPerformAccessibilityActionCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return performAccessibilityAction(action, arguments);

    assert(NULL != mOwner);
    Boolean actionTmp = FALSE;
    mOwner->PerformAccessibilityAction(mAction, mArguments, &actionTmp);
    AutoPtr<IBoolean> actionTmp1 = CoreUtils::Convert(actionTmp);
    *result = TO_IINTERFACE(actionTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//         WebViewChromium::InnerSetOverScrollModeRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerSetOverScrollModeRunnable, Object, IRunnable)

WebViewChromium::InnerSetOverScrollModeRunnable::InnerSetOverScrollModeRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Int32 mode)
    : mOwner(owner)
    , mMode(mode)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerSetOverScrollModeRunnable::Run()
{
    // ==================before translated======================
    // setOverScrollMode(mode);

    assert(NULL != mOwner);
    return mOwner->SetOverScrollMode(mMode);
}

//=====================================================================
//        WebViewChromium::InnerSetScrollBarStyleRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerSetScrollBarStyleRunnable, Object, IRunnable)

WebViewChromium::InnerSetScrollBarStyleRunnable::InnerSetScrollBarStyleRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Int32 style)
    : mOwner(owner)
    , mStyle(style)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerSetScrollBarStyleRunnable::Run()
{
    // ==================before translated======================
    // setScrollBarStyle(style);

    assert(NULL != mOwner);
    return mOwner->SetScrollBarStyle(mStyle);
}

//=====================================================================
//          WebViewChromium::InnerOnOverScrolledRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOnOverScrolledRunnable, Object, IRunnable)

WebViewChromium::InnerOnOverScrolledRunnable::InnerOnOverScrolledRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY,
    /* [in] */ Boolean clampedX,
    /* [in] */ Boolean clampedY)
    : mOwner(owner)
    , mScrollX(scrollX)
    , mScrollY(scrollY)
    , mClampedX(clampedX)
    , mClampedY(clampedY)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOnOverScrolledRunnable::Run()
{
    // ==================before translated======================
    // onOverScrolled(scrollX, scrollY, clampedX, clampedY);

    assert(NULL != mOwner);
    return mOwner->OnOverScrolled(mScrollX, mScrollY, mClampedX, mClampedY);
}

//=====================================================================
//      WebViewChromium::InnerOnWindowVisibilityChangedRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOnWindowVisibilityChangedRunnable, Object, IRunnable)

WebViewChromium::InnerOnWindowVisibilityChangedRunnable::InnerOnWindowVisibilityChangedRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Int32 visibility)
    : mOwner(owner)
    , mVisibility(visibility)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOnWindowVisibilityChangedRunnable::Run()
{
    // ==================before translated======================
    // onWindowVisibilityChanged(visibility);

    assert(NULL != mOwner);
    return mOwner->OnWindowVisibilityChanged(mVisibility);
}

//=====================================================================
//                   WebViewChromium::InnerOnDrawRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOnDrawRunnable, Object, IRunnable)

WebViewChromium::InnerOnDrawRunnable::InnerOnDrawRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ ICanvas* canvas)
    : mOwner(owner)
    , mCanvas(canvas)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOnDrawRunnable::Run()
{
    // ==================before translated======================
    // onDraw(canvas);

    assert(NULL != mOwner);
    return mOwner->OnDraw(mCanvas);
}

//=====================================================================
//        WebViewChromium::InnerOnConfigurationChangedRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOnConfigurationChangedRunnable, Object, IRunnable)

WebViewChromium::InnerOnConfigurationChangedRunnable::InnerOnConfigurationChangedRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ IConfiguration* newConfig)
    : mOwner(owner)
    , mNewConfig(newConfig)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOnConfigurationChangedRunnable::Run()
{
    // ==================before translated======================
    // onConfigurationChanged(newConfig);

    assert(NULL != mOwner);
    return mOwner->OnConfigurationChanged(mNewConfig);
}

//=====================================================================
//           WebViewChromium::InnerOnKeyMultipleCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOnKeyMultipleCallable, Object, ICallable)

WebViewChromium::InnerOnKeyMultipleCallable::InnerOnKeyMultipleCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 repeatCount,
    /* [in] */ IKeyEvent* event)
    : mOwner(owner)
    , mKeyCode(keyCode)
    , mRepeatCount(repeatCount)
    , mEvent(event)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOnKeyMultipleCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return onKeyMultiple(keyCode, repeatCount, event);

    assert(NULL != mOwner);
    Boolean resTmp = FALSE;
    mOwner->OnKeyMultiple(mKeyCode, mRepeatCount, mEvent, &resTmp);
    AutoPtr<IBoolean> resTmp1 = CoreUtils::Convert(resTmp);
    *result = TO_IINTERFACE(resTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//              WebViewChromium::InnerOnKeyDownCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOnKeyDownCallable, Object, ICallable)

WebViewChromium::InnerOnKeyDownCallable::InnerOnKeyDownCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
    : mOwner(owner)
    , mKeyCode(keyCode)
    , mEvent(event)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOnKeyDownCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return onKeyDown(keyCode, event);

    assert(NULL != mOwner);
    Boolean resTmp = FALSE;
    mOwner->OnKeyDown(mKeyCode, mEvent, &resTmp);
    AutoPtr<IBoolean> resTmp1 = CoreUtils::Convert(resTmp);
    *result = TO_IINTERFACE(resTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//              WebViewChromium::InnerOnKeyUpCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOnKeyUpCallable, Object, ICallable)

WebViewChromium::InnerOnKeyUpCallable::InnerOnKeyUpCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
    : mOwner(owner)
    , mKeyCode(keyCode)
    , mEvent(event)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOnKeyUpCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return onKeyUp(keyCode, event);

    assert(NULL != mOwner);
    Boolean resTmp = FALSE;
    mOwner->OnKeyUp(mKeyCode, mEvent, &resTmp);
    AutoPtr<IBoolean> resTmp1 = CoreUtils::Convert(resTmp);
    *result = TO_IINTERFACE(resTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//         WebViewChromium::InnerOnDetachedFromWindowRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOnDetachedFromWindowRunnable, Object, IRunnable)

WebViewChromium::InnerOnDetachedFromWindowRunnable::InnerOnDetachedFromWindowRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOnDetachedFromWindowRunnable::Run()
{
    // ==================before translated======================
    // onDetachedFromWindow();

    assert(NULL != mOwner);
    return mOwner->OnDetachedFromWindow();
}

//=====================================================================
//        WebViewChromium::InnerOnVisibilityChangedRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOnVisibilityChangedRunnable, Object, IRunnable)

WebViewChromium::InnerOnVisibilityChangedRunnable::InnerOnVisibilityChangedRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ IView* changedView,
    /* [in] */ Int32 visibility)
    : mOwner(owner)
    , mChangedView(changedView)
    , mVisibility(visibility)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOnVisibilityChangedRunnable::Run()
{
    // ==================before translated======================
    // onVisibilityChanged(changedView, visibility);

    assert(NULL != mOwner);
    return mOwner->OnVisibilityChanged(mChangedView, mVisibility);
}

//=====================================================================
//         WebViewChromium::InnerOnWindowFocusChangedRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOnWindowFocusChangedRunnable, Object, IRunnable)

WebViewChromium::InnerOnWindowFocusChangedRunnable::InnerOnWindowFocusChangedRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Boolean hasWindowFocus)
    : mOwner(owner)
    , mHasWindowFocus(hasWindowFocus)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOnWindowFocusChangedRunnable::Run()
{
    // ==================before translated======================
    // onWindowFocusChanged(hasWindowFocus);

    assert(NULL != mOwner);
    return mOwner->OnWindowFocusChanged(mHasWindowFocus);
}

//=====================================================================
//            WebViewChromium::InnerOnFocusChangedRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOnFocusChangedRunnable, Object, IRunnable)

WebViewChromium::InnerOnFocusChangedRunnable::InnerOnFocusChangedRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Boolean focused,
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
    : mOwner(owner)
    , mFocused(focused)
    , mDirection(direction)
    , mPreviouslyFocusedRect(previouslyFocusedRect)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOnFocusChangedRunnable::Run()
{
    // ==================before translated======================
    // onFocusChanged(focused, direction, previouslyFocusedRect);

    assert(NULL != mOwner);
    return mOwner->OnFocusChanged(mFocused, mDirection, mPreviouslyFocusedRect);
}

//=====================================================================
//              WebViewChromium::InnerOnSizeChangedRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOnSizeChangedRunnable, Object, IRunnable)

WebViewChromium::InnerOnSizeChangedRunnable::InnerOnSizeChangedRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 ow,
    /* [in] */ Int32 oh)
    : mOwner(owner)
    , mW(w)
    , mH(h)
    , mOw(ow)
    , mOh(oh)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOnSizeChangedRunnable::Run()
{
    // ==================before translated======================
    // onSizeChanged(w, h, ow, oh);

    assert(NULL != mOwner);
    return mOwner->OnSizeChanged(mW, mH, mOw, mOh);
}

//=====================================================================
//           WebViewChromium::InnerDispatchKeyEventCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerDispatchKeyEventCallable, Object, ICallable)

WebViewChromium::InnerDispatchKeyEventCallable::InnerDispatchKeyEventCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ IKeyEvent* event)
    : mOwner(owner)
    , mEvent(event)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerDispatchKeyEventCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return dispatchKeyEvent(event);

    assert(NULL != mOwner);
    Boolean resTmp = FALSE;
    mOwner->DispatchKeyEvent(mEvent, &resTmp);
    AutoPtr<IBoolean> resTmp1 = CoreUtils::Convert(resTmp);
    *result = TO_IINTERFACE(resTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//              WebViewChromium::InnerOnTouchEventCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOnTouchEventCallable, Object, ICallable)

WebViewChromium::InnerOnTouchEventCallable::InnerOnTouchEventCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ IMotionEvent* ev)
    : mOwner(owner)
    , mEv(ev)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOnTouchEventCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return onTouchEvent(ev);

    assert(NULL != mOwner);
    Boolean resTmp = FALSE;
    mOwner->OnTouchEvent(mEv, &resTmp);
    AutoPtr<IBoolean> resTmp1 = CoreUtils::Convert(resTmp);
    *result = TO_IINTERFACE(resTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//              WebViewChromium::InnerOnHoverEventCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOnHoverEventCallable, Object, ICallable)

WebViewChromium::InnerOnHoverEventCallable::InnerOnHoverEventCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ IMotionEvent* event)
    : mOwner(owner)
    , mEvent(event)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOnHoverEventCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return onHoverEvent(event);

    assert(NULL != mOwner);
    Boolean resTmp = FALSE;
    mOwner->OnHoverEvent(mEvent, &resTmp);
    AutoPtr<IBoolean> resTmp1 = CoreUtils::Convert(resTmp);
    *result = TO_IINTERFACE(resTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//          WebViewChromium::InnerOnGenericMotionEventCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOnGenericMotionEventCallable, Object, ICallable)

WebViewChromium::InnerOnGenericMotionEventCallable::InnerOnGenericMotionEventCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ IMotionEvent* event)
    : mOwner(owner)
    , mEvent(event)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOnGenericMotionEventCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return onGenericMotionEvent(event);

    assert(NULL != mOwner);
    Boolean resTmp = FALSE;
    mOwner->OnGenericMotionEvent(mEvent, &resTmp);
    AutoPtr<IBoolean> resTmp1 = CoreUtils::Convert(resTmp);
    *result = TO_IINTERFACE(resTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//                WebViewChromium::InnerRequestFocusCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerRequestFocusCallable, Object, ICallable)

WebViewChromium::InnerRequestFocusCallable::InnerRequestFocusCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
    : mOwner(owner)
    , mDirection(direction)
    , mPreviouslyFocusedRect(previouslyFocusedRect)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerRequestFocusCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return requestFocus(direction, previouslyFocusedRect);

    assert(NULL != mOwner);
    Boolean resTmp = FALSE;
    mOwner->RequestFocus(mDirection, mPreviouslyFocusedRect, &resTmp);
    AutoPtr<IBoolean> resTmp1 = CoreUtils::Convert(resTmp);
    *result = TO_IINTERFACE(resTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//               WebViewChromium::InnerOnMeasureRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerOnMeasureRunnable, Object, IRunnable)

WebViewChromium::InnerOnMeasureRunnable::InnerOnMeasureRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
    : mOwner(owner)
    , mWidthMeasureSpec(widthMeasureSpec)
    , mHeightMeasureSpec(heightMeasureSpec)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerOnMeasureRunnable::Run()
{
    // ==================before translated======================
    // onMeasure(widthMeasureSpec, heightMeasureSpec);

    assert(NULL != mOwner);
    return mOwner->OnMeasure(mWidthMeasureSpec, mHeightMeasureSpec);
}

//=====================================================================
//       WebViewChromium::InnerRequestChildRectangleOnScreenCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerRequestChildRectangleOnScreenCallable, Object, ICallable)

WebViewChromium::InnerRequestChildRectangleOnScreenCallable::InnerRequestChildRectangleOnScreenCallable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ IView* child,
    /* [in] */ IRect* rect,
    /* [in] */ Boolean immediate)
    : mOwner(owner)
    , mChild(child)
    , mRect(rect)
    , mImmediate(immediate)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerRequestChildRectangleOnScreenCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return requestChildRectangleOnScreen(child, rect, immediate);

    assert(NULL != mOwner);
    Boolean resTmp = FALSE;
    mOwner->RequestChildRectangleOnScreen(mChild, mRect, mImmediate, &resTmp);
    AutoPtr<IBoolean> resTmp1 = CoreUtils::Convert(resTmp);
    *result = TO_IINTERFACE(resTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//         WebViewChromium::InnerSetBackgroundColorRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerSetBackgroundColorRunnable, Object, IRunnable)

WebViewChromium::InnerSetBackgroundColorRunnable::InnerSetBackgroundColorRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Int32 color)
    : mOwner(owner)
    , mColor(color)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerSetBackgroundColorRunnable::Run()
{
    // ==================before translated======================
    // setBackgroundColor(color);

    assert(NULL != mOwner);
    return mOwner->SetBackgroundColor(mColor);
}

//=====================================================================
//              WebViewChromium::InnerSetLayerTypeRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerSetLayerTypeRunnable, Object, IRunnable)

WebViewChromium::InnerSetLayerTypeRunnable::InnerSetLayerTypeRunnable(
    /* [in] */ WebViewChromium* owner,
    /* [in] */ Int32 layerType,
    /* [in] */ IPaint* paint)
    : mOwner(owner)
    , mLayerType(layerType)
    , mPaint(paint)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerSetLayerTypeRunnable::Run()
{
    // ==================before translated======================
    // setLayerType(layerType, paint);

    assert(NULL != mOwner);
    return mOwner->SetLayerType(mLayerType, mPaint);
}

//=====================================================================
//     WebViewChromium::InnerComputeHorizontalScrollRangeCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerComputeHorizontalScrollRangeCallable, Object, ICallable)

WebViewChromium::InnerComputeHorizontalScrollRangeCallable::InnerComputeHorizontalScrollRangeCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerComputeHorizontalScrollRangeCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return computeHorizontalScrollRange();

    assert(NULL != mOwner);
    Int32 rangeTmp = 0;
    mOwner->ComputeHorizontalScrollRange(&rangeTmp);
    AutoPtr<IInteger32> rangeTmp1;
    CInteger32::New(rangeTmp, (IInteger32**)&rangeTmp1);
    *result = TO_IINTERFACE(rangeTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//      WebViewChromium::InnerComputeHorizontalScrollOffsetCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerComputeHorizontalScrollOffsetCallable, Object, ICallable)

WebViewChromium::InnerComputeHorizontalScrollOffsetCallable::InnerComputeHorizontalScrollOffsetCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerComputeHorizontalScrollOffsetCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return computeHorizontalScrollOffset();

    assert(NULL != mOwner);
    Int32 rangeTmp = 0;
    mOwner->ComputeHorizontalScrollOffset(&rangeTmp);
    AutoPtr<IInteger32> rangeTmp1;
    CInteger32::New(rangeTmp, (IInteger32**)&rangeTmp1);
    *result = TO_IINTERFACE(rangeTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//        WebViewChromium::InnerComputeVerticalScrollRangeCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerComputeVerticalScrollRangeCallable, Object, ICallable)

WebViewChromium::InnerComputeVerticalScrollRangeCallable::InnerComputeVerticalScrollRangeCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerComputeVerticalScrollRangeCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return computeVerticalScrollRange();

    assert(NULL != mOwner);
    Int32 rangeTmp = 0;
    mOwner->ComputeVerticalScrollRange(&rangeTmp);
    AutoPtr<IInteger32> rangeTmp1;
    CInteger32::New(rangeTmp, (IInteger32**)&rangeTmp1);
    *result = TO_IINTERFACE(rangeTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//        WebViewChromium::InnerComputeVerticalScrollOffsetCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerComputeVerticalScrollOffsetCallable, Object, ICallable)

WebViewChromium::InnerComputeVerticalScrollOffsetCallable::InnerComputeVerticalScrollOffsetCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerComputeVerticalScrollOffsetCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return computeVerticalScrollOffset();

    assert(NULL != mOwner);
    Int32 rangeTmp = 0;
    mOwner->ComputeVerticalScrollOffset(&rangeTmp);
    AutoPtr<IInteger32> rangeTmp1;
    CInteger32::New(rangeTmp, (IInteger32**)&rangeTmp1);
    *result = TO_IINTERFACE(rangeTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//     WebViewChromium::InnerComputeVerticalScrollExtentCallable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerComputeVerticalScrollExtentCallable, Object, ICallable)

WebViewChromium::InnerComputeVerticalScrollExtentCallable::InnerComputeVerticalScrollExtentCallable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerComputeVerticalScrollExtentCallable::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return computeVerticalScrollExtent();

    assert(NULL != mOwner);
    Int32 rangeTmp = 0;
    mOwner->ComputeVerticalScrollExtent(&rangeTmp);
    AutoPtr<IInteger32> rangeTmp1;
    CInteger32::New(rangeTmp, (IInteger32**)&rangeTmp1);
    *result = TO_IINTERFACE(rangeTmp1);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//             WebViewChromium::InnerComputeScrollRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewChromium::InnerComputeScrollRunnable, Object, IRunnable)

WebViewChromium::InnerComputeScrollRunnable::InnerComputeScrollRunnable(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewChromium::InnerComputeScrollRunnable::Run()
{
    // ==================before translated======================
    // computeScroll();

    assert(NULL != mOwner);
    return mOwner->ComputeScroll();
}

//=====================================================================
//               WebViewChromium::WebViewNativeGLDelegate
//=====================================================================
WebViewChromium::WebViewNativeGLDelegate::WebViewNativeGLDelegate(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

Boolean WebViewChromium::WebViewNativeGLDelegate::RequestDrawGL(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Boolean waitForCompletion,
    /* [in] */ IView* containerView)
{
    // ==================before translated======================
    // if (mGLfunctor == null) {
    //     mGLfunctor = new DrawGLFunctor(mAwContents.getAwDrawGLViewContext());
    // }
    // return mGLfunctor.requestDrawGL(
    //         (HardwareCanvas) canvas, containerView.getViewRootImpl(), waitForCompletion);

    assert(NULL != mOwner);
    if (NULL == mOwner->mGLfunctor) {
        mOwner->mGLfunctor = new DrawGLFunctor(mOwner->mAwContents->GetAwDrawGLViewContext());
    }

    IHardwareCanvas* hardwareCanvas = IHardwareCanvas::Probe(canvas);
    AutoPtr<IViewRootImpl> viewImpl;
    //containerView->GetViewRootImpl((IInterface/*ViewRootImpl*/**)&viewImpl);
    return mOwner->mGLfunctor->RequestDrawGL(hardwareCanvas, viewImpl, waitForCompletion);
}

CARAPI_(void) WebViewChromium::WebViewNativeGLDelegate::DetachGLFunctor()
{
    // ==================before translated======================
    // if (mGLfunctor != null) {
    //     mGLfunctor.detach();
    // }

    assert(NULL != mOwner);
    if (mOwner->mGLfunctor != NULL) {
        mOwner->mGLfunctor->Detach();
    }
}

//=====================================================================
//                WebViewChromium::InternalAccessAdapter
//=====================================================================
WebViewChromium::InternalAccessAdapter::InternalAccessAdapter(
    /* [in] */ WebViewChromium* owner)
    : mOwner(owner)
{
}

Boolean WebViewChromium::InternalAccessAdapter::DrawChild(
    /* [in] */ ICanvas* arg0,
    /* [in] */ IView* arg1,
    /* [in] */ Int64 arg2)
{
    // ==================before translated======================
    // UnimplementedWebViewApi.invoke();
    // return false;

    UnimplementedWebViewApi::Invoke();
    return FALSE;
}

Boolean WebViewChromium::InternalAccessAdapter::Super_onKeyUp(
    /* [in] */ Int32 arg0,
    /* [in] */ IKeyEvent* arg1)
{
    // ==================before translated======================
    // // Intentional no-op
    // return false;

    return FALSE;
}

Boolean WebViewChromium::InternalAccessAdapter::Super_dispatchKeyEventPreIme(
    /* [in] */ IKeyEvent* arg0)
{
    // ==================before translated======================
    // UnimplementedWebViewApi.invoke();
    // return false;

    UnimplementedWebViewApi::Invoke();
    return FALSE;
}

Boolean WebViewChromium::InternalAccessAdapter::Super_dispatchKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    // ==================before translated======================
    // return mWebViewPrivate.super_dispatchKeyEvent(event);

    Boolean result = FALSE;
    mOwner->mWebViewPrivate->Super_dispatchKeyEvent(event, &result);
    return result;
}

Boolean WebViewChromium::InternalAccessAdapter::Super_onGenericMotionEvent(
    /* [in] */ IMotionEvent* arg0)
{
    // ==================before translated======================
    // return mWebViewPrivate.super_onGenericMotionEvent(arg0);

    Boolean result = FALSE;
    mOwner->mWebViewPrivate->Super_onGenericMotionEvent(arg0, &result);
    return result;
}

void WebViewChromium::InternalAccessAdapter::Super_onConfigurationChanged(
    /* [in] */ IConfiguration* arg0)
{
    // ==================before translated======================
    // // Intentional no-op
}

Int32 WebViewChromium::InternalAccessAdapter::Super_getScrollBarStyle()
{
    // ==================before translated======================
    // return mWebViewPrivate.super_getScrollBarStyle();

    Int32 result = 0;
    mOwner->mWebViewPrivate->Super_getScrollBarStyle(&result);
    return result;
}

Boolean WebViewChromium::InternalAccessAdapter::AwakenScrollBars()
{
    // ==================before translated======================
    // mWebViewPrivate.awakenScrollBars(0);
    // // TODO: modify the WebView.PrivateAccess to provide a return value.
    // return true;

    mOwner->mWebViewPrivate->AwakenScrollBars(0);
    return TRUE;
}

Boolean WebViewChromium::InternalAccessAdapter::Super_awakenScrollBars(
    /* [in] */ Int32 arg0,
    /* [in] */ Boolean arg1)
{
    // ==================before translated======================
    // // TODO: need method on WebView.PrivateAccess?
    // UnimplementedWebViewApi.invoke();
    // return false;

    // TODO: need method on WebView.PrivateAccess?
    UnimplementedWebViewApi::Invoke();
    return FALSE;
}

void WebViewChromium::InternalAccessAdapter::OnScrollChanged(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 oldl,
    /* [in] */ Int32 oldt)
{
    // ==================before translated======================
    // // Intentional no-op.
    // // Chromium calls this directly to trigger accessibility events. That isn't needed
    // // for WebView since super_scrollTo invokes onScrollChanged for us.
}

void WebViewChromium::InternalAccessAdapter::OverScrollBy(
    /* [in] */ Int32 deltaX,
    /* [in] */ Int32 deltaY,
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY,
    /* [in] */ Int32 scrollRangeX,
    /* [in] */ Int32 scrollRangeY,
    /* [in] */ Int32 maxOverScrollX,
    /* [in] */ Int32 maxOverScrollY,
    /* [in] */ Boolean isTouchEvent)
{
    // ==================before translated======================
    // mWebViewPrivate.overScrollBy(deltaX, deltaY, scrollX, scrollY,
    //         scrollRangeX, scrollRangeY, maxOverScrollX, maxOverScrollY, isTouchEvent);

    mOwner->mWebViewPrivate->OverScrollBy(deltaX, deltaY, scrollX, scrollY, scrollRangeX, scrollRangeY, maxOverScrollX, maxOverScrollY, isTouchEvent);
}

void WebViewChromium::InternalAccessAdapter::Super_scrollTo(
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY)
{
    // ==================before translated======================
    // mWebViewPrivate.super_scrollTo(scrollX, scrollY);

    mOwner->mWebViewPrivate->Super_scrollTo(scrollX, scrollY);
}

void WebViewChromium::InternalAccessAdapter::SetMeasuredDimension(
    /* [in] */ Int32 measuredWidth,
    /* [in] */ Int32 measuredHeight)
{
    // ==================before translated======================
    // mWebViewPrivate.setMeasuredDimension(measuredWidth, measuredHeight);

    mOwner->mWebViewPrivate->SetMeasuredDimension(measuredWidth, measuredHeight);
}

Boolean WebViewChromium::InternalAccessAdapter::Super_onHoverEvent(
    /* [in] */ IMotionEvent* event)
{
    // ==================before translated======================
    // return mWebViewPrivate.super_onHoverEvent(event);

    Boolean result = FALSE;
    mOwner->mWebViewPrivate->Super_onHoverEvent(event, &result);
    return result;
}

//=====================================================================
//                           WebViewChromium
//=====================================================================
const String WebViewChromium::TAG("WebViewChromium");//WebViewChromium.class.getSimpleName());
Boolean WebViewChromium::sRecordWholeDocumentEnabledByApi = FALSE;

CAR_INTERFACE_IMPL_3(WebViewChromium, Object, IWebViewProvider, IWebViewProviderScrollDelegate, IWebViewProviderViewDelegate)

WebViewChromium::WebViewChromium(
    /* [in] */ IWebViewChromiumFactoryProvider* factory,
    /* [in] */ IWebView* webView,
    /* [in] */ IWebViewPrivateAccess* webViewPrivate)
    : mWebView(webView)
    , mWebViewPrivate(webViewPrivate)
    , mFactory(factory)
{
    // ==================before translated======================
    // mWebView = webView;
    // mWebViewPrivate = webViewPrivate;
    // mHitTestResult = new WebView.HitTestResult();
    // mAppTargetSdkVersion = mWebView.getContext().getApplicationInfo().targetSdkVersion;
    // mFactory = factory;
    // mRunQueue = new WebViewChromiumRunQueue();
    // String webViewAssetPath = WebViewFactory.getLoadedPackageInfo().applicationInfo.sourceDir;
    // mWebView.getContext().getAssets().addAssetPath(webViewAssetPath);

    CWebViewHitTestResult::New((IWebViewHitTestResult**)&mHitTestResult);

    IView* viewTmp = IView::Probe(mWebView);
    AutoPtr<IContext> context;
    viewTmp->GetContext((IContext**)&context);
    AutoPtr<IApplicationInfo> applicationInfo;
    context->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    applicationInfo->GetTargetSdkVersion(&mAppTargetSdkVersion);

    mRunQueue = new WebViewChromiumRunQueue(this);

    AutoPtr<IWebViewFactory> webViewFactory;
    CWebViewFactory::AcquireSingleton((IWebViewFactory**)&webViewFactory);
    AutoPtr<IPackageInfo> packageInfo;
    webViewFactory->GetLoadedPackageInfo((IPackageInfo**)&packageInfo);
    AutoPtr<IApplicationInfo> applicationInfo1;
    packageInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo1);
    String webViewAssetPath;
    applicationInfo1->GetSourceDir(&webViewAssetPath);
    Logger::D("WebViewChromium::WebViewChromium", "leliang_debug webViewAssetPath is %s", webViewAssetPath.string());

    AutoPtr<IAssetManager> assetManager;
    context->GetAssets((IAssetManager**)&assetManager);
    Int32 cookie = 0;
    assetManager->AddAssetPath(webViewAssetPath, &cookie);
}

ECode WebViewChromium::EnableSlowWholeDocumentDraw()
{
    // ==================before translated======================
    // sRecordWholeDocumentEnabledByApi = true;

    sRecordWholeDocumentEnabledByApi = TRUE;
    return NOERROR;
}

ECode WebViewChromium::CompleteWindowCreation(
    /* [in] */ IWebView* parent,
    /* [in] */ IWebView* child)
{
    VALIDATE_NOT_NULL(parent);
    VALIDATE_NOT_NULL(child);
    // ==================before translated======================
    // AwContents parentContents = ((WebViewChromium) parent.getWebViewProvider()).mAwContents;
    // AwContents childContents =
    //         child == null ? null : ((WebViewChromium) child.getWebViewProvider()).mAwContents;
    // parentContents.supplyContentsForPopup(childContents);

    AutoPtr<IWebViewProvider> parentWebViewProvider;
    parent->GetWebViewProvider((IWebViewProvider**)&parentWebViewProvider);
    WebViewChromium* parentWebViewChromium = (WebViewChromium*)parentWebViewProvider.Get();
    assert(NULL != parentWebViewChromium);
    AutoPtr<AwContents> parentContents = parentWebViewChromium->mAwContents;

    AutoPtr<IWebViewProvider> childWebViewProvider;
    child->GetWebViewProvider((IWebViewProvider**)&childWebViewProvider);
    WebViewChromium* childWebViewChromium = (WebViewChromium*)childWebViewProvider.Get();
    assert(NULL != childWebViewChromium);
    AutoPtr<AwContents> childContents = (NULL == child) ? NULL : childWebViewChromium->mAwContents;
    parentContents->SupplyContentsForPopup(childContents);
    return NOERROR;
}

ECode WebViewChromium::Init(
    /* [in] */ IMap* javaScriptInterfaces,
    /* [in] */ Boolean privateBrowsing)
{
    Logger::D("WebViewChromium::Init", "leliang_debug enter");
    VALIDATE_NOT_NULL(javaScriptInterfaces);
    // ==================before translated======================
    // if (privateBrowsing) {
    //     mFactory.startYourEngines(true);
    //     final String msg = "Private browsing is not supported in WebView.";
    //     if (mAppTargetSdkVersion >= Build.VERSION_CODES.KITKAT) {
    //         throw new IllegalArgumentException(msg);
    //     } else {
    //         Log.w(TAG, msg);
    //         TextView warningLabel = new TextView(mWebView.getContext());
    //         warningLabel.setText(mWebView.getContext().getString(
    //                 R.string.webviewchromium_private_browsing_warning));
    //         mWebView.addView(warningLabel);
    //     }
    // }
    //
    // // We will defer real initialization until we know which thread to do it on, unless:
    // // - we are on the main thread already (common case),
    // // - the app is targeting >= JB MR2, in which case checkThread enforces that all usage
    // //   comes from a single thread. (Note in JB MR2 this exception was in WebView.java).
    // if (mAppTargetSdkVersion >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
    //     mFactory.startYourEngines(false);
    //     checkThread();
    // } else if (!mFactory.hasStarted()) {
    //     if (Looper.myLooper() == Looper.getMainLooper()) {
    //         mFactory.startYourEngines(true);
    //     }
    // }
    //
    // final boolean isAccessFromFileURLsGrantedByDefault =
    //         mAppTargetSdkVersion < Build.VERSION_CODES.JELLY_BEAN;
    // final boolean areLegacyQuirksEnabled =
    //         mAppTargetSdkVersion < Build.VERSION_CODES.KITKAT;
    //
    // mContentsClientAdapter = new WebViewContentsClientAdapter(mWebView);
    // mWebSettings = new ContentSettingsAdapter(new AwSettings(
    //         mWebView.getContext(), isAccessFromFileURLsGrantedByDefault,
    //         areLegacyQuirksEnabled));
    //
    // if (mAppTargetSdkVersion <= Build.VERSION_CODES.KITKAT) {
    //     mWebSettings.setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);
    //     // On KK and older versions we always allowed third party cookies.
    //     mWebSettings.setAcceptThirdPartyCookies(true);
    //     mWebSettings.getAwSettings().setZeroLayoutHeightDisablesViewportQuirk(true);
    // }
    //
    // mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             initForReal();
    //             if (privateBrowsing) {
    //                 // Intentionally irreversibly disable the webview instance, so that private
    //                 // user data cannot leak through misuse of a non-privateBrowing WebView
    //                 // instance. Can't just null out mAwContents as we never null-check it
    //                 // before use.
    //                 destroy();
    //             }
    //         }
    // });

    if (privateBrowsing) {
        mFactory->StartYourEngines(TRUE);
        const String msg("Private browsing is not supported in WebView.");
        if (mAppTargetSdkVersion >= Build::VERSION_CODES::KITKAT) {
            //throw new IllegalArgumentException(msg);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        else {
            Logger::W(TAG, msg);

            IView* viewTmp = IView::Probe(mWebView);
            AutoPtr<IContext> context;
            viewTmp->GetContext((IContext**)&context);
            AutoPtr<ITextView> warningLabel;
            CTextView::New(context, (ITextView**)&warningLabel);

            String warning;
            context->GetString(R::string::webviewchromium_private_browsing_warning, &warning);
            AutoPtr<ICharSequence> charSequence;
            CString::New(warning, (ICharSequence**)&charSequence);
            warningLabel->SetText(charSequence);

            IView* warningLabelView = IView::Probe(warningLabel);
            IViewGroup* viewGroup = IViewGroup::Probe(mWebView);
            viewGroup->AddView(warningLabelView);
        }
    }

    // We will defer real initialization until we know which thread to do it on, unless:
    // - we are on the main thread already (common case),
    // - the app is targeting >= JB MR2, in which case checkThread enforces that all usage
    //   comes from a single thread. (Note in JB MR2 this exception was in WebView.java).
    Boolean hasStarted = FALSE;
    mFactory->HasStarted(&hasStarted);
    if (mAppTargetSdkVersion >= Build::VERSION_CODES::JELLY_BEAN_MR2) {
        mFactory->StartYourEngines(FALSE);
        CheckThread();
    }
    else if (!hasStarted) {
        IInterface* myLooper = TO_IINTERFACE(Looper::GetMyLooper());
        IInterface* mainLooper = TO_IINTERFACE(Looper::GetMainLooper());
        if (myLooper == mainLooper) {
            mFactory->StartYourEngines(TRUE);
        }
    }

    const Boolean isAccessFromFileURLsGrantedByDefault = mAppTargetSdkVersion < Build::VERSION_CODES::JELLY_BEAN;
    const Boolean areLegacyQuirksEnabled = mAppTargetSdkVersion < Build::VERSION_CODES::KITKAT;
    mContentsClientAdapter = new WebViewContentsClientAdapter(mWebView);
    IView* viewTmp = IView::Probe(mWebView);
    AutoPtr<IContext> context;
    viewTmp->GetContext((IContext**)&context);
    mWebSettings = new ContentSettingsAdapter(new AwSettings(context, isAccessFromFileURLsGrantedByDefault, areLegacyQuirksEnabled));

    if (mAppTargetSdkVersion <= Build::VERSION_CODES::KITKAT) {
        mWebSettings->SetMixedContentMode(IWebSettings::MIXED_CONTENT_ALWAYS_ALLOW);
        // On KK and older versions we always allowed third party cookies.
        mWebSettings->SetAcceptThirdPartyCookies(TRUE);

        AutoPtr<AwSettings> awSettings;
        mWebSettings->GetAwSettings((AwSettings**)&awSettings);
        awSettings->SetZeroLayoutHeightDisablesViewportQuirk(TRUE);
    }

    AutoPtr<IRunnable> runnable;
    CThread::New((IRunnable**)&runnable);
    mRunQueue->AddTask(runnable);
    return NOERROR;
}

ECode WebViewChromium::StartYourEngine()
{
    // ==================before translated======================
    // mRunQueue.drainQueue();

    mRunQueue->DrainQueue();
    return NOERROR;
}

ECode WebViewChromium::SetHorizontalScrollbarOverlay(
    /* [in] */ Boolean overlay)
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             setHorizontalScrollbarOverlay(overlay);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.setHorizontalScrollbarOverlay(overlay);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerSetHorizontalScrollbarOverlayRunnable(this, overlay);
        mRunQueue->AddTask(runnable);
        return NOERROR;
    }

    mAwContents->SetHorizontalScrollbarOverlay(overlay);
    return NOERROR;
}

ECode WebViewChromium::SetVerticalScrollbarOverlay(
    /* [in] */ Boolean overlay)
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             setVerticalScrollbarOverlay(overlay);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.setVerticalScrollbarOverlay(overlay);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerSetVerticalScrollbarOverlayRunnable(this, overlay);
        mRunQueue->AddTask(runnable);
        return NOERROR;
    }

    mAwContents->SetVerticalScrollbarOverlay(overlay);
    return NOERROR;
}

ECode WebViewChromium::OverlayHorizontalScrollbar(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     boolean ret = runOnUiThreadBlocking(new Callable<Boolean>() {
    //         @Override
    //         public Boolean call() {
    //             return overlayHorizontalScrollbar();
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.overlayHorizontalScrollbar();

    mFactory->StartYourEngines(FALSE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerOverlayHorizontalScrollbarCallable(this);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IBoolean* boolTmp = IBoolean::Probe(interfaceTmp);
        boolTmp->GetValue(result);
        return NOERROR;
    }

    *result = mAwContents->OverlayHorizontalScrollbar();
    return NOERROR;
}

ECode WebViewChromium::OverlayVerticalScrollbar(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     boolean ret = runOnUiThreadBlocking(new Callable<Boolean>() {
    //         @Override
    //         public Boolean call() {
    //             return overlayVerticalScrollbar();
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.overlayVerticalScrollbar();

    mFactory->StartYourEngines(FALSE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerOverlayVerticalScrollbarCallable(this);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IBoolean* boolTmp = IBoolean::Probe(interfaceTmp);
        boolTmp->GetValue(result);
        return NOERROR;
    }

    *result = mAwContents->OverlayVerticalScrollbar();
    return NOERROR;
}

ECode WebViewChromium::GetVisibleTitleHeight(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // This is deprecated in WebView and should always return 0.
    // return 0;

    *result = 0;
    return NOERROR;
}

ECode WebViewChromium::GetCertificate(
    /* [out] */ ISslCertificate** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(true);
    // if (checkNeedsPost()) {
    //     SslCertificate ret = runOnUiThreadBlocking(new Callable<SslCertificate>() {
    //         @Override
    //         public SslCertificate call() {
    //             return getCertificate();
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.getCertificate();

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerGetCertificateCallable(this);
        AutoPtr<IInterface> temp = RunOnUiThreadBlocking(callable);
        *result = ISslCertificate::Probe(temp);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<ISslCertificate> temp = mAwContents->GetCertificate();
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WebViewChromium::SetCertificate(
    /* [in] */ ISslCertificate* certificate)
{
    VALIDATE_NOT_NULL(certificate);
    // ==================before translated======================
    // // intentional no-op

    return NOERROR;
}

ECode WebViewChromium::SavePassword(
    /* [in] */ const String& host,
    /* [in] */ const String& username,
    /* [in] */ const String& password)
{
    // ==================before translated======================
    // // This is a deprecated API: intentional no-op.

    return NOERROR;
}

ECode WebViewChromium::SetHttpAuthUsernamePassword(
    /* [in] */ const String& host,
    /* [in] */ const String& realm,
    /* [in] */ const String& username,
    /* [in] */ const String& password)
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             setHttpAuthUsernamePassword(host, realm, username, password);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.setHttpAuthUsernamePassword(host, realm, username, password);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerSetHttpAuthUsernamePasswordRunnable(this, host, realm, username, password);
        mRunQueue->AddTask(runnable);
        return NOERROR;
    }

    mAwContents->SetHttpAuthUsernamePassword(host, realm, username, password);
    return NOERROR;
}

ECode WebViewChromium::GetHttpAuthUsernamePassword(
    /* [in] */ const String& host,
    /* [in] */ const String& realm,
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(true);
    // if (checkNeedsPost()) {
    //     String[] ret = runOnUiThreadBlocking(new Callable<String[]>() {
    //         @Override
    //         public String[] call() {
    //             return getHttpAuthUsernamePassword(host, realm);
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.getHttpAuthUsernamePassword(host, realm);

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerGetHttpAuthUsernamePasswordCallable(this, host, realm);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IList* list = IList::Probe(interfaceTmp);

        Int32 length = 0;
        list->GetSize(&length);
        *result = ArrayOf<String>::Alloc(length);
        String item;
        for (Int32 idx=0; idx<length; ++idx) {
            AutoPtr<IInterface> itemTmp;
            list->Get(idx, (IInterface**)&itemTmp);
            ICharSequence* charSequence = ICharSequence::Probe(itemTmp);
            charSequence->ToString(&item);
            (*result)->Set(idx, item);
        }
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    *result = mAwContents->GetHttpAuthUsernamePassword(host, realm);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WebViewChromium::Destroy()
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             destroy();
    //         }
    //     });
    //     return;
    // }
    //
    // mAwContents.destroy();
    // if (mGLfunctor != null) {
    //     mGLfunctor.destroy();
    //     mGLfunctor = null;
    // }

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerDestroyRunnable(this);
        mRunQueue->AddTask(runnable);
        return NOERROR;
    }

    mAwContents->Destroy();
    if (mGLfunctor != NULL) {
        mGLfunctor->Destroy();
        mGLfunctor = NULL;
    }
    return NOERROR;
}

ECode WebViewChromium::SetNetworkAvailable(
    /* [in] */ Boolean networkUp)
{
    // ==================before translated======================
    // // Note that this purely toggles the JS navigator.online property.
    // // It does not in affect chromium or network stack state in any way.
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             setNetworkAvailable(networkUp);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.setNetworkAvailable(networkUp);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerSetNetworkAvailableRunnable(this, networkUp);
        mRunQueue->AddTask(runnable);
        return NOERROR;
    }

    mAwContents->SetNetworkAvailable(networkUp);
    return NOERROR;
}

ECode WebViewChromium::SaveState(
    /* [in] */ IBundle* outState,
    /* [out] */ IWebBackForwardList** result)
{
    VALIDATE_NOT_NULL(outState);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(true);
    // if (checkNeedsPost()) {
    //     WebBackForwardList ret = runOnUiThreadBlocking(new Callable<WebBackForwardList>() {
    //         @Override
    //         public WebBackForwardList call() {
    //             return saveState(outState);
    //         }
    //     });
    //     return ret;
    // }
    // if (outState == null) return null;
    // if (!mAwContents.saveState(outState)) return null;
    // return copyBackForwardList();

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerSaveStateCallable(this, outState);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        *result = IWebBackForwardList::Probe(interfaceTmp);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    if (NULL == outState)
        *result = NULL;
    else if (!mAwContents->SaveState(outState))
        *result = NULL;
    else
        CopyBackForwardList(result);

    return NOERROR;
}

ECode WebViewChromium::SavePicture(
    /* [in] */ IBundle* b,
    /* [in] */ IFile* dest,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(b);
    VALIDATE_NOT_NULL(dest);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // Intentional no-op: hidden method on WebView.
    // return false;

    *result = FALSE;
    return NOERROR;
}

ECode WebViewChromium::RestorePicture(
    /* [in] */ IBundle* b,
    /* [in] */ IFile* src,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(b);
    VALIDATE_NOT_NULL(src);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // Intentional no-op: hidden method on WebView.
    // return false;

    *result = FALSE;
    return NOERROR;
}

ECode WebViewChromium::RestoreState(
    /* [in] */ IBundle* inState,
    /* [out] */ IWebBackForwardList** result)
{
    VALIDATE_NOT_NULL(inState);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(true);
    // if (checkNeedsPost()) {
    //     WebBackForwardList ret = runOnUiThreadBlocking(new Callable<WebBackForwardList>() {
    //         @Override
    //         public WebBackForwardList call() {
    //             return restoreState(inState);
    //         }
    //     });
    //     return ret;
    // }
    // if (inState == null) return null;
    // if (!mAwContents.restoreState(inState)) return null;
    // return copyBackForwardList();

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerRestoreStateCallable(this, inState);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        *result = IWebBackForwardList::Probe(interfaceTmp);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    if (NULL == inState)
        *result = NULL;
    else if (!mAwContents->RestoreState(inState))
        *result = NULL;
    else
        CopyBackForwardList(result);

    return NOERROR;
}

ECode WebViewChromium::LoadUrl(
    /* [in] */ const String& url,
    /* [in] */ IMap* additionalHttpHeaders)
{
    VALIDATE_NOT_NULL(additionalHttpHeaders);
    // ==================before translated======================
    // // TODO: We may actually want to do some sanity checks here (like filter about://chrome).
    //
    // // For backwards compatibility, apps targeting less than K will have JS URLs evaluated
    // // directly and any result of the evaluation will not replace the current page content.
    // // Matching Chrome behavior more closely; apps targetting >= K that load a JS URL will
    // // have the result of that URL replace the content of the current page.
    // final String JAVASCRIPT_SCHEME = "javascript:";
    // if (mAppTargetSdkVersion < Build.VERSION_CODES.KITKAT &&
    //         url != null && url.startsWith(JAVASCRIPT_SCHEME)) {
    //     mFactory.startYourEngines(true);
    //     if (checkNeedsPost()) {
    //         mRunQueue.addTask(new Runnable() {
    //             @Override
    //             public void run() {
    //                 mAwContents.evaluateJavaScriptEvenIfNotYetNavigated(
    //                         url.substring(JAVASCRIPT_SCHEME.length()));
    //             }
    //         });
    //     } else {
    //         mAwContents.evaluateJavaScriptEvenIfNotYetNavigated(
    //                 url.substring(JAVASCRIPT_SCHEME.length()));
    //     }
    //     return;
    // }
    //
    // LoadUrlParams params = new LoadUrlParams(url);
    // if (additionalHttpHeaders != null) params.setExtraHeaders(additionalHttpHeaders);
    // loadUrlOnUiThread(params);

    const String JAVASCRIPT_SCHEME("javascript:");
    if (mAppTargetSdkVersion < Build::VERSION_CODES::KITKAT && !url.IsEmpty() && url.StartWith(JAVASCRIPT_SCHEME)) {
        mFactory->StartYourEngines(TRUE);
        Boolean resTmp = FALSE;
        CheckNeedsPost(&resTmp);
        if (resTmp) {
            AutoPtr<IRunnable> javascriptRunnable = new InnerEvaluateJavaScriptRunnable(this, url, JAVASCRIPT_SCHEME);
            mRunQueue->AddTask(javascriptRunnable);
        }
        else {
            mAwContents->EvaluateJavaScriptEvenIfNotYetNavigated(url.Substring(JAVASCRIPT_SCHEME.GetLength()));
        }
        return NOERROR;
    }

    AutoPtr<LoadUrlParams> params = new LoadUrlParams(url);
    if (additionalHttpHeaders != NULL)
        params->SetExtraHeaders(additionalHttpHeaders);
    return LoadUrlOnUiThread(params);
}

ECode WebViewChromium::LoadUrl(
    /* [in] */ const String& url)
{
    // ==================before translated======================
    // // Early out to match old WebView implementation
    // if (url == null) {
    //     return;
    // }
    // loadUrl(url, null);

    // Early out to match old WebView implementation
    if (url.IsEmpty())
        return NOERROR;
    return LoadUrl(url, NULL);
}

ECode WebViewChromium::PostUrl(
    /* [in] */ const String& url,
    /* [in] */ ArrayOf<Byte>* postData)
{
    VALIDATE_NOT_NULL(postData);
    // ==================before translated======================
    // LoadUrlParams params = LoadUrlParams.createLoadHttpPostParams(url, postData);
    // Map<String,String> headers = new HashMap<String,String>();
    // headers.put("Content-Type", "application/x-www-form-urlencoded");
    // params.setExtraHeaders(headers);
    // loadUrlOnUiThread(params);

    AutoPtr<LoadUrlParams> params = LoadUrlParams::CreateLoadHttpPostParams(url, postData);
    AutoPtr<IMap> headers;
    CHashMap::New((IMap**)&headers);
    String keyTmp("Content-Type");
    String valueTmp("application/x-www-form-urlencoded");
    AutoPtr<ICharSequence> key;
    AutoPtr<ICharSequence> value;
    CString::New(keyTmp, (ICharSequence**)&key);
    CString::New(valueTmp, (ICharSequence**)&value);
    headers->Put(key, value);
    params->SetExtraHeaders(headers);
    return LoadUrlOnUiThread(params);
}

ECode WebViewChromium::LoadData(
    /* [in] */ const String& data,
    /* [in] */ const String& mimeType,
    /* [in] */ const String& encoding)
{
    // ==================before translated======================
    // loadUrlOnUiThread(LoadUrlParams.createLoadDataParams(
    //         fixupData(data), fixupMimeType(mimeType), isBase64Encoded(encoding)));

    String fixupData = FixupData(data);
    String fixupMimeType = FixupMimeType(mimeType);
    Boolean isBase64Encoded = IsBase64Encoded(encoding);
    AutoPtr<LoadUrlParams> params = LoadUrlParams::CreateLoadDataParams(fixupData, fixupMimeType, isBase64Encoded );
    return LoadUrlOnUiThread(params);
}

ECode WebViewChromium::LoadDataWithBaseURL(
    /* [in] */ const String& baseUrl,
    /* [in] */ const String& data,
    /* [in] */ const String& mimeType,
    /* [in] */ const String& encoding,
    /* [in] */ const String& historyUrl)
{
    // ==================before translated======================
    // data = fixupData(data);
    // mimeType = fixupMimeType(mimeType);
    // LoadUrlParams loadUrlParams;
    // baseUrl = fixupBase(baseUrl);
    // historyUrl = fixupHistory(historyUrl);
    //
    // if (baseUrl.startsWith("data:")) {
    //     // For backwards compatibility with WebViewClassic, we use the value of |encoding|
    //     // as the charset, as long as it's not "base64".
    //     boolean isBase64 = isBase64Encoded(encoding);
    //     loadUrlParams = LoadUrlParams.createLoadDataParamsWithBaseUrl(
    //             data, mimeType, isBase64, baseUrl, historyUrl, isBase64 ? null : encoding);
    // } else {
    //     // When loading data with a non-data: base URL, the classic WebView would effectively
    //     // "dump" that string of data into the WebView without going through regular URL
    //     // loading steps such as decoding URL-encoded entities. We achieve this same behavior by
    //     // base64 encoding the data that is passed here and then loading that as a data: URL.
    //     try {
    //         loadUrlParams = LoadUrlParams.createLoadDataParamsWithBaseUrl(
    //                 Base64.encodeToString(data.getBytes("utf-8"), Base64.DEFAULT), mimeType,
    //                 true, baseUrl, historyUrl, "utf-8");
    //     } catch (java.io.UnsupportedEncodingException e) {
    //         Log.wtf(TAG, "Unable to load data string " + data, e);
    //         return;
    //     }
    // }
    // loadUrlOnUiThread(loadUrlParams);

    String newData = FixupData(data);
    String newMimeType = FixupMimeType(mimeType);
    AutoPtr<LoadUrlParams> loadUrlParams;
    String newBaseUrl = FixupBase(baseUrl);
    String newHistoryUrl = FixupHistory(historyUrl);

    if (newBaseUrl.StartWith("newData:")) {
        // For backwards compatibility with WebViewClassic, we use the value of |encoding|
        // as the charset, as long as it's not "base64".
        Boolean isBase64 = IsBase64Encoded(encoding);
        String charset = isBase64 ? String("") : encoding;
        loadUrlParams = LoadUrlParams::CreateLoadDataParamsWithBaseUrl(
            newData, newMimeType, isBase64, newBaseUrl, newHistoryUrl, charset);
    }
    else {
        // When loading data with a non-data: base URL, the classic WebView would effectively
        // "dump" that string of data into the WebView without going through regular URL
        // loading steps such as decoding URL-encoded entities. We achieve this same behavior by
        // base64 encoding the data that is passed here and then loading that as a data: URL.
        try {
            AutoPtr<IBase64> base64;
            CBase64::AcquireSingleton((IBase64**)&base64);
            AutoPtr< ArrayOf<Byte> > base64StrBytes = newData.GetBytes();
            String base64Str;
            base64->EncodeToString(base64StrBytes, IBase64::DEFAULT, &base64Str);
            loadUrlParams = LoadUrlParams::CreateLoadDataParamsWithBaseUrl(base64Str,
                newMimeType, TRUE, newBaseUrl, newHistoryUrl, String("utf-8"));
        }
        //catch (java.io.UnsupportedEncodingException e) {
            //Log.wtf(TAG, "Unable to load data string " + newData, e);
            //return;
        //}
    }
    return LoadUrlOnUiThread(loadUrlParams);
}

ECode WebViewChromium::EvaluateJavaScript(
    /* [in] */ const String& script,
    /* [in] */ IValueCallback* resultCallback)
{
    VALIDATE_NOT_NULL(resultCallback);
    // ==================before translated======================
    // checkThread();
    // mAwContents.evaluateJavaScript(script, resultCallback);

    CheckThread();
    mAwContents->EvaluateJavaScript(script, resultCallback);
    return NOERROR;
}

ECode WebViewChromium::SaveWebArchive(
    /* [in] */ const String& filename)
{
    // ==================before translated======================
    // saveWebArchive(filename, false, null);

    return SaveWebArchive(filename, FALSE, NULL);
}

ECode WebViewChromium::SaveWebArchive(
    /* [in] */ const String& basename,
    /* [in] */ Boolean autoname,
    /* [in] */ IValueCallback* callback)
{
    VALIDATE_NOT_NULL(callback);
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             saveWebArchive(basename, autoname, callback);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.saveWebArchive(basename, autoname, callback);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerSaveWebArchiveRunnable(this, basename, autoname, callback);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->SaveWebArchive(basename, autoname, callback);
    return NOERROR;
}

ECode WebViewChromium::StopLoading()
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             stopLoading();
    //         }
    //     });
    //     return;
    // }
    //
    // mAwContents.stopLoading();

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerStopLoadingRunnable(this);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->StopLoading();
    return NOERROR;
}

ECode WebViewChromium::Reload()
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             reload();
    //         }
    //     });
    //     return;
    // }
    // mAwContents.reload();

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerReloadRunnable(this);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->Reload();
    return NOERROR;
}

ECode WebViewChromium::CanGoBack(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(true);
    // if (checkNeedsPost()) {
    //     Boolean ret = runOnUiThreadBlocking(new Callable<Boolean>() {
    //         @Override
    //         public Boolean call() {
    //             return canGoBack();
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.canGoBack();

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerCanGoBackCallable(this);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IBoolean* interfaceTmp1 = IBoolean::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    *result = mAwContents->CanGoBack();
    return NOERROR;
}

ECode WebViewChromium::GoBack()
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             goBack();
    //         }
    //     });
    //     return;
    // }
    // mAwContents.goBack();

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerGoBackRunnable(this);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->GoBack();
    return NOERROR;
}

ECode WebViewChromium::CanGoForward(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(true);
    // if (checkNeedsPost()) {
    //     Boolean ret = runOnUiThreadBlocking(new Callable<Boolean>() {
    //         @Override
    //         public Boolean call() {
    //             return canGoForward();
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.canGoForward();

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerCanGoForwardCallable(this);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IBoolean* interfaceTmp1 = IBoolean::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    *result = mAwContents->CanGoForward();
    return NOERROR;
}

ECode WebViewChromium::GoForward()
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             goForward();
    //         }
    //     });
    //     return;
    // }
    // mAwContents.goForward();

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerGoForwardRunnable(this);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->GoForward();
    return NOERROR;
}

ECode WebViewChromium::CanGoBackOrForward(
    /* [in] */ Int32 steps,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(true);
    // if (checkNeedsPost()) {
    //     Boolean ret = runOnUiThreadBlocking(new Callable<Boolean>() {
    //         @Override
    //         public Boolean call() {
    //             return canGoBackOrForward(steps);
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.canGoBackOrForward(steps);

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerCanGoBackOrForwardCallable(this, steps);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IBoolean* interfaceTmp1 = IBoolean::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    *result = mAwContents->CanGoBackOrForward(steps);
    return NOERROR;
}

ECode WebViewChromium::GoBackOrForward(
    /* [in] */ Int32 steps)
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             goBackOrForward(steps);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.goBackOrForward(steps);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerGoBackOrForwardRunnable(this, steps);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->GoBackOrForward(steps);
    return NOERROR;
}

ECode WebViewChromium::IsPrivateBrowsingEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // Not supported in this WebView implementation.
    // return false;

    *result = FALSE;
    return NOERROR;
}

ECode WebViewChromium::PageUp(
    /* [in] */ Boolean top,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(true);
    // if (checkNeedsPost()) {
    //     Boolean ret = runOnUiThreadBlocking(new Callable<Boolean>() {
    //         @Override
    //         public Boolean call() {
    //             return pageUp(top);
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.pageUp(top);

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerPageUpCallable(this, top);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IBoolean* interfaceTmp1 = IBoolean::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    *result = mAwContents->PageUp(top);
    return NOERROR;
}

ECode WebViewChromium::PageDown(
    /* [in] */ Boolean bottom,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(true);
    // if (checkNeedsPost()) {
    //     Boolean ret = runOnUiThreadBlocking(new Callable<Boolean>() {
    //         @Override
    //         public Boolean call() {
    //             return pageDown(bottom);
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.pageDown(bottom);

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerPageDownCallable(this, bottom);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IBoolean* interfaceTmp1 = IBoolean::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    *result = mAwContents->PageDown(bottom);
    return NOERROR;
}

ECode WebViewChromium::ClearView()
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             clearView();
    //         }
    //     });
    //     return;
    // }
    // mAwContents.clearView();

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerClearViewRunnable(this);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->ClearView();
    return NOERROR;
}

ECode WebViewChromium::CapturePicture(
    /* [out] */ IPicture** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(true);
    // if (checkNeedsPost()) {
    //     Picture ret = runOnUiThreadBlocking(new Callable<Picture>() {
    //         @Override
    //         public Picture call() {
    //             return capturePicture();
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.capturePicture();

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerCapturePictureCallable(this);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        *result = IPicture::Probe(interfaceTmp);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<IPicture> temp = mAwContents->CapturePicture();
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WebViewChromium::GetScale(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // No checkThread() as it is mostly thread safe (workaround for b/10652991).
    // mFactory.startYourEngines(true);
    // return mAwContents.getScale();

    mFactory->StartYourEngines(TRUE);
    *result = mAwContents->GetScale();
    return NOERROR;
}

ECode WebViewChromium::SetInitialScale(
    /* [in] */ Int32 scaleInPercent)
{
    // ==================before translated======================
    // // No checkThread() as it is thread safe
    // mWebSettings.getAwSettings().setInitialPageScale(scaleInPercent);

    AutoPtr<AwSettings> awSettings;
    mWebSettings->GetAwSettings((AwSettings**)&awSettings);
    awSettings->SetInitialPageScale(scaleInPercent);
    return NOERROR;
}

ECode WebViewChromium::InvokeZoomPicker()
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             invokeZoomPicker();
    //         }
    //     });
    //     return;
    // }
    // mAwContents.invokeZoomPicker();

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerInvokeZoomPickerRunnable(this);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->InvokeZoomPicker();
    return NOERROR;
}

ECode WebViewChromium::GetHitTestResult(
    /* [out] */ IWebViewHitTestResult** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(true);
    // if (checkNeedsPost()) {
    //     WebView.HitTestResult ret = runOnUiThreadBlocking(
    //             new Callable<WebView.HitTestResult>() {
    //         @Override
    //         public WebView.HitTestResult call() {
    //             return getHitTestResult();
    //         }
    //     });
    //     return ret;
    // }
    // AwContents.HitTestData data = mAwContents.getLastHitTestResult();
    // mHitTestResult.setType(data.hitTestResultType);
    // mHitTestResult.setExtra(data.hitTestResultExtraData);
    // return mHitTestResult;

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerGetHitTestResultCallable(this);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        *result = IWebViewHitTestResult::Probe(interfaceTmp);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<AwContents::HitTestData> data = mAwContents->GetLastHitTestResult();
    mHitTestResult->SetType(data->hitTestResultType);
    mHitTestResult->SetExtra(data->hitTestResultExtraData);
    *result = mHitTestResult;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WebViewChromium::RequestFocusNodeHref(
    /* [in] */ IMessage* hrefMsg)
{
    VALIDATE_NOT_NULL(hrefMsg);
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             requestFocusNodeHref(hrefMsg);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.requestFocusNodeHref(hrefMsg);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerRequestFocusNodeHrefRunnable(this, hrefMsg);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->RequestFocusNodeHref(hrefMsg);
    return NOERROR;
}

ECode WebViewChromium::RequestImageRef(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             requestImageRef(msg);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.requestImageRef(msg);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerRequestImageRefRunnable(this, msg);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->RequestImageRef(msg);
    return NOERROR;
}

ECode WebViewChromium::GetUrl(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(true);
    // if (checkNeedsPost()) {
    //     String ret = runOnUiThreadBlocking(new Callable<String>() {
    //         @Override
    //         public String call() {
    //             return getUrl();
    //         }
    //     });
    //     return ret;
    // }
    // String url =  mAwContents.getUrl();
    // if (url == null || url.trim().isEmpty()) return null;
    // return url;

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerGetUrlCallable(this);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        ICharSequence* interfaceTmp1 = ICharSequence::Probe(interfaceTmp);
        interfaceTmp1->ToString(result);
        return NOERROR;
    }

    String url = mAwContents->GetUrl();
    if (url.IsEmpty() || url.Trim().IsEmpty())
        *result = String("");
    else
        *result = url;

    return NOERROR;
}

ECode WebViewChromium::GetOriginalUrl(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(true);
    // if (checkNeedsPost()) {
    //     String ret = runOnUiThreadBlocking(new Callable<String>() {
    //         @Override
    //         public String call() {
    //             return getOriginalUrl();
    //         }
    //     });
    //     return ret;
    // }
    // String url =  mAwContents.getOriginalUrl();
    // if (url == null || url.trim().isEmpty()) return null;
    // return url;

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerGetOriginalUrlCallable(this);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        ICharSequence* interfaceTmp1 = ICharSequence::Probe(interfaceTmp);
        interfaceTmp1->ToString(result);
        return NOERROR;
    }

    String url = mAwContents->GetOriginalUrl();
    if (url.IsEmpty() || url.Trim().IsEmpty())
        *result = String("");
    else
        *result = url;

    return NOERROR;
}

ECode WebViewChromium::GetTitle(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(true);
    // if (checkNeedsPost()) {
    //     String ret = runOnUiThreadBlocking(new Callable<String>() {
    //         @Override
    //         public String call() {
    //             return getTitle();
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.getTitle();

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerGetTitleCallable(this);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        ICharSequence* interfaceTmp1 = ICharSequence::Probe(interfaceTmp);
        interfaceTmp1->ToString(result);
        return NOERROR;
    }

    *result = mAwContents->GetTitle();
    return NOERROR;
}

ECode WebViewChromium::GetFavicon(
    /* [out] */ IBitmap** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(true);
    // if (checkNeedsPost()) {
    //     Bitmap ret = runOnUiThreadBlocking(new Callable<Bitmap>() {
    //         @Override
    //         public Bitmap call() {
    //             return getFavicon();
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.getFavicon();

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerGetFaviconCallable(this);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        *result = IBitmap::Probe(interfaceTmp);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<IBitmap> temp = mAwContents->GetFavicon();
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WebViewChromium::GetTouchIconUrl(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // Intentional no-op: hidden method on WebView.
    // return null;

    *result = String("");
    return NOERROR;
}

ECode WebViewChromium::GetProgress(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mAwContents == null) return 100;
    // // No checkThread() because the value is cached java side (workaround for b/10533304).
    // return mAwContents.getMostRecentProgress();

    if (mAwContents == NULL) {
        *result = 100;
        return NOERROR;
    }
    // No checkThread() because the value is cached java side (workaround for b/10533304).
    *result = mAwContents->GetMostRecentProgress();
    return NOERROR;
}

ECode WebViewChromium::GetContentHeight(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mAwContents == null) return 0;
    // // No checkThread() as it is mostly thread safe (workaround for b/10594869).
    // return mAwContents.getContentHeightCss();

    if (mAwContents == NULL) {
        *result = 0;
        return NOERROR;
    }
    // No checkThread() as it is mostly thread safe (workaround for b/10594869).
    *result = mAwContents->GetContentHeightCss();
    return NOERROR;
}

ECode WebViewChromium::GetContentWidth(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mAwContents == null) return 0;
    // // No checkThread() as it is mostly thread safe (workaround for b/10594869).
    // return mAwContents.getContentWidthCss();

    if (mAwContents == NULL) {
        *result = 0;
        return NOERROR;
    }
    // No checkThread() as it is mostly thread safe (workaround for b/10594869).
    *result = mAwContents->GetContentWidthCss();
    return NOERROR;
}

ECode WebViewChromium::PauseTimers()
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             pauseTimers();
    //         }
    //     });
    //     return;
    // }
    // mAwContents.pauseTimers();

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerPauseTimersRunnable(this);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->PauseTimers();
    return NOERROR;
}

ECode WebViewChromium::ResumeTimers()
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             resumeTimers();
    //         }
    //     });
    //     return;
    // }
    // mAwContents.resumeTimers();

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerResumeTimersRunnable(this);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->ResumeTimers();
    return NOERROR;
}

ECode WebViewChromium::OnPause()
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             onPause();
    //         }
    //     });
    //     return;
    // }
    // mAwContents.onPause();

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerOnPauseRunnable(this);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->OnPause();
    return NOERROR;
}

ECode WebViewChromium::OnResume()
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             onResume();
    //         }
    //     });
    //     return;
    // }
    // mAwContents.onResume();

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerOnResumeRunnable(this);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->OnResume();
    return NOERROR;
}

ECode WebViewChromium::IsPaused(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(true);
    // if (checkNeedsPost()) {
    //     Boolean ret = runOnUiThreadBlocking(new Callable<Boolean>() {
    //         @Override
    //         public Boolean call() {
    //             return isPaused();
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.isPaused();

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerIsPausedCallable(this);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IBoolean* interfaceTmp1 = IBoolean::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    *result = mAwContents->IsPaused();
    return NOERROR;
}

ECode WebViewChromium::FreeMemory()
{
    // ==================before translated======================
    // // Intentional no-op. Memory is managed automatically by Chromium.

    return NOERROR;
}

ECode WebViewChromium::ClearCache(
    /* [in] */ Boolean includeDiskFiles)
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             clearCache(includeDiskFiles);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.clearCache(includeDiskFiles);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerClearCacheRunnable(this, includeDiskFiles);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->ClearCache(includeDiskFiles);
    return NOERROR;
}

ECode WebViewChromium::ClearFormData()
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             clearFormData();
    //         }
    //     });
    //     return;
    // }
    // mAwContents.hideAutofillPopup();

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerClearFormDataRunnable(this);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->HideAutofillPopup();
    return NOERROR;
}

ECode WebViewChromium::ClearHistory()
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             clearHistory();
    //         }
    //     });
    //     return;
    // }
    // mAwContents.clearHistory();

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerClearHistoryRunnable(this);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->ClearHistory();
    return NOERROR;
}

ECode WebViewChromium::ClearSslPreferences()
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             clearSslPreferences();
    //         }
    //     });
    //     return;
    // }
    // mAwContents.clearSslPreferences();

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerClearSslPreferencesRunnable(this);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->ClearSslPreferences();
    return NOERROR;
}

ECode WebViewChromium::CopyBackForwardList(
    /* [out] */ IWebBackForwardList** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(true);
    // if (checkNeedsPost()) {
    //     WebBackForwardList ret = runOnUiThreadBlocking(new Callable<WebBackForwardList>() {
    //         @Override
    //         public WebBackForwardList call() {
    //             return copyBackForwardList();
    //         }
    //     });
    //     return ret;
    // }
    // return new WebBackForwardListChromium(
    //         mAwContents.getNavigationHistory());

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerCopyBackForwardListCallable(this);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        *result = IWebBackForwardList::Probe(interfaceTmp);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    *result = new WebBackForwardListChromium(mAwContents->GetNavigationHistory());
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WebViewChromium::SetFindListener(
    /* [in] */ IWebViewFindListener* listener)
{
    // ==================before translated======================
    // mContentsClientAdapter.setFindListener(listener);

    mContentsClientAdapter->SetFindListener(listener);
    return NOERROR;
}

ECode WebViewChromium::FindNext(
    /* [in] */ Boolean forwards)
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             findNext(forwards);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.findNext(forwards);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerFindNextRunnable(this, forwards);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->FindNext(forwards);
    return NOERROR;
}

ECode WebViewChromium::FindAll(
    /* [in] */ const String& searchString,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // findAllAsync(searchString);
    // return 0;

    FindAllAsync(searchString);
    *result = 0;
    return NOERROR;
}

ECode WebViewChromium::FindAllAsync(
    /* [in] */ const String& searchString)
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             findAllAsync(searchString);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.findAllAsync(searchString);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerFindAllAsyncRunnable(this, searchString);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->FindAllAsync(searchString);
    return NOERROR;
}

ECode WebViewChromium::ShowFindDialog(
    /* [in] */ const String& text,
    /* [in] */ Boolean showIme,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     return false;
    // }
    // if (mWebView.getParent() == null) {
    //     return false;
    // }
    //
    // FindActionModeCallback findAction = new FindActionModeCallback(mWebView.getContext());
    // if (findAction == null) {
    //     return false;
    // }
    //
    // mWebView.startActionMode(findAction);
    // findAction.setWebView(mWebView);
    // if (showIme) {
    //     findAction.showSoftInput();
    // }
    //
    // if (text != null) {
    //     findAction.setText(text);
    //     findAction.findAll();
    // }
    //
    // return true;

    mFactory->StartYourEngines(FALSE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IViewParent> viewParent;
    IView* viewTmp = IView::Probe(mWebView);
    viewTmp->GetParent((IViewParent**)&viewParent);
    if (NULL == viewParent) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IContext> context;
    viewTmp->GetContext((IContext**)&context);
    AutoPtr<FindActionModeCallback> findAction = new FindActionModeCallback(context);
    if (NULL == findAction) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IActionMode> actionMode;
    viewTmp->StartActionMode(findAction, (IActionMode**)&actionMode);
    findAction->SetWebView(NULL/* base class params is wrong: mWebView*/);
    if (showIme) {
        findAction->ShowSoftInput();
    }

    if (text != NULL) {
        findAction->SetText(text);
        findAction->FindAll();
    }

    *result = TRUE;
    return NOERROR;
}

ECode WebViewChromium::NotifyFindDialogDismissed()
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             notifyFindDialogDismissed();
    //         }
    //     });
    //     return;
    // }
    // clearMatches();

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerNotifyFindDialogDismissedRunnable(this);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->ClearMatches();
    return NOERROR;
}

ECode WebViewChromium::ClearMatches()
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             clearMatches();
    //         }
    //     });
    //     return;
    // }
    // mAwContents.clearMatches();

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerClearMatchesRunnable(this);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->ClearMatches();
    return NOERROR;
}

ECode WebViewChromium::DocumentHasImages(
    /* [in] */ IMessage* response)
{
    VALIDATE_NOT_NULL(response);
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             documentHasImages(response);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.documentHasImages(response);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerDocumentHasImagesRunnable(this, response);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->DocumentHasImages(response);
    return NOERROR;
}

ECode WebViewChromium::SetWebViewClient(
    /* [in] */ IWebViewClient* client)
{
    VALIDATE_NOT_NULL(client);
    // ==================before translated======================
    // mContentsClientAdapter.setWebViewClient(client);

    mContentsClientAdapter->SetWebViewClient(client);
    return NOERROR;
}

ECode WebViewChromium::SetDownloadListener(
    /* [in] */ IDownloadListener* listener)
{
    VALIDATE_NOT_NULL(listener);
    // ==================before translated======================
    // mContentsClientAdapter.setDownloadListener(listener);

    mContentsClientAdapter->SetDownloadListener(listener);
    return NOERROR;
}

ECode WebViewChromium::SetWebChromeClient(
    /* [in] */ IWebChromeClient* client)
{
    VALIDATE_NOT_NULL(client);
    // ==================before translated======================
    // mWebSettings.getAwSettings().setFullscreenSupported(doesSupportFullscreen(client));
    // mContentsClientAdapter.setWebChromeClient(client);

    AutoPtr<AwSettings> awSettings;
    mWebSettings->GetAwSettings((AwSettings**)&awSettings);
    awSettings->SetFullscreenSupported(DoesSupportFullscreen(client));
    mContentsClientAdapter->SetWebChromeClient(client);
    return NOERROR;
}

ECode WebViewChromium::SetPictureListener(
    /* [in] */ IWebViewPictureListener* listener)
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             setPictureListener(listener);
    //         }
    //     });
    //     return;
    // }
    // mContentsClientAdapter.setPictureListener(listener);
    // mAwContents.enableOnNewPicture(listener != null,
    //         mAppTargetSdkVersion >= Build.VERSION_CODES.JELLY_BEAN_MR2);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerSetPictureListenerRunnable(this, listener);
        mRunQueue->AddTask(runnable);
    }

    mContentsClientAdapter->SetPictureListener(listener);
    mAwContents->EnableOnNewPicture(listener != NULL, mAppTargetSdkVersion >= Build::VERSION_CODES::JELLY_BEAN_MR2);
    return NOERROR;
}

ECode WebViewChromium::AddJavascriptInterface(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& interfaceName)
{
    VALIDATE_NOT_NULL(obj);
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             addJavascriptInterface(obj, interfaceName);
    //         }
    //     });
    //     return;
    // }
    // Class<? extends Annotation> requiredAnnotation = null;
    // if (mAppTargetSdkVersion >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
    //    requiredAnnotation = JavascriptInterface.class;
    // }
    // mAwContents.addPossiblyUnsafeJavascriptInterface(obj, interfaceName, requiredAnnotation);

    IObject* objTmp = IObject::Probe(obj);
    Object* objTmp1 = (Object*)objTmp;

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerAddJavascriptInterfaceRunnable(this, objTmp1, interfaceName);
        mRunQueue->AddTask(runnable);
    }

    AutoPtr<JavascriptInterface> requiredAnnotation = NULL;
    if (mAppTargetSdkVersion >= Build::VERSION_CODES::JELLY_BEAN_MR1) {
        requiredAnnotation = new JavascriptInterface();
    }

    IInterface* annotationTmp = TO_IINTERFACE(requiredAnnotation);
    mAwContents->AddPossiblyUnsafeJavascriptInterface(objTmp1, interfaceName, annotationTmp);
    return NOERROR;
}

ECode WebViewChromium::RemoveJavascriptInterface(
    /* [in] */ const String& interfaceName)
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             removeJavascriptInterface(interfaceName);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.removeJavascriptInterface(interfaceName);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerRemoveJavascriptInterfaceRunnable(this, interfaceName);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->RemoveJavascriptInterface(interfaceName);
    return NOERROR;
}

ECode WebViewChromium::GetSettings(
    /* [out] */ IWebSettings** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWebSettings;

    *result = mWebSettings;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WebViewChromium::SetMapTrackballToArrowKeys(
    /* [in] */ Boolean setMap)
{
    // ==================before translated======================
    // // This is a deprecated API: intentional no-op.

    return NOERROR;
}

ECode WebViewChromium::FlingScroll(
    /* [in] */ Int32 vx,
    /* [in] */ Int32 vy)
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             flingScroll(vx, vy);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.flingScroll(vx, vy);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerFlingScrollRunnable(this, vx, vy);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->FlingScroll(vx, vy);
    return NOERROR;
}

ECode WebViewChromium::GetZoomControls(
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     return null;
    // }
    //
    // // This was deprecated in 2009 and hidden in JB MR1, so just provide the minimum needed
    // // to stop very out-dated applications from crashing.
    // Log.w(TAG, "WebView doesn't support getZoomControls");
    // return mAwContents.getSettings().supportZoom() ? new View(mWebView.getContext()) : null;

    mFactory->StartYourEngines(FALSE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        *result = NULL;
        return NOERROR;
    }

    // This was deprecated in 2009 and hidden in JB MR1, so just provide the minimum needed
    // to stop very out-dated applications from crashing.
    Logger::W(TAG, "WebView doesn't support getZoomControls");

    IView* viewTmp = IView::Probe(mWebView);
    AutoPtr<IContext> context;
    viewTmp->GetContext((IContext**)&context);
    if (mAwContents->GetSettings()->SupportZoom()) {
        CView::New(context, result);
        REFCOUNT_ADD(*result);
    }
    else {
        *result = NULL;
    }

    return NOERROR;
}

ECode WebViewChromium::CanZoomIn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     return false;
    // }
    // return mAwContents.canZoomIn();

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp)
        *result = FALSE;
    else
        *result = mAwContents->CanZoomIn();

    return NOERROR;
}

ECode WebViewChromium::CanZoomOut(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     return false;
    // }
    // return mAwContents.canZoomOut();

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp)
        *result = FALSE;
    else
        *result = mAwContents->CanZoomOut();

    return NOERROR;
}

ECode WebViewChromium::ZoomIn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(true);
    // if (checkNeedsPost()) {
    //     boolean ret = runOnUiThreadBlocking(new Callable<Boolean>() {
    //         @Override
    //         public Boolean call() {
    //             return zoomIn();
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.zoomIn();

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerZoomInCallable(this);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IBoolean* interfaceTmp1 = IBoolean::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    *result = mAwContents->ZoomIn();
    return NOERROR;
}

ECode WebViewChromium::ZoomOut(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(true);
    // if (checkNeedsPost()) {
    //     boolean ret = runOnUiThreadBlocking(new Callable<Boolean>() {
    //         @Override
    //         public Boolean call() {
    //             return zoomOut();
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.zoomOut();

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerZoomOutCallable(this);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IBoolean* interfaceTmp1 = IBoolean::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    *result = mAwContents->ZoomOut();
    return NOERROR;
}

ECode WebViewChromium::ZoomBy(
    /* [in] */ Float factor,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(true);
    // // This is an L API and therefore we can enforce stricter threading constraints.
    // checkThread();
    // return mAwContents.zoomBy(factor);

    mFactory->StartYourEngines(TRUE);
    CheckThread();
    *result = mAwContents->ZoomBy(factor);
    return NOERROR;
}

ECode WebViewChromium::DumpViewHierarchyWithProperties(
    /* [in] */ IBufferedWriter* out,
    /* [in] */ Int32 level)
{
    VALIDATE_NOT_NULL(out);
    // ==================before translated======================
    // // Intentional no-op

    return NOERROR;
}

ECode WebViewChromium::FindHierarchyView(
    /* [in] */ const String& className,
    /* [in] */ Int32 hashCode,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // Intentional no-op
    // return null;

    *result = NULL;
    return NOERROR;
}

ECode WebViewChromium::GetViewDelegate(
    /* [out] */ IWebViewProviderViewDelegate** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return this;

    *result = this;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WebViewChromium::GetScrollDelegate(
    /* [out] */ IWebViewProviderScrollDelegate** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return this;

    *result = this;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WebViewChromium::ShouldDelayChildPressedState(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     boolean ret = runOnUiThreadBlocking(new Callable<Boolean>() {
    //         @Override
    //         public Boolean call() {
    //             return shouldDelayChildPressedState();
    //         }
    //     });
    //     return ret;
    // }
    // return true;

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerShouldDelayChildPressedStateCallable(this);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IBoolean* interfaceTmp1 = IBoolean::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    *result = TRUE;
    return NOERROR;
}

ECode WebViewChromium::GetAccessibilityNodeProvider(
    /* [out] */ IAccessibilityNodeProvider** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     AccessibilityNodeProvider ret = runOnUiThreadBlocking(
    //             new Callable<AccessibilityNodeProvider>() {
    //         @Override
    //         public AccessibilityNodeProvider call() {
    //             return getAccessibilityNodeProvider();
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.getAccessibilityNodeProvider();

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerGetAccessibilityNodeProviderCallable(this);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        *result = IAccessibilityNodeProvider::Probe(interfaceTmp);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<IAccessibilityNodeProvider> temp = mAwContents->GetAccessibilityNodeProvider();
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WebViewChromium::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    VALIDATE_NOT_NULL(info);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     runVoidTaskOnUiThreadBlocking(new Runnable() {
    //         @Override
    //         public void run() {
    //             onInitializeAccessibilityNodeInfo(info);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.onInitializeAccessibilityNodeInfo(info);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerOnInitializeAccessibilityNodeInfoRunnable(this, info);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->OnInitializeAccessibilityNodeInfo(info);
    return NOERROR;
}

ECode WebViewChromium::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    VALIDATE_NOT_NULL(event);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     runVoidTaskOnUiThreadBlocking(new Runnable() {
    //         @Override
    //         public void run() {
    //             onInitializeAccessibilityEvent(event);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.onInitializeAccessibilityEvent(event);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerOnInitializeAccessibilityEventRunnable(this, event);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->OnInitializeAccessibilityEvent(event);
    return NOERROR;
}

ECode WebViewChromium::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(arguments);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     boolean ret = runOnUiThreadBlocking(new Callable<Boolean>() {
    //         @Override
    //         public Boolean call() {
    //             return performAccessibilityAction(action, arguments);
    //         }
    //     });
    //     return ret;
    // }
    // if (mAwContents.supportsAccessibilityAction(action)) {
    //     return mAwContents.performAccessibilityAction(action, arguments);
    // }
    // return mWebViewPrivate.super_performAccessibilityAction(action, arguments);

    mFactory->StartYourEngines(FALSE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerPerformAccessibilityActionCallable(this, action, arguments);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IBoolean* interfaceTmp1 = IBoolean::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }
    if (mAwContents->SupportsAccessibilityAction(action)) {
        *result = mAwContents->PerformAccessibilityAction(action, arguments);
        return NOERROR;
    }

    return mWebViewPrivate->Super_performAccessibilityAction(action, arguments, result);
}

ECode WebViewChromium::SetOverScrollMode(
    /* [in] */ Int32 mode)
{
    // ==================before translated======================
    // // This gets called from the android.view.View c'tor that WebView inherits from. This
    // // causes the method to be called when mAwContents == null.
    // // It's safe to ignore these calls however since AwContents will read the current value of
    // // this setting when it's created.
    // if (mAwContents == null) return;
    //
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             setOverScrollMode(mode);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.setOverScrollMode(mode);

    // This gets called from the android.view.View c'tor that WebView inherits from. This
    // causes the method to be called when mAwContents == null.
    // It's safe to ignore these calls however since AwContents will read the current value of
    // this setting when it's created.
    if (NULL == mAwContents)
        return NOERROR;

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerSetOverScrollModeRunnable(this, mode);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->SetOverScrollMode(mode);
    return NOERROR;
}

ECode WebViewChromium::SetScrollBarStyle(
    /* [in] */ Int32 style)
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             setScrollBarStyle(style);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.setScrollBarStyle(style);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerSetScrollBarStyleRunnable(this, style);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->SetScrollBarStyle(style);
    return NOERROR;
}

ECode WebViewChromium::OnDrawVerticalScrollBar(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IDrawable* scrollBar,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    VALIDATE_NOT_NULL(canvas);
    VALIDATE_NOT_NULL(scrollBar);
    // ==================before translated======================
    // // WebViewClassic was overriding this method to handle rubberband over-scroll. Since
    // // WebViewChromium doesn't support that the vanilla implementation of this method can be
    // // used.
    // mWebViewPrivate.super_onDrawVerticalScrollBar(canvas, scrollBar, l, t, r, b);

    // WebViewClassic was overriding this method to handle rubberband over-scroll. Since
    // WebViewChromium doesn't support that the vanilla implementation of this method can be
    // used.
    mWebViewPrivate->Super_onDrawVerticalScrollBar(canvas, scrollBar, l, t, r, b);
    return NOERROR;
}

ECode WebViewChromium::OnOverScrolled(
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY,
    /* [in] */ Boolean clampedX,
    /* [in] */ Boolean clampedY)
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             onOverScrolled(scrollX, scrollY, clampedX, clampedY);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.onContainerViewOverScrolled(scrollX, scrollY, clampedX, clampedY);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerOnOverScrolledRunnable(this, scrollX, scrollY, clampedX, clampedY);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->OnContainerViewOverScrolled(scrollX, scrollY, clampedX, clampedY);
    return NOERROR;
}

ECode WebViewChromium::OnWindowVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             onWindowVisibilityChanged(visibility);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.onWindowVisibilityChanged(visibility);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerOnWindowVisibilityChangedRunnable(this, visibility);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->OnWindowVisibilityChanged(visibility);
    return NOERROR;
}

ECode WebViewChromium::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    VALIDATE_NOT_NULL(canvas);
    // ==================before translated======================
    // mFactory.startYourEngines(true);
    // if (checkNeedsPost()) {
    //     runVoidTaskOnUiThreadBlocking(new Runnable() {
    //         @Override
    //         public void run() {
    //             onDraw(canvas);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.onDraw(canvas);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerOnDrawRunnable(this, canvas);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->OnDraw(canvas);
    return NOERROR;
}

ECode WebViewChromium::SetLayoutParams(
    /* [in] */ IViewGroupLayoutParams* layoutParams)
{
    // ==================before translated======================
    // // This API is our strongest signal from the View system that this
    // // WebView is going to be bound to a View hierarchy and so at this
    // // point we must bind Chromium's UI thread to the current thread.
    // mFactory.startYourEngines(false);
    // checkThread();
    // mWebViewPrivate.super_setLayoutParams(layoutParams);

    // This API is our strongest signal from the View system that this
    // WebView is going to be bound to a View hierarchy and so at this
    // point we must bind Chromium's UI thread to the current thread.
    mFactory->StartYourEngines(FALSE);
    CheckThread();
    mWebViewPrivate->Super_setLayoutParams(layoutParams);
    return NOERROR;
}

ECode WebViewChromium::PerformLongClick(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // Return false unless the WebView is attached to a View with a parent
    // return mWebView.getParent() != null ? mWebViewPrivate.super_performLongClick() : false;

    // Return false unless the WebView is attached to a View with a parent
    IView* viewTmp = IView::Probe(mWebView);
    AutoPtr<IViewParent> viewParent;
    viewTmp->GetParent((IViewParent**)&viewParent);

    Boolean click = FALSE;
    mWebViewPrivate->Super_performLongClick(&click);
    *result = viewParent != NULL ? click : FALSE;
    return NOERROR;
}

ECode WebViewChromium::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    VALIDATE_NOT_NULL(newConfig);
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             onConfigurationChanged(newConfig);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.onConfigurationChanged(newConfig);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerOnConfigurationChangedRunnable(this, newConfig);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->OnConfigurationChanged(newConfig);
    return NOERROR;
}

ECode WebViewChromium::OnCreateInputConnection(
    /* [in] */ IEditorInfo* outAttrs,
    /* [out] */ IInputConnection** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(outAttrs);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //    return null;
    // }
    // return mAwContents.onCreateInputConnection(outAttrs);

    mFactory->StartYourEngines(FALSE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {

       return NOERROR;
    }

    AutoPtr<IInputConnection> temp = mAwContents->OnCreateInputConnection(outAttrs);
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WebViewChromium::OnKeyMultiple(
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 repeatCount,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     boolean ret = runOnUiThreadBlocking(new Callable<Boolean>() {
    //         @Override
    //         public Boolean call() {
    //             return onKeyMultiple(keyCode, repeatCount, event);
    //         }
    //     });
    //     return ret;
    // }
    // UnimplementedWebViewApi.invoke();
    // return false;

    mFactory->StartYourEngines(FALSE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerOnKeyMultipleCallable(this, keyCode, repeatCount, event);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IBoolean* interfaceTmp1 = IBoolean::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    UnimplementedWebViewApi::Invoke();
    *result = FALSE;
    return NOERROR;
}

ECode WebViewChromium::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     boolean ret = runOnUiThreadBlocking(new Callable<Boolean>() {
    //         @Override
    //         public Boolean call() {
    //             return onKeyDown(keyCode, event);
    //         }
    //     });
    //     return ret;
    // }
    // UnimplementedWebViewApi.invoke();
    // return false;

    mFactory->StartYourEngines(FALSE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerOnKeyDownCallable(this, keyCode, event);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IBoolean* interfaceTmp1 = IBoolean::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    UnimplementedWebViewApi::Invoke();
    *result = FALSE;
    return NOERROR;
}

ECode WebViewChromium::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     boolean ret = runOnUiThreadBlocking(new Callable<Boolean>() {
    //         @Override
    //         public Boolean call() {
    //             return onKeyUp(keyCode, event);
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.onKeyUp(keyCode, event);

    mFactory->StartYourEngines(FALSE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerOnKeyUpCallable(this, keyCode, event);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IBoolean* interfaceTmp1 = IBoolean::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    *result = mAwContents->OnKeyUp(keyCode, event);
    return NOERROR;
}

ECode WebViewChromium::OnAttachedToWindow()
{
    // ==================before translated======================
    // // This API is our strongest signal from the View system that this
    // // WebView is going to be bound to a View hierarchy and so at this
    // // point we must bind Chromium's UI thread to the current thread.
    // mFactory.startYourEngines(false);
    // checkThread();
    // mAwContents.onAttachedToWindow();

    // This API is our strongest signal from the View system that this
    // WebView is going to be bound to a View hierarchy and so at this
    // point we must bind Chromium's UI thread to the current thread.
    mFactory->StartYourEngines(FALSE);
    CheckThread();
    mAwContents->OnAttachedToWindow();
    return NOERROR;
}

ECode WebViewChromium::OnDetachedFromWindow()
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             onDetachedFromWindow();
    //         }
    //     });
    //     return;
    // }
    //
    // mAwContents.onDetachedFromWindow();

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerOnDetachedFromWindowRunnable(this);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->OnDetachedFromWindow();
    return NOERROR;
}

ECode WebViewChromium::OnVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 visibility)
{
    VALIDATE_NOT_NULL(changedView);
    // ==================before translated======================
    // // The AwContents will find out the container view visibility before the first draw so we
    // // can safely ignore onVisibilityChanged callbacks that happen before init().
    // if (mAwContents == null) return;
    //
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             onVisibilityChanged(changedView, visibility);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.onVisibilityChanged(changedView, visibility);

    // The AwContents will find out the container view visibility before the first draw so we
    // can safely ignore onVisibilityChanged callbacks that happen before init().
    if (NULL == mAwContents)
        return NOERROR;

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerOnVisibilityChangedRunnable(this, changedView, visibility);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->OnVisibilityChanged(changedView, visibility);
    return NOERROR;
}

ECode WebViewChromium::OnWindowFocusChanged(
    /* [in] */ Boolean hasWindowFocus)
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             onWindowFocusChanged(hasWindowFocus);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.onWindowFocusChanged(hasWindowFocus);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerOnWindowFocusChangedRunnable(this, hasWindowFocus);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->OnWindowFocusChanged(hasWindowFocus);
    return NOERROR;
}

ECode WebViewChromium::OnFocusChanged(
    /* [in] */ Boolean focused,
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    VALIDATE_NOT_NULL(previouslyFocusedRect);
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             onFocusChanged(focused, direction, previouslyFocusedRect);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.onFocusChanged(focused, direction, previouslyFocusedRect);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerOnFocusChangedRunnable(this, focused, direction, previouslyFocusedRect);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->OnFocusChanged(focused, direction, previouslyFocusedRect);
    return NOERROR;
}

ECode WebViewChromium::SetFrame(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWebViewPrivate.super_setFrame(left, top, right, bottom);

    return mWebViewPrivate->Super_setFrame(left, top, right, bottom, result);
}

ECode WebViewChromium::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 ow,
    /* [in] */ Int32 oh)
{
    // ==================before translated======================
    // if (checkNeedsPost()) {
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             onSizeChanged(w, h, ow, oh);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.onSizeChanged(w, h, ow, oh);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerOnSizeChangedRunnable(this, w, h, ow, oh);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->OnSizeChanged(w, h, ow, oh);
    return NOERROR;
}

ECode WebViewChromium::OnScrollChanged(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 oldl,
    /* [in] */ Int32 oldt)
{
    return NOERROR;
}

ECode WebViewChromium::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     boolean ret = runOnUiThreadBlocking(new Callable<Boolean>() {
    //         @Override
    //         public Boolean call() {
    //             return dispatchKeyEvent(event);
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.dispatchKeyEvent(event);

    mFactory->StartYourEngines(FALSE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerDispatchKeyEventCallable(this, event);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IBoolean* interfaceTmp1 = IBoolean::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    *result = mAwContents->DispatchKeyEvent(event);
    return NOERROR;
}

ECode WebViewChromium::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     boolean ret = runOnUiThreadBlocking(new Callable<Boolean>() {
    //         @Override
    //         public Boolean call() {
    //             return onTouchEvent(ev);
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.onTouchEvent(ev);

    mFactory->StartYourEngines(FALSE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerOnTouchEventCallable(this, event);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IBoolean* interfaceTmp1 = IBoolean::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    *result = mAwContents->OnTouchEvent(event);
    return NOERROR;
}

ECode WebViewChromium::OnHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     boolean ret = runOnUiThreadBlocking(new Callable<Boolean>() {
    //         @Override
    //         public Boolean call() {
    //             return onHoverEvent(event);
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.onHoverEvent(event);

    mFactory->StartYourEngines(FALSE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerOnHoverEventCallable(this, event);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IBoolean* interfaceTmp1 = IBoolean::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    *result = mAwContents->OnHoverEvent(event);
    return NOERROR;
}

ECode WebViewChromium::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     boolean ret = runOnUiThreadBlocking(new Callable<Boolean>() {
    //         @Override
    //         public Boolean call() {
    //             return onGenericMotionEvent(event);
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.onGenericMotionEvent(event);

    mFactory->StartYourEngines(FALSE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerOnGenericMotionEventCallable(this, event);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IBoolean* interfaceTmp1 = IBoolean::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    *result = mAwContents->OnGenericMotionEvent(event);
    return NOERROR;
}

ECode WebViewChromium::OnTrackballEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // Trackball event not handled, which eventually gets converted to DPAD keyevents
    // return false;

    *result = FALSE;
    return NOERROR;
}

ECode WebViewChromium::RequestFocus(
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(previouslyFocusedRect);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     boolean ret = runOnUiThreadBlocking(new Callable<Boolean>() {
    //         @Override
    //         public Boolean call() {
    //             return requestFocus(direction, previouslyFocusedRect);
    //         }
    //     });
    //     return ret;
    // }
    // mAwContents.requestFocus();
    // return mWebViewPrivate.super_requestFocus(direction, previouslyFocusedRect);

    mFactory->StartYourEngines(FALSE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerRequestFocusCallable(this, direction, previouslyFocusedRect);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IBoolean* interfaceTmp1 = IBoolean::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    mAwContents->RequestFocus();
    return mWebViewPrivate->Super_requestFocus(direction, previouslyFocusedRect, result);
}

void WebViewChromium::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     runVoidTaskOnUiThreadBlocking(new Runnable() {
    //         @Override
    //         public void run() {
    //             onMeasure(widthMeasureSpec, heightMeasureSpec);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.onMeasure(widthMeasureSpec, heightMeasureSpec);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerOnMeasureRunnable(this, widthMeasureSpec, heightMeasureSpec);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->OnMeasure(widthMeasureSpec, heightMeasureSpec);
}

ECode WebViewChromium::RequestChildRectangleOnScreen(
    /* [in] */ IView* child,
    /* [in] */ IRect* rect,
    /* [in] */ Boolean immediate,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(child);
    VALIDATE_NOT_NULL(rect);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     boolean ret = runOnUiThreadBlocking(new Callable<Boolean>() {
    //         @Override
    //         public Boolean call() {
    //             return requestChildRectangleOnScreen(child, rect, immediate);
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.requestChildRectangleOnScreen(child, rect, immediate);

    mFactory->StartYourEngines(FALSE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerRequestChildRectangleOnScreenCallable(this, child, rect, immediate);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IBoolean* interfaceTmp1 = IBoolean::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    *result = mAwContents->RequestChildRectangleOnScreen(child, rect, immediate);
    return NOERROR;
}

ECode WebViewChromium::SetBackgroundColor(
    /* [in] */ Int32 color)
{
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     ThreadUtils.postOnUiThread(new Runnable() {
    //         @Override
    //         public void run() {
    //             setBackgroundColor(color);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.setBackgroundColor(color);

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerSetBackgroundColorRunnable(this, color);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->SetBackgroundColor(color);
    return NOERROR;
}

ECode WebViewChromium::SetLayerType(
    /* [in] */ Int32 layerType,
    /* [in] */ IPaint* paint)
{
    VALIDATE_NOT_NULL(paint);
    // ==================before translated======================
    // // This can be called from WebView constructor in which case mAwContents
    // // is still null. We set the layer type in initForReal in that case.
    // if (mAwContents == null) return;
    // if (checkNeedsPost()) {
    //     ThreadUtils.postOnUiThread(new Runnable() {
    //         @Override
    //         public void run() {
    //             setLayerType(layerType, paint);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.setLayerType(layerType, paint);

    // This can be called from WebView constructor in which case mAwContents
    // is still null. We set the layer type in initForReal in that case.
    if (NULL == mAwContents)
        return NOERROR;

    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerSetLayerTypeRunnable(this, layerType, paint);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->SetLayerType(layerType, paint);
    return NOERROR;
}

ECode WebViewChromium::PreDispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    VALIDATE_NOT_NULL(canvas);
    // ==================before translated======================
    // // TODO(leandrogracia): remove this method from WebViewProvider if we think
    // // we won't need it again.

    return NOERROR;
}

ECode WebViewChromium::OnStartTemporaryDetach()
{
    // ==================before translated======================
    // mAwContents.onStartTemporaryDetach();

    mAwContents->OnStartTemporaryDetach();
    return NOERROR;
}

ECode WebViewChromium::OnFinishTemporaryDetach()
{
    // ==================before translated======================
    // mAwContents.onFinishTemporaryDetach();

    mAwContents->OnFinishTemporaryDetach();
    return NOERROR;
}

ECode WebViewChromium::ComputeHorizontalScrollRange(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     int ret = runOnUiThreadBlocking(new Callable<Integer>() {
    //         @Override
    //         public Integer call() {
    //             return computeHorizontalScrollRange();
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.computeHorizontalScrollRange();

    mFactory->StartYourEngines(FALSE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerComputeHorizontalScrollRangeCallable(this);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IInteger32* interfaceTmp1 = IInteger32::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    *result = mAwContents->ComputeHorizontalScrollRange();
    return NOERROR;
}

ECode WebViewChromium::ComputeHorizontalScrollOffset(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     int ret = runOnUiThreadBlocking(new Callable<Integer>() {
    //         @Override
    //         public Integer call() {
    //             return computeHorizontalScrollOffset();
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.computeHorizontalScrollOffset();

    mFactory->StartYourEngines(FALSE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerComputeHorizontalScrollOffsetCallable(this);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IInteger32* interfaceTmp1 = IInteger32::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    *result = mAwContents->ComputeHorizontalScrollOffset();
    return NOERROR;
}

ECode WebViewChromium::ComputeVerticalScrollRange(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     int ret = runOnUiThreadBlocking(new Callable<Integer>() {
    //         @Override
    //         public Integer call() {
    //             return computeVerticalScrollRange();
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.computeVerticalScrollRange();

    mFactory->StartYourEngines(FALSE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerComputeVerticalScrollRangeCallable(this);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IInteger32* interfaceTmp1 = IInteger32::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    *result = mAwContents->ComputeVerticalScrollRange();
    return NOERROR;
}

ECode WebViewChromium::ComputeVerticalScrollOffset(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     int ret = runOnUiThreadBlocking(new Callable<Integer>() {
    //         @Override
    //         public Integer call() {
    //             return computeVerticalScrollOffset();
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.computeVerticalScrollOffset();

    mFactory->StartYourEngines(FALSE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerComputeVerticalScrollOffsetCallable(this);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IInteger32* interfaceTmp1 = IInteger32::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    *result = mAwContents->ComputeVerticalScrollOffset();
    return NOERROR;
}

ECode WebViewChromium::ComputeVerticalScrollExtent(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     int ret = runOnUiThreadBlocking(new Callable<Integer>() {
    //         @Override
    //         public Integer call() {
    //             return computeVerticalScrollExtent();
    //         }
    //     });
    //     return ret;
    // }
    // return mAwContents.computeVerticalScrollExtent();

    mFactory->StartYourEngines(FALSE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<ICallable> callable = new InnerComputeVerticalScrollExtentCallable(this);
        AutoPtr<IInterface> interfaceTmp = RunOnUiThreadBlocking(callable);
        IInteger32* interfaceTmp1 = IInteger32::Probe(interfaceTmp);
        interfaceTmp1->GetValue(result);
        return NOERROR;
    }

    *result = mAwContents->ComputeVerticalScrollExtent();
    return NOERROR;
}

ECode WebViewChromium::ComputeScroll()
{
    // ==================before translated======================
    // mFactory.startYourEngines(false);
    // if (checkNeedsPost()) {
    //     runVoidTaskOnUiThreadBlocking(new Runnable() {
    //         @Override
    //         public void run() {
    //             computeScroll();
    //         }
    //     });
    //     return;
    // }
    // mAwContents.computeScroll();

    mFactory->StartYourEngines(FALSE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        AutoPtr<IRunnable> runnable = new InnerComputeScrollRunnable(this);
        mRunQueue->AddTask(runnable);
    }

    mAwContents->ComputeScroll();
    return NOERROR;
}

AutoPtr<IInterface/*IPrintDocumentAdapter*/> WebViewChromium::CreatePrintDocumentAdapter()
{
    // ==================before translated======================
    // return createPrintDocumentAdapter("default");

    AutoPtr<IInterface> result = CreatePrintDocumentAdapter(String("default"));  /*IPrintDocumentAdapter*/
    return result;
}

AutoPtr<IInterface/*IPrintDocumentAdapter*/> WebViewChromium::CreatePrintDocumentAdapter(
    /* [in] */ const String& documentName)
{
    // ==================before translated======================
    // checkThread();
    // return new AwPrintDocumentAdapter(mAwContents.getPdfExporter(), documentName);

    CheckThread();
    AutoPtr<IInterface> result;// = new AwPrintDocumentAdapter(mAwContents->GetPdfExporter(), documentName);  /*IPrintDocumentAdapter*/
    return result;
}

ECode WebViewChromium::ExtractSmartClipData(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    // ==================before translated======================
    // checkThread();
    // mAwContents.extractSmartClipData(x, y, width, height);

    CheckThread();
    return mAwContents->ExtractSmartClipData(x, y, width, height);
}

ECode WebViewChromium::SetSmartClipResultHandler(
    /* [in] */ IHandler* resultHandler)
{
    VALIDATE_NOT_NULL(resultHandler);
    // ==================before translated======================
    // checkThread();
    // mAwContents.setSmartClipResultHandler(resultHandler);

    CheckThread();
    IHandler* handler = const_cast<IHandler*>(resultHandler);
    mAwContents->SetSmartClipResultHandler(handler);
    return NOERROR;
}

ECode WebViewChromium::RunBlockingFuture(
    /* [in] */ FutureTask* task,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(task);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (!mFactory.hasStarted()) throw new RuntimeException("Must be started before we block!");
    // if (ThreadUtils.runningOnUiThread()) {
    //     throw new IllegalStateException("This method should only be called off the UI thread");
    // }
    // mRunQueue.addTask(task);
    // try {
    //     return task.get(4, TimeUnit.SECONDS);
    // } catch (java.util.concurrent.TimeoutException e) {
    //     throw new RuntimeException("Probable deadlock detected due to WebView API being called "
    //             + "on incorrect thread while the UI thread is blocked.", e);
    // } catch (Exception e) {
    //     throw new RuntimeException(e);
    // }

    Boolean hasStarted = FALSE;
    mFactory->HasStarted(&hasStarted);
    if (!hasStarted) {
        //throw new RuntimeException("Must be started before we block!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (ThreadUtils::RunningOnUiThread()) {
        //throw new IllegalStateException("This method should only be called off the UI thread");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mRunQueue->AddTask(task);
    //try {
        AutoPtr<ITimeUnitHelper> helper;
        CTimeUnitHelper::AcquireSingleton((ITimeUnitHelper**)&helper);
        AutoPtr<ITimeUnit> timeUnit;
        helper->GetSECONDS((ITimeUnit**)&timeUnit);

        task->Get(4, timeUnit, result);
        return NOERROR;
    //}
    //catch (java.util.concurrent.TimeoutException e) {
    //    throw new RuntimeException("Probable deadlock detected due to WebView API being called "
    //            + "on incorrect thread while the UI thread is blocked.", e);
    //}
    //catch (Exception e) {
    //    throw new RuntimeException(e);
    //}
}

ECode WebViewChromium::RunVoidTaskOnUiThreadBlocking(
    /* [in] */ IRunnable* r)
{
    VALIDATE_NOT_NULL(r);
    // ==================before translated======================
    // FutureTask<Void> task = new FutureTask<Void>(r, null);
    // runBlockingFuture(task);

    AutoPtr<FutureTask> task = new FutureTask(r, NULL);
    AutoPtr<IInterface> interfaceTmp;
    RunBlockingFuture(task, (IInterface**)&interfaceTmp);
    return NOERROR;
}

AutoPtr<IInterface> WebViewChromium::RunOnUiThreadBlocking(
    /* [in] */ ICallable* c)
{
    // ==================before translated======================
    // return runBlockingFuture(new FutureTask<T>(c));

    AutoPtr<FutureTask> task = new FutureTask(c);
    AutoPtr<IInterface> interfaceTmp;
    RunBlockingFuture(task, (IInterface**)&interfaceTmp);
    return interfaceTmp;
}

AutoPtr<IContext> WebViewChromium::ResourcesContextWrapper(
    /* [in] */ IContext* ctx)
{
    // ==================before translated======================
    // return new ContextWrapper(ctx) {
    //     @Override
    //     public ClassLoader getClassLoader() {
    //         final ClassLoader appCl = getBaseContext().getClassLoader();
    //         final ClassLoader webViewCl = this.getClass().getClassLoader();
    //         return new ClassLoader() {
    //             @Override
    //             protected Class<?> findClass(String name) throws ClassNotFoundException {
    //                 // First look in the WebViewProvider class loader.
    //                 try {
    //                     return webViewCl.loadClass(name);
    //                 } catch (ClassNotFoundException e) {
    //                     // Look in the app class loader; allowing it to throw ClassNotFoundException.
    //                     return appCl.loadClass(name);
    //                 }
    //             }
    //         };
    //     }
    //
    //     @Override
    //     public Object getSystemService(String name) {
    //         if (name.equals(Context.LAYOUT_INFLATER_SERVICE)) {
    //             LayoutInflater i = (LayoutInflater) getBaseContext().getSystemService(name);
    //             return i.cloneInContext(this);
    //         } else {
    //             return getBaseContext().getSystemService(name);
    //         }
    //     }
    //
    // };

    AutoPtr<InnerContextWrapper> contextWrapper = new InnerContextWrapper();
    AutoPtr<IContextWrapper> resTmp = (IContextWrapper*)contextWrapper.Get();
    IContext* result = IContext::Probe(resTmp);
    return result;
}

ECode WebViewChromium::InitForReal()
{
    // ==================before translated======================
    // Context ctx = resourcesContextWrapper(mWebView.getContext());
    // mAwContents = new AwContents(mFactory.getBrowserContext(), mWebView, ctx,
    //         new InternalAccessAdapter(), new WebViewNativeGLDelegate(),
    //         mContentsClientAdapter, mWebSettings.getAwSettings());
    //
    // if (mAppTargetSdkVersion >= Build.VERSION_CODES.KITKAT) {
    //     // On KK and above, favicons are automatically downloaded as the method
    //     // old apps use to enable that behavior is deprecated.
    //     AwContents.setShouldDownloadFavicons();
    // }
    //
    // AwContentsStatics.setRecordFullDocument(sRecordWholeDocumentEnabledByApi ||
    //         mAppTargetSdkVersion < Build.VERSION_CODES.L);
    //
    // if (mAppTargetSdkVersion <= Build.VERSION_CODES.KITKAT) {
    //     // On KK and older versions, JavaScript objects injected via addJavascriptInterface
    //     // were not inspectable.
    //     mAwContents.disableJavascriptInterfacesInspection();
    // }
    //
    // // TODO: This assumes AwContents ignores second Paint param.
    // mAwContents.setLayerType(mWebView.getLayerType(), null);

    assert(0);
    IView* viewTmp = IView::Probe(mWebView);
    AutoPtr<IContext> context;
    viewTmp->GetContext((IContext**)&context);
    AutoPtr<IContext> ctx = ResourcesContextWrapper(context);

    AutoPtr<IInterface> interfaceTmp;
    mFactory->GetBrowserContext((IInterface**)&interfaceTmp);
    //--AwContents cannot new, note before list for warning no used.
    //IObject* objectTmp = IObject::Probe(interfaceTmp);
    //Object* objectTmp1 = (Object*)objectTmp;
    //AwBrowserContext* browserContext = (AwBrowserContext*)objectTmp1;
    //AutoPtr<AwContents::InternalAccessDelegate> accessDelegate = new InternalAccessAdapter(this);
    //AutoPtr<AwContents::NativeGLDelegate> nativeDelegate = new WebViewNativeGLDelegate(this);
    //AwContentsClient* client = (AwContentsClient*)mContentsClientAdapter.Get();
    //IViewGroup* viewGroup = IViewGroup::Probe(mWebView);
    //AutoPtr<AwSettings> awSettings;
    //mWebSettings->GetAwSettings((AwSettings**)&awSettings);
    //mAwContents = new AwContents(browserContext, viewGroup, ctx, accessDelegate, nativeDelegate, client, awSettings);

    if (mAppTargetSdkVersion >= Build::VERSION_CODES::KITKAT) {
        // On KK and above, favicons are automatically downloaded as the method
        // old apps use to enable that behavior is deprecated.
        AwContents::SetShouldDownloadFavicons();
    }

    AwContentsStatics::SetRecordFullDocument(sRecordWholeDocumentEnabledByApi || mAppTargetSdkVersion < Build::VERSION_CODES::L);
    if (mAppTargetSdkVersion <= Build::VERSION_CODES::KITKAT) {
        // On KK and older versions, JavaScript objects injected via addJavascriptInterface
        // were not inspectable.
        mAwContents->DisableJavascriptInterfacesInspection();
    }

    // TODO: This assumes AwContents ignores second Paint param.
    Int32 layerType = 0;
    viewTmp->GetLayerType(&layerType);
    mAwContents->SetLayerType(layerType, NULL);
    return NOERROR;
}

//AutoPtr<RuntimeException> WebViewChromium::CreateThreadException()
//{
//    // ==================before translated======================
//    // return new IllegalStateException(
//    //         "Calling View methods on another thread than the UI thread.");
//    assert(0);
//    AutoPtr<RuntimeException> empty;
//    return empty;
//}

ECode WebViewChromium::CheckNeedsPost(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean needsPost = !mFactory.hasStarted() || !ThreadUtils.runningOnUiThread();
    // if (!needsPost && mAwContents == null) {
    //     throw new IllegalStateException(
    //             "AwContents must be created if we are not posting!");
    // }
    // return needsPost;

    Boolean hasStarted = FALSE;
    mFactory->HasStarted(&hasStarted);
    Boolean needsPost = !hasStarted || !ThreadUtils::RunningOnUiThread();
    if (!needsPost && NULL == mAwContents) {
        //throw new IllegalStateException("AwContents must be created if we are not posting!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = needsPost;
    return NOERROR;
}

ECode WebViewChromium::CheckThread()
{
    // ==================before translated======================
    // if (!ThreadUtils.runningOnUiThread()) {
    //     final RuntimeException threadViolation = createThreadException();
    //     ThreadUtils.postOnUiThread(new Runnable() {
    //         @Override
    //         public void run() {
    //             throw threadViolation;
    //         }
    //     });
    //     throw createThreadException();
    // }

    assert(0);
    if (!ThreadUtils::RunningOnUiThread()) {
        //final RuntimeException threadViolation = createThreadException();
        AutoPtr<IRunnable> runnable = new InnerThreadViolationRunnable(this);
        ThreadUtils::PostOnUiThread(runnable);
        //throw createThreadException();
        assert(0);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

String WebViewChromium::FixupMimeType(
    /* [in] */ const String& mimeType)
{
    // ==================before translated======================
    // return TextUtils.isEmpty(mimeType) ? "text/html" : mimeType;

    return TextUtils::IsEmpty(mimeType) ? String("text/html") : mimeType;
}

String WebViewChromium::FixupData(
    /* [in] */ const String& data)
{
    // ==================before translated======================
    // return TextUtils.isEmpty(data) ? "" : data;

    return TextUtils::IsEmpty(data) ? String("") : data;
}

String WebViewChromium::FixupBase(
    /* [in] */ const String& url)
{
    // ==================before translated======================
    // return TextUtils.isEmpty(url) ? "about:blank" : url;

    return TextUtils::IsEmpty(url) ? String("about:blank") : url;
}

String WebViewChromium::FixupHistory(
    /* [in] */ const String& url)
{
    // ==================before translated======================
    // return TextUtils.isEmpty(url) ? "about:blank" : url;

    return TextUtils::IsEmpty(url) ? String("about:blank") : url;
}

Boolean WebViewChromium::IsBase64Encoded(
    /* [in] */ const String& encoding)
{
    // ==================before translated======================
    // return "base64".equals(encoding);

    return String("base64").Equals(encoding);
}

ECode WebViewChromium::LoadUrlOnUiThread(
    /* [in] */ LoadUrlParams* loadUrlParams)
{
    VALIDATE_NOT_NULL(loadUrlParams);
    // ==================before translated======================
    // // This is the last point that we can delay starting the Chromium backend up
    // // and if the app has not caused us to bind the Chromium UI thread to a background thread
    // // we now bind Chromium's notion of the UI thread to the app main thread.
    // mFactory.startYourEngines(true);
    // if (checkNeedsPost()) {
    //     // Disallowed in WebView API for apps targetting a new SDK
    //     assert mAppTargetSdkVersion < Build.VERSION_CODES.JELLY_BEAN_MR2;
    //     mRunQueue.addTask(new Runnable() {
    //         @Override
    //         public void run() {
    //             mAwContents.loadUrl(loadUrlParams);
    //         }
    //     });
    //     return;
    // }
    // mAwContents.loadUrl(loadUrlParams);

    mFactory->StartYourEngines(TRUE);
    Boolean resTmp = FALSE;
    CheckNeedsPost(&resTmp);
    if (resTmp) {
        assert(mAppTargetSdkVersion < Build::VERSION_CODES::JELLY_BEAN_MR2);
        AutoPtr<IRunnable> runnable = new InnerLoadUrlRunnable(this, loadUrlParams);
        ThreadUtils::RunOnUiThread(runnable);
    }

    mAwContents->LoadUrl(loadUrlParams);
    return NOERROR;
}

Boolean WebViewChromium::DoesSupportFullscreen(
    /* [in] */ IWebChromeClient* client)
{
    // ==================before translated======================
    // if (client == null) {
    //     return false;
    // }
    // // If client is not a subclass of WebChromeClient then the methods have not been
    // // implemented because WebChromeClient has empty implementations.
    // if (client.getClass().isAssignableFrom(WebChromeClient.class)) {
    //     return false;
    // }
    // try {
    //     client.getClass().getDeclaredMethod("onShowCustomView", View.class,
    //             CustomViewCallback.class);
    //     client.getClass().getDeclaredMethod("onHideCustomView");
    //     return true;
    // } catch (NoSuchMethodException e) {
    //     return false;
    // }

    assert(0);
    if (NULL == client) {
        return FALSE;
    }

    // If client is not a subclass of WebChromeClient then the methods have not been
    // implemented because WebChromeClient has empty implementations.
    if (0/*client->GetClass()->IsAssignableFrom(WebChromeClient.class)*/) {
        return FALSE;
    }
    //try {
        //client->GetClass()->GetDeclaredMethod(String("onShowCustomView"), View.class, CustomViewCallback.class);
        //client->GetClass()->GetDeclaredMethod(String("onHideCustomView"));
        return TRUE;
    //}
    //catch (NoSuchMethodException e) {
    //    return FALSE;
    //}
}


} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


