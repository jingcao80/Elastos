
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/webkit/native/android_webview/NullAwViewMethods.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

NullAwViewMethods::NullAwViewMethods(
    /* [in] */ AwContents* awContents,
    /* [in] */ AwContents::InternalAccessDelegate* internalAccessAdapter,
    /* [in] */ IView* containerView)
    : mAwContents(awContents)
    , mInternalAccessAdapter(internalAccessAdapter)
    , mContainerView(containerView)
{
}

//@Override
void NullAwViewMethods::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    canvas->DrawColor(mAwContents->GetEffectiveBackgroundColor());
}

//@Override
void NullAwViewMethods::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    // When the containerView is using the NullNullAwViewMethods then it is not
    // attached to the AwContents. As such, we don't have any contents to measure
    // and using the last measured dimension is the best we can do.
    Int32 measuredWidth, measuredHeight;
    mContainerView->GetMeasuredWidth(&measuredWidth);
    mContainerView->GetMeasuredHeight(&measuredHeight);
    mInternalAccessAdapter->SetMeasuredDimension(measuredWidth, measuredHeight);
}

//@Override
void NullAwViewMethods::RequestFocus()
{
    // Intentional no-op.
}

//@Override
void NullAwViewMethods::SetLayerType(
    /* [in] */ Int32 layerType,
    /* [in] */ IPaint* paint)
{
    // Intentional no-op.
}

//@Override
AutoPtr<IInputConnection> NullAwViewMethods::OnCreateInputConnection(
    /* [in] */ IEditorInfo* outAttrs)
{
    return NULL; // Intentional no-op.
}

//@Override
Boolean NullAwViewMethods::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
{
    return FALSE; // Intentional no-op.
}

//@Override
Boolean NullAwViewMethods::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    return FALSE; // Intentional no-op.
}

//@Override
Boolean NullAwViewMethods::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    return FALSE; // Intentional no-op.
}

//@Override
Boolean NullAwViewMethods::OnHoverEvent(
    /* [in] */ IMotionEvent* event)
{
    return FALSE; // Intentional no-op.
}

//@Override
Boolean NullAwViewMethods::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event)
{
    return FALSE; // Intentional no-op.
}

//@Override
void NullAwViewMethods::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    // Intentional no-op.
}

//@Override
void NullAwViewMethods::OnAttachedToWindow()
{
    // Intentional no-op.
}

//@Override
void NullAwViewMethods::OnDetachedFromWindow()
{
    // Intentional no-op.
}

//@Override
void NullAwViewMethods::OnWindowFocusChanged(
    /* [in] */ Boolean hasWindowFocus)
{
    // Intentional no-op.
}

//@Override
void NullAwViewMethods::OnFocusChanged(
    /* [in] */ Boolean focused,
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    // Intentional no-op.
}

//@Override
void NullAwViewMethods::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 ow,
    /* [in] */ Int32 oh)
{
    // Intentional no-op.
}

//@Override
void NullAwViewMethods::OnVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 visibility)
{
    // Intentional no-op.
}

//@Override
void NullAwViewMethods::OnWindowVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    // Intentional no-op.
}

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
