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

#include "elastos/droid/systemui/statusbar/CAlphaImageView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CAR_OBJECT_IMPL(CAlphaImageView)
CAR_INTERFACE_IMPL(CAlphaImageView, ImageView, IAlphaImageView)
ECode CAlphaImageView::constructor(
    /* [in] */ IContext* context)
{
    return ImageView::constructor(context);
}

ECode CAlphaImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ImageView::constructor(context, attrs);
}

ECode CAlphaImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return ImageView::constructor(context, attrs, defStyleAttr);
}

ECode CAlphaImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return ImageView::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode CAlphaImageView::HasOverlappingRendering(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
