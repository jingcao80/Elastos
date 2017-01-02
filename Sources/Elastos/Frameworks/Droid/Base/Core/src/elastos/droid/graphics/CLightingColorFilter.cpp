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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CLightingColorFilter.h"
#include <skia/core/SkColorFilter.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CLightingColorFilter);
CAR_INTERFACE_IMPL(CLightingColorFilter, ColorFilter, ILightingColorFilter);
CLightingColorFilter::CLightingColorFilter()
    : mMul(0)
    , mAdd(0)
{}

ECode CLightingColorFilter::constructor(
    /* [in] */ Int32 mul,
    /* [in] */ Int32 add)
{
    mMul = mul;
    mAdd = add;
    Update();
    return NOERROR;
}

ECode CLightingColorFilter::GetColorMultiply(
    /* [out] */ Int32* colorMultiply)
{
    VALIDATE_NOT_NULL(colorMultiply);
    *colorMultiply = mMul;
    return NOERROR;
}

ECode CLightingColorFilter::SetColorMultiply(
    /* [in] */ Int32 mul)
{
    mMul = mul;
    Update();
    return NOERROR;
}

ECode CLightingColorFilter::GetColorAdd(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);
    *color = mAdd;
    return NOERROR;
}

ECode CLightingColorFilter::SetColorAdd(
    /* [in] */ Int32 add)
{
    mAdd = add;
    Update();
    return NOERROR;
}

void CLightingColorFilter::Update()
{
    DestroyFilter(mNativeInstance);
    mNativeInstance = NativeCreateLightingFilter(mMul, mAdd);
}

Int64 CLightingColorFilter::NativeCreateLightingFilter(
    /* [in] */ Int32 mul,
    /* [in] */ Int32 add)
{
    return reinterpret_cast<Int64>(SkColorFilter::CreateLightingFilter(mul, add));
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
