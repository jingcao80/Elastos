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

#include "elastos/droid/systemui/statusbar/phone/CBlackBarDrawable.h"
#include "elastos/droid/systemui/statusbar/phone/KeyguardPreviewContainer.h"

using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IPixelFormat;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CAR_OBJECT_IMPL(CBlackBarDrawable)
ECode CBlackBarDrawable::constructor(
    /* [in] */ IKeyguardPreviewContainer* host)
{
    mHost = (KeyguardPreviewContainer*)host;
    return Drawable::constructor();
}

ECode CBlackBarDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    Int32 s = 0;
    canvas->Save(&s);
    Int32 h = 0, b = 0, w = 0;
    mHost->GetHeight(&h);
    mHost->GetWidth(&w);
    mHost->GetPaddingBottom(&b);
    Boolean tmp = FALSE;
    canvas->ClipRect(0, h - b, w, h, &tmp);
    canvas->DrawColor(IColor::BLACK);
    canvas->Restore();
    return NOERROR;
}

ECode CBlackBarDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    // noop
    return NOERROR;
}

ECode CBlackBarDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    // noop
    return NOERROR;
}

ECode CBlackBarDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    *opacity = IPixelFormat::OPAQUE;
    return NOERROR;
}


} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
