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

#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/internal/widget/LinearLayoutWithDefaultTouchRecepient.h"

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Internal::Widget::EIID_ILinearLayoutWithDefaultTouchRecepient;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CAR_INTERFACE_IMPL(LinearLayoutWithDefaultTouchRecepient, LinearLayout, ILinearLayoutWithDefaultTouchRecepient  )

LinearLayoutWithDefaultTouchRecepient::LinearLayoutWithDefaultTouchRecepient()
{
    CRect::New((IRect**)&mTempRect);
}

ECode LinearLayoutWithDefaultTouchRecepient::constructor(
    /* [in] */ IContext* context)
{
    return LinearLayout::constructor(context);
}

ECode LinearLayoutWithDefaultTouchRecepient::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return LinearLayout::constructor(context, attrs);
}

ECode LinearLayoutWithDefaultTouchRecepient::SetDefaultTouchRecepient(
    /* [in] */ IView* defaultTouchRecepient)
{
    mDefaultTouchRecepient = defaultTouchRecepient;
    return NOERROR;
}

ECode LinearLayoutWithDefaultTouchRecepient::DispatchTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(ev);
    VALIDATE_NOT_NULL(result);

    if (mDefaultTouchRecepient == NULL) {
        return LinearLayout::DispatchTouchEvent(ev, result);
    }

    if (LinearLayout::DispatchTouchEvent(ev, result), *result) {
        *result = TRUE;
        return NOERROR;
    }
    mTempRect->Set(0, 0, 0, 0);
    OffsetRectIntoDescendantCoords(mDefaultTouchRecepient, mTempRect);

    Float x, y;
    ev->GetX(&x);
    ev->GetY(&y);

    Int32 left, top;
    mTempRect->GetLeft(&left);
    mTempRect->GetTop(&top);
    ev->SetLocation(x + left, y + top);
    mDefaultTouchRecepient->DispatchTouchEvent(ev, result);
    return NOERROR;
}

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

