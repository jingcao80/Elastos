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

#include "elastos/droid/systemui/statusbar/CBackDropView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CAR_OBJECT_IMPL(CBackDropView)
CAR_INTERFACE_IMPL(CBackDropView, FrameLayout, IBackDropView)
ECode CBackDropView::constructor(
    /* [in] */ IContext* context)
{
    return FrameLayout::constructor(context);
}

ECode CBackDropView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return FrameLayout::constructor(context, attrs);
}

ECode CBackDropView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return FrameLayout::constructor(context, attrs, defStyleAttr);
}

ECode CBackDropView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return FrameLayout::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode CBackDropView::HasOverlappingRendering(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode CBackDropView::OnVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 visibility)
{
    FrameLayout::OnVisibilityChanged(changedView, visibility);
    if (changedView == this && mOnVisibilityChangedRunnable != NULL) {
        mOnVisibilityChangedRunnable->Run();
    }
    return NOERROR;
}

ECode CBackDropView::SetOnVisibilityChangedRunnable(
    /* [in] */ IRunnable* runnable)
{
    mOnVisibilityChangedRunnable = runnable;
    return NOERROR;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
