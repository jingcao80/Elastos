#include "elastos/droid/webkit/native/android_webview/AwContentViewClient.h"
#include "elastos/droid/webkit/native/base/CommandLine.h"
#include "elastos/droid/webkit/native/content/browser/ContentVideoView.h"
#include "elastos/droid/webkit/native/content/common/ContentSwitches.h"
#include "elastos/droid/webkit/native/android_webview/AwContents.h"
//TODO #include "elastos/droid/widget/CFrameLayout.h"
#include "elastos/droid/webkit/URLUtil.h"

using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::EIID_IViewOnAttachStateChangeListener;
//TODO using Elastos::Droid::Webkit::EIID_IWebChromeClientCustomViewCallback;
using Elastos::Droid::Webkit::URLUtil;
using Elastos::Droid::Webkit::Base::CommandLine;
using Elastos::Droid::Webkit::Content::Browser::ContentVideoView;
using Elastos::Droid::Webkit::Content::Common::ContentSwitches;
//TODO using Elastos::Droid::Widget::CFrameLayout;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

//=======================================================================================================
//         AwContentViewClient::AwContentVideoViewClient::InnerWebChromeClientCustomViewCallback
//=======================================================================================================
//TODO CAR_INTERFACE_IMPL(AwContentViewClient::AwContentVideoViewClient::InnerWebChromeClientCustomViewCallback, Object, IWebChromeClientCustomViewCallback);

AwContentViewClient::AwContentVideoViewClient::InnerWebChromeClientCustomViewCallback::InnerWebChromeClientCustomViewCallback(
    /* [in] */ AwContentVideoViewClient* owner)
    : mOwner(owner)
{
}

ECode AwContentViewClient::AwContentVideoViewClient::InnerWebChromeClientCustomViewCallback::OnCustomViewHidden()
{
    AutoPtr<ContentVideoView> contentVideoView = ContentVideoView::GetContentVideoView();
    if (contentVideoView != NULL)
        contentVideoView->ExitFullscreen(FALSE);

    return NOERROR;
}

//=======================================================================================================
//          AwContentViewClient::AwContentVideoViewClient::InnerViewOnAttachStateChangeListener
//=======================================================================================================
CAR_INTERFACE_IMPL(AwContentViewClient::AwContentVideoViewClient::InnerViewOnAttachStateChangeListener, Object, IViewOnAttachStateChangeListener);

AwContentViewClient::AwContentVideoViewClient::InnerViewOnAttachStateChangeListener::InnerViewOnAttachStateChangeListener(
    /* [in] */ AwContentVideoViewClient* owner,
    /* [in] */ IFrameLayout* viewGroup)
    : mOwner(owner)
    , mViewGroup(viewGroup)
{
}

//@Override
ECode AwContentViewClient::AwContentVideoViewClient::InnerViewOnAttachStateChangeListener::OnViewDetachedFromWindow(
    /* [in] */ IView* v)
{
    // Intentional no-op (see onDestroyContentVideoView).
    return NOERROR;
}

//@Override
ECode AwContentViewClient::AwContentVideoViewClient::InnerViewOnAttachStateChangeListener::OnViewAttachedToWindow(
    /* [in] */ IView* v)
{
    if (mOwner->mOwner->mAwContents->IsFullScreen())
    {
        return NOERROR;
    }

    AutoPtr<IViewGroup> viewGroup = IViewGroup::Probe(mViewGroup);
    viewGroup->AddView((IView*)mOwner->mOwner->mAwContents->EnterFullScreen());

    return NOERROR;
}

//===============================================================
//        AwContentViewClient::AwContentVideoViewClient
//===============================================================

AwContentViewClient::AwContentVideoViewClient::AwContentVideoViewClient(
    /* [in] */ AwContentViewClient* owner)
    : mOwner(owner)
{
}

//@Override
Boolean AwContentViewClient::AwContentVideoViewClient::OnShowCustomView(
    /* [in] */ IView* view)
{
    AutoPtr</*TODO IWebChromeClientCustomViewCallback*/IInterface> cb ;//TODO = new InnerWebChromeClientCustomViewCallback(this);
    // TODO(igsolla): remove the legacy path (kept as a fallback if things go awry).
    if (!AreHtmlControlsEnabled()) {
        OnShowCustomViewLegacy(view, cb);
    }
    else {
        OnShowCustomView(view, cb);
    }
    return TRUE;
}

void AwContentViewClient::AwContentVideoViewClient::OnShowCustomViewLegacy(
    /* [in] */ IView* view,
    /* [in] */ /*TODO IWebChromeClientCustomViewCallback*/IInterface* cb)
{
    mOwner->mAwContentsClient->OnShowCustomView(view, cb);
}

void AwContentViewClient::AwContentVideoViewClient::OnShowCustomView(
    /* [in] */ IView* view,
    /* [in] */ /*TODO IWebChromeClientCustomViewCallback*/IInterface* cb)
{
    AutoPtr<IFrameLayout> frameLayout;
    //TODO CFrameLayout::New(mContext, (IFrameLayout**)&frameLayout);
    AutoPtr<IViewGroup> viewGroup = IViewGroup::Probe(frameLayout);
    viewGroup->AddView(view);
    AutoPtr<IViewOnAttachStateChangeListener> listener = new InnerViewOnAttachStateChangeListener(this, frameLayout);
    AutoPtr<IView> v = IView::Probe(frameLayout);
    v->AddOnAttachStateChangeListener(listener);
    mOwner->mAwContentsClient->OnShowCustomView(v, cb);
}

//@Override
void AwContentViewClient::AwContentVideoViewClient::OnDestroyContentVideoView()
{
    if (AreHtmlControlsEnabled()) {
        mOwner->mAwContents->ExitFullScreen();
    }
    mOwner->mAwContentsClient->OnHideCustomView();
}

//@Override
AutoPtr<IView> AwContentViewClient::AwContentVideoViewClient::GetVideoLoadingProgressView()
{
    return mOwner->mAwContentsClient->GetVideoLoadingProgressView();
}

//===============================================================
//                    AwContentViewClient
//===============================================================

AwContentViewClient::AwContentViewClient(
    /* [in] */ AwContentsClient* awContentsClient,
    /* [in] */ AwSettings* awSettings,
    /* [in] */ AwContents* awContents,
    /* [in] */ IContext* context)
    : mAwContentsClient(awContentsClient)
    , mAwSettings(awSettings)
    , mAwContents(awContents)
    , mContext(context)
{
}

//@Override
void AwContentViewClient::OnBackgroundColorChanged(
    /* [in] */ Int32 color)
{
    mAwContentsClient->OnBackgroundColorChanged(color);
}

//@Override
void AwContentViewClient::OnStartContentIntent(
    /* [in] */ IContext* context,
    /* [in] */ const String& contentUrl)
{
    //  Callback when detecting a click on a content link.
    mAwContentsClient->ShouldOverrideUrlLoading(contentUrl);
}

//@Override
void AwContentViewClient::OnUpdateTitle(
    /* [in] */ const String& title)
{
    mAwContentsClient->OnReceivedTitle(title);
}

//@Override
Boolean AwContentViewClient::ShouldOverrideKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    return mAwContentsClient->ShouldOverrideKeyEvent(event);
}

//@Override
AutoPtr<ContentVideoViewClient> AwContentViewClient::GetContentVideoViewClient()
{
    AutoPtr<ContentVideoViewClient> cvvc = new AwContentVideoViewClient(this);
    return cvvc;
}

//@Override
Boolean AwContentViewClient::ShouldBlockMediaRequest(
    /* [in] */ const String& url)
{
    return mAwSettings != NULL ?  mAwSettings->GetBlockNetworkLoads() && URLUtil::IsNetworkUrl(url) : TRUE;
}

Boolean AwContentViewClient::AreHtmlControlsEnabled()
{
    return !CommandLine::GetInstance()->HasSwitch(
            ContentSwitches::DISABLE_OVERLAY_FULLSCREEN_VIDEO_SUBTITLE);
}

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
