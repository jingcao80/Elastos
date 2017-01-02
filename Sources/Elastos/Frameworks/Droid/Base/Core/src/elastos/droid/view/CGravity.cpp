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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/CGravity.h"
#include "elastos/droid/view/Gravity.h"

namespace Elastos {
namespace Droid {
namespace View {
CAR_SINGLETON_IMPL(CGravity)

CAR_INTERFACE_IMPL(CGravity, Singleton, IGravity)

ECode CGravity::Apply(
    /* [in] */ Int32 gravity,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IRect* container,
    /* [in] */ IRect* outRect)
{
    return Gravity::Apply(gravity, w, h, container, outRect);
}

ECode CGravity::Apply(
    /* [in] */ Int32 gravity,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IRect* container,
    /* [in] */ IRect* outRect,
    /* [in] */ Int32 layoutDirection)
{
    return Gravity::Apply(gravity, w, h, container, outRect, layoutDirection);
}

ECode CGravity::Apply(
    /* [in] */ Int32 gravity,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IRect* container,
    /* [in] */ Int32 xAdj,
    /* [in] */ Int32 yAdj,
    /* [in] */ IRect* outRect)
{
    return Gravity::Apply(gravity, w, h, container, xAdj, yAdj, outRect);
}

ECode CGravity::Apply(
    /* [in] */ Int32 gravity,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IRect* container,
    /* [in] */ Int32 xAdj,
    /* [in] */ Int32 yAdj,
    /* [in] */ IRect* outRect,
    /* [in] */ Int32 layoutDirection)
{
    return Gravity::Apply(gravity, w, h, container, xAdj, yAdj, outRect, layoutDirection);
}

ECode CGravity::ApplyDisplay(
    /* [in] */ Int32 gravity,
    /* [in] */ IRect* display,
    /* [in] */ IRect* inoutObj)
{
    return Gravity::ApplyDisplay(gravity, display, inoutObj);
}

ECode CGravity::ApplyDisplay(
    /* [in] */ Int32 gravity,
    /* [in] */ IRect* display,
    /* [in] */ IRect* inoutObj,
    /* [in] */ Int32 layoutDirection)
{
    return Gravity::ApplyDisplay(gravity, display, inoutObj, layoutDirection);
}

ECode CGravity::IsVertical(
    /* [in] */ Int32 gravity,
    /* [out] */ Boolean* isVertical)
{
    VALIDATE_NOT_NULL(isVertical);
    *isVertical = Gravity::IsVertical(gravity);
    return NOERROR;
}

ECode CGravity::IsHorizontal(
    /* [in] */ Int32 gravity,
    /* [out] */ Boolean* isHorizontal)
{
    VALIDATE_NOT_NULL(isHorizontal);
    *isHorizontal = Gravity::IsHorizontal(gravity);
    return NOERROR;
}

ECode CGravity::GetAbsoluteGravity(
    /* [in] */ Int32 gravity,
    /* [in] */ Int32 layoutDirection,
    /* [out] */ Int32* absoluteGravity)
{
    VALIDATE_NOT_NULL(absoluteGravity);
    *absoluteGravity = Gravity::GetAbsoluteGravity(gravity, layoutDirection);
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
