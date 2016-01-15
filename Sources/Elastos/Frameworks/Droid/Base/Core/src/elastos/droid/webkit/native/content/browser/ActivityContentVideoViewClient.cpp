
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/webkit/native/content/browser/ActivityContentVideoViewClient.h"
#include "elastos/droid/os/Build.h"
//#include "elastos/droid/widget/CFrameLayoutLayoutParams.h"

using Elastos::Droid::Os::Build;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::EIID_IView;
//using Elastos::Droid::Widget::CFrameLayoutLayoutParams;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;

namespace Elastos {
namespace Droid {
namespace Webkit {
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
    AutoPtr<IFrameLayout> decor;
    window->GetDecorView((IView**)&decor);
    AutoPtr<IFrameLayoutLayoutParams> params;
    assert(0);
    // CFrameLayoutLayoutParams::New(
    //     IViewGroupLayoutParams::MATCH_PARENT,
    //     IViewGroupLayoutParams::MATCH_PARENT,
    //     IGravity::CENTER,
    //     (IFrameLayoutLayoutParams**)&params);
    AutoPtr<IView> decorView = IView::Probe(decor);
    // decor->AddView(view, 0, params);
    SetSystemUiVisibility(decorView, TRUE);
    mView = view;
    return TRUE;
}

//@Override
void ActivityContentVideoViewClient::OnDestroyContentVideoView()
{
    AutoPtr<IWindow> window;
    mActivity->GetWindow((IWindow**)&window);
    AutoPtr<IFrameLayout> decor;
    window->GetDecorView((IView**)&decor);
    assert(0);
//    decor->RemoveView(mView);
//    SetSystemUiVisibility(decor, FALSE);
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
    assert(0);
    // if (enterFullscreen) {
    //     window->SetFlags(
    //             IWindowManagerLayoutParams::FLAG_FULLSCREEN,
    //             IWindowManagerLayoutParams::FLAG_FULLSCREEN);
    // }
    // else {
    //     window->ClearFlags(IWindowManagerLayoutParams::FLAG_FULLSCREEN);
    // }

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
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
