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
