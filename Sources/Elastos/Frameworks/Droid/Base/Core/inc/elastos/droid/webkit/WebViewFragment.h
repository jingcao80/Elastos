#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEWFRAGMENT_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEWFRAGMENT_H__

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Fragment.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::Fragment;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::App::IFragmentManagerImpl;
using Elastos::Droid::App::IFragmentSavedState;
using Elastos::Droid::App::ILoaderManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::ILayoutInflater;

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * A fragment that displays a WebView.
 * <p>
 * The WebView is automically paused or resumed when the Fragment is paused or resumed.
 */
class WebViewFragment
    : public Fragment
    , public IWebViewFragment
{
public:
    CAR_INTERFACE_DECL();

    WebViewFragment();

    CARAPI constructor();

    /**
     * Called to instantiate the view. Creates and returns the WebView.
     */
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** view);

    /**
     * Called when the fragment is visible to the user and actively running. Resumes the WebView.
     */
    CARAPI OnPause();

    /**
     * Called when the fragment is no longer resumed. Pauses the WebView.
     */
    CARAPI OnResume();

    /**
     * Called when the WebView has been detached from the fragment.
     * The WebView is no longer available after this time.
     */
    CARAPI OnDestroyView();

    /**
     * Called when the fragment is no longer in use. Destroys the internal state of the WebView.
     */
    CARAPI OnDestroy();

    /**
     * Gets the WebView.
     */
    CARAPI GetWebView(
        /* [out] */ IWebView** webView);

    CARAPI ToString(
        /* [out] */ String* info);

private:
    AutoPtr<IWebView> mWebView;
    Boolean mIsWebViewAvailable;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_WEBVIEWFRAGMENT_H__
