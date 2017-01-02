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
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CPaintFontMetricsInt.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CPaintFontMetricsInt);
CAR_INTERFACE_IMPL(CPaintFontMetricsInt, Object, IPaintFontMetricsInt);
CPaintFontMetricsInt::CPaintFontMetricsInt()
    : mTop(0)
    , mAscent(0)
    , mDescent(0)
    , mBottom(0)
    , mLeading(0)
{}

ECode CPaintFontMetricsInt::GetTop(
    /* [out] */ Int32* top)
{
    VALIDATE_NOT_NULL(top);
    *top = mTop;
    return NOERROR;
}

ECode CPaintFontMetricsInt::SetTop(
    /* [in] */ Int32 top)
{
    mTop = top;
    return NOERROR;
}

ECode CPaintFontMetricsInt::GetAscent(
    /* [out] */ Int32* ascent)
{
    VALIDATE_NOT_NULL(ascent);
    *ascent = mAscent;
    return NOERROR;
}

ECode CPaintFontMetricsInt::SetAscent(
    /* [in] */ Int32 ascent)
{
    mAscent = ascent;
    return NOERROR;
}

ECode CPaintFontMetricsInt::GetDescent(
    /* [out] */ Int32* descent)
{
    VALIDATE_NOT_NULL(descent);
    *descent = mDescent;
    return NOERROR;
}

ECode CPaintFontMetricsInt::SetDescent(
    /* [in] */ Int32 descent)
{
    mDescent = descent;
    return NOERROR;
}

ECode CPaintFontMetricsInt::GetBottom(
    /* [out] */ Int32* bottom)
{
    VALIDATE_NOT_NULL(bottom);
    *bottom = mBottom;
    return NOERROR;
}

ECode CPaintFontMetricsInt::SetBottom(
    /* [in] */ Int32 bottom)
{
    mBottom = bottom;
    return NOERROR;
}

ECode CPaintFontMetricsInt::GetLeading(
    /* [out] */ Int32* leading)
{
    VALIDATE_NOT_NULL(leading);
    *leading = mLeading;
    return NOERROR;
}

ECode CPaintFontMetricsInt::SetLeading(
    /* [in] */ Int32 leading)
{
    mLeading = leading;
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
