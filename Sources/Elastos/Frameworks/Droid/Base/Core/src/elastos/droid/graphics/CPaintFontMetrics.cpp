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
#include "elastos/droid/graphics/CPaintFontMetrics.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CPaintFontMetrics);
CAR_INTERFACE_IMPL(CPaintFontMetrics, Object, IPaintFontMetrics);
ECode CPaintFontMetrics::GetTop(
    /* [out] */ Float* top)
{
    VALIDATE_NOT_NULL(top);
    *top = mTop;
    return NOERROR;
}

ECode CPaintFontMetrics::SetTop(
    /* [in] */ Float top)
{
    mTop = top;
    return NOERROR;
}

ECode CPaintFontMetrics::GetAscent(
    /* [out] */ Float* ascent)
{
    VALIDATE_NOT_NULL(ascent);
    *ascent = mAscent;
    return NOERROR;
}

ECode CPaintFontMetrics::SetAscent(
    /* [in] */ Float ascent)
{
    mAscent = ascent;
    return NOERROR;
}

ECode CPaintFontMetrics::GetDescent(
    /* [out] */ Float* descent)
{
    VALIDATE_NOT_NULL(descent);
    *descent = mDescent;
    return NOERROR;
}

ECode CPaintFontMetrics::SetDescent(
    /* [in] */ Float descent)
{
    mDescent = descent;
    return NOERROR;
}

ECode CPaintFontMetrics::GetBottom(
    /* [out] */ Float* bottom)
{
    VALIDATE_NOT_NULL(bottom);
    *bottom = mBottom;
    return NOERROR;
}

ECode CPaintFontMetrics::SetBottom(
    /* [in] */ Float bottom)
{
    mBottom = bottom;
    return NOERROR;
}

ECode CPaintFontMetrics::GetLeading(
    /* [out] */ Float* leading)
{
    VALIDATE_NOT_NULL(leading);
    *leading = mLeading;
    return NOERROR;
}

ECode CPaintFontMetrics::SetLeading(
    /* [in] */ Float leading)
{
    mLeading = leading;
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
