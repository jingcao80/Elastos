
#include "elastos/droid/webkit/WebViewFragment.h"
#include "elastos/droid/webkit/CWebView.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::EIID_IFragment;

namespace Elastos {
namespace Droid {
namespace Webkit {

//===============================================================
//                      WebViewFragment
//===============================================================

CAR_INTERFACE_IMPL(WebViewFragment, Fragment, IWebViewFragment);

WebViewFragment::WebViewFragment()
{
}

ECode WebViewFragment::constructor()
{
    return NOERROR;
}

/**
 * Called to instantiate the view. Creates and returns the WebView.
 */
ECode WebViewFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    if (mWebView != NULL) {
        mWebView->Destroy();
    }
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    mWebView = NULL;
    CWebView::New(IContext::Probe(activity), (IWebView**)&mWebView);
    mIsWebViewAvailable = TRUE;
    *view = IView::Probe(mWebView);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

/**
 * Called when the fragment is visible to the user and actively running. Resumes the WebView.
 */
ECode WebViewFragment::OnPause()
{
    Fragment::OnPause();
    mWebView->OnPause();
    return NOERROR;
}

/**
 * Called when the fragment is no longer resumed. Pauses the WebView.
 */
ECode WebViewFragment::OnResume()
{
    mWebView->OnResume();
    Fragment::OnResume();
    return NOERROR;
}

/**
 * Called when the WebView has been detached from the fragment.
 * The WebView is no longer available after this time.
 */
ECode WebViewFragment::OnDestroyView()
{
    mIsWebViewAvailable = FALSE;
    Fragment::OnDestroyView();
    return NOERROR;
}

/**
 * Called when the fragment is no longer in use. Destroys the internal state of the WebView.
 */
ECode WebViewFragment::OnDestroy()
{
    if (mWebView != NULL) {
        mWebView->Destroy();
        mWebView = NULL;
    }
    Fragment::OnDestroy();
    return NOERROR;
}

/**
 * Gets the WebView.
 */
ECode WebViewFragment::GetWebView(
    /* [out] */ IWebView** webView)
{
    VALIDATE_NOT_NULL(webView);
    *webView = mIsWebViewAvailable ? mWebView : NULL;
    REFCOUNT_ADD(*webView);
    return NOERROR;
}

ECode WebViewFragment::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "WebViewFragment";
    return NOERROR;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos
