#ifndef __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWCONTENTVIEWCLIENT_H__
#define __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWCONTENTVIEWCLIENT_H__
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/content/browser/ContentVideoViewClient.h"
#include "elastos/droid/webkit/native/content/browser/ContentViewClient.h"
#include "elastos/droid/webkit/native/android_webview/AwSettings.h"
#include "elastos/droid/webkit/native/android_webview/AwContentsClient.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnAttachStateChangeListener;
using Elastos::Droid::Webkit::IWebChromeClientCustomViewCallback;
using Elastos::Droid::Widget::IFrameLayout;

using Elastos::Droid::Webkit::Content::Browser::ContentVideoViewClient;
using Elastos::Droid::Webkit::Content::Browser::ContentViewClient;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {
    class AwContents;

/**
 * ContentViewClient implementation for WebView
 */
class AwContentViewClient : public ContentViewClient
{
public:
    class AwContentVideoViewClient : public ContentVideoViewClient
    {
    public:
        class InnerWebChromeClientCustomViewCallback
            : public Object
            , public IWebChromeClientCustomViewCallback
        {
        public:
            CAR_INTERFACE_DECL();
            InnerWebChromeClientCustomViewCallback(
                /* [in] */ AwContentVideoViewClient* owner);

            CARAPI OnCustomViewHidden();

        private:
            AwContentVideoViewClient* mOwner;
        };

        class InnerViewOnAttachStateChangeListener
            : public Object
            , public IViewOnAttachStateChangeListener
        {
        public:
            CAR_INTERFACE_DECL();
            InnerViewOnAttachStateChangeListener(
                /* [in] */ AwContentVideoViewClient* owner,
                /* [in] */ IFrameLayout* viewGroup);

            //@Override
            CARAPI OnViewDetachedFromWindow(
                /* [in] */ IView* v);

            //@Override
            CARAPI OnViewAttachedToWindow(
                /* [in] */ IView* v);

        private:
            AwContentVideoViewClient* mOwner;
            IFrameLayout* mViewGroup;
        };

    public:
        AwContentVideoViewClient(
            /* [in] */ AwContentViewClient* owner);

        //@Override
        CARAPI_(Boolean) OnShowCustomView(
            /* [in] */ IView* view);

        //@Override
        CARAPI_(void) OnDestroyContentVideoView();

        //@Override
        CARAPI_(AutoPtr<IView>) GetVideoLoadingProgressView();

    private:
        CARAPI_(void) OnShowCustomViewLegacy(
            /* [in] */ IView* view,
            /* [in] */ IWebChromeClientCustomViewCallback* cb);

        CARAPI_(void) OnShowCustomView(
            /* [in] */ IView* view,
            /* [in] */ IWebChromeClientCustomViewCallback* cb);

    private:
        AwContentViewClient* mOwner;
    };

public:
    AwContentViewClient(
        /* [in] */ AwContentsClient* awContentsClient,
        /* [in] */ AwSettings* awSettings,
        /* [in] */ AwContents* awContents,
        /* [in] */ IContext* context);

    //@Override
    CARAPI_(void) OnBackgroundColorChanged(
        /* [in] */ Int32 color);

    //@Override
    CARAPI_(void) OnStartContentIntent(
        /* [in] */ IContext* context,
        /* [in] */ const String& contentUrl);

    //@Override
    CARAPI_(void) OnUpdateTitle(
        /* [in] */ const String& title);

    //@Override
    CARAPI_(Boolean) ShouldOverrideKeyEvent(
        /* [in] */ IKeyEvent* event);

    //@Override
    CARAPI_(AutoPtr<ContentVideoViewClient>) GetContentVideoViewClient();

    //@Override
    CARAPI_(Boolean) ShouldBlockMediaRequest(
        /* [in] */ const String& url);

private:
    static CARAPI_(Boolean) AreHtmlControlsEnabled();

private:
    AutoPtr<AwContentsClient> mAwContentsClient;
    AutoPtr<AwSettings> mAwSettings;
    AwContents* mAwContents;
    AutoPtr<IContext> mContext;
};

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWCONTENTVIEWCLIENT_H__
