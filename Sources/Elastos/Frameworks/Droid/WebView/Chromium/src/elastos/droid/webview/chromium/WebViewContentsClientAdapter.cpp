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

#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Webkit.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Security.h>
//#include "elastos/droid/content/CIntentHelper.h"
//#include "elastos/droid/graphics/CBitmapFactory.h"
//#include "elastos/droid/graphics/CCanvas.h"
//#include "elastos/droid/graphics/CPicture.h"
//#include "elastos/droid/net/CUriHelper.h"
//#include "elastos/droid/net/http/CErrorStrings.h"
//#include "elastos/droid/webkit/CJsDialogHelper.h"
#include "elastos/droid/webkit/DebugFlags.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/base/TraceEvent.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewClient.h"
#include "elastos/droid/webkit/webview/chromium/FileChooserParamsAdapter.h"
#include "elastos/droid/webkit/webview/chromium/R_Chromium.h"
#include "elastos/droid/webkit/webview/chromium/UnimplementedWebViewApi.h"
#include "elastos/droid/webkit/webview/chromium/WebViewChromium.h"
#include "elastos/droid/webkit/webview/chromium/WebViewContentsClientAdapter.h"
#include "elastos/core/AutoLock.h"
#include "elastos/core/CoreUtils.h"
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Graphics::BitmapConfig;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CPicture;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::Http::CErrorStrings;
using Elastos::Droid::Net::Http::IErrorStrings;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Provider::IBrowser;
using Elastos::Droid::Webkit::CJsDialogHelper;
using Elastos::Droid::Webkit::DebugFlags;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;
using Elastos::Droid::Webkit::Webview::Chromium::Base::TraceEvent;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::ContentViewClient;
using Elastos::Droid::Webkit::IJsDialogHelper;
using Elastos::Droid::Webkit::IWebBackForwardList;
using Elastos::Droid::Webkit::IWebResourceResponse;
using Elastos::Droid::Webkit::Webview::Chromium::FileChooserParamsAdapter;
using Elastos::Droid::Webkit::Webview::Chromium::UnimplementedWebViewApi;
using Elastos::Droid::Webkit::Webview::Chromium::WebViewChromium;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CWeakHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

//=====================================================================
//           WebViewContentsClientAdapter::NullWebViewClient
//=====================================================================
ECode WebViewContentsClientAdapter::NullWebViewClient::ShouldOverrideKeyEvent(
    /* [in] */ IWebView* view,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(view);
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // TODO: Investigate more and add a test case.
    // // This is reflecting Clank's behavior.
    // int keyCode = event.getKeyCode();
    // return !ContentViewClient.shouldPropagateKey(keyCode);

    Int32 keyCode = 0;
    event->GetKeyCode(&keyCode);
    *result = !ContentViewClient::ShouldPropagateKey(keyCode);
    return NOERROR;
}

ECode WebViewContentsClientAdapter::NullWebViewClient::ShouldOverrideUrlLoading(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(view);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Intent intent;
    // // Perform generic parsing of the URI to turn it into an Intent.
    // try {
    //     intent = Intent.parseUri(url, Intent.URI_INTENT_SCHEME);
    // } catch (URISyntaxException ex) {
    //     Log.w(TAG, "Bad URI " + url + ": " + ex.getMessage());
    //     return false;
    // }
    // // Sanitize the Intent, ensuring web pages can not bypass browser
    // // security (only access to BROWSABLE activities).
    // intent.addCategory(Intent.CATEGORY_BROWSABLE);
    // intent.setComponent(null);
    // Intent selector = intent.getSelector();
    // if (selector != null) {
    //     selector.addCategory(Intent.CATEGORY_BROWSABLE);
    //     selector.setComponent(null);
    // }
    // // Pass the package name as application ID so that the intent from the
    // // same application can be opened in the same tab.
    // intent.putExtra(Browser.EXTRA_APPLICATION_ID,
    //         view.getContext().getPackageName());
    // try {
    //     view.getContext().startActivity(intent);
    // } catch (ActivityNotFoundException ex) {
    //     Log.w(TAG, "No application can handle " + url);
    //     return false;
    // }
    // return true;

    AutoPtr<IIntentHelper> helper;
    CIntentHelper::AcquireSingleton((IIntentHelper**)&helper);
    AutoPtr<IIntent> intent;
    // Perform generic parsing of the URI to turn it into an Intent.
    // try {
        helper->ParseUri(url, IIntent::URI_INTENT_SCHEME, (IIntent**)&intent);
    // } catch (URISyntaxException ex) {
        // Logger::W(TAG, "Bad URI " + url + ": " + ex.getMessage());
        // return FALSE;
    // }
    // Sanitize the Intent, ensuring web pages can not bypass browser
    // security (only access to BROWSABLE activities).
    intent->AddCategory(IIntent::CATEGORY_BROWSABLE);
    intent->SetComponent(NULL);
    AutoPtr<IIntent> selector;
    intent->GetSelector((IIntent**)&selector);
    if (selector != NULL) {
        selector->AddCategory(IIntent::CATEGORY_BROWSABLE);
        selector->SetComponent(NULL);
    }

    // Pass the package name as application ID so that the intent from the
    // same application can be opened in the same tab.
    IView* viewTmp = IView::Probe(view);
    AutoPtr<IContext> context;
    viewTmp->GetContext((IContext**)&context);
    String packageName;
    context->GetPackageName(&packageName);
    intent->PutExtra(IBrowser::EXTRA_APPLICATION_ID, packageName);
    // try {
        context->StartActivity(intent);
    // } catch (ActivityNotFoundException ex) {
        // Logger::W(TAG, "No application can handle " + url);
        // return false;
    // }

    *result = TRUE;
    return NOERROR;
}

//=====================================================================
//        WebViewContentsClientAdapter::PermissionRequestAdapter
//=====================================================================
const Int64 WebViewContentsClientAdapter::PermissionRequestAdapter::BITMASK_RESOURCE_VIDEO_CAPTURE;
const Int64 WebViewContentsClientAdapter::PermissionRequestAdapter::BITMASK_RESOURCE_AUDIO_CAPTURE;
const Int64 WebViewContentsClientAdapter::PermissionRequestAdapter::BITMASK_RESOURCE_PROTECTED_MEDIA_ID;

CAR_INTERFACE_IMPL(WebViewContentsClientAdapter::PermissionRequestAdapter, Object, IPermissionRequest)

WebViewContentsClientAdapter::PermissionRequestAdapter::PermissionRequestAdapter(
    /* [in] */ AwPermissionRequest* awPermissionRequest)
{
    // ==================before translated======================
    // assert awPermissionRequest != null;
    // mAwPermissionRequest = awPermissionRequest;

    assert(awPermissionRequest != NULL);
    mAwPermissionRequest = awPermissionRequest;
}

Int64 WebViewContentsClientAdapter::PermissionRequestAdapter::ToAwPermissionResources(
    /* [in] */ ArrayOf<String>* resources)
{
    // ==================before translated======================
    // long result = 0;
    // for (const String& resource : resources) {
    //     if (resource.equals(PermissionRequest.RESOURCE_VIDEO_CAPTURE))
    //         result |= BITMASK_RESOURCE_VIDEO_CAPTURE;
    //     else if (resource.equals(PermissionRequest.RESOURCE_AUDIO_CAPTURE))
    //         result |= BITMASK_RESOURCE_AUDIO_CAPTURE;
    //     else if (resource.equals(PermissionRequest.RESOURCE_PROTECTED_MEDIA_ID))
    //         result |= BITMASK_RESOURCE_PROTECTED_MEDIA_ID;
    // }
    // return result;

    Int64 result = 0;
    String resource;
    for (Int32 idx=0; idx<resources->GetLength(); ++idx) {
        resource = (*resources)[idx];
        if (resource.Equals(IPermissionRequest::RESOURCE_VIDEO_CAPTURE))
            result |= BITMASK_RESOURCE_VIDEO_CAPTURE;
        else if (resource.Equals(IPermissionRequest::RESOURCE_AUDIO_CAPTURE))
            result |= BITMASK_RESOURCE_AUDIO_CAPTURE;
        else if (resource.Equals(IPermissionRequest::RESOURCE_PROTECTED_MEDIA_ID))
            result |= BITMASK_RESOURCE_PROTECTED_MEDIA_ID;
    }

    return result;
}

ECode WebViewContentsClientAdapter::PermissionRequestAdapter::GetOrigin(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAwPermissionRequest.getOrigin();

    AutoPtr<IUri> temp = mAwPermissionRequest->GetOrigin();
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WebViewContentsClientAdapter::PermissionRequestAdapter::GetResources(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // {    AutoLock syncLock(this);
    //     if (mResources == null) {
    //         mResources = toPermissionResources(mAwPermissionRequest.getResources());
    //     }
    //     return mResources;
    // }

    AutoLock lock(this);
    if (NULL == mResources) {
        mResources = ToPermissionResources(mAwPermissionRequest->GetResources());
    }

    *result = mResources;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WebViewContentsClientAdapter::PermissionRequestAdapter::Grant(
    /* [in] */ ArrayOf<String>* resources)
{
    VALIDATE_NOT_NULL(resources);
    // ==================before translated======================
    // long requestedResource = mAwPermissionRequest.getResources();
    // if ((requestedResource & toAwPermissionResources(resources)) == requestedResource)
    //     mAwPermissionRequest.grant();
    // else
    //     mAwPermissionRequest.deny();

    Int64 requestedResource = mAwPermissionRequest->GetResources();
    if ((requestedResource & ToAwPermissionResources(resources)) == requestedResource)
        return mAwPermissionRequest->Grant();
    else
        return mAwPermissionRequest->Deny();
}

ECode WebViewContentsClientAdapter::PermissionRequestAdapter::Deny()
{
    // ==================before translated======================
    // mAwPermissionRequest.deny();

    return mAwPermissionRequest->Deny();
}

AutoPtr< ArrayOf<String> > WebViewContentsClientAdapter::PermissionRequestAdapter::ToPermissionResources(
    /* [in] */ Int64 resources)
{
    // ==================before translated======================
    // ArrayList<String> result = new ArrayList<String>();
    // if ((resources & BITMASK_RESOURCE_VIDEO_CAPTURE) != 0)
    //     result.add(PermissionRequest.RESOURCE_VIDEO_CAPTURE);
    // if ((resources & BITMASK_RESOURCE_AUDIO_CAPTURE) != 0)
    //     result.add(PermissionRequest.RESOURCE_AUDIO_CAPTURE);
    // if ((resources & BITMASK_RESOURCE_PROTECTED_MEDIA_ID) != 0)
    //     result.add(PermissionRequest.RESOURCE_PROTECTED_MEDIA_ID);
    // const String&[] resource_array = new const String&[result.size()];
    // return result.toArray(resource_array);

    Int32 size = 0;
    String tmps[4];
    if ((resources & BITMASK_RESOURCE_VIDEO_CAPTURE) != 0)
        tmps[size++] = IPermissionRequest::RESOURCE_VIDEO_CAPTURE;
    if ((resources & BITMASK_RESOURCE_AUDIO_CAPTURE) != 0)
        tmps[size++] = IPermissionRequest::RESOURCE_AUDIO_CAPTURE;
    if ((resources & BITMASK_RESOURCE_PROTECTED_MEDIA_ID) != 0)
        tmps[size++] = IPermissionRequest::RESOURCE_PROTECTED_MEDIA_ID;

    AutoPtr< ArrayOf<String> > result = ArrayOf<String>::Alloc(size);
    for (Int32 idx=0; idx<size; ++idx) {
        result->Set(idx, tmps[idx]);
    }

    return result;
}

//=====================================================================
//             WebViewContentsClientAdapter::InnerHandler
//=====================================================================
WebViewContentsClientAdapter::InnerHandler::InnerHandler(
    /* [in] */ WebViewContentsClientAdapter* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewContentsClientAdapter::InnerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    // ==================before translated======================
    // switch(msg.what) {
    //     case NEW_WEBVIEW_CREATED:
    //         WebView.WebViewTransport t = (WebView.WebViewTransport) msg.obj;
    //         WebView newWebView = t.getWebView();
    //         if (newWebView == mWebView) {
    //             throw new IllegalArgumentException(
    //                     "Parent WebView cannot host it's own popup window. Please " +
    //                     "use WebSettings.setSupportMultipleWindows(false)");
    //         }
    //
    //         if (newWebView != null && newWebView.copyBackForwardList().getSize() != 0) {
    //             throw new IllegalArgumentException(
    //                     "New WebView for popup window must not have been previously " +
    //                     "navigated.");
    //         }
    //
    //         WebViewChromium.completeWindowCreation(mWebView, newWebView);
    //         break;
    //     default:
    //         throw new IllegalStateException();
    // }

    assert(NULL != mOwner);
    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case NEW_WEBVIEW_CREATED:
            {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                IWebViewTransport* t = IWebViewTransport::Probe(obj);

                AutoPtr<IWebView> newWebView;
                t->GetWebView((IWebView**)&newWebView);

                IInterface* webViewInterfaceTmp = TO_IINTERFACE(mOwner->mWebView);
                IInterface* newWebViewInterfaceTmp = TO_IINTERFACE(newWebView);
                if (newWebViewInterfaceTmp == webViewInterfaceTmp) {
                    //throw new IllegalArgumentException(String("Parent WebView cannot host it's own popup window. Please ") +
                    //    String("use WebSettings.setSupportMultipleWindows(false)"));
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }

                AutoPtr<IWebBackForwardList> forwardList;
                newWebView->CopyBackForwardList((IWebBackForwardList**)&forwardList);
                Int32 size = 0;
                forwardList->GetSize(&size);
                if (newWebView != NULL && size != 0) {
                    //throw new IllegalArgumentException(String("New WebView for popup window must not have been previously ") +
                    //    String("navigated."));
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }

                WebViewChromium::CompleteWindowCreation(mOwner->mWebView, newWebView);
            }
            break;
        default:
            //throw new IllegalStateException();
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

//=====================================================================
//         WebViewContentsClientAdapter::WebResourceRequestImpl
//=====================================================================
CAR_INTERFACE_IMPL(WebViewContentsClientAdapter::WebResourceRequestImpl, Object, IWebResourceRequest)

WebViewContentsClientAdapter::WebResourceRequestImpl::WebResourceRequestImpl(
    /* [in] */ ShouldInterceptRequestParams* params)
    : mParams(params)
{
    // ==================before translated======================
    // mParams = params;
}

ECode WebViewContentsClientAdapter::WebResourceRequestImpl::GetUrl(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return Uri.parse(mParams.url);

    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    helper->Parse(mParams->url, result);
    return NOERROR;
}

ECode WebViewContentsClientAdapter::WebResourceRequestImpl::IsForMainFrame(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mParams.isMainFrame;

    *result = mParams->isMainFrame;
    return NOERROR;
}

ECode WebViewContentsClientAdapter::WebResourceRequestImpl::HasGesture(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mParams.hasUserGesture;

    *result = mParams->hasUserGesture;
    return NOERROR;
}

ECode WebViewContentsClientAdapter::WebResourceRequestImpl::GetMethod(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mParams.method;

    *result = mParams->method;
    return NOERROR;
}

ECode WebViewContentsClientAdapter::WebResourceRequestImpl::GetRequestHeaders(
    /* [out] */ IMap** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mParams.requestHeaders;

    assert(0);
    //*result = ;//base class is Map; = (IMap*)mParams->requestHeaders;
    return NOERROR;
}

//=====================================================================
//             WebViewContentsClientAdapter::InnerRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WebViewContentsClientAdapter::InnerRunnable, Object, IRunnable)

WebViewContentsClientAdapter::InnerRunnable::InnerRunnable(
    /* [in] */ WebViewContentsClientAdapter* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WebViewContentsClientAdapter::InnerRunnable::Run()
{
    // ==================before translated======================
    // UnimplementedWebViewApi.invoke();
    // if (mPictureListener != null) {
    //     if (TRACE) Log.d(TAG, "onPageFinished-fake");
    //     mPictureListener.onNewPicture(mWebView, new Picture());
    // }

    assert(NULL != mOwner);
    UnimplementedWebViewApi::Invoke();
    if (mOwner->mPictureListener != NULL) {
        if (TRACE) {
            Logger::D(TAG, "onPageFinished-fake");
        }

        AutoPtr<IPicture> picture;
        CPicture::New((IPicture**)&picture);
        mOwner->mPictureListener->OnNewPicture(mOwner->mWebView, picture);
    }
    return NOERROR;
}

//=====================================================================
//     WebViewContentsClientAdapter::JsPromptResultReceiverAdapter
//=====================================================================
CAR_INTERFACE_IMPL(WebViewContentsClientAdapter::JsPromptResultReceiverAdapter, Object, IJsResultReceiver)

WebViewContentsClientAdapter::JsPromptResultReceiverAdapter::JsPromptResultReceiverAdapter(
    /* [in] */ JsPromptResultReceiver* receiver)
    : mChromePromptResultReceiver(receiver)
{
    // ==================before translated======================
    // mChromePromptResultReceiver = receiver;
}

WebViewContentsClientAdapter::JsPromptResultReceiverAdapter::JsPromptResultReceiverAdapter(
    /* [in] */ JsResultReceiver* receiver)
    : mChromeResultReceiver(receiver)
{
    // ==================before translated======================
    // mChromeResultReceiver = receiver;
}

AutoPtr<JsPromptResult> WebViewContentsClientAdapter::JsPromptResultReceiverAdapter::GetPromptResult()
{
    // ==================before translated======================
    // return mPromptResult;

    return mPromptResult;
}

ECode WebViewContentsClientAdapter::JsPromptResultReceiverAdapter::OnJsResultComplete(
    /* [in] */ IJsResult* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mChromePromptResultReceiver != null) {
    //     if (mPromptResult.getResult()) {
    //         mChromePromptResultReceiver.confirm(mPromptResult.getStringResult());
    //     } else {
    //         mChromePromptResultReceiver.cancel();
    //     }
    // } else {
    //     if (mPromptResult.getResult()) {
    //         mChromeResultReceiver.confirm();
    //     } else {
    //         mChromeResultReceiver.cancel();
    //     }
    // }

    Boolean promptResult = FALSE;
    mPromptResult->GetResult(&promptResult);
    if (mChromePromptResultReceiver != NULL) {
        if (promptResult) {
            String strResult;
            mPromptResult->GetStringResult(&strResult);
            mChromePromptResultReceiver->Confirm(strResult);
        }
        else {
            mChromePromptResultReceiver->Cancel();
        }
    }
    else {
        if (promptResult) {
            mChromeResultReceiver->Confirm();
        }
        else {
            mChromeResultReceiver->Cancel();
        }
    }

    return NOERROR;
}

//=====================================================================
//         WebViewContentsClientAdapter::InnerSslErrorHandler
//=====================================================================
CAR_INTERFACE_IMPL(WebViewContentsClientAdapter::InnerSslErrorHandler, Object, ISslErrorHandler)

WebViewContentsClientAdapter::InnerSslErrorHandler::InnerSslErrorHandler(
    /* [in] */ IValueCallback* callback)
    : mCallback(callback)
{
    // ==================before translated======================
}

ECode WebViewContentsClientAdapter::InnerSslErrorHandler::Proceed()
{
    // ==================before translated======================
    // callback.onReceiveValue(true);

    AutoPtr<IBoolean> input = CoreUtils::Convert(TRUE);
    mCallback->OnReceiveValue(input);
    return NOERROR;
}

ECode WebViewContentsClientAdapter::InnerSslErrorHandler::Cancel()
{
    // ==================before translated======================
    // callback.onReceiveValue(false);

    AutoPtr<IBoolean> input = CoreUtils::Convert(FALSE);
    mCallback->OnReceiveValue(input);
    return NOERROR;
}

//=====================================================================
//         WebViewContentsClientAdapter::ClientCertRequestImpl
//=====================================================================
CAR_INTERFACE_IMPL(WebViewContentsClientAdapter::ClientCertRequestImpl, Object, IClientCertRequest)

WebViewContentsClientAdapter::ClientCertRequestImpl::ClientCertRequestImpl(
    /* [in] */ AwContentsClientBridge::ClientCertificateRequestCallback* callback,
    /* [in] */ ArrayOf<String>* keyTypes,
    /* [in] */ ArrayOf<IPrincipal*>* principals,
    /* [in] */ const String& host,
    /* [in] */ Int32 port)
    : mCallback(callback)
    , mKeyTypes(keyTypes)
    , mPrincipals(principals)
    , mHost(host)
    , mPort(port)
{
    // ==================before translated======================
    // mCallback = callback;
    // mKeyTypes = keyTypes;
    // mPrincipals = principals;
    // mHost = host;
    // mPort = port;
}

ECode WebViewContentsClientAdapter::ClientCertRequestImpl::GetKeyTypes(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // This is already a copy of native argument, so return directly.
    // return mKeyTypes;

    *result = mKeyTypes;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WebViewContentsClientAdapter::ClientCertRequestImpl::GetPrincipals(
    /* [out] */ ArrayOf<IPrincipal*>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // This is already a copy of native argument, so return directly.
    // return mPrincipals;

    *result = mPrincipals;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WebViewContentsClientAdapter::ClientCertRequestImpl::GetHost(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mHost;

    *result = mHost;
    return NOERROR;
}

ECode WebViewContentsClientAdapter::ClientCertRequestImpl::GetPort(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPort;

    *result = mPort;
    return NOERROR;
}

ECode WebViewContentsClientAdapter::ClientCertRequestImpl::Proceed(
    /* [in] */ IPrivateKey* privateKey,
    /* [in] */ ArrayOf<IX509Certificate*>* chain)
{
    VALIDATE_NOT_NULL(privateKey);
    VALIDATE_NOT_NULL(chain);
    // ==================before translated======================
    // mCallback.proceed(privateKey, chain);

    mCallback->Proceed(privateKey, chain);
    return NOERROR;
}

ECode WebViewContentsClientAdapter::ClientCertRequestImpl::Ignore()
{
    // ==================before translated======================
    // mCallback.ignore();

    mCallback->Ignore();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::ClientCertRequestImpl::Cancel()
{
    // ==================before translated======================
    // mCallback.cancel();

    mCallback->Cancel();
    return NOERROR;
}

//=====================================================================
//           WebViewContentsClientAdapter::InnerValueCallback1
//=====================================================================
CAR_INTERFACE_IMPL(WebViewContentsClientAdapter::InnerValueCallback1, Object, IValueCallback)

WebViewContentsClientAdapter::InnerValueCallback1::InnerValueCallback1(
    /* [in] */ IValueCallback* uploadFileCallback)
    : mUploadFileCallback(uploadFileCallback)
{
    // ==================before translated======================
}

ECode WebViewContentsClientAdapter::InnerValueCallback1::OnReceiveValue(
    /* [in] */ IInterface* uriList)
{
    VALIDATE_NOT_NULL(uriList);
    // ==================before translated======================
    // if (mCompleted) {
    //     throw new IllegalStateException("showFileChooser result was already called");
    // }
    // mCompleted = true;
    // String s[] = null;
    // if (uriList != null) {
    //     s = new const String&[uriList.length];
    //     for(int i = 0; i < uriList.length; i++) {
    //         s[i] = uriList[i].toString();
    //     }
    // }
    // uploadFileCallback.onReceiveValue(s);

    if (mCompleted) {
        //throw new IllegalStateException("showFileChooser result was already called");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mCompleted = TRUE;
    IList* uriListTmp = IList::Probe(uriList);
    assert(NULL != uriListTmp);

    AutoPtr<IList> s = NULL;
    if (uriListTmp != NULL) {
        Int32 size = 0;
        uriListTmp->GetSize(&size);
        CArrayList::New(size, (IList**)&s);
        String tmp;
        for(Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> valueTmp;
            uriListTmp->Get(i, (IInterface**)&valueTmp);
            IUri* uriTmp = IUri::Probe(valueTmp);
            assert(NULL != uriTmp);
            uriTmp->ToSafeString(&tmp);
            AutoPtr<ICharSequence> item;
            CString::New(tmp, (ICharSequence**)&item);
            s->Set(i, item);
        }
    }

    IInterface* sTmp = TO_IINTERFACE(s);
    return mUploadFileCallback->OnReceiveValue(sTmp);
}

//=====================================================================
//           WebViewContentsClientAdapter::InnerValueCallback2
//=====================================================================
CAR_INTERFACE_IMPL(WebViewContentsClientAdapter::InnerValueCallback2, Object, IValueCallback)

WebViewContentsClientAdapter::InnerValueCallback2::InnerValueCallback2(
    /* [in] */ IValueCallback* uploadFileCallback)
    : mUploadFileCallback(uploadFileCallback)
{
    // ==================before translated======================
}

ECode WebViewContentsClientAdapter::InnerValueCallback2::OnReceiveValue(
    /* [in] */ IInterface* uri)
{
    VALIDATE_NOT_NULL(uri);
    // ==================before translated======================
    // if (mCompleted) {
    //     throw new IllegalStateException("showFileChooser result was already called");
    // }
    // mCompleted = true;
    // uploadFileCallback.onReceiveValue(
    //         uri == null ? null : new String[] { uri.toString() });

    if (mCompleted) {
        //throw new IllegalStateException("showFileChooser result was already called");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mCompleted = TRUE;
    AutoPtr<ICharSequence> s = NULL;
    IUri* uriTmp = IUri::Probe(uri);
    if (uriTmp != NULL) {
        String tmp;
        uriTmp->ToSafeString(&tmp);
        CString::New(tmp, (ICharSequence**)&s);
        IInterface* interfaceTmp = TO_IINTERFACE(s);
        mUploadFileCallback->OnReceiveValue(interfaceTmp);
    }
    else {
        mUploadFileCallback->OnReceiveValue(NULL);
    }

    return NOERROR;
}

//=====================================================================
//        WebViewContentsClientAdapter::AwHttpAuthHandlerAdapter
//=====================================================================
WebViewContentsClientAdapter::AwHttpAuthHandlerAdapter::AwHttpAuthHandlerAdapter(
    /* [in] */ AwHttpAuthHandler* awHandler)
    : mAwHandler(awHandler)
{
    // ==================before translated======================
    // mAwHandler = awHandler;
}

ECode WebViewContentsClientAdapter::AwHttpAuthHandlerAdapter::Proceed(
    /* [in] */ const String& username,
    /* [in] */ const String& password)
{
    // ==================before translated======================
    // if (username == null) {
    //     username = "";
    // }
    //
    // if (password == null) {
    //     password = "";
    // }
    // mAwHandler.proceed(username, password);

    String userNameTmp = username;
    if (username.IsEmpty()) {
        userNameTmp = String("");
    }

    String passwordTmp = password;
    if (password.IsEmpty()) {
        passwordTmp = String("");
    }

    mAwHandler->Proceed(username, password);
    return NOERROR;
}

ECode WebViewContentsClientAdapter::AwHttpAuthHandlerAdapter::Cancel()
{
    // ==================before translated======================
    // mAwHandler.cancel();

    mAwHandler->Cancel();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::AwHttpAuthHandlerAdapter::UseHttpAuthUsernamePassword(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAwHandler.isFirstAttempt();

    *result = mAwHandler->IsFirstAttempt();
    return NOERROR;
}

//=====================================================================
//                     WebViewContentsClientAdapter
//=====================================================================
const String WebViewContentsClientAdapter::TAG("WebViewCallback");
const Boolean WebViewContentsClientAdapter::TRACE = FALSE;//DebugFlags::TRACE_CALLBACK;
const Int32 WebViewContentsClientAdapter::NEW_WEBVIEW_CREATED;

WebViewContentsClientAdapter::WebViewContentsClientAdapter(
    /* [in] */ IWebView* webView)
{
    // ==================before translated======================
    // if (webView == null) {
    //     throw new IllegalArgumentException("webView can't be null");
    // }
    //
    // mWebView = webView;
    // setWebViewClient(null);
    //
    // mUiThreadHandler = new IHandler() {
    //
    //     @Override
    //     public void handleMessage(Message msg) {
    //         switch(msg.what) {
    //             case NEW_WEBVIEW_CREATED:
    //                 WebView.WebViewTransport t = (WebView.WebViewTransport) msg.obj;
    //                 WebView newWebView = t.getWebView();
    //                 if (newWebView == mWebView) {
    //                     throw new IllegalArgumentException(
    //                             "Parent WebView cannot host it's own popup window. Please " +
    //                             "use WebSettings.setSupportMultipleWindows(false)");
    //                 }
    //
    //                 if (newWebView != null && newWebView.copyBackForwardList().getSize() != 0) {
    //                     throw new IllegalArgumentException(
    //                             "New WebView for popup window must not have been previously " +
    //                             "navigated.");
    //                 }
    //
    //                 WebViewChromium.completeWindowCreation(mWebView, newWebView);
    //                 break;
    //             default:
    //                 throw new IllegalStateException();
    //         }
    //     }
    // };

    if (NULL == webView) {
        //throw new IllegalArgumentException("webView can't be null");
        return ;
    }
    mWebView = webView;
    SetWebViewClient(NULL);
    mUiThreadHandler = new WebViewContentsClientAdapter::InnerHandler(this);
}

ECode WebViewContentsClientAdapter::SetWebViewClient(
    /* [in] */ IWebViewClient* client)
{
    VALIDATE_NOT_NULL(client);
    // ==================before translated======================
    // if (client != null) {
    //     mWebViewClient = client;
    // } else {
    //     mWebViewClient = new NullWebViewClient();
    // }

    if (client != NULL) {
        mWebViewClient = client;
    }
    else {
        mWebViewClient = new NullWebViewClient();
    }

    return NOERROR;
}

ECode WebViewContentsClientAdapter::SetWebChromeClient(
    /* [in] */ IWebChromeClient* client)
{
    VALIDATE_NOT_NULL(client);
    // ==================before translated======================
    // mWebChromeClient = client;

    mWebChromeClient = client;
    return NOERROR;
}

ECode WebViewContentsClientAdapter::SetDownloadListener(
    /* [in] */ IDownloadListener* listener)
{
    VALIDATE_NOT_NULL(listener);
    // ==================before translated======================
    // mDownloadListener = listener;

    mDownloadListener = listener;
    return NOERROR;
}

ECode WebViewContentsClientAdapter::SetFindListener(
    /* [in] */ IWebViewFindListener* listener)
{
    // ==================before translated======================
    // mFindListener = listener;

    mFindListener = listener;
    return NOERROR;
}

ECode WebViewContentsClientAdapter::SetPictureListener(
    /* [in] */ IWebViewPictureListener* listener)
{
    // ==================before translated======================
    // mPictureListener = listener;

    mPictureListener = listener;
    return NOERROR;
}

ECode WebViewContentsClientAdapter::GetVisitedHistory(
    /* [in] */ IValueCallback* callback)
{
    VALIDATE_NOT_NULL(callback);
    // ==================before translated======================
    // TraceEvent.begin();
    // if (mWebChromeClient != null) {
    //     if (TRACE) Log.d(TAG, "getVisitedHistory");
    //     mWebChromeClient.getVisitedHistory(callback);
    // }
    // TraceEvent.end();

    TraceEvent::Begin();
    if (mWebChromeClient != NULL) {
        if (TRACE) {
            Logger::D(TAG, "getVisitedHistory");
        }
        mWebChromeClient->GetVisitedHistory(callback);
    }
    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::DoUpdateVisitedHistory(
    /* [in] */ const String& url,
    /* [in] */ Boolean isReload)
{
    // ==================before translated======================
    // TraceEvent.begin();
    // if (TRACE) Log.d(TAG, "doUpdateVisitedHistory=" + url + " reload=" + isReload);
    // mWebViewClient.doUpdateVisitedHistory(mWebView, url, isReload);
    // TraceEvent.end();

    TraceEvent::Begin();
    if (TRACE) {
        String reloadStr = (TRUE == isReload) ? String("TRUE") : String("FALSE");
        Logger::D(TAG, String("doUpdateVisitedHistory=") + url + String(" reload=") + reloadStr);
    }

    mWebViewClient->DoUpdateVisitedHistory(mWebView, url, isReload);
    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnProgressChanged(
    /* [in] */ Int32 progress)
{
    // ==================before translated======================
    // TraceEvent.begin();
    // if (mWebChromeClient != null) {
    //     if (TRACE) Log.d(TAG, "onProgressChanged=" + progress);
    //     mWebChromeClient.onProgressChanged(mWebView, progress);
    // }
    // TraceEvent.end();

    TraceEvent::Begin();
    if (mWebChromeClient != NULL) {
        if (TRACE) {
            Logger::D(TAG, "onProgressChanged=%d", progress);
        }
        mWebChromeClient->OnProgressChanged(mWebView, progress);
    }
    TraceEvent::End();
    return NOERROR;
}

AutoPtr<AwWebResourceResponse> WebViewContentsClientAdapter::ShouldInterceptRequest(
    /* [in] */ ShouldInterceptRequestParams* params)
{
    // ==================before translated======================
    // TraceEvent.begin();
    // if (TRACE) Log.d(TAG, "shouldInterceptRequest=" + params.url);
    // WebResourceResponse response = mWebViewClient.shouldInterceptRequest(mWebView,
    //         new WebResourceRequestImpl(params));
    // TraceEvent.end();
    // if (response == null) return null;
    //
    // // AwWebResourceResponse should support null headers. b/16332774.
    // Map<String, String> responseHeaders = response.getResponseHeaders();
    // if (responseHeaders == null)
    //     responseHeaders = new HashMap<String, String>();
    //
    // return new AwWebResourceResponse(
    //         response.getMimeType(),
    //         response.getEncoding(),
    //         response.getData(),
    //         response.getStatusCode(),
    //         response.getReasonPhrase(),
    //         responseHeaders);

    TraceEvent::Begin();
    if (TRACE) {
        Logger::D(TAG, String("shouldInterceptRequest=") + params->url);
    }

    AutoPtr<IWebResourceRequest> webResourceRequest = new WebResourceRequestImpl(params);
    AutoPtr<IWebResourceResponse> response;
    mWebViewClient->ShouldInterceptRequest(mWebView, webResourceRequest, (IWebResourceResponse**)&response);

    TraceEvent::End();
    if (NULL == response)
        return NULL;

    // AwWebResourceResponse should support null headers. b/16332774.
    AutoPtr<IMap> responseHeaders;
    response->GetResponseHeaders((IMap**)&responseHeaders);
    if (NULL == responseHeaders)
        CHashMap::New((IMap**)&responseHeaders);

    String mimeType;
    response->GetMimeType(&mimeType);

    String encoding;
    response->GetEncoding(&encoding);

    AutoPtr<IInputStream> data;
    response->GetData((IInputStream**)&data);

    Int32 statusCode = 0;
    response->GetStatusCode(&statusCode);

    String reasonPhrase;
    response->GetReasonPhrase(&reasonPhrase);
    assert(0);
    AutoPtr<AwWebResourceResponse> result;//-- need map: = new AwWebResourceResponse(mimeType, encoding, data, statusCode, reasonPhrase, /* need Map<String, String>*/responseHeaders);
    return result;
}

Boolean WebViewContentsClientAdapter::ShouldOverrideUrlLoading(
    /* [in] */ const String& url)
{
    // ==================before translated======================
    // TraceEvent.begin();
    // if (TRACE) Log.d(TAG, "shouldOverrideUrlLoading=" + url);
    // boolean result = mWebViewClient.shouldOverrideUrlLoading(mWebView, url);
    // TraceEvent.end();
    // return result;

    TraceEvent::Begin();
    if (TRACE) {
        Logger::D(TAG, String("shouldOverrideUrlLoading=") + url);
    }

    Boolean result = FALSE;
    mWebViewClient->ShouldOverrideUrlLoading(mWebView, url, &result);
    TraceEvent::End();
    return result;
}

ECode WebViewContentsClientAdapter::OnUnhandledKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    VALIDATE_NOT_NULL(event);
    // ==================before translated======================
    // TraceEvent.begin();
    // if (TRACE) Log.d(TAG, "onUnhandledKeyEvent");
    // mWebViewClient.onUnhandledKeyEvent(mWebView, event);
    // TraceEvent.end();

    TraceEvent::Begin();
    if (TRACE) {
        Logger::D(TAG, "onUnhandledKeyEvent");
    }

    mWebViewClient->OnUnhandledKeyEvent(mWebView, event);
    TraceEvent::End();
    return NOERROR;
}

Boolean WebViewContentsClientAdapter::OnConsoleMessage(
    /* [in] */ IConsoleMessage* consoleMessage)
{
    // ==================before translated======================
    // TraceEvent.begin();
    // boolean result;
    // if (mWebChromeClient != null) {
    //     if (TRACE) Log.d(TAG, "onConsoleMessage: " + consoleMessage.message());
    //     result = mWebChromeClient.onConsoleMessage(consoleMessage);
    //     String message = consoleMessage.message();
    //     if (result && message != null && message.startsWith("[blocked]")) {
    //         Log.e(TAG, "Blocked URL: " + message);
    //     }
    // } else {
    //     result = false;
    // }
    // TraceEvent.end();
    // return result;

    TraceEvent::Begin();
    Boolean result = FALSE;
    if (mWebChromeClient != NULL) {
        if (TRACE) {
            String consoleMsg;
            consoleMessage->Message(&consoleMsg);
            Logger::D(TAG, String("onConsoleMessage: ") + consoleMsg);
        }

        mWebChromeClient->OnConsoleMessage(consoleMessage, &result);
        String message;
        consoleMessage->Message(&message);
        if (result && !message.IsEmpty() && message.StartWith(String("[blocked]"))) {
            Logger::E(TAG, String("Blocked URL: ") + message);
        }
    }

    TraceEvent::End();
    return result;
}

ECode WebViewContentsClientAdapter::OnFindResultReceived(
    /* [in] */ Int32 activeMatchOrdinal,
    /* [in] */ Int32 numberOfMatches,
    /* [in] */ Boolean isDoneCounting)
{
    // ==================before translated======================
    // if (mFindListener == null) return;
    // TraceEvent.begin();
    // if (TRACE) Log.d(TAG, "onFindResultReceived");
    // mFindListener.onFindResultReceived(activeMatchOrdinal, numberOfMatches, isDoneCounting);
    // TraceEvent.end();

    if (NULL == mFindListener)
        return NOERROR;

    TraceEvent::Begin();
    if (TRACE) {
        Logger::D(TAG, "onFindResultReceived");
    }

    mFindListener->OnFindResultReceived(activeMatchOrdinal, numberOfMatches, isDoneCounting);
    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnNewPicture(
    /* [in] */ IPicture* picture)
{
    VALIDATE_NOT_NULL(picture);
    // ==================before translated======================
    // if (mPictureListener == null) return;
    // TraceEvent.begin();
    // if (TRACE) Log.d(TAG, "onNewPicture");
    // mPictureListener.onNewPicture(mWebView, picture);
    // TraceEvent.end();

    if (NULL == mPictureListener)
        return NOERROR;

    TraceEvent::Begin();
    if (TRACE) {
        Logger::D(TAG, "onNewPicture");
    }

    mPictureListener->OnNewPicture(mWebView, picture);
    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnLoadResource(
    /* [in] */ const String& url)
{
    // ==================before translated======================
    // TraceEvent.begin();
    // if (TRACE) Log.d(TAG, "onLoadResource=" + url);
    // mWebViewClient.onLoadResource(mWebView, url);
    // TraceEvent.end();

    TraceEvent::Begin();
    if (TRACE) {
        Logger::D(TAG, String("onLoadResource=") + url);
    }

    mWebViewClient->OnLoadResource(mWebView, url);
    TraceEvent::End();
    return NOERROR;
}

Boolean WebViewContentsClientAdapter::OnCreateWindow(
    /* [in] */ Boolean isDialog,
    /* [in] */ Boolean isUserGesture)
{
    // ==================before translated======================
    // Message m = mUiThreadHandler.obtainMessage(
    //         NEW_WEBVIEW_CREATED, mWebView.new WebViewTransport());
    // TraceEvent.begin();
    // boolean result;
    // if (mWebChromeClient != null) {
    //     if (TRACE) Log.d(TAG, "onCreateWindow");
    //     result = mWebChromeClient.onCreateWindow(mWebView, isDialog, isUserGesture, m);
    // } else {
    //     result = false;
    // }
    // TraceEvent.end();
    // return result;

    AutoPtr<IMessage> m;
    mUiThreadHandler->ObtainMessage(NEW_WEBVIEW_CREATED, NULL/*mWebView.new WebViewTransport()*/, (IMessage**)&m);
    TraceEvent::Begin();
    Boolean result = FALSE;
    if (mWebChromeClient != NULL) {
        if (TRACE) {
            Logger::D(TAG, "onCreateWindow");
        }
        mWebChromeClient->OnCreateWindow(mWebView, isDialog, isUserGesture, m, &result);
    }

    TraceEvent::End();
    return result;
}

ECode WebViewContentsClientAdapter::OnCloseWindow()
{
    // ==================before translated======================
    // TraceEvent.begin();
    // if (mWebChromeClient != null) {
    //     if (TRACE) Log.d(TAG, "onCloseWindow");
    //     mWebChromeClient.onCloseWindow(mWebView);
    // }
    // TraceEvent.end();

    TraceEvent::Begin();
    if (mWebChromeClient != NULL) {
        if (TRACE) {
            Logger::D(TAG, "onCloseWindow");
        }
        mWebChromeClient->OnCloseWindow(mWebView);
    }

    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnRequestFocus()
{
    // ==================before translated======================
    // TraceEvent.begin();
    // if (mWebChromeClient != null) {
    //     if (TRACE) Log.d(TAG, "onRequestFocus");
    //     mWebChromeClient.onRequestFocus(mWebView);
    // }
    // TraceEvent.end();

    TraceEvent::Begin();
    if (mWebChromeClient != NULL) {
        if (TRACE) {
            Logger::D(TAG, "onRequestFocus");
        }
        mWebChromeClient->OnRequestFocus(mWebView);
    }

    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnReceivedTouchIconUrl(
    /* [in] */ const String& url,
    /* [in] */ Boolean precomposed)
{
    // ==================before translated======================
    // TraceEvent.begin();
    // if (mWebChromeClient != null) {
    //     if (TRACE) Log.d(TAG, "onReceivedTouchIconUrl=" + url);
    //     mWebChromeClient.onReceivedTouchIconUrl(mWebView, url, precomposed);
    // }
    // TraceEvent.end();

    TraceEvent::Begin();
    if (mWebChromeClient != NULL) {
        if (TRACE) {
            Logger::D(TAG, String("onReceivedTouchIconUrl=") + url);
        }
        mWebChromeClient->OnReceivedTouchIconUrl(mWebView, url, precomposed);
    }

    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnReceivedIcon(
    /* [in] */ IBitmap* bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    // ==================before translated======================
    // TraceEvent.begin();
    // if (mWebChromeClient != null) {
    //     if (TRACE) Log.d(TAG, "onReceivedIcon");
    //     mWebChromeClient.onReceivedIcon(mWebView, bitmap);
    // }
    // TraceEvent.end();

    TraceEvent::Begin();
    if (mWebChromeClient != NULL) {
        if (TRACE) {
            Logger::D(TAG, "onReceivedIcon");
        }
        mWebChromeClient->OnReceivedIcon(mWebView, bitmap);
    }

    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnPageStarted(
    /* [in] */ const String& url)
{
    // ==================before translated======================
    // TraceEvent.begin();
    // if (TRACE) Log.d(TAG, "onPageStarted=" + url);
    // mWebViewClient.onPageStarted(mWebView, url, mWebView.getFavicon());
    // TraceEvent.end();

    TraceEvent::Begin();
    if (TRACE) {
        Logger::D(TAG, String("onPageStarted=") + url);
    }

    AutoPtr<IBitmap> favicon;
    mWebView->GetFavicon((IBitmap**)&favicon);
    mWebViewClient->OnPageStarted(mWebView, url, favicon);
    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnPageFinished(
    /* [in] */ const String& url)
{
    // ==================before translated======================
    // TraceEvent.begin();
    // if (TRACE) Log.d(TAG, "onPageFinished=" + url);
    // mWebViewClient.onPageFinished(mWebView, url);
    // TraceEvent.end();
    //
    // // See b/8208948
    // // This fakes an onNewPicture callback after onPageFinished to allow
    // // CTS tests to run in an un-flaky manner. This is required as the
    // // path for sending Picture updates in Chromium are decoupled from the
    // // page loading callbacks, i.e. the Chrome compositor may draw our
    // // content and send the Picture before onPageStarted or onPageFinished
    // // are invoked. The CTS harness discards any pictures it receives before
    // // onPageStarted is invoked, so in the case we get the Picture before that and
    // // no further updates after onPageStarted, we'll fail the test by timing
    // // out waiting for a Picture.
    // // To ensure backwards compatibility, we need to defer sending Picture updates
    // // until onPageFinished has been invoked. This work is being done
    // // upstream, and we can revert this hack when it lands.
    // if (mPictureListener != null) {
    //     ThreadUtils.postOnUiThreadDelayed(new Runnable() {
    //         @Override
    //         public void run() {
    //             UnimplementedWebViewApi.invoke();
    //             if (mPictureListener != null) {
    //                 if (TRACE) Log.d(TAG, "onPageFinished-fake");
    //                 mPictureListener.onNewPicture(mWebView, new Picture());
    //             }
    //         }
    //     }, 100);
    // }

    TraceEvent::Begin();
    if (TRACE) {
        Logger::D(TAG, String("onPageFinished=") + url);
    }

    mWebViewClient->OnPageFinished(mWebView, url);
    TraceEvent::End();

    // See b/8208948
    // This fakes an onNewPicture callback after onPageFinished to allow
    // CTS tests to run in an un-flaky manner. This is required as the
    // path for sending Picture updates in Chromium are decoupled from the
    // page loading callbacks, i.e. the Chrome compositor may draw our
    // content and send the Picture before onPageStarted or onPageFinished
    // are invoked. The CTS harness discards any pictures it receives before
    // onPageStarted is invoked, so in the case we get the Picture before that and
    // no further updates after onPageStarted, we'll fail the test by timing
    // out waiting for a Picture.
    // To ensure backwards compatibility, we need to defer sending Picture updates
    // until onPageFinished has been invoked. This work is being done
    // upstream, and we can revert this hack when it lands.
    if (mPictureListener != NULL) {
        AutoPtr<IRunnable> runnable = new WebViewContentsClientAdapter::InnerRunnable(this);
        ThreadUtils::PostOnUiThreadDelayed(runnable, 100);
    }

    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnReceivedError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& description,
    /* [in] */ const String& failingUrl)
{
    // ==================before translated======================
    // if (description == null || description.isEmpty()) {
    //     // ErrorStrings is @hidden, so we can't do this in AwContents.
    //     // Normally the net/ layer will set a valid description, but for synthesized callbacks
    //     // (like in the case for intercepted requests) AwContents will pass in null.
    //     description = ErrorStrings.getString(errorCode, mWebView.getContext());
    // }
    // TraceEvent.begin();
    // if (TRACE) Log.d(TAG, "onReceivedError=" + failingUrl);
    // mWebViewClient.onReceivedError(mWebView, errorCode, description, failingUrl);
    // TraceEvent.end();

    String descriptionTmp = description;
    if (descriptionTmp.IsEmpty()) {
        // ErrorStrings is @hidden, so we can't do this in AwContents.
        // Normally the net/ layer will set a valid description, but for synthesized callbacks
        // (like in the case for intercepted requests) AwContents will pass in null.

        IView* viewTmp = IView::Probe(mWebView);
        AutoPtr<IContext> context;
        viewTmp->GetContext((IContext**)&context);

        AutoPtr<IErrorStrings> errorStrings;
        CErrorStrings::AcquireSingleton((IErrorStrings**)&errorStrings);
        errorStrings->GetString(errorCode, context, &descriptionTmp);
    }

    TraceEvent::Begin();
    if (TRACE) {
        Logger::D(TAG, String("onReceivedError=") + failingUrl);
    }

    mWebViewClient->OnReceivedError(mWebView, errorCode, description, failingUrl);
    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnReceivedTitle(
    /* [in] */ const String& title)
{
    // ==================before translated======================
    // TraceEvent.begin();
    // if (mWebChromeClient != null) {
    //     if (TRACE) Log.d(TAG, "onReceivedTitle");
    //     mWebChromeClient.onReceivedTitle(mWebView, title);
    // }
    // TraceEvent.end();

    TraceEvent::Begin();
    if (mWebChromeClient != NULL) {
        if (TRACE) {
            Logger::D(TAG, "onReceivedTitle");
        }
        mWebChromeClient->OnReceivedTitle(mWebView, title);
    }

    TraceEvent::End();
    return NOERROR;
}

Boolean WebViewContentsClientAdapter::ShouldOverrideKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    // ==================before translated======================
    // // The check below is reflecting Clank's behavior and is a workaround for http://b/7697782.
    // // 1. The check for system key should be made in AwContents or ContentViewCore, before
    // //    shouldOverrideKeyEvent() is called at all.
    // // 2. shouldOverrideKeyEvent() should be called in onKeyDown/onKeyUp, not from
    // //    dispatchKeyEvent().
    // if (!ContentViewClient.shouldPropagateKey(event.getKeyCode())) return true;
    // TraceEvent.begin();
    // if (TRACE) Log.d(TAG, "shouldOverrideKeyEvent");
    // boolean result = mWebViewClient.shouldOverrideKeyEvent(mWebView, event);
    // TraceEvent.end();
    // return result;

    Int32 keyCode = 0;
    event->GetKeyCode(&keyCode);
    if (!ContentViewClient::ShouldPropagateKey(keyCode))
        return TRUE;

    TraceEvent::Begin();
    if (TRACE) {
        Logger::D(TAG, "shouldOverrideKeyEvent");
    }

    Boolean result = FALSE;
    mWebViewClient->ShouldOverrideKeyEvent(mWebView, event, &result);
    TraceEvent::End();
    return result;
}

ECode WebViewContentsClientAdapter::OnStartContentIntent(
    /* [in] */ IContext* context,
    /* [in] */ const String& contentUrl)
{
    VALIDATE_NOT_NULL(context);
    // ==================before translated======================
    // TraceEvent.begin();
    // if (TRACE) Log.d(TAG, "shouldOverrideUrlLoading=" + contentUrl);
    // mWebViewClient.shouldOverrideUrlLoading(mWebView, contentUrl);
    // TraceEvent.end();

    TraceEvent::Begin();
    if (TRACE) {
        Logger::D(TAG, String("shouldOverrideUrlLoading=") + contentUrl);
    }

    Boolean result = FALSE;
    mWebViewClient->ShouldOverrideUrlLoading(mWebView, contentUrl, &result);
    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnGeolocationPermissionsShowPrompt(
    /* [in] */ const String& origin,
    /* [in] */ IGeolocationPermissionsCallback* callback)
{
    VALIDATE_NOT_NULL(callback);
    // ==================before translated======================
    // TraceEvent.begin();
    // if (mWebChromeClient != null) {
    //     if (TRACE) Log.d(TAG, "onGeolocationPermissionsShowPrompt");
    //     mWebChromeClient.onGeolocationPermissionsShowPrompt(origin, callback);
    // }
    // TraceEvent.end();

    TraceEvent::Begin();
    if (mWebChromeClient != NULL) {
        if (TRACE) {
            Logger::D(TAG, "onGeolocationPermissionsShowPrompt");
        }
        mWebChromeClient->OnGeolocationPermissionsShowPrompt(origin, callback);
    }

    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnGeolocationPermissionsHidePrompt()
{
    // ==================before translated======================
    // TraceEvent.begin();
    // if (mWebChromeClient != null) {
    //     if (TRACE) Log.d(TAG, "onGeolocationPermissionsHidePrompt");
    //     mWebChromeClient.onGeolocationPermissionsHidePrompt();
    // }
    // TraceEvent.end();

    TraceEvent::Begin();
    if (mWebChromeClient != NULL) {
        if (TRACE) {
            Logger::D(TAG, "onGeolocationPermissionsHidePrompt");
        }
        mWebChromeClient->OnGeolocationPermissionsHidePrompt();
    }

    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnPermissionRequest(
    /* [in] */ AwPermissionRequest* permissionRequest)
{
    VALIDATE_NOT_NULL(permissionRequest);
    // ==================before translated======================
    // TraceEvent.begin();
    // if (mWebChromeClient != null) {
    //     if (TRACE) Log.d(TAG, "onPermissionRequest");
    //     if (mOngoingPermissionRequests == null) {
    //         mOngoingPermissionRequests =
    //             new WeakHashMap<AwPermissionRequest, WeakReference<PermissionRequestAdapter>>();
    //     }
    //     PermissionRequestAdapter adapter = new PermissionRequestAdapter(permissionRequest);
    //     mOngoingPermissionRequests.put(
    //             permissionRequest, new WeakReference<PermissionRequestAdapter>(adapter));
    //     mWebChromeClient.onPermissionRequest(adapter);
    // } else {
    //     // By default, we deny the permission.
    //     permissionRequest.deny();
    // }
    // TraceEvent.end();

    TraceEvent::Begin();
    if (mWebChromeClient != NULL) {
        if (TRACE) {
            Logger::D(TAG, "onPermissionRequest");
        }

        if (NULL == mOngoingPermissionRequests) {
            CWeakHashMap::New((IWeakHashMap**)&mOngoingPermissionRequests);
        }

        AutoPtr<PermissionRequestAdapter> adapter = new PermissionRequestAdapter(permissionRequest);
        IInterface* key = TO_IINTERFACE(permissionRequest);

        IObject* adapterTmp = (IObject*)adapter;
        IWeakReferenceSource* source = IWeakReferenceSource::Probe(adapterTmp);
        AutoPtr<IWeakReference> wr;
        source->GetWeakReference((IWeakReference**)&wr);
        IInterface* value = TO_IINTERFACE(wr);

        mOngoingPermissionRequests->Put(key, value);
        mWebChromeClient->OnPermissionRequest(adapter);
    }
    else {
        // By default, we deny the permission.
        permissionRequest->Deny();
    }

    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnPermissionRequestCanceled(
    /* [in] */ AwPermissionRequest* permissionRequest)
{
    VALIDATE_NOT_NULL(permissionRequest);
    // ==================before translated======================
    // TraceEvent.begin();
    // if (mWebChromeClient != null && mOngoingPermissionRequests != null) {
    //     if (TRACE) Log.d(TAG, "onPermissionRequestCanceled");
    //     WeakReference<PermissionRequestAdapter> weakRef =
    //             mOngoingPermissionRequests.get(permissionRequest);
    //     // We don't hold strong reference to PermissionRequestAdpater and don't expect the
    //     // user only holds weak reference to it either, if so, user has no way to call
    //     // grant()/deny(), and no need to be notified the cancellation of request.
    //     if (weakRef != null) {
    //         PermissionRequestAdapter adapter = weakRef.get();
    //         if (adapter != null) mWebChromeClient.onPermissionRequestCanceled(adapter);
    //     }
    // }
    // TraceEvent.end();

    TraceEvent::Begin();
    if (mWebChromeClient != NULL && mOngoingPermissionRequests != NULL) {
        if (TRACE) {
            Logger::D(TAG, "onPermissionRequestCanceled");
        }

        AutoPtr<IInterface> value;
        mOngoingPermissionRequests->Get(TO_IINTERFACE(permissionRequest), (IInterface**)&value);
        IWeakReference* weakRef = IWeakReference::Probe(value);

        // We don't hold strong reference to PermissionRequestAdpater and don't expect the
        // user only holds weak reference to it either, if so, user has no way to call
        // grant()/deny(), and no need to be notified the cancellation of request.
        if (weakRef != NULL) {
            AutoPtr<IInterface> strongObj;
            weakRef->Resolve(EIID_IInterface, (IInterface**)&strongObj);
            IObject* strongObjTmp = (IObject*)strongObj.Get();
            PermissionRequestAdapter* adapter = (PermissionRequestAdapter*)strongObjTmp;
            if (adapter != NULL) {
                mWebChromeClient->OnPermissionRequestCanceled(adapter);
            }
        }
    }

    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::HandleJsAlert(
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ JsResultReceiver* receiver)
{
    VALIDATE_NOT_NULL(receiver);
    // ==================before translated======================
    // TraceEvent.begin();
    // if (mWebChromeClient != null) {
    //     final JsPromptResult res =
    //             new JsPromptResultReceiverAdapter(receiver).getPromptResult();
    //     if (TRACE) Log.d(TAG, "onJsAlert");
    //     if (!mWebChromeClient.onJsAlert(mWebView, url, message, res)) {
    //         new JsDialogHelper(res, JsDialogHelper.ALERT, null, message, url)
    //                 .showDialog(mWebView.getContext());
    //     }
    // } else {
    //     receiver.cancel();
    // }
    // TraceEvent.end();

    TraceEvent::Begin();
    if (mWebChromeClient != NULL) {
        AutoPtr<JsPromptResultReceiverAdapter> resultReceiver = new JsPromptResultReceiverAdapter(receiver);
        AutoPtr<JsPromptResult> res = resultReceiver->GetPromptResult();
        if (TRACE) {
            Logger::D(TAG, "onJsAlert");
        }

        Boolean jsAlert = FALSE;
        mWebChromeClient->OnJsAlert(mWebView, url, message, res, &jsAlert);
        if (!jsAlert) {
            AutoPtr<IJsDialogHelper> helper;
            assert(0);
            //-- helper hasnot base class named Singleton : CJsDialogHelper::AcquireSingleton(res, IJsDialogHelper::ALERT, NULL, message, url, (IJsDialogHelper**)&helper);
            AutoPtr<IContext> context;
            IView::Probe(mWebView)->GetContext((IContext**)&context);
            helper->ShowDialog(context);
        }
    }
    else {
        receiver->Cancel();
    }

    TraceEvent::End();
    return NOERROR;
}

void WebViewContentsClientAdapter::HandleJsBeforeUnload(
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ JsResultReceiver* receiver)
{
    // ==================before translated======================
    // TraceEvent.begin();
    // if (mWebChromeClient != null) {
    //     final JsPromptResult res =
    //             new JsPromptResultReceiverAdapter(receiver).getPromptResult();
    //     if (TRACE) Log.d(TAG, "onJsBeforeUnload");
    //     if (!mWebChromeClient.onJsBeforeUnload(mWebView, url, message, res)) {
    //         new JsDialogHelper(res, JsDialogHelper.UNLOAD, null, message, url)
    //                 .showDialog(mWebView.getContext());
    //     }
    // } else {
    //     receiver.cancel();
    // }
    // TraceEvent.end();

    TraceEvent::Begin();
    if (mWebChromeClient != NULL) {
        AutoPtr<JsPromptResultReceiverAdapter> receiverAdapter = new JsPromptResultReceiverAdapter(receiver);
        AutoPtr<JsPromptResult> res = receiverAdapter->GetPromptResult();
        if (TRACE) {
            Logger::D(TAG, "onJsBeforeUnload");
        }

        Boolean result = FALSE;
        mWebChromeClient->OnJsBeforeUnload(mWebView, url, message, res, &result);
        if (!result) {
            AutoPtr<IJsDialogHelper> helper;
            assert(0);
            //-- helper hasnot base class named Singleton : CJsDialogHelper::AcquireSingleton(res, IJsDialogHelper::UNLOAD, NULL, message, url, (IJsDialogHelper**)&helper);
            AutoPtr<IContext> context;
            IView::Probe(mWebView)->GetContext((IContext**)&context);
            helper->ShowDialog(context);
        }
    }
    else {
        receiver->Cancel();
    }

    TraceEvent::End();
}

void WebViewContentsClientAdapter::HandleJsConfirm(
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ JsResultReceiver* receiver)
{
    // ==================before translated======================
    // TraceEvent.begin();
    // if (mWebChromeClient != null) {
    //     final JsPromptResult res =
    //             new JsPromptResultReceiverAdapter(receiver).getPromptResult();
    //     if (TRACE) Log.d(TAG, "onJsConfirm");
    //     if (!mWebChromeClient.onJsConfirm(mWebView, url, message, res)) {
    //         new JsDialogHelper(res, JsDialogHelper.CONFIRM, null, message, url)
    //                 .showDialog(mWebView.getContext());
    //     }
    // } else {
    //     receiver.cancel();
    // }
    // TraceEvent.end();

    TraceEvent::Begin();
    if (mWebChromeClient != NULL) {
        AutoPtr<JsPromptResultReceiverAdapter> receiverAdapter = new JsPromptResultReceiverAdapter(receiver);
        AutoPtr<JsPromptResult> res = receiverAdapter->GetPromptResult();
        if (TRACE) {
            Logger::D(TAG, "onJsConfirm");
        }

        Boolean result = FALSE;
        mWebChromeClient->OnJsConfirm(mWebView, url, message, res, &result);
        if (!result) {
            AutoPtr<IJsDialogHelper> helper;
            assert(0);
            //-- helper hasnot base class named Singleton : CJsDialogHelper::AcquireSingleton(res, IJsDialogHelper::CONFIRM, NULL, message, url, (IJsDialogHelper**)&helper);
            AutoPtr<IContext> context;
            IView::Probe(mWebView)->GetContext((IContext**)&context);
            helper->ShowDialog(context);
        }
    }
    else {
        receiver->Cancel();
    }

    TraceEvent::End();
}

void WebViewContentsClientAdapter::HandleJsPrompt(
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ const String& defaultValue,
    /* [in] */ JsPromptResultReceiver* receiver)
{
    // ==================before translated======================
    // TraceEvent.begin();
    // if (mWebChromeClient != null) {
    //     final JsPromptResult res =
    //             new JsPromptResultReceiverAdapter(receiver).getPromptResult();
    //     if (TRACE) Log.d(TAG, "onJsPrompt");
    //     if (!mWebChromeClient.onJsPrompt(mWebView, url, message, defaultValue, res)) {
    //         new JsDialogHelper(res, JsDialogHelper.PROMPT, defaultValue, message, url)
    //                 .showDialog(mWebView.getContext());
    //     }
    // } else {
    //     receiver.cancel();
    // }
    // TraceEvent.end();

    TraceEvent::Begin();
    if (mWebChromeClient != NULL) {
        AutoPtr<JsPromptResultReceiverAdapter> receiverAdapter = new JsPromptResultReceiverAdapter(receiver);
        AutoPtr<JsPromptResult> res = receiverAdapter->GetPromptResult();
        if (TRACE) {
            Logger::D(TAG, "onJsConfirm");
        }

        Boolean result = FALSE;
        mWebChromeClient->OnJsPrompt(mWebView, url, message, defaultValue, res, &result);
        if (!result) {
            AutoPtr<IJsDialogHelper> helper;
            assert(0);
            //-- helper hasnot base class named Singleton : CJsDialogHelper::AcquireSingleton(res, IJsDialogHelper::PROMPT, NULL, message, url, (IJsDialogHelper**)&helper);
            AutoPtr<IContext> context;
            IView::Probe(mWebView)->GetContext((IContext**)&context);
            helper->ShowDialog(context);
        }
    }
    else {
        receiver->Cancel();
    }

    TraceEvent::End();
}

ECode WebViewContentsClientAdapter::OnReceivedHttpAuthRequest(
    /* [in] */ AwHttpAuthHandler* handler,
    /* [in] */ const String& host,
    /* [in] */ const String& realm)
{
    VALIDATE_NOT_NULL(handler);
    // ==================before translated======================
    // TraceEvent.begin();
    // if (TRACE) Log.d(TAG, "onReceivedHttpAuthRequest=" + host);
    // mWebViewClient.onReceivedHttpAuthRequest(mWebView,
    //         new AwHttpAuthHandlerAdapter(handler), host, realm);
    // TraceEvent.end();

    TraceEvent::Begin();
    if (TRACE) {
        Logger::D(TAG, String("onReceivedHttpAuthRequest=") + host);
    }

    AutoPtr<AwHttpAuthHandlerAdapter> adapter = new AwHttpAuthHandlerAdapter(handler);
    mWebViewClient->OnReceivedHttpAuthRequest(mWebView, adapter, host, realm);
    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnReceivedSslError(
    /* [in] */ IValueCallback* callback,
    /* [in] */ ISslError* error)
{
    VALIDATE_NOT_NULL(callback);
    VALIDATE_NOT_NULL(error);
    // ==================before translated======================
    // SslErrorHandler handler = new SslErrorHandler() {
    //     @Override
    //     public void proceed() {
    //         callback.onReceiveValue(true);
    //     }
    //     @Override
    //     public void cancel() {
    //         callback.onReceiveValue(false);
    //     }
    // };
    // TraceEvent.begin();
    // if (TRACE) Log.d(TAG, "onReceivedSslError");
    // mWebViewClient.onReceivedSslError(mWebView, handler, error);
    // TraceEvent.end();

    AutoPtr<InnerSslErrorHandler> handler = new InnerSslErrorHandler(callback);
    TraceEvent::Begin();
    if (TRACE) {
        Logger::D(TAG, "onReceivedSslError");
    }

    mWebViewClient->OnReceivedSslError(mWebView, handler, error);
    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnReceivedClientCertRequest(
    /* [in] */ AwContentsClientBridge::ClientCertificateRequestCallback* callback,
    /* [in] */ ArrayOf<String>* keyTypes,
    /* [in] */ ArrayOf<IPrincipal*>* principals,
    /* [in] */ const String& host,
    /* [in] */ Int32 port)
{
    VALIDATE_NOT_NULL(callback);
    VALIDATE_NOT_NULL(keyTypes);
    VALIDATE_NOT_NULL(principals);
    // ==================before translated======================
    // if (TRACE) Log.d(TAG, "onReceivedClientCertRequest");
    // TraceEvent.begin();
    // final ClientCertRequestImpl request = new ClientCertRequestImpl(callback,
    //     keyTypes, principals, host, port);
    // mWebViewClient.onReceivedClientCertRequest(mWebView, request);
    // TraceEvent.end();

    if (TRACE) {
        Logger::D(TAG, "onReceivedClientCertRequest");
    }

    TraceEvent::Begin();
    AutoPtr<ClientCertRequestImpl> request = new ClientCertRequestImpl(callback, keyTypes, principals, host, port);
    mWebViewClient->OnReceivedClientCertRequest(mWebView, request);
    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnReceivedLoginRequest(
    /* [in] */ const String& realm,
    /* [in] */ const String& account,
    /* [in] */ const String& args)
{
    // ==================before translated======================
    // TraceEvent.begin();
    // if (TRACE) Log.d(TAG, "onReceivedLoginRequest=" + realm);
    // mWebViewClient.onReceivedLoginRequest(mWebView, realm, account, args);
    // TraceEvent.end();

    TraceEvent::Begin();
    if (TRACE) {
        Logger::D(TAG, String("onReceivedLoginRequest=") + realm);
    }

    mWebViewClient->OnReceivedLoginRequest(mWebView, realm, account, args);
    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnFormResubmission(
    /* [in] */ IMessage* dontResend,
    /* [in] */ IMessage* resend)
{
    VALIDATE_NOT_NULL(dontResend);
    VALIDATE_NOT_NULL(resend);
    // ==================before translated======================
    // TraceEvent.begin();
    // if (TRACE) Log.d(TAG, "onFormResubmission");
    // mWebViewClient.onFormResubmission(mWebView, dontResend, resend);
    // TraceEvent.end();

    TraceEvent::Begin();
    if (TRACE) {
        Logger::D(TAG, "onFormResubmission");
    }

    mWebViewClient->OnFormResubmission(mWebView, dontResend, resend);
    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnDownloadStart(
    /* [in] */ const String& url,
    /* [in] */ const String& userAgent,
    /* [in] */ const String& contentDisposition,
    /* [in] */ const String& mimeType,
    /* [in] */ Int64 contentLength)
{
    // ==================before translated======================
    // if (mDownloadListener != null) {
    //     TraceEvent.begin();
    //     if (TRACE) Log.d(TAG, "onDownloadStart");
    //     mDownloadListener.onDownloadStart(url,
    //                                       userAgent,
    //                                       contentDisposition,
    //                                       mimeType,
    //                                       contentLength);
    //     TraceEvent.end();
    // }

    if (mDownloadListener != NULL) {
        TraceEvent::Begin();
        if (TRACE) {
            Logger::D(TAG, "onDownloadStart");
        }
        mDownloadListener->OnDownloadStart(url, userAgent, contentDisposition, mimeType, contentLength);
        TraceEvent::End();
    }
    return NOERROR;
}

ECode WebViewContentsClientAdapter::ShowFileChooser(
    /* [in] */ IValueCallback* uploadFileCallback,
    /* [in] */ AwContentsClient::FileChooserParams* fileChooserParams)
{
    VALIDATE_NOT_NULL(uploadFileCallback);
    VALIDATE_NOT_NULL(fileChooserParams);
    // ==================before translated======================
    // if (mWebChromeClient == null) {
    //     uploadFileCallback.onReceiveValue(null);
    //     return;
    // }
    // TraceEvent.begin();
    // FileChooserParamsAdapter adapter = new FileChooserParamsAdapter(
    //         fileChooserParams, mWebView.getContext());
    // if (TRACE) Log.d(TAG, "showFileChooser");
    // ValueCallback<IUri[]> callbackAdapter = new ValueCallback<IUri[]>() {
    //     private boolean mCompleted;
    //     @Override
    //     public void onReceiveValue(IUri[] uriList) {
    //         if (mCompleted) {
    //             throw new IllegalStateException("showFileChooser result was already called");
    //         }
    //         mCompleted = true;
    //         const String& s[] = null;
    //         if (uriList != null) {
    //             s = new const String&[uriList.length];
    //             for(int i = 0; i < uriList.length; i++) {
    //                 s[i] = uriList[i].toString();
    //             }
    //         }
    //         uploadFileCallback.onReceiveValue(s);
    //     }
    // };
    // if (mWebChromeClient.onShowFileChooser(mWebView, callbackAdapter, adapter)) {
    //     return;
    // }
    // if (mWebView.getContext().getApplicationInfo().targetSdkVersion >
    //         Build.VERSION_CODES.KITKAT) {
    //     uploadFileCallback.onReceiveValue(null);
    //     return;
    // }
    // ValueCallback<IUri> innerCallback = new ValueCallback<IUri>() {
    //     private boolean mCompleted;
    //     @Override
    //     public void onReceiveValue(IUri uri) {
    //         if (mCompleted) {
    //             throw new IllegalStateException("showFileChooser result was already called");
    //         }
    //         mCompleted = true;
    //         uploadFileCallback.onReceiveValue(
    //                 uri == null ? null : new const String&[] { uri.toString() });
    //     }
    // };
    // if (TRACE) Log.d(TAG, "openFileChooser");
    // mWebChromeClient.openFileChooser(innerCallback, fileChooserParams.acceptTypes,
    //         fileChooserParams.capture ? "*" : "");
    // TraceEvent.end();

    if (NULL == mWebChromeClient) {
        uploadFileCallback->OnReceiveValue(NULL);
        return NOERROR;
    }

    TraceEvent::Begin();
    IView* viewTmp = IView::Probe(mWebView);
    AutoPtr<IContext> context;
    viewTmp->GetContext((IContext**)&context);
    AutoPtr<FileChooserParamsAdapter> adapter = new FileChooserParamsAdapter(fileChooserParams, context);
    if (TRACE) {
        Logger::D(TAG, "showFileChooser");
    }

    AutoPtr<InnerValueCallback1> callbackAdapter = new InnerValueCallback1(uploadFileCallback);
    Boolean result = FALSE;
    mWebChromeClient->OnShowFileChooser(mWebView, callbackAdapter, adapter, &result);
    if (result) {
        return NOERROR;
    }

    AutoPtr<IApplicationInfo> applicationInfo;
    context->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    Int32 targetSdkVersion = 0;
    applicationInfo->GetTargetSdkVersion(&targetSdkVersion);
    if (targetSdkVersion > Build::VERSION_CODES::KITKAT) {
        uploadFileCallback->OnReceiveValue(NULL);
        return NOERROR;
    }

    AutoPtr<InnerValueCallback2> innerCallback = new InnerValueCallback2(uploadFileCallback);
    if (TRACE) {
        Logger::D(TAG, "openFileChooser");
    }

    String capture = fileChooserParams->capture ? String("*") : String("");
    mWebChromeClient->OpenFileChooser(innerCallback, fileChooserParams->acceptTypes, capture);
    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnScaleChangedScaled(
    /* [in] */ Float oldScale,
    /* [in] */ Float newScale)
{
    // ==================before translated======================
    // TraceEvent.begin();
    // if (TRACE) Log.d(TAG, " onScaleChangedScaled");
    // mWebViewClient.onScaleChanged(mWebView, oldScale, newScale);
    // TraceEvent.end();

    TraceEvent::Begin();
    if (TRACE) {
        Logger::D(TAG, " onScaleChangedScaled");
    }

    mWebViewClient->OnScaleChanged(mWebView, oldScale, newScale);
    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnShowCustomView(
    /* [in] */ IView* view,
    /* [in] */ IWebChromeClientCustomViewCallback* cb)
{
    VALIDATE_NOT_NULL(view);
    VALIDATE_NOT_NULL(cb);
    // ==================before translated======================
    // TraceEvent.begin();
    // if (mWebChromeClient != null) {
    //     if (TRACE) Log.d(TAG, "onShowCustomView");
    //     mWebChromeClient.onShowCustomView(view, cb);
    // }
    // TraceEvent.end();

    TraceEvent::Begin();
    if (mWebChromeClient != NULL) {
        if (TRACE) {
            Logger::D(TAG, "onShowCustomView");
        }
        mWebChromeClient->OnShowCustomView(view, cb);
    }

    TraceEvent::End();
    return NOERROR;
}

ECode WebViewContentsClientAdapter::OnHideCustomView()
{
    // ==================before translated======================
    // TraceEvent.begin();
    // if (mWebChromeClient != null) {
    //     if (TRACE) Log.d(TAG, "onHideCustomView");
    //     mWebChromeClient.onHideCustomView();
    // }
    // TraceEvent.end();

    TraceEvent::Begin();
    if (mWebChromeClient != NULL) {
        if (TRACE) {
            Logger::D(TAG, "onHideCustomView");
        }
        mWebChromeClient->OnHideCustomView();
    }

    TraceEvent::End();
    return NOERROR;
}

AutoPtr<IBitmap> WebViewContentsClientAdapter::GetDefaultVideoPoster()
{
    // ==================before translated======================
    // TraceEvent.begin();
    // Bitmap result = null;
    // if (mWebChromeClient != null) {
    //     if (TRACE) Log.d(TAG, "getDefaultVideoPoster");
    //     result = mWebChromeClient.getDefaultVideoPoster();
    // }
    // if (result == null) {
    //     // The ic_media_video_poster icon is transparent so we need to draw it on a gray
    //     // background.
    //     Bitmap poster = BitmapFactory.decodeResource(
    //             mWebView.getContext().getResources(),
    //             R.drawable.ic_media_video_poster);
    //     result = Bitmap.createBitmap(poster.getWidth(), poster.getHeight(), poster.getConfig());
    //     result.eraseColor(Color.GRAY);
    //     Canvas canvas = new Canvas(result);
    //     canvas.drawBitmap(poster, 0f, 0f, null);
    // }
    // TraceEvent.end();
    // return result;

    assert(0);
    TraceEvent::Begin();
    AutoPtr<IBitmap> result = NULL;
    if (mWebChromeClient != NULL) {
        if (TRACE) {
            Logger::D(TAG, "getDefaultVideoPoster");
        }
        mWebChromeClient->GetDefaultVideoPoster((IBitmap**)&result);
    }
    if (NULL == result) {
        // The ic_media_video_poster icon is transparent so we need to draw it on a gray
        // background.
        IView* viewTmp = IView::Probe(mWebView);
        AutoPtr<IContext> context;
        viewTmp->GetContext((IContext**)&context);

        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);

        AutoPtr<IBitmapFactory> factory;
        CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);

        AutoPtr<IBitmap> poster;
        factory->DecodeResource(resources, R::drawable::ic_media_video_poster, (IBitmap**)&poster);

        Int32 width = 0;
        Int32 height = 0;
        BitmapConfig bitmapConfig = 0;
        poster->GetWidth(&width);
        poster->GetHeight(&height);
        poster->GetConfig(&bitmapConfig);

        AutoPtr<IBitmapHelper> helper;
        //CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
        AutoPtr<IBitmap> bitmap;
        helper->CreateBitmap(width, height, bitmapConfig, (IBitmap**)&result);

        result->EraseColor(IColor::GRAY);
        AutoPtr<ICanvas> canvas;
        CCanvas::New(result, (ICanvas**)&canvas);
        canvas->DrawBitmap(poster, 0.0f, 0.0f, NULL);
    }

    TraceEvent::End();
    return result;
}

AutoPtr<IView> WebViewContentsClientAdapter::GetVideoLoadingProgressView()
{
    // ==================before translated======================
    // TraceEvent.begin();
    // View result;
    // if (mWebChromeClient != null) {
    //     if (TRACE) Log.d(TAG, "getVideoLoadingProgressView");
    //     result = mWebChromeClient.getVideoLoadingProgressView();
    // } else {
    //     result = null;
    // }
    // TraceEvent.end();
    // return result;

    TraceEvent::Begin();
    AutoPtr<IView> result = NULL;
    if (mWebChromeClient != NULL) {
        if (TRACE) {
            Logger::D(TAG, "getVideoLoadingProgressView");
        }
        mWebChromeClient->GetVideoLoadingProgressView((IView**)&result);
    }

    TraceEvent::End();
    return result;
}

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

