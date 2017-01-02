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

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ActivityContentVideoViewClient.h"
#include "elastos/droid/os/Build.h"

using Elastos::Droid::Os::Build;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::Widget::CFrameLayoutLayoutParams;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

ActivityContentVideoViewClient::ActivityContentVideoViewClient(
    /* [in] */ IActivity* activity)
    : mActivity(activity)
{
}

//@Override
Boolean ActivityContentVideoViewClient::OnShowCustomView(
    /* [in] */ IView* view)
{
    AutoPtr<IWindow> window;
    mActivity->GetWindow((IWindow**)&window);
    AutoPtr<IView> decorView;
    window->GetDecorView((IView**)&decorView);
    IFrameLayout* decor = IFrameLayout::Probe(decorView);
    AutoPtr<IFrameLayoutLayoutParams> params;
    CFrameLayoutLayoutParams::New(
         IViewGroupLayoutParams::MATCH_PARENT,
         IViewGroupLayoutParams::MATCH_PARENT,
         IGravity::CENTER,
         (IFrameLayoutLayoutParams**)&params);
    IViewGroup::Probe(decor)->AddView(view, 0, IViewGroupLayoutParams::Probe(params));
    SetSystemUiVisibility(decorView, TRUE);
    mView = view;
    return TRUE;
}

//@Override
void ActivityContentVideoViewClient::OnDestroyContentVideoView()
{
    AutoPtr<IWindow> window;
    mActivity->GetWindow((IWindow**)&window);
    AutoPtr<IView> decorView;
    window->GetDecorView((IView**)&decorView);
    IFrameLayout* decor = IFrameLayout::Probe(decorView);
    IViewGroup::Probe(decor)->RemoveView(mView);
    SetSystemUiVisibility(IView::Probe(decor), FALSE);
    mView = NULL;
}

//@Override
AutoPtr<IView> ActivityContentVideoViewClient::GetVideoLoadingProgressView()
{
    return NULL;
}

/**
 * Returns the system ui visibility after entering or exiting fullscreen.
 * @param view The decor view belongs to the activity window
 * @param enterFullscreen True if video is going fullscreen, or false otherwise.
 */
//@SuppressLint("InlinedApi")
void ActivityContentVideoViewClient::SetSystemUiVisibility(
    /* [in] */ IView* view,
    /* [in] */ Boolean enterFullscreen)
{
    AutoPtr<IWindow> window;
    mActivity->GetWindow((IWindow**)&window);
    if (enterFullscreen) {
        window->SetFlags(
                IWindowManagerLayoutParams::FLAG_FULLSCREEN,
                IWindowManagerLayoutParams::FLAG_FULLSCREEN);
    }
    else {
        window->ClearFlags(IWindowManagerLayoutParams::FLAG_FULLSCREEN);
    }

    if (Build::VERSION::SDK_INT < Build::VERSION_CODES::KITKAT) {
        return;
    }

    Int32 systemUiVisibility;
    view->GetSystemUiVisibility(&systemUiVisibility);
    Int32 flags = IView::SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
            | IView::SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
            | IView::SYSTEM_UI_FLAG_HIDE_NAVIGATION
            | IView::SYSTEM_UI_FLAG_FULLSCREEN
            | IView::SYSTEM_UI_FLAG_IMMERSIVE_STICKY;
    if (enterFullscreen) {
        systemUiVisibility |= flags;
    }
    else {
        systemUiVisibility &= ~flags;
    }

    view->SetSystemUiVisibility(systemUiVisibility);
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
