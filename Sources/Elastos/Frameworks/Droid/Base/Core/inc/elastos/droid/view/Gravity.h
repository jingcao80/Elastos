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

#ifndef __ELASTOS_DROID_VIEW_GRAVITY_H__
#define __ELASTOS_DROID_VIEW_GRAVITY_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Graphics::IRect;

namespace Elastos {
namespace Droid {
namespace View {

class Gravity
{
public:
    static CARAPI Apply(
        /* [in] */ Int32 gravity,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ IRect* container,
        /* [in] */ IRect* outRect);

    static CARAPI Apply(
        /* [in] */ Int32 gravity,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ IRect* container,
        /* [in] */ IRect* outRect,
        /* [in] */ Int32 layoutDirection);

    static CARAPI Apply(
        /* [in] */ Int32 gravity,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ IRect* container,
        /* [in] */ Int32 xAdj,
        /* [in] */ Int32 yAdj,
        /* [in] */ IRect* outRect);

    static CARAPI Apply(
        /* [in] */ Int32 gravity,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ IRect* container,
        /* [in] */ Int32 xAdj,
        /* [in] */ Int32 yAdj,
        /* [in] */ IRect* outRect,
        /* [in] */ Int32 layoutDirection);

    static CARAPI ApplyDisplay(
        /* [in] */ Int32 gravity,
        /* [in] */ IRect* display,
        /* [in] */ IRect* inoutObj);

    static CARAPI ApplyDisplay(
        /* [in] */ Int32 gravity,
        /* [in] */ IRect* display,
        /* [in] */ IRect* inoutObj,
        /* [in] */ Int32 layoutDirection);

    static CARAPI IsVertical(
        /* [in] */ Int32 gravity,
        /* [out] */ Boolean* rst);

    static CARAPI IsHorizontal(
        /* [in] */ Int32 gravity,
        /* [out] */ Boolean* rst);

    static CARAPI GetAbsoluteGravity(
        /* [in] */ Int32 gravity,
        /* [in] */ Int32 layoutDirection,
        /* [out] */ Int32* rst);

    static CARAPI_(Boolean) IsVertical(
        /* [in] */ Int32 gravity);

    static CARAPI_(Boolean) IsHorizontal(
        /* [in] */ Int32 gravity);

    static CARAPI_(Int32) GetAbsoluteGravity(
        /* [in] */ Int32 gravity,
        /* [in] */ Int32 layoutDirection);
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_GRAVITY_H__
