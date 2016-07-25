
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContents.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/api/AwContents_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/content_public/Referrer.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/base/ActivityWindowElastos.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/base/WindowElastos.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/gfx/DeviceDisplayInfo.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/ErrorCodeConversionHelper.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContentsClientCallbackHelper.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContentsStatics.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/NullAwViewMethods.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwPicture.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/FullScreenView.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/SslUtil.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::EIID_IActivity;
using Elastos::Droid::Content::EIID_IComponentCallbacks2;
using Elastos::Droid::View::EIID_IViewOnLayoutChangeListener;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Webkit::IGeolocationPermissions;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;
using Elastos::Droid::Webkit::Webview::Chromium::Content_Public::Referrer;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::Base::ActivityWindowElastos;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::Base::WindowElastos;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::Gfx::DeviceDisplayInfo;
using Elastos::Droid::Webkit::EIID_IGeolocationPermissionsCallback;
using Elastos::Droid::Widget::COverScroller;
using Elastos::Droid::Net::Uri;


using Elastos::Net::IURL;
using Elastos::Net::CURL;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::IHashMap;
using Elastos::Utility::CHashMap;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Concurrent::EIID_ICallable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

//===============================================================
//               AwContents::DependencyFactory
//===============================================================

AutoPtr<AwLayoutSizer> AwContents::DependencyFactory::CreateLayoutSizer()
{
    AutoPtr<AwLayoutSizer> als = new AwLayoutSizer();
    return als;
}

AutoPtr<AwScrollOffsetManager> AwContents::DependencyFactory::CreateScrollOffsetManager(
    /* [in] */ AwScrollOffsetManager::Delegate* delegate,
    /* [in] */ IOverScroller* overScroller)
{
    AutoPtr<AwScrollOffsetManager> asom = new AwScrollOffsetManager(delegate, overScroller);
    return asom;
}

//===============================================================
//                AwContents::DestroyRunnable
//===============================================================
CAR_INTERFACE_IMPL(AwContents::DestroyRunnable, Object, IRunnable);

AwContents::DestroyRunnable::DestroyRunnable(
    /* [in] */ Handle64 nativeAwContents)
    : mNativeAwContents(nativeAwContents)
{
}

//@Override
ECode AwContents::DestroyRunnable::Run()
{
    NativeDestroy(mNativeAwContents);
    return NOERROR;
}

//===============================================================
//           AwContents::FullScreenTransitionsState
//===============================================================

AwContents::FullScreenTransitionsState::FullScreenTransitionsState(
    /* [in] */ IViewGroup* initialContainerView,
    /* [in] */ InternalAccessDelegate* initialInternalAccessAdapter,
    /* [in] */ AwViewMethods* initialAwViewMethods)
    : mInitialContainerView(initialContainerView)
    , mInitialInternalAccessAdapter(initialInternalAccessAdapter)
    , mInitialAwViewMethods(initialAwViewMethods)
{
}

void AwContents::FullScreenTransitionsState::EnterFullScreen(
    /* [in] */ FullScreenView* fullScreenView)
{
    mFullScreenView = fullScreenView;
}

void AwContents::FullScreenTransitionsState::ExitFullScreen()
{
    mFullScreenView = NULL;
}

Boolean AwContents::FullScreenTransitionsState::IsFullScreen()
{
    return mFullScreenView != NULL;
}

AutoPtr<IViewGroup> AwContents::FullScreenTransitionsState::GetInitialContainerView()
{
    return mInitialContainerView;
}

AutoPtr<AwContents::InternalAccessDelegate> AwContents::FullScreenTransitionsState::GetInitialInternalAccessDelegate()
{
    return mInitialInternalAccessAdapter;
}

AutoPtr<AwViewMethods> AwContents::FullScreenTransitionsState::GetInitialAwViewMethods()
{
    return mInitialAwViewMethods;
}

AutoPtr<FullScreenView> AwContents::FullScreenTransitionsState::GetFullScreenView()
{
    return mFullScreenView;
}

//===============================================================
//             AwContents::IoThreadClientImpl
//===============================================================

AwContents::IoThreadClientImpl::IoThreadClientImpl(
    /* [in] */ AwContents* owner)
    : mOwner(owner)
{
}

//@Override
Int32 AwContents::IoThreadClientImpl::GetCacheMode()
{
    return mOwner->mSettings->GetCacheMode();
}

//@Override
AutoPtr<AwWebResourceResponse> AwContents::IoThreadClientImpl::ShouldInterceptRequest(
    /* [in] */ AwContentsClient::ShouldInterceptRequestParams* params)
{
    String url = params->url;
    AutoPtr<AwWebResourceResponse> awWebResourceResponse;
    // Return the response directly if the url is default video poster url.
    awWebResourceResponse = mOwner->mDefaultVideoPosterRequestHandler->ShouldInterceptRequest(url);
    if (awWebResourceResponse != NULL) return awWebResourceResponse;

    awWebResourceResponse = mOwner->mContentsClient->ShouldInterceptRequest(params);

    if (awWebResourceResponse == NULL) {
        mOwner->mContentsClient->GetCallbackHelper()->PostOnLoadResource(url);
    }

    if (params->isMainFrame && awWebResourceResponse != NULL &&
            awWebResourceResponse->GetData() == NULL) {
        // In this case the intercepted URLRequest job will simulate an empty response
        // which doesn't trigger the onReceivedError callback. For WebViewClassic
        // compatibility we synthesize that callback. http://crbug.com/180950
        mOwner->mContentsClient->GetCallbackHelper()->PostOnReceivedError(
                ErrorCodeConversionHelper::ERROR_UNKNOWN,
                String(NULL) /* filled in by the glue layer */, url);
    }
    return awWebResourceResponse;
}

//@Override
Boolean AwContents::IoThreadClientImpl::ShouldBlockContentUrls()
{
    return (!mOwner->mSettings->GetAllowContentAccess());
}

//@Override
Boolean AwContents::IoThreadClientImpl::ShouldBlockFileUrls()
{
    return (!mOwner->mSettings->GetAllowFileAccess());
}

//@Override
Boolean AwContents::IoThreadClientImpl::ShouldBlockNetworkLoads()
{
    return mOwner->mSettings->GetBlockNetworkLoads();
}

//@Override
Boolean AwContents::IoThreadClientImpl::ShouldAcceptThirdPartyCookies()
{
    return mOwner->mSettings->GetAcceptThirdPartyCookies();
}

//@Override
void AwContents::IoThreadClientImpl::OnDownloadStart(
    /* [in] */ const String& url,
    /* [in] */ const String& userAgent,
    /* [in] */ const String& contentDisposition,
    /* [in] */ const String& mimeType,
    /* [in] */ Int64 contentLength)
{
    mOwner->mContentsClient->GetCallbackHelper()->PostOnDownloadStart(url, userAgent,
            contentDisposition, mimeType, contentLength);
}

//@Override
void AwContents::IoThreadClientImpl::NewLoginRequest(
    /* [in] */ const String& realm,
    /* [in] */ const String& account,
    /* [in] */ const String& args)
{
    mOwner->mContentsClient->GetCallbackHelper()->PostOnReceivedLoginRequest(realm, account, args);
}

//===============================================================
//       AwContents::InterceptNavigationDelegateImpl
//===============================================================

AwContents::InterceptNavigationDelegateImpl::InterceptNavigationDelegateImpl(
    /* [in] */ AwContents* owner)
    : mOwner(owner)
{
}

//@Override
Boolean AwContents::InterceptNavigationDelegateImpl::ShouldIgnoreNavigation(
    /* [in] */ NavigationParams* navigationParams)
{
    const String url = navigationParams->url;
    Boolean ignoreNavigation = FALSE;
    if (mOwner->mDeferredShouldOverrideUrlLoadingIsPendingForPopup) {
        mOwner->mDeferredShouldOverrideUrlLoadingIsPendingForPopup = FALSE;
        // If this is used for all navigations in future, cases for application initiated
        // load, redirect and backforward should also be filtered out.
        if (!navigationParams->isPost) {
            ignoreNavigation = mOwner->mContentsClient->ShouldOverrideUrlLoading(url);
        }
    }
    // The shouldOverrideUrlLoading call might have resulted in posting messages to the
    // UI thread. Using sendMessage here (instead of calling onPageStarted directly)
    // will allow those to run in order.
    if (!ignoreNavigation) {
        mOwner->mContentsClient->GetCallbackHelper()->PostOnPageStarted(url);
    }
    return ignoreNavigation;
}

//===============================================================
//             AwContents::AwLayoutSizerDelegate
//===============================================================

AwContents::AwLayoutSizerDelegate::AwLayoutSizerDelegate(
    /* [in] */ AwContents* owner)
    : mOwner(owner)
{
}

//@Override
void AwContents::AwLayoutSizerDelegate::RequestLayout()
{
    AutoPtr<IView> view = IView::Probe(mOwner->mContainerView);
    view->RequestLayout();
}

//@Override
void AwContents::AwLayoutSizerDelegate::SetMeasuredDimension(
    /* [in] */ Int32 measuredWidth,
    /* [in] */ Int32 measuredHeight)
{
    mOwner->mInternalAccessAdapter->SetMeasuredDimension(measuredWidth, measuredHeight);
}

//@Override
Boolean AwContents::AwLayoutSizerDelegate::IsLayoutParamsHeightWrapContent()
{
    AutoPtr<IViewGroupLayoutParams> layoutParams;
    AutoPtr<IView> view = IView::Probe(mOwner->mContainerView);
    view->GetLayoutParams((IViewGroupLayoutParams**)&layoutParams);
    if (layoutParams == NULL)
        return FALSE;
    Int32 height;
    layoutParams->GetHeight(&height);
    return height == IViewGroupLayoutParams::WRAP_CONTENT;
}

//@Override
void AwContents::AwLayoutSizerDelegate::SetForceZeroLayoutHeight(
    /* [in] */ Boolean forceZeroHeight)
{
    mOwner->GetSettings()->SetForceZeroLayoutHeight(forceZeroHeight);
}

//===============================================================
//          AwContents::AwScrollOffsetManagerDelegate
//===============================================================

AwContents::AwScrollOffsetManagerDelegate::AwScrollOffsetManagerDelegate(
    /* [in] */ AwContents* owner)
    : mOwner(owner)
{
}

//@Override
void AwContents::AwScrollOffsetManagerDelegate::OverScrollContainerViewBy(
    /* [in] */ Int32 deltaX,
    /* [in] */ Int32 deltaY,
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY,
    /* [in] */ Int32 scrollRangeX,
    /* [in] */ Int32 scrollRangeY,
    /* [in] */ Boolean isTouchEvent)
{
    mOwner->mInternalAccessAdapter->OverScrollBy(deltaX, deltaY, scrollX, scrollY,
            scrollRangeX, scrollRangeY, 0, 0, isTouchEvent);
}

//@Override
void AwContents::AwScrollOffsetManagerDelegate::ScrollContainerViewTo(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    mOwner->mInternalAccessAdapter->Super_scrollTo(x, y);
}

//@Override
void AwContents::AwScrollOffsetManagerDelegate::ScrollNativeTo(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    if (mOwner->mNativeAwContents == 0) return;
    mOwner->NativeScrollTo(mOwner->mNativeAwContents, x, y);
}

//@Override
Int32 AwContents::AwScrollOffsetManagerDelegate::GetContainerViewScrollX()
{
    AutoPtr<IView> view = IView::Probe(mOwner->mContainerView);
    //return mOwner->mContainerView->GetScrollX();
    Int32 x;
    view->GetScrollX(&x);
    return x;
}

//@Override
Int32 AwContents::AwScrollOffsetManagerDelegate::GetContainerViewScrollY()
{
    AutoPtr<IView> view = IView::Probe(mOwner->mContainerView);
    //return mOwner->mContainerView->GetScrollY();
    Int32 y;
    view->GetScrollY(&y);
    return y;
}

//@Override
void AwContents::AwScrollOffsetManagerDelegate::Invalidate()
{
    AutoPtr<IView> view = IView::Probe(mOwner->mContainerView);
    view->Invalidate();
}

//===============================================================
//             AwContents::AwGestureStateListener
//===============================================================

AwContents::AwGestureStateListener::AwGestureStateListener(
    /* [in] */ AwContents* owner)
    : mOwner(owner)
{
}

//@Override
void AwContents::AwGestureStateListener::OnPinchStarted()
{
    // While it's possible to re-layout the view during a pinch gesture, the effect is very
    // janky (especially that the page scale update notification comes from the renderer
    // main thread, not from the impl thread, so it's usually out of sync with what's on
    // screen). It's also quite expensive to do a re-layout, so we simply postpone
    // re-layout for the duration of the gesture. This is compatible with what
    // WebViewClassic does.
    mOwner->mLayoutSizer->FreezeLayoutRequests();
}

//@Override
void AwContents::AwGestureStateListener::OnPinchEnded()
{
    mOwner->mLayoutSizer->UnfreezeLayoutRequests();
}

//@Override
void AwContents::AwGestureStateListener::OnFlingCancelGesture()
{
    mOwner->mScrollOffsetManager->OnFlingCancelGesture();
}

//@Override
void AwContents::AwGestureStateListener::OnUnhandledFlingStartEvent(
    /* [in] */ Int32 velocityX,
    /* [in] */ Int32 velocityY)
{
    mOwner->mScrollOffsetManager->OnUnhandledFlingStartEvent(velocityX, velocityY);
}

//@Override
void AwContents::AwGestureStateListener::OnScrollUpdateGestureConsumed()
{
    mOwner->mScrollAccessibilityHelper->PostViewScrolledAccessibilityEventCallback();
}

//===============================================================
//              AwContents::AwComponentCallbacks
//===============================================================
CAR_INTERFACE_IMPL(AwContents::AwComponentCallbacks, Object, IComponentCallbacks2);

AwContents::AwComponentCallbacks::AwComponentCallbacks(
    /* [in] */ AwContents* owner)
    : mOwner(owner)
{
}

//@Override
ECode AwContents::AwComponentCallbacks::OnTrimMemory(
    /* [in] */ Int32 level)
{
    if (mOwner->mNativeAwContents == 0) return NOERROR;
    Boolean visibleRectEmpty;
    mOwner->GetGlobalVisibleRect()->IsEmpty(&visibleRectEmpty);
    const Boolean visible = mOwner->mIsViewVisible && mOwner->mIsWindowVisible && !visibleRectEmpty;
    mOwner->NativeTrimMemory(mOwner->mNativeAwContents, level, visible);
    return NOERROR;
}

//@Override
void AwContents::AwComponentCallbacks::OnLowMemory()
{
}

//@Override
void AwContents::AwComponentCallbacks::OnConfigurationChanged(
    /* [in] */ IConfiguration* configuration)
{
}

//===============================================================
//             AwContents::AwLayoutChangeListener
//===============================================================
CAR_INTERFACE_IMPL(AwContents::AwLayoutChangeListener, Object, IViewOnLayoutChangeListener);

AwContents::AwLayoutChangeListener::AwLayoutChangeListener(
    /* [in] */ AwContents* owner)
    : mOwner(owner)
{
}

//@Override
ECode AwContents::AwLayoutChangeListener::OnLayoutChange(
    /* [in] */ IView* v,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 oldLeft,
    /* [in] */ Int32 oldTop,
    /* [in] */ Int32 oldRight,
    /* [in] */ Int32 oldBottom)
{
    //assert(v == mContainerView);
    mOwner->mLayoutSizer->OnLayoutChange();
    return NOERROR;
}

//===============================================================
//         AwContents::InnerZoomSupportChangeListener
//===============================================================

AwContents::InnerZoomSupportChangeListener::InnerZoomSupportChangeListener(
    /* [in] */ AwContents* owner)
    :mOwner(owner)
{
}

//@Override
void AwContents::InnerZoomSupportChangeListener::OnGestureZoomSupportChanged(
    /* [in] */ Boolean supportsDoubleTapZoom,
    /* [in] */ Boolean supportsMultiTouchZoom)
{
    mOwner->mContentViewCore->UpdateDoubleTapSupport(supportsDoubleTapZoom);
    mOwner->mContentViewCore->UpdateMultiTouchZoomSupport(supportsMultiTouchZoom);
}

//===============================================================
//                   AwContents::InnerCallable
//===============================================================
CAR_INTERFACE_IMPL(AwContents::InnerCallable, Object, ICallable);

AwContents::InnerCallable::InnerCallable(
    /* [in] */ AwContents* owner)
    : mOwner(owner)
{
}

ECode AwContents::InnerCallable::Call(
    /* [out] */ IInterface** pic)//IPicture
{
    VALIDATE_NOT_NULL(pic);
    AutoPtr<IPicture> picture = mOwner->CapturePicture();
    *pic = TO_IINTERFACE(picture);
    REFCOUNT_ADD(*pic);
    return NOERROR;
}


//===============================================================
//                AwContents::InnerValueCallback
//===============================================================
CAR_INTERFACE_IMPL(AwContents::InnerValueCallback, Object, IValueCallback)

AwContents::InnerValueCallback::InnerValueCallback(
    /* [in] */ AwContents* owner)
    : mOwner(owner)
{
}

ECode AwContents::InnerValueCallback::OnReceiveValue(
    /* [in] */ IInterface* value)//ArrayOf<String>
{
    AutoPtr<ArrayOf<String> > arrayStr;
    if (value != NULL)
    {
        //TODO how to get the strings, what kind of form in value, ArrayList??
    }
    AutoPtr<IRunnable> runnable = new InnerRunnable(this, arrayStr);
    ThreadUtils::RunOnUiThread(runnable);
    return NOERROR;
}

//===============================================================
//        AwContents::InnerValueCallback::InnerRunnable
//===============================================================
CAR_INTERFACE_IMPL(AwContents::InnerValueCallback::InnerRunnable, Object, IRunnable);

AwContents::InnerValueCallback::InnerRunnable::InnerRunnable(
    /* [in] */ InnerValueCallback* owner,
    /* [in] */ ArrayOf<String>* value)
    : mOwner(owner)
    , mValue(value)
{
}

ECode AwContents::InnerValueCallback::InnerRunnable::Run()
{
    if (mOwner->mOwner->mNativeAwContents == 0) return NOERROR;
    mOwner->mOwner->NativeAddVisitedLinks(mOwner->mOwner->mNativeAwContents, mValue);
    return NOERROR;
}

//===============================================================
//            AwContents::InnerJavaScriptCallback
//===============================================================

AwContents::InnerJavaScriptCallback::InnerJavaScriptCallback(
    /* [in] */ AwContents* owner,
    /* [in] */ IValueCallback* callback)
    : mOwner(owner)
    , mCallback(callback)
{
}

void AwContents::InnerJavaScriptCallback::HandleJavaScriptResult(
    /* [in] */ const String& jsonResult)
{
    //TODO transfer jsonResult to a kind of object with IInterface support
    AutoPtr<ICharSequence> icsq;
    CString::New(jsonResult, (ICharSequence**)&icsq);
    AutoPtr<IInterface> result = TO_IINTERFACE(icsq);
    mCallback->OnReceiveValue(result);
}

//===============================================================
//             AwContents::AwGeolocationCallback
//===============================================================
CAR_INTERFACE_IMPL(AwContents::AwGeolocationCallback, Object, IGeolocationPermissionsCallback);

AwContents::AwGeolocationCallback::AwGeolocationCallback(
    /* [in] */ AwContents* owner)
    : mOwner(owner)
{}

//@Override
ECode AwContents::AwGeolocationCallback::Invoke(
    /* [in] */ const String& origin,
    /* [in] */ Boolean allow,
    /* [in] */ Boolean retain)
{
    AutoPtr<IRunnable> runnable = new InnerRunnable(this, origin, allow, retain);
    ThreadUtils::RunOnUiThread(runnable);
    return NOERROR;
}

//===============================================================
//       AwContents::AwGeolocationCallback::InnerRunnable
//===============================================================
CAR_INTERFACE_IMPL(AwContents::AwGeolocationCallback::InnerRunnable, Object, IRunnable);

AwContents::AwGeolocationCallback::InnerRunnable::InnerRunnable(
    /* [in] */ AwGeolocationCallback* owner,
    /* [in] */ const String& origin,
    /* [in] */ Boolean allow,
    /* [in] */ Boolean retain)
    : mOwner(owner)
    , mOrigin(origin)
    , mAllow(allow)
    , mRetain(retain)
{
}

ECode AwContents::AwGeolocationCallback::InnerRunnable::Run()
{
    if (mRetain) {
        if (mAllow) {
            mOwner->mOwner->mBrowserContext->GetGeolocationPermissions()->Allow(mOrigin);
        }
        else {
            mOwner->mOwner->mBrowserContext->GetGeolocationPermissions()->Deny(mOrigin);
        }
    }
    if (mOwner->mOwner->mNativeAwContents == 0) return NOERROR;;
    mOwner->mOwner->NativeInvokeGeolocationCallback(mOwner->mOwner->mNativeAwContents, mAllow, mOrigin);

    return NOERROR;
}

//===============================================================
//          AwContents::SaveWebArchiveInternalTask
//===============================================================
AwContents::SaveWebArchiveInternalTask::SaveWebArchiveInternalTask(
    /* [in] */ AwContents* owner,
    /* [in] */ const String& basename,
    /* [in] */ IValueCallback* callback)
    : mOwner(owner)
    , mBasename(basename)
    , mCallback(callback)
{
}

ECode AwContents::SaveWebArchiveInternalTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    String namePath = mOwner->GenerateArchiveAutoNamePath(mOwner->GetOriginalUrl(), mBasename);
    AutoPtr<ICharSequence> csq;
    CString::New(namePath, (ICharSequence**)&csq);
    *result = csq.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//@Override
ECode AwContents::SaveWebArchiveInternalTask::OnPostExecute(
    /* [in] */ IInterface* result)
{
    AutoPtr<ICharSequence> csq = ICharSequence::Probe(result);
    String res;
    csq->ToString(&res);
    mOwner->SaveWebArchiveInternal(res, mCallback);
    return NOERROR;
}

//===============================================================
//          AwContents::SaveWebArchiveInternalRunnable
//===============================================================
CAR_INTERFACE_IMPL(AwContents::SaveWebArchiveInternalRunnable, Object, IRunnable);

AwContents::SaveWebArchiveInternalRunnable::SaveWebArchiveInternalRunnable(
    /* [in] */ AwContents* owner,
    /* [in] */ IValueCallback* callback)
    : mOwner(owner)
    , mCallback(callback)
{
}

ECode AwContents::SaveWebArchiveInternalRunnable::Run()
{
    return mCallback->OnReceiveValue(NULL);
}

//===============================================================
//            AwContents::InnerSmartClipDataListener
//===============================================================

AwContents::InnerSmartClipDataListener::InnerSmartClipDataListener(
    /* [in] */ AwContents* owner,
    /* [in] */ IHandler* resultHandler)
    : mOwner(owner)
    , mResultHandler(resultHandler)
{
}

void AwContents::InnerSmartClipDataListener::OnSmartClipDataExtracted(
    /* [in] */ const String& text,
    /* [in] */ const String& html,
    /* [in] */ IRect* clipRect)
{
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    bundle->PutString(String("url"), mOwner->mContentViewCore->GetWebContents()->GetVisibleUrl());
    bundle->PutString(String("title"), mOwner->mContentViewCore->GetWebContents()->GetTitle());
    AutoPtr<IParcelable> rectParcelable = IParcelable::Probe(clipRect);
    bundle->PutParcelable(String("rect"), rectParcelable);
    bundle->PutString(String("text"), text);
    bundle->PutString(String("html"), html);
    //try {
        AutoPtr<IMessageHelper> msgHelper;
        CMessageHelper::AcquireSingleton((IMessageHelper**)&msgHelper);
        AutoPtr<IMessage> msg;
        msgHelper->Obtain(mResultHandler, 0, (IMessage**)&msg);
        msg->SetData(bundle);
        msg->SendToTarget();
    //} catch (Exception e) {
    //    Log.e(TAG, "Error calling handler for smart clip data: ", e);
    //}
}

//===============================================================
//               AwContents::AwViewMethodsImpl
//===============================================================

static AutoPtr<IRect> ClipBoundsTemporaryCreate()
{
    AutoPtr<IRect> rect;
    CRect::New((IRect**)&rect);
    return rect;
}

AwContents::AwViewMethodsImpl::AwViewMethodsImpl(
    /* [in] */ AwContents* owner)
    : mOwner(owner)
    , mClipBoundsTemporary(ClipBoundsTemporaryCreate())
    , mLayerType(IView::LAYER_TYPE_NONE)
{
}

//@Override
void AwContents::AwViewMethodsImpl::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    if (mOwner->mNativeAwContents == 0) {
        canvas->DrawColor(mOwner->GetEffectiveBackgroundColor());
        return;
    }

    // For hardware draws, the clip at onDraw time could be different
    // from the clip during DrawGL.
    Boolean bIsHardwareAccelerated = FALSE;
    Boolean bIsNonEmpty = FALSE;
    canvas->IsHardwareAccelerated(&bIsHardwareAccelerated);
    canvas->GetClipBounds(mClipBoundsTemporary, &bIsNonEmpty);
    if (!bIsHardwareAccelerated && !bIsNonEmpty) {
        return;
    }

    mOwner->mScrollOffsetManager->SyncScrollOffsetFromOnDraw();
    AutoPtr<IRect> globalVisibleRect = mOwner->GetGlobalVisibleRect();
    Int32 left, right, top, bottom;
    globalVisibleRect->GetLeft(&left);
    globalVisibleRect->GetRight(&right);
    globalVisibleRect->GetTop(&top);
    globalVisibleRect->GetBottom(&bottom);
    Int32 scrollX, scrollY;
    AutoPtr<IView> view = IView::Probe(mOwner->mContainerView);
    view->GetScrollX(&scrollX);
    view->GetScrollY(&scrollY);
    if (!mOwner->NativeOnDraw(mOwner->mNativeAwContents, canvas, bIsHardwareAccelerated,
            scrollX, scrollY, left, top, right, bottom)) {
        // Can happen during initialization when compositor is not set
        // up. Or when clearView
        // is in effect. Just draw background color instead.
        canvas->DrawColor(mOwner->GetEffectiveBackgroundColor());
    }

    if (mOwner->mOverScrollGlow != NULL && mOwner->mOverScrollGlow->DrawEdgeGlows(canvas,
            mOwner->mScrollOffsetManager->ComputeMaximumHorizontalScrollOffset(),
            mOwner->mScrollOffsetManager->ComputeMaximumVerticalScrollOffset())) {
        view->Invalidate();
    }
}

//@Override
ECode AwContents::AwViewMethodsImpl::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    mOwner->mLayoutSizer->OnMeasure(widthMeasureSpec, heightMeasureSpec);
}

//@Override
void AwContents::AwViewMethodsImpl::RequestFocus()
{
    if (mOwner->mNativeAwContents == 0) return;
    AutoPtr<IView> view = IView::Probe(mOwner->mContainerView);
    Boolean inTouchMode;
    view->IsInTouchMode(&inTouchMode);
    if (!inTouchMode && mOwner->mSettings->ShouldFocusFirstNode()) {
        mOwner->NativeFocusFirstNode(mOwner->mNativeAwContents);
    }
}

//@Override
void AwContents::AwViewMethodsImpl::SetLayerType(
    /* [in] */ Int32 layerType,
    /* [in] */ IPaint* paint)
{
    mLayerType = layerType;
    UpdateHardwareAcceleratedFeaturesToggle();
}

void AwContents::AwViewMethodsImpl::UpdateHardwareAcceleratedFeaturesToggle()
{
    AutoPtr<IView> view = IView::Probe(mOwner->mContainerView);
    Boolean isHardWareAccelerated;
    view->IsHardwareAccelerated(&isHardWareAccelerated);
    mOwner->mSettings->SetEnableSupportedHardwareAcceleratedFeatures(
            mOwner->mIsAttachedToWindow && isHardWareAccelerated &&
                    (mLayerType == IView::LAYER_TYPE_NONE
                    || mLayerType == IView::LAYER_TYPE_HARDWARE));
}

//@Override
AutoPtr<IInputConnection> AwContents::AwViewMethodsImpl::OnCreateInputConnection(
    /* [in] */ IEditorInfo* outAttrs)
{
    return mOwner->mContentViewCore->OnCreateInputConnection(outAttrs);
}

//@Override
Boolean AwContents::AwViewMethodsImpl::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
{
    return mOwner->mContentViewCore->OnKeyUp(keyCode, event);
}

//@Override
Boolean AwContents::AwViewMethodsImpl::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    if (IsDpadEvent(event)) {
        mOwner->mSettings->SetSpatialNavigationEnabled(TRUE);
    }
    return mOwner->mContentViewCore->DispatchKeyEvent(event);
}

Boolean AwContents::AwViewMethodsImpl::IsDpadEvent(
    /* [in] */ IKeyEvent* event)
{
    Int32 action;
    event->GetAction(&action);
    if (action == IKeyEvent::ACTION_DOWN) {
        Int32 code;
        event->GetKeyCode(&code);
        switch (code) {
            case IKeyEvent::KEYCODE_DPAD_CENTER:
            case IKeyEvent::KEYCODE_DPAD_DOWN:
            case IKeyEvent::KEYCODE_DPAD_UP:
            case IKeyEvent::KEYCODE_DPAD_LEFT:
            case IKeyEvent::KEYCODE_DPAD_RIGHT:
                return TRUE;
        }
    }
    return FALSE;
}

//@Override
Boolean AwContents::AwViewMethodsImpl::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    if (mOwner->mNativeAwContents == 0) return FALSE;

    Int32 mask;
    event->GetActionMasked(&mask);
    if (mask == IMotionEvent::ACTION_DOWN) {
        mOwner->mSettings->SetSpatialNavigationEnabled(FALSE);
    }

    mOwner->mScrollOffsetManager->SetProcessingTouchEvent(TRUE);
    Boolean rv = mOwner->mContentViewCore->OnTouchEvent(event);
    mOwner->mScrollOffsetManager->SetProcessingTouchEvent(FALSE);

    if (mask == IMotionEvent::ACTION_DOWN) {
        Int32 actionIndex;
        event->GetActionIndex(&actionIndex);

        // Note this will trigger IPC back to browser even if nothing is
        // hit.
        Float x, y;
        event->GetX(actionIndex, &x);
        event->GetY(actionIndex, &y);
        mOwner->NativeRequestNewHitTestDataAt(mOwner->mNativeAwContents,
                (Int32) Elastos::Core::Math::Round(x / mOwner->mDIPScale),
                (Int32) Elastos::Core::Math::Round(y / mOwner->mDIPScale));
    }

    if (mOwner->mOverScrollGlow != NULL) {
        if (mask == IMotionEvent::ACTION_DOWN) {
            mOwner->mOverScrollGlow->SetShouldPull(TRUE);
        }
        else if (mask == IMotionEvent::ACTION_UP ||
                mask == IMotionEvent::ACTION_CANCEL) {
            mOwner->mOverScrollGlow->SetShouldPull(FALSE);
            mOwner->mOverScrollGlow->ReleaseAll();
        }
    }

    return rv;
}

//@Override
Boolean AwContents::AwViewMethodsImpl::OnHoverEvent(
    /* [in] */ IMotionEvent* event)
{
    return mOwner->mContentViewCore->OnHoverEvent(event);
}

//@Override
Boolean AwContents::AwViewMethodsImpl::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event)
{
    return mOwner->mContentViewCore->OnGenericMotionEvent(event);
}

//@Override
void AwContents::AwViewMethodsImpl::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    mOwner->mContentViewCore->OnConfigurationChanged(newConfig);
}

//@Override
void AwContents::AwViewMethodsImpl::OnAttachedToWindow()
{
    if (mOwner->mNativeAwContents == 0) return;
    if (mOwner->mIsAttachedToWindow) {
        Logger::W(TAG, "onAttachedToWindow called when already attached. Ignoring");
        return;
    }
    mOwner->mIsAttachedToWindow = TRUE;

    mOwner->mContentViewCore->OnAttachedToWindow();
    Int32 width, height;
    AutoPtr<IView> view = IView::Probe(mOwner->mContainerView);
    view->GetWidth(&width);
    view->GetHeight(&height);
    mOwner->NativeOnAttachedToWindow(mOwner->mNativeAwContents, height, height);
    UpdateHardwareAcceleratedFeaturesToggle();

    if (mComponentCallbacks != NULL) return;
    mComponentCallbacks = new AwComponentCallbacks(mOwner);
    AutoPtr<IComponentCallbacks> cbs = IComponentCallbacks::Probe(mComponentCallbacks);
    mOwner->mContext->RegisterComponentCallbacks(cbs);
}

//@Override
void AwContents::AwViewMethodsImpl::OnDetachedFromWindow()
{
    if (!mOwner->mIsAttachedToWindow) {
        Logger::W(TAG, "onDetachedFromWindow called when already detached. Ignoring");
        return;
    }
    mOwner->mIsAttachedToWindow = FALSE;
    mOwner->HideAutofillPopup();
    if (mOwner->mNativeAwContents != 0) {
        mOwner->NativeOnDetachedFromWindow(mOwner->mNativeAwContents);
    }

    mOwner->mContentViewCore->OnDetachedFromWindow();
    UpdateHardwareAcceleratedFeaturesToggle();

    if (mComponentCallbacks != NULL) {
        AutoPtr<IComponentCallbacks> cbs = IComponentCallbacks::Probe(mComponentCallbacks);
        mOwner->mContext->UnregisterComponentCallbacks(cbs);
        mComponentCallbacks = NULL;
    }

    mOwner->mScrollAccessibilityHelper->RemovePostedCallbacks();
    mOwner->mNativeGLDelegate->DetachGLFunctor();
}

//@Override
void AwContents::AwViewMethodsImpl::OnWindowFocusChanged(
    /* [in] */ Boolean hasWindowFocus)
{
    mOwner->mWindowFocused = hasWindowFocus;
    mOwner->mContentViewCore->OnWindowFocusChanged(hasWindowFocus);
}

//@Override
void AwContents::AwViewMethodsImpl::OnFocusChanged(
    /* [in] */ Boolean focused,
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    mOwner->mContainerViewFocused = focused;
    mOwner->mContentViewCore->OnFocusChanged(focused);
}

//@Override
void AwContents::AwViewMethodsImpl::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 ow,
    /* [in] */ Int32 oh)
{
    if (mOwner->mNativeAwContents == 0) return;
    mOwner->mScrollOffsetManager->SetContainerViewSize(w, h);
    // The AwLayoutSizer needs to go first so that if we're in
    // fixedLayoutSize mode the update
    // to enter fixedLayoutSize mode is sent before the first resize
    // update.
    mOwner->mLayoutSizer->OnSizeChanged(w, h, ow, oh);
    mOwner->mContentViewCore->OnPhysicalBackingSizeChanged(w, h);
    mOwner->mContentViewCore->OnSizeChanged(w, h, ow, oh);
    mOwner->NativeOnSizeChanged(mOwner->mNativeAwContents, w, h, ow, oh);
}

//@Override
void AwContents::AwViewMethodsImpl::OnVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 _visibility)
{
    Int32 visibility;
    AutoPtr<IView> view = IView::Probe(mOwner->mContainerView);
    view->GetVisibility(&visibility);
    Boolean viewVisible = visibility == IView::VISIBLE;
    if (mOwner->mIsViewVisible == viewVisible) return;
    mOwner->SetViewVisibilityInternal(viewVisible);
}

//@Override
void AwContents::AwViewMethodsImpl::OnWindowVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    Boolean windowVisible = visibility == IView::VISIBLE;
    if (mOwner->mIsWindowVisible == windowVisible) return;
    mOwner->SetWindowVisibilityInternal(windowVisible);
}

//===============================================================
//                         AwContents
//===============================================================

AutoPtr<IRect> sLocalGlobalVisibleRectCreate()
{
    AutoPtr<IRect> rect;
    CRect::New((IRect**)&rect);
    return rect;
}

const String AwContents::TAG("AwContents");
const String AwContents::WEB_ARCHIVE_EXTENSION(".mht");
const Float AwContents::ZOOM_CONTROLS_EPSILON = 0.007f;
const AutoPtr<IRect> AwContents::sLocalGlobalVisibleRect = sLocalGlobalVisibleRectCreate();
const String AwContents::SAVE_RESTORE_STATE_KEY("WEBVIEW_CHROMIUM_STATE");

void AwContents::Init(
    /* [in] */ AwBrowserContext* browserContext,
    /* [in] */ IViewGroup* containerView,
    /* [in] */ IContext* context,
    /* [in] */ AwContents::InternalAccessDelegate* internalAccessAdapter,
    /* [in] */ NativeGLDelegate* nativeGLDelegate,
    /* [in] */ AwContentsClient* contentsClient,
    /* [in] */ AwSettings* settings,
    /* [in] */ DependencyFactory* dependencyFactory)
{
    mNativeAwContents = 0;
    mIsPaused = FALSE;
    mIsViewVisible = FALSE;
    mIsWindowVisible = FALSE;
    mIsAttachedToWindow = FALSE;
    mHasRequestedVisitedHistoryFromClient = FALSE;
    mBaseBackgroundColor = IColor::WHITE;
    mPossiblyStaleHitTestData = new HitTestData();
    mContainerViewFocused = FALSE;
    mWindowFocused = FALSE;
    mPageScaleFactor = 1.0f;
    mMinPageScaleFactor = 1.0f;
    mMaxPageScaleFactor = 1.0f;
    mContentWidthDip = 0.0f;
    mContentHeightDip = 0.0f;
    mDeferredShouldOverrideUrlLoadingIsPendingForPopup = FALSE;
    mTemporarilyDetached = FALSE;
    mOverlayHorizontalScrollbar = TRUE;
    mOverlayVerticalScrollbar = FALSE;


    mBrowserContext = browserContext;
    mContainerView = containerView;
    mContext = context;
    mInternalAccessAdapter = internalAccessAdapter;
    mNativeGLDelegate = nativeGLDelegate;
    mContentsClient = contentsClient;
    mAwViewMethods = new AwViewMethodsImpl(this);
    mFullScreenTransitionsState = new FullScreenTransitionsState(
            mContainerView, mInternalAccessAdapter, mAwViewMethods);
    mContentViewClient = new AwContentViewClient(contentsClient, settings, this, mContext);
    mLayoutSizer = dependencyFactory->CreateLayoutSizer();
    mSettings = settings;
    mDIPScale = DeviceDisplayInfo::Create(mContext)->GetDIPScale();
    AutoPtr<AwLayoutSizerDelegate> alsd = new AwLayoutSizerDelegate(this);
    mLayoutSizer->SetDelegate(alsd);
    mLayoutSizer->SetDIPScale(mDIPScale);
    AutoPtr<IView> view = IView::Probe(mContainerView);
    mWebContentsDelegate = new AwWebContentsDelegateAdapter(
            contentsClient, view, mContext);
    mContentsClientBridge = new AwContentsClientBridge(contentsClient,
            mBrowserContext->GetKeyStore(), AwContentsStatics::GetClientCertLookupTable());
    mZoomControls = new AwZoomControls(this);
    mIoThreadClient = new IoThreadClientImpl(this);
    mInterceptNavigationDelegate = new InterceptNavigationDelegateImpl(this);

    AutoPtr<AwSettings::ZoomSupportChangeListener> zoomListener = new InnerZoomSupportChangeListener(this);
    mSettings->SetZoomListener(zoomListener);
    mDefaultVideoPosterRequestHandler = new DefaultVideoPosterRequestHandler(mContentsClient);
    mSettings->SetDefaultVideoPosterURL(
            mDefaultVideoPosterRequestHandler->GetDefaultVideoPosterURL());
    mSettings->SetDIPScale(mDIPScale);

    AutoPtr<AwScrollOffsetManagerDelegate> asomd = new AwScrollOffsetManagerDelegate(this);
    AutoPtr<IOverScroller> overScroller;
    COverScroller::New(mContext, (IOverScroller**)&overScroller);
    mScrollOffsetManager = dependencyFactory->CreateScrollOffsetManager(asomd, overScroller);
    mScrollAccessibilityHelper = new ScrollAccessibilityHelper(view);

    Int32 overScrollMode;
    view->GetOverScrollMode(&overScrollMode);
    SetOverScrollMode(overScrollMode);
    SetScrollBarStyle(mInternalAccessAdapter->Super_getScrollBarStyle());
    mLayoutChangeListener = new AwLayoutChangeListener(this);
    view->AddOnLayoutChangeListener(mLayoutChangeListener);

    SetNewAwContents(NativeInit(mBrowserContext));

    OnContainerViewChanged();
}

CAR_INTERFACE_IMPL(AwContents, Object, IObject);
/**
 * @param browserContext the browsing context to associate this view contents with.
 * @param containerView the view-hierarchy item this object will be bound to.
 * @param context the context to use, usually containerView.getContext().
 * @param internalAccessAdapter to access private methods on containerView.
 * @param nativeGLDelegate to access the GL functor provided by the WebView.
 * @param contentsClient will receive API callbacks from this WebView Contents.
 * @param awSettings AwSettings instance used to configure the AwContents.
 *
 * This constructor uses the default view sizing policy.
 */
AwContents::AwContents(
    /* [in] */ AwBrowserContext* browserContext,
    /* [in] */ IViewGroup* containerView,
    /* [in] */ IContext* context,
    /* [in] */ AwContents::InternalAccessDelegate* internalAccessAdapter,
    /* [in] */ NativeGLDelegate* nativeGLDelegate,
    /* [in] */ AwContentsClient* contentsClient,
    /* [in] */ AwSettings* awSettings)
{
    AutoPtr<DependencyFactory> factory = new DependencyFactory();
    Init(browserContext, containerView, context, internalAccessAdapter, nativeGLDelegate,
            contentsClient, awSettings, factory);
}

/**
 * @param dependencyFactory an instance of the DependencyFactory used to provide instances of
 *                          classes that this class depends on.
 *
 * This version of the constructor is used in test code to inject test versions of the above
 * documented classes.
 */
AwContents::AwContents(
    /* [in] */ AwBrowserContext* browserContext,
    /* [in] */ IViewGroup* containerView,
    /* [in] */ IContext* context,
    /* [in] */ AwContents::InternalAccessDelegate* internalAccessAdapter,
    /* [in] */ NativeGLDelegate* nativeGLDelegate,
    /* [in] */ AwContentsClient* contentsClient,
    /* [in] */ AwSettings* settings,
    /* [in] */ DependencyFactory* dependencyFactory)
{
    Init(browserContext, containerView, context,
          internalAccessAdapter, nativeGLDelegate, contentsClient, settings, dependencyFactory);
}

AutoPtr<ContentViewCore> AwContents::CreateAndInitializeContentViewCore(
    /* [in] */ IViewGroup* containerView,
    /* [in] */ IContext* context,
    /* [in] */ AwContents::InternalAccessDelegate* internalDispatcher,
    /* [in] */ Handle64 nativeWebContents,
    /* [in] */ GestureStateListener* gestureStateListener,
    /* [in] */ ContentViewClient* contentViewClient,
    /* [in] */ ContentViewCore::ZoomControlsDelegate* zoomControlsDelegate)
{
    AutoPtr<ContentViewCore> contentViewCore = new ContentViewCore(context);
    AutoPtr<WindowElastos> window;
    AutoPtr<IActivity> activity = IActivity::Probe(context);
    if (activity != NULL) {
        window = new ActivityWindowElastos(activity);
    }
    else {
        AutoPtr<IContext> ctx;
        context->GetApplicationContext((IContext**)&ctx);
        window = new WindowElastos(ctx);
    }
    contentViewCore->Initialize(containerView, internalDispatcher, nativeWebContents, window);
    contentViewCore->AddGestureStateListener(gestureStateListener);
    contentViewCore->SetContentViewClient(contentViewClient);
    contentViewCore->SetZoomControlsDelegate(zoomControlsDelegate);
    return contentViewCore;
}

Boolean AwContents::IsFullScreen()
{
    return mFullScreenTransitionsState->IsFullScreen();
}

/**
 * Transitions this {@link AwContents} to fullscreen mode and returns the
 * {@link View} where the contents will be drawn while in fullscreen.
 */
AutoPtr<IView> AwContents::EnterFullScreen()
{
    assert(!IsFullScreen());

    // Detach to tear down the GL functor if this is still associated with the old
    // container view. It will be recreated during the next call to onDraw attached to
    // the new container view.
    OnDetachedFromWindow();

    // In fullscreen mode FullScreenView owns the AwViewMethodsImpl and AwContents
    // a NullAwViewMethods.
    AutoPtr<FullScreenView> fullScreenView = new FullScreenView(mContext, mAwViewMethods);
    mFullScreenTransitionsState->EnterFullScreen(fullScreenView);
    AutoPtr<IView> view = IView::Probe(mContainerView);
    mAwViewMethods = new NullAwViewMethods(this, mInternalAccessAdapter, view);
    view->RemoveOnLayoutChangeListener(mLayoutChangeListener);
    //TODO because the parent class AbsoluteLayout of FullScreenView
    fullScreenView->AddOnLayoutChangeListener(mLayoutChangeListener);

    // Associate this AwContents with the FullScreenView.
    SetInternalAccessAdapter(fullScreenView->GetInternalAccessAdapter());
    //because the parent class AbsoluteLayout of FullScreenView
    SetContainerView(fullScreenView);

    AutoPtr<IView> res = IView::Probe(fullScreenView);
    return res;
}

/**
 * Returns this {@link AwContents} to embedded mode, where the {@link AwContents} are drawn
 * in the WebView.
 */
void AwContents::ExitFullScreen()
{
    if (!IsFullScreen())
        // exitFullScreen() can be called without a prior call to enterFullScreen() if a
        // "misbehave" app overrides onShowCustomView but does not add the custom view to
        // the window. Exiting avoids a crash.
        return;

    // Detach to tear down the GL functor if this is still associated with the old
    // container view. It will be recreated during the next call to onDraw attached to
    // the new container view.
    // NOTE: we cannot use mAwViewMethods here because its type is NullAwViewMethods.
    AutoPtr<AwViewMethods> awViewMethodsImpl = mFullScreenTransitionsState->GetInitialAwViewMethods();
    awViewMethodsImpl->OnDetachedFromWindow();

    // Swap the view delegates. In embedded mode the FullScreenView owns a
    // NullAwViewMethods and AwContents the AwViewMethodsImpl.
    AutoPtr<FullScreenView> fullscreenView = mFullScreenTransitionsState->GetFullScreenView();
    AutoPtr<IView> fsView;
    fsView = IView::Probe(fullscreenView);
    AutoPtr<NullAwViewMethods> viewMethods = new NullAwViewMethods(this, fullscreenView->GetInternalAccessAdapter(), fsView);
    fullscreenView->SetAwViewMethods(viewMethods);
    mAwViewMethods = awViewMethodsImpl;
    AutoPtr<IViewGroup> initialContainerView = mFullScreenTransitionsState->GetInitialContainerView();
    AutoPtr<IView> icView = IView::Probe(initialContainerView);
    icView->AddOnLayoutChangeListener(mLayoutChangeListener);
    fsView->RemoveOnLayoutChangeListener(mLayoutChangeListener);

    // Re-associate this AwContents with the WebView.
    SetInternalAccessAdapter(mFullScreenTransitionsState->GetInitialInternalAccessDelegate());
    SetContainerView(initialContainerView);

    mFullScreenTransitionsState->ExitFullScreen();
}

void AwContents::SetInternalAccessAdapter(
    /* [in] */ InternalAccessDelegate* internalAccessAdapter)
{
    mInternalAccessAdapter = internalAccessAdapter;
    mContentViewCore->SetContainerViewInternals(mInternalAccessAdapter);
}

void AwContents::SetContainerView(
    /* [in] */ IViewGroup* newContainerView)
{
    mContainerView = newContainerView;
    mContentViewCore->SetContainerView(mContainerView);
    if (mAwPdfExporter != NULL) {
        mAwPdfExporter->SetContainerView(mContainerView);
    }
    AutoPtr<IView> view = IView::Probe(mContainerView);
    mWebContentsDelegate->SetContainerView(view);

    OnContainerViewChanged();
}

/**
 * Reconciles the state of this AwContents object with the state of the new container view.
 */
void AwContents::OnContainerViewChanged()
{
    // NOTE: mAwViewMethods is used by the old container view, the WebView, so it might refer
    // to a NullAwViewMethods when in fullscreen. To ensure that the state is reconciled with
    // the new container view correctly, we bypass mAwViewMethods and use the real
    // implementation directly.
    AutoPtr<AwViewMethods> awViewMethodsImpl = mFullScreenTransitionsState->GetInitialAwViewMethods();
    AutoPtr<IView> view = IView::Probe(mContainerView);
    Int32 visibility;
    view->GetVisibility(&visibility);
    awViewMethodsImpl->OnVisibilityChanged(view, visibility);
    Int32 windowVisibility;
    view->GetWindowVisibility(&windowVisibility);
    awViewMethodsImpl->OnWindowVisibilityChanged(windowVisibility);
    Boolean isATW;
    view->IsAttachedToWindow(&isATW);
    if (isATW) {
        awViewMethodsImpl->OnAttachedToWindow();
    }
    else {
        awViewMethodsImpl->OnDetachedFromWindow();
    }
    Int32 width, height;
    view->GetWidth(&width);
    view->GetHeight(&height);
    awViewMethodsImpl->OnSizeChanged(width, height, 0, 0);
    Boolean hWFocus;
    view->HasWindowFocus(&hWFocus);
    awViewMethodsImpl->OnWindowFocusChanged(hWFocus);
    Boolean hFocus;
    view->HasFocus(&hFocus);
    awViewMethodsImpl->OnFocusChanged(hFocus, 0, NULL);
    view->RequestLayout();
}

/* Common initialization routine for adopting a native AwContents instance into this
 * java instance.
 *
 * TAKE CARE! This method can get called multiple times per java instance. Code accordingly.
 * ^^^^^^^^^  See the native class declaration for more details on relative object lifetimes.
 */
void AwContents::SetNewAwContents(
    /* [in] */ Handle64 newAwContentsPtr)
{
    if (mNativeAwContents != 0) {
        Destroy();
        mContentViewCore = NULL;
    }

    assert(mNativeAwContents == 0 && mCleanupReference == NULL && mContentViewCore == NULL);

    mNativeAwContents = newAwContentsPtr;
    // TODO(joth): when the native and java counterparts of AwBrowserContext are hooked up to
    // each other, we should update |mBrowserContext| according to the newly received native
    // WebContent's browser context.

    // The native side object has been bound to this java instance, so now is the time to
    // bind all the native->java relationships.
    AutoPtr<IRunnable> runnable = new DestroyRunnable(mNativeAwContents);
    mCleanupReference = new CleanupReference(this, runnable);

    Handle64 nativeWebContents = NativeGetWebContents(mNativeAwContents);
    AutoPtr<AwGestureStateListener> listener = new AwGestureStateListener(this);
    mContentViewCore = CreateAndInitializeContentViewCore(
            mContainerView, mContext, mInternalAccessAdapter, nativeWebContents,
            listener, mContentViewClient, mZoomControls);
    NativeSetJavaPeers(mNativeAwContents, this, mWebContentsDelegate, mContentsClientBridge,
            mIoThreadClient, mInterceptNavigationDelegate);
    mContentsClient->InstallWebContentsObserver(mContentViewCore);
    mSettings->SetWebContents(nativeWebContents);
    NativeSetDipScale(mNativeAwContents, (Float) mDIPScale);

    // The only call to onShow. onHide should never be called.
    mContentViewCore->OnShow();
}

/**
 * Called on the "source" AwContents that is opening the popup window to
 * provide the AwContents to host the pop up content.
 */
void AwContents::SupplyContentsForPopup(
    /* [in] */ AwContents* newContents)
{
    Handle64 popupNativeAwContents = NativeReleasePopupAwContents(mNativeAwContents);
    if (popupNativeAwContents == 0) {
        Logger::W(TAG, "Popup WebView bind failed: no pending content.");
        if (newContents != NULL) newContents->Destroy();
        return;
    }
    if (newContents == NULL) {
        NativeDestroy(popupNativeAwContents);
        return;
    }

    newContents->ReceivePopupContents(popupNativeAwContents);
}

// Recap: supplyContentsForPopup() is called on the parent window's content, this method is
// called on the popup window's content.
void AwContents::ReceivePopupContents(
    /* [in] */ Handle64 popupNativeAwContents)
{
    mDeferredShouldOverrideUrlLoadingIsPendingForPopup = TRUE;
    // Save existing view state.
    const Boolean wasAttached = mIsAttachedToWindow;
    const Boolean wasViewVisible = mIsViewVisible;
    const Boolean wasWindowVisible = mIsWindowVisible;
    const Boolean wasPaused = mIsPaused;
    const Boolean wasFocused = mContainerViewFocused;
    const Boolean wasWindowFocused = mWindowFocused;

    // Properly clean up existing mContentViewCore and mNativeAwContents.
    if (wasFocused) OnFocusChanged(FALSE, 0, NULL);
    if (wasWindowFocused) OnWindowFocusChanged(FALSE);
    if (wasViewVisible) SetViewVisibilityInternal(FALSE);
    if (wasWindowVisible) SetWindowVisibilityInternal(FALSE);
    if (wasAttached) OnDetachedFromWindow();
    if (!wasPaused) OnPause();

    // Save injected JavaScript interfaces.
    //Map<String, Pair<Object, Class>> javascriptInterfaces = new HashMap<String, Pair<Object, Class>>();
    AutoPtr<IMap> javascriptInterfaces;
    CHashMap::New((IMap**)&javascriptInterfaces);

    if (mContentViewCore != NULL) {
        javascriptInterfaces->PutAll(mContentViewCore->GetJavascriptInterfaces());
    }

    SetNewAwContents(popupNativeAwContents);

    // Finally refresh all view state for mContentViewCore and mNativeAwContents.
    if (!wasPaused) OnResume();
    if (wasAttached) {
        OnAttachedToWindow();
        PostInvalidateOnAnimation();
    }
    AutoPtr<IView> view = IView::Probe(mContainerView);
    Int32 width, height;
    view->GetWidth(&width);
    view->GetHeight(&height);
    OnSizeChanged(width, height, 0, 0);
    if (wasWindowVisible) SetWindowVisibilityInternal(TRUE);
    if (wasViewVisible) SetViewVisibilityInternal(TRUE);
    if (wasWindowFocused) OnWindowFocusChanged(wasWindowFocused);
    if (wasFocused) OnFocusChanged(TRUE, 0, NULL);

    // Restore injected JavaScript interfaces.
    //for (Map.Entry<String, Pair<Object, Class>> entry : javascriptInterfaces.entrySet())
    AutoPtr<ISet> entrySet;
    javascriptInterfaces->GetEntrySet((ISet**)&entrySet);
    AutoPtr<IIterator> it;
    entrySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    AutoPtr<IMapEntry> entry;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> mapEntry;
        it->GetNext((IInterface**)&mapEntry);
        entry = IMapEntry::Probe(mapEntry);
        AutoPtr<IInterface> ko, vo;
        entry->GetKey((IInterface**)&ko);//ko string, vo is Pair
        entry->GetValue((IInterface**)&vo);
        AutoPtr<ICharSequence> ics = ICharSequence::Probe(ko);
        String keyStr;
        ics->ToString(&keyStr);
        AutoPtr<IPair> pair = IPair::Probe(vo);

        AutoPtr<IInterface> objectValue, classValue;
        pair->GetFirst((IInterface**)&objectValue);
        pair->GetSecond((IInterface**)&classValue);

        AutoPtr<Object> obj = (Object*)IObject::Probe(objectValue);
        mContentViewCore->AddPossiblyUnsafeJavascriptInterface(objectValue, keyStr, classValue);
    }
}

/**
 * Deletes the native counterpart of this object.
 */
void AwContents::Destroy()
{
    if (mCleanupReference != NULL) {
        assert(mNativeAwContents != 0);
        // If we are attached, we have to call native detach to clean up
        // hardware resources.
        if (mIsAttachedToWindow) {
            NativeOnDetachedFromWindow(mNativeAwContents);
        }

        // We explicitly do not null out the mContentViewCore reference here
        // because ContentViewCore already has code to deal with the case
        // methods are called on it after it's been destroyed, and other
        // code relies on AwContents.mContentViewCore to be non-null.
        mContentViewCore->Destroy();
        mNativeAwContents = 0;

        mCleanupReference->CleanupNow();
        mCleanupReference = NULL;
    }

    assert(!mContentViewCore->IsAlive());
    assert(mNativeAwContents == 0);
}

//@VisibleForTesting
AutoPtr<ContentViewCore> AwContents::GetContentViewCore()
{
    return mContentViewCore;
}

// Can be called from any thread.
AutoPtr<AwSettings> AwContents::GetSettings()
{
    return mSettings;
}

AutoPtr<AwPdfExporter> AwContents::GetPdfExporter()
{
    // mNativeAwContents can be null, due to destroy().
    if (mNativeAwContents == 0) {
        return NULL;
    }
    if (mAwPdfExporter == NULL) {
        mAwPdfExporter = new AwPdfExporter(mContainerView);
        NativeCreatePdfExporter(mNativeAwContents, mAwPdfExporter);
    }
    return mAwPdfExporter;
}

void AwContents::SetAwDrawSWFunctionTable(
    /* [in] */ Handle64 functionTablePointer)
{
    NativeSetAwDrawSWFunctionTable(functionTablePointer);
}

void AwContents::SetAwDrawGLFunctionTable(
    /* [in] */ Handle64 functionTablePointer)
{
    NativeSetAwDrawGLFunctionTable(functionTablePointer);
}

Handle64 AwContents::GetAwDrawGLFunction()
{
    return NativeGetAwDrawGLFunction();
}

void AwContents::SetShouldDownloadFavicons()
{
    NativeSetShouldDownloadFavicons();
}

/**
 * Disables contents of JS-to-Java bridge objects to be inspectable using
 * Object.keys() method and "for .. in" loops. This is intended for applications
 * targeting earlier Android releases where this was not possible, and we want
 * to ensure backwards compatible behavior.
 */
void AwContents::DisableJavascriptInterfacesInspection()
{
    mContentViewCore->SetAllowJavascriptInterfacesInspection(FALSE);
}

/**
 * Intended for test code.
 * @return the number of native instances of this class.
 */
//@VisibleForTesting
Int32 AwContents::GetNativeInstanceCount()
{
    return NativeGetNativeInstanceCount();
}

Handle64 AwContents::GetAwDrawGLViewContext()
{
    // Only called during early construction, so client should not have had a chance to
    // call destroy yet.
    assert(mNativeAwContents != 0);

    // Using the native pointer as the returned viewContext. This is matched by the
    // reinterpret_cast back to BrowserViewRenderer pointer in the native DrawGLFunction.
    return NativeGetAwDrawGLViewContext(mNativeAwContents);
}

AutoPtr<IRect> AwContents::GetGlobalVisibleRect()
{
    AutoPtr<IView> view = IView::Probe(mContainerView);
    Boolean res;
    view->GetGlobalVisibleRect(sLocalGlobalVisibleRect, &res);
    if (!res) {
        sLocalGlobalVisibleRect->SetEmpty();
    }
    return sLocalGlobalVisibleRect;
}

//--------------------------------------------------------------------------------------------
//  WebView[Provider] method implementations (where not provided by ContentViewCore)
//--------------------------------------------------------------------------------------------

void AwContents::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    mAwViewMethods->OnDraw(canvas);
}

ECode AwContents::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    mAwViewMethods->OnMeasure(widthMeasureSpec, heightMeasureSpec);
    return NOERROR;
}

Int32 AwContents::GetContentHeightCss()
{
    return (Int32) Elastos::Core::Math::Ceil(mContentHeightDip);
}

Int32 AwContents::GetContentWidthCss()
{
    return (Int32) Elastos::Core::Math::Ceil(mContentWidthDip);
}

AutoPtr<IPicture> AwContents::CapturePicture()
{
    if (mNativeAwContents == 0) return NULL;
    AutoPtr<IPicture> picture = new AwPicture(NativeCapturePicture(mNativeAwContents,
            mScrollOffsetManager->ComputeHorizontalScrollRange(),
            mScrollOffsetManager->ComputeVerticalScrollRange()));

    return picture;
}

void AwContents::ClearView()
{
    if (mNativeAwContents == 0) return;
    NativeClearView(mNativeAwContents);
}

/**
 * Enable the onNewPicture callback.
 * @param enabled Flag to enable the callback.
 * @param invalidationOnly Flag to call back only on invalidation without providing a picture.
 */
void AwContents::EnableOnNewPicture(
    /* [in] */ Boolean enabled,
    /* [in] */ Boolean invalidationOnly)
{
    if (mNativeAwContents == 0) return;
    if (invalidationOnly) {
        mPictureListenerContentProvider = NULL;
    }
    else if (enabled && mPictureListenerContentProvider == NULL) {
        mPictureListenerContentProvider = new InnerCallable(this);
    }
    NativeEnableOnNewPicture(mNativeAwContents, enabled);
}

void AwContents::FindAllAsync(
    /* [in] */ const String& searchString)
{
    if (mNativeAwContents == 0) return;
    NativeFindAllAsync(mNativeAwContents, searchString);
}

void AwContents::FindNext(
    /* [in] */ Boolean forward)
{
    if (mNativeAwContents == 0) return;
    NativeFindNext(mNativeAwContents, forward);
}

void AwContents::ClearMatches()
{
    if (mNativeAwContents == 0) return;
    NativeClearMatches(mNativeAwContents);
}

/**
 * @return load progress of the WebContents.
 */
Int32 AwContents::GetMostRecentProgress()
{
    // WebContentsDelegateAndroid conveniently caches the most recent notified value for us.
    return mWebContentsDelegate->GetMostRecentProgress();
}

AutoPtr<IBitmap> AwContents::GetFavicon()
{
    return mFavicon;
}

void AwContents::RequestVisitedHistoryFromClient()
{
    AutoPtr<IValueCallback> callback = new InnerValueCallback(this);
    mContentsClient->GetVisitedHistory(callback);
}

/**
 * Load url without fixing up the url string. Consumers of ContentView are responsible for
 * ensuring the URL passed in is properly formatted (i.e. the scheme has been added if left
 * off during user input).
 *
 * @param params Parameters for this load.
 */
void AwContents::LoadUrl(
    /* [in] */ LoadUrlParams* params) {
    if (params->GetLoadUrlType() == LoadUrlParams::LOAD_TYPE_DATA &&
            !params->IsBaseUrlDataScheme()) {
        // This allows data URLs with a non-data base URL access to file:///android_asset/ and
        // file:///android_res/ URLs. If AwSettings.getAllowFileAccess permits, it will also
        // allow access to file:// URLs (subject to OS level permission checks).
        params->SetCanLoadLocalResources(TRUE);
    }

    // If we are reloading the same url, then set transition type as reload.
    if (params->GetUrl() != NULL &&
            params->GetUrl().Equals(mContentViewCore->GetUrl()) &&
            params->GetTransitionType() == PageTransitionTypes::PAGE_TRANSITION_LINK) {
        params->SetTransitionType(PageTransitionTypes::PAGE_TRANSITION_RELOAD);
    }
    params->SetTransitionType(
            params->GetTransitionType() | PageTransitionTypes::PAGE_TRANSITION_FROM_API);

    // For WebView, always use the user agent override, which is set
    // every time the user agent in AwSettings is modified.
    params->SetOverrideUserAgent(LoadUrlParams::UA_OVERRIDE_TRUE);


    // We don't pass extra headers to the content layer, as WebViewClassic
    // was adding them in a very narrow set of conditions. See http://crbug.com/306873
    // However, if the embedder is attempting to inject a Referer header for their
    // loadUrl call, then we set that separately and remove it from the extra headers map/
    String REFERER("referer");
    AutoPtr<IMap> extraHeaders = params->GetExtraHeaders();//Map<String, String>
    //for (String header : extraHeaders.keySet())
    if (extraHeaders != NULL) {
        AutoPtr<ISet> entrySet;
        extraHeaders->GetEntrySet((ISet**)&entrySet);
        AutoPtr<IIterator> it;
        entrySet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        AutoPtr<IMapEntry> entry;
        while(it->HasNext(&hasNext), hasNext)
        {
            AutoPtr<IInterface> mapEntry;
            it->GetNext((IInterface**)&mapEntry);
            entry = IMapEntry::Probe(mapEntry);
            AutoPtr<IInterface> ko, vo;
            entry->GetKey((IInterface**)&ko);
            entry->GetValue((IInterface**)&vo);
            AutoPtr<ICharSequence> ics = ICharSequence::Probe(ko);
            String header;
            ics->ToString(&header);
            if (REFERER.Equals(header.ToLowerCase(/*Locale.US*/))) {//TODO not support Local.US
                AutoPtr<ICharSequence> vics = ICharSequence::Probe(vo);
                String strValue;
                vics->ToString(&strValue);
                extraHeaders->Remove(ko);
                AutoPtr<Referrer> referrer = new Referrer(strValue, 1);
                params->SetReferrer(referrer);
                params->SetExtraHeaders(extraHeaders);
                break;
            }
        }
    }

    if (mNativeAwContents != 0) {
        NativeSetExtraHeadersForUrl(
                mNativeAwContents, params->GetUrl(), params->GetExtraHttpRequestHeadersString());
    }
    AutoPtr<IMap> newExtraHeaders;
    CHashMap::New((IMap**)&newExtraHeaders);
    params->SetExtraHeaders(newExtraHeaders);

    mContentViewCore->LoadUrl(params);

    // The behavior of WebViewClassic uses the populateVisitedLinks callback in WebKit.
    // Chromium does not use this use code path and the best emulation of this behavior to call
    // request visited links once on the first URL load of the WebView.
    if (!mHasRequestedVisitedHistoryFromClient) {
        mHasRequestedVisitedHistoryFromClient = TRUE;
        RequestVisitedHistoryFromClient();
    }

    if (params->GetLoadUrlType() == LoadUrlParams::LOAD_TYPE_DATA &&
            params->GetBaseUrl() != NULL) {
        // Data loads with a base url will be resolved in Blink, and not cause an onPageStarted
        // event to be sent. Sending the callback directly from here.
        mContentsClient->GetCallbackHelper()->PostOnPageStarted(params->GetBaseUrl());
    }
}

/**
 * Get the URL of the current page.
 *
 * @return The URL of the current page or null if it's empty.
 */
String AwContents::GetUrl()
{
    String url =  mContentViewCore->GetUrl();
    if (url.IsNull() || url.Trim().IsEmpty()) return String(NULL);
    return url;
}

void AwContents::RequestFocus() {
    mAwViewMethods->RequestFocus();
}

void AwContents::SetBackgroundColor(
    /* [in] */ Int32 color)
{
    mBaseBackgroundColor = color;
    if (mNativeAwContents != 0) NativeSetBackgroundColor(mNativeAwContents, color);
}

/**
 * @see android.view.View#setLayerType()
 */
void AwContents::SetLayerType(
    /* [in] */ Int32 layerType,
    /* [in] */ IPaint* paint)
{
    mAwViewMethods->SetLayerType(layerType, paint);
}

Int32 AwContents::GetEffectiveBackgroundColor()
{
    // Do not ask the ContentViewCore for the background color, as it will always
    // report white prior to initial navigation or post destruction,  whereas we want
    // to use the client supplied base value in those cases.
    if (mNativeAwContents == 0 || !mContentsClient->IsCachedRendererBackgroundColorValid()) {
        return mBaseBackgroundColor;
    }
    return mContentsClient->GetCachedRendererBackgroundColor();
}

Boolean AwContents::IsMultiTouchZoomSupported()
{
    return mSettings->SupportsMultiTouchZoom();
}

AutoPtr<IView> AwContents::GetZoomControlsForTest()
{
    return mZoomControls->GetZoomControlsViewForTest();
}

/**
 * @see ContentViewCore#getContentSettings()
 */
AutoPtr<ContentSettings> AwContents::GetContentSettings()
{
    return mContentViewCore->GetContentSettings();
}

/**
 * @see View#setOverScrollMode(int)
 */
void AwContents::SetOverScrollMode(int mode)
{
    if (mode != IView::OVER_SCROLL_NEVER) {
        AutoPtr<IView> view = IView::Probe(mContainerView);
        mOverScrollGlow = new OverScrollGlow(mContext, view);
    }
    else {
        mOverScrollGlow = NULL;
    }
}

/**
 * @see View#setScrollBarStyle(int)
 */
void AwContents::SetScrollBarStyle(
    /* [in] */ Int32 style)
{
    if (style == IView::SCROLLBARS_INSIDE_OVERLAY
            || style == IView::SCROLLBARS_OUTSIDE_OVERLAY) {
        mOverlayHorizontalScrollbar = mOverlayVerticalScrollbar = TRUE;
    }
    else {
        mOverlayHorizontalScrollbar = mOverlayVerticalScrollbar = FALSE;
    }
}

/**
 * @see View#setHorizontalScrollbarOverlay(boolean)
 */
void AwContents::SetHorizontalScrollbarOverlay(
    /* [in] */ Boolean overlay)
{
    mOverlayHorizontalScrollbar = overlay;
}

/**
 * @see View#setVerticalScrollbarOverlay(boolean)
 */
void AwContents::SetVerticalScrollbarOverlay(
    /* [in] */ Boolean overlay)
{
    mOverlayVerticalScrollbar = overlay;
}

/**
 * @see View#overlayHorizontalScrollbar()
 */
Boolean AwContents::OverlayHorizontalScrollbar()
{
    return mOverlayHorizontalScrollbar;
}

/**
 * @see View#overlayVerticalScrollbar()
 */
Boolean AwContents::OverlayVerticalScrollbar()
{
    return mOverlayVerticalScrollbar;
}

/**
 * Called by the embedder when the scroll offset of the containing view has changed.
 * @see View#onScrollChanged(int,int)
 */
void AwContents::OnContainerViewScrollChanged(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 oldl,
    /* [in] */ Int32 oldt)
{
    // A side-effect of View.onScrollChanged is that the scroll accessibility event being sent
    // by the base class implementation. This is completely hidden from the base classes and
    // cannot be prevented, which is why we need the code below.
    mScrollAccessibilityHelper->RemovePostedViewScrolledAccessibilityEventCallback();
    mScrollOffsetManager->OnContainerViewScrollChanged(l, t);
}

/**
 * Called by the embedder when the containing view is to be scrolled or overscrolled.
 * @see View#onOverScrolled(int,int,int,int)
 */
void AwContents::OnContainerViewOverScrolled(
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY,
    /* [in] */ Boolean clampedX,
    /* [in] */ Boolean clampedY)
{
    AutoPtr<IView> view = IView::Probe(mContainerView);
    Int32 oldX;
    view->GetScrollX(&oldX);
    Int32 oldY;
    view->GetScrollY(&oldY);

    mScrollOffsetManager->OnContainerViewOverScrolled(scrollX, scrollY, clampedX, clampedY);

    if (mOverScrollGlow != NULL) {
        Int32 x;
        view->GetScrollX(&x);
        Int32 y;
        view->GetScrollY(&y);
        mOverScrollGlow->PullGlow(x, y, oldX, oldY,
                mScrollOffsetManager->ComputeMaximumHorizontalScrollOffset(),
                mScrollOffsetManager->ComputeMaximumVerticalScrollOffset());
    }
}

/**
 * @see android.webkit.WebView#requestChildRectangleOnScreen(View, Rect, boolean)
 */
Boolean AwContents::RequestChildRectangleOnScreen(
    /* [in] */ IView* child,
    /* [in] */ IRect* rect,
    /* [in] */ Boolean immediate)
{
    Int32 x, y, left, top;
    child->GetLeft(&left);
    child->GetScrollX(&x);
    child->GetTop(&top);
    child->GetScrollY(&y);
    return mScrollOffsetManager->RequestChildRectangleOnScreen(
            left - x, top - y,
            rect, immediate);
}

/**
 * @see View.computeScroll()
 */
void AwContents::ComputeScroll()
{
    mScrollOffsetManager->ComputeScrollAndAbsorbGlow(mOverScrollGlow);
}

/**
 * @see View#computeHorizontalScrollRange()
 */
Int32 AwContents::ComputeHorizontalScrollRange()
{
    return mScrollOffsetManager->ComputeHorizontalScrollRange();
}

/**
 * @see View#computeHorizontalScrollOffset()
 */
Int32 AwContents::ComputeHorizontalScrollOffset()
{
    return mScrollOffsetManager->ComputeHorizontalScrollOffset();
}

/**
 * @see View#computeVerticalScrollRange()
 */
Int32 AwContents::ComputeVerticalScrollRange()
{
    return mScrollOffsetManager->ComputeVerticalScrollRange();
}

/**
 * @see View#computeVerticalScrollOffset()
 */
Int32 AwContents::ComputeVerticalScrollOffset()
{
    return mScrollOffsetManager->ComputeVerticalScrollOffset();
}

/**
 * @see View#computeVerticalScrollExtent()
 */
Int32 AwContents::ComputeVerticalScrollExtent()
{
    return mScrollOffsetManager->ComputeVerticalScrollExtent();
}

/**
 * @see android.webkit.WebView#stopLoading()
 */
void AwContents::StopLoading()
{
    mContentViewCore->StopLoading();
}

/**
 * @see android.webkit.WebView#reload()
 */
void AwContents::Reload()
{
    mContentViewCore->Reload(TRUE);
}

/**
 * @see android.webkit.WebView#canGoBack()
 */
Boolean AwContents::CanGoBack()
{
    return mContentViewCore->CanGoBack();
}

/**
 * @see android.webkit.WebView#goBack()
 */
void AwContents::GoBack()
{
    mContentViewCore->GoBack();
}

/**
 * @see android.webkit.WebView#canGoForward()
 */
Boolean AwContents::CanGoForward()
{
    return mContentViewCore->CanGoForward();
}

/**
 * @see android.webkit.WebView#goForward()
 */
void AwContents::GoForward()
{
    mContentViewCore->GoForward();
}

/**
 * @see android.webkit.WebView#canGoBackOrForward(int)
 */
Boolean AwContents::CanGoBackOrForward(
    /* [in] */ Int32 steps)
{
    return mContentViewCore->CanGoToOffset(steps);
}

/**
 * @see android.webkit.WebView#goBackOrForward(int)
 */
void AwContents::GoBackOrForward(
    /* [in] */ Int32 steps)
{
    mContentViewCore->GoToOffset(steps);
}

/**
 * @see android.webkit.WebView#pauseTimers()
 */
void AwContents::PauseTimers()
{
    ContentViewStatics::SetWebKitSharedTimersSuspended(TRUE);
}

/**
 * @see android.webkit.WebView#resumeTimers()
 */
void AwContents::ResumeTimers() {
    ContentViewStatics::SetWebKitSharedTimersSuspended(FALSE);
}

/**
 * @see android.webkit.WebView#onPause()
 */
void AwContents::OnPause() {
    if (mIsPaused || mNativeAwContents == 0) return;
    mIsPaused = TRUE;
    NativeSetIsPaused(mNativeAwContents, mIsPaused);
}

/**
 * @see android.webkit.WebView#onResume()
 */
void AwContents::OnResume() {
    if (!mIsPaused || mNativeAwContents == 0) return;
    mIsPaused = FALSE;
    NativeSetIsPaused(mNativeAwContents, mIsPaused);
}

/**
 * @see android.webkit.WebView#isPaused()
 */
Boolean AwContents::IsPaused()
{
    return mIsPaused;
}

/**
 * @see android.webkit.WebView#onCreateInputConnection(EditorInfo)
 */
AutoPtr<IInputConnection> AwContents::OnCreateInputConnection(
    /* [in] */ IEditorInfo* outAttrs)
{
    return mAwViewMethods->OnCreateInputConnection(outAttrs);
}

/**
 * @see android.webkit.WebView#onKeyUp(int, KeyEvent)
 */
Boolean AwContents::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
{
    return mAwViewMethods->OnKeyUp(keyCode, event);
}

/**
 * @see android.webkit.WebView#dispatchKeyEvent(KeyEvent)
 */
Boolean AwContents::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    return mAwViewMethods->DispatchKeyEvent(event);
}

/**
 * Clears the resource cache. Note that the cache is per-application, so this will clear the
 * cache for all WebViews used.
 *
 * @param includeDiskFiles if false, only the RAM cache is cleared
 */
void AwContents::ClearCache(
    /* [in] */ Boolean includeDiskFiles)
{
    if (mNativeAwContents == 0) return;
    NativeClearCache(mNativeAwContents, includeDiskFiles);
}

void AwContents::DocumentHasImages(
    /* [in] */ IMessage* message)
{
    if (mNativeAwContents == 0) return;
    NativeDocumentHasImages(mNativeAwContents, message);
}

void AwContents::SaveWebArchive(
    /* [in] */ const String& basename,
    /* [in] */ Boolean autoname,
    /* [in] */ IValueCallback* callback)
{
    if (!autoname) {
        SaveWebArchiveInternal(basename, callback);
        return;
    }
    // If auto-generating the file name, handle the name generation on a background thread
    // as it will require I/O access for checking whether previous files existed.
    AutoPtr<SaveWebArchiveInternalTask> task = new SaveWebArchiveInternalTask(this, basename, callback);
    task->Execute((ArrayOf<IInterface*>*)NULL);
}

String AwContents::GetOriginalUrl()
{
    AutoPtr<NavigationHistory> history = mContentViewCore->GetNavigationHistory();
    Int32 currentIndex = history->GetCurrentEntryIndex();
    if (currentIndex >= 0 && currentIndex < history->GetEntryCount()) {
        return history->GetEntryAtIndex(currentIndex)->GetOriginalUrl();
    }
    return String(NULL);
}

/**
 * @see ContentViewCore#getNavigationHistory()
 */
AutoPtr<NavigationHistory> AwContents::GetNavigationHistory()
{
    return mContentViewCore->GetNavigationHistory();
}

/**
 * @see android.webkit.WebView#getTitle()
 */
String AwContents::GetTitle()
{
    return mContentViewCore->GetTitle();
}

/**
 * @see android.webkit.WebView#clearHistory()
 */
void AwContents::ClearHistory()
{
    mContentViewCore->ClearHistory();
}

AutoPtr<ArrayOf<String> > AwContents::GetHttpAuthUsernamePassword(
    /* [in] */ const String& host,
    /* [in] */ const String& realm)
{
    return mBrowserContext->GetHttpAuthDatabase(mContext)->GetHttpAuthUsernamePassword(host, realm);
}

void AwContents::SetHttpAuthUsernamePassword(
    /* [in] */ const String& host,
    /* [in] */ const String& realm,
    /* [in] */ const String& username,
    /* [in] */ const String& password)
{
    mBrowserContext->GetHttpAuthDatabase(mContext)->SetHttpAuthUsernamePassword(host, realm, username, password);
}

/**
 * @see android.webkit.WebView#getCertificate()
 */
AutoPtr<ISslCertificate> AwContents::GetCertificate()
{
    if (mNativeAwContents == 0) return NULL;
    return SslUtil::GetCertificateFromDerBytes(NativeGetCertificate(mNativeAwContents));
}

/**
 * @see android.webkit.WebView#clearSslPreferences()
 */
void AwContents::ClearSslPreferences()
{
    mContentViewCore->ClearSslPreferences();
}

// TODO(sgurun) remove after this rolls in. To keep internal tree happy.
void AwContents::ClearClientCertPreferences()
{
}

/**
 * Method to return all hit test values relevant to public WebView API.
 * Note that this expose more data than needed for WebView.getHitTestResult.
 * Unsafely returning reference to mutable internal object to avoid excessive
 * garbage allocation on repeated calls.
 */
AutoPtr<AwContents::HitTestData> AwContents::GetLastHitTestResult()
{
    if (mNativeAwContents == 0) return NULL;
    NativeUpdateLastHitTestData(mNativeAwContents);
    return mPossiblyStaleHitTestData;
}

/**
 * @see android.webkit.WebView#requestFocusNodeHref()
 */
void AwContents::RequestFocusNodeHref(
    /* [in] */ IMessage* msg)
{
    if (msg == NULL || mNativeAwContents == 0) return;

    NativeUpdateLastHitTestData(mNativeAwContents);
    AutoPtr<IBundle> data;
    msg->GetData((IBundle**)&data);

    // In order to maintain compatibility with the old WebView's implementation,
    // the absolute (full) url is passed in the |url| field, not only the href attribute.
    // Note: HitTestData could be cleaned up at this point. See http://crbug.com/290992.
    data->PutString(String("url"), mPossiblyStaleHitTestData->href);
    data->PutString(String("title"), mPossiblyStaleHitTestData->anchorText);
    data->PutString(String("src"), mPossiblyStaleHitTestData->imgSrc);
    msg->SetData(data);
    msg->SendToTarget();
}

/**
 * @see android.webkit.WebView#requestImageRef()
 */
void AwContents::RequestImageRef(
    /* [in] */ IMessage* msg)
{
    if (msg == NULL || mNativeAwContents == 0) return;

    NativeUpdateLastHitTestData(mNativeAwContents);
    AutoPtr<IBundle> data;
    msg->GetData((IBundle**)&data);
    data->PutString(String("url"), mPossiblyStaleHitTestData->imgSrc);
    msg->SetData(data);
    msg->SendToTarget();
}

//@VisibleForTesting
Float AwContents::GetPageScaleFactor()
{
    return mPageScaleFactor;
}

/**
 * @see android.webkit.WebView#getScale()
 *
 * Please note that the scale returned is the page scale multiplied by
 * the screen density factor. See CTS WebViewTest.testSetInitialScale.
 */
Float AwContents::GetScale()
{
    return (Float)(mPageScaleFactor * mDIPScale);
}

/**
 * @see android.webkit.WebView#flingScroll(int, int)
 */
void AwContents::FlingScroll(
    /* [in] */ Int32 velocityX,
    /* [in] */ Int32 velocityY)
{
    mScrollOffsetManager->FlingScroll(velocityX, velocityY);
}

/**
 * @see android.webkit.WebView#pageUp(boolean)
 */
Boolean AwContents::PageUp(
    /* [in] */ Boolean top)
{
    return mScrollOffsetManager->PageUp(top);
}

/**
 * @see android.webkit.WebView#pageDown(boolean)
 */
Boolean AwContents::PageDown(
    /* [in] */ Boolean bottom)
{
    return mScrollOffsetManager->PageDown(bottom);
}

/**
 * @see android.webkit.WebView#canZoomIn()
 */
// This method uses the term 'zoom' for legacy reasons, but relates
// to what chrome calls the 'page scale factor'.
Boolean AwContents::CanZoomIn()
{
    const Float zoomInExtent = mMaxPageScaleFactor - mPageScaleFactor;
    return zoomInExtent > ZOOM_CONTROLS_EPSILON;
}

/**
 * @see android.webkit.WebView#canZoomOut()
 */
// This method uses the term 'zoom' for legacy reasons, but relates
// to what chrome calls the 'page scale factor'.
Boolean AwContents::CanZoomOut()
{
    const Float zoomOutExtent = mPageScaleFactor - mMinPageScaleFactor;
    return zoomOutExtent > ZOOM_CONTROLS_EPSILON;
}

/**
 * @see android.webkit.WebView#zoomIn()
 */
// This method uses the term 'zoom' for legacy reasons, but relates
// to what chrome calls the 'page scale factor'.
Boolean AwContents::ZoomIn()
{
    if (!CanZoomIn()) {
        return FALSE;
    }
    return ZoomBy(1.25f);
}

/**
 * @see android.webkit.WebView#zoomOut()
 */
// This method uses the term 'zoom' for legacy reasons, but relates
// to what chrome calls the 'page scale factor'.
Boolean AwContents::ZoomOut()
{
    if (!CanZoomOut()) {
        return FALSE;
    }
    return ZoomBy(0.8f);
}

/**
 * @see android.webkit.WebView#zoomBy()
 */
// This method uses the term 'zoom' for legacy reasons, but relates
// to what chrome calls the 'page scale factor'.
Boolean AwContents::ZoomBy(
    /* [in] */ Float delta)
{
    if (delta < 0.01f || delta > 100.0f) {
        //throw new IllegalStateException("zoom delta value outside [0.01, 100] range.");
        Logger::E(TAG, "zoom delta value outside [0.01, 100] range.");
        return FALSE;
    }
    return mContentViewCore->PinchByDelta(delta);
}

/**
 * @see android.webkit.WebView#invokeZoomPicker()
 */
void AwContents::InvokeZoomPicker()
{
    mContentViewCore->InvokeZoomPicker();
}

/**
 * @see android.webkit.WebView#preauthorizePermission(Uri, long)
 */
void AwContents::PreauthorizePermission(
    /* [in] */ IUri* origin,
    /* [in] */ Int64 resources)
{
    if (mNativeAwContents == 0) return;
    AutoPtr<Uri> cOrigin = (Uri*)origin;
    String originStr;
    cOrigin->ToString(&originStr);
    NativePreauthorizePermission(mNativeAwContents, originStr, resources);
}

/**
 * @see ContentViewCore.evaluateJavaScript(String, ContentViewCore.JavaScriptCallback)
 */
void AwContents::EvaluateJavaScript(
    /* [in] */ const String& script,
    /* [in] */ IValueCallback* callback)
{
    AutoPtr<ContentViewCore::JavaScriptCallback> jsCallback;
    if (callback != NULL) {
        jsCallback = new InnerJavaScriptCallback(this, callback);
    }

    mContentViewCore->EvaluateJavaScript(script, jsCallback);
}

/**
 * @see ContentViewCore.evaluateJavaScriptEvenIfNotYetNavigated(String)
 */
void AwContents::EvaluateJavaScriptEvenIfNotYetNavigated(
    /* [in] */ const String& script)
{
    mContentViewCore->EvaluateJavaScriptEvenIfNotYetNavigated(script);
}

//--------------------------------------------------------------------------------------------
//  View and ViewGroup method implementations
//--------------------------------------------------------------------------------------------

/**
 * @see android.webkit.View#onTouchEvent()
 */
Boolean AwContents::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    return mAwViewMethods->OnTouchEvent(event);
}

/**
 * @see android.view.View#onHoverEvent()
 */
Boolean AwContents::OnHoverEvent(
    /* [in] */ IMotionEvent* event)
{
    return mAwViewMethods->OnHoverEvent(event);
}

/**
 * @see android.view.View#onGenericMotionEvent()
 */
Boolean AwContents::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event)
{
    return mContentViewCore->OnGenericMotionEvent(event);
}

/**
 * @see android.view.View#onConfigurationChanged()
 */
ECode AwContents::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    mAwViewMethods->OnConfigurationChanged(newConfig);
    return NOERROR;
}

/**
 * @see android.view.View#onAttachedToWindow()
 */
void AwContents::OnAttachedToWindow()
{
    mTemporarilyDetached = FALSE;
    mAwViewMethods->OnAttachedToWindow();
}

/**
 * @see android.view.View#onDetachedFromWindow()
 */
//@SuppressLint("MissingSuperCall")
void AwContents::OnDetachedFromWindow()
{
    mAwViewMethods->OnDetachedFromWindow();
}

/**
 * @see android.view.View#onWindowFocusChanged()
 */
void AwContents::OnWindowFocusChanged(
    /* [in] */ Boolean hasWindowFocus)
{
    mAwViewMethods->OnWindowFocusChanged(hasWindowFocus);
}

/**
 * @see android.view.View#onFocusChanged()
 */
void AwContents::OnFocusChanged(
    /* [in] */ Boolean focused,
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    if (!mTemporarilyDetached) {
        mAwViewMethods->OnFocusChanged(focused, direction, previouslyFocusedRect);
    }
}

/**
 * @see android.view.View#onStartTemporaryDetach()
 */
void AwContents::OnStartTemporaryDetach()
{
    mTemporarilyDetached = TRUE;
}

/**
 * @see android.view.View#onFinishTemporaryDetach()
 */
void AwContents::OnFinishTemporaryDetach()
{
    mTemporarilyDetached = FALSE;
}

/**
 * @see android.view.View#onSizeChanged()
 */
void AwContents::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 ow,
    /* [in] */ Int32 oh)
{
    mAwViewMethods->OnSizeChanged(w, h, ow, oh);
}

/**
 * @see android.view.View#onVisibilityChanged()
 */
void AwContents::OnVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 visibility)
{
    mAwViewMethods->OnVisibilityChanged(changedView, visibility);
}

/**
 * @see android.view.View#onWindowVisibilityChanged()
 */
void AwContents::OnWindowVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    mAwViewMethods->OnWindowVisibilityChanged(visibility);
}

void AwContents::SetViewVisibilityInternal(
    /* [in] */ Boolean visible)
{
    mIsViewVisible = visible;
    if (mNativeAwContents == 0) return;
    NativeSetViewVisibility(mNativeAwContents, mIsViewVisible);
}

void AwContents::SetWindowVisibilityInternal(
    /* [in] */ Boolean visible)
{
    mIsWindowVisible = visible;
    if (mNativeAwContents == 0) return;
    NativeSetWindowVisibility(mNativeAwContents, mIsWindowVisible);
}

/**
 * Save the state of this AwContents into provided Bundle.
 * @return False if saving state failed.
 */
Boolean AwContents::SaveState(
    /* [in] */ IBundle* outState)
{
    if (mNativeAwContents == 0 || outState == NULL) return FALSE;

    AutoPtr<ArrayOf<Byte> > state = NativeGetOpaqueState(mNativeAwContents);
    if (state == NULL) return FALSE;

    outState->PutByteArray(SAVE_RESTORE_STATE_KEY, state);
    return TRUE;
}

/**
 * Restore the state of this AwContents into provided Bundle.
 * @param inState Must be a bundle returned by saveState.
 * @return False if restoring state failed.
 */
Boolean AwContents::RestoreState(
    /* [in] */ IBundle* inState)
{
    if (mNativeAwContents == 0 || inState == NULL) return FALSE;

    AutoPtr<ArrayOf<Byte> > state;
    inState->GetByteArray(SAVE_RESTORE_STATE_KEY, (ArrayOf<Byte>**)&state);
    if (state == NULL) return FALSE;

    Boolean result = NativeRestoreFromOpaqueState(mNativeAwContents, state);

    // The onUpdateTitle callback normally happens when a page is loaded,
    // but is optimized out in the restoreState case because the title is
    // already restored. See WebContentsImpl::UpdateTitleForEntry. So we
    // call the callback explicitly here.
    if (result) mContentsClient->OnReceivedTitle(mContentViewCore->GetTitle());

    return result;
}

/**
 * @see ContentViewCore#addPossiblyUnsafeJavascriptInterface(Object, String, Class)
 */
void AwContents::AddPossiblyUnsafeJavascriptInterface(
    /* [in] */ Object* object,
    /* [in] */ const String& name,
    /* [in] */ IInterface* requiredAnnotation)
{
    mContentViewCore->AddPossiblyUnsafeJavascriptInterface(TO_IINTERFACE(object), name, requiredAnnotation);
}

/**
 * @see android.webkit.WebView#removeJavascriptInterface(String)
 */
void AwContents::RemoveJavascriptInterface(
    /* [in] */ const String& interfaceName)
{
    mContentViewCore->RemoveJavascriptInterface(interfaceName);
}

/**
 * If native accessibility (not script injection) is enabled, and if this is
 * running on JellyBean or later, returns an AccessibilityNodeProvider that
 * implements native accessibility for this view. Returns null otherwise.
 * @return The AccessibilityNodeProvider, if available, or null otherwise.
 */
AutoPtr<IAccessibilityNodeProvider> AwContents::GetAccessibilityNodeProvider()
{
    return mContentViewCore->GetAccessibilityNodeProvider();
}

/**
 * @see android.webkit.WebView#onInitializeAccessibilityNodeInfo(AccessibilityNodeInfo)
 */
void AwContents::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    mContentViewCore->OnInitializeAccessibilityNodeInfo(info);
}

/**
 * @see android.webkit.WebView#onInitializeAccessibilityEvent(AccessibilityEvent)
 */
void AwContents::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    mContentViewCore->OnInitializeAccessibilityEvent(event);
}

Boolean AwContents::SupportsAccessibilityAction(
    /* [in] */ Int32 action)
{
    return mContentViewCore->SupportsAccessibilityAction(action);
}

/**
 * @see android.webkit.WebView#performAccessibilityAction(int, Bundle)
 */
Boolean AwContents::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments)
{
    return mContentViewCore->PerformAccessibilityAction(action, arguments);
}

/**
 * @see android.webkit.WebView#clearFormData()
 */
void AwContents::HideAutofillPopup()
{
    if (mAwAutofillClient != NULL) {
        mAwAutofillClient->HideAutofillPopup();
    }
}

void AwContents::SetNetworkAvailable(
    /* [in] */ Boolean networkUp)
{
    if (mNativeAwContents == 0) return;
    NativeSetJsOnlineProperty(mNativeAwContents, networkUp);
}

//--------------------------------------------------------------------------------------------
//  Methods called from native via JNI
//--------------------------------------------------------------------------------------------

//@CalledByNative
void AwContents::OnDocumentHasImagesResponse(
    /* [in] */ Boolean result,
    /* [in] */ IMessage* message)
{
    message->SetArg1(result ? 1 : 0);
    message->SendToTarget();
}

//@CalledByNative
void AwContents::OnReceivedTouchIconUrl(
    /* [in] */ const String& url,
    /* [in] */ Boolean precomposed)
{
    mContentsClient->OnReceivedTouchIconUrl(url, precomposed);
}

//@CalledByNative
void AwContents::OnReceivedIcon(
    /* [in] */ IBitmap* bitmap)
{
    mContentsClient->OnReceivedIcon(bitmap);
    mFavicon = bitmap;
}

/** Callback for generateMHTML. */
//@CalledByNative
void AwContents::GenerateMHTMLCallback(
    /* [in] */ const String& path,
    /* [in] */ Int64 size,
    /* [in] */ IInterface* callback)
{
    if (callback == NULL) return;
    AutoPtr<ICharSequence> icsq;
    CString::New(path, (ICharSequence**)&icsq);
    AutoPtr<IInterface> iPath = TO_IINTERFACE(icsq);
    AutoPtr<IValueCallback> vcb = IValueCallback::Probe(callback);
    vcb->OnReceiveValue(size < 0 ? NULL : iPath);
}

//@CalledByNative
void AwContents::OnReceivedHttpAuthRequest(
    /* [in] */ AwHttpAuthHandler* handler,
    /* [in] */ const String& host,
    /* [in] */ const String& realm)
{
    mContentsClient->OnReceivedHttpAuthRequest(handler, host, realm);
}

//@CalledByNative
void AwContents::OnGeolocationPermissionsShowPrompt(
    /* [in] */ const String& origin)
{
    if (mNativeAwContents == 0) return;
    AutoPtr<AwGeolocationPermissions> permissions = mBrowserContext->GetGeolocationPermissions();
    // Reject if geoloaction is disabled, or the origin has a retained deny
    if (!mSettings->GetGeolocationEnabled()) {
        NativeInvokeGeolocationCallback(mNativeAwContents, FALSE, origin);
        return;
    }
    // Allow if the origin has a retained allow
    if (permissions->HasOrigin(origin)) {
        NativeInvokeGeolocationCallback(mNativeAwContents, permissions->IsOriginAllowed(origin),
                origin);
        return;
    }
    AutoPtr<AwGeolocationCallback> callback =  new AwGeolocationCallback(this);
    mContentsClient->OnGeolocationPermissionsShowPrompt(origin, callback);
}

//@CalledByNative
void AwContents::OnGeolocationPermissionsHidePrompt()
{
    mContentsClient->OnGeolocationPermissionsHidePrompt();
}

//@CalledByNative
void AwContents::OnPermissionRequest(
    /* [in] */ AwPermissionRequest* awPermissionRequest)
{
    mContentsClient->OnPermissionRequest(awPermissionRequest);
}

//@CalledByNative
void AwContents::OnPermissionRequestCanceled(
    /* [in] */ AwPermissionRequest* awPermissionRequest)
{
    mContentsClient->OnPermissionRequestCanceled(awPermissionRequest);
}

//@CalledByNative
void AwContents::OnFindResultReceived(
    /* [in] */ Int32 activeMatchOrdinal,
    /* [in] */ Int32 numberOfMatches,
    /* [in] */ Boolean isDoneCounting)
{
    mContentsClient->OnFindResultReceived(activeMatchOrdinal, numberOfMatches, isDoneCounting);
}

//@CalledByNative
void AwContents::OnNewPicture()
{
    // Don't call capturePicture() here but instead defer it until the posted task runs within
    // the callback helper, to avoid doubling back into the renderer compositor in the middle
    // of the notification it is sending up to here.
    mContentsClient->GetCallbackHelper()->PostOnNewPicture(mPictureListenerContentProvider);
}

// Called as a result of nativeUpdateLastHitTestData.
//@CalledByNative
void AwContents::UpdateHitTestData(
    /* [in] */ Int32 type,
    /* [in] */ const String& extra,
    /* [in] */ const String& href,
    /* [in] */ const String& anchorText,
    /* [in] */ const String& imgSrc)
{
    mPossiblyStaleHitTestData->hitTestResultType = type;
    mPossiblyStaleHitTestData->hitTestResultExtraData = extra;
    mPossiblyStaleHitTestData->href = href;
    mPossiblyStaleHitTestData->anchorText = anchorText;
    mPossiblyStaleHitTestData->imgSrc = imgSrc;
}

//@CalledByNative
Boolean AwContents::RequestDrawGL(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Boolean waitForCompletion)
{
    AutoPtr<IView> view = IView::Probe(mContainerView);
    return mNativeGLDelegate->RequestDrawGL(canvas, waitForCompletion, view);
}

//@CalledByNative
void AwContents::PostInvalidateOnAnimation()
{
    AutoPtr<IView> view = IView::Probe(mContainerView);
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN) {
        view->PostInvalidateOnAnimation();
    }
    else {
        view->PostInvalidate();
    }
}

// Call postInvalidateOnAnimation for invalidations. This is only used to synchronize
// draw functor destruction.
//@CalledByNative
void AwContents::InvalidateOnFunctorDestroy()
{
    AutoPtr<IView> view = IView::Probe(mContainerView);
    view->Invalidate();
}

//@CalledByNative
AutoPtr<ArrayOf<Int32> > AwContents::GetLocationOnScreen()
{
    AutoPtr< ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(2);
    AutoPtr<IView> view = IView::Probe(mContainerView);
    view->GetLocationOnScreen((ArrayOf<Int32>*)result);
    return result;
}

//@CalledByNative
void AwContents::OnWebLayoutPageScaleFactorChanged(
    /* [in] */ Float webLayoutPageScaleFactor)
{
    // This change notification comes from the renderer thread, not from the cc/ impl thread.
    mLayoutSizer->OnPageScaleChanged(webLayoutPageScaleFactor);
}

//@CalledByNative
void AwContents::OnWebLayoutContentsSizeChanged(
    /* [in] */ Int32 widthCss,
    /* [in] */ Int32 heightCss)
{
    // This change notification comes from the renderer thread, not from the cc/ impl thread.
    mLayoutSizer->OnContentSizeChanged(widthCss, heightCss);
}

//@CalledByNative
void AwContents::ScrollContainerViewTo(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    mScrollOffsetManager->ScrollContainerViewTo(x, y);
}

//@CalledByNative
Boolean AwContents::IsFlingActive()
{
    return mScrollOffsetManager->IsFlingActive();
}

//@CalledByNative
void AwContents::UpdateScrollState(
    /* [in] */ Int32 maxContainerViewScrollOffsetX,
    /* [in] */ Int32 maxContainerViewScrollOffsetY,
    /* [in] */ Int32 contentWidthDip,
    /* [in] */ Int32 contentHeightDip,
    /* [in] */ Float pageScaleFactor,
    /* [in] */ Float minPageScaleFactor,
    /* [in] */ Float maxPageScaleFactor)
{
    mContentWidthDip = contentWidthDip;
    mContentHeightDip = contentHeightDip;
    mScrollOffsetManager->SetMaxScrollOffset(maxContainerViewScrollOffsetX,
        maxContainerViewScrollOffsetY);
    SetPageScaleFactorAndLimits(pageScaleFactor, minPageScaleFactor, maxPageScaleFactor);
}

//@CalledByNative
void AwContents::SetAwAutofillClient(
    /* [in] */ AwAutofillClient* client)
{
    mAwAutofillClient = client;
    client->Init(mContentViewCore);
}

//@CalledByNative
void AwContents::DidOverscroll(
    /* [in] */ Int32 deltaX,
    /* [in] */ Int32 deltaY)
{
    if (mOverScrollGlow != NULL) {
        mOverScrollGlow->SetOverScrollDeltas(deltaX, deltaY);
    }

    mScrollOffsetManager->OverScrollBy(deltaX, deltaY);

    if (mOverScrollGlow != NULL && mOverScrollGlow->IsAnimating()) {
        AutoPtr<IView> view = IView::Probe(mContainerView);
        view->Invalidate();
    }
}

// -------------------------------------------------------------------------------------------
// Helper methods
// -------------------------------------------------------------------------------------------

void AwContents::SetPageScaleFactorAndLimits(
    /* [in] */ Float pageScaleFactor,
    /* [in] */ Float minPageScaleFactor,
    /* [in] */ Float maxPageScaleFactor)
{
    if (mPageScaleFactor == pageScaleFactor &&
            mMinPageScaleFactor == minPageScaleFactor &&
            mMaxPageScaleFactor == maxPageScaleFactor) {
        return;
    }
    mMinPageScaleFactor = minPageScaleFactor;
    mMaxPageScaleFactor = maxPageScaleFactor;
    if (mPageScaleFactor != pageScaleFactor) {
        Float oldPageScaleFactor = mPageScaleFactor;
        mPageScaleFactor = pageScaleFactor;
        mContentsClient->GetCallbackHelper()->PostOnScaleChangedScaled(
                (Float)(oldPageScaleFactor * mDIPScale),
                (Float)(mPageScaleFactor * mDIPScale));
    }
}

void AwContents::SaveWebArchiveInternal(
    /* [in] */ const String& path,
    /* [in] */ IValueCallback* callback)
{
    if (path.IsNull() || mNativeAwContents == 0) {
        AutoPtr<IRunnable> runnable = new SaveWebArchiveInternalRunnable(this, callback);
        ThreadUtils::RunOnUiThread(runnable);
    }
    else {
        NativeGenerateMHTML(mNativeAwContents, path, callback);
    }
}

/**
 * Try to generate a pathname for saving an MHTML archive. This roughly follows WebView's
 * autoname logic.
 */
String AwContents::GenerateArchiveAutoNamePath(
    /* [in] */ const String& originalUrl,
    /* [in] */ const String& baseName)
{
    String name;
    if (!originalUrl.IsNullOrEmpty()) {
        //try {
            AutoPtr<IURL> url;
            CURL::New(originalUrl, (IURL**)&url);
            String path;
            url->GetPath(&path);
            Int32 lastSlash = path.LastIndexOf('/');
            if (lastSlash > 0) {
                name = path.Substring(lastSlash + 1);
            }
            else {
                name = path;
            }
        //} catch (MalformedURLException e) {
            // If it fails parsing the URL, we'll just rely on the default name below.
        //}
    }

    if (TextUtils::IsEmpty(name)) name = "index";
    if (name.IsNullOrEmpty()) name = "index";
    String testName = baseName + name + WEB_ARCHIVE_EXTENSION;
    AutoPtr<IFile> file;
    CFile::New(testName, (IFile**)&file);
    Boolean bExists = FALSE;
    file->Exists(&bExists);
    if (!bExists) return testName;

    for (Int32 i = 1; i < 100; i++) {
        String iStr = StringUtils::ToString(i);
        testName = baseName + name + "-" + iStr + WEB_ARCHIVE_EXTENSION;
        file = NULL;
        CFile::New(testName, (IFile**)&file);
        Boolean bExists = FALSE;
        file->Exists(&bExists);
        if (!bExists) return testName;
    }

    Logger::E(TAG, "Unable to auto generate archive name for path: %s", baseName.string());
    return String(NULL);
}

//@Override
ECode AwContents::ExtractSmartClipData(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mContentViewCore->ExtractSmartClipData(x, y, width, height);
    return NOERROR;
}

//@Override
ECode AwContents::SetSmartClipResultHandler(
    /* [in] */ IHandler* resultHandler)
{
    if (resultHandler == NULL) {
        mContentViewCore->SetSmartClipDataListener(NULL);
        return NOERROR;
    }

    AutoPtr<ContentViewCore::SmartClipDataListener> listener = new InnerSmartClipDataListener(this, resultHandler);
    mContentViewCore->SetSmartClipDataListener(listener);
    return NOERROR;
}

// Return true if the GeolocationPermissionAPI should be used.
//@CalledByNative
Boolean AwContents::UseLegacyGeolocationPermissionAPI()
{
    // Always return true since we are not ready to swap the geolocation yet.
    // TODO: If we decide not to migrate the geolocation, there are some unreachable
    // code need to remove. http://crbug.com/396184.
    return TRUE;
}

//--------------------------------------------------------------------------------------------
//  Native methods
//--------------------------------------------------------------------------------------------

Handle64 AwContents::NativeInit(
    /* [in] */ AwBrowserContext* browserContext)
{
    return Elastos_AwContents_nativeInit(TO_IINTERFACE(browserContext));
}

void AwContents::NativeDestroy(
    /* [in] */ Handle64 nativeAwContents)
{
    Elastos_AwContents_nativeDestroy(nativeAwContents);
}

void AwContents::NativeSetAwDrawSWFunctionTable(
    /* [in] */ Handle64 functionTablePointer)
{
    Elastos_AwContents_nativeSetAwDrawSWFunctionTable(functionTablePointer);
}

void AwContents::NativeSetAwDrawGLFunctionTable(
    /* [in] */ Handle64 functionTablePointer)
{
    Elastos_AwContents_nativeSetAwDrawGLFunctionTable(functionTablePointer);
}

Handle64 AwContents::NativeGetAwDrawGLFunction()
{
    return Elastos_AwContents_nativeGetAwDrawGLFunction();
}

Int32 AwContents::NativeGetNativeInstanceCount()
{
    return Elastos_AwContents_nativeGetNativeInstanceCount();
}

void AwContents::NativeSetShouldDownloadFavicons()
{
    Elastos_AwContents_nativeSetShouldDownloadFavicons();
}

void AwContents::NativeSetJavaPeers(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ AwContents* awContents,
    /* [in] */ AwWebContentsDelegate* webViewWebContentsDelegate,
    /* [in] */ AwContentsClientBridge* contentsClientBridge,
    /* [in] */ AwContentsIoThreadClient* ioThreadClient,
    /* [in] */ InterceptNavigationDelegate* navigationInterceptionDelegate)
{
    Elastos_AwContents_nativeSetJavaPeers(TO_IINTERFACE(this), nativeAwContents,
            TO_IINTERFACE(awContents),
            TO_IINTERFACE(webViewWebContentsDelegate),
            TO_IINTERFACE(contentsClientBridge),
            TO_IINTERFACE(ioThreadClient),
            TO_IINTERFACE(navigationInterceptionDelegate));
}

Handle64 AwContents::NativeGetWebContents(
    /* [in] */ Handle64 nativeAwContents)
{
    return Elastos_AwContents_nativeGetWebContents(TO_IINTERFACE(this), nativeAwContents);
}

void AwContents::NativeDocumentHasImages(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ IMessage* message)
{
    Elastos_AwContents_nativeDocumentHasImages(TO_IINTERFACE(this), nativeAwContents,
            TO_IINTERFACE(message));
}

void AwContents::NativeGenerateMHTML(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ const String& path,
    /* [in] */ IValueCallback* callback)
{
    Elastos_AwContents_nativeGenerateMHTML(TO_IINTERFACE(this), nativeAwContents, path, callback);
}

void AwContents::NativeAddVisitedLinks(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ ArrayOf<String>* visitedLinks)
{
    Elastos_AwContents_nativeAddVisitedLinks(TO_IINTERFACE(this), nativeAwContents, visitedLinks);
}

Boolean AwContents::NativeOnDraw(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ ICanvas* canvas,
    /* [in] */ Boolean isHardwareAccelerated,
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY,
    /* [in] */ Int32 visibleLeft,
    /* [in] */ Int32 visibleTop,
    /* [in] */ Int32 visibleRight,
    /* [in] */ Int32 visibleBottom)
{
    return Elastos_AwContents_nativeOnDraw(TO_IINTERFACE(this), nativeAwContents, TO_IINTERFACE(canvas),
            isHardwareAccelerated, scrollX, scrollY, visibleLeft, visibleTop, visibleRight, visibleBottom);
}

void AwContents::NativeFindAllAsync(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ const String& searchString)
{
    Elastos_AwContents_nativeFindAllAsync(TO_IINTERFACE(this), nativeAwContents, searchString);
}

void AwContents::NativeFindNext(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ Boolean forward)
{
    Elastos_AwContents_nativeFindNext(TO_IINTERFACE(this), nativeAwContents, forward);
}

void AwContents::NativeClearMatches(
    /* [in] */ Handle64 nativeAwContents)
{
    Elastos_AwContents_nativeClearMatches(TO_IINTERFACE(this), nativeAwContents);
}

void AwContents::NativeClearCache(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ Boolean includeDiskFiles)
{
    Elastos_AwContents_nativeClearCache(TO_IINTERFACE(this), nativeAwContents, includeDiskFiles);
}

AutoPtr< ArrayOf<Byte> > AwContents::NativeGetCertificate(
    /* [in] */ Handle64 nativeAwContents)
{
    return Elastos_AwContents_nativeGetCertificate(TO_IINTERFACE(this), nativeAwContents);
}

// Coordinates in desity independent pixels.
void AwContents::NativeRequestNewHitTestDataAt(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Elastos_AwContents_nativeRequestNewHitTestDataAt(TO_IINTERFACE(this), nativeAwContents, x, y);
}

void AwContents::NativeUpdateLastHitTestData(
    /* [in] */ Handle64 nativeAwContents)
{
    Elastos_AwContents_nativeUpdateLastHitTestData(TO_IINTERFACE(this), nativeAwContents);
}

void AwContents::NativeOnSizeChanged(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */  Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 ow,
    /* [in] */ Int32 oh)
{
    Elastos_AwContents_nativeOnSizeChanged(TO_IINTERFACE(this), nativeAwContents, w, h, ow, oh);
}

void AwContents::NativeScrollTo(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Elastos_AwContents_nativeScrollTo(TO_IINTERFACE(this), nativeAwContents, x, y);
}

void AwContents::NativeSetViewVisibility(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ Boolean visible)
{
    Elastos_AwContents_nativeSetViewVisibility(TO_IINTERFACE(this), nativeAwContents, visible);
}

void AwContents::NativeSetWindowVisibility(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ Boolean visible)
{
    Elastos_AwContents_nativeSetWindowVisibility(TO_IINTERFACE(this), nativeAwContents, visible);
}

void AwContents::NativeSetIsPaused(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ Boolean paused)
{
    Elastos_AwContents_nativeSetIsPaused(TO_IINTERFACE(this), nativeAwContents, paused);
}

void AwContents::NativeOnAttachedToWindow(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    Elastos_AwContents_nativeOnAttachedToWindow(TO_IINTERFACE(this), nativeAwContents, w, h);
}

void AwContents::NativeOnDetachedFromWindow(
    /* [in] */ Handle64 nativeAwContents)
{
    Elastos_AwContents_nativeOnDetachedFromWindow(nativeAwContents);
}

void AwContents::NativeSetDipScale(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ Float dipScale)
{
    Elastos_AwContents_nativeSetDipScale(TO_IINTERFACE(this), nativeAwContents, dipScale);
}

// Returns null if save state fails.
AutoPtr< ArrayOf<Byte> > AwContents::NativeGetOpaqueState(
    /* [in] */ Handle64 nativeAwContents)
{
    return Elastos_AwContents_nativeGetOpaqueState(TO_IINTERFACE(this), nativeAwContents);
}

// Returns false if restore state fails.
Boolean AwContents::NativeRestoreFromOpaqueState(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ ArrayOf<Byte>* state)
{
    return Elastos_AwContents_nativeRestoreFromOpaqueState(TO_IINTERFACE(this), nativeAwContents, state);
}

Handle64 AwContents::NativeReleasePopupAwContents(
    /* [in] */ Handle64 nativeAwContents)
{
    return Elastos_AwContents_nativeReleasePopupAwContents(TO_IINTERFACE(this), nativeAwContents);
}

void AwContents::NativeFocusFirstNode(
    /* [in] */ Handle64 nativeAwContents)
{
    Elastos_AwContents_nativeFocusFirstNode(TO_IINTERFACE(this), nativeAwContents);
}

void AwContents::NativeSetBackgroundColor(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ Int32 color)
{
    Elastos_AwContents_nativeSetBackgroundColor(TO_IINTERFACE(this), nativeAwContents, color);
}

Handle64 AwContents::NativeGetAwDrawGLViewContext(
    /* [in] */ Handle64 nativeAwContents)
{
    return Elastos_AwContents_nativeGetAwDrawGLViewContext(TO_IINTERFACE(this), nativeAwContents);
}

Handle64 AwContents::NativeCapturePicture(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return Elastos_AwContents_nativeCapturePicture(TO_IINTERFACE(this), nativeAwContents, width, height);
}

void AwContents::NativeEnableOnNewPicture(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ Boolean enabled)
{
    Elastos_AwContents_nativeEnableOnNewPicture(TO_IINTERFACE(this), nativeAwContents, enabled);
}

void AwContents::NativeClearView(
    /* [in] */ Handle64 nativeAwContents)
{
    Elastos_AwContents_nativeClearView(TO_IINTERFACE(this), nativeAwContents);
}

void AwContents::NativeSetExtraHeadersForUrl(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ const String& url,
    /* [in] */ const String& extraHeaders)
{
    Elastos_AwContents_nativeSetExtraHeadersForUrl(TO_IINTERFACE(this), nativeAwContents, url, extraHeaders);
}

void AwContents::NativeInvokeGeolocationCallback(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ Boolean value,
    /* [in] */ const String& requestingFrame)
{
    Elastos_AwContents_nativeInvokeGeolocationCallback(TO_IINTERFACE(this), nativeAwContents, value, requestingFrame);
}

void AwContents::NativeSetJsOnlineProperty(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ Boolean networkUp)
{
    Elastos_AwContents_nativeSetJsOnlineProperty(TO_IINTERFACE(this), nativeAwContents, networkUp);
}

void AwContents::NativeTrimMemory(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ Int32 level,
    /* [in] */ Boolean visible)
{
    Elastos_AwContents_nativeTrimMemory(TO_IINTERFACE(this), nativeAwContents, level, visible);
}

void AwContents::NativeCreatePdfExporter(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ AwPdfExporter* awPdfExporter)
{
    Elastos_AwContents_nativeCreatePdfExporter(TO_IINTERFACE(this), nativeAwContents, TO_IINTERFACE(awPdfExporter));
}

void AwContents::NativePreauthorizePermission(
    /* [in] */ Handle64 nativeAwContents,
    /* [in] */ const String& origin,
    /* [in] */ Int64 resources)
{
    Elastos_AwContents_nativePreauthorizePermission(TO_IINTERFACE(this), nativeAwContents, origin, resources);
}
//callback function definition
void AwContents::OnDocumentHasImagesResponse(
    /* [in] */ Boolean result,
    /* [in] */ IInterface* message)
{
    AutoPtr<IMessage> m = IMessage::Probe(message);
    OnDocumentHasImagesResponse(result, m);
}

void AwContents::OnReceivedTouchIconUrl(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& url,
    /* [in] */ Boolean precomposed)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::OnReceivedTouchIconUrl, mObj is NULL");
        return;
    }
    mObj->OnReceivedTouchIconUrl(url, precomposed);
}

void AwContents::OnReceivedIcon(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* bitmap)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::OnReceivedIcon, mObj is NULL");
        return;
    }
    AutoPtr<IBitmap> b = IBitmap::Probe(bitmap);
    mObj->OnReceivedIcon(b);
}

void AwContents::OnReceivedHttpAuthRequest(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* handler,
    /* [in] */ const String& host,
    /* [in] */ const String& realm)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::OnReceivedHttpAuthRequest, mObj is NULL");
        return;
    }
    AutoPtr<AwHttpAuthHandler> hah = (AwHttpAuthHandler*)(IObject::Probe(handler));
    mObj->OnReceivedHttpAuthRequest(hah, host, realm);
}

void AwContents::OnGeolocationPermissionsShowPrompt(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& origin)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::OnGeolocationPermissionsShowPrompt, mObj is NULL");
        return;
    }
    mObj->OnGeolocationPermissionsShowPrompt(origin);
}

void AwContents::OnGeolocationPermissionsHidePrompt(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::OnGeolocationPermissionsHidePrompt, mObj is NULL");
        return;
    }
    mObj->OnGeolocationPermissionsHidePrompt();
}

void AwContents::OnPermissionRequest(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* awPermissionRequest)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::OnPermissionRequest, mObj is NULL");
        return;
    }
    AutoPtr<AwPermissionRequest> pr = (AwPermissionRequest*)(IObject::Probe(awPermissionRequest));
    mObj->OnPermissionRequest(pr);
}

void AwContents::OnPermissionRequestCanceled(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* awPermissionRequest)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::OnPermissionRequestCanceled, mObj is NULL");
        return;
    }
    AutoPtr<AwPermissionRequest> pr = (AwPermissionRequest*)(IObject::Probe(awPermissionRequest));
    mObj->OnPermissionRequestCanceled(pr);
}

void AwContents::OnFindResultReceived(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 activeMatchOrdinal,
    /* [in] */ Int32 numberOfMatches,
    /* [in] */ Boolean isDoneCounting)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::OnFindResultReceived, mObj is NULL");
        return;
    }
    mObj->OnFindResultReceived(activeMatchOrdinal, numberOfMatches, isDoneCounting);
}

void AwContents::OnNewPicture(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::OnNewPicture, mObj is NULL");
        return;
    }
    mObj->OnNewPicture();
}

void AwContents::UpdateHitTestData(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 type,
    /* [in] */ const String& extra,
    /* [in] */ const String& href,
    /* [in] */ const String& anchorText,
    /* [in] */ const String& imgSrc)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::UpdateHitTestData, mObj is NULL");
        return;
    }
    mObj->UpdateHitTestData(type, extra, href, anchorText, imgSrc);
}

Boolean AwContents::RequestDrawGL(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* canvas,
    /* [in] */ Boolean waitForCompletion)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::RequestDrawGL, mObj is NULL");
        return FALSE;
    }
    AutoPtr<ICanvas> c = ICanvas::Probe(canvas);
    return mObj->RequestDrawGL(c, waitForCompletion);
}

void AwContents::PostInvalidateOnAnimation(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::PostInvalidateOnAnimation, mObj is NULL");
        return;
    }
    mObj->PostInvalidateOnAnimation();
}

void AwContents::InvalidateOnFunctorDestroy(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::InvalidateOnFunctorDestroy, mObj is NULL");
        return;
    }
    mObj->InvalidateOnFunctorDestroy();
}

AutoPtr<ArrayOf<Int32> > AwContents::GetLocationOnScreen(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::GetLocationOnScreen, mObj is NULL");
        return NULL;
    }
    return mObj->GetLocationOnScreen();
}

void AwContents::OnWebLayoutPageScaleFactorChanged(
    /* [in] */ IInterface* obj,
    /* [in] */ Float webLayoutPageScaleFactor)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::OnWebLayoutPageScaleFactorChanged, mObj is NULL");
        return;
    }
    mObj->OnWebLayoutPageScaleFactorChanged(webLayoutPageScaleFactor);
}

void AwContents::OnWebLayoutContentsSizeChanged(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 widthCss,
    /* [in] */ Int32 heightCss)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::OnWebLayoutContentsSizeChanged, mObj is NULL");
        return;
    }
    mObj->OnWebLayoutContentsSizeChanged(widthCss, heightCss);
}

void AwContents::ScrollContainerViewTo(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::ScrollContainerViewTo, mObj is NULL");
        return;
    }
    mObj->ScrollContainerViewTo(x, y);
}

Boolean AwContents::IsFlingActive(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::IsFlingActive, mObj is NULL");
        return FALSE;
    }
    return mObj->IsFlingActive();
}

void AwContents::UpdateScrollState(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 maxContainerViewScrollOffsetX,
    /* [in] */ Int32 maxContainerViewScrollOffsetY,
    /* [in] */ Int32 contentWidthDip,
    /* [in] */ Int32 contentHeightDip,
    /* [in] */ Float pageScaleFactor,
    /* [in] */ Float minPageScaleFactor,
    /* [in] */ Float maxPageScaleFactor)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::UpdateScrollState, mObj is NULL");
        return;
    }
    mObj->UpdateScrollState(maxContainerViewScrollOffsetX, maxContainerViewScrollOffsetY, contentWidthDip, contentHeightDip, pageScaleFactor, minPageScaleFactor, maxPageScaleFactor);
}

void AwContents::SetAwAutofillClient(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* client)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::SetAwAutofillClient, mObj is NULL");
        return;
    }
    AutoPtr<AwAutofillClient> c = (AwAutofillClient*)(IObject::Probe(client));
    mObj->SetAwAutofillClient(c);
}

void AwContents::DidOverscroll(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 deltaX,
    /* [in] */ Int32 deltaY)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::DidOverscroll, mObj is NULL");
        return;
    }
    mObj->DidOverscroll(deltaX, deltaY);
}

Boolean AwContents::UseLegacyGeolocationPermissionAPI(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwContents> mObj = (AwContents*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContents::UseLegacyGeolocationPermissionAPI, mObj is NULL");
        return FALSE;
    }
    return mObj->UseLegacyGeolocationPermissionAPI();
}


} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
