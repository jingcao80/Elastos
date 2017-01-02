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

#include "elastos/droid/systemui/statusbar/phone/KeyguardPreviewContainer.h"
#include "elastos/droid/systemui/statusbar/phone/CBlackBarDrawable.h"

using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IPixelFormat;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CAR_INTERFACE_IMPL(KeyguardPreviewContainer, FrameLayout, IKeyguardPreviewContainer)

KeyguardPreviewContainer::KeyguardPreviewContainer()
{
    CBlackBarDrawable::New(this, (IDrawable**)&mBlackBarDrawable);
}

ECode KeyguardPreviewContainer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(FrameLayout::constructor(context, attrs));
    SetBackground(mBlackBarDrawable);
    return NOERROR;
}

ECode KeyguardPreviewContainer::OnApplyWindowInsets(
    /* [in] */ IWindowInsets* insets,
    /* [out] */ IWindowInsets** sets)
{
    VALIDATE_NOT_NULL(sets);
    Int32 value = 0;
    insets->GetStableInsetBottom(&value);
    SetPadding(0, 0, 0, value);
    return FrameLayout::OnApplyWindowInsets(insets, sets);
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
