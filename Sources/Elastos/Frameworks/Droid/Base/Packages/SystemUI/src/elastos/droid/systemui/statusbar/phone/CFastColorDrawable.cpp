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

#include "elastos/droid/systemui/statusbar/phone/CFastColorDrawable.h"

using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::PorterDuffMode_SRC;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CAR_OBJECT_IMPL(CFastColorDrawable)
CFastColorDrawable::CFastColorDrawable()
    : mColor(0)
{}

ECode CFastColorDrawable::constructor(
    /* [in] */ Int32 color)
{
    mColor = 0xff000000 | color;
    return Drawable::constructor();
}

ECode CFastColorDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    canvas->DrawColor(mColor, PorterDuffMode_SRC);
    return NOERROR;
}

ECode CFastColorDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    return NOERROR;
}

ECode CFastColorDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    return NOERROR;
}

ECode CFastColorDrawable::GetOpacity(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IPixelFormat::OPAQUE;
    return NOERROR;
}

ECode CFastColorDrawable::SetBounds(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    return NOERROR;
}

ECode CFastColorDrawable::SetBounds(
    /* [in] */ IRect* bounds)
{
    return NOERROR;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
