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
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/style/RasterizerSpan.h"

using Elastos::Droid::Graphics::IPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_2(RasterizerSpan, CharacterStyle, IRasterizerSpan, IUpdateAppearance)

RasterizerSpan::RasterizerSpan()
{}

RasterizerSpan::~RasterizerSpan()
{}

ECode RasterizerSpan::constructor(
    /* [in] */ IRasterizer* r)
{
    mRasterizer = r;
    return NOERROR;
}

ECode RasterizerSpan::GetRasterizer(
    /* [out] */ IRasterizer** rastersizer)
{
    VALIDATE_NOT_NULL(rastersizer)
    *rastersizer = mRasterizer;
    REFCOUNT_ADD(*rastersizer)
    return NOERROR;
}

ECode RasterizerSpan::UpdateDrawState(
    /* [in] */ ITextPaint* ds)
{
    VALIDATE_NOT_NULL(ds);
    IPaint::Probe(ds)->SetRasterizer(mRasterizer);
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos
