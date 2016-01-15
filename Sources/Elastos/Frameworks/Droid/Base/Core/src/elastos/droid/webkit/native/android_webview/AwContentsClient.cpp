#include "elastos/droid/webkit/native/android_webview/AwContentsClient.h"
#include "elastos/droid/webkit/native/net/NetError.h"
#include "elastos/droid/webkit/native/android_webview/AwContentsStatics.h"
#include "elastos/droid/webkit/native/android_webview/AwContentsClientBridge.h"
#include "elastos/droid/webkit/native/android_webview/AwContentsClientCallbackHelper.h"
#include "elastos/droid/webkit/native/android_webview/ErrorCodeConversionHelper.h"
#include "elastos/droid/os/CLooperHelper.h"

using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Webkit::Net::NetError;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

//===============================================================
//            AwContentsClient::AwWebContentsObserver
//===============================================================

AwContentsClient::AwWebContentsObserver::AwWebContentsObserver(
    /* [in] */ AwContentsClient* owner,
    /* [in] */ ContentViewCore* contentViewCore)
    : WebContentsObserverElastos(contentViewCore)
    , mOwner(owner)
{
}

//@Override
ECode AwContentsClient::AwWebContentsObserver::DidFinishLoad(
    /* [in] */ Int64 frameId,
    /* [in] */ const String& validatedUrl,
    /* [in] */ Boolean isMainFrame)
{
    String unreachableWebDataUrl = AwContentsStatics::GetUnreachableWebDataUrl();
    Boolean isErrorUrl =
            (!unreachableWebDataUrl.IsNull() && unreachableWebDataUrl.Equals(validatedUrl));
    if (isMainFrame && !isErrorUrl) {
        mOwner->OnPageFinished(validatedUrl);
    }
    return NOERROR;
}

//@Override
ECode AwContentsClient::AwWebContentsObserver::DidFailLoad(
    /* [in] */ Boolean isProvisionalLoad,
    /* [in] */ Boolean isMainFrame,
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& description,
    /* [in] */ const String& failingUrl)
{
    if (isMainFrame) {
        if (errorCode != NetError::ERR_ABORTED) {
            // This error code is generated for the following reasons:
            // - WebView.stopLoading is called,
            // - the navigation is intercepted by the embedder via shouldOverrideNavigation.
            //
            // The Android WebView does not notify the embedder of these situations using
            // this error code with the WebViewClient.onReceivedError callback.
            mOwner->OnReceivedError(
                    ErrorCodeConversionHelper::ConvertErrorCode(errorCode), description,
                            failingUrl);
        }
        // Need to call onPageFinished after onReceivedError (if there is an error) for
        // backwards compatibility with the classic webview.
        mOwner->OnPageFinished(failingUrl);
    }
    return NOERROR;
}

//@Override
ECode AwContentsClient::AwWebContentsObserver::DidNavigateMainFrame(
    /* [in] */ const String& url,
    /* [in] */ const String& baseUrl,
    /* [in] */ Boolean isNavigationToDifferentPage,
    /* [in] */ Boolean isFragmentNavigation)
{
    // This is here to emulate the Classic WebView firing onPageFinished for main frame
    // navigations where only the hash fragment changes.
    if (isFragmentNavigation) {
        mOwner->OnPageFinished(url);
    }
    return NOERROR;
}

//@Override
ECode AwContentsClient::AwWebContentsObserver::DidNavigateAnyFrame(
    /* [in] */ const String& url,
    /* [in] */ const String& baseUrl,
    /* [in] */ Boolean isReload)
{
    mOwner->DoUpdateVisitedHistory(url, isReload);
    return NOERROR;
}

//===============================================================
//                      AwContentsClient
//===============================================================

const Int32 AwContentsClient::INVALID_COLOR;

AwContentsClient::AwContentsClient()
{
    AutoPtr<ILooperHelper> helper;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&helper);
    AutoPtr<ILooper> looper;
    helper->GetMyLooper((ILooper**)&looper);
    Init(looper);
}

// Alllow injection of the callback thread, for testing.
AwContentsClient::AwContentsClient(
    /* [in] */ ILooper* looper)
{
    Init(looper);
}

void AwContentsClient::Init(
    /* [in] */ ILooper* looper)
{
    mCallbackHelper = new AwContentsClientCallbackHelper(looper, this);
    mCachedRendererBackgroundColor = INVALID_COLOR;
}

ECode AwContentsClient::InstallWebContentsObserver(
    /* [in] */ ContentViewCore* contentViewCore)
{
    if (mWebContentsObserver != NULL) {
        mWebContentsObserver->DetachFromWebContents();
    }
    mWebContentsObserver = new AwWebContentsObserver(this, contentViewCore);
    return NOERROR;
}

AwContentsClientCallbackHelper* AwContentsClient::GetCallbackHelper()
{
    return mCallbackHelper;
}

Int32 AwContentsClient::GetCachedRendererBackgroundColor()
{
    assert(IsCachedRendererBackgroundColorValid());
    return mCachedRendererBackgroundColor;
}

Boolean AwContentsClient::IsCachedRendererBackgroundColorValid()
{
    return mCachedRendererBackgroundColor != INVALID_COLOR;
}

ECode AwContentsClient::OnBackgroundColorChanged(
    /* [in] */ Int32 color)
{
    // Avoid storing the sentinal INVALID_COLOR (note that both 0 and 1 are both
    // fully transparent so this transpose makes no visible difference).
    mCachedRendererBackgroundColor = (color == INVALID_COLOR ? 1 : color);
    return NOERROR;
}


// TODO(sgurun): Make abstract once this has rolled in downstream.
ECode AwContentsClient::OnReceivedClientCertRequest(
    /* [in] */ AwContentsClientBridge::ClientCertificateRequestCallback* callback,
    /* [in] */ ArrayOf<String>* keyTypes,
    /* [in] */ ArrayOf<IPrincipal*>* principals,
    /* [in] */ const String& host,
    /* [in] */ Int32 port)
{
    return NOERROR;
}


// TODO(joth): Make abstract once this has rolled in downstream.
ECode AwContentsClient::ShowFileChooser(
    /* [in] */ /*TODO IValueCallback*/IInterface* uploadFilePathsCallback,
    /* [in] */ FileChooserParams* fileChooserParams)
{
    return NOERROR;
}

// TODO(michaelbai): Change the abstract once merged
ECode AwContentsClient::OnPermissionRequest(
    /* [in] */ AwPermissionRequest* awPermissionRequest)
{
    return NOERROR;
}

// TODO(michaelbai): Change the abstract once merged
ECode AwContentsClient::OnPermissionRequestCanceled(
    /* [in] */ AwPermissionRequest* awPermissionRequest)
{
    return NOERROR;
}


// TODO (michaelbai): Remove this method once the same method remove from
// WebViewContentsClientAdapter.
ECode AwContentsClient::OnShowCustomView(
    /* [in] */ IView* view,
    /* [in] */ Int32 requestedOrientation,
    /* [in] */ /*TODO IWebChromeClientCustomViewCallback*/IInterface* callback)
{
    return NOERROR;
}

// TODO (michaelbai): This method should be abstract, having empty body here
// makes the merge to the Android easy.
ECode AwContentsClient::OnShowCustomView(
    /* [in] */ IView* view,
    /* [in] */ /*TODO IWebChromeClientCustomViewCallback*/IInterface* callback)
{
    return OnShowCustomView(view, IActivityInfo::SCREEN_ORIENTATION_UNSPECIFIED, callback);
}

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
