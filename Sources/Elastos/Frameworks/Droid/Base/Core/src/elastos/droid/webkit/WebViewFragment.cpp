
#include "elastos/droid/webkit/WebViewFragment.h"
#include "elastos/droid/webkit/CWebView.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::EIID_IFragment;

namespace Elastos {
namespace Droid {
namespace Webkit {

//===============================================================
//                   InnerWebViewFragment
//===============================================================
// InnerWebViewFragment::InnerWebViewFragment()
// {
// }

// IFRAGMENT_METHODS_IMPL(InnerWebViewFragment, Fragment);

//===============================================================
//                      WebViewFragment
//===============================================================

CAR_INTERFACE_IMPL(WebViewFragment, Object, IWebViewFragment);

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
    assert(0);
    // TODO
    // VALIDATE_NOT_NULL(view);
    // if (mWebView != NULL) {
    //     mWebView->Destroy();
    // }
    // AutoPtr<IActivity> activity;
    // GetActivity((IActivity**)&activity);
    // mWebView = NULL;
    // CWebView::New(activity, (IWebView**)&mWebView);
    // mIsWebViewAvailable = TRUE;
    // *view = (IView*)mWebView.Get();
    // REFCOUNT_ADD(*view);
    return E_NOT_IMPLEMENTED;
}

/**
 * Called when the fragment is visible to the user and actively running. Resumes the WebView.
 */
ECode WebViewFragment::OnPause()
{
    assert(0);
    // TODO
    // InnerWebViewFragment::OnPause();
    // mWebView->OnPause();
    return E_NOT_IMPLEMENTED;
}

/**
 * Called when the fragment is no longer resumed. Pauses the WebView.
 */
ECode WebViewFragment::OnResume()
{
    assert(0);
    // TODO
    // mWebView->OnResume();
    // InnerWebViewFragment::OnResume();
    return E_NOT_IMPLEMENTED;
}

/**
 * Called when the WebView has been detached from the fragment.
 * The WebView is no longer available after this time.
 */
ECode WebViewFragment::OnDestroyView()
{
    assert(0);
    // TODO
    // mIsWebViewAvailable = FALSE;
    // InnerWebViewFragment::OnDestroyView();
    return E_NOT_IMPLEMENTED;
}

/**
 * Called when the fragment is no longer in use. Destroys the internal state of the WebView.
 */
ECode WebViewFragment::OnDestroy()
{
    assert(0);
    // TODO
    // if (mWebView != NULL) {
    //     mWebView->Destroy();
    //     mWebView = NULL;
    // }
    // InnerWebViewFragment::OnDestroy();
    return E_NOT_IMPLEMENTED;
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
