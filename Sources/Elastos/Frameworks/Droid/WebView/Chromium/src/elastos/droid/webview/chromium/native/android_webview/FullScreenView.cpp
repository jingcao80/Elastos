#include "elastos/droid/webkit/webview/chromium/native/android_webview/FullScreenView.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

//===============================================================
//             FullScreenView::InternalAccessAdapter
//===============================================================

FullScreenView::InternalAccessAdapter::InternalAccessAdapter(
    /* [in] */ FullScreenView* owner)
    : mOwner(owner)
{
}

//@Override
Boolean FullScreenView::InternalAccessAdapter::DrawChild(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IView* child,
    /* [in] */ Int64 drawingTime)
{
    // Intentional no-op
    return FALSE;
}

//@Override
Boolean FullScreenView::InternalAccessAdapter::Super_onKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
{
    //return FullScreenView.super.onKeyUp(keyCode, event);
    Boolean resTmp = FALSE;
    ((AbsoluteLayout*)mOwner)->OnKeyUp(keyCode, event, &resTmp);
    return resTmp;
}

//@Override
Boolean FullScreenView::InternalAccessAdapter::Super_dispatchKeyEventPreIme(
    /* [in] */ IKeyEvent* event)
{
    //return FullScreenView.super.dispatchKeyEventPreIme(event);
    Boolean resTmp = FALSE;
    ((AbsoluteLayout*)mOwner)->DispatchKeyEventPreIme(event, &resTmp);
    return resTmp;
}

//@Override
Boolean FullScreenView::InternalAccessAdapter::Super_dispatchKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    //return FullScreenView.super.dispatchKeyEvent(event);
    Boolean resTmp = FALSE;
    ((AbsoluteLayout*)mOwner)->DispatchKeyEventPreIme(event, &resTmp);
    return resTmp;
}

//@Override
Boolean FullScreenView::InternalAccessAdapter::Super_onGenericMotionEvent(
    /* [in] */ IMotionEvent* event)
{
    //return FullScreenView.super.onGenericMotionEvent(event);
    Boolean resTmp = FALSE;
    ((AbsoluteLayout*)mOwner)->OnGenericMotionEvent(event, &resTmp);
    return resTmp;
}

//@Override
void FullScreenView::InternalAccessAdapter::Super_onConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    // Intentional no-op
}

//@Override
Int32 FullScreenView::InternalAccessAdapter::Super_getScrollBarStyle()
{
    //return FullScreenView.super.getScrollBarStyle();
    Int32 resTmp = 0;
    ((AbsoluteLayout*)mOwner)->GetScrollBarStyle(&resTmp);
    return resTmp;
}

//@Override
Boolean FullScreenView::InternalAccessAdapter::AwakenScrollBars()
{
    return FALSE;
}

//@Override
Boolean FullScreenView::InternalAccessAdapter::Super_awakenScrollBars(
    /* [in] */ Int32 startDelay,
    /* [in] */ Boolean invalidate)
{
    return FALSE;
}

//@Override
void FullScreenView::InternalAccessAdapter::OnScrollChanged(
    /* [in] */ Int32 lPix,
    /* [in] */ Int32 tPix,
    /* [in] */ Int32 oldlPix,
    /* [in] */ Int32 oldtPix)
{
    // Intentional no-op.
}

//@Override
void FullScreenView::InternalAccessAdapter::OverScrollBy(
    /* [in] */ Int32 deltaX,
    /* [in] */ Int32 deltaY,
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY,
    /* [in] */ Int32 scrollRangeX,
    /* [in] */ Int32 scrollRangeY,
    /* [in] */ Int32 maxOverScrollX,
    /* [in] */ Int32 maxOverScrollY,
    /* [in] */ Boolean isTouchEvent)
{
    // Intentional no-op.
}

//@Override
void FullScreenView::InternalAccessAdapter::Super_scrollTo(
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY)
{
    // Intentional no-op.
}

//@Override
void FullScreenView::InternalAccessAdapter::SetMeasuredDimension(
    /* [in] */ Int32 measuredWidth,
    /* [in] */ Int32 measuredHeight)
{
    mOwner->SetMeasuredDimension(measuredWidth, measuredHeight);
}

//===============================================================
//                      FullScreenView
//===============================================================

FullScreenView::FullScreenView(
    /* [in] */ IContext* context,
    /* [in] */ AwViewMethods* awViewMethods)
{
    AbsoluteLayout::constructor(context);
    SetAwViewMethods(awViewMethods);
    mInternalAccessAdapter = new FullScreenView::InternalAccessAdapter(this);
}

AutoPtr<FullScreenView::InternalAccessAdapter> FullScreenView::GetInternalAccessAdapter()
{
    return mInternalAccessAdapter;
}

void FullScreenView::SetAwViewMethods(
    /* [in] */ AwViewMethods* awViewMethods)
{
    mAwViewMethods = awViewMethods;
}

//@Override
void FullScreenView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    mAwViewMethods->OnDraw(canvas);
}

//@Override
ECode FullScreenView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    mAwViewMethods->OnMeasure(widthMeasureSpec, heightMeasureSpec);
    return NOERROR;
}

//@Override
ECode FullScreenView::RequestFocus(
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mAwViewMethods->RequestFocus();
    return AbsoluteLayout::RequestFocus(direction, previouslyFocusedRect, result);
}

//@Override
ECode FullScreenView::SetLayerType(
    /* [in] */ Int32 layerType,
    /* [in] */ IPaint* paint)
{
    AbsoluteLayout::SetLayerType(layerType, paint);
    mAwViewMethods->SetLayerType(layerType, paint);
    return NOERROR;
}

//@Override
AutoPtr<IInputConnection> FullScreenView::OnCreateInputConnection(
    /* [in] */ IEditorInfo* outAttrs)
{
    return mAwViewMethods->OnCreateInputConnection(outAttrs);
}

//@Override
ECode FullScreenView::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAwViewMethods->OnKeyUp(keyCode, event);
    return NOERROR;
}

//@Override
Boolean FullScreenView::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    return mAwViewMethods->DispatchKeyEvent(event);
}

//@Override
Boolean FullScreenView::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    return mAwViewMethods->OnTouchEvent(event);
}

//@Override
Boolean FullScreenView::OnHoverEvent(
    /* [in] */ IMotionEvent* event)
{
    return mAwViewMethods->OnHoverEvent(event);
}

//@Override
Boolean FullScreenView::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event)
{
    return mAwViewMethods->OnGenericMotionEvent(event);
}

//@Override
ECode FullScreenView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    mAwViewMethods->OnConfigurationChanged(newConfig);
    return NOERROR;
}

//@Override
ECode FullScreenView::OnAttachedToWindow()
{
    AbsoluteLayout::OnAttachedToWindow();
    mAwViewMethods->OnAttachedToWindow();
    return NOERROR;
}

//@Override
ECode FullScreenView::OnDetachedFromWindow()
{
    AbsoluteLayout::OnDetachedFromWindow();
    mAwViewMethods->OnDetachedFromWindow();
    return NOERROR;
}

//@Override
ECode FullScreenView::OnWindowFocusChanged(
    /* [in] */ Boolean hasWindowFocus)
{
    AbsoluteLayout::OnWindowFocusChanged(hasWindowFocus);
    mAwViewMethods->OnWindowFocusChanged(hasWindowFocus);
    return NOERROR;
}

//@Override
void FullScreenView::OnFocusChanged(
    /* [in] */ Boolean focused,
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    AbsoluteLayout::OnFocusChanged(focused, direction, previouslyFocusedRect);
    mAwViewMethods->OnFocusChanged(
            focused, direction, previouslyFocusedRect);
}

//@Override
void FullScreenView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 ow,
    /* [in] */ Int32 oh)
{
    AbsoluteLayout::OnSizeChanged(w, h, ow, oh);
    mAwViewMethods->OnSizeChanged(w, h, ow, oh);
}

//@Override
ECode FullScreenView::OnVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 visibility)
{
    AbsoluteLayout::OnVisibilityChanged(changedView, visibility);
    mAwViewMethods->OnVisibilityChanged(changedView, visibility);
    return NOERROR;
}

//@Override
void FullScreenView::OnWindowVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    AbsoluteLayout::OnWindowVisibilityChanged(visibility);
    mAwViewMethods->OnWindowVisibilityChanged(visibility);
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
