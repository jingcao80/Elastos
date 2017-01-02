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

#include "elastos/droid/text/method/CTouch.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_SINGLETON_IMPL(CTouch)

CAR_INTERFACE_IMPL(CTouch, Singleton, ITouch)

ECode CTouch::ScrollTo(
    /* [in] */ ITextView* widget,
    /* [in] */ ILayout* layout,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    return Touch::ScrollTo(widget, layout, x, y);
}

ECode CTouch::OnTouchEvent(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* ret)
{
    return Touch::OnTouchEvent(widget, buffer, event, ret);
}

ECode CTouch::GetInitialScrollX(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer,
    /* [out] */ Int32* ret)
{
    return Touch::GetInitialScrollX(widget, buffer, ret);
}

ECode CTouch::GetInitialScrollY(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* buffer,
    /* [out] */ Int32* ret)
{
    return Touch::GetInitialScrollY(widget, buffer, ret);
}

ECode CTouch::IsActivelySelecting(
    /* [in] */ ISpannable* buffer,
    /* [out] */ Boolean* ret)
{
    return Touch::IsActivelySelecting(buffer, ret);
}

ECode CTouch::IsSelectionStarted(
    /* [in] */ ISpannable* buffer,
    /* [out] */ Boolean* ret)
{
    return Touch::IsSelectionStarted(buffer, ret);
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos