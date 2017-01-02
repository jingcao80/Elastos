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

#ifndef __ELASTOS_DROID_GRAPHICS_CPAINTFONTMETRICSINT_H__
#define __ELASTOS_DROID_GRAPHICS_CPAINTFONTMETRICSINT_H__

#include "_Elastos_Droid_Graphics_CPaintFontMetricsInt.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CPaintFontMetricsInt)
    , public Object
    , public IPaintFontMetricsInt
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPaintFontMetricsInt();

    CARAPI GetTop(
        /* [out] */ Int32* top);

    CARAPI SetTop(
        /* [in] */ Int32 top);

    CARAPI GetAscent(
        /* [out] */ Int32* ascent);

    CARAPI SetAscent(
        /* [in] */ Int32 ascent);

    CARAPI GetDescent(
        /* [out] */ Int32* descent);

    CARAPI SetDescent(
        /* [in] */ Int32 descent);

    CARAPI GetBottom(
        /* [out] */ Int32* bottom);

    CARAPI SetBottom(
        /* [in] */ Int32 bottom);

    CARAPI GetLeading(
        /* [out] */ Int32* leading);

    CARAPI SetLeading(
        /* [in] */ Int32 leading);

private:
    Int32 mTop;
    Int32 mAscent;
    Int32 mDescent;
    Int32 mBottom;
    Int32 mLeading;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CPAINTFONTMETRICSINT_H__
