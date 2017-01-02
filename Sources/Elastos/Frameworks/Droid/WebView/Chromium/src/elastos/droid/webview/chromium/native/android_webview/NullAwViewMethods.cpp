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

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/NullAwViewMethods.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
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
ECode NullAwViewMethods::OnMeasure(
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
    return NOERROR;
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
ECode NullAwViewMethods::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    // Intentional no-op.
    return NOERROR;
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
ECode NullAwViewMethods::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 ow,
    /* [in] */ Int32 oh)
{
    // Intentional no-op.
    return NOERROR;
}

//@Override
ECode NullAwViewMethods::OnVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 visibility)
{
    // Intentional no-op.
    return NOERROR;
}

//@Override
ECode NullAwViewMethods::OnWindowVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    // Intentional no-op.
    return NOERROR;
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
