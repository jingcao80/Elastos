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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_FULLSCREENVIEW_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_FULLSCREENVIEW_H__
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/AbsoluteLayout.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContents.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwViewMethods.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::InputMethod::IInputConnection;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::Widget::AbsoluteLayout;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * A view that is used to render the web contents in fullscreen mode, ie.
 * html controls and subtitles, over the {@link ContentVideoView}.
 */
class FullScreenView
    : public AbsoluteLayout
{
private:
    // AwContents.InternalAccessDelegate implementation --------------------------------------
    class InternalAccessAdapter
    : public AwContents::InternalAccessDelegate
    {
    public:
        InternalAccessAdapter(
            /* [in] */ FullScreenView* owner);

        //@Override
        CARAPI_(Boolean) DrawChild(
            /* [in] */ ICanvas* canvas,
            /* [in] */ IView* child,
            /* [in] */ Int64 drawingTime);

        //@Override
        CARAPI_(Boolean) Super_onKeyUp(
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event);

        //@Override
        CARAPI_(Boolean) Super_dispatchKeyEventPreIme(
            /* [in] */ IKeyEvent* event);

        //@Override
        CARAPI_(Boolean) Super_dispatchKeyEvent(
            /* [in] */ IKeyEvent* event);

        //@Override
        CARAPI_(Boolean) Super_onGenericMotionEvent(
            /* [in] */ IMotionEvent* event);

        //@Override
        CARAPI_(void) Super_onConfigurationChanged(
            /* [in] */ IConfiguration* newConfig);

        //@Override
        CARAPI_(Int32) Super_getScrollBarStyle();

        //@Override
        CARAPI_(Boolean) AwakenScrollBars();

        //@Override
        CARAPI_(Boolean) Super_awakenScrollBars(
            /* [in] */ Int32 startDelay,
            /* [in] */ Boolean invalidate);

        //@Override
        CARAPI OnScrollChanged(
            /* [in] */ Int32 lPix,
            /* [in] */ Int32 tPix,
            /* [in] */ Int32 oldlPix,
            /* [in] */ Int32 oldtPix);

        //@Override
        CARAPI_(void) OverScrollBy(
            /* [in] */ Int32 deltaX,
            /* [in] */ Int32 deltaY,
            /* [in] */ Int32 scrollX,
            /* [in] */ Int32 scrollY,
            /* [in] */ Int32 scrollRangeX,
            /* [in] */ Int32 scrollRangeY,
            /* [in] */ Int32 maxOverScrollX,
            /* [in] */ Int32 maxOverScrollY,
            /* [in] */ Boolean isTouchEvent);

        //@Override
        CARAPI_(void) Super_scrollTo(
            /* [in] */ Int32 scrollX,
            /* [in] */ Int32 scrollY);

        //@Override
        CARAPI_(void) SetMeasuredDimension(
            /* [in] */ Int32 measuredWidth,
            /* [in] */ Int32 measuredHeight);

    private:
        FullScreenView* mOwner;
    };

public:
    FullScreenView(
        /* [in] */ IContext* context,
        /* [in] */ AwViewMethods* awViewMethods);

    CARAPI_(AutoPtr<InternalAccessAdapter>) GetInternalAccessAdapter();

    CARAPI_(void) SetAwViewMethods(
        /* [in] */ AwViewMethods* awViewMethods);

    //@Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    //@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //@Override
    CARAPI RequestFocus(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI SetLayerType(
        /* [in] */ Int32 layerType,
        /* [in] */ IPaint* paint);

    //@Override
    CARAPI_(AutoPtr<IInputConnection>) OnCreateInputConnection(IEditorInfo* outAttrs);

    //@Override
    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

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
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    //@Override
    CARAPI OnDetachedFromWindow();

    //@Override
    CARAPI OnWindowFocusChanged(
        /* [in] */ Boolean hasWindowFocus);

    //@Override
    CARAPI_(void) OnFocusChanged(
        /* [in] */ Boolean focused,
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

    //@Override
    CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 ow,
        /* [in] */ Int32 oh);

    //@Override
    CARAPI OnWindowVisibilityChanged(
        /* [in] */ Int32 visibility);

protected:
    //@Override
    CARAPI OnAttachedToWindow();

    //@Override
    CARAPI OnVisibilityChanged(
        /* [in] */ IView* changedView,
        /* [in] */ Int32 visibility);

private:
    AutoPtr<AwViewMethods> mAwViewMethods;
    AutoPtr<InternalAccessAdapter> mInternalAccessAdapter;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_FULLSCREENVIEW_H__
