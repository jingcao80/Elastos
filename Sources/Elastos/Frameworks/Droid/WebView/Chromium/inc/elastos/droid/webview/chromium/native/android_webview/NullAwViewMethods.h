#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_NULLAWVIEWMETHODS_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_NULLAWVIEWMETHODS_H__
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContents.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwViewMethods.h"

using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::View::InputMethod::IInputConnection;


namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * No-op implementation of {@link AwViewMethods} that follows the null object pattern.
 * This {@link NullAwViewMethods} is hooked up to the WebView in fullscreen mode, and
 * to the {@link FullScreenView} in embedded mode, but not to both at the same time.
 */
class NullAwViewMethods : public AwViewMethods
{
public:
    NullAwViewMethods(
        /* [in] */ AwContents* awContents,
        /* [in] */ AwContents::InternalAccessDelegate* internalAccessAdapter,
        /* [in] */ IView* containerView);

    //@Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    //@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //@Override
    CARAPI_(void) RequestFocus();

    //@Override
    CARAPI_(void) SetLayerType(
        /* [in] */ Int32 layerType,
        /* [in] */ IPaint* paint);

    //@Override
    CARAPI_(AutoPtr<IInputConnection>) OnCreateInputConnection(
        /* [in] */ IEditorInfo* outAttrs);

    //@Override
    CARAPI_(Boolean) OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event);

    //@Override
    CARAPI_(Boolean) DispatchKeyEvent(
        /* [in] */ IKeyEvent* event);

    //@Override
    CARAPI_(Boolean) OnTouchEvent(
        /* [in] */ IMotionEvent* event);

    //@Override
    CARAPI_(Boolean) OnHoverEvent(
        /* [in] */ IMotionEvent* event);

    //@Override
    CARAPI_(Boolean) OnGenericMotionEvent(
        /* [in] */ IMotionEvent* event);

    //@Override
    CARAPI_(void) OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    //@Override
    CARAPI_(void) OnAttachedToWindow();

    //@Override
    CARAPI_(void) OnDetachedFromWindow();

    //@Override
    CARAPI_(void) OnWindowFocusChanged(
        /* [in] */ Boolean hasWindowFocus);

    //@Override
    CARAPI_(void) OnFocusChanged(
        /* [in] */ Boolean focused,
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

    //@Override
    CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 ow,
        /* [in] */ Int32 oh);

    //@Override
    CARAPI_(void) OnVisibilityChanged(
        /* [in] */ IView* changedView,
        /* [in] */ Int32 visibility);

    //@Override
    CARAPI_(void) OnWindowVisibilityChanged(
        /* [in] */ Int32 visibility);

private:
    NullAwViewMethods(const NullAwViewMethods&);
    NullAwViewMethods& operator=(const NullAwViewMethods&);
    AwContents* mAwContents;
    AutoPtr<AwContents::InternalAccessDelegate> mInternalAccessAdapter;
    AutoPtr<IView> mContainerView;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_NULLAWVIEWMETHODS_H__
