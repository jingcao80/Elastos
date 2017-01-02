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

#include "elastos/droid/inputmethodservice/CExtractButton.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace InputMethodService {

CAR_OBJECT_IMPL(CExtractButton);
ECode CExtractButton::constructor(
    /* [in] */ IContext* context)
{
    return Button::constructor(context, NULL);
}

ECode CExtractButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return Button::constructor(context, attrs, R::attr::buttonStyle);
}

ECode CExtractButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode CExtractButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return Button::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode CExtractButton::HasWindowFocus(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 v = 0;
    *res = (IsEnabled(res), *res) && (GetVisibility(&v), v) == IView::VISIBLE;
    return NOERROR;
}

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos
