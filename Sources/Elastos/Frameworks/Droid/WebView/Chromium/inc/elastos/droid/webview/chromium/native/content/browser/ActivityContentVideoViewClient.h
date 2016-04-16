
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_ACTIVITYCONTENTVIDEOVIEWCLIENT_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_ACTIVITYCONTENTVIDEOVIEWCLIENT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentVideoViewClient.h"

// import android.annotation.SuppressLint;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::Widget::IFrameLayout;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
 * Uses an existing Activity to handle displaying video in full screen.
 */
class ActivityContentVideoViewClient
    : public ContentVideoViewClient
{
public:
    ActivityContentVideoViewClient(
        /* [in] */ IActivity* activity);

    //@Override
    CARAPI_(Boolean) OnShowCustomView(
        /* [in] */ IView* view);

    //@Override
    CARAPI_(void) OnDestroyContentVideoView();

    //@Override
    CARAPI_(AutoPtr<IView>) GetVideoLoadingProgressView();

private:
    /**
     * Returns the system ui visibility after entering or exiting fullscreen.
     * @param view The decor view belongs to the activity window
     * @param enterFullscreen True if video is going fullscreen, or false otherwise.
     */
    //@SuppressLint("InlinedApi")
    CARAPI_(void) SetSystemUiVisibility(
        /* [in] */ IView* view,
        /* [in] */ Boolean enterFullscreen);

private:
    const AutoPtr<IActivity> mActivity;
    AutoPtr<IView> mView;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_ACTIVITYCONTENTVIDEOVIEWCLIENT_H__
